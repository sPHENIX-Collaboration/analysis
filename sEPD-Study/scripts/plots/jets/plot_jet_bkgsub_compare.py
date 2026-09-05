#!/usr/bin/env python3

import argparse
from pathlib import Path
import re
import sys
import numpy as np
import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
from matplotlib.ticker import LogLocator, MaxNLocator, ScalarFormatter
import mplhep as hep

# Known variation metadata (id, label, color, marker)
KNOWN_VARIATIONS = {
    "default": {
        "label": "Default",
        "color": "black",
        "marker": "o",
        "order": 0,
    },
    "no-neg-thresh": {
        "label": "No Neg Thresh",
        "color": "#e41a1c",
        "marker": "s",
        "order": 1,
    },
    "neg-thresh-1": {
        "label": "Neg Thresh -1",
        "color": "#377eb8",
        "marker": "^",
        "order": 2,
    },
    "neg-thresh-2": {
        "label": "Neg Thresh -2",
        "color": "#4daf4a",
        "marker": "v",
        "order": 3,
    },
    "neg-thresh-3": {
        "label": "Neg Thresh -3",
        "color": "#ff7f00",
        "marker": "p",
        "order": 4,
    },
    "neg-thresh-5": {
        "label": "Neg Thresh -5",
        "color": "#984ea3",
        "marker": "D",
        "order": 5,
    },
}

FALLBACK_COLORS = ["#a65628", "#f781bf", "#999999", "#66c2a5", "#fc8d62", "#e7298a"]
FALLBACK_MARKERS = ["P", "*", "X", "h", "d", "<"]

COMMON_TEXT_INFO = (
    r"$|z| < 10$ cm & MB" "\n"
    "Good Calo-Cent\n"
    r"Centrality: 0–60%" "\n"
    "No Flow Failure"
)


def get_variation_meta(var_id, fallback_idx=0):
    if var_id in KNOWN_VARIATIONS:
        return KNOWN_VARIATIONS[var_id]
    color = FALLBACK_COLORS[fallback_idx % len(FALLBACK_COLORS)]
    marker = FALLBACK_MARKERS[fallback_idx % len(FALLBACK_MARKERS)]
    return {
        "label": var_id.replace("-", " ").title(),
        "color": color,
        "marker": marker,
        "order": 100 + fallback_idx,
    }


def parse_list_files(input_dir, list_files=None):
    """
    Parses list files to find ROOT file paths for each variation and run.
    """
    if list_files:
        files = [Path(p) for p in list_files]
    else:
        files = sorted(Path(input_dir).glob("*.list"))

    if not files:
        return {}, set()

    variations = {}
    all_runs = set()

    for list_file in files:
        var_id = list_file.stem
        variations[var_id] = {}
        with list_file.open("r") as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith("#"):
                    continue
                root_path = Path(line)
                match = re.search(r"(\d+)\.root", root_path.name)
                if match:
                    run_num = int(match.group(1))
                    variations[var_id][run_num] = root_path
                    all_runs.add(run_num)

    return variations, all_runs


def extract_profile_x(file_path, hist_name="h2Seeds_iter", positive_only=None):
    """
    Extracts ProfileX of a TH2 using uproot and hist.
    """
    if not file_path.exists():
        return None

    if positive_only is None:
        positive_only = ("Seeds" in hist_name)

    with uproot.open(file_path) as rf:
        if hist_name not in rf:
            print(f"Warning: {hist_name} not found in {file_path}")
            return None

        h2 = rf[hist_name]
        hist_obj = h2.to_hist()
        prof = hist_obj.profile(1)

        edges_x = np.array(prof.axes[0].edges)
        centers_x = (edges_x[:-1] + edges_x[1:]) / 2.0
        values = np.array(prof.values())
        variances = np.array(prof.variances())
        errors = np.sqrt(np.where(np.isnan(variances) | (variances < 0), 0.0, variances))

        mask = ~np.isnan(values) & ~np.isnan(errors)
        if positive_only:
            mask &= (values > 0)

        return {
            "centers": centers_x,
            "edges": edges_x,
            "values": values,
            "errors": errors,
            "mask": mask,
        }


# --- Helpers for Refactored Plotting ---

def save_fig(fig, output_path, save_pdf=False, dpi=300):
    """Saves figure to output_path, and optionally as .pdf if save_pdf=True."""
    output_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output_path, dpi=dpi)
    print(f"Saved: {output_path}")
    if save_pdf:
        pdf_path = output_path.with_suffix(".pdf")
        fig.savefig(pdf_path)
        print(f"Saved: {pdf_path}")


