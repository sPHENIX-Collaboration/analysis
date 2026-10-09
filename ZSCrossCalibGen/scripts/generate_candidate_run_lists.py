#!/usr/bin/env python3
"""
generate_candidate_run_lists.py

Selects candidate runs for each run group (epoch) and generates 19-line segment list files.

For each run group defined in epoch.csv:
1. Selects one candidate run from ZSCrossCalib_runs.list that has the maximum number
   of valid segments containing all 19 DST_TRIGGERED_EVENT files (seb00 through seb18).
2. For each valid segment of the candidate run, writes a list file containing exactly
   19 lines (one per SEB, sorted from seb00 to seb18), matching the format of test.list.
3. Saves all generated list files neatly in a single subfolder in ZSCrossCalibGen/files/
   (without run subdirectories).
4. Prints comprehensive statistics and reports any run group that has no candidate run.
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
        description="Select candidate runs per epoch and generate 19-SEB DST_TRIGGERED_EVENT segment list files."
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
        help="Directory where segment list files will be saved (default: ZSCrossCalibGen/files/lists)",
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


def find_candidate_run(
    cursor,
    run_start: int,
    run_end: int,
    eligible_runs: List[int],
    verbose: bool = False,
) -> Optional[Tuple[int, int]]:
    """
    Find the run in eligible_runs with the maximum number of valid segments.
    A valid segment has all 19 DST_TRIGGERED_EVENT files for seb00-seb18.
    Returns (candidate_run, valid_segment_count) or None if no valid runs.
    """
    if not eligible_runs:
        return None

    query = """
        WITH seb_files AS (
            SELECT runnumber, segment, count(DISTINCT substring(filename from 'seb[0-9]+')) AS n_seb
            FROM datasets
            WHERE runnumber >= %s AND runnumber <= %s AND runnumber = ANY(%s)
              AND dsttype LIKE 'DST_TRIGGERED%%'
              AND substring(filename from 'seb[0-9]+') IN (
                'seb00','seb01','seb02','seb03','seb04','seb05','seb06','seb07','seb08','seb09',
                'seb10','seb11','seb12','seb13','seb14','seb15','seb16','seb17','seb18'
              )
            GROUP BY runnumber, segment
            HAVING count(DISTINCT substring(filename from 'seb[0-9]+')) = 19
        )
        SELECT runnumber, count(*) AS valid_segments
        FROM seb_files
        GROUP BY runnumber
        ORDER BY valid_segments DESC, runnumber DESC
        LIMIT 5;
    """

    cursor.execute(query, (run_start, run_end, eligible_runs))
    results = cursor.fetchall()

    if not results:
        return None

    best_run, max_segs = results[0]
    if verbose:
        print(f"    Top runs: {results}")

    return best_run, max_segs


def fetch_segment_files(cursor, runnumber: int) -> Dict[int, List[str]]:
    """
    Fetch all 19 seb00-seb18 files for each valid segment of a candidate run.
    Returns a dictionary mapping segment number -> sorted list of 19 filenames.
    """
    query = """
        SELECT segment, filename, substring(filename from 'seb[0-9]+') AS seb_id
        FROM datasets
        WHERE runnumber = %s
          AND dsttype LIKE 'DST_TRIGGERED%%'
          AND substring(filename from 'seb[0-9]+') IN (
            'seb00','seb01','seb02','seb03','seb04','seb05','seb06','seb07','seb08','seb09',
            'seb10','seb11','seb12','seb13','seb14','seb15','seb16','seb17','seb18'
          )
        ORDER BY segment, seb_id;
    """
    cursor.execute(query, (runnumber,))
    rows = cursor.fetchall()

    # Group by segment, deduplicate per SEB if needed, ensuring exactly 19 SEBs
    segments: Dict[int, Dict[str, str]] = {}
    for seg, filename, seb_id in rows:
        if seg not in segments:
            segments[seg] = {}
        segments[seg][seb_id] = filename

    valid_segment_files: Dict[int, List[str]] = {}
    for seg, sebs in segments.items():
        if len(sebs) == 19 and all(seb in sebs for seb in TARGET_SEBS):
            valid_segment_files[seg] = [sebs[seb] for seb in TARGET_SEBS]

    return valid_segment_files


def main():
    args = parse_arguments()

    original_stdout = sys.stdout
    log_file_handle = None
    if args.log_file:
        args.log_file.parent.mkdir(parents=True, exist_ok=True)
        log_file_handle = open(args.log_file, "w", encoding="utf-8")
        sys.stdout = Tee(sys.stdout, log_file_handle)

    print("=" * 80)
    print(" sPHENIX ZSCrossCalib Candidate Run & Segment List Generator")
    print("=" * 80)
    print(f"Epoch file:       {args.epoch_file}")
    print(f"Runs list file:   {args.runs_file}")
    print(f"Output directory: {args.output_dir}")
    if args.log_file:
        print(f"Log file:         {args.log_file}")
    print(f"Database:         {args.dbname}")
    print(f"Dry run mode:     {'Enabled (no files written)' if args.dry_run else 'Disabled'}")
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

    # 3. Process each epoch
    stats = []
    total_lists_written = 0
    t_start = time.time()

    for idx, (start, end) in enumerate(epochs, start=1):
        group_label = f"Group {idx} [{start}, {end}]"
        eligible_runs = sorted([r for r in zs_runs if start <= r <= end])

        if args.verbose:
            print(f"\nProcessing {group_label}: {len(eligible_runs)} runs in ZSCrossCalib_runs.list")

        candidate_info = find_candidate_run(
            cursor, start, end, eligible_runs, verbose=args.verbose
        )

        if candidate_info is None:
            stats.append({
                "group_idx": idx,
                "range": (start, end),
                "eligible_count": len(eligible_runs),
                "candidate_run": None,
                "valid_segments": 0,
                "lists_written": 0,
                "status": "NO CANDIDATE",
            })
            print(f"[{group_label}] -> NO candidate run found with valid segments.")
            continue

        candidate_run, valid_segs_count = candidate_info
        segment_files = fetch_segment_files(cursor, candidate_run)
        num_valid_segs = len(segment_files)

        written_for_run = 0
        if not args.dry_run:
            for seg in sorted(segment_files.keys()):
                files = segment_files[seg]
                list_filename = f"dst_triggered_run{candidate_run}_seg{seg:05d}.list"
                list_path = args.output_dir / list_filename
                with open(list_path, "w", encoding="utf-8") as out_f:
                    for fn in files:
                        out_f.write(f"{fn}\n")
                written_for_run += 1
        else:
            written_for_run = num_valid_segs

        total_lists_written += written_for_run
        stats.append({
            "group_idx": idx,
            "range": (start, end),
            "eligible_count": len(eligible_runs),
            "candidate_run": candidate_run,
            "valid_segments": num_valid_segs,
            "lists_written": written_for_run,
            "status": "OK",
        })

        print(
            f"[{group_label}] -> Candidate: Run {candidate_run} "
            f"({num_valid_segs} valid segments with 19 files, "
            f"{'generated' if not args.dry_run else 'found'} {written_for_run} lists)"
        )

    cursor.close()
    conn.close()

    elapsed = time.time() - t_start

    # 4. Print final summary and statistics
    print("\n" + "=" * 90)
    print(" CANDIDATE RUN SELECTION & SEGMENT LIST GENERATION SUMMARY")
    print("=" * 90)
    print(
        f"{'Group':<10} {'Run Range':<17} {'Runs in List':<14} {'Candidate Run':<15} "
        f"{'Valid Segs':<12} {'Lists Made':<12} {'Status':<12}"
    )
    print("-" * 90)

    no_candidate_groups = []
    for s in stats:
        start, end = s["range"]
        range_str = f"[{start}, {end}]"
        group_name = f"Group {s['group_idx']}"
        cand_str = str(s["candidate_run"]) if s["candidate_run"] is not None else "-"
        print(
            f"{group_name:<10} {range_str:<17} {s['eligible_count']:<14} "
            f"{cand_str:<15} {s['valid_segments']:<12} {s['lists_written']:<12} {s['status']:<12}"
        )
        if s["candidate_run"] is None:
            no_candidate_groups.append(s)

    print("-" * 90)
    print(f"Total run groups processed:             {len(epochs)}")
    print(f"Run groups with candidate runs:         {len(epochs) - len(no_candidate_groups)}")
    print(f"Run groups WITHOUT candidate runs:      {len(no_candidate_groups)}")
    print(f"Total segment list files generated:     {total_lists_written}")
    print(f"Output directory:                       {args.output_dir}")
    print(f"Execution time:                         {elapsed:.2f} seconds")
    print("=" * 90)

    if no_candidate_groups:
        print("\n [!] WARNING: The following run group(s) have NO candidate run:")
        for s in no_candidate_groups:
            start, end = s["range"]
            print(
                f"    - Group {s['group_idx']} (runs {start} to {end}): "
                f"{s['eligible_count']} runs in ZSCrossCalib_runs.list, "
                f"but none have valid segments with all 19 seb00-seb18 files."
            )
        print("=" * 90)

    if log_file_handle:
        print(f"\nReport and logs saved to: {args.log_file}")
        sys.stdout = original_stdout
        log_file_handle.close()


if __name__ == "__main__":
    main()
