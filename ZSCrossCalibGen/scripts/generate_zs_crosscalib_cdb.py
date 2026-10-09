#!/usr/bin/env python3
"""
generate_zs_crosscalib_cdb.py

Generates new ZSCrossCalib CDBTTrees for runs in ZSCrossCalib_runs.list:
1. Maps each run to its epoch run group defined in epoch.csv.
2. Identifies the reference/candidate run from hist-list (e.g. test.list) for each epoch.
3. Fetches the baseline ZSCrossCalib CDB payload for both target and reference runs.
4. Reads the TProfile2D QA histograms (main, minus5, plus5) for CEMC, HCALIN, HCALOUT.
5. Computes updated ratio fields:
   - ratio: original CDB value
   - ratio_main: (ZScrosscalib_main / ZSCrossCalib_ref) * original ZSCrossCalib
   - ratio_minus5: (ZScrosscalib_minus5 / ZSCrossCalib_ref) * original ZSCrossCalib
   - ratio_plus5: (ZScrosscalib_plus5 / ZSCrossCalib_ref) * original ZSCrossCalib
   If ZSCrossCalib_ref == 0: sets ratio_main, ratio_minus5, ratio_plus5 to 0.0.
6. Writes new CDBTTree files named:
   <DET>_ZSCrossCalibT0_pro001_pcdb001_v001_<run>.root
   to the output directory (default: scratch/calibs).
Supports parallel execution across runs using multiprocessing.
"""

import argparse
import concurrent.futures
import csv
import os
from pathlib import Path
import re
import sys
import time
from typing import Dict, List, Optional, Set, Tuple

import tqdm

# Supported detector configurations
DETECTOR_MAP = {
    "CEMC": {"cdb_name": "CEMC_ZSCrossCalib", "hist_prefix": "cemc", "nchannels": 24576},
    "HCALIN": {"cdb_name": "HCALIN_ZSCrossCalib", "hist_prefix": "ihcal", "nchannels": 1536},
    "HCALOUT": {"cdb_name": "HCALOUT_ZSCrossCalib", "hist_prefix": "ohcal", "nchannels": 1536},
}

DETECTOR_ALIASES = {
    "cemc": "CEMC",
    "emcal": "CEMC",
    "ihcal": "HCALIN",
    "hcalin": "HCALIN",
    "ohcal": "HCALOUT",
    "hcalout": "HCALOUT",
}

# Worker process state
_worker_initialized = False
_ROOT = None
_worker_cache: Dict[Tuple[int, str], any] = {}


class Tee:
    """Duplicate output stream to both console and a log file."""

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
    repo_root = script_dir.parent
    repo_files_dir = repo_root / "files"

    parser = argparse.ArgumentParser(
        description="Generate new ZSCrossCalib CDBTTrees with relative QA cross-calibrations."
    )
    parser.add_argument(
        "--run-list",
        type=Path,
        default=repo_files_dir / "ZSCrossCalib_runs.list",
        help="Path to runs list file (default: ZSCrossCalibGen/files/ZSCrossCalib_runs.list)",
    )
    parser.add_argument(
        "--hist-list",
        type=Path,
        required=True,
        help="Path to hist list file containing QA histogram files",
    )
    parser.add_argument(
        "--epoch-file",
        type=Path,
        default=repo_files_dir / "epoch.csv",
        help="Path to epoch CSV file (default: ZSCrossCalibGen/files/epoch.csv)",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=repo_root / "scratch" / "calibs",
        help="Directory to write output CDBTTree files (default: ZSCrossCalibGen/scratch/calibs)",
    )
    parser.add_argument(
        "--dbtag",
        type=str,
        default="newcdbtag",
        help="CDB global tag (default: newcdbtag)",
    )
    parser.add_argument(
        "--detectors",
        type=str,
        default="CEMC,HCALIN,HCALOUT",
        help="Comma-separated list of detectors to process (default: CEMC,HCALIN,HCALOUT)",
    )
    parser.add_argument(
        "--runs",
        type=str,
        default=None,
        help="Optional comma-separated list or range of runs to filter (e.g. 67597,67633 or 67597-67610)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=min(os.cpu_count() or 4, 16),
        help="Number of parallel worker processes (default: auto, up to 16)",
    )
    parser.add_argument(
        "--log-file",
        type=Path,
        default=None,
        help="Optional path to write log output",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Check inputs and candidate mappings without creating CDBTree files",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Print verbose execution details",
    )
    return parser.parse_args()


