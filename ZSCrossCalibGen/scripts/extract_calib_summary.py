#!/usr/bin/env python3
"""
extract_calib_summary.py

Extracts per-run aggregate calibration metrics from generated ZSCrossCalib CDB ROOT files.
Uses uproot and multiprocessing for high-throughput vectorized extraction across thousands of runs.
Outputs a compact CSV summary table used for rapid, interactive validation plotting.
"""

import argparse
import concurrent.futures
import csv
import glob
import os
from pathlib import Path
import re
import sys
import time
from typing import Dict, List, Optional, Tuple

import numpy as np
import pandas as pd
import tqdm
import uproot


def parse_arguments() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    repo_files_dir = repo_root / "files"

    parser = argparse.ArgumentParser(
        description="Extract aggregate metrics from generated ZSCrossCalib CDB ROOT files."
    )
    parser.add_argument(
        "--input-dir",
        type=Path,
        required=True,
        help="Directory containing generated CDB ROOT files",
    )
    parser.add_argument(
        "--output-file",
        type=Path,
        default=repo_root / "scratch" / "calib_validation_summary.csv",
        help="Path for output CSV summary (default: ZSCrossCalibGen/scratch/calib_validation_summary.csv)",
    )
    parser.add_argument(
        "--epoch-file",
        type=Path,
        default=repo_files_dir / "epoch.csv",
        help="Path to epoch CSV file (default: ZSCrossCalibGen/files/epoch.csv)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=min(os.cpu_count() or 4, 16),
        help="Number of parallel worker jobs (default: auto, up to 16)",
    )
    return parser.parse_args()


def load_epochs(epoch_path: Path) -> List[Tuple[int, int, int]]:
    """Load epoch run ranges from CSV file. Returns list of (run_start, run_end, epoch_idx)."""
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


def get_epoch_for_run(run: int, epochs: List[Tuple[int, int, int]]) -> Optional[int]:
    """Find epoch index (1-based) for a given run number."""
    for start, end, idx in epochs:
        if start <= run <= end:
            return idx
    return None


def process_single_file(file_path: str, epochs: List[Tuple[int, int, int]]) -> Optional[Dict]:
    """Worker task extracting summary statistics for a single CDB ROOT file."""
    fname = os.path.basename(file_path)
    # Parse filename format: <DET>_ZSCrossCalibT0_pro001_pcdb001_v001_<run>.root
    m = re.match(r"^([A-Z]+)_ZSCrossCalibT0_.*_(\d+)\.root$", fname)
    if not m:
        return None

    detector = m.group(1)
    run = int(m.group(2))
    epoch_idx = get_epoch_for_run(run, epochs)

    try:
        with uproot.open(file_path) as f:
            if "Multiple" not in f:
                return None
            tree = f["Multiple"]
            arrays = tree.arrays(
                ["IID", "Fratio", "Fratio_main", "Fratio_minus5", "Fratio_plus5"],
                library="np",
            )
    except Exception as e:
        print(f"Error reading {file_path}: {e}")
        return None

    orig = arrays["Fratio"]
    main = arrays["Fratio_main"]
    minus5 = arrays["Fratio_minus5"]
    plus5 = arrays["Fratio_plus5"]

    n_total = len(orig)
    active_mask = orig > 0.0
    n_active = int(np.sum(active_mask))
    n_zero = n_total - n_active

    row = {
        "run": run,
        "detector": detector,
        "epoch": epoch_idx if epoch_idx is not None else -1,
        "n_total": n_total,
        "n_active": n_active,
        "n_zero": n_zero,
        "zero_frac": n_zero / n_total if n_total > 0 else 0.0,
    }

    if n_active > 0:
        o_act = orig[active_mask]
        m_act = main[active_mask]
        m5_act = minus5[active_mask]
        p5_act = plus5[active_mask]

        row["orig_mean"] = float(np.mean(o_act))
        row["orig_std"] = float(np.std(o_act))
        row["orig_median"] = float(np.median(o_act))

        row["main_mean"] = float(np.mean(m_act))
        row["main_std"] = float(np.std(m_act))
        row["main_median"] = float(np.median(m_act))

        row["minus5_mean"] = float(np.mean(m5_act))
        row["minus5_std"] = float(np.std(m5_act))
        row["minus5_median"] = float(np.median(m5_act))

        row["plus5_mean"] = float(np.mean(p5_act))
        row["plus5_std"] = float(np.std(p5_act))
        row["plus5_median"] = float(np.median(p5_act))

        # Relative shift metrics
        ratio_shift = m_act / o_act
        row["shift_main_mean"] = float(np.mean(ratio_shift))
        row["shift_main_std"] = float(np.std(ratio_shift))

        m5_to_main = m5_act / np.where(m_act > 0, m_act, 1.0)
        p5_to_main = p5_act / np.where(m_act > 0, m_act, 1.0)
        row["ratio_minus5_to_main_mean"] = float(np.mean(m5_to_main))
        row["ratio_plus5_to_main_mean"] = float(np.mean(p5_to_main))

        envelope = (p5_act - m5_act) / np.where(m_act > 0, m_act, 1.0)
        row["envelope_spread_mean"] = float(np.mean(envelope))
    else:
        for k in [
            "orig_mean", "orig_std", "orig_median",
            "main_mean", "main_std", "main_median",
            "minus5_mean", "minus5_std", "minus5_median",
            "plus5_mean", "plus5_std", "plus5_median",
            "shift_main_mean", "shift_main_std",
            "ratio_minus5_to_main_mean", "ratio_plus5_to_main_mean",
            "envelope_spread_mean",
        ]:
            row[k] = 0.0

    return row


