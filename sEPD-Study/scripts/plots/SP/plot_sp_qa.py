#!/usr/bin/env python3

import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.colors import LogNorm
import mplhep as hep
import os
import argparse
from pathlib import Path
import numpy as np
import sys
import traceback
import logging

# Suppress harmless mathtext Unicode mapping warnings for tall radical symbols
logging.getLogger('matplotlib.mathtext').setLevel(logging.ERROR)

from mpl_toolkits.axes_grid1 import make_axes_locatable
from scipy.optimize import curve_fit
import re

def clean_and_format_latex(text):
    if not text:
        return ""
    text = text.strip()

    # Replace ROOT TLatex # with \ (converts #leq to \leq, #pi to \pi, etc.)
    text = text.replace("#", "\\")

    # If already contains math mode $, return stripped
    if "$" in text:
        return text.strip()

    # Handle Re(...) expression for Scalar Product
    if "Re(" in text and ("_{" in text or "^{" in text):
        text = text.replace("Re(", r"\mathrm{Re}(")
        return f"${text}$"

    # Handle general LaTeX math fragments in titles/labels
    if any(kw in text for kw in ["\\leq", "_{", "^{"]):
        # Wrap \leq in math mode $\leq$
        text = text.replace("\\leq", r"$\leq$")
        # Match words with subscripts/superscripts like p_{T}, q_{2}, Q^{S|N*}_{2}
        text = re.sub(r'([a-zA-Z0-9_*|()]+(?:_{[^}]+}|^{[^}]+})+)', r'$\1$', text)
        # Clean double $$ if created
        text = text.replace("$$", "$")
        return text.strip()

    return text.strip()

def get_hist_titles(hist, hist_name):
    raw_title = ""
    xlabel = ""
    ylabel = ""

    # 1. Try uproot all_members dict
    if hasattr(hist, "all_members"):
        members = hist.all_members
        raw_title = members.get("fTitle", "").strip()

        # X axis
        fXaxis = members.get("fXaxis")
        if fXaxis is not None:
            if hasattr(fXaxis, "all_members"):
                xlabel = fXaxis.all_members.get("fTitle", "").strip()
            elif hasattr(fXaxis, "member"):
                try:
                    xlabel = fXaxis.member("fTitle").strip()
                except Exception:
                    pass

        # Y axis
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
                val = getattr(hist.axis(0), attr, None)
                if val:
                    xlabel = str(val).strip()
                    break
            except Exception:
                pass

    if not ylabel:
        for attr in ["label", "title"]:
            try:
                val = getattr(hist.axis(1), attr, None)
                if val:
                    ylabel = str(val).strip()
                    break
            except Exception:
                pass

    # 3. Fallback to getattr title or raw_title
    if not raw_title:
        raw_title = (getattr(hist, "title", "") or hist_name).strip()

    # Check if raw_title is in ROOT "MainTitle;XTitle;YTitle" format
    if ";" in raw_title:
        parts = [p.strip() for p in raw_title.split(";")]
        title = parts[0] if parts[0] else hist_name
        if not xlabel and len(parts) > 1:
            xlabel = parts[1]
        if not ylabel and len(parts) > 2:
            ylabel = parts[2]
    else:
        title = raw_title.strip()

    # Fallback to standard Scalar Product QA labels if empty
    if not xlabel:
        xlabel = "Centrality [%]"
    if not ylabel:
        ylabel = r"$\mathrm{Re}(q_{2} Q^{S|N*}_{2})$"
    if not title:
        title = hist_name

    clean_title = clean_and_format_latex(title)
    clean_x = clean_and_format_latex(xlabel)
    clean_y = clean_and_format_latex(ylabel)

    print(f"Extracted Histogram Title: '{clean_title}'")
    print(f"Extracted X-axis Title:    '{clean_x}'")
    print(f"Extracted Y-axis Title:    '{clean_y}'")

    return clean_title, clean_x, clean_y

