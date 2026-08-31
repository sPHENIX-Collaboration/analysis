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
import traceback

from matplotlib.ticker import LogLocator, ScalarFormatter
from matplotlib.colors import LogNorm
from matplotlib.patches import Patch
from mpl_toolkits.axes_grid1 import make_axes_locatable

def clean_root_latex(text):
    if not text:
        return ""
    text = text.strip()
    # Replace ROOT TLatex # with \ for LaTeX math
    text = text.replace("#", "\\")
    if "$" not in text:
        # Wrap LaTeX command terms (like \chi, \eta, \Psi) or terms containing subscripts/superscripts in math mode $...$
        text = re.sub(
            r'(\\[a-zA-Z]+(?:\^\{[^}\s]+\}|_\{[^}\s]+\}|\^[a-zA-Z0-9]+|_[a-zA-Z0-9]+)*|[a-zA-Z0-9\\_*|()]*(?:\^\{[^}\s]+\}|_\{[^}\s]+\}|\^[a-zA-Z0-9]+|_[a-zA-Z0-9]+)+|\\[a-zA-Z]+)',
            r'$\1$',
            text
        )
    return text

def get_hist_axis_titles(hist2d, hist_name=""):
    raw_title = ""
    xlabel = ""
    ylabel = ""

    # 1. Try uproot all_members dict
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

    # 2. Try uproot axis high-level properties
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

    # 3. Check if raw_title or hist2d.title is formatted as "Title;XTitle;YTitle"
    if not raw_title:
        raw_title = getattr(hist2d, "title", "") or ""

    if ";" in raw_title:
        parts = [p.strip() for p in raw_title.split(";")]
        if not xlabel and len(parts) > 1:
            xlabel = parts[1]
        if not ylabel and len(parts) > 2:
            ylabel = parts[2]

    xlabel = clean_root_latex(xlabel)
    ylabel = clean_root_latex(ylabel)

    return xlabel, ylabel

