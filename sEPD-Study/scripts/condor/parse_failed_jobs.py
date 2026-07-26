#!/usr/bin/env python3
import argparse
import multiprocessing
import re
from pathlib import Path

def check_log_file(log_path: Path, input_dir: Path):
    """
    Checks the log file for fully skimmed status, MB: 0 failure, total events skimmed,
    total events in 'All: X' field for MB failures, and extracts all run-segments.
    Returns (is_fully_skimmed, is_mb_0, run_segments, events_skimmed, events_all)
    """
    is_fully_skimmed = False
    is_mb_0 = False
    run_segments = []

    if not log_path.exists():
        return False, False, [], 0, 0

    events_processed = -1
    events_skimmed = 0
    events_all = 0
    list_file_path = None

    # Read the log file line by line
    with log_path.open('r', errors='ignore') as f:
        for line in f:
            if "CaloStatusSkimmer::End Total events processed:" in line:
                m = re.search(r'Total events processed:\s*(\d+)', line)
                if m:
                    events_processed = int(m.group(1))
            elif "CaloStatusSkimmer::End Total events skimmed:" in line:
                m = re.search(r'Total events skimmed:\s*(\d+)', line)
                if m:
                    events_skimmed = int(m.group(1))
            elif line.startswith("All:"):
                m = re.search(r'All:\s*(\d+)', line)
                if m:
                    events_all = int(m.group(1))
            elif "MB: 0" in line:
                is_mb_0 = True

            if list_file_path is None and "Reading inputs from:" in line:
                m = re.search(r'Reading inputs from:\s*(\S+)', line)
                if m:
                    raw_path = Path(m.group(1))
                    if raw_path.exists():
                        list_file_path = raw_path
                    elif (input_dir / "files" / raw_path.name).exists():
                        list_file_path = input_dir / "files" / raw_path.name
                    elif (input_dir / raw_path.name).exists():
                        list_file_path = input_dir / raw_path.name

    if events_processed != -1 and events_processed == events_skimmed:
        is_fully_skimmed = True

    # Extract all run segments from the .list file if found
    if list_file_path and list_file_path.exists():
        with list_file_path.open('r', errors='ignore') as f:
            for line in f:
                matches = re.findall(r'(\d{8}-\d{5})', line)
                for seg in matches:
                    if seg not in run_segments:
                        run_segments.append(seg)
    else:
        # Fallback: scan log file for all matches
        with log_path.open('r', errors='ignore') as f:
            for line in f:
                matches = re.findall(r'-(\d{8}-\d{5})\.root', line)
                for seg in matches:
                    if seg not in run_segments:
                        run_segments.append(seg)

    return is_fully_skimmed, is_mb_0, run_segments, events_skimmed, events_all

def process_entry(entry):
    tree_id, input_dir = entry
    # tree_id format: tree-JOBID-SUBJOBID, e.g. tree-642132-4259
    parts = tree_id.split('-')
    if len(parts) >= 3:
        job_id = parts[1]
        subjob_id = parts[2]
        log_path = input_dir / "stdout" / f"job-{job_id}-{subjob_id}.out"
        fully_skimmed, mb_0, run_segments, events_skimmed, events_all = check_log_file(log_path, input_dir)
        return (tree_id, fully_skimmed, mb_0, run_segments, events_skimmed, events_all)
    return (tree_id, False, False, [], 0, 0)

def filter_input_list_dir(filter_dir: Path, bad_segments: list, custom_removed_dir_name: str = None, dry_run: bool = False):
    """
    Removes lines matching bad run-segments from input .list files in filter_dir,
    saves the removed lines into a new folder, and deletes any empty .list files.
    If dry_run is True, performs all checks without altering files on disk.
    """
    filter_dir = filter_dir.resolve()
    if not filter_dir.is_dir():
        print(f"Error: Specified filter directory '{filter_dir}' is not a directory.")
        return

    dir_name = filter_dir.name
    if custom_removed_dir_name:
        new_dir_name = custom_removed_dir_name
    elif "-pro" in dir_name:
        idx = dir_name.find("-pro")
        new_dir_name = dir_name[:idx] + "-fully-skimmed" + dir_name[idx:]
    else:
        new_dir_name = dir_name + "-fully-skimmed"

    removed_dir = filter_dir.parent / new_dir_name
    if not dry_run:
        removed_dir.mkdir(parents=True, exist_ok=True)

    bad_set = set(bad_segments)

    total_files_processed = 0
    total_files_modified = 0
    total_files_removed = 0
    total_lines_removed = 0

    list_files = sorted(list(filter_dir.glob("*.list")))
    if not list_files:
        list_files = sorted([f for f in filter_dir.iterdir() if f.is_file()])

    for list_file in list_files:
        total_files_processed += 1
        kept_lines = []
        removed_lines = []

        with list_file.open('r', errors='ignore') as f:
            for line in f:
                raw_line = line.rstrip('\r\n')
                if not raw_line.strip():
                    continue
                matches = re.findall(r'(\d{8}-\d{5})', raw_line)
                if any(m in bad_set for m in matches):
                    removed_lines.append(raw_line)
                else:
                    kept_lines.append(raw_line)

        if removed_lines:
            total_lines_removed += len(removed_lines)
            if not dry_run:
                removed_file = removed_dir / list_file.name
                with removed_file.open('w') as f:
                    for l in removed_lines:
                        f.write(f"{l}\n")

            if not kept_lines:
                if not dry_run:
                    list_file.unlink()
                total_files_removed += 1
            else:
                if not dry_run:
                    with list_file.open('w') as f:
                        for l in kept_lines:
                            f.write(f"{l}\n")
                total_files_modified += 1

    print("-" * 50)
    dry_prefix = "[DRY RUN] " if dry_run else ""
    print(f"{dry_prefix}Filter Input Directory Summary ({filter_dir}):")
    print(f"  Target removed lines folder: {removed_dir}")
    print(f"  Total list files processed: {total_files_processed}")
    print(f"  Files to modify (partially cleaned): {total_files_modified}")
    print(f"  Files to completely remove (all lines bad): {total_files_removed}")
    print(f"  Total bad run-segment lines extracted/removed: {total_lines_removed}")
    if dry_run:
        print("  * Note: Dry run mode active. No files were created, modified, or deleted on disk.")
    print("-" * 50)

