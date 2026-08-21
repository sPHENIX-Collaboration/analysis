#!/usr/bin/env python3

import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.ticker import ScalarFormatter
from mpl_toolkits.axes_grid1 import make_axes_locatable
import mplhep as hep
import os
import sys
import re
import tqdm
import argparse
import functools
import concurrent.futures
from pathlib import Path
from datetime import datetime
import numpy as np


SPHENIX_LABEL = "Internal"  # Change to "Performance", "Preliminary", etc. as needed

HIST_NAMES = [
    "h2sEPD_Centrality",
    "h2sEPD_MBD",
    "h2sEPD_CaloE"
]

CENTRALITY_INTERVALS = [
    (0, 10),
    (10, 20),
    (20, 40),
    (40, 60),
    (60, 80),
    (80, 100),
]


def get_sphenix_label(status=SPHENIX_LABEL):
    if not status:
        return r"$\boldsymbol{sPHENIX}$"
    return rf"$\boldsymbol{{sPHENIX}}$ {status}"


def clean_root_latex(text):
    if not text:
        return ""
    text = text.strip()
    text = text.replace("#", "\\")
    if "$" not in text and any(kw in text for kw in ["\\", "_{", "^{"]):
        text = re.sub(r'([a-zA-Z0-9\\_*|()]+(?:_{[^}\s]+}|^{[^}\s]+}|_[a-zA-Z0-9]+|\^[a-zA-Z0-9]+)+)', r'$\1$', text)
    return text.strip()


def get_hist_axis_titles(hist2d, hist_name=""):
    raw_title = ""
    xlabel = ""
    ylabel = ""

    if hasattr(hist2d, "all_members"):
        members = hist2d.all_members
        raw_title = members.get("fTitle", "").strip()

        fXaxis = members.get("fXaxis")
        if fXaxis is not None:
            if hasattr(fXaxis, "all_members"):
                xlabel = fXaxis.all_members.get("fTitle", "").strip()
            elif hasattr(fXaxis, "member"):
                try:
                    xlabel = fXaxis.member("fTitle").strip()
                except Exception:
                    pass

        fYaxis = members.get("fYaxis")
        if fYaxis is not None:
            if hasattr(fYaxis, "all_members"):
                ylabel = fYaxis.all_members.get("fTitle", "").strip()
            elif hasattr(fYaxis, "member"):
                try:
                    ylabel = fYaxis.member("fTitle").strip()
                except Exception:
                    pass

    if not xlabel:
        for attr in ["label", "title"]:
            try:
                val = getattr(hist2d.axis(0), attr, None)
                if val:
                    xlabel = str(val).strip()
                    break
            except Exception:
                pass

    if not ylabel:
        for attr in ["label", "title"]:
            try:
                val = getattr(hist2d.axis(1), attr, None)
                if val:
                    ylabel = str(val).strip()
                    break
            except Exception:
                pass

    if not raw_title:
        raw_title = getattr(hist2d, "title", "") or ""

    if ";" in raw_title:
        parts = [p.strip() for p in raw_title.split(";")]
        if not xlabel and len(parts) > 1:
            xlabel = parts[1]
        if not ylabel and len(parts) > 2:
            ylabel = parts[2]

    return clean_root_latex(xlabel), clean_root_latex(ylabel)


def parse_run_number(path):
    path = Path(path)
    try:
        return int(path.name.split('.')[0])
    except ValueError:
        pass

    match = re.search(r'(?:run[_-]?|part[_-]?)?(\d{5,8})', str(path))
    if match:
        return int(match.group(1))

    match = re.search(r'\d+', path.name)
    if match:
        return int(match.group())

    return None