def rebin_2d(values, variances, xedges, yedges, rebin_factor=64):
    nx, ny = values.shape

    # Rebin Y-axis only
    if ny >= rebin_factor:
        ny_new = ny // rebin_factor
        ny_trimmed = ny_new * rebin_factor
        values = values[:, :ny_trimmed]
        variances = variances[:, :ny_trimmed]

        values = values.reshape(nx, ny_new, rebin_factor).sum(axis=2)
        variances = variances.reshape(nx, ny_new, rebin_factor).sum(axis=2)

        yedges = yedges[:ny_trimmed + 1:rebin_factor]

    return values, variances, xedges, yedges

def gaussian(x, amp, mean, sigma):
    return amp * np.exp(-0.5 * ((x - mean) / sigma)**2)

def format_angled_brackets(text):
    if not text:
        return r"$\langle \mathrm{Mean} \rangle$"
    text = text.strip()
    if text.startswith("$") and text.endswith("$"):
        inner = text[1:-1].strip()
        return f"$\\langle {inner} \\rangle$"
    else:
        return f"$\\langle \\mathrm{{{text}}} \\rangle$"

def calc_sqrt_means_and_errors(means, errs):
    means_arr = np.array(means, dtype=float)
    errs_arr = np.array(errs, dtype=float)

    sqrt_means = np.full_like(means_arr, np.nan)
    sqrt_errs = np.full_like(errs_arr, np.nan)

    valid = (means_arr > 0) & ~np.isnan(means_arr) & ~np.isnan(errs_arr)
    sqrt_means[valid] = np.sqrt(means_arr[valid])
    sqrt_errs[valid] = errs_arr[valid] / (2.0 * sqrt_means[valid])

    return sqrt_means, sqrt_errs

def calc_ratio_and_errors(y_means, y_errs, ref_means, ref_errs):
    y_m = np.array(y_means, dtype=float)
    y_e = np.array(y_errs, dtype=float)
    r_m = np.array(ref_means, dtype=float)
    r_e = np.array(ref_errs, dtype=float)

    ratio = np.full_like(y_m, np.nan)
    ratio_err = np.full_like(y_e, np.nan)

    valid = (r_m > 0) & ~np.isnan(y_m) & ~np.isnan(y_e) & ~np.isnan(r_m) & ~np.isnan(r_e)

    ratio[valid] = y_m[valid] / r_m[valid]

    term_y = np.zeros_like(y_m)
    term_r = np.zeros_like(r_m)

    non_zero_y = valid & (y_m != 0)
    term_y[non_zero_y] = (y_e[non_zero_y] / y_m[non_zero_y])**2
    term_r[valid] = (r_e[valid] / r_m[valid])**2

    ratio_err[valid] = np.abs(ratio[valid]) * np.sqrt(term_y[valid] + term_r[valid])

    return ratio, ratio_err