def normalize_detector_list(det_str: str) -> List[str]:
    """Parse and normalize user detector list to canonical uppercase names."""
    tokens = [t.strip() for t in det_str.split(",") if t.strip()]
    normalized = []
    for t in tokens:
        key = t.upper()
        if key in DETECTOR_MAP:
            if key not in normalized:
                normalized.append(key)
        elif t.lower() in DETECTOR_ALIASES:
            canonical = DETECTOR_ALIASES[t.lower()]
            if canonical not in normalized:
                normalized.append(canonical)
        else:
            sys.exit(f"Error: Unknown detector '{t}'. Supported: CEMC, HCALIN, HCALOUT")
    return normalized


def load_epochs(epoch_path: Path) -> List[Tuple[int, int, int]]:
    """Load epoch run ranges from CSV file. Returns list of (run_start, run_end, epoch_index)."""
    if not epoch_path.is_file():
        sys.exit(f"Error: Epoch file not found: {epoch_path}")

    epochs = []
    with open(epoch_path, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for idx, row in enumerate(reader, start=1):
            try:
                start = int(row["run_start"].strip())
                end = int(row["run_end"].strip())
                epochs.append((start, end, idx))
            except (KeyError, ValueError) as err:
                sys.exit(f"Error parsing {epoch_path} at line {idx+1}: {err}")
    return epochs


def get_epoch_for_run(run: int, epochs: List[Tuple[int, int, int]]) -> Optional[Tuple[int, int, int]]:
    """Find epoch tuple (start, end, idx) for a given run number."""
    for start, end, idx in epochs:
        if start <= run <= end:
            return (start, end, idx)
    return None


def extract_run_number(filepath: str) -> Optional[int]:
    """Extract run number from a file path or filename."""
    stem = Path(filepath).stem
    m = re.search(r'(?:^|run|[_-])(\d{5,7})(?:[_-]|$|\.)', stem)
    if m:
        return int(m.group(1))
    m2 = re.search(r'(\d{5,7})', stem)
    if m2:
        return int(m2.group(1))
    return None


def load_hist_list(hist_list_path: Path, epochs: List[Tuple[int, int, int]]) -> Tuple[Dict[int, Dict], Dict[int, List[int]]]:
    """
    Load reference histogram files from hist list.
    Maps epoch_index -> { 'run': int, 'path': str, 'epoch_range': (start, end) }
    Also returns conflicted_epochs: epoch_index -> [runs...] for groups with >1 candidate run.
    """
    if not hist_list_path.is_file():
        sys.exit(f"Error: Hist list file not found: {hist_list_path}")

    epoch_cands: Dict[int, List[Dict]] = {}
    with open(hist_list_path, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, start=1):
            path_str = line.strip()
            if not path_str or path_str.startswith("#"):
                continue

            if not os.path.exists(path_str):
                print(f"Warning: Histogram file at line {line_num} does not exist: {path_str}")

            run = extract_run_number(path_str)
            if run is None:
                print(f"Warning: Could not extract run number from '{path_str}' at line {line_num}")
                continue

            epoch_info = get_epoch_for_run(run, epochs)
            if epoch_info is None:
                print(f"Warning: Candidate run {run} does not match any epoch range in epoch.csv")
                continue

            start, end, epoch_idx = epoch_info
            epoch_cands.setdefault(epoch_idx, []).append({
                "run": run,
                "path": path_str,
                "epoch_range": (start, end),
                "epoch_idx": epoch_idx,
                "line_num": line_num,
            })

    candidate_map: Dict[int, Dict] = {}
    conflicted_epochs: Dict[int, List[int]] = {}

    for epoch_idx, cands in epoch_cands.items():
        if len(cands) == 1:
            candidate_map[epoch_idx] = cands[0]
        else:
            cand_runs = [c["run"] for c in cands]
            conflicted_epochs[epoch_idx] = cand_runs
            start, end = cands[0]["epoch_range"]
            print(
                f"\n[!] WARNING: Group {epoch_idx} [{start}, {end}] has {len(cands)} candidate runs "
                f"({', '.join(map(str, cand_runs))}) in {hist_list_path.name}."
            )
            print(f"    -> Skipping Group {epoch_idx} for manual investigation.\n")

    return candidate_map, conflicted_epochs


def load_run_list(runs_path: Path, filter_runs: Optional[Set[int]] = None) -> List[int]:
    """Load runs from run list file, maintaining file order and deduplicating."""
    if not runs_path.is_file():
        sys.exit(f"Error: Runs list file not found: {runs_path}")

    runs = []
    seen = set()
    with open(runs_path, "r", encoding="utf-8") as f:
        for line_num, line in enumerate(f, start=1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            try:
                r = int(line)
                if filter_runs is not None and r not in filter_runs:
                    continue
                if r not in seen:
                    runs.append(r)
                    seen.add(r)
            except ValueError:
                print(f"Warning: Invalid run number '{line}' at line {line_num}")
    return runs


def parse_runs_filter(filter_str: Optional[str]) -> Optional[Set[int]]:
    """Parse comma-separated runs or ranges (e.g. '67597,67633,67645-67650')."""
    if not filter_str:
        return None
    result = set()
    for token in filter_str.split(","):
        token = token.strip()
        if not token:
            continue
        if "-" in token:
            parts = token.split("-")
            if len(parts) == 2:
                try:
                    s, e = int(parts[0].strip()), int(parts[1].strip())
                    for r in range(s, e + 1):
                        result.add(r)
                    continue
                except ValueError:
                    pass
        try:
            result.add(int(token))
        except ValueError:
            sys.exit(f"Error parsing --runs filter token '{token}'")
    return result


def _init_root_environment():
    """Import and initialize ROOT and sPHENIX libraries inside a worker process."""
    global _worker_initialized, _ROOT
    if _worker_initialized:
        return _ROOT

    import ROOT
    _ROOT = ROOT
    _ROOT.gSystem.Load("libsphenixnpc")
    _ROOT.gSystem.Load("libcdbobjects")
    _ROOT.gSystem.Load("libcalo_io")

    processor_header = Path(__file__).resolve().parent / "zs_crosscalib_processor.h"
    if not processor_header.is_file():
        raise FileNotFoundError(f"Required header not found: {processor_header}")

    _ROOT.gInterpreter.ProcessLine(f'#include "{processor_header}"')
    _ROOT.gInterpreter.Declare("""
    #ifndef SPHENIX_CDB_RESOLVER
    #define SPHENIX_CDB_RESOLVER
    #include <sphenixnpc/CDBUtils.h>
    CDBUtils* g_sphenix_cdb_utils = nullptr;
    std::string g_current_tag = "";
    std::string resolve_sphenix_cdb_url(const std::string& pl_type, unsigned int runnumber, const std::string& tag) {
        if (!g_sphenix_cdb_utils) {
            g_sphenix_cdb_utils = new CDBUtils();
        }
        if (g_current_tag != tag) {
            g_sphenix_cdb_utils->setGlobalTag(tag);
            g_current_tag = tag;
        }
        return g_sphenix_cdb_utils->getUrl(pl_type, runnumber);
    }
    #endif
    """)
    _worker_initialized = True
    return _ROOT


def _get_worker_instance(epoch_idx: int, det: str, cand_run: int, hist_path: str, dbtag: str):
    """Retrieve or create a cached ZSCrossCalibWorker for (epoch_idx, detector) in worker process."""
    cache_key = (epoch_idx, det)
    if cache_key in _worker_cache:
        return _worker_cache[cache_key]

    root = _init_root_environment()
    cdb_type = DETECTOR_MAP[det]["cdb_name"]
    ref_url = str(root.resolve_sphenix_cdb_url(cdb_type, cand_run, dbtag))

    if ref_url.startswith("DataBaseException") or "error" in ref_url.lower():
        raise RuntimeError(f"Failed to fetch CDB payload for candidate run {cand_run}, det {det}: {ref_url}")

    worker = root.ZSCrossCalibWorker(det)
    success = worker.initCandidate(hist_path, ref_url)
    if not success:
        raise RuntimeError(f"Failed to initialize candidate histograms for {det} from {hist_path}")

    _worker_cache[cache_key] = worker
    return worker


def process_single_run_task(task_args: Tuple) -> Dict:
    """Worker task executed in parallel for a single run."""
    run, epoch_idx, cand_info, detectors, dbtag, output_dir_str = task_args
    cand_run = cand_info["run"]
    hist_path = cand_info["path"]
    output_dir = Path(output_dir_str)

    root = _init_root_environment()
    tree_status = {det: False for det in detectors}
    errors = []

    for det in detectors:
        try:
            worker = _get_worker_instance(epoch_idx, det, cand_run, hist_path, dbtag)
            cdb_type = DETECTOR_MAP[det]["cdb_name"]
            url = str(root.resolve_sphenix_cdb_url(cdb_type, run, dbtag))

            if url.startswith("DataBaseException") or "error" in url.lower():
                errors.append(f"{det}: CDB lookup failed ({url})")
                continue

            out_filename = f"{det}_ZSCrossCalibT0_pro001_pcdb001_v001_{run}.root"
            out_filepath = output_dir / out_filename

            ok = worker.processRun(run, url, str(out_filepath))
            tree_status[det] = bool(ok)
            if not ok:
                errors.append(f"{det}: processRun returned false")
        except Exception as e:
            errors.append(f"{det}: {str(e)}")

    all_success = all(tree_status.values())
    return {
        "run": run,
        "success": all_success,
        "tree_status": tree_status,
        "errors": errors,
    }


def main():
    args = parse_arguments()

    original_stdout = sys.stdout
    log_file_handle = None
    if args.log_file:
        args.log_file.parent.mkdir(parents=True, exist_ok=True)
        log_file_handle = open(args.log_file, "w", encoding="utf-8")
        sys.stdout = Tee(sys.stdout, log_file_handle)

    detectors = normalize_detector_list(args.detectors)
    runs_filter = parse_runs_filter(args.runs)

    print("=" * 85)
    print(" sPHENIX ZSCrossCalib CDBTTree Generator")
    print("=" * 85)
    print(f"Run list file:    {args.run_list}")
    print(f"Hist list file:   {args.hist_list}")
    print(f"Epoch file:       {args.epoch_file}")
    print(f"Output directory: {args.output_dir}")
    print(f"CDB Global Tag:   {args.dbtag}")
    print(f"Detectors:        {', '.join(detectors)}")
    print(f"Parallel jobs:    {args.jobs} worker process(es)")
    if runs_filter:
        print(f"Run filter:       {len(runs_filter)} explicit runs selected")
    print(f"Mode:             {'Dry run (validation only)' if args.dry_run else 'Production'}")
    print("-" * 85)

    # 1. Load inputs
    epochs = load_epochs(args.epoch_file)
    candidate_map, conflicted_epochs = load_hist_list(args.hist_list, epochs)
    target_runs = load_run_list(args.run_list, filter_runs=runs_filter)

    print(f"Loaded {len(epochs)} run groups from {args.epoch_file.name}")
    print(f"Loaded {len(candidate_map)} valid candidate run(s) from {args.hist_list.name}")
    if conflicted_epochs:
        print(f"Skipped {len(conflicted_epochs)} run group(s) with multiple candidate runs")
    print(f"Loaded {len(target_runs)} target runs to process\n")

    # 2. Check candidate runs per epoch
    print("Epoch Group & Candidate Mapping:")
    for start, end, idx in epochs:
        if idx in candidate_map:
            cand_info = candidate_map[idx]
            cand_run = cand_info["run"]
            hist_name = Path(cand_info["path"]).name
            print(f"  Group {idx:2d} [{start:5d}, {end:5d}] -> Candidate Run {cand_run:5d} ({hist_name})")
        elif idx in conflicted_epochs:
            cands_str = ", ".join(map(str, conflicted_epochs[idx]))
            print(f"  Group {idx:2d} [{start:5d}, {end:5d}] -> [SKIPPED: {len(conflicted_epochs[idx])} CANDIDATE RUNS ({cands_str})]")
        else:
            print(f"  Group {idx:2d} [{start:5d}, {end:5d}] -> [NO CANDIDATE IN HIST LIST]")
    print("-" * 85)

    if not candidate_map:
        if conflicted_epochs:
            sys.exit("Error: No single-candidate run groups available (all matching groups had multiple candidates).")
        else:
            sys.exit("Error: No valid candidate runs found in hist list matching epoch.csv.")

    # 3. Initialize ROOT in main process to evaluate Candidate zero-ref summary
    t_start = time.time()
    ROOT = _init_root_environment()

    print("\nEvaluating Candidate Zero-Ref Denominators...")
    zero_ref_stats: Dict[int, Dict[str, Dict[str, int]]] = {}

    for epoch_idx, cand_info in candidate_map.items():
        cand_run = cand_info["run"]
        hist_path = cand_info["path"]
        zero_ref_stats[epoch_idx] = {}

        for det in detectors:
            cdb_type = DETECTOR_MAP[det]["cdb_name"]
            ref_url = str(ROOT.resolve_sphenix_cdb_url(cdb_type, cand_run, args.dbtag))

            if ref_url.startswith("DataBaseException") or "error" in ref_url.lower():
                print(f"Error: Failed to fetch CDB payload for candidate run {cand_run}, detector {det}: {ref_url}")
                continue

            worker = ROOT.ZSCrossCalibWorker(det)
            success = worker.initCandidate(hist_path, ref_url)
            if not success:
                print(f"Error: Failed to initialize candidate histograms for {det} from {hist_path}")
                continue

            zero_count = worker.getZeroRefCount()
            total_ch = worker.getTotalChannels()
            zero_ref_stats[epoch_idx][det] = {
                "zero": zero_count,
                "total": total_ch,
                "pct": (zero_count / total_ch * 100.0) if total_ch > 0 else 0.0,
            }

    # Print Candidate Zero-Ref Summary
    print("\n" + "=" * 85)
    print(" CANDIDATE RUN ZERO-REF DENOMINATOR SUMMARY")
    print(" (Towers where reference CDB ratio == 0, falling back to 0.0)")
    print("=" * 85)
    print(f"{'Group':<9} {'Cand Run':<10} {'Detector':<10} {'Zero Towers':<15} {'Total Towers':<15} {'Zero %':<10}")
    print("-" * 85)
    for epoch_idx, cand_info in candidate_map.items():
        cand_run = cand_info["run"]
        for det in detectors:
            if det in zero_ref_stats.get(epoch_idx, {}):
                st = zero_ref_stats[epoch_idx][det]
                print(
                    f"Group {epoch_idx:<3} {cand_run:<10} {det:<10} "
                    f"{st['zero']:<15} {st['total']:<15} {st['pct']:>6.2f}%"
                )
    print("-" * 85)

    if args.dry_run:
        print("\nDry run completed successfully. No CDBTree files written.")
        if log_file_handle:
            sys.stdout = original_stdout
            log_file_handle.close()
        return

    # 4. Prepare tasks for target runs
    args.output_dir.mkdir(parents=True, exist_ok=True)
    out_dir_str = str(args.output_dir)

    tasks = []
    skipped_no_epoch = 0
    skipped_no_cand = 0
    skipped_multi_cand = 0

    for run in target_runs:
        epoch_info = get_epoch_for_run(run, epochs)
        if epoch_info is None:
            skipped_no_epoch += 1
            continue

        start, end, epoch_idx = epoch_info
        if epoch_idx in conflicted_epochs:
            skipped_multi_cand += 1
            continue

        cand_info = candidate_map.get(epoch_idx)
        if cand_info is None:
            skipped_no_cand += 1
            continue

        tasks.append((run, epoch_idx, cand_info, detectors, args.dbtag, out_dir_str))

    print(f"\nProcessing {len(tasks)} eligible target run(s) across {args.jobs} worker process(es)...")

    processed_count = 0
    failed_cdb = 0
    success_trees = {det: 0 for det in detectors}

    # 5. Execute processing (parallel if jobs > 1, sequential if jobs == 1)
    if args.jobs > 1 and len(tasks) > 1:
        with concurrent.futures.ProcessPoolExecutor(max_workers=args.jobs) as executor:
            for result in tqdm.tqdm(executor.map(process_single_run_task, tasks), total=len(tasks), desc="Runs"):
                if result["success"]:
                    processed_count += 1
                else:
                    failed_cdb += 1
                    if args.verbose and result["errors"]:
                        print(f"Run {result['run']} issues: {', '.join(result['errors'])}")

                for det, ok in result["tree_status"].items():
                    if ok:
                        success_trees[det] += 1
    else:
        for task in tqdm.tqdm(tasks, desc="Runs"):
            result = process_single_run_task(task)
            if result["success"]:
                processed_count += 1
            else:
                failed_cdb += 1
                if args.verbose and result["errors"]:
                    print(f"Run {result['run']} issues: {', '.join(result['errors'])}")

            for det, ok in result["tree_status"].items():
                if ok:
                    success_trees[det] += 1

    elapsed = time.time() - t_start

    # 6. Final Summary Report
    print("\n" + "=" * 85)
    print(" EXECUTION SUMMARY")
    print("=" * 85)
    print(f"Total target runs in input list:        {len(target_runs)}")
    print(f"Target runs successfully processed:     {processed_count}")
    if skipped_multi_cand > 0:
        print(f"Runs skipped (multiple candidate runs): {skipped_multi_cand}")
    print(f"Runs skipped (no candidate in hist list): {skipped_no_cand}")
    print(f"Runs skipped (no matching epoch range):  {skipped_no_epoch}")
    print(f"Runs with failed CDB/generation issues: {failed_cdb}")
    print("-" * 85)
    print("CDBTTree files created:")
    for det in detectors:
        print(f"  - {det:<10}: {success_trees[det]} files")
    print("-" * 85)
    print(f"Output directory:                       {args.output_dir}")
    print(f"Workers used:                           {args.jobs}")
    print(f"Total execution time:                   {elapsed:.2f} seconds")
    print("=" * 85)

    if conflicted_epochs:
        print("\n [!] NOTE: The following run group(s) were SKIPPED due to multiple candidate runs:")
        for epoch_idx, cands in conflicted_epochs.items():
            start, end, _ = [e for e in epochs if e[2] == epoch_idx][0]
            print(f"    - Group {epoch_idx} [{start}, {end}]: {len(cands)} runs found ({', '.join(map(str, cands))})")
        print("    Please investigate the hist list to ensure each group has at most one candidate run.")
        print("=" * 85)

    if log_file_handle:
        print(f"\nLog saved to: {args.log_file}")
        sys.stdout = original_stdout
        log_file_handle.close()


if __name__ == "__main__":
    main()
