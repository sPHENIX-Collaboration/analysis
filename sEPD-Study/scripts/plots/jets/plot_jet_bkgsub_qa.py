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
from mpl_toolkits.axes_grid1 import make_axes_locatable

def make_combined_plot(edges_x, hists, run_number, output_path, xmax=None, ymax=1e8, extra_energy_cut=False, is_v3=False, r_jet=0.2, is_eta=False, pt_min_label=None, flow_fail=False):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    for values, label, color in hists:
        hep.histplot((values, edges_x), ax=ax, histtype='step', color=color, linewidth=2, label=label)

    if is_eta:
        ax.set_xlabel(r"$\eta$", labelpad=-5)
        ax.set_ylabel(r"$\frac{1}{N_{\mathrm{jet}}} \frac{\mathrm{d}N}{\mathrm{d}\eta}$", labelpad=5)
        eta_limit = round(1.1 - r_jet, 1)
        ax.set_xlim(-eta_limit, eta_limit)
        ax.set_ylim(bottom=0)
    else:
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
    if r_jet is not None:
        ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)
    else:
        ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    flow_text = "Flow Failure" if flow_fail else "No Flow Failure"
    text_info = (
        r"$|z| < 10$ cm & MB" "\n"
        f"Good Calo-Cent" "\n"
        f"Centrality: 0-60%" "\n"
        f"{flow_text}"
    )
    if is_v3:
        text_info += "\n" r"$|\text{calo } v_{2}| < 0.48$"

    ax.text(0.95, 0.95, text_info, transform=ax.transAxes, ha='right', va='top', fontsize=15)

    jet_sel_lines = [r"Jets:"]
    if pt_min_label:
        jet_sel_lines.append(rf"$p_{{T}} \geq {pt_min_label}$ GeV")
    if extra_energy_cut:
        jet_sel_lines.append(r"Energy > 0")
    jet_sel_lines.append(r"$|\eta| < 1.1 - R$")

    jet_selection_text = "\n".join(jet_sel_lines)

    if is_eta:
        ax.text(0.5, 0.95, jet_selection_text, transform=ax.transAxes, ha='right', va='top', fontsize=15)
        ax.legend(loc='lower left', frameon=False, fontsize=15)
    else:
        ax.text(0.95, 0.5, jet_selection_text, transform=ax.transAxes, ha='right', va='top', fontsize=15)
        ax.legend(loc='upper right', bbox_to_anchor=(0.99, 0.7), frameon=False, fontsize=15)

    fig.tight_layout()
    if is_eta:
        plt.subplots_adjust(left=0.12, bottom=0.1, top=0.95)
    else:
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
        # Wrap LaTeX command terms (like \eta, \Psi) or terms containing subscripts/superscripts in math mode $...$
        text = re.sub(r'(\\[a-zA-Z]+(?:_{[^}\s]+}|^{[^}\s]+}|_[a-zA-Z0-9]+|\^[a-zA-Z0-9]+)*|[a-zA-Z0-9\\_*|()]+(?:_{[^}\s]+}|^{[^}\s]+}|_[a-zA-Z0-9]+|\^[a-zA-Z0-9]+)+)', r'$\1$', text)
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

