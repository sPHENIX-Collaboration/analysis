#!/usr/bin/env python3

import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import mplhep as hep
import os
import tqdm
import concurrent.futures
import argparse
from pathlib import Path
import numpy as np
import re
import pandas as pd
import pickle
import sys

from matplotlib.lines import Line2D

def process_file(path):
    path = Path(path)
    if not path.exists():
        return None, None, None, None, f"File not found: {path}"

    try:
        # Try to extract run number from filename (e.g. EMCalHotMap_..._78348.root)
        match = re.search(r'_(\d+)\.root', path.name)
        if match:
            run_number = int(match.group(1))
        else:
            # Fallback
            match = re.search(r'\d+', path.name)
            if match:
                run_number = int(match.group())
            else:
                return None, None, None, None, f"Could not parse run number from {path.name}"

        with uproot.open(path) as file:
            if "h_hot" not in file:
                return None, None, None, None, f"h_hot not found in {path}"

            hist = file["h_hot"]
            values = hist.values()

            # Count bad (non-zero), dead (value == 1), and hot (value == 2) towers
            bad_towers_count = np.count_nonzero(values != 0)
            dead_towers_count = np.count_nonzero(values == 1)
            hot_towers_count = np.count_nonzero(values == 2)

            return run_number, bad_towers_count, dead_towers_count, hot_towers_count, None
    except Exception as e:
        return None, None, None, None, f"Error processing {path}: {e}"

def plot_towers(run_numbers, towers_count, output_dir, name, ylabel="Number of Bad Towers", suffix="", extra_text=None, ylim_bottom=None, legend_loc='lower left', legend_fontsize=18, sigma_threshold=None):
    hep.style.use("ATLAS")

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(run_numbers, towers_count, marker='o', markersize=4, linestyle='none', color='black')

    runs = np.array(run_numbers)
    towers = np.array(towers_count)

    group_defs = [
        (runs <= 69137, 'blue', r'$\leq$ 69137'),
        ((runs >= 69272) & (runs <= 69433), 'cyan', '69272-69433'),
        ((runs >= 69539) & (runs < 73500), 'magenta', '69539-73500'),
        ((runs >= 73500) & (runs <= 78217), 'red', '73500-78217'),
        (runs >= 78218, 'green', r'$\geq$ 78218'),
    ]

    total_runs = len(run_numbers)
    text_info = f"Runs = {total_runs}"

    avg_handles = []
    sigma_handles = []

    for mask, color, label_str in group_defs:
        if np.any(mask):
            group_towers = towers[mask]
            group_runs = runs[mask]
            avg = np.mean(group_towers)
            xmin, xmax = np.min(group_runs), np.max(group_runs)

            ax.hlines(avg, xmin, xmax, colors=color, linestyles='--', linewidth=1.5)
            avg_handles.append(Line2D([0], [0], color=color, linestyle='--', linewidth=1.5,
                                      label=rf'Avg ({label_str}) = {avg:.0f}'))

            if sigma_threshold is not None:
                std = np.std(group_towers)
                if std > 0:
                    thresh = avg + sigma_threshold * std
                    sig_str = f"{sigma_threshold:g}"
                    n_crossed = np.count_nonzero(group_towers > thresh)
                    ax.hlines(thresh, xmin, xmax, colors=color, linestyles=':', linewidth=1.2)
                    sigma_handles.append(Line2D([0], [0], color=color, linestyle=':', linewidth=1.2,
                                                label=rf'+{sig_str}$\sigma$ = {thresh:.0f} (Runs={n_crossed})'))

    if legend_loc == 'lower left':
        leg1 = ax.legend(handles=avg_handles, loc=legend_loc, bbox_to_anchor=(0.0, -0.03), frameon=False, fontsize=legend_fontsize)
    else:
        leg1 = ax.legend(handles=avg_handles, loc=legend_loc, frameon=False, fontsize=legend_fontsize)

    if sigma_threshold is not None and sigma_handles:
        ax.add_artist(leg1)
        ax.legend(handles=sigma_handles, loc='lower left', bbox_to_anchor=(0.3, -0.03), frameon=False, fontsize=legend_fontsize)

    ax.set_xlabel("Run Number", labelpad=18)
    ax.set_ylabel(ylabel)
    ax.set_title("EMCal QA")
    if ylim_bottom is not None:
        ax.set_ylim(bottom=ylim_bottom)

    if extra_text:
        text_info += f"\n{extra_text}"

    ax.text(0.05, 0.95, text_info, transform=ax.transAxes, ha='left', va='top', fontsize=18)

    plt.tight_layout()
    plt.subplots_adjust(top=0.94, bottom=0.14)

    pdf_dir = output_dir / "pdf"
    image_dir = output_dir / "images"
    pdf_dir.mkdir(parents=True, exist_ok=True)
    image_dir.mkdir(parents=True, exist_ok=True)

    pdf_path = pdf_dir / f"{name}{suffix}.pdf"
    png_path = image_dir / f"{name}{suffix}.png"
    plt.savefig(pdf_path)
    plt.savefig(png_path, dpi=300)
    plt.close(fig)
    print(f"Saved plots as {pdf_path} and {png_path}")

