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
import functools
import pickle
import sys

def process_file(path, pt_cut=30.0, neg_pt_cut=0.0):
    path = Path(path)
    if not path.exists():
        return None, None, None, None, None, f"File not found: {path}"

    try:
        try:
            run_number = int(path.name.split('.')[0])
        except ValueError:
            match = re.search(r'\d+', path.name)
            if match:
                run_number = int(match.group())
            else:
                return None, None, None, None, None, f"Could not parse run number from {path.name}"

        with uproot.open(path) as file:
            if "hJetPt" not in file:
                return None, None, None, None, None, f"hJetPt not found in {path}"
            if "hEvent" not in file:
                return None, None, None, None, None, f"hEvent not found in {path}"

            hist_jet = file["hJetPt"]
            values_jet, edges_jet = hist_jet.to_numpy()

            # Integrated counts above pt_cut GeV (assuming bin edges are in GeV)
            mask = edges_jet[:-1] >= pt_cut
            counts_above_cut = np.sum(values_jet[mask])

            neg_jets_counts = None
            if "hJetPtv2" in file:
                hist_jet_v2 = file["hJetPtv2"]
                values_jet_v2 = hist_jet_v2.values()
                mask_neg = edges_jet[:-1] >= neg_pt_cut
                neg_jets_counts = np.sum(values_jet[mask_neg]) - np.sum(values_jet_v2[mask_neg])

            v3_jets_counts = None
            if "hJetPtv3" in file:
                hist_jet_v3 = file["hJetPtv3"]
                values_jet_v3, edges_jet_v3 = hist_jet_v3.to_numpy()
                mask_v3 = edges_jet_v3[:-1] >= pt_cut
                v3_jets_counts = np.sum(values_jet_v3[mask_v3])

            hist_event = file["hEvent"]
            values_event = hist_event.values()
            if len(values_event) == 0 or values_event[0] <= 0:
                return None, None, None, None, None, f"Invalid event count in {path}"

            n_events = values_event[0]

            return run_number, counts_above_cut, n_events, neg_jets_counts, v3_jets_counts, None
    except Exception as e:
        return None, None, None, None, None, f"Error processing {path}: {e}"

def plot_normalized_counts(run_numbers, normalized_counts, output_dir, name, ylabel=r"Raw Counts ($p_{T} > 30$ GeV) / Event", suffix="", extra_text=None):
    hep.style.use("ATLAS")

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(run_numbers, normalized_counts, marker='o', markersize=4, linestyle='none', color='black', label='Data')

    # Add a light dashed line for the average counts
    avg_counts = sum(normalized_counts) / len(normalized_counts)
    ax.axhline(avg_counts, color='gray', linestyle='--', alpha=0.5, label='Average')
    ax.legend(loc='upper right', frameon=False)

    ax.set_xlabel("Run Number", labelpad=18)
    ax.set_ylabel(ylabel)
    ax.set_title("Jets")
    ax.set_ylim(bottom=0)

    # Calculate and display the number of runs and event selections
    total_runs = len(run_numbers)
    text_info = (
        f"Runs = {total_runs}\n"
        r"$|z| < 10$ cm & MB" + "\n"
        r"Centrality: 0-60%"
    )
    if extra_text:
        if isinstance(extra_text, (list, tuple)):
            text_info += "\n" + "\n".join(extra_text)
        else:
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
    print(f"Saved normalized plots as {pdf_path} and {png_path}")