def make_generic_overlay(edges_x, hists, run_number, output_path, xlabel, ylabel, xmax=None, ymax=None, logy=False, extra_labels=None, legend_loc='best', xlim=None, r_jet=None):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    for values, label, color, ls in hists:
        hep.histplot((values, edges_x), ax=ax, histtype='step', color=color, linewidth=2, linestyle=ls, label=label)

    if logy:
        ax.set_yscale('log')
        ax.yaxis.set_major_locator(LogLocator(base=10.0, numticks=20))
        if ymax is not None:
            ax.set_ylim(bottom=0.5, top=ymax)
        else:
            ax.set_ylim(bottom=0.5)
    else:
        if ymax is not None:
            ax.set_ylim(bottom=0, top=ymax)
        else:
            ax.set_ylim(bottom=0)

        max_val = 0
        for values, _, _, _ in hists:
            m = np.max(values)
            if m > max_val: max_val = m
        if max_val >= 1000:
            formatter_y = ScalarFormatter(useMathText=True)
            formatter_y.set_powerlimits((3, 3))
            ax.yaxis.set_major_formatter(formatter_y)

    if xlabel:
        ax.set_xlabel(xlabel)
    if ylabel:
        ax.set_ylabel(ylabel)

    if xlim is not None:
        ax.set_xlim(xlim)
    elif xmax == 'auto':
        max_x_val = 0
        for values, _, _, _ in hists:
            nonzero_indices = np.where(values > 0)[0]
            if len(nonzero_indices) > 0:
                max_bin_edge = edges_x[nonzero_indices[-1] + 1]
                if max_bin_edge > max_x_val:
                    max_x_val = max_bin_edge
        if max_x_val > 0:
            ax.set_xlim(left=0, right=max_x_val)
        else:
            ax.set_xlim(left=0)
    elif xmax is not None:
        ax.set_xlim(left=0, right=xmax)

    if r_jet is not None:
        ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)
    else:
        ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    if extra_labels:
        ax.text(0.95, 0.95, "\n".join(extra_labels), transform=ax.transAxes, ha='right', va='top', fontsize=15)

    if hists:
        ax.legend(loc=legend_loc, frameon=False, fontsize=15)

    fig.tight_layout()
    plt.subplots_adjust(left=0.12, bottom=0.13, top=0.93)
    fig.savefig(output_path, dpi=300)
    plt.close(fig)

def make_profile_overlay(edges_x, profiles, run_number, output_path, xlabel, ylabel, xmax=None, ymin=None, ymax=None, extra_labels=None, legend_loc='best', xlim=None, r_jet=None, add_hline_zero=False):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(8, 6))

    centers_x = (edges_x[:-1] + edges_x[1:]) / 2.0

    for values, errors, label, color in profiles:
        mask = ~np.isnan(values) & ~np.isnan(errors) & ((values != 0) | (errors > 0))
        if np.any(mask):
            ax.errorbar(centers_x[mask], values[mask], yerr=errors[mask], fmt='o', color=color, label=label, markersize=4, capsize=2)

    if add_hline_zero:
        ax.axhline(0, color='black', linestyle='--', linewidth=1.5, alpha=0.7)

    if ymin is not None:
        ax.set_ylim(bottom=ymin, top=ymax)
    elif ymax is not None:
        ax.set_ylim(bottom=0, top=ymax)
    else:
        ax.set_ylim(bottom=0)

    if xlabel:
        ax.set_xlabel(xlabel)
    if ylabel:
        ax.set_ylabel(ylabel)

    if xlim is not None:
        ax.set_xlim(xlim)
    elif xmax == 'auto':
        max_x_val = 0
        for values, errors, _, _ in profiles:
            nonzero_indices = np.where(~np.isnan(values) & (values > 0))[0]
            if len(nonzero_indices) > 0:
                max_bin_edge = edges_x[nonzero_indices[-1] + 1]
                if max_bin_edge > max_x_val:
                    max_x_val = max_bin_edge
        if max_x_val > 0:
            ax.set_xlim(left=0, right=max_x_val)
        else:
            ax.set_xlim(left=0)
    elif xmax is not None:
        ax.set_xlim(left=0, right=xmax)

    if r_jet is not None:
        ax.text(1.0, 1.01, rf"Run: {run_number}, $R = {r_jet:g}$", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)
    else:
        ax.text(1.0, 1.01, rf"Run: {run_number}", transform=ax.transAxes, ha='right', va='bottom', fontsize=15)

    if extra_labels:
        ax.text(0.95, 0.95, "\n".join(extra_labels), transform=ax.transAxes, ha='right', va='top', fontsize=15)

    if profiles:
        ax.legend(loc=legend_loc, frameon=False, fontsize=18)

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