def calc_ratio_errors(ratio, errs_num, errs_den, vals_den):
    """Standard error propagation for ratio = num / den."""
    return np.sqrt(errs_num ** 2 + (ratio * errs_den) ** 2) / np.abs(vals_den)


def setup_ratio_panel(ax_ratio, xlabel, ylabel="Var / Default", ratio_ylim=None, all_ratios=None, margin_factor=1.35, min_margin=0.01, max_margin=None):
    """Configures the ratio subplot panel with standard layout and dynamic limits."""
    ax_ratio.axhline(1.0, color="gray", linestyle="--", linewidth=1.0)
    ax_ratio.set_xlabel(xlabel)
    ax_ratio.set_ylabel(ylabel, fontsize=13)
    ax_ratio.yaxis.set_major_locator(MaxNLocator(nbins=5, prune=None))

    if ratio_ylim is not None:
        ax_ratio.set_ylim(ratio_ylim[0], ratio_ylim[1])
    elif all_ratios and len(all_ratios) > 0:
        arr_r = np.array(all_ratios)
        valid_r = arr_r[~np.isnan(arr_r)]
        if len(valid_r) > 0:
            max_dev = np.max(np.abs(valid_r - 1.0))
            margin = max(max_dev * margin_factor, min_margin)
            if max_margin is not None:
                margin = min(margin, max_margin)
                margin = np.ceil(margin * 100.0) / 100.0
            ax_ratio.set_ylim(1.0 - margin, 1.0 + margin)
        else:
            ax_ratio.set_ylim(1.0 - min_margin, 1.0 + min_margin)
    else:
        ax_ratio.set_ylim(1.0 - min_margin, 1.0 + min_margin)


# --- Plotting Functions ---

def plot_profile_overlay(
    run_number,
    profiles_by_var,
    output_path,
    ylabel=r"$\langle\mathrm{Seeds}_{\mathrm{iter}}\rangle$",
    shift_step=0.12,
    show_ratio=False,
    ratio_ylim=None,
    legend_loc="lower left",
    ylim=None,
    add_hline_zero=False,
    save_pdf=False,
):
    hep.style.use("ATLAS")

    sorted_vars = sorted(profiles_by_var.keys(), key=lambda k: profiles_by_var[k]["meta"].get("order", 999))
    n_vars = len(sorted_vars)

    if show_ratio and "default" in profiles_by_var:
        fig, (ax, ax_ratio) = plt.subplots(2, 1, figsize=(8, 8), gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08}, sharex=True)
    else:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax_ratio = None

    default_prof = profiles_by_var.get("default")
    all_ratios = []

    for idx, var_id in enumerate(sorted_vars):
        data = profiles_by_var[var_id]
        meta = data["meta"]
        prof = data["prof"]

        centers = prof["centers"]
        values = prof["values"]
        errors = prof["errors"]
        mask = prof["mask"]

        dx = (idx - (n_vars - 1) / 2.0) * shift_step if n_vars > 1 else 0.0
        shifted_x = centers + dx

        ax.errorbar(
            shifted_x[mask], values[mask], yerr=errors[mask],
            fmt=meta["marker"], color=meta["color"], label=meta["label"],
            markersize=5, capsize=2, linewidth=1.2,
        )

        if ax_ratio is not None and default_prof is not None and var_id != "default":
            def_vals = default_prof["prof"]["values"]
            def_errs = default_prof["prof"]["errors"]
            ratio_mask = mask & default_prof["prof"]["mask"] & (def_vals != 0)

            ratio = np.full_like(values, np.nan)
            ratio_err = np.full_like(errors, np.nan)

            ratio[ratio_mask] = values[ratio_mask] / def_vals[ratio_mask]
            ratio_err[ratio_mask] = calc_ratio_errors(
                ratio[ratio_mask], errors[ratio_mask], def_errs[ratio_mask], def_vals[ratio_mask]
            )

            reliable = (ratio_err[ratio_mask] < 0.25)
            if np.any(reliable):
                all_ratios.extend(ratio[ratio_mask][reliable])
            else:
                all_ratios.extend(ratio[ratio_mask])

            ax_ratio.errorbar(
                shifted_x[ratio_mask], ratio[ratio_mask], yerr=ratio_err[ratio_mask],
                fmt=meta["marker"], color=meta["color"], markersize=4, capsize=2, linewidth=1.0,
            )

    ax.set_ylabel(ylabel)
    ax.set_xlim(0, 60)

    if ylim is not None:
        ax.set_ylim(ylim[0], ylim[1])
    else:
        all_vals = np.concatenate([
            profiles_by_var[v]["prof"]["values"][profiles_by_var[v]["prof"]["mask"]]
            for v in sorted_vars
        ])
        if len(all_vals) > 0:
            min_v = np.min(all_vals)
            max_v = np.max(all_vals)
            if min_v > 0 and min_v > 0.3 * max_v:
                ymin, ymax = max(0.0, min_v * 0.85), max_v * 1.25
            elif min_v >= 0:
                ymin, ymax = 0.0, max_v * 1.35
            else:
                span = max_v - min_v
                ymin, ymax = min_v - 0.15 * span, max_v + 0.25 * span
            ax.set_ylim(ymin, ymax)

    if add_hline_zero:
        ax.axhline(0, color="gray", linestyle="--", linewidth=1.0, alpha=0.5)

    ax.text(1.0, 1.01, f"Run: {run_number}", transform=ax.transAxes, ha="right", va="bottom", fontsize=15)
    ax.text(0.95, 0.95, COMMON_TEXT_INFO, transform=ax.transAxes, ha="right", va="top", fontsize=14)
    ax.legend(loc=legend_loc, frameon=False, fontsize=13)

    if ax_ratio is not None:
        setup_ratio_panel(ax_ratio, xlabel="Centrality [%]", ratio_ylim=ratio_ylim, all_ratios=all_ratios, min_margin=0.01)
        plt.subplots_adjust(left=0.14, right=0.96, bottom=0.10, top=0.94)
    else:
        ax.set_xlabel("Centrality [%]")
        fig.tight_layout()
        plt.subplots_adjust(left=0.13, bottom=0.13, top=0.94)

    save_fig(fig, output_path, save_pdf)
    plt.close(fig)


