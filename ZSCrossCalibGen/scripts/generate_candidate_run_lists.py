#!/usr/bin/env python3
"""
generate_candidate_run_lists.py

Selects candidate runs (1st most valid segments) and test runs (2nd most valid segments)
for each run group (epoch), and generates 19-line segment list files.

For each run group defined in epoch.csv:
1. Selects the candidate run (rank 1) from ZSCrossCalib_runs.list that has the maximum number
   of valid segments containing all 19 DST_TRIGGERED_EVENT files (seb00 through seb18).
   All 19 files in a segment come from a single production tag: --tag if given, otherwise
   (per run) the tag with the newest file timestamps that has complete segments.
   Candidate and test runs must also have the per-run MBD_QFIT payload in the CDB global tag
   given by --cdb-tag (default: newcdbtag); higher-ranked runs without it are skipped. Without
   MBD_QFIT the MBD t0 is NaN and CaloFittingQA leaves all t0-window histograms empty.
   Generates 19-line segment list files in --output-dir (default: ZSCrossCalibGen/files/lists).
2. Selects the test run (rank 2) from ZSCrossCalib_runs.list that has the 2nd most valid
   segments containing all 19 DST_TRIGGERED_EVENT files (same tag and CDB requirements).
   Generates 19-line segment list files in --test-output-dir (default: ZSCrossCalibGen/files/test_lists).
3. If an epoch group has no candidate run, writes a list of all runs in that epoch
   from ZSCrossCalib_runs.list to --runs-output-dir (no_candidate_runs_epoch<E>_<start>_<end>.list).
4. If any runs in ZSCrossCalib_runs.list fall outside all epochs, writes a separate list
   file for those unassigned runs (unassigned_runs.list).
5. Prints comprehensive statistics and reports any run group that has no candidate run.
"""

import argparse
import csv
import os
import re
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Set, Tuple

import psycopg2

TARGET_SEBS = tuple(f"seb{i:02d}" for i in range(19))  # seb00 to seb18 (19 SEBs)

# Per-run CDB payloads a candidate/test run must have. Without MBD_QFIT (MBD charge gains),
# MbdReco yields no hits -> MBD t0 is NaN -> CaloFittingQA leaves every t0-window profile empty.
REQUIRED_CDB_PAYLOADS = ("MBD_QFIT",)


class Tee:
    """Duplicate stream output to both console and a log file."""

    def __init__(self, *files):
        self.files = files

    def write(self, obj):
        for f in self.files:
            f.write(obj)
            f.flush()

    def flush(self):
        for f in self.files:
            f.flush()


def parse_arguments() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    repo_files_dir = script_dir.parent / "files"

    parser = argparse.ArgumentParser(
        description="Select candidate & test runs per epoch and generate 19-SEB DST_TRIGGERED_EVENT segment list files."
    )
    parser.add_argument(
        "--epoch-file",
        type=Path,
        default=repo_files_dir / "epoch.csv",
        help="Path to epoch CSV file containing run_start,run_end (default: ZSCrossCalibGen/files/epoch.csv)",
    )
    parser.add_argument(
        "--runs-file",
        type=Path,
        default=repo_files_dir / "ZSCrossCalib_runs.list",
        help="Path to ZSCrossCalib runs list file (default: ZSCrossCalibGen/files/ZSCrossCalib_runs.list)",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=repo_files_dir / "lists",
        help="Directory where candidate segment list files will be saved (default: ZSCrossCalibGen/files/lists)",
    )
    parser.add_argument(
        "--test-output-dir",
        type=Path,
        default=repo_files_dir / "test_lists",
        help="Directory where test segment list files (2nd most valid segments) will be saved (default: ZSCrossCalibGen/files/test_lists)",
    )
    parser.add_argument(
        "--runs-output-dir",
        type=Path,
        default=repo_files_dir,
        help="Directory to save run lists for epochs without candidates and unassigned runs (default: ZSCrossCalibGen/files)",
    )
    parser.add_argument(
        "--log-file",
        type=Path,
        default=None,
        help="Path to save execution logs and the summary report to a file",
    )
    parser.add_argument(
        "--dbname",
        type=str,
        default="FileCatalog",
        help="PostgreSQL database name (default: FileCatalog)",
    )
    parser.add_argument(
        "--dbhost",
        type=str,
        default=None,
        help="PostgreSQL host (optional, defaults to local socket / PGHOST)",
    )
    parser.add_argument(
        "--dbport",
        type=int,
        default=None,
        help="PostgreSQL port (optional, defaults to 5432 / PGPORT)",
    )
    parser.add_argument(
        "--tag",
        type=str,
        default=None,
        help="Use only this production tag (e.g. ana581_nocdbtag_v001). "
             "Default: per run, the tag with the newest file timestamps that has all 19 SEBs",
    )
    parser.add_argument(
        "--cdb-tag",
        type=str,
        default="newcdbtag",
        help=f"CDB global tag used to require per-run payloads ({', '.join(REQUIRED_CDB_PAYLOADS)}) "
             "for candidate and test runs (default: newcdbtag)",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Query candidate runs and print statistics without writing list files to disk",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Print detailed progress for each query and run",
    )
    return parser.parse_args()