def process_file(path, output_dir=None, xmax=None, ymax=1e8, do_iter=True, do_mult=True, do_unsub=True, do_rcone=True, rcone_pt_max=80):
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
                hists = []
                edges_iter = None

                if do_iter and hist_iter_name in file:
                    hist_iter = file[hist_iter_name]
                    values_iter, edges_iter = hist_iter.to_numpy()
                    hists.append((values_iter, "Iterative Bkg Sub", "blue"))

                if do_mult and hist_mult_name in file:
                    hist_mult = file[hist_mult_name]
                    values_mult, edges_mult = hist_mult.to_numpy()
                    if edges_iter is None:
                        edges_iter = edges_mult
                    hists.append((values_mult, "Multiplicity Bkg Sub", "crimson"))

                if do_unsub and hist_unsub_name in file:
                    hist_unsub = file[hist_unsub_name]
                    values_unsub, edges_unsub = hist_unsub.to_numpy()
                    if edges_iter is None:
                        edges_iter = edges_unsub
                    hists.append((values_unsub, "Unsubtracted", "green"))

                if not hists or edges_iter is None:
                    continue

                if run_output_dir is not None:
                    output_path = run_output_dir / out_filename
                    make_combined_plot(edges_iter, hists, run_number, output_path, xmax=xmax, ymax=ymax, extra_energy_cut=extra_energy_cut, is_v3=is_v3, r_jet=r_jet)

            if do_iter:
                hist_ff_r02_name = "hJetPtFlowFail_r02_iter"
                hist_ff_r03_name = "hJetPtFlowFail_r03_iter"

                if hist_ff_r02_name in file and hist_ff_r03_name in file:
                    hist_ff_r02 = file[hist_ff_r02_name]
                    values_ff_r02, edges_ff_r02 = hist_ff_r02.to_numpy()

                    hist_ff_r03 = file[hist_ff_r03_name]
                    values_ff_r03, edges_ff_r03 = hist_ff_r03.to_numpy()

                    if run_output_dir is not None:
                        hists_ff = [
                            (values_ff_r02, r"Iterative Bkg Sub ($R = 0.2$)", "blue"),
                            (values_ff_r03, r"Iterative Bkg Sub ($R = 0.3$)", "crimson"),
                        ]

                        output_path = run_output_dir / f"run_{run_number}_flowfail_iter_qa.png"
                        make_combined_plot(
                            edges_ff_r02,
                            hists_ff,
                            run_number,
                            output_path,
                            xmax=xmax,
                            ymax=1e4,
                            extra_energy_cut=True,
                            is_v3=False,
                            r_jet=None,
                            flow_fail=True,
                        )

            eta_hist_pairs = [
                (10.0, "pt10", 10),
                (20.0, "pt20", 20),
            ]

            eta_configs = [
                # r02 plots
                ("h2JetEta_r02_iter", "h2JetEta_r02_mult", "h2JetEta_r02_unsub", False, False, 0.2, ""),
                ("h2JetEtav2_r02_iter", "h2JetEtav2_r02_mult", "h2JetEtav2_r02_unsub", True, False, 0.2, "_v2"),
                ("h2JetEtav3_r02_iter", "h2JetEtav3_r02_mult", "h2JetEtav3_r02_unsub", True, True, 0.2, "_v3"),
                # r03 plots
                ("h2JetEta_r03_iter", "h2JetEta_r03_mult", "h2JetEta_r03_unsub", False, False, 0.3, "_r03"),
                ("h2JetEtav2_r03_iter", "h2JetEtav2_r03_mult", "h2JetEtav2_r03_unsub", True, False, 0.3, "_r03_v2"),
                ("h2JetEtav3_r03_iter", "h2JetEtav3_r03_mult", "h2JetEtav3_r03_unsub", True, True, 0.3, "_r03_v3"),
            ]

            for min_pt_val, pt_suffix, pt_label in eta_hist_pairs:
                for h2_iter_name, h2_mult_name, h2_unsub_name, extra_energy_cut, is_v3, r_jet, label_prefix in eta_configs:
                    hists_eta = []
                    yedges = None

                    if do_iter and h2_iter_name in file:
                        h2_iter = file[h2_iter_name]
                        val_iter, xedges_iter, yedges_i = h2_iter.to_numpy()
                        if yedges is None:
                            yedges = yedges_i
                        idx_pt = np.searchsorted(xedges_iter[:-1], min_pt_val, side='left')
                        if idx_pt >= len(xedges_iter) - 1:
                            idx_pt = 0
                        proj_iter = np.sum(val_iter[idx_pt:, :], axis=0)
                        sum_iter = np.sum(proj_iter)
                        deta = yedges[1] - yedges[0]
                        norm_iter = proj_iter / (sum_iter * deta) if sum_iter > 0 else proj_iter
                        hists_eta.append((norm_iter, "Iterative Bkg Sub", "blue"))

                    if do_mult and h2_mult_name in file:
                        h2_mult = file[h2_mult_name]
                        val_mult, xedges_mult, yedges_m = h2_mult.to_numpy()
                        if yedges is None:
                            yedges = yedges_m
                        idx_pt = np.searchsorted(xedges_mult[:-1], min_pt_val, side='left')
                        if idx_pt >= len(xedges_mult) - 1:
                            idx_pt = 0
                        proj_mult = np.sum(val_mult[idx_pt:, :], axis=0)
                        sum_mult = np.sum(proj_mult)
                        deta = yedges[1] - yedges[0]
                        norm_mult = proj_mult / (sum_mult * deta) if sum_mult > 0 else proj_mult
                        hists_eta.append((norm_mult, "Multiplicity Bkg Sub", "crimson"))

                    if do_unsub and h2_unsub_name in file:
                        h2_unsub = file[h2_unsub_name]
                        val_unsub, xedges_unsub, yedges_u = h2_unsub.to_numpy()
                        if yedges is None:
                            yedges = yedges_u
                        idx_pt = np.searchsorted(xedges_unsub[:-1], min_pt_val, side='left')
                        if idx_pt >= len(xedges_unsub) - 1:
                            idx_pt = 0
                        proj_unsub = np.sum(val_unsub[idx_pt:, :], axis=0)
                        sum_unsub = np.sum(proj_unsub)
                        deta = yedges[1] - yedges[0]
                        norm_unsub = proj_unsub / (sum_unsub * deta) if sum_unsub > 0 else proj_unsub
                        hists_eta.append((norm_unsub, "Unsubtracted", "green"))

                    if not hists_eta or yedges is None:
                        continue

                    if r_jet == 0.2:
                        if label_prefix == "":
                            out_filename = f"run_{run_number}_eta_{pt_suffix}_bkgsub_qa.png"
                        elif label_prefix == "_v2":
                            out_filename = f"run_{run_number}_eta_{pt_suffix}_v2_bkgsub_qa.png"
                        elif label_prefix == "_v3":
                            out_filename = f"run_{run_number}_eta_{pt_suffix}_v3_bkgsub_qa.png"
                    else:
                        if label_prefix == "_r03":
                            out_filename = f"run_{run_number}_r03_eta_{pt_suffix}_bkgsub_qa.png"
                        elif label_prefix == "_r03_v2":
                            out_filename = f"run_{run_number}_r03_eta_{pt_suffix}_v2_bkgsub_qa.png"
                        elif label_prefix == "_r03_v3":
                            out_filename = f"run_{run_number}_r03_eta_{pt_suffix}_v3_bkgsub_qa.png"

                    if run_output_dir is not None:
                        output_path = run_output_dir / out_filename
                        make_combined_plot(
                            yedges,
                            hists_eta,
                            run_number,
                            output_path,
                            ymax=ymax,
                            extra_energy_cut=extra_energy_cut,
                            is_v3=is_v3,
                            r_jet=r_jet,
                            is_eta=True,
                            pt_min_label=pt_label,
                        )

            if do_rcone and run_output_dir is not None:
                rcone_1d_pairs = [
                    ("hRConeEta_r02", "hRConeEta_r03", f"run_{run_number}_rcone_eta_qa.png", r"Random Cone $\eta$", "Counts", (-1.1, 1.1)),
                    ("hRConeEtaNorm_r02", "hRConeEtaNorm_r03", f"run_{run_number}_rcone_eta_norm_qa.png", r"Random Cone $\eta_{norm}$", "Counts", (0, 1)),
                    ("hRConePhi_r02", "hRConePhi_r03", f"run_{run_number}_rcone_phi_qa.png", r"Random Cone $\phi$", "Counts", (0, 2 * np.pi)),
                ]

                for name_r02, name_r03, out_filename, xlabel, ylabel, xlim in rcone_1d_pairs:
                    hists = []
                    edges = None
                    if name_r02 in file:
                        val_r02, edges_r02 = file[name_r02].to_numpy()
                        hists.append((val_r02, "R = 0.2", "blue", "-"))
                        edges = edges_r02
                    if name_r03 in file:
                        val_r03, edges_r03 = file[name_r03].to_numpy()
                        hists.append((val_r03, "R = 0.3", "crimson", "-"))
                        if edges is None: edges = edges_r03

                    if hists and edges is not None:
                        output_path = run_output_dir / out_filename
                        make_generic_overlay(edges, hists, run_number, output_path, xlabel, ylabel, legend_loc='best', xlim=xlim)

                # RCone Energy
                for r_val, color_unsub, color_iter in [("r02", "green", "blue"), ("r03", "green", "blue")]:
                    hists = []
                    edges = None
                    name_unsub = f"hRConeEnergy_unsub_{r_val}"
                    name_iter = f"hRConeEnergy_iter_{r_val}"

                    if do_unsub and name_unsub in file:
                        val_u, edg_u = file[name_unsub].to_numpy()
                        hists.append((val_u, f"Unsubtracted (R = {r_val.replace('r0', '0.')})", color_unsub, "-"))
                        edges = edg_u
                    if do_iter and name_iter in file:
                        val_i, edg_i = file[name_iter].to_numpy()
                        hists.append((val_i, f"Iterative Sub (R = {r_val.replace('r0', '0.')})", color_iter, "-"))
                        if edges is None: edges = edg_i

                    if hists and edges is not None:
                        output_path = run_output_dir / f"run_{run_number}_rcone_energy_{r_val}_qa.png"
                        r_jet_val = float(r_val.replace("r0", "0."))
                        make_generic_overlay(edges, hists, run_number, output_path, r"Random Cone Energy (GeV)", "Counts", logy=True, legend_loc='upper right', r_jet=r_jet_val)

                # RCone Pt v2 vs Jet Pt v2
                rcone_ptv2_ratio_unsub = []
                rcone_ptv2_ratio_iter = []
                ratio_edges = None

                for r_val in ["r02", "r03"]:
                    r_val_float = float(r_val.replace("r0", "0."))
                    area_total = 2.2 * 2.0 * np.pi
                    area_cone = np.pi * (r_val_float ** 2)
                    scale_factor = int(np.floor(area_total / area_cone))

                    if do_unsub:
                        name_jet = f"hJetPtv2_raw_{r_val}_unsub"
                        name_rcone = f"hRConePtv2_unsub_{r_val}"
                        if name_jet in file and name_rcone in file:
                            val_jet, edg_j = file[name_jet].to_numpy()
                            val_rc, edg_r = file[name_rcone].to_numpy()
                            hists_unsub = [
                                (val_jet, "Jet $p_{T}$ Unsubtracted", "red", "-"),
                                (val_rc, "RCone $p_{T}$ Unsubtracted", "blue", "--"),
                                (val_rc * scale_factor, f"Scaled RCone $p_{{T}}$ Unsubtracted ($\\times {scale_factor}$)", "forestgreen", "--"),
                            ]

                            # Ratio calculation (only valid when both jet and rcone have counts > 0)
                            val_rc_scaled = val_rc * scale_factor
                            err_jet = file[name_jet].errors()
                            err_rc_scaled = file[name_rcone].errors() * scale_factor

                            valid = (val_jet > 0) & (val_rc_scaled > 0)
                            ratio_unsub = np.full_like(val_rc, np.nan, dtype=float)
                            ratio_err_unsub = np.full_like(val_rc, np.nan, dtype=float)

                            ratio_unsub[valid] = val_rc_scaled[valid] / val_jet[valid]
                            ratio_err_unsub[valid] = np.sqrt( (err_rc_scaled[valid] / val_jet[valid])**2 + (val_rc_scaled[valid] * err_jet[valid] / (val_jet[valid]**2))**2 )

                            rcone_ptv2_ratio_unsub.append((ratio_unsub, ratio_err_unsub, f"R = {r_val.replace('r0', '0.')} Unsubtracted", "blue" if r_val=="r02" else "crimson"))
                            if ratio_edges is None: ratio_edges = edg_j

                            # Plot unsub overlay
                            output_path = run_output_dir / f"run_{run_number}_rcone_vs_jet_ptv2_raw_{r_val}_unsub_qa.png"
                            r_jet_val = float(r_val.replace("r0", "0."))

                            max_x_val = 0
                            for values, _, _, _ in hists_unsub:
                                nonzero_indices = np.where(values > 0)[0]
                                if len(nonzero_indices) > 0:
                                    max_bin_edge = edg_j[nonzero_indices[-1] + 1]
                                    if max_bin_edge > max_x_val:
                                        max_x_val = max_bin_edge

                            if max_x_val > rcone_pt_max:
                                make_generic_overlay(edg_j, hists_unsub, run_number, output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax='auto', legend_loc='upper right', r_jet=r_jet_val)
                                zoom_output_path = run_output_dir / f"run_{run_number}_rcone_vs_jet_ptv2_raw_{r_val}_unsub_zoom_qa.png"
                                make_generic_overlay(edg_j, hists_unsub, run_number, zoom_output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax=rcone_pt_max, legend_loc='upper right', r_jet=r_jet_val)
                            else:
                                make_generic_overlay(edg_j, hists_unsub, run_number, output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax=rcone_pt_max, legend_loc='upper right', r_jet=r_jet_val)

                    if do_iter:
                        name_jet = f"hJetPtv2_raw_{r_val}_iter"
                        name_rcone = f"hRConePtv2_iter_{r_val}"
                        if name_jet in file and name_rcone in file:
                            val_jet, edg_j = file[name_jet].to_numpy()
                            val_rc, edg_r = file[name_rcone].to_numpy()
                            hists_iter = [
                                (val_jet, "Jet $p_{T}$ Iterative Sub", "red", "-"),
                                (val_rc, "RCone $p_{T}$ Iterative Sub", "blue", "--"),
                                (val_rc * scale_factor, f"Scaled RCone $p_{{T}}$ Iterative Sub ($\\times {scale_factor}$)", "forestgreen", "--"),
                            ]

                            # Ratio calculation (only valid when both jet and rcone have counts > 0)
                            val_rc_scaled = val_rc * scale_factor
                            err_jet = file[name_jet].errors()
                            err_rc_scaled = file[name_rcone].errors() * scale_factor

                            valid = (val_jet > 0) & (val_rc_scaled > 0)
                            ratio_iter = np.full_like(val_rc, np.nan, dtype=float)
                            ratio_err_iter = np.full_like(val_rc, np.nan, dtype=float)

                            ratio_iter[valid] = val_rc_scaled[valid] / val_jet[valid]
                            ratio_err_iter[valid] = np.sqrt( (err_rc_scaled[valid] / val_jet[valid])**2 + (val_rc_scaled[valid] * err_jet[valid] / (val_jet[valid]**2))**2 )

                            rcone_ptv2_ratio_iter.append((ratio_iter, ratio_err_iter, f"R = {r_val.replace('r0', '0.')} Iterative Sub", "blue" if r_val=="r02" else "crimson"))
                            if ratio_edges is None: ratio_edges = edg_j

                            # Plot iter overlay
                            output_path = run_output_dir / f"run_{run_number}_rcone_vs_jet_ptv2_raw_{r_val}_iter_qa.png"
                            r_jet_val = float(r_val.replace("r0", "0."))

                            max_x_val = 0
                            for values, _, _, _ in hists_iter:
                                nonzero_indices = np.where(values > 0)[0]
                                if len(nonzero_indices) > 0:
                                    max_bin_edge = edg_j[nonzero_indices[-1] + 1]
                                    if max_bin_edge > max_x_val:
                                        max_x_val = max_bin_edge

                            if max_x_val > rcone_pt_max:
                                make_generic_overlay(edg_j, hists_iter, run_number, output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax='auto', legend_loc='upper right', r_jet=r_jet_val)
                                zoom_output_path = run_output_dir / f"run_{run_number}_rcone_vs_jet_ptv2_raw_{r_val}_iter_zoom_qa.png"
                                make_generic_overlay(edg_j, hists_iter, run_number, zoom_output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax=rcone_pt_max, legend_loc='upper right', r_jet=r_jet_val)
                            else:
                                make_generic_overlay(edg_j, hists_iter, run_number, output_path, r"Raw $p_{T}$ (GeV)", "Counts", logy=True, xmax=rcone_pt_max, legend_loc='upper right', r_jet=r_jet_val)

                # Ratio plots
                if ratio_edges is not None:
                    if rcone_ptv2_ratio_unsub:
                        output_path = run_output_dir / f"run_{run_number}_rcone_pt_ratio_unsub_qa.png"
                        valid_unsub = [np.nanmax(vals + errs) for vals, errs, _, _ in rcone_ptv2_ratio_unsub if np.any(~np.isnan(vals))]
                        max_y = max(valid_unsub) if valid_unsub else 0
                        ymax_val = 2.0 if max_y > 2.0 else None
                        make_profile_overlay(ratio_edges, rcone_ptv2_ratio_unsub, run_number, output_path, r"Raw $p_{T}$ (GeV)", "RCone (Scaled) / Jet", xmax='auto', ymax=ymax_val, legend_loc='best')

                    if rcone_ptv2_ratio_iter:
                        output_path = run_output_dir / f"run_{run_number}_rcone_pt_ratio_iter_qa.png"
                        valid_iter = [np.nanmax(vals + errs) for vals, errs, _, _ in rcone_ptv2_ratio_iter if np.any(~np.isnan(vals))]
                        max_y = max(valid_iter) if valid_iter else 0
                        ymax_val = 2.0 if max_y > 2.0 else None
                        make_profile_overlay(ratio_edges, rcone_ptv2_ratio_iter, run_number, output_path, r"Raw $p_{T}$ (GeV)", "RCone (Scaled) / Jet", xmax='auto', ymax=ymax_val, legend_loc='best')

                # Profiles
                for r_val, color_unsub, color_iter in [("r02", "green", "blue"), ("r03", "green", "blue")]:
                    profiles = []
                    edges = None
                    name_unsub = f"pRConeEtaPt_unsub_{r_val}"
                    name_iter = f"pRConeEtaPt_iter_{r_val}"

                    if do_unsub and name_unsub in file:
                        prof_u = file[name_unsub]
                        val_u = np.copy(prof_u.values())
                        edg_u = prof_u.axis().edges()
                        err_u = np.copy(prof_u.errors())
                        if hasattr(prof_u, "counts"):
                            try:
                                cnt_u = prof_u.counts(flow=False)
                            except TypeError:
                                cnt_u = prof_u.counts()
                                if len(cnt_u) == len(val_u) + 2:
                                    cnt_u = cnt_u[1:-1]
                            val_u[cnt_u == 0] = np.nan
                            err_u[cnt_u == 0] = np.nan
                        else:
                            empty = (val_u == 0) & (err_u == 0)
                            val_u[empty] = np.nan
                            err_u[empty] = np.nan
                        profiles.append((val_u, err_u, f"Unsubtracted (R = {r_val.replace('r0', '0.')})", color_unsub))
                        edges = edg_u
                    if do_iter and name_iter in file:
                        prof_i = file[name_iter]
                        val_i = np.copy(prof_i.values())
                        edg_i = prof_i.axis().edges()
                        err_i = np.copy(prof_i.errors())
                        if hasattr(prof_i, "counts"):
                            try:
                                cnt_i = prof_i.counts(flow=False)
                            except TypeError:
                                cnt_i = prof_i.counts()
                                if len(cnt_i) == len(val_i) + 2:
                                    cnt_i = cnt_i[1:-1]
                            val_i[cnt_i == 0] = np.nan
                            err_i[cnt_i == 0] = np.nan
                        else:
                            empty = (val_i == 0) & (err_i == 0)
                            val_i[empty] = np.nan
                            err_i[empty] = np.nan
                        profiles.append((val_i, err_i, f"Iterative Sub (R = {r_val.replace('r0', '0.')})", color_iter))
                        if edges is None: edges = edg_i

                    if profiles and edges is not None:
                        output_path = run_output_dir / f"run_{run_number}_rcone_eta_pt_profile_{r_val}_qa.png"
                        r_jet_val = float(r_val.replace("r0", "0."))
                        make_profile_overlay(edges, profiles, run_number, output_path, r"Random Cone $\eta$", r"Average $p_{T}$ (GeV)", legend_loc='best', xlim=(-1.1, 1.1), ymin=-1, add_hline_zero=True, r_jet=r_jet_val)

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
                "h2JetEta_r02_iter",
                "h2JetEta_r02_mult",
                "h2JetEta_r02_unsub",
                "h2JetEta_r03_iter",
                "h2JetEta_r03_mult",
                "h2JetEta_r03_unsub",
                "h2JetEtav2_r02_iter",
                "h2JetEtav2_r02_mult",
                "h2JetEtav2_r02_unsub",
                "h2JetEtav2_r03_iter",
                "h2JetEtav2_r03_mult",
                "h2JetEtav2_r03_unsub",
                "h2JetEtav3_r02_iter",
                "h2JetEtav3_r02_mult",
                "h2JetEtav3_r02_unsub",
                "h2JetEtav3_r03_iter",
                "h2JetEtav3_r03_mult",
                "h2JetEtav3_r03_unsub",
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

            filtered_h2_names = []
            for name in h2_names:
                if "_iter" in name and not do_iter: continue
                if "_mult" in name and not do_mult: continue
                if "_unsub" in name and not do_unsub: continue
                filtered_h2_names.append(name)

            if do_rcone:
                filtered_h2_names.extend(["h2RConeEtaZvtx_r02", "h2RConeEtaZvtx_r03"])

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

            for h2_name in filtered_h2_names:
                if h2_name not in file:
                    print(f"Warning: {h2_name} not found in {path}")
                    continue

                hist2d = file[h2_name]
                if run_output_dir is not None:
                    output_path = run_output_dir / f"run_{run_number}_{h2_name}.png"
                    xlim_left = 0.0 if h2_name in h2_start_zero else None
                    ylim_bottom = 0.0 if h2_name in h2_start_zero_y else None
                    ylim_top = None

                    if any(key in h2_name for key in ["h2JetEta", "h2JetEtav2", "h2JetEtav3"]):
                        if "_r03_" in h2_name:
                            ylim_bottom = -0.8
                            ylim_top = 0.8
                        else:
                            ylim_bottom = -0.9
                            ylim_top = 0.9
                    elif "h2RConeEtaZvtx" in h2_name:
                        ylim_bottom = -1.1
                        ylim_top = 1.1

                    extra_label = calo_cut_label if h2_name == "h2CaloECentrality" else None
                    make_2d_plot(hist2d, run_number, output_path, hist_name=h2_name, xlim_left=xlim_left, ylim_bottom=ylim_bottom, ylim_top=ylim_top, extra_label=extra_label)

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

            filtered_h1_configs = []
            for cfg in h1_configs:
                if "_iter" in cfg[0] and not do_iter: continue
                if "_mult" in cfg[0] and not do_mult: continue
                filtered_h1_configs.append(cfg)

            for h1_name, extra_labels, logy in filtered_h1_configs:
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
        traceback.print_exc()
        return f"Error processing {path}: {e}"

def main():
    parser = argparse.ArgumentParser(description="Plot Jet Pt QA for iter and mult bkg subtraction per run.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("--do-iter", type=int, default=1, help="Process iter bkg sub (1=True, 0=False). Default: 1")
    parser.add_argument("--do-mult", type=int, default=1, help="Process mult bkg sub (1=True, 0=False). Default: 1")
    parser.add_argument("--do-unsub", type=int, default=1, help="Process unsubtracted (1=True, 0=False). Default: 1")
    parser.add_argument("--do-rcone", type=int, default=1, help="Process random cones (1=True, 0=False). Default: 1")
    parser.add_argument("--rcone-pt-max", type=float, default=80, help="Maximum x-axis range for random cone pT plots (and zoom threshold). Default: 80")
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
        do_iter=bool(args.do_iter),
        do_mult=bool(args.do_mult),
        do_unsub=bool(args.do_unsub),
        do_rcone=bool(args.do_rcone),
        rcone_pt_max=args.rcone_pt_max,
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