def plot_1d_hist_overlay(run_number, hist_name, r_jet, variations, var_metadata, output_path, show_ratio=False, ratio_ylim=None, shift_step=0.12, pt_max=None, save_pdf=False):
    hep.style.use("ATLAS")

    sorted_vars = sorted(variations.keys(), key=lambda k: var_metadata[k].get("order", 999))
    hists_data = {}
    def_data = None
    max_x_val = 0

    for v in sorted_vars:
        file_path = variations[v].get(run_number)
        if file_path and file_path.exists():
            with uproot.open(file_path) as rf:
                if hist_name in rf:
                    h = rf[hist_name]
                    vals, edges = h.to_numpy()
                    hists_data[v] = {
                        "values": vals,
                        "edges": edges,
                        "errors": h.errors(),
                        "meta": var_metadata[v],
                    }
                    nz = np.where(vals > 0)[0]
                    if len(nz) > 0:
                        max_x_val = max(max_x_val, edges[nz[-1] + 1])
                    if v == "default":
                        def_data = hists_data[v]

    if not hists_data:
        return

    n_vars = len(hists_data)
    if show_ratio and def_data is not None:
        fig, (ax, ax_ratio) = plt.subplots(2, 1, figsize=(8, 8), gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08}, sharex=True)
    else:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax_ratio = None

    all_ratios = []
    max_y_val = 1.0

    for idx, v in enumerate(sorted_vars):
        if v not in hists_data:
            continue
        d = hists_data[v]
        vals, edges, errs, meta = d["values"], d["edges"], d["errors"], d["meta"]

        max_y_val = max(max_y_val, np.max(vals))
        hep.histplot((vals, edges), ax=ax, histtype="step", color=meta["color"], linewidth=2, label=meta["label"])

        if ax_ratio is not None and def_data is not None and v != "default":
            def_vals, def_errs = def_data["values"], def_data["errors"]
            centers = (edges[:-1] + edges[1:]) / 2.0

            valid_mask = (vals > 0) & (def_vals > 0)
            valid_mask &= (centers <= pt_max) if pt_max is not None else (centers <= max(max_x_val, 60))

            if np.any(valid_mask):
                ratio = vals[valid_mask] / def_vals[valid_mask]
                ratio_err = calc_ratio_errors(ratio, errs[valid_mask], def_errs[valid_mask], def_vals[valid_mask])
                dx = (idx - (n_vars - 1) / 2.0) * shift_step if n_vars > 1 else 0.0

                ax_ratio.errorbar(
                    centers[valid_mask] + dx, ratio, yerr=ratio_err,
                    fmt=meta["marker"], color=meta["color"], markersize=4, capsize=2, linewidth=1.0,
                )

                reliable = (ratio_err < 0.25)
                if np.any(reliable):
                    all_ratios.extend(ratio[reliable])

    ax.set_yscale("log")
    ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
    ax.set_ylim(bottom=0.5, top=max_y_val * 10)
    ax.set_xlim(left=0, right=pt_max if pt_max is not None else min(max(max_x_val, 60), 80))
    ax.set_ylabel("Counts")

    ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha="right", va="bottom", fontsize=15)
    ax.text(0.95, 0.95, f"{COMMON_TEXT_INFO}\nEnergy > 0", transform=ax.transAxes, ha="right", va="top", fontsize=13)
    ax.legend(loc="upper right", bbox_to_anchor=(0.95, 0.65), frameon=False, fontsize=13)

    if ax_ratio is not None:
        setup_ratio_panel(ax_ratio, xlabel=r"Jet $p_{T}$ [GeV]", ratio_ylim=ratio_ylim, all_ratios=all_ratios, min_margin=0.05)
        plt.subplots_adjust(left=0.14, right=0.96, bottom=0.10, top=0.94)
    else:
        ax.set_xlabel(r"Jet $p_{T}$ [GeV]")
        fig.tight_layout()
        plt.subplots_adjust(left=0.13, bottom=0.13, top=0.94)

    save_fig(fig, output_path, save_pdf)
    plt.close(fig)