def plot_raw_counts(run_numbers, raw_counts, output_dir, name, ylabel=r"Raw Counts ($p_{T} > 30$ GeV)", suffix="-raw", extra_text=None):
    hep.style.use("ATLAS")

    fig, ax = plt.subplots(figsize=(10, 6))
    ax.plot(run_numbers, raw_counts, marker='o', markersize=4, linestyle='none', color='black', label='Data')

    avg_raw_counts = sum(raw_counts) / len(raw_counts)
    ax.axhline(avg_raw_counts, color='gray', linestyle='--', alpha=0.5, label='Average')
    ax.legend(loc='upper right', frameon=False)

    ax.set_xlabel("Run Number", labelpad=18)
    ax.set_ylabel(ylabel)
    ax.set_title("Jets")
    ax.set_ylim(bottom=0)

    total_runs = len(run_numbers)
    text_info = (
        f"Runs = {total_runs}\n"
        r"$|z| < 10$ cm & MB" + "\n"
        r"Centrality: 0-60%"
    )
    if extra_text:
        if isinstance(extra_text, (list, tuple)):
            text_info += "\n" + "\n".join(extra_text)
        else:
            text_info += f"\n{extra_text}"
    ax.text(0.05, 0.95, text_info, transform=ax.transAxes, ha='left', va='top', fontsize=18)

    plt.tight_layout()
    plt.subplots_adjust(top=0.94, bottom=0.14)

    pdf_dir = output_dir / "pdf"
    image_dir = output_dir / "images"
    pdf_dir.mkdir(parents=True, exist_ok=True)
    image_dir.mkdir(parents=True, exist_ok=True)

    raw_pdf_path = pdf_dir / f"{name}{suffix}.pdf"
    raw_png_path = image_dir / f"{name}{suffix}.png"
    plt.savefig(raw_pdf_path)
    plt.savefig(raw_png_path, dpi=300)
    plt.close(fig)
    print(f"Saved raw plots as {raw_pdf_path} and {raw_png_path}")