def make_2d_plot(values, xedges, yedges, run_number, output_path, xlabel="", ylabel="", sphenix_label=SPHENIX_LABEL, date_str=None, save_pdf=False):
    if date_str is None:
        date_str = datetime.now().strftime("%m/%d/%Y")

    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    values_masked = np.ma.masked_where(values <= 0, values)

    if np.all(values <= 0):
        mesh = ax.pcolormesh(xedges, yedges, values.T, cmap='viridis', rasterized=True)
    else:
        mesh = ax.pcolormesh(xedges, yedges, values_masked.T, norm=LogNorm(), cmap='viridis', rasterized=True)

    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    cbar = fig.colorbar(mesh, cax=cax)
    cbar.set_label("Counts")

    if xlabel:
        ax.set_xlabel(xlabel, loc='center')
    if ylabel:
        ax.set_ylabel(ylabel, loc='center', labelpad=10)

    ax.set_xlim(xedges[0], xedges[-1])
    ax.set_ylim(yedges[0], yedges[-1])

    if np.max(np.abs(xedges)) >= 1000:
        formatter_x = ScalarFormatter(useMathText=True)
        formatter_x.set_powerlimits((3, 3))
        ax.xaxis.set_major_formatter(formatter_x)

    has_y_offset = np.max(np.abs(yedges)) >= 1000
    if has_y_offset:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    # Top border labels: sPHENIX on left (shifted to avoid overlap with y-axis x10^3 multiplier), Run & Date on right
    sphenix_x = 0.14 if has_y_offset else 0.0
    ax.text(sphenix_x, 1.01, get_sphenix_label(sphenix_label), transform=ax.transAxes, ha='left', va='bottom', fontsize=16)

    right_text = f"Run: {run_number}, {date_str}" if run_number is not None else date_str
    ax.text(1.0, 1.01, right_text, transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=300)
    if save_pdf:
        fig.savefig(output_path.with_suffix('.pdf'))
    plt.close(fig)


def make_centrality_slices_plot(values, xedges, yedges, run_number, output_path, xlabel="sEPD Total Charge", sphenix_label=SPHENIX_LABEL, date_str=None, save_pdf=False):
    if date_str is None:
        date_str = datetime.now().strftime("%m/%d/%Y")

    hep.style.use("ATLAS")
    fig, axes = plt.subplots(2, 3, figsize=(15, 8), sharex=True, sharey='row', gridspec_kw={'hspace': 0, 'wspace': 0})

    bin_centers_x = (xedges[:-1] + xedges[1:]) / 2.0

    projections = []
    for cent_min, cent_max in CENTRALITY_INTERVALS:
        mask = (bin_centers_x >= cent_min) & (bin_centers_x < cent_max)
        if np.any(mask):
            proj_1d = np.sum(values[mask, :], axis=0)
        else:
            proj_1d = np.zeros(len(yedges) - 1)
        projections.append(proj_1d)

    # Calculate y-limits separately for top row (indices 0, 1, 2) and bottom row (indices 3, 4, 5)
    max_y_top = max(np.max(projections[i]) for i in range(3)) if len(projections) >= 3 else 1
    max_y_bottom = max(np.max(projections[i]) for i in range(3, 6)) if len(projections) >= 6 else 1

    # Auto-adjust x-max based on the highest non-zero bin edge across all 6 histograms
    max_x_val = 0
    for proj in projections:
        nonzero_indices = np.where(proj > 0)[0]
        if len(nonzero_indices) > 0:
            max_bin_edge = yedges[nonzero_indices[-1] + 1]
            if max_bin_edge > max_x_val:
                max_x_val = max_bin_edge

    xmax = min(max_x_val * 1.05, yedges[-1]) if max_x_val > 0 else yedges[-1]

    for r in range(2):
        for c in range(3):
            idx = r * 3 + c
            ax = axes[r, c]
            proj_1d = projections[idx]
            cent_min, cent_max = CENTRALITY_INTERVALS[idx]

            hep.histplot((proj_1d, yedges), ax=ax, histtype='step', color='navy', linewidth=2)

            ax.set_xlim(left=yedges[0], right=xmax)
            if r == 0:
                ax.set_ylim(bottom=0, top=max_y_top * 1.2 if max_y_top > 0 else 1)
            else:
                ax.set_ylim(bottom=0, top=max_y_bottom * 1.2 if max_y_bottom > 0 else 1)

            # X-axis scalar formatter on bottom row
            if r == 1 and xmax >= 1000:
                formatter_x = ScalarFormatter(useMathText=True)
                formatter_x.set_powerlimits((3, 3))
                ax.xaxis.set_major_formatter(formatter_x)

            # Y-axis scalar formatter on leftmost column
            max_y_r = max_y_top if r == 0 else max_y_bottom
            if c == 0 and max_y_r >= 1000:
                formatter_y = ScalarFormatter(useMathText=True)
                formatter_y.set_powerlimits((3, 3))
                ax.yaxis.set_major_formatter(formatter_y)

            # Centrality range label on top-right of each subplot
            cent_label = f"Centrality: {cent_min}–{cent_max}%"
            ax.text(0.92, 0.90, cent_label, transform=ax.transAxes, ha='right', va='top', fontsize=18)

            # Axis labels for outer edges (centered)
            if r == 1:
                ax.set_xlabel(xlabel if xlabel else "sEPD Total Charge", loc='center', fontsize=18)
            if c == 0:
                ax.set_ylabel("Counts", loc='center', fontsize=18)

    # Top border labels on 2x3 figure: sPHENIX on left (shifted if top row has y-axis multiplier), Run & Date on right
    has_top_y_offset = max_y_top >= 1000
    sphenix_slices_x = 0.14 if has_top_y_offset else 0.0
    axes[0, 0].text(sphenix_slices_x, 1.02, get_sphenix_label(sphenix_label), transform=axes[0, 0].transAxes, ha='left', va='bottom', fontsize=16)
    right_text = f"Run: {run_number}, {date_str}" if run_number is not None else date_str
    axes[0, 2].text(1.0, 1.02, right_text, transform=axes[0, 2].transAxes, ha='right', va='bottom', fontsize=15)

    plt.subplots_adjust(left=0.08, right=0.97, top=0.94, bottom=0.10, hspace=0, wspace=0)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=300)
    if save_pdf:
        fig.savefig(output_path.with_suffix('.pdf'))
    plt.close(fig)