# Maintain backward compatibility for plot_bad_towers function name if imported elsewhere
plot_bad_towers = plot_towers

def main():
    parser = argparse.ArgumentParser(description="Plot Bad, Dead, and Hot Towers vs Run from a list of ROOT files.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("-n", "--name", "--plot-name", dest="name", type=str, default="bad_towers_per_run", help="Base filename for output plots.")
    parser.add_argument("-s", "--sigma-threshold", type=float, default=1.0, help="N-sigma threshold above average for identifying outlier runs (default: 1.0).")
    parser.add_argument("--cache-file", type=Path, default=None, help="Path to cache file for processed ROOT file data.")
    parser.add_argument("--no-cache", action="store_true", help="Disable caching and force re-processing of all ROOT files.")
    parser.add_argument("files", nargs="*", type=Path, help="List of ROOT file paths")
    args = parser.parse_args()

    file_list = []
    if args.files:
        file_list.extend(args.files)

    if args.file:
        try:
            with args.file.open('r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#'):
                        file_list.append(Path(line))
        except Exception as e:
            print(f"Error reading file {args.file}: {e}")
            sys.exit(1)

    if not file_list:
        print("Error: You must provide at least one ROOT file or a text file containing ROOT file paths.")
        parser.print_help()
        sys.exit(1)

    print(f"Found {len(file_list)} input files.")

    cache = {}
    default_cache_name = ".bad_towers_cache.pkl"
    cache_path = args.cache_file if args.cache_file is not None else args.output_dir / default_cache_name

    if not args.no_cache and cache_path.exists():
        try:
            with cache_path.open("rb") as f:
                cache = pickle.load(f)
            print(f"Loaded {len(cache)} records from cache ({cache_path}).")
        except Exception as e:
            print(f"Warning: Could not read cache file {cache_path}: {e}")
            cache = {}

    files_to_process = []
    results = []

    for path in file_list:
        path = Path(path)
        resolved_str = str(path.resolve())
        mtime = path.stat().st_mtime if path.exists() else 0

        if not args.no_cache and resolved_str in cache:
            entry = cache[resolved_str]
            res = entry.get("result")
            if entry.get("mtime") == mtime and isinstance(res, (tuple, list)) and len(res) == 5:
                results.append(res)
                continue

        files_to_process.append(path)

    if files_to_process:
        print(f"Processing {len(files_to_process)} files ({len(results)} loaded from cache)...")
        max_workers = min(os.cpu_count() or 4, 32)
        with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
            new_results = list(tqdm.tqdm(executor.map(process_file, files_to_process), total=len(files_to_process)))

        for path, res in zip(files_to_process, new_results):
            results.append(res)
            resolved_str = str(path.resolve())
            mtime = path.stat().st_mtime if path.exists() else 0
            cache[resolved_str] = {
                "mtime": mtime,
                "result": res
            }

        if not args.no_cache:
            try:
                args.output_dir.mkdir(parents=True, exist_ok=True)
                with cache_path.open("wb") as f:
                    pickle.dump(cache, f)
                print(f"Saved updated cache to {cache_path}.")
            except Exception as e:
                print(f"Warning: Could not write cache file {cache_path}: {e}")
    else:
        print(f"All {len(results)} files loaded from cache.")

    run_numbers = []
    bad_towers_list = []
    dead_towers_list = []
    hot_towers_list = []

    for run_num, bad_count, dead_count, hot_count, err in results:
        if err:
            print(err)
        elif run_num is not None:
            run_numbers.append(run_num)
            bad_towers_list.append(bad_count)
            dead_towers_list.append(dead_count)
            hot_towers_list.append(hot_count)

    if not run_numbers:
        print("No valid data found to plot.")
        return

    # Sort by run number
    sorted_tuples = sorted(zip(run_numbers, bad_towers_list, dead_towers_list, hot_towers_list))
    run_numbers, bad_towers_list, dead_towers_list, hot_towers_list = zip(*sorted_tuples)

    counts_data = [
        {'Run': r, 'BadTowers': b, 'DeadTowers': d, 'HotTowers': h}
        for r, b, d, h in zip(run_numbers, bad_towers_list, dead_towers_list, hot_towers_list)
    ]

    args.output_dir.mkdir(parents=True, exist_ok=True)

    if counts_data:
        counts_df = pd.DataFrame(counts_data)
        csv_path = args.output_dir / f"{args.name}.csv"
        counts_df.to_csv(csv_path, index=False)
        print(f"Saved {len(counts_df)} runs to {csv_path}")

    # Outlier Detection for Bad Towers
    runs_arr = np.array(run_numbers)
    bad_arr = np.array(bad_towers_list)
    dead_arr = np.array(dead_towers_list)
    hot_arr = np.array(hot_towers_list)

    groups_for_outliers = [
        ("<= 69137", runs_arr <= 69137),
        ("69272-69433", (runs_arr >= 69272) & (runs_arr <= 69433)),
        ("69539-73500", (runs_arr >= 69539) & (runs_arr < 73500)),
        ("73500-78217", (runs_arr >= 73500) & (runs_arr <= 78217)),
        (">= 78218", runs_arr >= 78218),
    ]

    sigma_val = args.sigma_threshold
    sigma_str = f"{sigma_val:g}"
    sigma_tag = sigma_str.replace('.', 'p')

    outlier_rows = []
    for g_name, g_mask in groups_for_outliers:
        if not np.any(g_mask):
            continue
        g_bad = bad_arr[g_mask]
        g_runs = runs_arr[g_mask]
        g_dead = dead_arr[g_mask]
        g_hot = hot_arr[g_mask]

        g_mean = np.mean(g_bad)
        g_std = np.std(g_bad)
        thresh_sigma = g_mean + sigma_val * g_std

        outlier_mask = g_bad > thresh_sigma
        for r, b, d, h in zip(g_runs[outlier_mask], g_bad[outlier_mask], g_dead[outlier_mask], g_hot[outlier_mask]):
            sigma_dev = (b - g_mean) / g_std if g_std > 0 else 0
            outlier_rows.append({
                'Run': int(r),
                'Group': g_name,
                'BadTowers': int(b),
                'DeadTowers': int(d),
                'HotTowers': int(h),
                'GroupMean': round(g_mean, 1),
                'GroupStd': round(g_std, 1),
                f'Threshold{sigma_tag}Sigma': round(thresh_sigma, 1),
                'SigmaDeviation': round(sigma_dev, 2)
            })

    if outlier_rows:
        outliers_df = pd.DataFrame(outlier_rows)
        outliers_csv_path = args.output_dir / f"{args.name}_outliers_{sigma_tag}sigma.csv"
        outliers_df.to_csv(outliers_csv_path, index=False)
        print(f"Saved {len(outliers_df)} {sigma_str}-sigma outlier runs to {outliers_csv_path}")
    else:
        print(f"No {sigma_str}-sigma outlier runs detected.")

    # Plot Bad Towers with Sigma threshold lines
    plot_towers(
        run_numbers, bad_towers_list, args.output_dir, args.name,
        ylabel="Number of Bad Towers", suffix="", ylim_bottom=500,
        legend_fontsize=12, sigma_threshold=args.sigma_threshold
    )

    # Plot Dead Towers (value == 1)
    dead_name = "dead_towers_per_run" if args.name == "bad_towers_per_run" else f"{args.name}_dead"
    plot_towers(
        run_numbers, dead_towers_list, args.output_dir, dead_name,
        ylabel="Number of Dead Towers", suffix="", ylim_bottom=None
    )

    # Plot Hot Towers (value == 2)
    hot_name = "hot_towers_per_run" if args.name == "bad_towers_per_run" else f"{args.name}_hot"
    plot_towers(
        run_numbers, hot_towers_list, args.output_dir, hot_name,
        ylabel="Number of Hot Towers", suffix="", ylim_bottom=None, legend_loc='upper right', legend_fontsize=14
    )

if __name__ == "__main__":
    main()
