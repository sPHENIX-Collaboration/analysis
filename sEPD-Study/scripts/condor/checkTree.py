#!/usr/bin/env python3
import argparse
import sys
import os
import multiprocessing
from pathlib import Path
from collections import Counter
import uproot
from tqdm import tqdm

def check_tree(filename):
    """
    Checks the status of the TTree 'T' in a given ROOT file.
    Returns a tuple (stem, status) where status is one of 'ok', 'corrupt', 'missing', 'empty'.
    """
    filename = filename.strip()
    if not filename:
        return None
    stem = Path(filename).stem
    try:
        # Check if file can be opened. Uproot raises exceptions for corrupt/zombie files.
        with uproot.open(filename) as f:
            if "T" not in f:
                # Also check keys in case it's named 'T;1' etc.
                keys = [k.split(';')[0] for k in f.keys()]
                if "T" not in keys:
                    return stem, "missing"

            tree = f["T"]

            # Check if it has the num_entries attribute (indicates it's a TTree)
            if not hasattr(tree, "num_entries"):
                return stem, "missing"

            if tree.num_entries == 0:
                return stem, "empty"

            return stem, "ok"
    except Exception:
        # Any exception during open or reading indicates a corrupt file
        return stem, "corrupt"

def main():
    parser = argparse.ArgumentParser(description="Check ROOT trees in parallel.")
    parser.add_argument("input_file_list", help="List of ROOT files to check")
    parser.add_argument("output", nargs="?", default="tree-check-log.txt", help="Output file")
    parser.add_argument("-j", "--jobs", type=int, default=multiprocessing.cpu_count(), help="Number of parallel jobs to use")
    args = parser.parse_args()

    print(f"{'':#<20}")
    print(f"Run Params")
    print(f"Input: {args.input_file_list}")
    print(f"Output: {args.output}")
    print(f"Jobs: {args.jobs}")
    print(f"{'':#<20}")

    if not os.path.exists(args.input_file_list):
        print(f"Error: Could not open file {args.input_file_list}")
        sys.exit(1)

    with open(args.input_file_list, "r") as f:
        filenames = [line.strip() for line in f if line.strip()]

    total_files = len(filenames)

    if total_files == 0:
        print("Error: Input file list is empty")
        sys.exit(1)

    results = []

    # We use a multiprocessing Pool to process files in parallel
    with multiprocessing.Pool(args.jobs) as pool:
        # Use tqdm progress bar over parallel execution
        for res in tqdm(pool.imap_unordered(check_tree, filenames), total=total_files, desc="Checking ROOT files"):
            if res is None:
                continue
            results.append(res)

    # Write output log
    with open(args.output, "w") as f_out:
        f_out.write("file,status\n")
        # Optional: sort results by stem to have deterministic output like sequential processing
        for stem, status in sorted(results, key=lambda x: x[0]):
            f_out.write(f"{stem},{status}\n")

    # Stats reporting
    ctr = Counter(status for _, status in results)
    ctr["total_files"] = total_files

    print(f"{'':#<20}")
    # Sort keys alphabetically to match std::map behavior in C++
    for name, counts in sorted(ctr.items()):
        print(f"{name}: {counts}")
    print(f"{'':#<20}")

    print("Analysis complete.")

if __name__ == "__main__":
    main()