def main():
    parser = argparse.ArgumentParser(
        description="Parse job log files for status != 'ok' entries from a tree-check file."
    )
    parser.add_argument(
        "input_file",
        type=Path,
        help="Path to input tree-check file (e.g., tree-check.txt)"
    )
    parser.add_argument(
        "-o", "--output-dir",
        type=Path,
        default=Path.cwd(),
        help="Directory to write output list files (default: current working directory)"
    )
    parser.add_argument(
        "-f", "--filter-input-dir",
        type=Path,
        default=None,
        help="Optional path to input list directory to filter out bad run-segments"
    )
    parser.add_argument(
        "--removed-dir-name",
        type=str,
        default=None,
        help="Optional custom folder name for extracted bad lines (default: inserts '-fully-skimmed')"
    )
    parser.add_argument(
        "-n", "--dry-run",
        action="store_true",
        help="Perform a dry run when filtering input list directory without modifying any files on disk"
    )

    args = parser.parse_args()

    input_file = args.input_file.resolve()
    if not input_file.is_file():
        parser.error(f"Input file '{input_file}' does not exist or is not a file.")

    input_dir = input_file.parent
    output_dir = args.output_dir.resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    entries_to_check = []

    print(f"Reading input file: {input_file}")
    with input_file.open('r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(',')
            if len(parts) >= 2:
                tree_id = parts[0]
                status = parts[1]
                # Process lines that are not "ok" and skip the header
                if status != "ok" and tree_id != "file":
                    entries_to_check.append((tree_id, input_dir))

    print(f"Found {len(entries_to_check)} jobs with status != 'ok'")

    if len(entries_to_check) == 0:
        print("No failed jobs to process. Exiting.")
        return

    print("Parsing log files and input list files...")

    fully_skimmed_runs = []
    mb_0_runs = []
    mb_0_not_skimmed_runs = []
    all_bad_runs = []
    other_failures_count = 0
    total_events_skimmed = 0
    total_events_mb_fail = 0

    # Use multiprocessing for efficiency
    pool_size = min(multiprocessing.cpu_count(), 16)
    with multiprocessing.Pool(processes=pool_size) as pool:
        results = pool.map(process_entry, entries_to_check)

    for res in results:
        tree_id, fully_skimmed, mb_0, run_segments, events_skimmed, events_all = res

        if fully_skimmed:
            total_events_skimmed += events_skimmed
        elif mb_0:
            total_events_mb_fail += events_all

        segments_to_add = run_segments if run_segments else [f"UNKNOWN_RUNSEG_{tree_id}"]
        all_bad_runs.extend(segments_to_add)

        if fully_skimmed:
            fully_skimmed_runs.extend(segments_to_add)

        if mb_0:
            mb_0_runs.extend(segments_to_add)
            if not fully_skimmed:
                mb_0_not_skimmed_runs.extend(segments_to_add)

        if not fully_skimmed and not mb_0:
            other_failures_count += 1

    print("-" * 50)
    print(f"Total fully skimmed jobs: {len([r for r in results if r[1]])} (Total run-segments: {len(fully_skimmed_runs)})")
    print(f"Total MB: 0 failures (all jobs): {len([r for r in results if r[2]])} (Total run-segments: {len(mb_0_runs)})")
    print(f"Total MB: 0 failures (NOT fully skimmed jobs): {len([r for r in results if r[2] and not r[1]])} (Total run-segments: {len(mb_0_not_skimmed_runs)})")
    print(f"Total bad jobs (all status != 'ok'): {len(results)} (Total run-segments: {len(all_bad_runs)})")
    print(f"Total failures due to other reasons: {other_failures_count}")
    print(f"Total events skimmed (fully skimmed jobs): {total_events_skimmed}")
    print(f"Total events skipped due to MB failure (non-skimmed MB: 0 jobs, sum of 'All:'): {total_events_mb_fail}")
    print("-" * 50)

    if fully_skimmed_runs:
        fs_file = output_dir / "fully_skimmed_runsegments.list"
        with fs_file.open("w") as f:
            for r in fully_skimmed_runs:
                f.write(f"{r}\n")
        print(f"Wrote fully skimmed run-segments to: {fs_file}")

    if mb_0_not_skimmed_runs:
        mb_file = output_dir / "mb_0_not_fully_skimmed_runsegments.list"
        with mb_file.open("w") as f:
            for r in mb_0_not_skimmed_runs:
                f.write(f"{r}\n")
        print(f"Wrote MB: 0 (not fully skimmed) run-segments to: {mb_file}")

    if all_bad_runs:
        all_file = output_dir / "all_bad_runsegments.list"
        with all_file.open("w") as f:
            for r in all_bad_runs:
                f.write(f"{r}\n")
        print(f"Wrote all bad run-segments to: {all_file}")

    # Optional feature: Filter input list directory
    if args.filter_input_dir:
        filter_input_list_dir(args.filter_input_dir, all_bad_runs, args.removed_dir_name, dry_run=args.dry_run)

if __name__ == '__main__':
    main()