def plot_means_vs_centrality_ax(ax, cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs, hist_title, ylabel, y_limits=None, custom_ylabel=None):
    if custom_ylabel:
        y_mean_label = custom_ylabel
    else:
        y_mean_label = format_angled_brackets(ylabel if ylabel else "Scalar Product")

    c_centers = np.array(cent_centers, dtype=float)
    c_xerrs = np.array(cent_xerrs, dtype=float)
    a_means = np.array(arith_means, dtype=float)
    a_errs = np.array(arith_errs, dtype=float)
    g_means = np.array(gauss_means, dtype=float)
    g_errs = np.array(gauss_errs, dtype=float)

    # Plot Gaussian fit means (mask NaN points)
    g_valid = ~np.isnan(g_means) & ~np.isnan(g_errs)
    if np.any(g_valid):
        ax.errorbar(
            c_centers[g_valid],
            g_means[g_valid],
            yerr=g_errs[g_valid],
            xerr=c_xerrs[g_valid],
            fmt='o',
            color='red',
            linewidth=1.5,
            markersize=7,
            capsize=3,
            label='Gaussian Fit Mean'
        )

    # Plot Arithmetic means (mask NaN points)
    a_valid = ~np.isnan(a_means) & ~np.isnan(a_errs)
    if np.any(a_valid):
        ax.errorbar(
            c_centers[a_valid],
            a_means[a_valid],
            yerr=a_errs[a_valid],
            xerr=c_xerrs[a_valid],
            fmt='s',
            color='blue',
            mfc='none',
            mec='blue',
            mew=1.5,
            linewidth=1.5,
            markersize=7,
            capsize=3,
            label='Arithmetic Mean'
        )

    ax.set_xlabel("Centrality [%]")
    ax.set_ylabel(y_mean_label)
    ax.set_title(hist_title, fontsize=22)
    ax.set_xlim(0, 60)

    if y_limits is not None and len(y_limits) == 2:
        ymin, ymax = float(y_limits[0]), float(y_limits[1])
        if np.isnan(ymin) or np.isnan(ymax) or abs(ymax - ymin) < 1e-12:
            ymin, ymax = 0.0, 1e-3
        ax.set_ylim(ymin, ymax)
    else:
        # Dynamic Y limit with headroom
        all_means = [m for m in (list(arith_means) + list(gauss_means)) if not np.isnan(m) and not np.isinf(m)]
        if all_means:
            max_m = float(max(all_means))
            min_m = float(min(all_means))
            if abs(max_m - min_m) < 1e-12:
                if min_m >= 0:
                    ax.set_ylim(0.0, max(max_m * 1.4, 1e-3))
                else:
                    ax.set_ylim(min_m - 0.1, min_m + 0.1)
            elif min_m >= 0:
                ax.set_ylim(0.0, max(max_m * 1.4, 1e-4))
            else:
                span = max_m - min_m
                ax.set_ylim(min_m - 0.2 * span, max_m + 0.4 * span)
        else:
            ax.set_ylim(0.0, 8e-3)

    ax.legend(loc='best', frameon=False, fontsize=18)

