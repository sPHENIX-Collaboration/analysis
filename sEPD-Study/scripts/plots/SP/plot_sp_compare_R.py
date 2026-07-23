#!/usr/bin/env python3

import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import mplhep as hep
import numpy as np
import argparse
from pathlib import Path
import sys
import traceback
import logging

logging.getLogger('matplotlib.mathtext').setLevel(logging.ERROR)

from plot_sp_qa import (
    rebin_2d,
    get_hist_titles,
    plot_centrality_projections,
    plot_ref_flow,
    calc_ratio_and_errors,
    format_angled_brackets
)

def process_file_scalar_product(file_path):
    pt_ranges = [(20, 25), (25, 30), (30, 35), (35, 40), (40, 45), (45, 50)]
    results = {'sp_data': [], 'ref_flow': None, 'v2_data': []}

    with uproot.open(file_path) as file:
        for i, (pt_low, pt_high) in enumerate(pt_ranges):
            hist_name = f"h2ScalarProductv1_2_pt_{pt_low}_{pt_high}"
            if hist_name not in file:
                print(f"Warning: Histogram '{hist_name}' not found in {file_path}")
                results['sp_data'].append(None)
                continue

            hist = file[hist_name]
            values, xedges, yedges = hist.to_numpy()
            if hasattr(hist, "variances") and hist.variances() is not None:
                variances = hist.variances()
            else:
                variances = np.copy(values)

            values = np.nan_to_num(values, nan=0.0, posinf=0.0, neginf=0.0)
            variances = np.nan_to_num(variances, nan=0.0, posinf=0.0, neginf=0.0)

            values, variances, xedges, yedges = rebin_2d(values, variances, xedges, yedges, rebin_factor=64)
            hist_title, xlabel, ylabel = get_hist_titles(hist, hist_name)

            cent_centers, cent_xerrs, arith_means, arith_errs, gauss_means, gauss_errs = plot_centrality_projections(
                values, variances, xedges, yedges, hist_title, ylabel, ".", hist_name, save_pdf=False, make_plots=False
            )

            results['sp_data'].append({
                'pt_range': (pt_low, pt_high),
                'cent_centers': cent_centers,
                'cent_xerrs': cent_xerrs,
                'arith_means': arith_means,
                'arith_errs': arith_errs,
                'gauss_means': gauss_means,
                'gauss_errs': gauss_errs,
                'hist_title': hist_title,
                'ylabel': ylabel
            })

        ref_res = plot_ref_flow(file, ".", hist_name="h2RefFlow_2", save_pdf=False, make_plots=False)
        results['ref_flow'] = ref_res

        if ref_res is not None:
            r_cent_centers, r_cent_xerrs, r_arith_m, r_arith_e, r_gauss_m, r_gauss_e = ref_res
            for sp in results['sp_data']:
                if sp is None:
                    results['v2_data'].append(None)
                    continue

                v2_arith, v2_arith_e = calc_ratio_and_errors(sp['arith_means'], sp['arith_errs'], r_arith_m, r_arith_e)
                v2_gauss, v2_gauss_e = calc_ratio_and_errors(sp['gauss_means'], sp['gauss_errs'], r_gauss_m, r_gauss_e)

                results['v2_data'].append({
                    'pt_range': sp['pt_range'],
                    'cent_centers': sp['cent_centers'],
                    'cent_xerrs': sp['cent_xerrs'],
                    'v2_arith': v2_arith,
                    'v2_arith_e': v2_arith_e,
                    'v2_gauss': v2_gauss,
                    'v2_gauss_e': v2_gauss_e,
                    'hist_title': sp['hist_title']
                })

    return results

def plot_comparison_ax(ax, data1, data2, label1, label2, value_key, err_key, title, ylabel, y_limits=None):
    if data1 is not None:
        c1 = np.array(data1['cent_centers'])
        cx1 = np.array(data1['cent_xerrs'])
        y1 = np.array(data1[value_key])
        ye1 = np.array(data1[err_key])
        valid1 = ~np.isnan(y1) & ~np.isnan(ye1)
        if np.any(valid1):
            ax.errorbar(
                c1[valid1], y1[valid1], yerr=ye1[valid1], xerr=cx1[valid1],
                fmt='o', color='blue', linewidth=1.5, markersize=7, capsize=3, label=label1
            )

    if data2 is not None:
        c2 = np.array(data2['cent_centers'])
        cx2 = np.array(data2['cent_xerrs'])
        y2 = np.array(data2[value_key])
        ye2 = np.array(data2[err_key])
        valid2 = ~np.isnan(y2) & ~np.isnan(ye2)
        if np.any(valid2):
            ax.errorbar(
                c2[valid2], y2[valid2], yerr=ye2[valid2], xerr=cx2[valid2],
                fmt='s', color='red', mfc='none', mec='red', mew=1.5, linewidth=1.5, markersize=7, capsize=3, label=label2
            )

    ax.set_xlabel("Centrality [%]")
    ax.set_ylabel(ylabel)
    ax.set_title(title, fontsize=22)
    ax.set_xlim(0, 60)

    if y_limits:
        ax.set_ylim(y_limits)

    ax.legend(loc='best', frameon=False, fontsize=18)

