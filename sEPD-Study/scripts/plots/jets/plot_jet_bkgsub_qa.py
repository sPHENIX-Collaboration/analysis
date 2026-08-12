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

from matplotlib.ticker import LogLocator, ScalarFormatter
from matplotlib.colors import LogNorm
from mpl_toolkits.axes_grid1 import make_axes_locatable

def make_combined_plot(edges_x, hists, run_number, output_path, xmax=None, ymax=1e8, extra_energy_cut=False, is_v3=False, r_jet=0.2):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    for values, label, color in hists:
        hep.histplot((values, edges_x), ax=ax, histtype='step', color=color, linewidth=2, label=label)

    max_x_val = 0
    for values, _, _ in hists:
        nonzero_indices = np.where(values > 0)[0]
        if len(nonzero_indices) > 0:
            max_bin_edge = edges_x[nonzero_indices[-1] + 1]
            if max_bin_edge > max_x_val:
                max_x_val = max_bin_edge

    ax.set_yscale('log')
    ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
    ax.set_xlabel(r"$p_{T}$ (GeV)")
    ax.set_ylabel("Counts")
    if ymax is not None:
        ax.set_ylim(top=ymax)
    if xmax is not None:
        ax.set_xlim(left=0, right=xmax)
    elif max_x_val > 0:
        ax.set_xlim(left=0, right=max_x_val)
    else:
        ax.set_xlim(left=0)

    # Top right border label
    ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    text_info = (
        r"$|z| < 10$ cm & MB" "\n"
        f"Good Calo-Cent" "\n"
        f"Centrality: 0-60%" "\n"
        f"No Flow Failure"
    )
    if is_v3:
        text_info += "\n" r"$|\text{calo } v_{2}| < 0.48$"

    ax.text(0.95, 0.95, text_info, transform=ax.transAxes, ha='right', va='top', fontsize=15)

    if extra_energy_cut:
        jet_selection_text = (
            r"Jets:" + "\n"
            r"Energy > 0" + "\n"
            r"$|\eta| < 1.1 - R$"
        )
    else:
        jet_selection_text = (
            r"Jets:" + "\n"
            r"$|\eta| < 1.1 - R$"
        )
    ax.text(0.95, 0.5, jet_selection_text, transform=ax.transAxes, ha='right', va='top', fontsize=15)

    ax.legend(loc='upper right', bbox_to_anchor=(0.99, 0.7), frameon=False, fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.95)

    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def clean_root_latex(text):
    if not text:
        return ""
    text = text.strip()
    # Replace ROOT TLatex # with \ for LaTeX math
    text = text.replace("#", "\\")
    if "$" not in text:
        # Wrap terms containing subscripts/superscripts (like v_{2}, v_2, p_{T}) in math mode $...$
        text = re.sub(r'([a-zA-Z0-9\\_*|()]+(?:_{[^}\s]+}|^{[^}\s]+}|_[a-zA-Z0-9]+|\^[a-zA-Z0-9]+)+)', r'$\1$', text)
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

def make_2d_plot(hist2d, run_number, output_path, hist_name="", xlim_left=None, ylim_bottom=None, ylim_top=None, extra_label=None):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    values, xedges, yedges = hist2d.to_numpy()

    xlabel, ylabel = get_hist_axis_titles(hist2d, hist_name)

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
        ax.set_xlabel(xlabel)
    if ylabel:
        ax.set_ylabel(ylabel, labelpad=2)

    if xlim_left is not None:
        ax.set_xlim(left=xlim_left)
    if ylim_bottom is not None or ylim_top is not None:
        ax.set_ylim(bottom=ylim_bottom, top=ylim_top)

    if np.max(xedges) >= 1000:
        formatter_x = ScalarFormatter(useMathText=True)
        formatter_x.set_powerlimits((3, 3))
        ax.xaxis.set_major_formatter(formatter_x)

    if ylim_top is None and np.max(yedges) >= 1000:
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