def plot_ref_flow(file, output_dir, hist_name="h2RefFlow_2", save_pdf=False, make_plots=True):
    if make_plots:
        hep.style.use("ATLAS")

    if hist_name not in file:
        print(f"Warning: Histogram '{hist_name}' not found.")
        return

    hist = file[hist_name]
    values, xedges, yedges = hist.to_numpy()
    if hasattr(hist, "variances") and hist.variances() is not None:
        variances = hist.variances()
    else:
        variances = np.copy(values)

    values = np.nan_to_num(values, nan=0.0, posinf=0.0, neginf=0.0)
    variances = np.nan_to_num(variances, nan=0.0, posinf=0.0, neginf=0.0)

    # Rebin 2D histogram by a factor of 64
    values, variances, xedges, yedges = rebin_2d(values, variances, xedges, yedges, rebin_factor=64)

    # Retrieve titles/labels
    hist_title, xlabel, ylabel = get_hist_titles(hist, hist_name)
    if not ylabel or ylabel == "Y-axis":
        ylabel = r"$\mathrm{Re}(Q^{S}_{2} Q^{N*}_{2})$"

    if make_plots:
        output_dir = Path(output_dir)
        pdf_dir = output_dir / "pdf"
        image_dir = output_dir / "images"
        pdf_dir.mkdir(parents=True, exist_ok=True)
        image_dir.mkdir(parents=True, exist_ok=True)

    # 1. --- Plot 2D ---
    if make_plots:
        fig_2d, ax_2d = plt.subplots(figsize=(10, 8))
        values_masked = np.ma.masked_where(values <= 0, values)
        mesh = ax_2d.pcolormesh(xedges, yedges, values_masked.T, norm=LogNorm(), cmap='viridis', rasterized=True)

        divider = make_axes_locatable(ax_2d)
        cax = divider.append_axes("right", size="5%", pad=0.05)
        cbar = fig_2d.colorbar(mesh, cax=cax)
        cbar.set_label("Counts")

        hist_title = "sEPD North-South Correlation"

        ax_2d.set_xlabel(xlabel if xlabel else "Centrality [%]")
        ax_2d.set_ylabel(ylabel)
        ax_2d.set_title(hist_title, fontsize=22)
        ax_2d.set_xlim(left=-0.5)


        fig_2d.tight_layout()
        pdf_path_2d = pdf_dir / f"{hist_name}_2D.pdf"
        png_path_2d = image_dir / f"{hist_name}_2D.png"
        if save_pdf:
            fig_2d.savefig(pdf_path_2d)
        fig_2d.savefig(png_path_2d, dpi=300)
        plt.close(fig_2d)
        if save_pdf:
            print(f"Saved 2D plot for {hist_name} to {pdf_path_2d} and {png_path_2d}")
        else:
            print(f"Saved 2D plot for {hist_name} to {png_path_2d}")

    # 2. --- 1D Centrality Projections ---
    cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs = plot_centrality_projections(
        values, variances, xedges, yedges, hist_title, ylabel, output_dir, hist_name, save_pdf=save_pdf, make_plots=make_plots
    )

    # 3. --- Means vs Centrality Plot ---
    if make_plots:
        fig_means, ax_means = plt.subplots(figsize=(10, 8))
        plot_means_vs_centrality_ax(
            ax_means, cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs, hist_title, ylabel
        )
        fig_means.tight_layout()
        pdf_path_means = pdf_dir / f"{hist_name}_Means.pdf"
        png_path_means = image_dir / f"{hist_name}_Means.png"
        if save_pdf:
            fig_means.savefig(pdf_path_means)
        fig_means.savefig(png_path_means, dpi=300)
        plt.close(fig_means)
        if save_pdf:
            print(f"Saved Means plot for {hist_name} to {pdf_path_means} and {png_path_means}")
        else:
            print(f"Saved Means plot for {hist_name} to {png_path_means}")

    # 4. --- Sqrt(Means) vs Centrality Plot ---
    sqrt_arith_means, sqrt_arith_errs = calc_sqrt_means_and_errors(arith_means, arith_errs)
    sqrt_gauss_means, sqrt_gauss_errs = calc_sqrt_means_and_errors(gauss_means, gauss_errs)

    if make_plots:
        fig_sqrt, ax_sqrt = plt.subplots(figsize=(10, 8))
        if ylabel.startswith("$") and ylabel.endswith("$"):
            inner_label = ylabel[1:-1].strip()
            sqrt_ylabel = f"$\\sqrt{{\\langle {inner_label} \\rangle}}$"
        else:
            sqrt_ylabel = f"$\\sqrt{{\\langle \\mathrm{{{ylabel}}} \\rangle}}$"

        hist_title = "Reference Flow"

        plot_means_vs_centrality_ax(
            ax_sqrt, cent_centers, cent_xerrs,
            sqrt_arith_means, sqrt_arith_errs,
            sqrt_gauss_means, sqrt_gauss_errs,
            hist_title, ylabel,
            custom_ylabel=sqrt_ylabel
        )
        fig_sqrt.tight_layout()
        pdf_path_sqrt = pdf_dir / f"{hist_name}_Sqrt_Means.pdf"
        png_path_sqrt = image_dir / f"{hist_name}_Sqrt_Means.png"
        if save_pdf:
            fig_sqrt.savefig(pdf_path_sqrt)
        fig_sqrt.savefig(png_path_sqrt, dpi=300)
        plt.close(fig_sqrt)
        if save_pdf:
            print(f"Saved Sqrt Means plot for {hist_name} to {pdf_path_sqrt} and {png_path_sqrt}")
        else:
            print(f"Saved Sqrt Means plot for {png_path_sqrt}")

    return cent_centers, cent_xerrs, sqrt_arith_means, sqrt_arith_errs, sqrt_gauss_means, sqrt_gauss_errs