def main():
    parser = argparse.ArgumentParser(description="Compare Scalar Product QA between R=0.2 and R=0.3.")
    parser.add_argument("--file_r02", type=Path, required=True, help="Path to R=0.2 ROOT file")
    parser.add_argument("--file_r03", type=Path, required=True, help="Path to R=0.3 ROOT file")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Output directory.")
    parser.add_argument("--save-pdf", action="store_true", help="Save PDF in addition to PNG.")
    args = parser.parse_args()

    if not args.file_r02.exists():
        print(f"Error: {args.file_r02} not found.")
        sys.exit(1)
    if not args.file_r03.exists():
        print(f"Error: {args.file_r03} not found.")
        sys.exit(1)

    hep.style.use("ATLAS")

    res02 = process_file_scalar_product(args.file_r02)
    res03 = process_file_scalar_product(args.file_r03)

    output_dir = Path(args.output_dir)
    pdf_dir = output_dir / "pdf"
    image_dir = output_dir / "images"
    pdf_dir.mkdir(parents=True, exist_ok=True)
    image_dir.mkdir(parents=True, exist_ok=True)

    pt_ranges = [(20, 25), (25, 30), (30, 35), (35, 40), (40, 45), (45, 50)]

    def calculate_shared_ylim(res1, res2, list_key, value_keys, is_v2=False):
        if isinstance(value_keys, str):
            value_keys = [value_keys]
        all_vals = []
        for i in range(len(pt_ranges)):
            for vk in value_keys:
                if i < len(res1[list_key]) and res1[list_key][i] is not None:
                    all_vals.extend(res1[list_key][i][vk])
                if i < len(res2[list_key]) and res2[list_key][i] is not None:
                    all_vals.extend(res2[list_key][i][vk])

        all_vals = [v for v in all_vals if not np.isnan(v) and not np.isinf(v)]
        if not all_vals:
            return (0, 0.1)

        min_v = min(all_vals)
        max_v = max(all_vals)

        if min_v >= 0:
            if is_v2:
                return (0, max(max_v * 1.3, 0.05))
            else:
                return (0, max(max_v * 1.4, 1e-4))
        else:
            span = max_v - min_v
            if is_v2:
                return (min_v - 0.2 * span, max_v + 0.3 * span)
            else:
                return (min_v - 0.2 * span, max_v + 0.4 * span)

    figs = {
        'SP_Gaussian': plt.subplots(2, 3, figsize=(24, 14)),
        'SP_Arithmetic': plt.subplots(2, 3, figsize=(24, 14)),
        'v2_Gaussian': plt.subplots(2, 3, figsize=(24, 14)),
        'v2_Arithmetic': plt.subplots(2, 3, figsize=(24, 14))
    }

    sp_ylim = calculate_shared_ylim(res02, res03, 'sp_data', ['gauss_means', 'arith_means'], is_v2=False)
    v2_ylim = calculate_shared_ylim(res02, res03, 'v2_data', ['v2_gauss', 'v2_arith'], is_v2=True)

    ylims = {
        'SP_Gaussian': sp_ylim,
        'SP_Arithmetic': sp_ylim,
        'v2_Gaussian': v2_ylim,
        'v2_Arithmetic': v2_ylim
    }

    for i, _ in enumerate(pt_ranges):
        sp02 = res02['sp_data'][i] if i < len(res02['sp_data']) else None
        sp03 = res03['sp_data'][i] if i < len(res03['sp_data']) else None

        v202 = res02['v2_data'][i] if i < len(res02['v2_data']) else None
        v203 = res03['v2_data'][i] if i < len(res03['v2_data']) else None

        title = sp02['hist_title'] if sp02 else (sp03['hist_title'] if sp03 else f"Pt bin {i}")
        ylabel = sp02['ylabel'] if sp02 else (sp03['ylabel'] if sp03 else "Scalar Product")

        sp_ylabel = format_angled_brackets(ylabel)
        v2_ylabel = r"$v_{2}\{\mathrm{SP}\}$"

        plot_comparison_ax(figs['SP_Gaussian'][1].flatten()[i], sp02, sp03, "R = 0.2", "R = 0.3", "gauss_means", "gauss_errs", title, sp_ylabel, ylims['SP_Gaussian'])
        plot_comparison_ax(figs['SP_Arithmetic'][1].flatten()[i], sp02, sp03, "R = 0.2", "R = 0.3", "arith_means", "arith_errs", title, sp_ylabel, ylims['SP_Arithmetic'])

        plot_comparison_ax(figs['v2_Gaussian'][1].flatten()[i], v202, v203, "R = 0.2", "R = 0.3", "v2_gauss", "v2_gauss_e", title, v2_ylabel, ylims['v2_Gaussian'])
        plot_comparison_ax(figs['v2_Arithmetic'][1].flatten()[i], v202, v203, "R = 0.2", "R = 0.3", "v2_arith", "v2_arith_e", title, v2_ylabel, ylims['v2_Arithmetic'])

    for name, (fig, _) in figs.items():
        if "SP" in name:
            fig.suptitle(f"Scalar Product Numerator ({name.split('_')[1]}) Comparison", fontsize=22)
        else:
            fig.suptitle(f"Jet v2 ({name.split('_')[1]}) Comparison", fontsize=22)

        fig.tight_layout()
        fig.subplots_adjust(top=0.92)

        png_path = image_dir / f"{name}_Comparison.png"
        fig.savefig(png_path, dpi=300)
        if args.save_pdf:
            pdf_path = pdf_dir / f"{name}_Comparison.pdf"
            fig.savefig(pdf_path)

        print(f"Saved {name} comparison plots to {png_path}")
        plt.close(fig)

if __name__ == "__main__":
    main()