def load_epochs(epoch_path: Path) -> List[Tuple[int, int]]:
    """Load epoch run ranges from CSV file."""
    if not epoch_path.is_file():
        sys.exit(f"Error: Epoch file not found: {epoch_path}")

    epochs = []
    with open(epoch_path, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for row_num, row in enumerate(reader, start=2):
            try:
                start = int(row["run_start"].strip())
                end = int(row["run_end"].strip())
                epochs.append((start, end))
            except (KeyError, ValueError) as err:
                sys.exit(f"Error parsing {epoch_path} at line {row_num}: {err}")
    return epochs


def load_runs(runs_path: Path) -> Set[int]:
    """Load allowed run numbers from text list file."""
    if not runs_path.is_file():
        sys.exit(f"Error: Runs list file not found: {runs_path}")

    runs = set()
    with open(runs_path, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, start=1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            try:
                runs.add(int(line))
            except ValueError:
                sys.exit(f"Error parsing {runs_path} at line {line_num}: invalid run '{line}'")
    return runs


def get_db_connection(dbname: str, host: Optional[str] = None, port: Optional[int] = None):
    """Establish connection to PostgreSQL FileCatalog."""
    conn_params = {"dbname": dbname}
    if host:
        conn_params["host"] = host
    if port:
        conn_params["port"] = port
    try:
        return psycopg2.connect(**conn_params)
    except psycopg2.Error as err:
        sys.exit(f"Database connection error to '{dbname}': {err}")


class CDBChecker:
    """
    Checks that required per-run CDB payloads (e.g. MBD_QFIT) resolve under a global tag,
    using sPHENIX CDBUtils through PyROOT. Results are cached per run.
    """

    def __init__(self, global_tag: str, payload_types: Tuple[str, ...] = REQUIRED_CDB_PAYLOADS):
        import ROOT

        ROOT.gSystem.Load("libsphenixnpc")
        ROOT.gInterpreter.Declare("""
        #ifndef ZSCC_CANDIDATE_CDB_CHECK
        #define ZSCC_CANDIDATE_CDB_CHECK
        #include <sphenixnpc/CDBUtils.h>
        #include <sphenixnpc/SphenixClient.h>
        std::string zscc_cdb_url(const std::string& gtag, const std::string& pl_type, unsigned long run) {
            static CDBUtils* utils = nullptr;
            static std::string current_tag;
            if (!utils) utils = new CDBUtils();
            if (current_tag != gtag) { utils->setGlobalTag(gtag); current_tag = gtag; }
            return utils->getUrl(pl_type, run);
        }
        #endif
        """)
        self._root = ROOT
        self.global_tag = global_tag
        self.payload_types = payload_types
        self._cache: Dict[int, List[str]] = {}

    def missing_payloads(self, run: int) -> List[str]:
        """Return the required payload types that do NOT resolve for this run (empty list = all present)."""
        if run not in self._cache:
            missing = []
            for pl_type in self.payload_types:
                url = str(self._root.zscc_cdb_url(self.global_tag, pl_type, run))
                if not url or url.startswith("DataBaseException"):
                    missing.append(pl_type)
            self._cache[run] = missing
        return self._cache[run]


def rank_runs(
    cursor,
    run_start: int,
    run_end: int,
    eligible_runs: List[int],
    tag: Optional[str] = None,
    verbose: bool = False,
) -> List[Tuple[int, str, int]]:
    """
    Rank runs in eligible_runs by number of valid segments (most first).
    A valid segment has all 19 DST_TRIGGERED_EVENT files for seb00-seb18, all
    from the SAME production tag (tags are never mixed within a segment).

    Tag choice per run:
      - If `tag` is given, only that tag is considered.
      - Otherwise, among the tags that have at least one valid segment for the
        run, the tag whose files are newest (max file timestamp in the `files`
        table) is chosen.

    Returns [(runnumber, tag, valid_segment_count), ...] sorted best first.
    """
    if not eligible_runs:
        return []

    tag_filter = "AND tag = %s" if tag else ""
    query = f"""
        WITH seb_rows AS (
            SELECT runnumber, tag, segment, filename,
                   substring(filename from 'seb[0-9]+') AS seb_id
            FROM datasets
            WHERE runnumber >= %s AND runnumber <= %s AND runnumber = ANY(%s)
              AND dsttype LIKE 'DST_TRIGGERED%%'
              AND substring(filename from 'seb[0-9]+') IN (
                'seb00','seb01','seb02','seb03','seb04','seb05','seb06','seb07','seb08','seb09',
                'seb10','seb11','seb12','seb13','seb14','seb15','seb16','seb17','seb18'
              )
              {tag_filter}
        ),
        seg_tag AS (
            SELECT runnumber, tag, segment
            FROM seb_rows
            GROUP BY runnumber, tag, segment
            HAVING count(DISTINCT seb_id) = 19
        ),
        run_tag AS (
            SELECT runnumber, tag, count(*) AS valid_segments
            FROM seg_tag
            GROUP BY runnumber, tag
        ),
        tag_time AS (
            SELECT r.runnumber, r.tag, max(f.time) AS newest
            FROM seb_rows r
            JOIN files f ON f.lfn = r.filename
            GROUP BY r.runnumber, r.tag
        ),
        best_tag AS (
            SELECT DISTINCT ON (rt.runnumber) rt.runnumber, rt.tag, rt.valid_segments
            FROM run_tag rt
            LEFT JOIN tag_time tt USING (runnumber, tag)
            ORDER BY rt.runnumber, tt.newest DESC NULLS LAST, rt.tag DESC
        )
        SELECT runnumber, tag, valid_segments
        FROM best_tag
        ORDER BY valid_segments DESC, runnumber DESC;
    """

    params = [run_start, run_end, eligible_runs]
    if tag:
        params.append(tag)
    cursor.execute(query, params)
    results = cursor.fetchall()

    if verbose:
        print(f"    Top runs (run, tag, valid_segments): {results[:5]}")

    return results


def select_runs_with_cdb(
    ranked: List[Tuple[int, str, int]], checker: CDBChecker, n_select: int = 2, verbose: bool = False
) -> Tuple[List[Tuple[int, str, int]], List[Tuple[int, List[str]]]]:
    """
    Walk the ranked runs best-first and keep the first n_select runs whose required CDB payloads
    all resolve. Returns (selected, skipped) where skipped = [(run, missing_payloads), ...] for runs
    ranked above the last selected run that were rejected.
    """
    selected: List[Tuple[int, str, int]] = []
    skipped: List[Tuple[int, List[str]]] = []
    for info in ranked:
        missing = checker.missing_payloads(info[0])
        if missing:
            skipped.append((info[0], missing))
            if verbose:
                print(f"    Skip run {info[0]}: missing {', '.join(missing)} in CDB tag {checker.global_tag}")
            continue
        selected.append(info)
        if len(selected) == n_select:
            break
    return selected, skipped


def fetch_segment_files(cursor, runnumber: int, tag: str) -> Dict[int, List[str]]:
    """
    Fetch all 19 seb00-seb18 files of the given tag for each valid segment of a run.
    Returns a dictionary mapping segment number -> sorted list of 19 filenames.
    """
    query = """
        SELECT segment, filename, substring(filename from 'seb[0-9]+') AS seb_id
        FROM datasets
        WHERE runnumber = %s
          AND tag = %s
          AND dsttype LIKE 'DST_TRIGGERED%%'
          AND substring(filename from 'seb[0-9]+') IN (
            'seb00','seb01','seb02','seb03','seb04','seb05','seb06','seb07','seb08','seb09',
            'seb10','seb11','seb12','seb13','seb14','seb15','seb16','seb17','seb18'
          )
        ORDER BY segment, seb_id;
    """
    cursor.execute(query, (runnumber, tag))
    rows = cursor.fetchall()

    # Group by segment; a single tag must yield exactly one file per SEB
    segments: Dict[int, Dict[str, str]] = {}
    for seg, filename, seb_id in rows:
        sebs = segments.setdefault(seg, {})
        if seb_id in sebs and sebs[seb_id] != filename:
            raise RuntimeError(
                f"Run {runnumber} seg {seg} {seb_id}: multiple files for tag {tag}: "
                f"{sebs[seb_id]} and {filename}"
            )
        sebs[seb_id] = filename

    valid_segment_files: Dict[int, List[str]] = {}
    for seg, sebs in segments.items():
        if len(sebs) == 19 and all(seb in sebs for seb in TARGET_SEBS):
            valid_segment_files[seg] = [sebs[seb] for seb in TARGET_SEBS]

    return valid_segment_files


def write_segment_lists(
    segment_files: Dict[int, List[str]],
    runnumber: int,
    target_dir: Path,
    dry_run: bool = False,
) -> int:
    """
    Write 19-line segment list files to target_dir.
    Returns the number of lists written (or found, if dry_run).
    """
    written = 0
    if not dry_run:
        for seg in sorted(segment_files.keys()):
            files = segment_files[seg]
            list_filename = f"dst_triggered_run{runnumber}_seg{seg:05d}.list"
            list_path = target_dir / list_filename
            with open(list_path, "w", encoding="utf-8") as out_f:
                for fn in files:
                    out_f.write(f"{fn}\n")
            written += 1
    else:
        written = len(segment_files)
    return written


def main():
    args = parse_arguments()

    original_stdout = sys.stdout
    log_file_handle = None
    if args.log_file:
        args.log_file.parent.mkdir(parents=True, exist_ok=True)
        log_file_handle = open(args.log_file, "w", encoding="utf-8")
        sys.stdout = Tee(sys.stdout, log_file_handle)

    runs_out_dir = args.runs_output_dir

    print("=" * 80)
    print(" sPHENIX ZSCrossCalib Candidate & Test Run List Generator")
    print("=" * 80)
    print(f"Epoch file:             {args.epoch_file}")
    print(f"Runs list file:         {args.runs_file}")
    print(f"Candidate output dir:   {args.output_dir}")
    print(f"Test output dir:        {args.test_output_dir}")
    print(f"Runs output dir:        {runs_out_dir}")
    if args.log_file:
        print(f"Log file:               {args.log_file}")
    print(f"Database:               {args.dbname}")
    print(f"Tag:                    {args.tag if args.tag else 'auto (newest by file timestamp, per run)'}")
    print(f"CDB global tag:         {args.cdb_tag} (requires {', '.join(REQUIRED_CDB_PAYLOADS)} per run)")
    print(f"Dry run mode:           {'Enabled (no files written)' if args.dry_run else 'Disabled'}")
    print("-" * 80)

    # 1. Load inputs
    epochs = load_epochs(args.epoch_file)
    zs_runs = load_runs(args.runs_file)
    print(f"Loaded {len(epochs)} run groups from {args.epoch_file.name}")
    print(f"Loaded {len(zs_runs)} allowed runs from {args.runs_file.name}\n")

    # 2. Connect to database
    conn = get_db_connection(args.dbname, args.dbhost, args.dbport)
    cursor = conn.cursor()

    if not args.dry_run:
        args.output_dir.mkdir(parents=True, exist_ok=True)
        args.test_output_dir.mkdir(parents=True, exist_ok=True)
        runs_out_dir.mkdir(parents=True, exist_ok=True)

    # CDB payload checker (only runs that reach the top of a ranking are queried)
    cdb_checker = CDBChecker(args.cdb_tag)
    all_skipped_cdb: List[Tuple[int, int, List[str]]] = []

    # 3. Process each epoch
    stats = []
    total_cand_lists_written = 0
    total_test_lists_written = 0
    t_start = time.time()

    for idx, (start, end) in enumerate(epochs, start=1):
        group_label = f"Group {idx} [{start}, {end}]"
        eligible_runs = sorted([r for r in zs_runs if start <= r <= end])

        if args.verbose:
            print(f"\nProcessing {group_label}: {len(eligible_runs)} runs in ZSCrossCalib_runs.list")

        ranked = rank_runs(cursor, start, end, eligible_runs, tag=args.tag, verbose=args.verbose)
        selected, skipped_cdb = select_runs_with_cdb(ranked, cdb_checker, n_select=2, verbose=args.verbose)
        cand_info = selected[0] if selected else None
        test_info = selected[1] if len(selected) > 1 else None
        if skipped_cdb:
            all_skipped_cdb.extend((idx, run, missing) for run, missing in skipped_cdb)
            print(
                f"[{group_label}] Skipped {len(skipped_cdb)} higher-ranked run(s) missing CDB payloads "
                f"in '{args.cdb_tag}': "
                + ", ".join(f"{run} ({'/'.join(missing)})" for run, missing in skipped_cdb)
            )

        if cand_info is None:
            no_cand_filename = f"no_candidate_runs_epoch{idx}_{start}_{end}.list"
            no_cand_path = runs_out_dir / no_cand_filename
            if not args.dry_run:
                with open(no_cand_path, "w", encoding="utf-8") as out_f:
                    for r in eligible_runs:
                        out_f.write(f"{r}\n")

            stats.append({
                "group_idx": idx,
                "range": (start, end),
                "eligible_count": len(eligible_runs),
                "candidate_run": None,
                "cand_tag": None,
                "cand_segments": 0,
                "cand_lists_written": 0,
                "test_run": None,
                "test_tag": None,
                "test_segments": 0,
                "test_lists_written": 0,
                "no_cand_file": no_cand_path,
                "no_cand_filename": no_cand_filename,
                "status": "NO CANDIDATE",
            })
            print(
                f"[{group_label}] -> NO candidate run found with valid segments and "
                f"{', '.join(REQUIRED_CDB_PAYLOADS)} in '{args.cdb_tag}'. "
                f"Saved {len(eligible_runs)} runs in this epoch to: {no_cand_filename}"
            )
            continue

        cand_run, cand_tag, cand_segs_count = cand_info
        cand_files = fetch_segment_files(cursor, cand_run, cand_tag)
        cand_written = write_segment_lists(cand_files, cand_run, args.output_dir, args.dry_run)
        total_cand_lists_written += cand_written

        test_run = None
        test_tag = None
        test_written = 0
        test_segs_count = 0
        if test_info is not None:
            test_run, test_tag, test_segs_count = test_info
            test_files = fetch_segment_files(cursor, test_run, test_tag)
            test_written = write_segment_lists(test_files, test_run, args.test_output_dir, args.dry_run)
            total_test_lists_written += test_written

        stats.append({
            "group_idx": idx,
            "range": (start, end),
            "eligible_count": len(eligible_runs),
            "candidate_run": cand_run,
            "cand_tag": cand_tag,
            "cand_segments": len(cand_files),
            "cand_lists_written": cand_written,
            "test_run": test_run,
            "test_tag": test_tag,
            "test_segments": test_segs_count,
            "test_lists_written": test_written,
            "status": "OK",
        })

        test_msg = (
            f", Test (2nd): Run {test_run} [{test_tag}] ({test_segs_count} segs)" if test_run else ""
        )
        print(
            f"[{group_label}] -> Candidate (1st): Run {cand_run} [{cand_tag}] ({len(cand_files)} segs)"
            f"{test_msg}"
        )

    cursor.close()
    conn.close()

    # Identify runs from runs_file that fall into none of the epochs
    unassigned_runs = []
    for r in sorted(zs_runs):
        if not any(start <= r <= end for start, end in epochs):
            unassigned_runs.append(r)

    unassigned_filename = "unassigned_runs.list"
    unassigned_path = runs_out_dir / unassigned_filename
    if unassigned_runs:
        if not args.dry_run:
            with open(unassigned_path, "w", encoding="utf-8") as out_f:
                for r in unassigned_runs:
                    out_f.write(f"{r}\n")
        print(
            f"\nFound {len(unassigned_runs)} runs in {args.runs_file.name} that fall outside all epochs. "
            f"Saved to: {unassigned_filename}"
        )

    elapsed = time.time() - t_start

    # 4. Print final summary and statistics
    print("\n" + "=" * 105)
    print(" CANDIDATE & TEST RUN SELECTION SUMMARY")
    print("=" * 105)
    print(
        f"{'Group':<10} {'Run Range':<17} {'Runs':<6} "
        f"{'Candidate (1st)':<18} {'Cand Segs':<11} "
        f"{'Test (2nd)':<14} {'Test Segs':<11} {'Status':<12}"
    )
    print("-" * 105)
    tag_lines = []

    no_candidate_groups = []
    for s in stats:
        start, end = s["range"]
        range_str = f"[{start}, {end}]"
        group_name = f"Group {s['group_idx']}"
        cand_str = str(s["candidate_run"]) if s["candidate_run"] is not None else "-"
        test_str = str(s["test_run"]) if s["test_run"] is not None else "-"
        print(
            f"{group_name:<10} {range_str:<17} {s['eligible_count']:<6} "
            f"{cand_str:<18} {s['cand_segments']:<11} "
            f"{test_str:<14} {s['test_segments']:<11} {s['status']:<12}"
        )
        if s["candidate_run"] is None:
            no_candidate_groups.append(s)
        else:
            tag_lines.append(
                f"  Group {s['group_idx']}: candidate {s['candidate_run']} -> {s['cand_tag']}"
                + (f", test {s['test_run']} -> {s['test_tag']}" if s["test_run"] is not None else "")
            )

    print("-" * 105)
    if tag_lines:
        print("Tags used:")
        for line in tag_lines:
            print(line)
        print("-" * 105)
    print(f"Total run groups processed:             {len(epochs)}")
    print(f"Run groups with candidates:             {len(epochs) - len(no_candidate_groups)}")
    print(f"Run groups WITHOUT candidates:          {len(no_candidate_groups)}")
    for s in no_candidate_groups:
        file_desc = f"{s['no_cand_filename']} ({s['eligible_count']} runs)" if not args.dry_run else f"{s['no_cand_filename']} (dry run, {s['eligible_count']} runs)"
        print(f"  * Group {s['group_idx']} run list:               {file_desc}")
    print(f"Runs outside all epochs:                {len(unassigned_runs)}")
    if unassigned_runs:
        unassigned_desc = f"{unassigned_filename} ({len(unassigned_runs)} runs)" if not args.dry_run else f"{unassigned_filename} (dry run, {len(unassigned_runs)} runs)"
        print(f"  * Unassigned run list:                {unassigned_desc}")
    print(f"Total Candidate segment list files:     {total_cand_lists_written}")
    print(f"Total Test segment list files:          {total_test_lists_written}")
    print(f"Candidate lists directory:              {args.output_dir}")
    print(f"Test lists directory:                   {args.test_output_dir}")
    if runs_out_dir != args.output_dir:
        print(f"Run lists directory:                    {runs_out_dir}")
    print(f"Execution time:                         {elapsed:.2f} seconds")
    print("=" * 105)

    if no_candidate_groups:
        print("\n [!] WARNING: The following run group(s) have NO candidate run:")
        for s in no_candidate_groups:
            start, end = s["range"]
            print(
                f"    - Group {s['group_idx']} (runs {start} to {end}): "
                f"{s['eligible_count']} runs in ZSCrossCalib_runs.list, "
                f"but none have valid segments with all 19 seb00-seb18 files "
                f"and {', '.join(REQUIRED_CDB_PAYLOADS)} in CDB tag '{args.cdb_tag}'."
            )
            print(f"      -> Wrote {s['eligible_count']} runs to: {s['no_cand_file']}")
        print("=" * 105)

    if all_skipped_cdb:
        print(
            f"\n [i] NOTE: {len(all_skipped_cdb)} higher-ranked run(s) were skipped as candidate/test because "
            f"required CDB payloads are missing in '{args.cdb_tag}':"
        )
        for g_idx, run, missing in all_skipped_cdb:
            print(f"    - Group {g_idx}: run {run} missing {', '.join(missing)}")
        print("=" * 105)

    if unassigned_runs:
        print(f"\n [i] NOTE: {len(unassigned_runs)} runs in ZSCrossCalib_runs.list do not fall into any epoch:")
        print(f"    -> Wrote {len(unassigned_runs)} unassigned runs to: {unassigned_path}")
        print("=" * 105)

    if log_file_handle:
        print(f"\nReport and logs saved to: {args.log_file}")
        sys.stdout = original_stdout
        log_file_handle.close()


if __name__ == "__main__":
    main()