def plot_centrality_projections(values, variances, xedges, yedges, hist_title, ylabel, output_dir, hist_name, save_pdf=False, make_plots=True):
    if make_plots:
        hep.style.use("ATLAS")

    # 6 centrality bins of 10% each from 0% to 60%
    cent_ranges = [(0, 10), (10, 20), (20, 30), (30, 40), (40, 50), (50, 60)]

    if make_plots:
        fig, axes = plt.subplots(2, 3, figsize=(18, 10), sharex=True, sharey=True)
        axes_flat = axes.flatten()

    bin_centers_x = (xedges[:-1] + xedges[1:]) / 2.0
    ycenters = (yedges[:-1] + yedges[1:]) / 2.0

    # Pre-calculate 1D projections and variances
    projections = []
    proj_variances = []
    for c_low, c_high in cent_ranges:
        mask = (bin_centers_x >= c_low) & (bin_centers_x < c_high)
        if np.any(mask):
            proj = np.sum(values[mask, :], axis=0)
            proj_var = np.sum(variances[mask, :], axis=0)
        else:
            proj = np.zeros(len(yedges) - 1)
            proj_var = np.zeros(len(yedges) - 1)
        projections.append(proj)
        proj_variances.append(proj_var)

    global_max_counts = max([np.max(p) for p in projections if len(p) > 0] + [10])

    cent_centers = []
    cent_xerrs = []
    arith_means = []
    arith_errs = []
    gauss_means = []
    gauss_errs = []

    for i, (c_low, c_high) in enumerate(cent_ranges):
        proj_1d = projections[i]
        proj_var_1d = proj_variances[i]

        yerr_1d = np.sqrt(np.maximum(proj_var_1d, 0))
        if make_plots:
            ax = axes_flat[i]
            hep.histplot(proj_1d, yedges, yerr=yerr_1d, ax=ax, histtype="errorbar", color="black", marker="o", markersize=3, label="Data")

        c_mid = (c_low + c_high) / 2.0
        c_err = (c_high - c_low) / 2.0
        cent_centers.append(c_mid)
        cent_xerrs.append(c_err)

        # Calculate arithmetic mean and Sumw2 error on the mean
        total_entries = np.sum(proj_1d)
        if total_entries > 0:
            arith_mean = np.sum(proj_1d * ycenters) / total_entries
            # Error on arithmetic mean using Sumw2 variance propagation: sqrt(sum(w2_i * (y_i - mean)^2)) / W_tot
            arith_mean_var = np.sum(proj_var_1d * (ycenters - arith_mean)**2) / (total_entries**2)
            arith_err = np.sqrt(max(arith_mean_var, 0.0))
        else:
            arith_mean = np.nan
            arith_err = np.nan

        arith_means.append(arith_mean)
        arith_errs.append(arith_err)

        # Fit Gaussian to 1D projection using Sumw2 bin uncertainties
        gauss_mean_val = np.nan
        gauss_mean_err = np.nan
        gauss_mean_str = "N/A"
        if total_entries > 0:
            try:
                fit_mask = (proj_1d > 0) & (proj_var_1d > 0)
                if np.sum(fit_mask) >= 3:
                    arith_std = np.sqrt(max(np.sum(proj_1d * (ycenters - arith_mean)**2) / total_entries, 1e-6))
                    p0 = [np.max(proj_1d), arith_mean, arith_std]
                    weights = np.sqrt(proj_var_1d[fit_mask])
                    popt, pcov = curve_fit(gaussian, ycenters[fit_mask], proj_1d[fit_mask], p0=p0, sigma=weights, absolute_sigma=True, maxfev=3000)
                    gauss_amp, gauss_mean, gauss_sigma = popt

                    gauss_mean_val = gauss_mean
                    if pcov is not None and not np.isinf(pcov[1, 1]) and pcov[1, 1] >= 0:
                        gauss_mean_err = np.sqrt(pcov[1, 1])
                    else:
                        gauss_mean_err = 0.0

                    # Draw fitted Gaussian curve (filter points below 0.5 to avoid distorting log y-axis limits)
                    x_fit = np.linspace(yedges[0], yedges[-1], 500)
                    y_fit = gaussian(x_fit, gauss_amp, gauss_mean, gauss_sigma)
                    fit_mask_y = y_fit >= 0.5
                    if make_plots and np.any(fit_mask_y):
                        ax.plot(x_fit[fit_mask_y], y_fit[fit_mask_y], color="red", linestyle="--", lw=1.5, label="Gaussian Fit")

                    gauss_mean_str = f"{gauss_mean:.4e}"
            except Exception:
                gauss_mean_str = "Fit Failed"

        gauss_means.append(gauss_mean_val)
        gauss_errs.append(gauss_mean_err)

        if make_plots:
            ax.set_yscale("log")
            ax.set_xlim(left=yedges[0], right=yedges[-1])

            # Set shared y-axis limit based on global max counts with headroom for text overlay
            ax.set_ylim(bottom=0.5, top=global_max_counts * 15.0)

            # Set subplot title to Centrality range
            ax.set_title(f"Centrality: {c_low}-{c_high}%", fontsize=18)

            # Overlay means text
            text_info = (
                f"Arith. Mean: {arith_mean:.4e}\n"
                f"Gauss. Mean: {gauss_mean_str}"
            )
            ax.text(0.03, 0.95, text_info, transform=ax.transAxes, ha='left', va='top', fontsize=14)

            if i == 0:
                ax.legend(loc='upper right', frameon=False, fontsize=18)

            if i >= 3:
                ax.set_xlabel(ylabel if ylabel else "Scalar Product")
            if i % 3 == 0:
                ax.set_ylabel("Counts")

    if make_plots:
        fig.suptitle(f"{hist_title}", fontsize=18, y=0.98)
        plt.tight_layout()
        plt.subplots_adjust(top=0.92)

        output_dir = Path(output_dir)
        pdf_dir = output_dir / "pdf"
        image_dir = output_dir / "images"
        pdf_dir.mkdir(parents=True, exist_ok=True)
        image_dir.mkdir(parents=True, exist_ok=True)

        pdf_path = pdf_dir / f"{hist_name}_proj_cent.pdf"
        png_path = image_dir / f"{hist_name}_proj_cent.png"

        if save_pdf:
            plt.savefig(pdf_path)
        plt.savefig(png_path, dpi=300)
        plt.close(fig)

        if save_pdf:
            print(f"Saved 1D projections with Gaussian fits to {pdf_path} and {png_path}")
        else:
            print(f"Saved 1D projections with Gaussian fits to {png_path}")

    return cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs

