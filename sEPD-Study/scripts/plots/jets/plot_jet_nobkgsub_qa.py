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
import functools
import sys

from matplotlib.ticker import LogLocator

def make_combined_plot(centers_y, projections, run_number, output_path, r_jet=0.2, xmax=70.0):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    for proj, errors, label, color, marker, fillstyle in projections:
        mask_nonzero = proj > 0
        ax.errorbar(centers_y[mask_nonzero], proj[mask_nonzero],
                    yerr=errors[mask_nonzero], fmt=marker, color=color,
                    fillstyle=fillstyle, markersize=4, linestyle='none', label=label)

    ax.set_yscale('log')
    ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
    ax.set_xlabel(r"$p_{T}$ (GeV)")
    ax.set_ylabel("Counts")
    if xmax is not None:
        ax.set_xlim(left=0, right=xmax)
    else:
        ax.set_xlim(left=0)

    # Top right border R label
    ax.text(1.0, 1.01, rf"$R = {r_jet:g}$", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    text_info = (
        f"Run: {run_number}\n"
        r"$|z| < 10$ cm & MB" "\n"
        f"Good Calo-Cent"
    )
    ax.text(0.95, 0.95, text_info, transform=ax.transAxes, ha='right', va='top', fontsize=15)

    jet_selection_text = (
        r"Jet Selection:" + "\n"
        r"Energy > 0" + "\n"
        r"$|\eta| < 1.1 - R$"
    )
    ax.text(0.95, 0.5, jet_selection_text, transform=ax.transAxes, ha='right', va='top', fontsize=15)

    ax.legend(loc='upper right', bbox_to_anchor=(0.99, 0.78), frameon=False, fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.95)

    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def process_file(path, output_dir=None, r_jet=0.2, xmax=70.0):
    path = Path(path)
    if not path.exists():
        return f"File not found: {path}"

    try:
        try:
            run_number = int(path.name.split('.')[0])
        except ValueError:
            match = re.search(r'\d+', path.name)
            if match:
                run_number = int(match.group())
            else:
                return f"Could not parse run number from {path.name}"

        with uproot.open(path) as file:
            if "h2CentralityJetPt" not in file:
                return f"h2CentralityJetPt not found in {path}"

            hist2d = file["h2CentralityJetPt"]
            values, edges_x, edges_y = hist2d.to_numpy()
            errors = hist2d.errors()

            # Assuming X is Centrality and Y is Jet Pt
            centers_y = (edges_y[:-1] + edges_y[1:]) / 2.0
            centers_x = (edges_x[:-1] + edges_x[1:]) / 2.0

            # 1. Projection using all centrality bins
            proj_all = np.sum(values, axis=0)
            errors_all = np.sqrt(np.sum(errors**2, axis=0))

            # 2. Projection using centrality >= 60%
            mask_60 = centers_x >= 60.0
            proj_60 = np.sum(values[mask_60, :], axis=0)
            errors_60 = np.sqrt(np.sum(errors[mask_60, :]**2, axis=0))

            # 3. Projection using centrality 0-60%
            mask_0_60 = (centers_x >= 0.0) & (centers_x <= 60.0)
            proj_0_60 = np.sum(values[mask_0_60, :], axis=0)
            errors_0_60 = np.sqrt(np.sum(errors[mask_0_60, :]**2, axis=0))

            if output_dir is not None:
                projections = [
                    (proj_all, errors_all, "Cent: 0-100%", "black", "o", "full"),
                    (proj_0_60, errors_0_60, "Cent: 0-60%", "crimson", "o", "none"),
                    (proj_60, errors_60, r"Cent: $\geq$ 60%", "blue", "o", "full")
                ]
                output_path = output_dir / f"run_{run_number}_jetpt_projections.png"
                make_combined_plot(centers_y, projections, run_number, output_path, r_jet=r_jet, xmax=xmax)

            return None
    except Exception as e:
        return f"Error processing {path}: {e}"

def main():
    parser = argparse.ArgumentParser(description="Plot Jet Pt projections from TH2 (h2CentralityJetPt) per run.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("-r", "--r-jet", "--r", dest="r_jet", type=float, default=0.2, help="Jet resolution parameter R (default: 0.2).")
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

    args.output_dir.mkdir(parents=True, exist_ok=True)
    plot_run_dir = args.output_dir / "run_qa_plots_nobkgsub"
    plot_run_dir.mkdir(parents=True, exist_ok=True)

    print(f"Found {len(file_list)} input files. Starting processing...")

    files_to_process = [Path(p) for p in file_list]
    process_func = functools.partial(process_file, output_dir=plot_run_dir, r_jet=args.r_jet, xmax=70.0)
    max_workers = min(os.cpu_count() or 4, 32)

    errors = []
    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        results = list(tqdm.tqdm(executor.map(process_func, files_to_process), total=len(files_to_process)))

    for path, err in zip(files_to_process, results):
        if err:
            print(err)
            errors.append(err)

    if not errors:
        print(f"Successfully processed all {len(file_list)} files.")
        print(f"Plots saved to {plot_run_dir}")
    else:
        print(f"Processed with {len(errors)} errors.")
        print(f"Plots saved to {plot_run_dir}")

if __name__ == "__main__":
    main()