def make_1d_plot(values, bin_edges, run_number, output_path, xlabel="", ylabel="Events", sphenix_label=SPHENIX_LABEL, date_str=None, save_pdf=False):
    if date_str is None:
        date_str = datetime.now().strftime("%m/%d/%Y")

    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    hep.histplot((values, bin_edges), ax=ax, histtype='step', color='navy', linewidth=2)

    ax.set_xlim(bin_edges[0], bin_edges[-1])
    max_y = np.max(values) if len(values) > 0 else 0
    ax.set_ylim(bottom=0, top=max_y * 1.15 if max_y > 0 else 1)

    if xlabel:
        ax.set_xlabel(xlabel, loc='center')
    if ylabel:
        ax.set_ylabel(ylabel, loc='center', labelpad=10)

    if np.max(np.abs(bin_edges)) >= 1000:
        formatter_x = ScalarFormatter(useMathText=True)
        formatter_x.set_powerlimits((3, 3))
        ax.xaxis.set_major_formatter(formatter_x)

    has_y_offset = max_y >= 1000
    if has_y_offset:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    sphenix_x = 0.14 if has_y_offset else 0.0
    ax.text(sphenix_x, 1.01, get_sphenix_label(sphenix_label), transform=ax.transAxes, ha='left', va='bottom', fontsize=16)

    right_text = f"Run: {run_number}, {date_str}" if run_number is not None else date_str
    ax.text(1.0, 1.01, right_text, transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=300)
    if save_pdf:
        fig.savefig(output_path.with_suffix('.pdf'))
    plt.close(fig)