def plot_jet_eta_overlay(run_number, hist_name, r_jet, variations, var_metadata, output_path, show_ratio=False, ratio_ylim=None, shift_step=0.005, save_pdf=False):
    hep.style.use("ATLAS")

    sorted_vars = sorted(variations.keys(), key=lambda k: var_metadata[k].get("order", 999))
    eta_limit = round(1.1 - r_jet, 1)
    hists_data = {}
    def_data = None

    for v in sorted_vars:
        file_path = variations[v].get(run_number)
        if file_path and file_path.exists():
            with uproot.open(file_path) as rf:
                if hist_name in rf:
                    proj = rf[hist_name].to_hist().project(1)
                    vals = proj.values()
                    vars_ = proj.variances()
                    hists_data[v] = {
                        "values": vals,
                        "edges": proj.axes[0].edges,
                        "errors": np.sqrt(np.where(np.isnan(vars_) | (vars_ < 0), 0.0, vars_)),
                        "meta": var_metadata[v],
                    }
                    if v == "default":
                        def_data = hists_data[v]

    if not hists_data:
        return

    n_vars = len(hists_data)
    if show_ratio and def_data is not None:
        fig, (ax, ax_ratio) = plt.subplots(2, 1, figsize=(8, 8), gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08}, sharex=True)
    else:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax_ratio = None

    all_ratios = []
    max_y_val = 1.0

    for idx, v in enumerate(sorted_vars):
        if v not in hists_data:
            continue
        d = hists_data[v]
        vals, edges, errs, meta = d["values"], d["edges"], d["errors"], d["meta"]

        max_y_val = max(max_y_val, np.max(vals))
        hep.histplot((vals, edges), ax=ax, histtype="step", color=meta["color"], linewidth=2, label=meta["label"])

        if ax_ratio is not None and def_data is not None and v != "default":
            def_vals, def_errs = def_data["values"], def_data["errors"]
            centers = (edges[:-1] + edges[1:]) / 2.0

            valid_mask = (vals > 0) & (def_vals > 0) & (centers >= -eta_limit) & (centers <= eta_limit)

            if np.any(valid_mask):
                ratio = vals[valid_mask] / def_vals[valid_mask]
                ratio_err = calc_ratio_errors(ratio, errs[valid_mask], def_errs[valid_mask], def_vals[valid_mask])
                dx = (idx - (n_vars - 1) / 2.0) * shift_step if n_vars > 1 else 0.0

                ax_ratio.errorbar(
                    centers[valid_mask] + dx, ratio, yerr=ratio_err,
                    fmt=meta["marker"], color=meta["color"], markersize=4, capsize=2, linewidth=1.0,
                )

                bulk_reliable = (ratio_err < 0.25) & (np.abs(centers[valid_mask]) <= (eta_limit - 0.05))
                if np.any(bulk_reliable):
                    all_ratios.extend(ratio[bulk_reliable])
                elif np.any(ratio_err < 0.25):
                    all_ratios.extend(ratio[ratio_err < 0.25])

    ax.set_ylim(bottom=0, top=max_y_val * 1.55)
    ax.set_xlim(-eta_limit, eta_limit)
    ax.set_ylabel("Counts")

    if max_y_val >= 1000:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha="right", va="bottom", fontsize=15)
    ax.text(0.95, 0.95, f"{COMMON_TEXT_INFO}\nEnergy > 0", transform=ax.transAxes, ha="right", va="top", fontsize=13)
    ax.legend(loc="upper left", frameon=False, fontsize=13)

    if ax_ratio is not None:
        setup_ratio_panel(ax_ratio, xlabel=r"$\eta$", ratio_ylim=ratio_ylim, all_ratios=all_ratios, min_margin=0.05, max_margin=0.08)
        plt.subplots_adjust(left=0.14, right=0.96, bottom=0.10, top=0.94)
    else:
        ax.set_xlabel(r"$\eta$")
        fig.tight_layout()
        plt.subplots_adjust(left=0.13, bottom=0.13, top=0.94)

    save_fig(fig, output_path, save_pdf)
    plt.close(fig)