def extract_summary(
    input_dir: Path,
    output_file: Optional[Path] = None,
    epoch_file: Optional[Path] = None,
    jobs: Optional[int] = None,
) -> pd.DataFrame:
    """Extract calibration summary DataFrame from ROOT files in input_dir."""
    input_dir = Path(input_dir)
    if not input_dir.is_dir():
        sys.exit(f"Error: Input directory does not exist: {input_dir}")

    script_dir = Path(__file__).resolve().parent
    repo_files_dir = script_dir.parent / "files"
    if epoch_file is None:
        epoch_file = repo_files_dir / "epoch.csv"
    epoch_file = Path(epoch_file)

    if jobs is None:
        jobs = min(os.cpu_count() or 4, 16)

    epochs = load_epochs(epoch_file)
    print(f"Loaded {len(epochs)} run groups from {epoch_file.name}")

    pattern = str(input_dir / "*.root")
    file_list = sorted(glob.glob(pattern))
    if not file_list:
        sys.exit(f"Error: No .root files found in {input_dir}")

    print(f"Found {len(file_list)} ROOT files in {input_dir}")
    print(f"Extracting calibration metrics using {jobs} worker process(es)...")

    t_start = time.time()
    results = []

    tasks = [(f, epochs) for f in file_list]

    if jobs > 1 and len(tasks) > 1:
        with concurrent.futures.ProcessPoolExecutor(max_workers=jobs) as executor:
            for row in tqdm.tqdm(executor.map(process_single_file_wrapper, tasks), total=len(tasks), desc="Extracting"):
                if row:
                    results.append(row)
    else:
        for f, eps in tqdm.tqdm(tasks, desc="Extracting"):
            row = process_single_file(f, eps)
            if row:
                results.append(row)

    elapsed = time.time() - t_start

    if not results:
        sys.exit("Error: No valid records extracted.")

    df = pd.DataFrame(results)
    df.sort_values(by=["detector", "run"], inplace=True)

    if output_file is not None:
        output_file = Path(output_file)
        output_file.parent.mkdir(parents=True, exist_ok=True)
        df.to_csv(output_file, index=False)

    print("\n" + "=" * 80)
    print(" EXTRACTION SUMMARY")
    print("=" * 80)
    print(f"Files processed:           {len(results)} / {len(file_list)}")
    print(f"Unique runs:               {df['run'].nunique()}")
    print(f"Detectors:                 {', '.join(df['detector'].unique())}")
    for det in sorted(df["detector"].unique()):
        det_df = df[df["detector"] == det]
        print(f"  - {det:<10}: {len(det_df)} runs (Run range {det_df['run'].min()} - {det_df['run'].max()})")
    if output_file is not None:
        print(f"Output summary CSV:        {output_file}")
    print(f"Extraction time:           {elapsed:.2f} seconds")
    print("=" * 80)

    return df


def process_single_file_wrapper(args_tuple):
    return process_single_file(*args_tuple)


def main():
    args = parse_arguments()
    extract_summary(
        input_dir=args.input_dir,
        output_file=args.output_file,
        epoch_file=args.epoch_file,
        jobs=args.jobs,
    )


if __name__ == "__main__":
    main()