def process_file(path, output_dir, runs_to_plot=None, sphenix_label=SPHENIX_LABEL, date_str=None, save_pdf=False):
    path = Path(path)
    if not path.exists():
        return f"File not found: {path}"

    run_number = parse_run_number(path)
    if runs_to_plot is not None and run_number is not None and run_number not in runs_to_plot:
        return None

    try:
        with uproot.open(path) as file:
            plotted_any = False
            for name in HIST_NAMES:
                if name in file:
                    obj = file[name]
                    values, xedges, yedges = obj.to_numpy()
                    xlabel, ylabel = get_hist_axis_titles(obj, name)

                    prefix = f"run_{run_number}_" if run_number is not None else f"{path.stem}_"
                    out_path = output_dir / f"{prefix}{name}.png"
                    make_2d_plot(values, xedges, yedges, run_number, out_path, xlabel=xlabel, ylabel=ylabel, sphenix_label=sphenix_label, date_str=date_str, save_pdf=save_pdf)

                    if name == "h2sEPD_Centrality":
                        slices_out_path = output_dir / f"{prefix}{name}_slices.png"
                        make_centrality_slices_plot(values, xedges, yedges, run_number, slices_out_path, xlabel=ylabel, sphenix_label=sphenix_label, date_str=date_str, save_pdf=save_pdf)

                        cent_1d_out_path = output_dir / f"{prefix}{name}_1D.png"
                        proj_cent = np.sum(values, axis=1)
                        make_1d_plot(proj_cent, xedges, run_number, cent_1d_out_path, xlabel=xlabel if xlabel else "Centrality [%]", ylabel="Events", sphenix_label=sphenix_label, date_str=date_str, save_pdf=save_pdf)

                    plotted_any = True

            if not plotted_any:
                return f"None of target histograms found in {path.name}"

        return None
    except Exception as e:
        return f"Error processing {path}: {e}"


def main():
    parser = argparse.ArgumentParser(description="Plot sEPD QA histograms from ROOT files.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text/list file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("plots/sepd_qa"), help="Directory to save the plots (default: plots/sepd_qa).")
    parser.add_argument("--label", "--sphenix-label", type=str, default=SPHENIX_LABEL, help=f"sPHENIX label status (e.g. Internal, Performance, Preliminary). Default: {SPHENIX_LABEL}")
    parser.add_argument("--date", type=str, default=datetime.now().strftime("%m/%d/%Y"), help=f"Date string in mm/dd/yyyy format. Default: today's date ({datetime.now().strftime('%m/%d/%Y')})")
    parser.add_argument("--save-pdf", action="store_true", help="Enable saving of plots in PDF format (in addition to PNG).")
    parser.add_argument("--runs", type=int, nargs="+", help="Specific run number(s) to plot.")
    parser.add_argument("-j", "--max-workers", type=int, default=None, help="Number of parallel workers (default: automatic).")
    parser.add_argument("files", nargs="*", type=Path, help="List of ROOT file paths")

    args = parser.parse_args()

    file_list = []
    if args.files:
        file_list.extend(args.files)

    if args.file:
        if not args.file.exists():
            print(f"Error: File list {args.file} does not exist.")
            sys.exit(1)
        with args.file.open('r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    file_list.append(Path(line))

    if not file_list:
        print("Error: You must provide at least one ROOT file or a text file with -f/--file.")
        parser.print_help()
        sys.exit(1)

    args.output_dir.mkdir(parents=True, exist_ok=True)
    runs_to_filter = set(args.runs) if args.runs else None

    print(f"Found {len(file_list)} input file(s). Starting plotting...")

    max_workers = args.max_workers or min(os.cpu_count() or 4, 32)
    process_func = functools.partial(
        process_file,
        output_dir=args.output_dir,
        runs_to_plot=runs_to_filter,
        sphenix_label=args.label,
        date_str=args.date,
        save_pdf=args.save_pdf
    )

    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        errors = list(tqdm.tqdm(executor.map(process_func, file_list), total=len(file_list)))

    for err in errors:
        if err:
            print(f"Warning: {err}")

    print(f"Plots saved to: {args.output_dir}")
    print("Done!")


if __name__ == "__main__":
    main()