def plot_calo_v2_fail_overlay(run_number, variations, var_metadata, output_path, hist_name="hCaloV2Fail_iter", show_ratio=False, ratio_ylim=None, shift_step=0.12, save_pdf=False):
    hep.style.use("ATLAS")

    sorted_vars = sorted(variations.keys(), key=lambda k: var_metadata[k].get("order", 999))
    hists_data = {}
    def_data = None
    max_y_val = 1.0

    for v in sorted_vars:
        file_path = variations[v].get(run_number)
        if file_path and file_path.exists():
            with uproot.open(file_path) as rf:
                if hist_name in rf:
                    h = rf[hist_name]
                    vals, edges = h.to_numpy()
                    hists_data[v] = {
                        "values": vals,
                        "edges": edges,
                        "errors": h.errors(),
                        "meta": var_metadata[v],
                    }
                    if np.max(vals) > max_y_val:
                        max_y_val = np.max(vals)
                    if v == "default":
                        def_data = hists_data[v]

    if not hists_data:
        return

    n_vars = len(hists_data)
    if show_ratio and def_data is not None:
        fig, (ax, ax_ratio) = plt.subplots(2, 1, figsize=(8, 8), gridspec_kw={"height_ratios": [3, 1], "hspace": 0.08}, sharex=True)
    else:
        fig, ax = plt.subplots(figsize=(8, 6))
        ax_ratio = None

    all_ratios = []

    for idx, v in enumerate(sorted_vars):
        if v not in hists_data:
            continue
        d = hists_data[v]
        vals, edges, errs, meta = d["values"], d["edges"], d["errors"], d["meta"]

        hep.histplot((vals, edges), ax=ax, histtype="step", color=meta["color"], linewidth=2, label=meta["label"])

        if ax_ratio is not None and def_data is not None and v != "default":
            def_vals, def_errs = def_data["values"], def_data["errors"]
            centers = (edges[:-1] + edges[1:]) / 2.0

            valid_mask = (vals > 0) & (def_vals > 0) & (centers >= 0) & (centers <= 60)

            if np.any(valid_mask):
                ratio = vals[valid_mask] / def_vals[valid_mask]
                ratio_err = calc_ratio_errors(ratio, errs[valid_mask], def_errs[valid_mask], def_vals[valid_mask])
                dx = (idx - (n_vars - 1) / 2.0) * shift_step if n_vars > 1 else 0.0

                ax_ratio.errorbar(
                    centers[valid_mask] + dx, ratio, yerr=ratio_err,
                    fmt=meta["marker"], color=meta["color"], markersize=4, capsize=2, linewidth=1.0,
                )

                reliable = (ratio_err < 0.35)
                if np.any(reliable):
                    all_ratios.extend(ratio[reliable])
                else:
                    all_ratios.extend(ratio)

    ax.set_ylim(bottom=0, top=max_y_val * 1.45)
    ax.set_xlim(0, 60)
    ax.set_ylabel("Events")

    if max_y_val >= 1000:
        formatter_y = ScalarFormatter(useMathText=True)
        formatter_y.set_powerlimits((3, 3))
        ax.yaxis.set_major_formatter(formatter_y)

    ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha="right", va="bottom", fontsize=15)
    ax.text(0.95, 0.95, r"$|z| < 10$ cm & MB" "\nGood Calo-Cent\n" r"Centrality: 0–60%" "\n" r"Calo $v_{2}$ Failures", transform=ax.transAxes, ha="right", va="top", fontsize=13)
    ax.legend(loc="upper right", bbox_to_anchor=(0.95, 0.70), frameon=False, fontsize=13)

    if ax_ratio is not None:
        setup_ratio_panel(ax_ratio, xlabel="Centrality [%]", ratio_ylim=ratio_ylim, all_ratios=all_ratios, margin_factor=1.25, min_margin=0.05)
        plt.subplots_adjust(left=0.14, right=0.96, bottom=0.10, top=0.94)
    else:
        ax.set_xlabel("Centrality [%]")
        fig.tight_layout()
        plt.subplots_adjust(left=0.13, bottom=0.13, top=0.94)

    save_fig(fig, output_path, save_pdf)
    plt.close(fig)