def main():
    parser = argparse.ArgumentParser(description="Plot integrated jet counts above pT cut per run from a list of ROOT files.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("-n", "--name", "--plot-name", dest="name", type=str, default="jet_qa_counts_per_run", help="Base filename for output plots (default: jet_qa_counts_per_run).")
    parser.add_argument("-p", "--pt-cut", type=float, default=30.0, help="pT threshold in GeV for integrated jet counts (default: 30.0).")
    parser.add_argument("-np", "--neg-pt-cut", type=float, default=0.0, help="pT threshold in GeV for negative energy jet counts (default: 0.0).")
    parser.add_argument("--cache-file", type=Path, default=None, help="Path to cache file for processed ROOT file data (default: <output_dir>/.jet_qa_cache_pt<pt>_negpt<neg_pt>.pkl).")
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

    run_numbers = []
    normalized_counts = []
    events_list = []
    raw_counts = []
    neg_raw_counts = []
    neg_normalized_counts = []
    v3_raw_counts = []
    v3_normalized_counts = []

    print(f"Found {len(file_list)} input files.")

    cache = {}
    pt_tag = f"{args.pt_cut:g}".replace('.', 'p')
    neg_pt_tag = f"{args.neg_pt_cut:g}".replace('.', 'p')
    default_cache_name = f".jet_qa_cache_pt{pt_tag}_negpt{neg_pt_tag}.pkl"
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
            if (entry.get("mtime") == mtime and
                entry.get("pt_cut") == args.pt_cut and
                entry.get("neg_pt_cut") == args.neg_pt_cut):
                results.append(entry["result"])
                continue

        files_to_process.append(path)

    if files_to_process:
        print(f"Processing {len(files_to_process)} files ({len(results)} loaded from cache)...")
        process_func = functools.partial(process_file, pt_cut=args.pt_cut, neg_pt_cut=args.neg_pt_cut)
        max_workers = min(os.cpu_count() or 4, 32)
        with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
            new_results = list(tqdm.tqdm(executor.map(process_func, files_to_process), total=len(files_to_process)))

        for path, res in zip(files_to_process, new_results):
            results.append(res)
            resolved_str = str(path.resolve())
            mtime = path.stat().st_mtime if path.exists() else 0
            cache[resolved_str] = {
                "mtime": mtime,
                "pt_cut": args.pt_cut,
                "neg_pt_cut": args.neg_pt_cut,
                "result": res
            }

        if not args.no_cache:
            try:
                with cache_path.open("wb") as f:
                    pickle.dump(cache, f)
                print(f"Saved updated cache to {cache_path}.")
            except Exception as e:
                print(f"Warning: Could not write cache file {cache_path}: {e}")
    else:
        print(f"All {len(results)} files loaded from cache.")

    for run_num, counts, n_events, neg_jets, v3_jets, err in results:
        if err:
            print(err)
        elif run_num is not None:
            norm = counts / n_events
            run_numbers.append(run_num)
            normalized_counts.append(norm)
            events_list.append(n_events)
            raw_counts.append(counts)
            if neg_jets is not None:
                neg_raw_counts.append(neg_jets)
                neg_normalized_counts.append(neg_jets / n_events)
            else:
                neg_raw_counts.append(0)
                neg_normalized_counts.append(0)
            if v3_jets is not None:
                v3_raw_counts.append(v3_jets)
                v3_normalized_counts.append(v3_jets / n_events)
            else:
                v3_raw_counts.append(0)
                v3_normalized_counts.append(0)

    if not run_numbers:
        print("No valid data found to plot.")
        return

    # Sort by run number to ensure the plot is ordered
    sorted_pairs = sorted(zip(run_numbers, normalized_counts, events_list, raw_counts, neg_raw_counts, neg_normalized_counts, v3_raw_counts, v3_normalized_counts))
    run_numbers, normalized_counts, events_list, raw_counts, neg_raw_counts, neg_normalized_counts, v3_raw_counts, v3_normalized_counts = zip(*sorted_pairs)

    # Save all processed runs to CSV sorted by V3RawCounts descending
    counts_data = [
        {
            'Run': r, 'Events': ev, 'RawCounts': raw, 'NormalizedCounts': c,
            'NegRawCounts': neg_raw, 'NegNormalizedCounts': neg_norm,
            'V3RawCounts': v3_raw, 'V3NormalizedCounts': v3_norm
        }
        for r, c, ev, raw, neg_raw, neg_norm, v3_raw, v3_norm in zip(
            run_numbers, normalized_counts, events_list, raw_counts, neg_raw_counts, neg_normalized_counts, v3_raw_counts, v3_normalized_counts
        )
    ]

    args.output_dir.mkdir(parents=True, exist_ok=True)

    if counts_data:
        counts_df = pd.DataFrame(counts_data)
        counts_df = counts_df.sort_values(by='V3RawCounts', ascending=False)
        csv_path = args.output_dir / f"{args.name}.csv"
        counts_df.to_csv(csv_path, index=False)
        print(f"Saved {len(counts_df)} runs to {csv_path}")
    else:
        print("No valid runs found to write to CSV.")

    pt_str = f"{args.pt_cut:g}"
    plot_normalized_counts(
        run_numbers, normalized_counts, args.output_dir, args.name,
        ylabel=rf"Raw Counts ($p_{{T}} > {pt_str}$ GeV) / Event"
    )
    plot_raw_counts(
        run_numbers, raw_counts, args.output_dir, args.name,
        ylabel=rf"Raw Counts ($p_{{T}} > {pt_str}$ GeV)"
    )

    v3_extra_text = ["Jet Energy > 0 GeV", r"$|\mathrm{calo}\ v_{2}| < 0.48$"]
    plot_normalized_counts(
        run_numbers, v3_normalized_counts, args.output_dir, args.name,
        ylabel=rf"Raw Counts ($p_{{T}} > {pt_str}$ GeV) / Event", suffix="-v3-filters",
        extra_text=v3_extra_text
    )
    plot_raw_counts(
        run_numbers, v3_raw_counts, args.output_dir, args.name,
        ylabel=rf"Raw Counts ($p_{{T}} > {pt_str}$ GeV)", suffix="-v3-filters-raw",
        extra_text=v3_extra_text
    )

    neg_pt_str = f"{args.neg_pt_cut:g}"
    plot_normalized_counts(
        run_numbers, neg_normalized_counts, args.output_dir, args.name,
        ylabel=rf"Negative Energy Jets ($p_{{T}} > {neg_pt_str}$ GeV) / Event", suffix="-neg"
    )
    plot_raw_counts(
        run_numbers, neg_raw_counts, args.output_dir, args.name,
        ylabel=rf"Negative Energy Jets ($p_{{T}} > {neg_pt_str}$ GeV)", suffix="-neg-raw"
    )

if __name__ == "__main__":
    main()