def plot_scalar_product(file_path, output_dir, save_pdf=False):
    hep.style.use("ATLAS")

    pt_ranges = [(20, 25), (25, 30), (30, 35), (35, 40), (40, 45), (45, 50)]

    try:
        with uproot.open(file_path) as file:
            fig_2d, axes_2d = plt.subplots(2, 3, figsize=(24, 14))
            fig_means, axes_means = plt.subplots(2, 3, figsize=(24, 14))
            axes_2d_flat = axes_2d.flatten()
            axes_means_flat = axes_means.flatten()


            means_data_list = []

            for i, (pt_low, pt_high) in enumerate(pt_ranges):
                hist_name = f"h2ScalarProductv1_2_pt_{pt_low}_{pt_high}"
                if hist_name not in file:
                    print(f"Warning: Histogram '{hist_name}' not found in {file_path}")
                    continue

                hist = file[hist_name]
                values, xedges, yedges = hist.to_numpy()
                if hasattr(hist, "variances") and hist.variances() is not None:
                    variances = hist.variances()
                else:
                    variances = np.copy(values)

                values = np.nan_to_num(values, nan=0.0, posinf=0.0, neginf=0.0)
                variances = np.nan_to_num(variances, nan=0.0, posinf=0.0, neginf=0.0)

                # Rebin 2D histogram by a factor of 64
                values, variances, xedges, yedges = rebin_2d(values, variances, xedges, yedges, rebin_factor=64)

                # Retrieve titles/labels from the histogram
                hist_title, xlabel, ylabel = get_hist_titles(hist, hist_name)

                # --- Plot 2D ---
                ax_2d = axes_2d_flat[i]
                values_masked = np.ma.masked_where(values <= 0, values)
                mesh = ax_2d.pcolormesh(xedges, yedges, values_masked.T, norm=LogNorm(), cmap='viridis', rasterized=True)

                divider = make_axes_locatable(ax_2d)
                cax = divider.append_axes("right", size="5%", pad=0.05)
                cbar = fig_2d.colorbar(mesh, cax=cax)
                cbar.set_label("Counts")

                ax_2d.set_xlabel(xlabel if xlabel else "Centrality [%]")
                ax_2d.set_ylabel(ylabel if ylabel else "Y-axis")
                ax_2d.set_title(hist_title if hist_title else hist_name, fontsize=18)
                ax_2d.set_xlim(left=-0.5)


                # --- 1D Projections ---
                cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs = plot_centrality_projections(
                    values, variances, xedges, yedges, hist_title, ylabel, output_dir, hist_name, save_pdf=save_pdf
                )

                means_data_list.append((
                    axes_means_flat[i], cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs, hist_title, ylabel
                ))

            # --- Calculate shared Y-limits for all Means vs Centrality subplots ---
            all_means_combined = []
            for item in means_data_list:
                _, _, _, a_m, _, g_m, _, _, _ = item
                all_means_combined.extend([m for m in (list(a_m) + list(g_m)) if not np.isnan(m) and not np.isinf(m)])

            if all_means_combined:
                min_y = min(all_means_combined)
                max_y = max(all_means_combined)
                if min_y >= 0:
                    shared_ylim = (0, max(max_y * 1.4, 1e-4))
                else:
                    span = max_y - min_y
                    shared_ylim = (min_y - 0.2 * span, max_y + 0.4 * span)
            else:
                shared_ylim = (0, 8e-3)

            # Ensure valid non-NaN shared_ylim
            if np.isnan(shared_ylim[0]) or np.isnan(shared_ylim[1]):
                shared_ylim = (0, 8e-3)

            for i, ax_m in enumerate(axes_means_flat):
                ax_m.set_xlim(0, 60)
                ax_m.set_ylim(shared_ylim)
                if i < len(means_data_list):
                    item = means_data_list[i]
                    _, c_cent, c_xerr, a_m, a_err, g_m, g_err, h_title, y_lbl = item
                    plot_means_vs_centrality_ax(
                        ax_m, c_cent, c_xerr, a_m, a_err, g_m, g_err, h_title, y_lbl, y_limits=shared_ylim
                    )
                else:
                    ax_m.axis('off')

            for i, ax_2d in enumerate(axes_2d_flat):
                if i >= len(means_data_list):
                    ax_2d.axis('off')

            output_dir = Path(output_dir)
            pdf_dir = output_dir / "pdf"
            image_dir = output_dir / "images"
            pdf_dir.mkdir(parents=True, exist_ok=True)
            image_dir.mkdir(parents=True, exist_ok=True)

            # Finalize 2D plots
            fig_2d.suptitle("Scalar Product", fontsize=22)
            fig_2d.tight_layout()
            fig_2d.subplots_adjust(top=0.92)

            pdf_path_2d = pdf_dir / "h2ScalarProduct_2D_all.pdf"
            png_path_2d = image_dir / "h2ScalarProduct_2D_all.png"
            if save_pdf:
                fig_2d.savefig(pdf_path_2d)
            fig_2d.savefig(png_path_2d, dpi=300)
            plt.close(fig_2d)
            if save_pdf:
                print(f"Saved 2D plots to {pdf_path_2d} and {png_path_2d}")
            else:
                print(f"Saved 2D plots to {png_path_2d}")

            # Finalize Means vs Centrality plots
            fig_means.suptitle("Scalar Product Numerator", fontsize=22)
            fig_means.tight_layout()
            fig_means.subplots_adjust(top=0.92)

            pdf_path_means = pdf_dir / "h2ScalarProduct_Means_all.pdf"
            png_path_means = image_dir / "h2ScalarProduct_Means_all.png"
            if save_pdf:
                fig_means.savefig(pdf_path_means)
            fig_means.savefig(png_path_means, dpi=300)
            plt.close(fig_means)
            if save_pdf:
                print(f"Saved Means plots to {pdf_path_means} and {png_path_means}")
            else:
                print(f"Saved Means plots to {png_path_means}")

            # --- Process h2RefFlow_2 ---
            ref_res = plot_ref_flow(file, output_dir, hist_name="h2RefFlow_2", save_pdf=save_pdf)

            # --- Plot Jet v2 (Scalar Product / Sqrt(RefFlow)) ---
            if ref_res is not None:
                r_cent_centers, r_cent_xerrs, r_arith_m, r_arith_e, r_gauss_m, r_gauss_e = ref_res

                fig_v2, axes_v2 = plt.subplots(2, 3, figsize=(24, 14))
                axes_v2_flat = axes_v2.flatten()

                v2_data_list = []
                for item in means_data_list:
                    _, c_cent, c_xerr, a_m, a_err, g_m, g_err, h_title, _ = item

                    v2_arith, v2_arith_e = calc_ratio_and_errors(a_m, a_err, r_arith_m, r_arith_e)
                    v2_gauss, v2_gauss_e = calc_ratio_and_errors(g_m, g_err, r_gauss_m, r_gauss_e)

                    v2_data_list.append((c_cent, c_xerr, v2_arith, v2_arith_e, v2_gauss, v2_gauss_e, h_title))

                # Shared Y-limits for Jet v2 plots
                all_v2_combined = []
                for item in v2_data_list:
                    _, _, a_v, _, g_v, _, _ = item
                    all_v2_combined.extend([v for v in (list(a_v) + list(g_v)) if not np.isnan(v) and not np.isinf(v)])

                if all_v2_combined:
                    min_v2 = min(all_v2_combined)
                    max_v2 = max(all_v2_combined)
                    if min_v2 >= 0:
                        shared_v2_ylim = (0, max(max_v2 * 1.3, 0.05))
                    else:
                        span = max_v2 - min_v2
                        shared_v2_ylim = (min_v2 - 0.2 * span, max_v2 + 0.3 * span)
                else:
                    shared_v2_ylim = (0, 0.1)

                if np.isnan(shared_v2_ylim[0]) or np.isnan(shared_v2_ylim[1]):
                    shared_v2_ylim = (0, 0.1)

                for i, ax_v2 in enumerate(axes_v2_flat):
                    ax_v2.set_xlim(0, 60)
                    ax_v2.set_ylim(shared_v2_ylim)
                    if i < len(v2_data_list):
                        c_cent, c_xerr, a_v, a_ve, g_v, g_ve, h_title = v2_data_list[i]
                        plot_means_vs_centrality_ax(
                            ax_v2, c_cent, c_xerr, a_v, a_ve, g_v, g_ve, h_title, ylabel="",
                            y_limits=shared_v2_ylim, custom_ylabel=r"$v_{2}\{\mathrm{SP}\}$"
                        )
                    else:
                        ax_v2.axis('off')

                fig_v2.suptitle(r"Jet $v_{2}\{\mathrm{SP}\}$", fontsize=22)
                fig_v2.tight_layout()
                fig_v2.subplots_adjust(top=0.92)

                pdf_path_v2 = pdf_dir / "Jet_v2_vs_Centrality.pdf"
                png_path_v2 = image_dir / "Jet_v2_vs_Centrality.png"
                if save_pdf:
                    fig_v2.savefig(pdf_path_v2)
                fig_v2.savefig(png_path_v2, dpi=300)
                plt.close(fig_v2)
                if save_pdf:
                    print(f"Saved Jet v2 plots to {pdf_path_v2} and {png_path_v2}")
                else:
                    print(f"Saved Jet v2 plots to {png_path_v2}")

    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        traceback.print_exc()
        sys.exit(1)

def main():
    parser = argparse.ArgumentParser(description="Plot Scalar Product QA from a ROOT file.")
    parser.add_argument("input_file", type=Path, help="Path to the input ROOT file")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("--save-pdf", action="store_true", help="Enable saving of plots in PDF format (in addition to PNG).")

    args = parser.parse_args()

    if not args.input_file.exists():
        print(f"Error: Input file {args.input_file} does not exist.")
        sys.exit(1)

    plot_scalar_product(args.input_file, args.output_dir, save_pdf=args.save_pdf)

if __name__ == "__main__":
    main()