def make_psi_overlay_plot(edges_x, proj_raw, proj_flat, run_number, output_path, xlabel=""):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    hep.histplot((proj_raw, edges_x), ax=ax, histtype='step', color='blue', linewidth=2, label="Raw")
    hep.histplot((proj_flat, edges_x), ax=ax, histtype='step', color='crimson', linewidth=2, label="Flattened")

    ax.set_ylabel("Events")
    if xlabel:
        ax.set_xlabel(xlabel)
    else:
        ax.set_xlabel(r"$\Psi_{2}$")

    ax.set_xlim(-np.pi, np.pi)
    ax.set_ylim(bottom=0)

    if max(np.max(proj_raw), np.max(proj_flat)) >= 1000:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    ax.legend(loc='upper right', frameon=False, fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def make_1d_plot(hist1d, run_number, output_path, hist_name="", extra_labels=None, logy=True):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    values, edges = hist1d.to_numpy()
    errors = hist1d.errors()
    centers = (edges[:-1] + edges[1:]) / 2.0

    xlabel, ylabel = get_hist_axis_titles(hist1d, hist_name)
    if not ylabel:
        ylabel = "Counts"

    if logy:
        hep.histplot((values, edges), ax=ax, histtype='step', color='blue', linewidth=3)
        ax.set_yscale('log')
        ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
    else:
        hep.histplot((values, edges), ax=ax, histtype='step', color='blue', linewidth=3)
        ax.set_ylim(bottom=0)
        if np.max(values) >= 1000:
            formatter_y = ScalarFormatter(useMathText=True)
            formatter_y.set_powerlimits((3, 3))
            ax.yaxis.set_major_formatter(formatter_y)

    if xlabel:
        ax.set_xlabel(xlabel)
    ax.set_ylabel(ylabel)
    ax.set_xlim(left=0)

    ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    if extra_labels:
        ax.text(0.5, 0.98, "\n".join(extra_labels), transform=ax.transAxes, ha='right', va='top', fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def process_file(path, output_dir=None, xmax=None, ymax=1e8):
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
            hist_pairs = [
                # r02 plots
                ("hJetPt_r02_iter", "hJetPt_r02_mult", "hJetPt_r02_unsub", f"run_{run_number}_bkgsub_qa.png", False, False, 0.2),
                ("hJetPtv2_r02_iter", "hJetPtv2_r02_mult", "hJetPtv2_r02_unsub", f"run_{run_number}_v2_bkgsub_qa.png", True, False, 0.2),
                ("hJetPtv3_r02_iter", "hJetPtv3_r02_mult", "hJetPtv3_r02_unsub", f"run_{run_number}_v3_bkgsub_qa.png", True, True, 0.2),
                # r03 plots
                ("hJetPt_r03_iter", "hJetPt_r03_mult", "hJetPt_r03_unsub", f"run_{run_number}_r03_bkgsub_qa.png", False, False, 0.3),
                ("hJetPtv2_r03_iter", "hJetPtv2_r03_mult", "hJetPtv2_r03_unsub", f"run_{run_number}_r03_v2_bkgsub_qa.png", True, False, 0.3),
                ("hJetPtv3_r03_iter", "hJetPtv3_r03_mult", "hJetPtv3_r03_unsub", f"run_{run_number}_r03_v3_bkgsub_qa.png", True, True, 0.3),
            ]

            for hist_iter_name, hist_mult_name, hist_unsub_name, out_filename, extra_energy_cut, is_v3, r_jet in hist_pairs:
                if hist_iter_name not in file:
                    print(f"Warning: {hist_iter_name} not found in {path}")
                    continue
                if hist_mult_name not in file:
                    print(f"Warning: {hist_mult_name} not found in {path}")
                    continue
                if hist_unsub_name not in file:
                    print(f"Warning: {hist_unsub_name} not found in {path}")
                    continue

                hist_iter = file[hist_iter_name]
                values_iter, edges_iter = hist_iter.to_numpy()
                errors_iter = hist_iter.errors()

                hist_mult = file[hist_mult_name]
                values_mult, edges_mult = hist_mult.to_numpy()
                errors_mult = hist_mult.errors()

                hist_unsub = file[hist_unsub_name]
                values_unsub, edges_unsub = hist_unsub.to_numpy()
                errors_unsub = hist_unsub.errors()

                # Using edges from the iter histogram for centers
                centers_x = (edges_iter[:-1] + edges_iter[1:]) / 2.0

                if run_output_dir is not None:
                    hists = [
                        (values_iter, "Iterative Bkg Sub", "blue"),
                        (values_mult, "Multiplicity Bkg Sub", "crimson"),
                        (values_unsub, "Unsubtracted", "green"),
                    ]

                    output_path = run_output_dir / out_filename
                    make_combined_plot(edges_iter, hists, run_number, output_path, xmax=xmax, ymax=ymax, extra_energy_cut=extra_energy_cut, is_v3=is_v3, r_jet=r_jet)

            h2_names = [
                "h2CaloECentrality_default",
                "h2CaloECentrality",
                "h2Seeds_iter",
                "h2Seeds_iter_mult",
                "h2CaloV2_iter_Centrality",
                "h2CaloV2_mult_Centrality",
                "h2CaloV2_mult_iter",
                "h2JetPtv2_r02_iter",
                "h2JetPtv2_r02_mult",
                "h2JetPtv2_r02_unsub",
                "h2JetPtv2_r03_iter",
                "h2JetPtv2_r03_mult",
                "h2JetPtv2_r03_unsub",
            ]

            h2_start_zero = {
                "h2CaloV2_iter_Centrality",
                "h2CaloV2_mult_Centrality",
                "h2Seeds_iter",
            }

            h2_start_zero_y = {
                "h2CaloECentrality_default",
                "h2CaloECentrality",
            }

            h2_zoom_names = {
                "h2JetPtv2_r02_iter",
                "h2JetPtv2_r02_mult",
                "h2JetPtv2_r02_unsub",
                "h2JetPtv2_r03_iter",
                "h2JetPtv2_r03_mult",
                "h2JetPtv2_r03_unsub",
            }

            calo_cut_label = None
            if "h2CaloECentrality_default" in file and "h2CaloECentrality" in file:
                val_default, _, _ = file["h2CaloECentrality_default"].to_numpy()
                val_cut, _, _ = file["h2CaloECentrality"].to_numpy()
                sum_default = np.sum(val_default)
                sum_cut = np.sum(val_cut)
                if sum_default > 0:
                    events_cut = sum_default - sum_cut
                    pct_cut = (events_cut / sum_default) * 100.0
                    calo_cut_label = f"Events Cut: {pct_cut:.2f}%"

            for h2_name in h2_names:
                if h2_name not in file:
                    print(f"Warning: {h2_name} not found in {path}")
                    continue

                hist2d = file[h2_name]
                if run_output_dir is not None:
                    output_path = run_output_dir / f"run_{run_number}_{h2_name}.png"
                    xlim_left = 0.0 if h2_name in h2_start_zero else None
                    ylim_bottom = 0.0 if h2_name in h2_start_zero_y else None
                    extra_label = calo_cut_label if h2_name == "h2CaloECentrality" else None
                    make_2d_plot(hist2d, run_number, output_path, hist_name=h2_name, xlim_left=xlim_left, ylim_bottom=ylim_bottom, extra_label=extra_label)

                    if h2_name in h2_zoom_names:
                        zoom_output_path = run_output_dir / f"run_{run_number}_{h2_name}_zoom.png"
                        make_2d_plot(hist2d, run_number, zoom_output_path, hist_name=f"{h2_name} (Zoomed)", xlim_left=xlim_left, ylim_bottom=10, ylim_top=40)

            psi_pairs = [
                ("h2Psi2_S_raw", "h2Psi2_S", f"run_{run_number}_psi2_S.png"),
                ("h2Psi2_N_raw", "h2Psi2_N", f"run_{run_number}_psi2_N.png"),
                ("h2Psi2_NS_raw", "h2Psi2_NS", f"run_{run_number}_psi2_NS.png"),
            ]

            for raw_name, flat_name, out_filename in psi_pairs:
                if raw_name not in file:
                    print(f"Warning: {raw_name} not found in {path}")
                    continue
                if flat_name not in file:
                    print(f"Warning: {flat_name} not found in {path}")
                    continue

                hist_raw = file[raw_name]
                values_raw, edges_x, _ = hist_raw.to_numpy()

                hist_flat = file[flat_name]
                values_flat, _, _ = hist_flat.to_numpy()

                proj_raw = np.sum(values_raw, axis=1)
                proj_flat = np.sum(values_flat, axis=1)

                xlabel, _ = get_hist_axis_titles(hist_raw, raw_name)

                if run_output_dir is not None:
                    output_path = run_output_dir / out_filename
                    make_psi_overlay_plot(edges_x, proj_raw, proj_flat, run_number, output_path, xlabel=xlabel)

            total_events = None
            if "hCentrality" in file:
                values_cent, _ = file["hCentrality"].to_numpy()
                total_events = np.sum(values_cent)

            h1_configs = [
                ("hCentrality", [r"$|z| < 10$ cm & MB"], False),
                ("hCentralityCaloFail", [r"$|z| < 10$ cm & MB"], False),
                ("hCaloV2Fail_iter", [r"$|z| < 10$ cm & MB", "Good Calo-Cent"], False),
                ("hCaloV2Fail_mult", [r"$|z| < 10$ cm & MB", "Good Calo-Cent"], False),
            ]

            for h1_name, extra_labels, logy in h1_configs:
                if h1_name not in file:
                    print(f"Warning: {h1_name} not found in {path}")
                    continue

                hist1d = file[h1_name]
                values_1d, _ = hist1d.to_numpy()
                fail_sum = np.sum(values_1d)

                labels_to_show = list(extra_labels)
                if h1_name != "hCentrality" and total_events is not None and total_events > 0:
                    pct = (fail_sum / total_events) * 100.0
                    labels_to_show.append(f"Fail: {fail_sum:.2e} ({pct:.2f}%)")
                elif h1_name == "hCentrality":
                    labels_to_show.append(f"Total: {fail_sum:.2e}")

                if run_output_dir is not None:
                    output_path = run_output_dir / f"run_{run_number}_{h1_name}.png"
                    make_1d_plot(hist1d, run_number, output_path, hist_name=h1_name, extra_labels=labels_to_show, logy=logy)

            return None
    except Exception as e:
        return f"Error processing {path}: {e}"

def main():
    parser = argparse.ArgumentParser(description="Plot Jet Pt QA for iter and mult bkg subtraction per run.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
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
    process_func = functools.partial(process_file, output_dir=args.output_dir)
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