def make_2d_plot(hist2d, run_number, output_path, hist_name="", xlim_left=None, xlim_right=None, ylim_bottom=None, ylim_top=None, extra_label=None, logx=False, logy=False):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    values, xedges, yedges = hist2d.to_numpy()

    xlabel, ylabel = get_hist_axis_titles(hist2d, hist_name)
    if not xlabel and hist_name == "h2EMCalChi2Energy":
        xlabel = r"Tower Energy [ADC]"
    if not ylabel and hist_name == "h2EMCalChi2Energy":
        ylabel = r"$\chi^{2}$"

    max_val = np.max(values) if values.size > 0 else 0
    if max_val <= 0:
        mesh = ax.pcolormesh(xedges, yedges, values.T, cmap='viridis', rasterized=True)
    else:
        values_masked = np.ma.masked_where(values <= 0, values)
        norm = LogNorm(vmin=1, vmax=max(max_val, 10))
        mesh = ax.pcolormesh(xedges, yedges, values_masked.T, norm=norm, cmap='viridis', rasterized=True)

    divider = make_axes_locatable(ax)
    cax = divider.append_axes("right", size="5%", pad=0.05)
    cbar = fig.colorbar(mesh, cax=cax)
    cbar.set_label("Counts")

    if xlabel:
        ax.set_xlabel(xlabel, loc='center')
    if ylabel:
        ax.set_ylabel(ylabel, labelpad=2)

    if logx:
        ax.set_xscale('log')
        ax.xaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
        if xlim_left is None or xlim_left <= 0:
            xlim_left = 1.0
        if xlim_right is None:
            xlim_right = np.max(xedges)

    if logy:
        ax.set_yscale('log')
        ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
        if ylim_bottom is None or ylim_bottom <= 0:
            ylim_bottom = 1.0
        if ylim_top is None:
            ylim_top = np.max(yedges)

    if xlim_left is not None or xlim_right is not None:
        ax.set_xlim(left=xlim_left, right=xlim_right)
    if ylim_bottom is not None or ylim_top is not None:
        ax.set_ylim(bottom=ylim_bottom, top=ylim_top)

    if hist_name == "h2EMCalChi2Energy":
        x_min = max(xlim_left, 1.0) if (logx and xlim_left is not None) else (xlim_left if xlim_left is not None else np.min(xedges))
        x_max = xlim_right if xlim_right is not None else np.max(xedges)
        y_max = ylim_top if ylim_top is not None else np.max(yedges)

        x_vals = np.geomspace(x_min, x_max, 1000) if logx else np.linspace(x_min, x_max, 1000)
        badChi2_threshold_const = 1e4
        badChi2_threshold_quadratic = 0.01
        badChi2_threshold_max = 1e8
        y_cut = np.minimum(np.maximum(badChi2_threshold_const, (x_vals ** 2) * badChi2_threshold_quadratic), badChi2_threshold_max)

        ax.fill_between(x_vals, y_cut, y_max, where=(y_max >= y_cut), color='red', alpha=0.3, zorder=3)
        ax.plot(x_vals, y_cut, color='red', linestyle='--', linewidth=1.5, zorder=4)

        # Compute bad chi2 tower count and fraction over total
        x_centers = (xedges[:-1] + xedges[1:]) / 2.0
        y_centers = (yedges[:-1] + yedges[1:]) / 2.0
        X_grid, Y_grid = np.meshgrid(x_centers, y_centers, indexing='ij')
        threshold_grid = np.minimum(np.maximum(badChi2_threshold_const, (X_grid ** 2) * badChi2_threshold_quadratic), badChi2_threshold_max)
        bad_mask = Y_grid > threshold_grid
        bad_towers = np.sum(values[bad_mask])
        total_towers = np.sum(values)
        pct = (bad_towers / total_towers) * 100.0 if total_towers > 0 else 0.0

        patch = Patch(facecolor=(1, 0, 0, 0.3), edgecolor='red', linewidth=1.5, linestyle='--',
                      label=r"$\chi^{2} > \min(\max(10^{4}, 0.01 \cdot \mathrm{ADC}^{2}), 10^{8})$")
        legend_loc = 'upper left' if (logx and logy) else 'upper right'
        ax.legend(handles=[patch], loc=legend_loc, frameon=True, facecolor='white', edgecolor='none', framealpha=0.8, fontsize=12)

        if not logx:
            info_text = (
                f"Total Towers: {total_towers:.2e}\n"
                f"Bad $\\chi^{{2}}$ Towers: {bad_towers:.2e}\n"
                f"Bad $\\chi^{{2}}$ Percentage: {pct:.2f}%"
            )
            ax.text(0.96, 0.78, info_text, transform=ax.transAxes, ha='right', va='top', fontsize=13,
                    bbox=dict(boxstyle='round,pad=0.3', facecolor='white', alpha=0.8, edgecolor='none'))

    if not logx and np.max(xedges) >= 1000:
        formatter_x = ScalarFormatter(useMathText=True)
        formatter_x.set_powerlimits((3, 3))
        ax.xaxis.set_major_formatter(formatter_x)

    if not logy and np.max(yedges) >= 1000:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    if extra_label:
        ax.text(0.95, 0.95, extra_label, transform=ax.transAxes, ha='right', va='top', fontsize=15, color='white', bbox=dict(boxstyle='round,pad=0.2', facecolor='black', alpha=0.4, edgecolor='none'))

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def process_file(path, output_dir=None, do_nolog=True, do_logy=True, do_logxy=True, do_logx=False):
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

        run_output_dir = None
        if output_dir is not None:
            run_output_dir = output_dir / f"{run_number}"
            run_output_dir.mkdir(parents=True, exist_ok=True)

        with uproot.open(path) as file:
            h2_names = [
                "h2EMCalChi2Energy",
            ]

            plot_modes = []
            if do_nolog:
                plot_modes.append(("", False, False))
            if do_logy:
                plot_modes.append(("_logy", False, True))
            if do_logxy:
                plot_modes.append(("_logxy", True, True))
            if do_logx:
                plot_modes.append(("_logx", True, False))

            for h2_name in h2_names:
                if h2_name not in file:
                    print(f"Warning: {h2_name} not found in {path}")
                    continue

                hist2d = file[h2_name]

                if run_output_dir is not None:
                    for suffix, lx, ly in plot_modes:
                        out_filename = f"run_{run_number}_{h2_name}{suffix}.png"
                        output_path = run_output_dir / out_filename
                        xlim_left = 1.0 if lx else 0
                        ylim_bottom = 1.0 if ly else 0
                        make_2d_plot(
                            hist2d,
                            run_number,
                            output_path,
                            hist_name=h2_name,
                            xlim_left=xlim_left,
                            ylim_bottom=ylim_bottom,
                            logx=lx,
                            logy=ly
                        )

            return None
    except Exception as e:
        traceback.print_exc()
        return f"Error processing {path}: {e}"

def main():
    parser = argparse.ArgumentParser(description="Plot Calorimeter QA for sPHENIX.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("--do-nolog", type=int, default=1, help="Generate linear/no-log scale plots (1=True, 0=False). Default: 1")
    parser.add_argument("--do-logy", type=int, default=1, help="Generate log-y scale plots (1=True, 0=False). Default: 1")
    parser.add_argument("--do-logxy", type=int, default=1, help="Generate log-xy scale plots (1=True, 0=False). Default: 1")
    parser.add_argument("--do-logx", type=int, default=0, help="Generate log-x scale plots (1=True, 0=False). Default: 0")
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

    print(f"Found {len(file_list)} input files. Starting processing...")

    files_to_process = [Path(p) for p in file_list]
    process_func = functools.partial(
        process_file,
        output_dir=args.output_dir,
        do_nolog=bool(args.do_nolog),
        do_logy=bool(args.do_logy),
        do_logxy=bool(args.do_logxy),
        do_logx=bool(args.do_logx),
    )
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
        print(f"Plots saved to {args.output_dir}")
    else:
        print(f"Processed with {len(errors)} errors.")
        print(f"Plots saved to {args.output_dir}")

if __name__ == "__main__":
    main()