def plot_2d_grid(
    variations,
    runs,
    var_metadata,
    output_path,
    hist_name="h2Seeds_iter",
    xlim=(0, 60),
    ylim=(0, 30),
    xlabel="Centrality [%]",
    ylabel="Seeds Iterative",
    add_hline_zero=False,
    save_pdf=False,
):
    hep.style.use("ATLAS")

    sorted_runs = sorted(runs)
    sorted_vars = sorted(variations.keys(), key=lambda k: var_metadata[k].get("order", 999))
    n_rows, n_cols = len(sorted_runs), len(sorted_vars)

    if n_rows == 0 or n_cols == 0:
        print(f"Warning: No runs or variations to plot for 2D grid ({hist_name}).")
        return

    vmax_global = 10
    cached_hists = {}
    for r in sorted_runs:
        for v in sorted_vars:
            file_path = variations[v].get(r)
            if file_path and file_path.exists():
                with uproot.open(file_path) as rf:
                    if hist_name in rf:
                        vals, xedges, yedges = rf[hist_name].to_numpy()
                        cached_hists[(r, v)] = (vals, xedges, yedges)
                        if np.max(vals) > vmax_global:
                            vmax_global = np.max(vals)

    if not cached_hists:
        print(f"Warning: Histogram {hist_name} not found in any input files.")
        return

    fig, axes = plt.subplots(n_rows, n_cols, figsize=(4.8 * n_cols, 4.5 * n_rows), sharex=True, sharey=True)
    if n_rows == 1 and n_cols == 1: axes = np.array([[axes]])
    elif n_rows == 1: axes = np.expand_dims(axes, axis=0)
    elif n_cols == 1: axes = np.expand_dims(axes, axis=1)

    mesh = None
    for row_idx, r in enumerate(sorted_runs):
        for col_idx, v in enumerate(sorted_vars):
            ax = axes[row_idx, col_idx]
            if (r, v) in cached_hists:
                vals, xedges, yedges = cached_hists[(r, v)]
                vals_masked = np.ma.masked_where(vals <= 0, vals)
                mesh = ax.pcolormesh(xedges, yedges, vals_masked.T, norm=LogNorm(vmin=1, vmax=vmax_global), cmap="viridis", rasterized=True)

            if xlim is not None: ax.set_xlim(xlim[0], xlim[1])
            if ylim is not None: ax.set_ylim(ylim[0], ylim[1])
            if add_hline_zero: ax.axhline(0, color="white", linestyle="--", linewidth=1.0, alpha=0.6)

            ax.set_title(f"Run: {r} | {var_metadata[v]['label']}", fontsize=18, pad=7, weight="bold")
            if row_idx == n_rows - 1 and xlabel: ax.set_xlabel(xlabel, fontsize=18)
            if col_idx == 0 and ylabel: ax.set_ylabel(ylabel, fontsize=18)

    plt.subplots_adjust(left=0.06, right=0.91, bottom=0.06, top=0.95, wspace=0.10, hspace=0.20)

    if mesh is not None:
        cbar_ax = fig.add_axes([0.92, 0.08, 0.015, 0.84])
        cbar = fig.colorbar(mesh, cax=cbar_ax)
        cbar.ax.tick_params(labelsize=20)
        cbar.set_label("Counts", fontsize=25)

    save_fig(fig, output_path, save_pdf, dpi=200)
    plt.close(fig)


def main():
    parser = argparse.ArgumentParser(
        description="Compare jet background subtraction QA across configuration variations."
    )
    parser.add_argument(
        "-i", "--input-dir",
        type=Path,
        default=Path("/sphenix/user/anarde/sEPD-Study/jetAnav3/neg-thresh-test"),
        help="Directory containing variation .list files (default: /sphenix/user/anarde/sEPD-Study/jetAnav3/neg-thresh-test)"
    )
    parser.add_argument(
        "-o", "--output-dir",
        type=Path,
        default=Path("/sphenix/user/anarde/sEPD-Study/jetAnav3/neg-thresh-test/plots"),
        help="Directory to save generated comparison plots"
    )
    parser.add_argument(
        "--lists",
        nargs="*",
        type=Path,
        help="Explicit list of .list files to process (overrides --input-dir)"
    )
    parser.add_argument(
        "--runs",
        nargs="*",
        type=int,
        help="Specific run number(s) to process (default: all runs found across variations)"
    )
    parser.add_argument(
        "--shift-x",
        type=float,
        default=0.12,
        help="Horizontal offset between variation data points for readability (default: 0.12)"
    )
    parser.add_argument(
        "--ratio",
        action="store_true",
        help="Include a bottom ratio panel (variation / default)"
    )
    parser.add_argument(
        "--ratio-ylim",
        nargs=2,
        type=float,
        default=None,
        metavar=("YMIN", "YMAX"),
        help="Custom y-axis limits for the ratio subplot (default: auto-zoom around 1.0)"
    )
    parser.add_argument(
        "--no-2d-grid",
        action="store_true",
        help="Skip generating all 2D grid comparison plots"
    )
    parser.add_argument(
        "--no-seeds-grid",
        action="store_true",
        help="Skip generating the 2D grid comparison plot for h2Seeds_iter"
    )
    parser.add_argument(
        "--no-v2-grid",
        action="store_true",
        help="Skip generating the 2D grid comparison plot for h2CaloV2_iter_Centrality"
    )
    parser.add_argument(
        "--v2-grid-ylim",
        nargs=2,
        type=float,
        default=[-0.5, 0.5],
        metavar=("YMIN", "YMAX"),
        help="Y-axis limits for h2CaloV2_iter_Centrality 2D grid plot (default: -0.5 0.5)"
    )
    parser.add_argument(
        "--no-seeds-profile",
        action="store_true",
        help="Skip generating the 1D Profile X comparison plots for h2Seeds_iter"
    )
    parser.add_argument(
        "--no-v2-profile",
        action="store_true",
        help="Skip generating the 1D Profile X comparison plots for h2CaloV2_iter_Centrality"
    )
    parser.add_argument(
        "--ratio-v2-ylim",
        nargs=2,
        type=float,
        default=None,
        metavar=("YMIN", "YMAX"),
        help="Custom y-axis limits for the Calo v2 profile ratio subplot (default: auto-zoom around 1.0)"
    )
    parser.add_argument(
        "--no-v2-fail",
        action="store_true",
        help="Skip generating the 1D Calo v2 failure comparison plots"
    )
    parser.add_argument(
        "--ratio-v2fail-ylim",
        nargs=2,
        type=float,
        default=None,
        metavar=("YMIN", "YMAX"),
        help="Custom y-axis limits for the Calo v2 failure ratio subplot (default: auto-zoom around 1.0)"
    )
    parser.add_argument(
        "--no-jet-pt",
        action="store_true",
        help="Skip generating the 1D Jet pT comparison plots"
    )
    parser.add_argument(
        "--no-jet-eta",
        action="store_true",
        help="Skip generating the 1D Jet eta comparison plots"
    )
    parser.add_argument(
        "--shift-eta",
        type=float,
        default=0.005,
        help="Horizontal offset between variation data points for eta ratio plot (default: 0.005)"
    )
    parser.add_argument(
        "--ratio-eta-ylim",
        nargs=2,
        type=float,
        default=None,
        metavar=("YMIN", "YMAX"),
        help="Custom y-axis limits for the Jet eta ratio subplot (default: auto-zoom around 1.0, e.g. 0.95 1.05)"
    )
    parser.add_argument(
        "--pt-max",
        type=float,
        default=None,
        help="Maximum x-axis limit for Jet pT plots (default: auto)"
    )
    parser.add_argument(
        "--save-pdf",
        action="store_true",
        help="Save PDF version of plots in addition to PNG"
    )
    args = parser.parse_args()

    variations, all_runs = parse_list_files(args.input_dir, args.lists)
    if not variations:
        print(f"Error: No .list files found in {args.input_dir}")
        sys.exit(1)

    runs_to_process = sorted(args.runs) if args.runs else sorted(all_runs)
    if not runs_to_process:
        print("Error: No runs found to process.")
        sys.exit(1)

    args.output_dir.mkdir(parents=True, exist_ok=True)

    fallback_counter = 0
    var_metadata = {}
    for v in variations.keys():
        var_metadata[v] = get_variation_meta(v, fallback_counter)
        if v not in KNOWN_VARIATIONS:
            fallback_counter += 1

    success_count = 0
    for run in runs_to_process:
        if not args.no_seeds_profile:
            seeds_profiles_by_var = {}
            for var_id, run_dict in variations.items():
                if run in run_dict:
                    prof_data = extract_profile_x(run_dict[run], "h2Seeds_iter")
                    if prof_data is not None:
                        seeds_profiles_by_var[var_id] = {"meta": var_metadata[var_id], "prof": prof_data}

            if seeds_profiles_by_var:
                out_png = args.output_dir / f"run_{run}_h2Seeds_iter_profile_compare.png"
                plot_profile_overlay(
                    run, seeds_profiles_by_var, out_png,
                    ylabel=r"$\langle\mathrm{Seeds}_{\mathrm{iter}}\rangle$",
                    shift_step=args.shift_x, show_ratio=args.ratio, ratio_ylim=args.ratio_ylim,
                    legend_loc="lower left", save_pdf=args.save_pdf,
                )

        if not args.no_v2_profile:
            v2_profiles_by_var = {}
            for var_id, run_dict in variations.items():
                if run in run_dict:
                    v2_prof_data = extract_profile_x(run_dict[run], "h2CaloV2_iter_Centrality")
                    if v2_prof_data is not None:
                        v2_profiles_by_var[var_id] = {"meta": var_metadata[var_id], "prof": v2_prof_data}

            if v2_profiles_by_var:
                v2_ratio_ylim = args.ratio_v2_ylim if args.ratio_v2_ylim is not None else args.ratio_ylim
                v2_out_png = args.output_dir / f"run_{run}_h2CaloV2_iter_Centrality_profile_compare.png"
                plot_profile_overlay(
                    run, v2_profiles_by_var, v2_out_png,
                    ylabel=r"$\langle\mathrm{Calo}\ v_{2}\rangle$",
                    shift_step=args.shift_x, show_ratio=args.ratio, ratio_ylim=v2_ratio_ylim,
                    legend_loc="upper left", add_hline_zero=True, save_pdf=args.save_pdf,
                )

        if not args.no_jet_pt:
            for hist_name, r_jet in [("hJetPtv2_r02_iter", 0.2), ("hJetPtv2_r03_iter", 0.3)]:
                pt_out_png = args.output_dir / f"run_{run}_{hist_name}_compare.png"
                plot_1d_hist_overlay(
                    run, hist_name, r_jet, variations, var_metadata, pt_out_png,
                    show_ratio=args.ratio, ratio_ylim=args.ratio_ylim, shift_step=args.shift_x,
                    pt_max=args.pt_max, save_pdf=args.save_pdf,
                )

        if not args.no_jet_eta:
            eta_ratio_ylim = args.ratio_eta_ylim if args.ratio_eta_ylim is not None else args.ratio_ylim
            for hist_name, r_jet in [("h2JetEtav2_r02_iter", 0.2), ("h2JetEtav2_r03_iter", 0.3)]:
                eta_out_png = args.output_dir / f"run_{run}_{hist_name}_compare.png"
                plot_jet_eta_overlay(
                    run, hist_name, r_jet, variations, var_metadata, eta_out_png,
                    show_ratio=args.ratio, ratio_ylim=eta_ratio_ylim, shift_step=args.shift_eta, save_pdf=args.save_pdf,
                )

        if not args.no_v2_fail:
            fail_out_png = args.output_dir / f"run_{run}_hCaloV2Fail_iter_compare.png"
            fail_ratio_ylim = args.ratio_v2fail_ylim if args.ratio_v2fail_ylim is not None else args.ratio_ylim
            plot_calo_v2_fail_overlay(
                run, variations, var_metadata, fail_out_png,
                hist_name="hCaloV2Fail_iter", show_ratio=args.ratio, ratio_ylim=fail_ratio_ylim,
                shift_step=args.shift_x, save_pdf=args.save_pdf,
            )

        success_count += 1

    print(f"Successfully generated comparison plots for {success_count} runs in {args.output_dir}")

    if not args.no_2d_grid and len(runs_to_process) > 0 and len(variations) > 0:
        grid_dim = f"{len(runs_to_process)}x{len(variations)}"

        if not args.no_seeds_grid:
            out_grid_seeds_png = args.output_dir / f"grid_{grid_dim}_h2Seeds_iter.png"
            plot_2d_grid(
                variations, runs_to_process, var_metadata, out_grid_seeds_png,
                hist_name="h2Seeds_iter", xlim=(0, 60), ylim=(0, 30),
                xlabel="Centrality [%]", ylabel="Seeds Iterative", save_pdf=args.save_pdf,
            )

        if not args.no_v2_grid:
            v2_ylim = tuple(args.v2_grid_ylim) if args.v2_grid_ylim is not None else (-0.5, 0.5)
            out_grid_v2_png = args.output_dir / f"grid_{grid_dim}_h2CaloV2_iter_Centrality.png"
            plot_2d_grid(
                variations, runs_to_process, var_metadata, out_grid_v2_png,
                hist_name="h2CaloV2_iter_Centrality", xlim=(0, 60), ylim=v2_ylim,
                xlabel="Centrality [%]", ylabel=r"Calo $v_{2}$", add_hline_zero=True, save_pdf=args.save_pdf,
            )

if __name__ == "__main__":
    main()
