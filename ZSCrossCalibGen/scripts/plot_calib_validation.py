#!/usr/bin/env python3
"""
plot_calib_validation.py

Generates validation and QA trend plots for ZSCrossCalib CDBTTrees

Plots organized into subdirectories:
1. trends/ (trends_vs_run_<det>.png & trends_vs_run_<det>.pdf):
   - Two-panel plot: Top panel has mean active ratio vs Run Number for T0 Integrated, Main [-2.5, 2.5] ns, -5 [-7.5, -2.5] ns, +5 [2.5, 7.5] ns.
     Epoch boundaries marked with dashed lines and labels.
   - Bottom panel shows ratio with respect to T0 Integrated for all three sample windows, dynamically scaled to capture all points without legend overlap.
2. dead_towers/ (dead_towers_vs_run_<det>.png):
   - Zero-calibration channel counts across all runs.
3. distributions/ (distributions_epoch<E>_<det>.png):
   - Overlaid 1D step histograms comparing T0 Integrated, Main [-2.5, 2.5] ns, -5 [-7.5, -2.5] ns, and +5 [2.5, 7.5] ns.
4. closure/ (candidate_closure_<det>.png):
   - Verifies ratio_main / QA_hist_main == 1.0 for candidate runs (square aspect ratio).
5. spatial_maps/ (spatial_map_<det>_run<run>_*.png):
   - 2D (eta, phi) heatmaps for candidate/selected runs with square cell aspect ratio.
6. validation/ (when --val-hist-list is provided):
   - val_residuals_<det>.png & .pdf: Combined 1D step histograms comparing residuals (CDB - Hist) for Main, -5 ns, and +5 ns windows across all validation runs.
   - val_ratios_<det>.png & .pdf: Combined 1D step histograms comparing ratios (CDB / Hist) for all three sample windows across all validation runs.
   - val_trend_by_run_<det>.png & .pdf: Categorical validation run trends for mean residuals & ratios with offset timing windows and epoch grouping.
   - val_summary_<det>.csv: Tabular channel statistics per validation run and window.
   - validation/residuals/val_residuals_<det>_run<run>.png: Per-run 1D residual distributions for each validation run.
   - validation/ratios/val_ratios_<det>_run<run>.png: Per-run 1D ratio distributions for each validation run.
   - validation/spatial_maps/spatial_map_val_<det>_run<run>_<win>.png: 2D (eta, phi) relative residual heatmaps [%] across Main, -5 ns, and +5 ns windows.
7. scale_factor_maps/:
   - Scale factor SF_win = H_win / T0 Integrated, built directly from each run's QA hist and its own T0 Integrated (Fratio) value.
   - candidate/sf_map_<det>_run<run>.png: 3 stacked (eta, phi) panels (-5 ns, Main, +5 ns) per candidate run.
   - validation/sf_map_<det>_run<run>.png: Same for each validation run (when --val-hist-list is provided).
     Colour ranges are shared per (detector, window) across all candidate and validation runs.
   - epoch_ratio/sf_ratio_<det>_epoch<E>_val<vrun>_cand<crun>.png: SF_val / SF_cand per window for each epoch pairing.
     Colour ranges are symmetric around 1.0 and shared per (detector, window) across epochs.
     Algebraically equal to Hist / CDB for the validation run when the CDB uses the candidate method, which is
     cross-checked per window. For raw-time fit CDBs (time_fit_runs.csv in --input-dir) only Main is cross-checked.
   - epoch_ratio/sf_ratio_summary.csv: Per (detector, epoch, window) ratio statistics and closure deviation.
8. time_maps/ (disable with --no-time-maps):
   - Mean raw tower time from h_CaloValid_<det>_etaphi_time_raw [ADC samples, template-fit peak shift].
     The segment-0 HIST_JETQA file per run is looked up in FileCatalog, keeping the latest by file time.
   - time_hist_paths.csv: Run, role, production tag and file used.
   - candidate/time_map_run<run>.png & validation/time_map_run<run>.png: Stacked panels, one per detector.
   - epoch_diff/time_diff_epoch<E>_val<vrun>_cand<crun>.png: t_val - t_cand per detector for each epoch pairing.
   - epoch_diff/time_diff_summary.csv: Per (detector, epoch) difference statistics.
9. sf_vs_time/ (requires both scale factor and time maps):
   - Tower-by-tower 2D histograms (log counts) with a mean-y profile overlay, one panel per sample window.
   - candidate/ & validation/sf_vs_time_<det>_run<run>.png: SF_win vs mean raw tower time.
   - epoch_ratio_vs_dt/sf_ratio_vs_dt_<det>_epoch<E>_val<vrun>_cand<crun>.png: SF_val / SF_cand vs t_val - t_cand.
     Axis ranges are shared per (detector, window) across runs, and across epochs for the pairings.
   - sf_vs_time_summary.csv: Pearson r and linear fit (slope, intercept) per plot and window.
   - Candidate plots also show the polynomial fit (order TIME_FIT_ORDER) used by time_fit/ for the ±5 ns windows.
10. time_fit/ (requires scale factor and time maps, and --val-hist-list):
   - ±5 ns calibration from raw tower time: per (detector, epoch, window) a cubic SF(t) is fit on the
     candidate run and applied to each validation run, CDB_fit = f(t_val) * T0_val. Main is unchanged.
   - val_ratios_timefit_<det>.png & per_run/val_ratios_timefit_<det>_run<run>.png: CDB / Hist distributions
     comparing the current method (candidate SF) with the raw-time fit on the same towers.
   - fit_coefficients.csv: Polynomial coefficients p<k> (coefficient of t^k), fit tower count and residual RMS per fit.
   - time_fit_summary.csv: Per (detector, epoch, window, method) ratio statistics and fallback tower count.
"""

import argparse
import concurrent.futures
import csv
import glob
import os
from pathlib import Path
import re
import sys
import time
from typing import Any, Dict, List, Optional, Tuple

import matplotlib
matplotlib.use("Agg")
from matplotlib.colors import LogNorm
from mpl_toolkits.axes_grid1 import make_axes_locatable
import matplotlib.pyplot as plt
import mplhep as hep
import numpy as np
import pandas as pd
import tqdm
import uproot

# Import extraction module and shared raw-time helpers
script_dir = Path(__file__).resolve().parent
if str(script_dir) not in sys.path:
    sys.path.insert(0, str(script_dir))
from extract_calib_summary import extract_summary  # noqa: E402
from raw_time_utils import (  # noqa: E402
    TIME_FIT_ORDER,
    TIME_FIT_WINDOWS,
    TIME_HIST_DSTTYPE,
    fit_sf_vs_time_poly,
    load_time_map,
    predict_sf_from_time,
    query_time_hist_paths,
    time_fit_label,
)

# Set mplhep ATLAS style
hep.style.use(hep.style.ATLAS)

# Calorimeter geometries
DETECTOR_CONFIG = {
    "CEMC": {"neta": 96, "nphi": 256, "hist_det": "cemc"},
    "HCALIN": {"neta": 24, "nphi": 64, "hist_det": "ihcal"},
    "HCALOUT": {"neta": 24, "nphi": 64, "hist_det": "ohcal"},
}

# Epoch colors for visual shading / boundaries
EPOCH_COLORS = [
    "#4e79a7", "#f28e2b", "#e15759", "#76b7b2",
    "#59a14f", "#edc948", "#b07aa1", "#ff9da7"
]

# Sample window configurations for cross-calibration comparison
SAMPLE_WINDOWS = [
    {
        "key": "main",
        "cdb_col": "Fratio_main",
        "hist_suffix": "main",
        "label": "Main [-2.5, 2.5] ns",
        "color": "#1f77b4",
    },
    {
        "key": "minus5",
        "cdb_col": "Fratio_minus5",
        "hist_suffix": "minus5",
        "label": "-5 [-7.5, -2.5] ns",
        "color": "#d62728",
    },
    {
        "key": "plus5",
        "cdb_col": "Fratio_plus5",
        "hist_suffix": "plus5",
        "label": "+5 [2.5, 7.5] ns",
        "color": "#2ca02c",
    },
]
WINDOW_BY_KEY = {w["key"]: w for w in SAMPLE_WINDOWS}

# Top-to-bottom panel order for stacked scale factor figures
SF_PANEL_ORDER = ("minus5", "main", "plus5")

TIME_FIT_LABEL = time_fit_label(TIME_FIT_ORDER)


def parse_arguments() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent
    repo_files_dir = repo_root / "files"

    parser = argparse.ArgumentParser(
        description="Generate validation and QA trend plots for ZSCrossCalib CDBTTrees."
    )
    parser.add_argument(
        "--summary-csv",
        type=Path,
        required=True,
        help="Path to summary CSV (loaded if exists, or generated and saved here by extract_calib_summary)",
    )
    parser.add_argument(
        "--input-dir",
        type=Path,
        required=True,
        help="Directory containing generated CDB ROOT files",
    )
    parser.add_argument(
        "--hist-list",
        type=Path,
        required=True,
        help="Path to candidate hist list file",
    )
    parser.add_argument(
        "--val-hist-list",
        "--validation-hist-list",
        type=Path,
        default=None,
        dest="val_hist_list",
        help="Path to validation/test hist list file to validate CDB calibrations against independent runs (optional)",
    )
    parser.add_argument(
        "--epoch-file",
        type=Path,
        default=repo_files_dir / "epoch.csv",
        help="Path to epoch CSV file (default: ZSCrossCalibGen/files/epoch.csv)",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=repo_root / "scratch" / "plots",
        help="Directory where plots will be saved (default: ZSCrossCalibGen/scratch/plots)",
    )
    parser.add_argument(
        "--detectors",
        type=str,
        default="CEMC,HCALIN,HCALOUT",
        help="Comma-separated list of detectors to plot (default: CEMC,HCALIN,HCALOUT)",
    )
    parser.add_argument(
        "--force-extract",
        action="store_true",
        help="Force re-extraction of calibration summary even if --summary-csv already exists",
    )
    parser.add_argument(
        "--spatial-runs",
        type=str,
        default=None,
        help="Comma-separated list of run numbers to generate 2D spatial maps for (default: candidate runs from hist-list)",
    )
    parser.add_argument(
        "-j",
        "--jobs",
        type=int,
        default=min(os.cpu_count() or 4, 16),
        help="Number of parallel worker jobs for extraction and plotting (default: auto, up to 16)",
    )
    parser.add_argument(
        "--val-ylim-residual",
        type=str,
        default=None,
        help="Custom y-axis limits for validation residual panel in trend plots, formatted as min,max (e.g. -0.15,0.15)",
    )
    parser.add_argument(
        "--val-ylim-ratio",
        type=str,
        default=None,
        help="Custom y-axis limits for validation ratio panel in trend plots, formatted as min,max (e.g. 0.85,1.15)",
    )
    parser.add_argument(
        "--no-time-maps",
        action="store_true",
        help="Skip raw tower time maps (requires FileCatalog access via psycopg2)",
    )
    return parser.parse_args()


def parse_range(range_str: Optional[str]) -> Optional[Tuple[float, float]]:
    """Parse comma-separated min,max range string (e.g. '-0.15,0.15')."""
    if not range_str:
        return None
    try:
        parts = [float(x.strip()) for x in range_str.split(",") if x.strip()]
        if len(parts) != 2:
            raise ValueError("Expected format: min,max (e.g. -0.15,0.15)")
        return (parts[0], parts[1])
    except Exception as err:
        sys.exit(f"Error parsing range '{range_str}': {err}")


def load_epochs(epoch_path: Path) -> List[Tuple[int, int, int]]:
    """Load epoch run ranges from CSV file. Returns list of (run_start, run_end, epoch_idx)."""
    if not epoch_path.is_file():
        sys.exit(f"Error: Epoch file not found: {epoch_path}")

    epochs = []
    with open(epoch_path, "r", newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for idx, row in enumerate(reader, start=1):
            try:
                start = int(row["run_start"].strip())
                end = int(row["run_end"].strip())
                epochs.append((start, end, idx))
            except (KeyError, ValueError) as err:
                sys.exit(f"Error parsing {epoch_path} at line {idx+1}: {err}")
    return epochs


def load_hist_list(hist_list_path: Optional[Path]) -> Dict[int, str]:
    """Load run -> file path from hist list file."""
    runs_map = {}
    if not hist_list_path or not Path(hist_list_path).is_file():
        return runs_map

    with open(hist_list_path, "r", encoding="utf-8") as f:
        for line in f:
            p = line.strip()
            if not p or p.startswith("#"):
                continue
            stem = Path(p).stem
            m = re.search(r"(\d{5,7})", stem)
            if m:
                run = int(m.group(1))
                runs_map[run] = p
    return runs_map


load_candidate_runs = load_hist_list


def get_epoch_for_run(run: int, epochs: List[Tuple[int, int, int]]) -> Optional[int]:
    """Return the epoch index containing run, or None if unassigned."""
    for s, e, ep_idx in epochs:
        if s <= run <= e:
            return ep_idx
    return None


def find_cdb_file(input_dir: Path, det: str, run: int) -> Optional[Path]:
    """Find CDB file for (det, run) allowing flexible tag naming."""
    exact = input_dir / f"{det}_ZSCrossCalibT0_pro001_pcdb001_v001_{run}.root"
    if exact.is_file():
        return exact
    matches = list(input_dir.glob(f"{det}_ZSCrossCalibT0_*_{run}.root"))
    if matches:
        return matches[0]
    return None


def render_trend_figure(
    det: str,
    runs: np.ndarray,
    orig: np.ndarray,
    main: np.ndarray,
    minus5: np.ndarray,
    plus5: np.ndarray,
    epochs: List[Tuple[int, int, int]],
    output_dir: Path,
    candidate_runs: Optional[List[int]] = None,
) -> List[str]:
    """Render a 2-panel trend plot. The bottom ratio panel dynamically scales to capture all points."""
    fig, (ax_top, ax_ratio) = plt.subplots(
        2, 1, figsize=(14, 8), sharex=True, gridspec_kw={"height_ratios": [2.6, 1.2], "hspace": 0.08}
    )

    # Top panel: Calibration means
    ax_top.plot(
        runs, orig, color="#222222", marker="s", markersize=4.2, fillstyle="none",
        markeredgewidth=1.2, ls="none", label="T0 Integrated"
    )
    ax_top.plot(
        runs, main, color="#1f77b4", marker="o", markersize=3.5,
        alpha=0.85, ls="none", label="Main [-2.5, 2.5] ns"
    )
    ax_top.plot(
        runs, minus5, color="#d62728", marker="v", markersize=4.2, fillstyle="none",
        markeredgewidth=1.2, ls="none", label="-5 [-7.5, -2.5] ns"
    )
    ax_top.plot(
        runs, plus5, color="#2ca02c", marker="^", markersize=4.2, fillstyle="none",
        markeredgewidth=1.2, ls="none", label="+5 [2.5, 7.5] ns"
    )

    min_run, max_run = runs.min(), runs.max()
    all_top_vals = np.concatenate([orig, main, minus5, plus5])
    y_min_top, y_max_top = float(all_top_vals.min()), float(all_top_vals.max())
    y_span = max(y_max_top - y_min_top, 0.05)
    # Generous headroom: 45% at top for legend/badges, and 40% at bottom for candidate run labels
    ax_top.set_ylim(y_min_top - 0.40 * y_span, y_max_top + 0.45 * y_span)

    for s, e, idx in epochs:
        if e < min_run or s > max_run:
            continue
        color = EPOCH_COLORS[(idx - 1) % len(EPOCH_COLORS)]
        ax_top.axvline(s, color="gray", ls=":", lw=1.0, alpha=0.7)
        ax_ratio.axvline(s, color="gray", ls=":", lw=1.0, alpha=0.7)

        s_vis = max(s, min_run)
        e_vis = min(e, max_run)
        if s_vis < e_vis:
            ax_top.axvspan(s_vis, e_vis, color=color, alpha=0.18)
            ax_ratio.axvspan(s_vis, e_vis, color=color, alpha=0.18)
            mid_x = (s_vis + e_vis) / 2.0
            ax_top.text(
                mid_x, 0.97,
                f"Epoch {idx}",
                transform=ax_top.get_xaxis_transform(),
                ha="center", va="top", fontsize=9.5, color="#222222", weight="bold",
                bbox=dict(boxstyle="round,pad=0.2", facecolor="white", edgecolor="none", alpha=0.75)
            )

    # Mark candidate runs with vertical dashed lines and rotated run numbers
    if candidate_runs:
        first_cand = True
        run_span = max(max_run - min_run, 1)
        for c_run in candidate_runs:
            if (min_run - 50) <= c_run <= (max_run + 50):
                lbl = "Candidate Run" if first_cand else None
                ax_top.axvline(c_run, color="#6a3d9a", ls="--", lw=1.3, alpha=0.85, label=lbl, zorder=2)
                ax_ratio.axvline(c_run, color="#6a3d9a", ls="--", lw=1.3, alpha=0.85, zorder=2)
                first_cand = False

                # Position run number rotated 90 degrees anchored at bottom of axis to guarantee no collision with data points
                if (max_run - c_run) < 0.02 * run_span:
                    x_text = c_run - 0.016 * run_span
                else:
                    x_text = c_run + 0.005 * run_span

                ax_top.text(
                    x_text, 0.03, f"Run {c_run}",
                    transform=ax_top.get_xaxis_transform(),
                    rotation=90, rotation_mode="anchor",
                    ha="left", va="bottom", fontsize=8.5, color="#6a3d9a", weight="bold",
                    bbox=dict(boxstyle="round,pad=0.18", facecolor="white", edgecolor="#6a3d9a", alpha=0.92, lw=0.7),
                    zorder=4
                )

    ax_top.set_ylabel(r"Detector Mean Active Ratio", fontsize=14)
    ax_top.legend(
        loc="upper right", bbox_to_anchor=(0.99, 0.94),
        frameon=True, facecolor="white", framealpha=0.5, edgecolor="#dddddd",
        fontsize=10.5, ncol=3, markerscale=1.8
    )

    ax_top.text(
        0.02, 0.85, f"{det}  |  {len(runs)} Runs",
        transform=ax_top.transAxes, fontsize=16, weight="bold", color="#333333"
    )

    # Bottom panel: Ratios with respect to T0 Integrated
    denom = np.where(orig > 0, orig, 1.0)
    ratio_main_to_t0 = main / denom
    ratio_minus5_to_t0 = minus5 / denom
    ratio_plus5_to_t0 = plus5 / denom

    ax_ratio.axhline(1.0, color="#555555", ls="-", lw=1.0)
    ax_ratio.plot(
        runs, ratio_main_to_t0, color="#1f77b4", marker="o", markersize=3.5,
        alpha=0.85, ls="none", label="Main [-2.5, 2.5] ns / T0 Integrated"
    )
    ax_ratio.plot(
        runs, ratio_minus5_to_t0, color="#d62728", marker="v", markersize=4.2, fillstyle="none",
        markeredgewidth=1.2, ls="none", label="-5 [-7.5, -2.5] ns / T0 Integrated"
    )
    ax_ratio.plot(
        runs, ratio_plus5_to_t0, color="#2ca02c", marker="^", markersize=4.2, fillstyle="none",
        markeredgewidth=1.2, ls="none", label="+5 [2.5, 7.5] ns / T0 Integrated"
    )

    ax_ratio.set_xlabel(r"Run Number", fontsize=14)
    ax_ratio.set_ylabel(r"Ratio to T0 Integrated", fontsize=13)
    ax_ratio.ticklabel_format(useOffset=False, style="plain")

    ax_top.set_xlim(min_run - 50, max_run + 50)
    ax_ratio.set_xlim(min_run - 50, max_run + 50)

    # Dynamically scale ratio panel to capture all points and leave headroom for legend
    all_ratios = np.concatenate([ratio_main_to_t0, ratio_minus5_to_t0, ratio_plus5_to_t0])
    valid_ratios = all_ratios[np.isfinite(all_ratios) & (all_ratios > 0)]

    if len(valid_ratios) > 0:
        r_min = float(valid_ratios.min())
        r_max = float(valid_ratios.max())
    else:
        r_min, r_max = 0.90, 1.10

    # Ensure reference line 1.0 is comfortably within view
    r_min = min(r_min, 0.95)
    r_max = max(r_max, 1.05)
    r_span = max(r_max - r_min, 0.10)

    # 45% headroom above highest point guarantees the legend never overlaps data points
    ax_ratio.set_ylim(r_min - 0.05 * r_span, r_max + 0.45 * r_span)

    ax_ratio.legend(
        loc="upper right", bbox_to_anchor=(0.99, 0.98),
        frameon=True, facecolor="white", framealpha=0.5, edgecolor="#dddddd",
        fontsize=9.5, ncol=3, markerscale=1.8
    )

    saved = []
    out_png = output_dir / f"trends_vs_run_{det}.png"
    out_pdf = output_dir / f"trends_vs_run_{det}.pdf"
    fig.savefig(out_png, dpi=200, bbox_inches="tight")
    fig.savefig(out_pdf, bbox_inches="tight")
    plt.close(fig)
    saved.extend([out_png.name, out_pdf.name])
    return saved


def worker_plot_trend(args: Tuple) -> List[str]:
    """Worker task to render 2-panel trend plot for a single detector."""
    det, runs, orig, main, minus5, plus5, epochs, output_dir = args[:8]
    candidate_runs = args[8] if len(args) > 8 else None
    return render_trend_figure(det, runs, orig, main, minus5, plus5, epochs, output_dir, candidate_runs)


def worker_plot_dead_towers(args: Tuple) -> str:
    """Worker task to render uncalibrated / zero towers plot for a single detector."""
    det, runs, zeros, pct, n_total, epochs, output_dir = args

    fig, ax = plt.subplots(figsize=(13, 6))
    ax.plot(runs, zeros, color="#e15759", lw=1.8, marker=".", markersize=4, label=f"Uncalibrated Towers ({det})")

    min_run, max_run = runs.min(), runs.max()
    for s, e, idx in epochs:
        if e < min_run or s > max_run:
            continue
        ax.axvline(s, color="gray", ls=":", lw=1.0, alpha=0.7)

    z_span = max(float(zeros.max() - zeros.min()), 10.0)
    ax.set_ylim(max(0.0, float(zeros.min()) - 0.08 * z_span), float(zeros.max()) + 0.22 * z_span)
    ax.set_xlabel(r"Run Number", fontsize=14)
    ax.set_ylabel(r"Zero-Calibration Towers Count", fontsize=14)
    ax.grid(True, ls=":", alpha=0.5)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=11)
    ax.ticklabel_format(useOffset=False, style="plain")

    ax.text(
        0.02, 0.90,
        f"{det} Zero Channels  |  Mean: {zeros.mean():.1f} ({pct.mean():.2f}%)  |  Total: {n_total}",
        transform=ax.transAxes, fontsize=11, weight="bold", color="#333333"
    )

    out_path = output_dir / f"dead_towers_vs_run_{det}.png"
    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def worker_plot_distribution(args: Tuple) -> Optional[str]:
    """Worker task to render 1D ratio distribution overlay for a single (detector, epoch)."""
    det, s, e, idx, sample_files, output_dir = args
    if not sample_files:
        return None

    orig_vals, main_vals, m5_vals, p5_vals = [], [], [], []
    for fpath in sample_files:
        try:
            with uproot.open(fpath) as f:
                t = f["Multiple"]
                arr = t.arrays(["Fratio", "Fratio_main", "Fratio_minus5", "Fratio_plus5"], library="np")
                mask = arr["Fratio"] > 0
                orig_vals.append(arr["Fratio"][mask])
                main_vals.append(arr["Fratio_main"][mask])
                m5_vals.append(arr["Fratio_minus5"][mask])
                p5_vals.append(arr["Fratio_plus5"][mask])
        except Exception as err:
            print(f"Error reading {fpath}: {err}")

    if not orig_vals:
        return None

    o_all = np.concatenate(orig_vals)
    m_all = np.concatenate(main_vals)
    m5_all = np.concatenate(m5_vals)
    p5_all = np.concatenate(p5_vals)

    fig, ax = plt.subplots(figsize=(10, 6.5))
    bins = np.linspace(0.85, 1.15, 60)

    ax.hist(o_all, bins=bins, histtype="step", color="#333333", lw=1.8, ls="--", label=f"T0 Integrated ({o_all.mean():.3f} ± {o_all.std():.3f})")
    ax.hist(m_all, bins=bins, histtype="step", color="#1f77b4", lw=2.2, label=f"Main [-2.5, 2.5] ns ({m_all.mean():.3f} ± {m_all.std():.3f})")
    ax.hist(m5_all, bins=bins, histtype="step", color="#d62728", lw=1.8, ls=":", label=f"-5 [-7.5, -2.5] ns ({m5_all.mean():.3f} ± {m5_all.std():.3f})")
    ax.hist(p5_all, bins=bins, histtype="step", color="#2ca02c", lw=1.8, ls="-.", label=f"+5 [2.5, 7.5] ns ({p5_all.mean():.3f} ± {p5_all.std():.3f})")

    ax.set_xlabel(r"Calibration Ratio", fontsize=14)
    ax.set_ylabel(r"Towers / Bin (Normalized per run)", fontsize=14)
    ax.set_yscale("log")
    ax.set_ylim(0.5, ax.get_ylim()[1] * 2.8)
    ax.grid(True, ls=":", alpha=0.5)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.5)

    ax.text(
        0.03, 0.90, f"{det}  |  Epoch {idx} [{s}, {e}]  |  Sampled {len(sample_files)} Runs",
        transform=ax.transAxes, fontsize=11, weight="bold", color="#333333"
    )

    out_path = output_dir / f"distributions_epoch{idx}_{det}.png"
    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def worker_plot_closure(args: Tuple) -> Optional[str]:
    """Worker task to render mathematical closure residual for a single detector."""
    det, input_dir, candidates, output_dir = args
    cfg = DETECTOR_CONFIG[det]
    hist_det = cfg["hist_det"]

    closure_diffs = []
    tested_runs = []

    for cand_run, hist_file in candidates.items():
        out_file = find_cdb_file(input_dir, det, cand_run)
        if not out_file or not out_file.is_file() or not os.path.exists(hist_file):
            continue

        try:
            with uproot.open(out_file) as f_cdb:
                t = f_cdb["Multiple"]
                arr = t.arrays(["IID", "Fratio", "Fratio_main"], library="np")
                cdb_main = arr["Fratio_main"]
                active = arr["Fratio"] > 0

            with uproot.open(hist_file) as f_hist:
                hname = f"h_CaloFittingQA_{hist_det}_etaphi_ZScrosscalib_main"
                if hname not in f_hist:
                    continue
                h_obj = f_hist[hname]
                if hasattr(h_obj, "values"):
                    h = h_obj.values()
                else:
                    h = h_obj.to_numpy()[0]

            hist_flat = h.flatten()
            if len(hist_flat) == len(cdb_main):
                diff = (cdb_main[active] - hist_flat[active])
                closure_diffs.append(diff)
                tested_runs.append(cand_run)
        except Exception as e:
            print(f"Closure check error for run {cand_run} {det}: {e}")

    if not closure_diffs:
        return None

    all_diffs = np.concatenate(closure_diffs)
    max_abs_err = np.max(np.abs(all_diffs))
    rms_err = np.std(all_diffs)

    fig, ax = plt.subplots(figsize=(8, 8))
    ax.set_box_aspect(1)
    bins = np.linspace(-1e-6, 1e-6, 80)
    ax.hist(all_diffs, bins=bins, color="#1f77b4", edgecolor="#0e4f7d", alpha=0.85, label=r"Residual (Main [-2.5, 2.5] ns $-$ Hist)")

    ax.set_xlabel(r"Absolute Residual ($\mathrm{Main\ [-2.5, 2.5]\ ns} - H_{\mathrm{main}}$)", fontsize=13)
    ax.set_ylabel(r"Tower Count", fontsize=13)
    ax.set_yscale("log")
    ax.grid(True, ls=":", alpha=0.5)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.5)

    ax.text(
        0.03, 0.88,
        f"{det} Candidate Closure Test\n"
        f"Candidate Runs: {', '.join(map(str, tested_runs))}\n"
        f"Max Absolute Error: {max_abs_err:.2e}  |  RMS: {rms_err:.2e}",
        transform=ax.transAxes, fontsize=10, weight="bold", color="#333333"
    )

    out_path = output_dir / f"candidate_closure_{det}.png"
    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def worker_plot_spatial_maps(args: Tuple) -> List[str]:
    """Worker task to render 2D spatial maps (main and spread) for a (detector, run)."""
    det, run, input_dir, output_dir = args
    cfg = DETECTOR_CONFIG[det]
    neta, nphi = cfg["neta"], cfg["nphi"]

    file_path = find_cdb_file(input_dir, det, run)
    if not file_path or not file_path.is_file():
        return []

    saved = []
    try:
        with uproot.open(file_path) as f:
            t = f["Multiple"]
            arr = t.arrays(["IID", "Fratio_main", "Fratio_minus5", "Fratio_plus5"], library="np")

        main = arr["Fratio_main"].reshape((neta, nphi))
        m5 = arr["Fratio_minus5"].reshape((neta, nphi))
        p5 = arr["Fratio_plus5"].reshape((neta, nphi))

        main_masked = np.ma.masked_equal(main, 0.0)
        diff_pct = np.divide((p5 - m5) * 100.0, main, out=np.zeros_like(main), where=main > 0)
        diff_masked = np.ma.masked_equal(diff_pct, 0.0)

        # 1. 2D Map of ratio_main (aspect="equal" ensures individual cells are square)
        fig, ax = plt.subplots(figsize=(14, 5.8))
        im = ax.imshow(
            main_masked,
            origin="lower",
            aspect="equal",
            cmap="viridis",
            vmin=0.88,
            vmax=1.05,
            extent=[0, nphi, 0, neta],
        )
        divider = make_axes_locatable(ax)
        cax = divider.append_axes("right", size="2.5%", pad=0.15)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_label(r"Main [-2.5, 2.5] ns Ratio", fontsize=13)

        ax.set_xlabel(r"$\phi$ Bin Index", fontsize=13)
        ax.set_ylabel(r"$\eta$ Bin Index", fontsize=13)
        ax.text(
            0.02, 0.92, rf"{det}  |  Run {run}  |  Main [-2.5, 2.5] ns Map",
            transform=ax.transAxes, fontsize=11, weight="bold", color="white",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="black", edgecolor="none", alpha=0.6)
        )

        out_path1 = output_dir / f"spatial_map_{det}_run{run}_main.png"
        fig.savefig(out_path1, dpi=200, bbox_inches="tight")
        plt.close(fig)
        saved.append(out_path1.name)

        # 2. 2D Difference map (±5 spread, aspect="equal" ensures individual cells are square)
        fig, ax = plt.subplots(figsize=(14, 5.8))
        im2 = ax.imshow(
            diff_masked,
            origin="lower",
            aspect="equal",
            cmap="coolwarm",
            vmin=-8.0,
            vmax=8.0,
            extent=[0, nphi, 0, neta],
        )
        divider2 = make_axes_locatable(ax)
        cax2 = divider2.append_axes("right", size="2.5%", pad=0.15)
        cbar2 = plt.colorbar(im2, cax=cax2)
        cbar2.set_label(r"Sample Variation Spread: $(+5 - -5) / \mathrm{Main}$ [%]", fontsize=12)

        ax.set_xlabel(r"$\phi$ Bin Index", fontsize=13)
        ax.set_ylabel(r"$\eta$ Bin Index", fontsize=13)
        ax.text(
            0.02, 0.92, rf"{det}  |  Run {run}  |  Spread: (+5 - -5) / Main [%]",
            transform=ax.transAxes, fontsize=11, weight="bold", color="black",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="none", alpha=0.75)
        )

        out_path2 = output_dir / f"spatial_map_{det}_run{run}_plus_minus_spread.png"
        fig.savefig(out_path2, dpi=200, bbox_inches="tight")
        plt.close(fig)
        saved.append(out_path2.name)

        return saved
    except Exception as e:
        print(f"Spatial map error for run {run} {det}: {e}")
        return []


def render_single_run_val_residuals(
    det: str,
    run: int,
    run_diffs: Dict[str, np.ndarray],
    output_dir: Path,
) -> str:
    """Render 1D residual distribution plot for a single validation run."""
    valid_diffs = [run_diffs[w["key"]] for w in SAMPLE_WINDOWS if len(run_diffs.get(w["key"], [])) > 0]
    if not valid_diffs:
        return ""

    all_diffs_combined = np.concatenate(valid_diffs)
    p05_diff = np.percentile(all_diffs_combined, 0.5)
    p995_diff = np.percentile(all_diffs_combined, 99.5)
    span = max(abs(p05_diff), abs(p995_diff))
    lim = max(span * 1.25, 0.05)
    bins = np.linspace(-lim, lim, 70)

    fig, ax = plt.subplots(figsize=(10, 6.5))
    for win in SAMPLE_WINDOWS:
        w_key = win["key"]
        diff = run_diffs.get(w_key, np.array([]))
        if len(diff) == 0:
            continue
        ax.hist(
            diff,
            bins=bins,
            histtype="step",
            lw=2.0,
            color=win["color"],
            label=f"{win['label']} ({diff.mean():+.3f} ± {diff.std():.3f})",
        )

    ax.axvline(0.0, color="#555555", ls="--", lw=1.0)
    ax.set_xlabel(r"Absolute Residual ($\mathrm{CDB} - \mathrm{Hist}$)", fontsize=14)
    ax.set_ylabel(r"Towers / Bin", fontsize=14)
    ax.set_yscale("log")
    ax.set_ylim(0.5, ax.get_ylim()[1] * 3.0)
    ax.grid(True, ls=":", alpha=0.5)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.5)

    ax.text(
        0.03, 0.88,
        f"{det}  |  Run {run}  |  Validation Residuals\n"
        f"Active Channels: {len(valid_diffs[0]):,}",
        transform=ax.transAxes, fontsize=10.5, weight="bold", color="#333333"
    )

    out_png = output_dir / f"val_residuals_{det}_run{run}.png"
    fig.savefig(out_png, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_png.name


def render_single_run_val_ratios(
    det: str,
    run: int,
    run_ratios: Dict[str, np.ndarray],
    output_dir: Path,
) -> str:
    """Render 1D ratio distribution plot for a single validation run."""
    valid_ratios = [run_ratios[w["key"]] for w in SAMPLE_WINDOWS if len(run_ratios.get(w["key"], [])) > 0]
    if not valid_ratios:
        return ""

    all_ratios_combined = np.concatenate(valid_ratios)
    p05_r = np.percentile(all_ratios_combined, 0.5)
    p995_r = np.percentile(all_ratios_combined, 99.5)
    r_dev = max(abs(1.0 - p05_r), abs(p995_r - 1.0), 0.05)
    r_min = 1.0 - r_dev * 1.25
    r_max = 1.0 + r_dev * 1.25
    bins_r = np.linspace(r_min, r_max, 70)

    fig, ax = plt.subplots(figsize=(10, 6.5))
    for win in SAMPLE_WINDOWS:
        w_key = win["key"]
        rat = run_ratios.get(w_key, np.array([]))
        if len(rat) == 0:
            continue
        frac_5pct = float(np.mean(np.abs(rat - 1.0) <= 0.05)) * 100.0
        ax.hist(
            rat,
            bins=bins_r,
            histtype="step",
            lw=2.0,
            color=win["color"],
            label=f"{win['label']} ({rat.mean():.3f} ± {rat.std():.3f} | ±5%: {frac_5pct:.1f}%)",
        )

    ax.axvline(1.0, color="#555555", ls="--", lw=1.0)
    ax.axvline(0.95, color="#888888", ls=":", lw=1.0, alpha=0.7)
    ax.axvline(1.05, color="#888888", ls=":", lw=1.0, alpha=0.7)
    ax.set_xlabel(r"Ratio ($\mathrm{CDB} / \mathrm{Hist}$)", fontsize=14)
    ax.set_ylabel(r"Towers / Bin", fontsize=14)
    ax.set_yscale("log")
    ax.set_ylim(0.5, ax.get_ylim()[1] * 3.0)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.0)

    ax.text(
        0.03, 0.88,
        f"{det}  |  Run {run}  |  Validation Ratios\n"
        f"Reference Target = 1.0 (Dotted ±5% Bounds)",
        transform=ax.transAxes, fontsize=10.5, weight="bold", color="#333333"
    )

    out_png = output_dir / f"val_ratios_{det}_run{run}.png"
    fig.savefig(out_png, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_png.name


def worker_plot_validation_residuals(args: Tuple) -> List[str]:
    """Worker task to render validation residual & ratio distributions and summary stats for a detector."""
    det, input_dir, val_candidates, epochs, output_dir, dir_val_residuals, dir_val_ratios = args
    cfg = DETECTOR_CONFIG[det]
    hist_det = cfg["hist_det"]

    saved = []
    window_diffs = {w["key"]: [] for w in SAMPLE_WINDOWS}
    window_ratios = {w["key"]: [] for w in SAMPLE_WINDOWS}
    tested_runs = []
    summary_records = []

    for val_run, hist_file in sorted(val_candidates.items()):
        cdb_file = find_cdb_file(input_dir, det, val_run)
        if not cdb_file or not cdb_file.is_file() or not os.path.exists(hist_file):
            continue

        try:
            with uproot.open(cdb_file) as f_cdb:
                if "Multiple" not in f_cdb:
                    continue
                t = f_cdb["Multiple"]
                cdb_arr = t.arrays(["IID", "Fratio", "Fratio_main", "Fratio_minus5", "Fratio_plus5"], library="np")

            with uproot.open(hist_file) as f_hist:
                run_has_data = False
                run_diffs = {}
                run_ratios = {}
                for win in SAMPLE_WINDOWS:
                    w_key = win["key"]
                    cdb_col = win["cdb_col"]
                    hname = f"h_CaloFittingQA_{hist_det}_etaphi_ZScrosscalib_{win['hist_suffix']}"
                    if hname not in f_hist:
                        continue

                    h_obj = f_hist[hname]
                    h_vals = h_obj.values() if hasattr(h_obj, "values") else h_obj.to_numpy()[0]
                    hist_flat = h_vals.flatten()
                    cdb_vals = cdb_arr[cdb_col]

                    if len(hist_flat) != len(cdb_vals):
                        continue

                    active = (
                        (cdb_arr["Fratio"] > 0)
                        & (cdb_vals > 0)
                        & (hist_flat > 0)
                        & np.isfinite(cdb_vals)
                        & np.isfinite(hist_flat)
                    )

                    if not np.any(active):
                        continue

                    diff = cdb_vals[active] - hist_flat[active]
                    ratio = cdb_vals[active] / hist_flat[active]

                    run_diffs[w_key] = diff
                    run_ratios[w_key] = ratio
                    window_diffs[w_key].append(diff)
                    window_ratios[w_key].append(ratio)
                    run_has_data = True

                    summary_records.append({
                        "run": val_run,
                        "detector": det,
                        "window": w_key,
                        "window_label": win["label"],
                        "n_towers": int(np.sum(active)),
                        "mean_cdb": float(np.mean(cdb_vals[active])),
                        "mean_hist": float(np.mean(hist_flat[active])),
                        "mean_diff": float(np.mean(diff)),
                        "std_diff": float(np.std(diff)),
                        "sem_diff": float(np.std(diff) / np.sqrt(np.sum(active))),
                        "median_diff": float(np.median(diff)),
                        "p05_diff": float(np.percentile(diff, 5)),
                        "p95_diff": float(np.percentile(diff, 95)),
                        "mean_ratio": float(np.mean(ratio)),
                        "std_ratio": float(np.std(ratio)),
                        "sem_ratio": float(np.std(ratio) / np.sqrt(np.sum(active))),
                        "frac_within_5pct": float(np.mean(np.abs(ratio - 1.0) <= 0.05)),
                    })

                if run_has_data:
                    tested_runs.append(val_run)
                    # Render per-run 1D residual and ratio plots in dedicated subfolders
                    res_file = render_single_run_val_residuals(det, val_run, run_diffs, dir_val_residuals)
                    if res_file:
                        saved.append(res_file)
                    rat_file = render_single_run_val_ratios(det, val_run, run_ratios, dir_val_ratios)
                    if rat_file:
                        saved.append(rat_file)

        except Exception as e:
            print(f"Validation comparison error for run {val_run} {det}: {e}")

    # If no valid data across all runs
    valid_window_diffs = [np.concatenate(window_diffs[w["key"]]) for w in SAMPLE_WINDOWS if window_diffs[w["key"]]]
    if not valid_window_diffs:
        return []

    # 1. Combined residual distribution plot across all validation runs (CDB - Hist)
    all_diffs_combined = np.concatenate(valid_window_diffs)
    p05_diff = np.percentile(all_diffs_combined, 0.5)
    p995_diff = np.percentile(all_diffs_combined, 99.5)
    span = max(abs(p05_diff), abs(p995_diff))
    lim = max(span * 1.25, 0.05)
    bins = np.linspace(-lim, lim, 75)

    fig, ax = plt.subplots(figsize=(10, 6.5))
    for win in SAMPLE_WINDOWS:
        w_key = win["key"]
        if not window_diffs[w_key]:
            continue
        w_diff = np.concatenate(window_diffs[w_key])
        ax.hist(
            w_diff,
            bins=bins,
            histtype="step",
            lw=2.0,
            color=win["color"],
            label=f"{win['label']} ({w_diff.mean():+.3f} ± {w_diff.std():.3f})",
        )

    ax.axvline(0.0, color="#555555", ls="--", lw=1.0)
    ax.set_xlabel(r"Absolute Residual ($\mathrm{CDB} - \mathrm{Hist}$)", fontsize=14)
    ax.set_ylabel(r"Towers / Bin", fontsize=14)
    ax.set_yscale("log")
    ax.set_ylim(0.5, ax.get_ylim()[1] * 3.0)
    ax.grid(True, ls=":", alpha=0.5)
    ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.5)

    run_str = ", ".join(map(str, tested_runs[:8]))
    if len(tested_runs) > 8:
        run_str += f"... (+{len(tested_runs)-8} more)"

    ax.text(
        0.03, 0.88,
        f"{det} Validation Residuals (Combined)\n"
        f"Validation Runs: {run_str}\n"
        f"Total Channel Matches: {len(all_diffs_combined):,}",
        transform=ax.transAxes, fontsize=10.5, weight="bold", color="#333333"
    )

    out_res_png = output_dir / f"val_residuals_{det}.png"
    out_res_pdf = output_dir / f"val_residuals_{det}.pdf"
    fig.savefig(out_res_png, dpi=200, bbox_inches="tight")
    fig.savefig(out_res_pdf, bbox_inches="tight")
    plt.close(fig)
    saved.extend([out_res_png.name, out_res_pdf.name])

    # 2. Combined ratio distribution plot across all validation runs (CDB / Hist)
    valid_window_ratios = [np.concatenate(window_ratios[w["key"]]) for w in SAMPLE_WINDOWS if window_ratios[w["key"]]]
    if valid_window_ratios:
        all_ratios_combined = np.concatenate(valid_window_ratios)
        p05_r = np.percentile(all_ratios_combined, 0.5)
        p995_r = np.percentile(all_ratios_combined, 99.5)
        r_dev = max(abs(1.0 - p05_r), abs(p995_r - 1.0), 0.05)
        r_min = 1.0 - r_dev * 1.25
        r_max = 1.0 + r_dev * 1.25
        bins_r = np.linspace(r_min, r_max, 75)

        fig, ax = plt.subplots(figsize=(10, 6.5))
        for win in SAMPLE_WINDOWS:
            w_key = win["key"]
            if not window_ratios[w_key]:
                continue
            w_ratio = np.concatenate(window_ratios[w_key])
            frac_5pct = float(np.mean(np.abs(w_ratio - 1.0) <= 0.05)) * 100.0
            ax.hist(
                w_ratio,
                bins=bins_r,
                histtype="step",
                lw=2.0,
                color=win["color"],
                label=f"{win['label']} ({w_ratio.mean():.3f} ± {w_ratio.std():.3f} | ±5%: {frac_5pct:.1f}%)",
            )

        ax.axvline(1.0, color="#555555", ls="--", lw=1.0)
        ax.axvline(0.95, color="#888888", ls=":", lw=1.0, alpha=0.7)
        ax.axvline(1.05, color="#888888", ls=":", lw=1.0, alpha=0.7)
        ax.set_xlabel(r"Ratio ($\mathrm{CDB} / \mathrm{Hist}$)", fontsize=14)
        ax.set_ylabel(r"Towers / Bin", fontsize=14)
        ax.set_yscale("log")
        ax.set_ylim(0.5, ax.get_ylim()[1] * 3.0)
        ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=10.0)

        ax.text(
            0.03, 0.88,
            f"{det} Validation Ratios (Combined)\n"
            f"Validation Runs: {run_str}\n"
            f"Reference Target = 1.0 (Dotted ±5% Bounds)",
            transform=ax.transAxes, fontsize=10.5, weight="bold", color="#333333"
        )

        out_rat_png = output_dir / f"val_ratios_{det}.png"
        out_rat_pdf = output_dir / f"val_ratios_{det}.pdf"
        fig.savefig(out_rat_png, dpi=200, bbox_inches="tight")
        fig.savefig(out_rat_pdf, bbox_inches="tight")
        plt.close(fig)
        saved.extend([out_rat_png.name, out_rat_pdf.name])

    # 3. Save tabular CSV summary
    if summary_records:
        df_sum = pd.DataFrame(summary_records)
        csv_path = output_dir / f"val_summary_{det}.csv"
        df_sum.to_csv(csv_path, index=False)
        saved.append(csv_path.name)

    return saved


def render_val_trend_by_run(
    det: str,
    df_records: pd.DataFrame,
    epochs: List[Tuple[int, int, int]],
    output_dir: Path,
    ylim_residual: Optional[Tuple[float, float]] = None,
    ylim_ratio: Optional[Tuple[float, float]] = None,
) -> List[str]:
    """Render per-run trend plot across validation runs with categorical x-axis, timing offsets, and synchronized y-ranges."""
    if df_records.empty:
        return []

    sorted_runs = sorted(list(dict.fromkeys(df_records["run"].tolist())))
    n_runs = len(sorted_runs)
    if n_runs == 0:
        return []

    run_to_idx = {r: i for i, r in enumerate(sorted_runs)}

    def get_run_epoch(r_num):
        return get_epoch_for_run(r_num, epochs)

    # Build contiguous epoch intervals across the sorted validation runs
    run_epochs = [get_run_epoch(r) for r in sorted_runs]
    epoch_blocks = []
    curr_ep = None
    start_idx = 0
    for i, ep in enumerate(run_epochs):
        if ep != curr_ep:
            if curr_ep is not None:
                epoch_blocks.append((curr_ep, start_idx, i - 1))
            curr_ep = ep
            start_idx = i
    if curr_ep is not None:
        epoch_blocks.append((curr_ep, start_idx, n_runs - 1))

    # Dynamic figure width based on the number of validation runs
    fig_width = max(11.0, min(24.0, 1.2 * n_runs + 3.5))
    fig, (ax_top, ax_bot) = plt.subplots(
        2, 1, figsize=(fig_width, 8), sharex=True, gridspec_kw={"height_ratios": [2.6, 1.2], "hspace": 0.08}
    )

    xlim_min = -0.55
    xlim_max = n_runs - 0.45

    # Draw shaded epoch backgrounds, separating lines, and top epoch badges
    for ep_idx, start_i, end_i in epoch_blocks:
        color = EPOCH_COLORS[(ep_idx - 1) % len(EPOCH_COLORS)] if ep_idx is not None else "#aaaaaa"
        x_s = start_i - 0.45
        x_e = end_i + 0.45
        ax_top.axvspan(x_s, x_e, color=color, alpha=0.15)
        ax_bot.axvspan(x_s, x_e, color=color, alpha=0.15)

        mid_x = (start_i + end_i) / 2.0
        lbl = f"Epoch {ep_idx}" if ep_idx is not None else "Unassigned"
        ax_top.text(
            mid_x, 0.97,
            lbl,
            transform=ax_top.get_xaxis_transform(),
            ha="center", va="top", fontsize=10, weight="bold", color="#222222",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="#cccccc", alpha=0.85)
        )

        # Prominent boundary between different epochs
        if end_i < n_runs - 1:
            sep_x = end_i + 0.5
            ax_top.axvline(sep_x, color="#666666", ls="--", lw=1.2, alpha=0.7)
            ax_bot.axvline(sep_x, color="#666666", ls="--", lw=1.2, alpha=0.7)

        # Subtle dividers between individual runs within the same epoch
        for i in range(start_i, end_i):
            sep_x = i + 0.5
            ax_top.axvline(sep_x, color="#dddddd", ls=":", lw=0.8, alpha=0.6)
            ax_bot.axvline(sep_x, color="#dddddd", ls=":", lw=0.8, alpha=0.6)

    ax_top.axhline(0.0, color="#555555", ls="-", lw=1.0)
    ax_bot.axhline(1.0, color="#555555", ls="-", lw=1.0)
    ax_bot.axhline(0.95, color="#888888", ls=":", lw=1.0, alpha=0.6)
    ax_bot.axhline(1.05, color="#888888", ls=":", lw=1.0, alpha=0.6)

    # Offsets and styling for the 3 sample windows: -5 ns (left), Main (center), +5 ns (right)
    win_offsets = {
        "minus5": -0.20,
        "main": 0.0,
        "plus5": 0.20,
    }
    win_markers = {
        "minus5": dict(marker="v", markersize=5.2, fillstyle="none", markeredgewidth=1.3, color="#d62728"),
        "main": dict(marker="o", markersize=4.6, fillstyle="full", markeredgewidth=1.0, color="#1f77b4"),
        "plus5": dict(marker="^", markersize=5.2, fillstyle="none", markeredgewidth=1.3, color="#2ca02c"),
    }

    all_diff_lows = []
    all_diff_highs = []
    all_ratio_lows = []
    all_ratio_highs = []

    # Plot in left-to-right plot order matching x-offsets: -5 ns, Main, +5 ns
    ordered_windows = sorted(SAMPLE_WINDOWS, key=lambda w: win_offsets[w["key"]])
    for win in ordered_windows:
        w_key = win["key"]
        sub = df_records[df_records["window"] == w_key].sort_values(by="run")
        if sub.empty:
            continue
        m_cfg = win_markers[w_key]
        offset = win_offsets[w_key]
        x_coords = [run_to_idx[r] + offset for r in sub["run"]]

        ax_top.errorbar(
            x_coords, sub["mean_diff"], yerr=sub["std_diff"],
            fmt=m_cfg["marker"], fillstyle=m_cfg["fillstyle"], markersize=m_cfg["markersize"],
            markeredgewidth=m_cfg["markeredgewidth"], color=m_cfg["color"], ecolor=m_cfg["color"],
            elinewidth=1.2, capsize=3.5, capthick=1.0, linestyle="none",
            label=win["label"]
        )
        ax_bot.errorbar(
            x_coords, sub["mean_ratio"], yerr=sub["std_ratio"],
            fmt=m_cfg["marker"], fillstyle=m_cfg["fillstyle"], markersize=m_cfg["markersize"],
            markeredgewidth=m_cfg["markeredgewidth"], color=m_cfg["color"], ecolor=m_cfg["color"],
            elinewidth=1.2, capsize=3.5, capthick=1.0, linestyle="none",
            label=win["label"]
        )
        all_diff_lows.extend((sub["mean_diff"] - sub["std_diff"]).tolist())
        all_diff_highs.extend((sub["mean_diff"] + sub["std_diff"]).tolist())
        all_ratio_lows.extend((sub["mean_ratio"] - sub["std_ratio"]).tolist())
        all_ratio_highs.extend((sub["mean_ratio"] + sub["std_ratio"]).tolist())

    # Top panel scaling & styling
    if ylim_residual is not None:
        ax_top.set_ylim(*ylim_residual)
    else:
        y_min_top = min(all_diff_lows) if all_diff_lows else -0.05
        y_max_top = max(all_diff_highs) if all_diff_highs else 0.05
        y_span_top = max(y_max_top - y_min_top, 0.02)
        ax_top.set_ylim(y_min_top - 0.15 * y_span_top, y_max_top + 0.45 * y_span_top)

    ax_top.set_xlim(xlim_min, xlim_max)
    ax_top.set_ylabel(r"Validation Residual ($\mathrm{CDB} - \mathrm{Hist}$)", fontsize=14)
    ax_top.text(
        0.02, 0.85, f"{det}  |  {n_runs} Validation Runs (Mean $\\pm\\ 1\\sigma$)",
        transform=ax_top.transAxes, fontsize=15, weight="bold", color="#333333"
    )
    ax_top.legend(
        loc="upper right", bbox_to_anchor=(0.99, 0.94),
        frameon=True, facecolor="white", framealpha=0.5, edgecolor="#dddddd",
        fontsize=10.5, ncol=3, markerscale=1.6
    )

    # Bottom panel scaling & styling
    if ylim_ratio is not None:
        ax_bot.set_ylim(*ylim_ratio)
    else:
        y_min_bot = min(all_ratio_lows) if all_ratio_lows else 0.95
        y_max_bot = max(all_ratio_highs) if all_ratio_highs else 1.05
        r_min = min(y_min_bot, 0.95)
        r_max = max(y_max_bot, 1.05)
        r_span = max(r_max - r_min, 0.10)
        ax_bot.set_ylim(r_min - 0.08 * r_span, r_max + 0.45 * r_span)

    ax_bot.set_xlim(xlim_min, xlim_max)

    # Categorical x-tick labels for each validation run with epoch note
    x_ticks = list(range(n_runs))
    x_labels = []
    for r in sorted_runs:
        ep = get_run_epoch(r)
        if ep is not None:
            x_labels.append(f"Run {r}\n(Epoch {ep})")
        else:
            x_labels.append(f"Run {r}")

    ax_bot.set_xticks(x_ticks)
    if n_runs > 10:
        ax_bot.set_xticklabels(x_labels, rotation=45, ha="right", fontsize=10)
    else:
        ax_bot.set_xticklabels(x_labels, rotation=0, ha="center", fontsize=11)

    ax_bot.set_xlabel(r"Validation Run", fontsize=14)
    ax_bot.set_ylabel(r"Ratio ($\mathrm{CDB} / \mathrm{Hist}$)", fontsize=13)
    ax_bot.legend(
        loc="upper right", bbox_to_anchor=(0.99, 0.98),
        frameon=True, facecolor="white", framealpha=0.5, edgecolor="#dddddd",
        fontsize=9.5, ncol=3, markerscale=1.6
    )

    out_trend_png = output_dir / f"val_trend_by_run_{det}.png"
    out_trend_pdf = output_dir / f"val_trend_by_run_{det}.pdf"
    fig.savefig(out_trend_png, dpi=200, bbox_inches="tight")
    fig.savefig(out_trend_pdf, bbox_inches="tight")
    plt.close(fig)
    return [out_trend_png.name, out_trend_pdf.name]


def worker_plot_val_spatial_maps(args: Tuple) -> List[str]:
    """Worker task to render 2D spatial residual maps for all sample windows for a (detector, run)."""
    det, run, hist_file, input_dir, output_dir = args
    cfg = DETECTOR_CONFIG[det]
    neta, nphi = cfg["neta"], cfg["nphi"]
    hist_det = cfg["hist_det"]

    cdb_file = find_cdb_file(input_dir, det, run)
    if not cdb_file or not cdb_file.is_file() or not os.path.exists(hist_file):
        return []

    saved = []
    try:
        with uproot.open(cdb_file) as f_cdb:
            if "Multiple" not in f_cdb:
                return []
            t = f_cdb["Multiple"]
            cdb_arr = t.arrays(["IID", "Fratio", "Fratio_main", "Fratio_minus5", "Fratio_plus5"], library="np")

        with uproot.open(hist_file) as f_hist:
            for win in SAMPLE_WINDOWS:
                w_key = win["key"]
                cdb_col = win["cdb_col"]
                hname = f"h_CaloFittingQA_{hist_det}_etaphi_ZScrosscalib_{win['hist_suffix']}"
                if hname not in f_hist:
                    continue

                h_obj = f_hist[hname]
                h_vals = h_obj.values() if hasattr(h_obj, "values") else h_obj.to_numpy()[0]
                cdb_vals = cdb_arr[cdb_col]

                if h_vals.shape != (neta, nphi) or len(cdb_vals) != (neta * nphi):
                    continue

                cdb_2d = cdb_vals.reshape((neta, nphi))
                hist_2d = h_vals

                valid_mask = (
                    (cdb_arr["Fratio"].reshape((neta, nphi)) > 0)
                    & (cdb_2d > 0)
                    & (hist_2d > 0)
                    & np.isfinite(cdb_2d)
                    & np.isfinite(hist_2d)
                )

                if not np.any(valid_mask):
                    continue

                diff_pct = np.zeros_like(cdb_2d)
                diff_pct[valid_mask] = ((cdb_2d[valid_mask] - hist_2d[valid_mask]) / hist_2d[valid_mask]) * 100.0
                diff_masked = np.ma.masked_where(~valid_mask, diff_pct)

                valid_vals = diff_pct[valid_mask]
                v_limit = float(np.percentile(np.abs(valid_vals), 98))
                v_limit = max(5.0, min(v_limit, 25.0))

                fig, ax = plt.subplots(figsize=(14, 5.8))
                im = ax.imshow(
                    diff_masked,
                    origin="lower",
                    aspect="equal",
                    cmap="coolwarm",
                    vmin=-v_limit,
                    vmax=v_limit,
                    extent=[0, nphi, 0, neta],
                )
                divider = make_axes_locatable(ax)
                cax = divider.append_axes("right", size="2.5%", pad=0.15)
                cbar = plt.colorbar(im, cax=cax)
                cbar.set_label(rf"Residual: $(\mathrm{{CDB}} - \mathrm{{Hist}}) / \mathrm{{Hist}}$ [%] ({win['label']})", fontsize=12)

                ax.set_xlabel(r"$\phi$ Bin Index", fontsize=13)
                ax.set_ylabel(r"$\eta$ Bin Index", fontsize=13)

                ax.text(
                    0.02, 0.92,
                    rf"{det}  |  Run {run}  |  {win['label']}" + "\n"
                    rf"Mean: {valid_vals.mean():+.2f}%  |  RMS: {valid_vals.std():.2f}%",
                    transform=ax.transAxes, fontsize=10.5, weight="bold", color="black",
                    bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="#cccccc", alpha=0.85)
                )

                out_path = output_dir / f"spatial_map_val_{det}_run{run}_{w_key}.png"
                fig.savefig(out_path, dpi=200, bbox_inches="tight")
                plt.close(fig)
                saved.append(out_path.name)

    except Exception as e:
        print(f"Validation spatial map error for run {run} {det}: {e}")

    return saved


def load_scale_factors(det: str, run: int, hist_file: str, input_dir: Path) -> Optional[Dict[str, np.ndarray]]:
    """
    Load per-tower scale factors SF_win = H_win / T0 Integrated for a (detector, run).
    T0 Integrated is the run's own Fratio from its CDB file. Returns {win_key: (neta, nphi) array}
    with NaN for invalid towers, or None if any window is unavailable.
    """
    cfg = DETECTOR_CONFIG[det]
    neta, nphi = cfg["neta"], cfg["nphi"]
    hist_det = cfg["hist_det"]

    cdb_file = find_cdb_file(input_dir, det, run)
    if not cdb_file or not cdb_file.is_file() or not os.path.exists(hist_file):
        print(f"Warning: Missing CDB or hist file for {det} run {run}; skipping scale factor maps.")
        return None

    try:
        with uproot.open(cdb_file) as f_cdb:
            t0 = f_cdb["Multiple"].arrays(["Fratio"], library="np")["Fratio"]
        if len(t0) != neta * nphi:
            return None
        t0_2d = t0.reshape((neta, nphi))

        sf = {}
        with uproot.open(hist_file) as f_hist:
            for w_key in SF_PANEL_ORDER:
                hname = f"h_CaloFittingQA_{hist_det}_etaphi_ZScrosscalib_{WINDOW_BY_KEY[w_key]['hist_suffix']}"
                if hname not in f_hist:
                    print(f"Warning: {hname} missing in {hist_file}; skipping {det} run {run}.")
                    return None
                h_obj = f_hist[hname]
                h_vals = h_obj.values() if hasattr(h_obj, "values") else h_obj.to_numpy()[0]
                if h_vals.shape != (neta, nphi):
                    return None

                valid = (t0_2d > 0) & (h_vals > 0) & np.isfinite(t0_2d) & np.isfinite(h_vals)
                if not np.any(valid):
                    return None
                sf[w_key] = np.where(valid, h_vals / np.where(valid, t0_2d, 1.0), np.nan)
        return sf
    except Exception as e:
        print(f"Scale factor load error for run {run} {det}: {e}")
        return None


def load_pm5_methods(input_dir: Path) -> Dict[Tuple[int, str], str]:
    """
    Read the ±5 ns method used per (run, detector) from the generator's time_fit_runs.csv in the CDB directory.
    Returns an empty dict when the file is absent (CDBs produced with the candidate method throughout).
    """
    path = input_dir / "time_fit_runs.csv"
    if not path.is_file():
        return {}
    df = pd.read_csv(path)
    return {(int(r), str(d)): str(m) for r, d, m in zip(df["run"], df["detector"], df["method"])}


def check_ratio_closure(
    det: str,
    val_run: int,
    input_dir: Path,
    sf_val: Dict[str, np.ndarray],
    ratios: Dict[str, np.ndarray],
    windows: Tuple[str, ...] = SF_PANEL_ORDER,
) -> Dict[str, float]:
    """
    Cross-check SF_val / SF_cand against Hist_val / CDB_val per window. The two are algebraically
    identical when the CDB uses the candidate method (CDB_val = SF_cand * T0_val, Hist_val = SF_val * T0_val).
    Only the given windows are checked; others return NaN. Returns max absolute deviation per window.
    """
    cfg = DETECTOR_CONFIG[det]
    neta, nphi = cfg["neta"], cfg["nphi"]
    out = {w_key: float("nan") for w_key in SF_PANEL_ORDER}

    cdb_file = find_cdb_file(input_dir, det, val_run)
    if not cdb_file:
        return out
    with uproot.open(cdb_file) as f_cdb:
        arr = f_cdb["Multiple"].arrays(["Fratio"] + [w["cdb_col"] for w in SAMPLE_WINDOWS], library="np")
    t0_2d = arr["Fratio"].reshape((neta, nphi))

    for w_key in windows:
        cdb_2d = arr[WINDOW_BY_KEY[w_key]["cdb_col"]].reshape((neta, nphi))
        ok = np.isfinite(ratios[w_key]) & (cdb_2d > 0)
        if not np.any(ok):
            continue
        hist_over_cdb = (sf_val[w_key][ok] * t0_2d[ok]) / cdb_2d[ok]
        out[w_key] = float(np.max(np.abs(ratios[w_key][ok] - hist_over_cdb)))
        if out[w_key] > 1e-4:
            print(
                f"Warning: {det} run {val_run} {w_key}: SF_val / SF_cand deviates from Hist / CDB "
                f"by up to {out[w_key]:.2e} (expected identical)."
            )
    return out


def render_sf_panels(panels: List[Dict[str, Any]], out_path: Path) -> str:
    """Render vertically stacked (eta, phi) heatmap panels, each with its own colour bar and stats badge."""
    fig, axes = plt.subplots(len(panels), 1, figsize=(14, 5.4 * len(panels)), squeeze=False)
    for i, (ax, p) in enumerate(zip(axes[:, 0], panels)):
        neta, nphi = p["data"].shape
        im = ax.imshow(
            np.ma.masked_invalid(p["data"]),
            origin="lower",
            aspect="equal",
            cmap=p["cmap"],
            vmin=p["vmin"],
            vmax=p["vmax"],
            extent=[0, nphi, 0, neta],
        )
        divider = make_axes_locatable(ax)
        cax = divider.append_axes("right", size="2.5%", pad=0.15)
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_label(p["cbar_label"], fontsize=12)

        ax.set_ylabel(r"$\eta$ Bin Index", fontsize=13)
        if i == len(panels) - 1:
            ax.set_xlabel(r"$\phi$ Bin Index", fontsize=13)

        ax.text(
            0.02, 0.95, p["badge"],
            transform=ax.transAxes, fontsize=10.5, weight="bold", color="black", va="top",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="#cccccc", alpha=0.85)
        )

    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def worker_plot_sf_map(args: Tuple) -> str:
    """Worker task to render stacked scale factor (H_win / T0 Integrated) maps for a (detector, run)."""
    det, run, role, ep_idx, sf, vranges, output_dir = args
    ep_str = f"Epoch {ep_idx}" if ep_idx is not None else "Unassigned"

    panels = []
    for w_key in SF_PANEL_ORDER:
        win = WINDOW_BY_KEY[w_key]
        vals = sf[w_key][np.isfinite(sf[w_key])]
        vmin, vmax = vranges[w_key]
        panels.append({
            "data": sf[w_key],
            "cmap": "viridis",
            "vmin": vmin,
            "vmax": vmax,
            "cbar_label": rf"{win['label']} / T0 Integrated",
            "badge": (
                f"{det}  |  {role} Run {run} ({ep_str})  |  {win['label']} / T0 Integrated\n"
                f"Mean: {vals.mean():.4f}  |  RMS: {vals.std():.4f}  |  Towers: {len(vals):,}"
            ),
        })

    return render_sf_panels(panels, output_dir / f"sf_map_{det}_run{run}.png")


def worker_plot_sf_epoch_ratio(args: Tuple) -> str:
    """Worker task to render stacked SF_val / SF_cand maps for one (detector, epoch) pairing."""
    det, ep_idx, cand_run, val_run, ratios, lims, output_dir = args

    panels = []
    for w_key in SF_PANEL_ORDER:
        win = WINDOW_BY_KEY[w_key]
        lim = lims[w_key]
        vals = ratios[w_key][np.isfinite(ratios[w_key])]
        frac_2pct = float(np.mean(np.abs(vals - 1.0) <= 0.02)) * 100.0
        frac_5pct = float(np.mean(np.abs(vals - 1.0) <= 0.05)) * 100.0
        panels.append({
            "data": ratios[w_key],
            "cmap": "coolwarm",
            "vmin": 1.0 - lim,
            "vmax": 1.0 + lim,
            "cbar_label": rf"$\mathrm{{SF}}_\mathrm{{val}} / \mathrm{{SF}}_\mathrm{{cand}}$ ({win['label']})",
            "badge": (
                f"{det}  |  Epoch {ep_idx}  |  Val Run {val_run} / Cand Run {cand_run}  |  {win['label']}\n"
                f"Mean: {vals.mean():.4f}  |  RMS: {vals.std():.4f}  |  ±2%: {frac_2pct:.1f}%  |  ±5%: {frac_5pct:.1f}%"
            ),
        })

    return render_sf_panels(panels, output_dir / f"sf_ratio_{det}_epoch{ep_idx}_val{val_run}_cand{cand_run}.png")


def pair_runs_by_epoch(
    cand_runs, val_runs, epochs: List[Tuple[int, int, int]], label: str
) -> List[Tuple[int, int, int]]:
    """Pair each validation run with the single candidate run of its epoch. Returns (epoch, cand_run, val_run)."""
    pairs = []
    for s, e, ep_idx in epochs:
        ep_vals = sorted(r for r in val_runs if s <= r <= e)
        if not ep_vals:
            continue
        ep_cands = [r for r in cand_runs if s <= r <= e]
        if len(ep_cands) != 1:
            print(f"  Skipping {label} Epoch {ep_idx}: expected 1 candidate run, found {len(ep_cands)}.")
            continue
        pairs.extend((ep_idx, ep_cands[0], v) for v in ep_vals)
    return pairs


def build_scale_factor_tasks(
    detectors: List[str],
    epochs: List[Tuple[int, int, int]],
    input_dir: Path,
    candidates: Dict[int, str],
    val_runs: Dict[int, str],
    output_dir: Path,
) -> Tuple[List[Tuple], Dict[str, Dict[int, Dict[str, np.ndarray]]]]:
    """
    Load scale factors for candidate and validation runs, derive shared colour ranges, pair runs by
    epoch to build SF_val / SF_cand ratios, and write the ratio summary CSV.
    Returns (plotting tasks, {det: {run: {win_key: sf}}}).
    """
    dir_cand = output_dir / "candidate"
    dir_val = output_dir / "validation"
    dir_ratio = output_dir / "epoch_ratio"
    dir_cand.mkdir(parents=True, exist_ok=True)
    if val_runs:
        dir_val.mkdir(parents=True, exist_ok=True)
        dir_ratio.mkdir(parents=True, exist_ok=True)

    print(
        f"\nLoading scale factors (H_win / T0 Integrated) for {len(candidates)} candidate "
        f"and {len(val_runs)} validation run(s) across {len(detectors)} detector(s)..."
    )

    # The Hist / CDB closure identity only holds where the CDB used the candidate method. For runs whose
    # ±5 ns came from the raw-time fit (per the generator's time_fit_runs.csv), only Main is checked.
    pm5_methods = load_pm5_methods(input_dir)
    if pm5_methods:
        print(f"  {input_dir / 'time_fit_runs.csv'} found: ±5 ns closure skipped for runs calibrated with the raw-time fit.")

    tasks = []
    summary_records = []
    sf_maps = {}
    for det in detectors:
        sf_cand = {r: load_scale_factors(det, r, p, input_dir) for r, p in sorted(candidates.items())}
        sf_val = {r: load_scale_factors(det, r, p, input_dir) for r, p in sorted(val_runs.items())}
        sf_cand = {r: s for r, s in sf_cand.items() if s is not None}
        sf_val = {r: s for r, s in sf_val.items() if s is not None}
        if not sf_cand and not sf_val:
            continue
        sf_maps[det] = {**sf_cand, **sf_val}

        # Colour range per window shared across all candidate and validation runs of this detector
        sf_vranges = {}
        for w_key in SF_PANEL_ORDER:
            pooled = np.concatenate([s[w_key][np.isfinite(s[w_key])] for s in [*sf_cand.values(), *sf_val.values()]])
            lo, hi = np.percentile(pooled, [1, 99])
            if hi - lo < 1e-4:
                lo, hi = lo - 0.005, hi + 0.005
            sf_vranges[w_key] = (float(lo), float(hi))

        for role, sf_map, role_dir in (("Candidate", sf_cand, dir_cand), ("Validation", sf_val, dir_val)):
            for run, sf in sf_map.items():
                tasks.append((
                    worker_plot_sf_map,
                    (det, run, role, get_epoch_for_run(run, epochs), sf, sf_vranges, role_dir),
                    f"SFMap {det} {role} Run {run}",
                ))

        ratio_maps = []
        for ep_idx, cand_run, val_run in pair_runs_by_epoch(sf_cand, sf_val, epochs, f"{det} SF ratio maps"):
            ratios = {w_key: sf_val[val_run][w_key] / sf_cand[cand_run][w_key] for w_key in SF_PANEL_ORDER}
            closure_windows = ("main",) if pm5_methods.get((val_run, det)) == "timefit" else SF_PANEL_ORDER
            closure = check_ratio_closure(det, val_run, input_dir, sf_val[val_run], ratios, closure_windows)
            ratio_maps.append((ep_idx, cand_run, val_run, ratios))

            for w_key in SF_PANEL_ORDER:
                vals = ratios[w_key][np.isfinite(ratios[w_key])]
                summary_records.append({
                    "detector": det,
                    "epoch": ep_idx,
                    "cand_run": cand_run,
                    "val_run": val_run,
                    "window": w_key,
                    "window_label": WINDOW_BY_KEY[w_key]["label"],
                    "n_towers": int(len(vals)),
                    "mean_ratio": float(np.mean(vals)),
                    "std_ratio": float(np.std(vals)),
                    "median_ratio": float(np.median(vals)),
                    "frac_within_2pct": float(np.mean(np.abs(vals - 1.0) <= 0.02)),
                    "frac_within_5pct": float(np.mean(np.abs(vals - 1.0) <= 0.05)),
                    "closure_max_abs_dev": closure[w_key],
                })

        if ratio_maps:
            # Symmetric colour range around 1.0 per window, shared across all epochs of this detector
            ratio_lims = {}
            for w_key in SF_PANEL_ORDER:
                pooled_dev = np.concatenate([np.abs(r[w_key][np.isfinite(r[w_key])] - 1.0) for _, _, _, r in ratio_maps])
                ratio_lims[w_key] = float(np.clip(np.percentile(pooled_dev, 98), 0.002, 0.25))
            for ep_idx, cand_run, val_run, ratios in ratio_maps:
                tasks.append((
                    worker_plot_sf_epoch_ratio,
                    (det, ep_idx, cand_run, val_run, ratios, ratio_lims, dir_ratio),
                    f"SFRatio {det} Epoch {ep_idx}",
                ))

    if summary_records:
        csv_path = dir_ratio / "sf_ratio_summary.csv"
        pd.DataFrame(summary_records).to_csv(csv_path, index=False)
        print(f"  Saved scale factor ratio summary: {csv_path}")

    return tasks, sf_maps






def worker_plot_time_map(args: Tuple) -> str:
    """Worker task to render stacked per-detector mean raw time maps for a run."""
    run, role, ep_idx, tmaps, lims, output_dir = args
    ep_str = f"Epoch {ep_idx}" if ep_idx is not None else "Unassigned"

    panels = []
    for det, tmap in tmaps.items():
        vals = tmap[np.isfinite(tmap)]
        panels.append({
            "data": tmap,
            "cmap": "coolwarm",
            "vmin": -lims[det],
            "vmax": lims[det],
            "cbar_label": f"{det} Mean Raw Time [samples]",
            "badge": (
                f"{det}  |  {role} Run {run} ({ep_str})  |  Mean Raw Time\n"
                f"Mean: {vals.mean():+.3f}  |  RMS: {vals.std():.3f} samples  |  Towers: {len(vals):,}"
            ),
        })

    return render_sf_panels(panels, output_dir / f"time_map_run{run}.png")


def worker_plot_time_diff(args: Tuple) -> str:
    """Worker task to render stacked per-detector t_val - t_cand maps for one epoch pairing."""
    ep_idx, cand_run, val_run, diffs, lims, output_dir = args

    panels = []
    for det, diff in diffs.items():
        vals = diff[np.isfinite(diff)]
        frac_010 = float(np.mean(np.abs(vals) <= 0.10)) * 100.0
        frac_025 = float(np.mean(np.abs(vals) <= 0.25)) * 100.0
        panels.append({
            "data": diff,
            "cmap": "coolwarm",
            "vmin": -lims[det],
            "vmax": lims[det],
            "cbar_label": rf"{det} $t_\mathrm{{val}} - t_\mathrm{{cand}}$ [samples]",
            "badge": (
                f"{det}  |  Epoch {ep_idx}  |  Val Run {val_run} - Cand Run {cand_run}  |  Raw Time Difference\n"
                f"Mean: {vals.mean():+.3f}  |  RMS: {vals.std():.3f} samples  |  ±0.1: {frac_010:.1f}%  |  ±0.25: {frac_025:.1f}%"
            ),
        })

    return render_sf_panels(panels, output_dir / f"time_diff_epoch{ep_idx}_val{val_run}_cand{cand_run}.png")


def build_time_map_tasks(
    detectors: List[str],
    epochs: List[Tuple[int, int, int]],
    candidates: Dict[int, str],
    val_runs: Dict[int, str],
    output_dir: Path,
) -> Tuple[List[Tuple], Dict[int, Dict[str, np.ndarray]]]:
    """
    Look up and load raw time maps for candidate and validation runs, derive per-detector colour ranges,
    pair runs by epoch to build t_val - t_cand maps, and write path and summary CSVs.
    Returns (plotting tasks, {run: {det: time map}}).
    """
    roles = {r: "Candidate" for r in candidates}
    roles.update({r: "Validation" for r in val_runs})
    runs = sorted(roles)

    print(f"\nLooking up {TIME_HIST_DSTTYPE} segment-0 hist files in FileCatalog for {len(runs)} run(s)...")
    try:
        paths = query_time_hist_paths(runs)
    except Exception as err:
        print(f"Warning: FileCatalog lookup failed ({err}); skipping time maps.")
        return [], {}

    output_dir.mkdir(parents=True, exist_ok=True)
    pd.DataFrame([
        {"run": r, "role": roles[r], "tag": paths[r][0] if r in paths else "", "path": paths[r][1] if r in paths else ""}
        for r in runs
    ]).to_csv(output_dir / "time_hist_paths.csv", index=False)

    # Load maps; keep only runs with every requested detector available
    tmaps = {}
    for run in runs:
        if run not in paths:
            print(f"Warning: No {TIME_HIST_DSTTYPE} segment-0 file found for run {run}.")
            continue
        path = paths[run][1]
        if not os.path.exists(path):
            print(f"Warning: Time hist file for run {run} not found on disk: {path}")
            continue
        run_maps = {det: load_time_map(det, path) for det in detectors}
        if any(m is None for m in run_maps.values()):
            print(f"Warning: Incomplete raw time maps for run {run}; skipping.")
            continue
        tmaps[run] = run_maps

    if not tmaps:
        return [], {}

    cand_maps = {r: m for r, m in tmaps.items() if roles[r] == "Candidate"}
    val_maps = {r: m for r, m in tmaps.items() if roles[r] == "Validation"}

    # Symmetric colour range around 0 per detector, shared across all candidate and validation runs
    lims = {}
    for det in detectors:
        pooled = np.concatenate([np.abs(m[det][np.isfinite(m[det])]) for m in tmaps.values()])
        lims[det] = float(np.clip(np.percentile(pooled, 99), 0.1, 10.0))

    tasks = []
    for role, maps, role_dir in (("Candidate", cand_maps, output_dir / "candidate"), ("Validation", val_maps, output_dir / "validation")):
        if maps:
            role_dir.mkdir(parents=True, exist_ok=True)
        for run, run_maps in maps.items():
            tasks.append((
                worker_plot_time_map,
                (run, role, get_epoch_for_run(run, epochs), run_maps, lims, role_dir),
                f"TimeMap {role} Run {run}",
            ))

    diff_maps = []
    summary_records = []
    for ep_idx, cand_run, val_run in pair_runs_by_epoch(cand_maps, val_maps, epochs, "time difference maps"):
        diffs = {det: val_maps[val_run][det] - cand_maps[cand_run][det] for det in detectors}
        diff_maps.append((ep_idx, cand_run, val_run, diffs))
        for det, diff in diffs.items():
            vals = diff[np.isfinite(diff)]
            summary_records.append({
                "detector": det,
                "epoch": ep_idx,
                "cand_run": cand_run,
                "val_run": val_run,
                "n_towers": int(len(vals)),
                "mean_diff": float(np.mean(vals)),
                "std_diff": float(np.std(vals)),
                "median_diff": float(np.median(vals)),
                "frac_within_0p10": float(np.mean(np.abs(vals) <= 0.10)),
                "frac_within_0p25": float(np.mean(np.abs(vals) <= 0.25)),
            })

    if diff_maps:
        dir_diff = output_dir / "epoch_diff"
        dir_diff.mkdir(parents=True, exist_ok=True)

        # Symmetric colour range around 0 per detector, shared across epochs
        diff_lims = {}
        for det in detectors:
            pooled = np.concatenate([np.abs(d[det][np.isfinite(d[det])]) for _, _, _, d in diff_maps])
            diff_lims[det] = float(np.clip(np.percentile(pooled, 98), 0.02, 10.0))

        for ep_idx, cand_run, val_run, diffs in diff_maps:
            tasks.append((
                worker_plot_time_diff,
                (ep_idx, cand_run, val_run, diffs, diff_lims, dir_diff),
                f"TimeDiff Epoch {ep_idx}",
            ))

        csv_path = dir_diff / "time_diff_summary.csv"
        pd.DataFrame(summary_records).to_csv(csv_path, index=False)
        print(f"  Saved raw time difference summary: {csv_path}")

    return tasks, tmaps


def linear_fit_stats(x: np.ndarray, y: np.ndarray) -> Dict[str, float]:
    """Pearson r and least-squares line y = slope * x + intercept over towers valid in both x and y."""
    ok = np.isfinite(x) & np.isfinite(y)
    x, y = x[ok], y[ok]
    stats = {"n_towers": int(len(x)), "pearson_r": float("nan"), "slope": float("nan"), "intercept": float("nan")}
    if len(x) >= 3 and np.std(x) > 0 and np.std(y) > 0:
        stats["pearson_r"] = float(np.corrcoef(x, y)[0, 1])
        stats["slope"], stats["intercept"] = (float(v) for v in np.polyfit(x, y, 1))
    return stats


def padded_range(vals: np.ndarray, lo_pct: float = 0.5, hi_pct: float = 99.5, min_span: float = 1e-3) -> Tuple[float, float]:
    """Percentile range of the finite values, padded by 5% on each side."""
    lo, hi = np.percentile(vals[np.isfinite(vals)], [lo_pct, hi_pct])
    span = max(hi - lo, min_span)
    return float(lo - 0.05 * span), float(hi + 0.05 * span)




def worker_plot_sf_vs_time(args: Tuple) -> str:
    """
    Worker task to render tower-by-tower 2D histograms of a scale factor quantity (y) against a raw time
    quantity (x), one panel per sample window, with a mean-y profile overlay and linear fit stats.
    Optional polynomial fit curves ({win_key: np.polyfit coefficients}) are drawn on top.
    """
    det, header, x, ys, stats, xlim, ylims, xlabel, ylabel, ref, fits, out_path = args
    neta = DETECTOR_CONFIG[det]["neta"]
    nbins_x, nbins_y, nbins_prof = (80, 60, 24) if neta > 24 else (40, 30, 16)

    fig, axes = plt.subplots(1, len(SF_PANEL_ORDER), figsize=(24, 7.4))
    for ax, w_key in zip(axes, SF_PANEL_ORDER):
        win = WINDOW_BY_KEY[w_key]
        y = ys[w_key]
        ok = np.isfinite(x) & np.isfinite(y)
        xv, yv = x[ok], y[ok]

        _, _, _, im = ax.hist2d(
            xv, yv, bins=[nbins_x, nbins_y], range=[xlim, ylims[w_key]],
            cmap="viridis", norm=LogNorm(), cmin=1,
        )
        ax.set_box_aspect(1)
        # Inset colour bar tracks the square axes exactly (make_axes_locatable does not with a fixed box aspect)
        cax = ax.inset_axes([1.03, 0.0, 0.06, 1.0])
        cbar = plt.colorbar(im, cax=cax)
        cbar.set_label("Towers", fontsize=15)
        cbar.ax.tick_params(labelsize=13)

        # Profile: mean y and its standard error in x bins with at least 5 towers
        edges = np.linspace(*xlim, nbins_prof + 1)
        idx = np.digitize(xv, edges) - 1
        centers, means, sems = [], [], []
        for b in range(nbins_prof):
            sel = yv[idx == b]
            if len(sel) >= 5:
                centers.append(0.5 * (edges[b] + edges[b + 1]))
                means.append(sel.mean())
                sems.append(sel.std() / np.sqrt(len(sel)))
        ax.errorbar(
            centers, means, yerr=sems, fmt="o", markersize=4.5, color="black",
            markerfacecolor="white", markeredgewidth=1.2, elinewidth=1.2, capsize=2.5, label="Profile (mean ± SEM)",
        )

        if ref.get("x") is not None:
            ax.axvline(ref["x"], color="#555555", ls="--", lw=1.0)
        if ref.get("y") is not None:
            ax.axhline(ref["y"], color="#555555", ls="--", lw=1.0)
        if fits.get(w_key) is not None:
            x_curve = np.linspace(*xlim, 200)
            ax.plot(x_curve, np.polyval(fits[w_key], x_curve), color="#e15759", lw=2.2, label=f"{TIME_FIT_LABEL} fit")

        s = stats[w_key]
        ax.text(
            0.03, 0.97,
            f"{win['label']}\n"
            f"r = {s['pearson_r']:+.3f}  |  slope = {s['slope']:+.4f} / sample\n"
            f"Towers: {s['n_towers']:,}",
            transform=ax.transAxes, fontsize=13, weight="bold", color="black", va="top",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="#cccccc", alpha=0.85)
        )
        ax.set_xlim(*xlim)
        ax.set_ylim(*ylims[w_key])
        ax.set_xlabel(xlabel, fontsize=17)
        # Plain substitution: LaTeX braces in the label would break str.format
        ax.set_ylabel(ylabel.replace("{label}", win["label"]), fontsize=16)
        ax.legend(loc="lower right", frameon=True, framealpha=0.85, fontsize=13)

    fig.tight_layout()
    # Anchor the title just above the actual (square) panel tops, which tight_layout does not account for
    fig.canvas.draw()
    panel_top = max(ax.get_position().y1 for ax in axes)
    fig.suptitle(header, fontsize=19, weight="bold", y=panel_top + 0.025, va="bottom")
    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def build_sf_vs_time_tasks(
    detectors: List[str],
    epochs: List[Tuple[int, int, int]],
    candidates: Dict[int, str],
    val_runs: Dict[int, str],
    sf_maps: Dict[str, Dict[int, Dict[str, np.ndarray]]],
    tmaps: Dict[int, Dict[str, np.ndarray]],
    output_dir: Path,
) -> List[Tuple]:
    """
    Build tower-by-tower correlation tasks: SF_win vs mean raw time per candidate / validation run, and
    SF_val / SF_cand vs t_val - t_cand per epoch pairing. Writes a combined fit summary CSV.
    """
    print("\nBuilding scale factor vs raw time correlations...")
    dir_cand = output_dir / "candidate"
    dir_val = output_dir / "validation"
    dir_epoch = output_dir / "epoch_ratio_vs_dt"

    tasks = []
    summary_records = []
    for det in detectors:
        runs = sorted(r for r in sf_maps.get(det, {}) if r in tmaps)
        if not runs:
            continue
        sf_runs = sf_maps[det]

        # Axis ranges shared across all runs of this detector (x) and per window (y)
        xlim = padded_range(np.concatenate([tmaps[r][det].ravel() for r in runs]))
        ylims = {
            w_key: padded_range(np.concatenate([sf_runs[r][w_key].ravel() for r in runs]))
            for w_key in SF_PANEL_ORDER
        }

        for run in runs:
            role, role_dir = ("Candidate", dir_cand) if run in candidates else ("Validation", dir_val)
            role_dir.mkdir(parents=True, exist_ok=True)
            ep_idx = get_epoch_for_run(run, epochs)
            ep_str = f"Epoch {ep_idx}" if ep_idx is not None else "Unassigned"
            x = tmaps[run][det].ravel()
            ys = {w_key: sf_runs[run][w_key].ravel() for w_key in SF_PANEL_ORDER}
            stats = {w_key: linear_fit_stats(x, ys[w_key]) for w_key in SF_PANEL_ORDER}
            for w_key in SF_PANEL_ORDER:
                summary_records.append({
                    "kind": role, "detector": det, "epoch": ep_idx, "run": run,
                    "cand_run": None, "val_run": None, "window": w_key, **stats[w_key],
                })
            # Candidate runs show the polynomial used for the time-fit ±5 ns calibration
            fits = (
                {w_key: fit_sf_vs_time_poly(x, ys[w_key]) for w_key in TIME_FIT_WINDOWS}
                if role == "Candidate" else {}
            )
            tasks.append((
                worker_plot_sf_vs_time,
                (
                    det, f"{det}  |  {role} Run {run} ({ep_str})  |  Scale Factor vs Mean Raw Time",
                    x, ys, stats, xlim, ylims,
                    "Mean Raw Tower Time [samples]", "{label} / T0 Integrated", {}, fits,
                    role_dir / f"sf_vs_time_{det}_run{run}.png",
                ),
                f"SFvsTime {det} Run {run}",
            ))

        # Per-epoch: SF_val / SF_cand vs t_val - t_cand
        cand_runs = [r for r in runs if r in candidates]
        val_only = [r for r in runs if r in val_runs]
        pairs = pair_runs_by_epoch(cand_runs, val_only, epochs, f"{det} SF ratio vs time difference")
        if not pairs:
            continue
        dir_epoch.mkdir(parents=True, exist_ok=True)

        pair_data = []
        for ep_idx, cand_run, val_run in pairs:
            dx = (tmaps[val_run][det] - tmaps[cand_run][det]).ravel()
            dys = {w_key: (sf_runs[val_run][w_key] / sf_runs[cand_run][w_key]).ravel() for w_key in SF_PANEL_ORDER}
            pair_data.append((ep_idx, cand_run, val_run, dx, dys))

        dxlim = padded_range(np.concatenate([d[3] for d in pair_data]))
        dylims = {
            w_key: padded_range(np.concatenate([d[4][w_key] for d in pair_data]), min_span=2e-3)
            for w_key in SF_PANEL_ORDER
        }
        for ep_idx, cand_run, val_run, dx, dys in pair_data:
            stats = {w_key: linear_fit_stats(dx, dys[w_key]) for w_key in SF_PANEL_ORDER}
            for w_key in SF_PANEL_ORDER:
                summary_records.append({
                    "kind": "EpochPair", "detector": det, "epoch": ep_idx, "run": None,
                    "cand_run": cand_run, "val_run": val_run, "window": w_key, **stats[w_key],
                })
            tasks.append((
                worker_plot_sf_vs_time,
                (
                    det, f"{det}  |  Epoch {ep_idx}  |  Val Run {val_run} vs Cand Run {cand_run}  |  SF Ratio vs Time Difference",
                    dx, dys, stats, dxlim, dylims,
                    r"$t_\mathrm{val} - t_\mathrm{cand}$ [samples]",
                    r"$\mathrm{SF}_\mathrm{val} / \mathrm{SF}_\mathrm{cand}$ ({label})",
                    {"x": 0.0, "y": 1.0}, {},
                    dir_epoch / f"sf_ratio_vs_dt_{det}_epoch{ep_idx}_val{val_run}_cand{cand_run}.png",
                ),
                f"SFRatioVsDt {det} Epoch {ep_idx}",
            ))

    if summary_records:
        output_dir.mkdir(parents=True, exist_ok=True)
        csv_path = output_dir / "sf_vs_time_summary.csv"
        pd.DataFrame(summary_records).to_csv(csv_path, index=False)
        print(f"  Saved scale factor vs time fit summary: {csv_path}")

    return tasks


def ratio_stats(r: np.ndarray) -> Dict[str, float]:
    """Mean, RMS and fractions within ±2% / ±5% of 1.0 for a CDB / Hist ratio array."""
    return {
        "mean_ratio": float(np.mean(r)),
        "std_ratio": float(np.std(r)),
        "frac_within_2pct": float(np.mean(np.abs(r - 1.0) <= 0.02)),
        "frac_within_5pct": float(np.mean(np.abs(r - 1.0) <= 0.05)),
    }


def worker_plot_time_fit_ratios(args: Tuple) -> str:
    """Worker task to render val_ratios-style CDB / Hist distributions comparing current vs raw-time-fit ±5 ns."""
    header, ratios, n_fallback, out_path = args
    methods = (
        ("current", "Current (candidate SF)", "#7f7f7f", "--", 2.0),
        ("time_fit", f"Raw-time {TIME_FIT_LABEL.lower()} fit", None, "-", 2.4),
    )

    fig, axes = plt.subplots(1, len(TIME_FIT_WINDOWS), figsize=(22, 8))
    for ax, w_key in zip(axes, TIME_FIT_WINDOWS):
        win = WINDOW_BY_KEY[w_key]
        both = np.concatenate([ratios[w_key][m] for m, *_ in methods])
        p05, p995 = np.percentile(both, [0.5, 99.5])
        r_dev = max(abs(1.0 - p05), abs(p995 - 1.0), 0.05)
        bins = np.linspace(1.0 - 1.25 * r_dev, 1.0 + 1.25 * r_dev, 80)

        for m_key, m_label, color, ls, lw in methods:
            r = ratios[w_key][m_key]
            s = ratio_stats(r)
            ax.hist(
                r, bins=bins, histtype="step", color=color or win["color"], ls=ls, lw=lw,
                label=(
                    f"{m_label}\n{s['mean_ratio']:.4f} ± {s['std_ratio']:.4f}  |  "
                    f"±2%: {s['frac_within_2pct'] * 100:.1f}%  |  ±5%: {s['frac_within_5pct'] * 100:.1f}%"
                ),
            )

        ax.axvline(1.0, color="#555555", ls="--", lw=1.0)
        ax.axvline(0.95, color="#888888", ls=":", lw=1.0, alpha=0.7)
        ax.axvline(1.05, color="#888888", ls=":", lw=1.0, alpha=0.7)
        ax.set_xlabel(r"Ratio ($\mathrm{CDB} / \mathrm{Hist}$)", fontsize=17)
        ax.set_ylabel(r"Towers / Bin", fontsize=16)
        ax.set_yscale("log")
        ax.set_ylim(0.5, ax.get_ylim()[1] * 60.0)
        ax.legend(loc="upper right", frameon=True, framealpha=0.9, fontsize=13)
        ax.text(
            0.03, 0.97,
            f"{win['label']}\nTowers: {len(ratios[w_key]['current']):,}\nTime-fit fallback: {n_fallback[w_key]:,}",
            transform=ax.transAxes, fontsize=13, weight="bold", color="black", va="top",
            bbox=dict(boxstyle="round,pad=0.25", facecolor="white", edgecolor="#cccccc", alpha=0.85)
        )

    fig.suptitle(header, fontsize=19, weight="bold")
    fig.tight_layout()
    fig.savefig(out_path, dpi=200, bbox_inches="tight")
    plt.close(fig)
    return out_path.name


def build_time_fit_tasks(
    detectors: List[str],
    epochs: List[Tuple[int, int, int]],
    candidates: Dict[int, str],
    val_runs: Dict[int, str],
    sf_maps: Dict[str, Dict[int, Dict[str, np.ndarray]]],
    tmaps: Dict[int, Dict[str, np.ndarray]],
    output_dir: Path,
) -> List[Tuple]:
    """
    Raw-time ±5 ns calibration study. Per (detector, epoch, window) a polynomial SF(t) is fit on the candidate
    run and applied to each validation run's raw time: CDB_fit = f(t_val) * T0_val, so CDB / Hist = f(t_val) / SF_val.
    The current method gives SF_cand / SF_val. Towers without a valid prediction fall back to the current method.
    Both are compared on the same towers; fit coefficients and per-run statistics are written to CSV.
    """
    print(f"\nBuilding raw-time {TIME_FIT_LABEL.lower()} fit validation for ±5 ns windows...")
    dir_runs = output_dir / "per_run"

    tasks = []
    fit_records = []
    summary_records = []
    for det in detectors:
        runs = [r for r in sf_maps.get(det, {}) if r in tmaps]
        pairs = pair_runs_by_epoch(
            [r for r in runs if r in candidates], [r for r in runs if r in val_runs], epochs, f"{det} time-fit validation"
        )
        if not pairs:
            continue
        dir_runs.mkdir(parents=True, exist_ok=True)

        pooled = {w_key: {"current": [], "time_fit": []} for w_key in TIME_FIT_WINDOWS}
        pooled_fallback = {w_key: 0 for w_key in TIME_FIT_WINDOWS}
        for ep_idx, cand_run, val_run in pairs:
            t_cand, t_val = tmaps[cand_run][det], tmaps[val_run][det]
            run_ratios, run_fallback = {}, {}
            for w_key in TIME_FIT_WINDOWS:
                sf_cand = sf_maps[det][cand_run][w_key]
                sf_val = sf_maps[det][val_run][w_key]

                coef = fit_sf_vs_time_poly(t_cand, sf_cand)
                ok_fit = np.isfinite(t_cand) & np.isfinite(sf_cand)
                t_range = (float(np.min(t_cand[ok_fit])), float(np.max(t_cand[ok_fit]))) if np.any(ok_fit) else (np.nan, np.nan)
                resid = sf_cand[ok_fit] - np.polyval(coef, t_cand[ok_fit]) if coef is not None else np.array([np.nan])
                fit_records.append({
                    "detector": det, "epoch": ep_idx, "cand_run": cand_run, "window": w_key,
                    **{
                        f"p{TIME_FIT_ORDER - i}": coef[i] if coef is not None else np.nan
                        for i in range(TIME_FIT_ORDER + 1)
                    },
                    "t_min": t_range[0], "t_max": t_range[1],
                    "n_fit_towers": int(np.sum(ok_fit)),
                    "fit_resid_rms": float(np.std(resid)),
                })

                # Compare both methods on the towers the current method calibrates.
                # Prediction matches production: t clamped to the candidate's fitted range.
                mask = np.isfinite(sf_val) & np.isfinite(sf_cand)
                pred = predict_sf_from_time(coef, t_val, t_range) if coef is not None else np.full_like(sf_val, np.nan)
                use_fit = mask & np.isfinite(pred)
                sf_new = np.where(use_fit, pred, sf_cand)

                current = (sf_cand / sf_val)[mask]
                time_fit = (sf_new / sf_val)[mask]
                n_fallback = int(np.sum(mask) - np.sum(use_fit))
                run_ratios[w_key] = {"current": current, "time_fit": time_fit}
                run_fallback[w_key] = n_fallback
                pooled[w_key]["current"].append(current)
                pooled[w_key]["time_fit"].append(time_fit)
                pooled_fallback[w_key] += n_fallback

                for method, r in (("current", current), ("time_fit", time_fit)):
                    summary_records.append({
                        "detector": det, "epoch": ep_idx, "cand_run": cand_run, "val_run": val_run,
                        "window": w_key, "method": method, "n_towers": int(len(r)),
                        "n_fallback": n_fallback if method == "time_fit" else 0, **ratio_stats(r),
                    })

            tasks.append((
                worker_plot_time_fit_ratios,
                (
                    f"{det}  |  Epoch {ep_idx}  |  Val Run {val_run} (Cand Run {cand_run})  |  ±5 ns CDB / Hist: Current vs Raw-Time Fit",
                    run_ratios, run_fallback, dir_runs / f"val_ratios_timefit_{det}_run{val_run}.png",
                ),
                f"TimeFitRatios {det} Run {val_run}",
            ))

        combined = {w_key: {m: np.concatenate(v) for m, v in pooled[w_key].items()} for w_key in TIME_FIT_WINDOWS}
        tasks.append((
            worker_plot_time_fit_ratios,
            (
                f"{det}  |  {len(pairs)} Validation Runs (Combined)  |  ±5 ns CDB / Hist: Current vs Raw-Time Fit",
                combined, pooled_fallback, output_dir / f"val_ratios_timefit_{det}.png",
            ),
            f"TimeFitRatios {det} Combined",
        ))

    if fit_records:
        output_dir.mkdir(parents=True, exist_ok=True)
        pd.DataFrame(fit_records).to_csv(output_dir / "fit_coefficients.csv", index=False)
        pd.DataFrame(summary_records).to_csv(output_dir / "time_fit_summary.csv", index=False)
        print(f"  Saved fit coefficients and time-fit summary to: {output_dir}")

    return tasks


def execute_plotting_task(task_spec: Tuple) -> Tuple[str, Any]:
    """Top-level task dispatcher for ProcessPoolExecutor."""
    fn, args, label = task_spec
    result = fn(args)
    return label, result


def plot_all_validation_parallel(
    df: pd.DataFrame,
    epochs: List[Tuple[int, int, int]],
    input_dir: Path,
    hist_list_path: Path,
    output_dir: Path,
    detectors: List[str],
    spatial_runs: Optional[List[int]] = None,
    val_hist_list_path: Optional[Path] = None,
    val_ylim_residual: Optional[Tuple[float, float]] = None,
    val_ylim_ratio: Optional[Tuple[float, float]] = None,
    time_maps: bool = True,
    jobs: int = 8,
):
    """Orchestrate all validation plotting tasks in parallel across worker processes."""
    tasks = []

    # Subdirectories for organized outputs
    dir_trends = output_dir / "trends"
    dir_dead_towers = output_dir / "dead_towers"
    dir_distributions = output_dir / "distributions"
    dir_closure = output_dir / "closure"
    dir_spatial = output_dir / "spatial_maps"

    for d in [dir_trends, dir_dead_towers, dir_distributions, dir_closure, dir_spatial]:
        d.mkdir(parents=True, exist_ok=True)

    # Load candidate runs from hist list
    candidates = load_candidate_runs(hist_list_path)
    candidate_runs = sorted(candidates.keys()) if candidates else []

    # 1. Trend plots
    for det in detectors:
        sub_df = df[df["detector"] == det]
        if not sub_df.empty:
            sub_df = sub_df.sort_values(by="run")
            runs = sub_df["run"].values
            orig = sub_df["orig_mean"].values
            main = sub_df["main_mean"].values
            minus5 = sub_df["minus5_mean"].values
            plus5 = sub_df["plus5_mean"].values
            tasks.append((
                worker_plot_trend,
                (det, runs, orig, main, minus5, plus5, epochs, dir_trends, candidate_runs),
                f"Trend {det}"
            ))

    # 2. Dead towers plots
    for det in detectors:
        sub_df = df[df["detector"] == det]
        if not sub_df.empty:
            sub_df = sub_df.sort_values(by="run")
            runs = sub_df["run"].values
            zeros = sub_df["n_zero"].values
            pct = sub_df["zero_frac"].values * 100.0
            n_total = int(sub_df["n_total"].iloc[0])
            tasks.append((worker_plot_dead_towers, (det, runs, zeros, pct, n_total, epochs, dir_dead_towers), f"DeadTowers {det}"))

    # 3. Epoch distribution plots
    for det in detectors:
        pattern = str(input_dir / f"{det}_ZSCrossCalibT0_*_*.root")
        files = glob.glob(pattern)
        if not files:
            continue
        run_file_map = {}
        for f in files:
            m = re.search(r"_(\d+)\.root$", os.path.basename(f))
            if m:
                run_file_map[int(m.group(1))] = f

        for s, e, idx in epochs:
            epoch_runs = [r for r in run_file_map if s <= r <= e]
            if not epoch_runs:
                continue
            sample_runs = np.random.choice(epoch_runs, size=min(10, len(epoch_runs)), replace=False)
            sample_files = [run_file_map[r] for r in sample_runs]
            tasks.append((worker_plot_distribution, (det, s, e, idx, sample_files, dir_distributions), f"Distribution {det} Epoch {idx}"))

    # 4. Candidate closure validation
    if candidates:
        for det in detectors:
            tasks.append((worker_plot_closure, (det, input_dir, candidates, dir_closure), f"Closure {det}"))

    # 5. Spatial 2D heatmaps (configurable via spatial_runs, defaulting to candidate runs)
    if spatial_runs is not None and len(spatial_runs) > 0:
        map_runs = spatial_runs
    elif candidates:
        map_runs = list(candidates.keys())
    else:
        available_runs = df["run"].unique().tolist()
        map_runs = available_runs[:min(5, len(available_runs))]

    for det in detectors:
        for run in map_runs:
            tasks.append((worker_plot_spatial_maps, (det, run, input_dir, dir_spatial), f"SpatialMap {det} Run {run}"))

    # 6. Validation vs independent test hists (when val_hist_list_path is provided)
    has_val = False
    dir_validation = output_dir / "validation"
    val_runs = load_hist_list(val_hist_list_path) if val_hist_list_path else {}
    if val_hist_list_path:
        if val_runs:
            has_val = True
            dir_val_spatial = dir_validation / "spatial_maps"
            dir_val_residuals = dir_validation / "residuals"
            dir_val_ratios = dir_validation / "ratios"
            dir_validation.mkdir(parents=True, exist_ok=True)
            dir_val_spatial.mkdir(parents=True, exist_ok=True)
            dir_val_residuals.mkdir(parents=True, exist_ok=True)
            dir_val_ratios.mkdir(parents=True, exist_ok=True)

            print(f"\nQueuing validation comparisons for {len(val_runs)} test run(s) across {len(detectors)} detector(s)...")

            for det in detectors:
                tasks.append((
                    worker_plot_validation_residuals,
                    (det, input_dir, val_runs, epochs, dir_validation, dir_val_residuals, dir_val_ratios),
                    f"ValResiduals {det}",
                ))
                for run, hist_file in sorted(val_runs.items()):
                    tasks.append((
                        worker_plot_val_spatial_maps,
                        (det, run, hist_file, input_dir, dir_val_spatial),
                        f"ValSpatialMap {det} Run {run}",
                    ))
        else:
            print(f"\nWarning: No valid runs parsed from validation hist list: {val_hist_list_path}")

    # 7. Scale factor maps (H_win / T0 Integrated) and per-epoch SF_val / SF_cand ratios
    sf_maps = {}
    if candidates or val_runs:
        sf_tasks, sf_maps = build_scale_factor_tasks(
            detectors, epochs, input_dir, candidates, val_runs, output_dir / "scale_factor_maps"
        )
        tasks.extend(sf_tasks)

    # 8. Raw tower time maps and per-epoch t_val - t_cand differences
    tmaps = {}
    if time_maps and (candidates or val_runs):
        time_tasks, tmaps = build_time_map_tasks(detectors, epochs, candidates, val_runs, output_dir / "time_maps")
        tasks.extend(time_tasks)

    # 9. Tower-by-tower scale factor vs raw time correlations
    if sf_maps and tmaps:
        tasks.extend(build_sf_vs_time_tasks(
            detectors, epochs, candidates, val_runs, sf_maps, tmaps, output_dir / "sf_vs_time"
        ))

    # 10. ±5 ns calibration from a per-epoch raw-time polynomial fit, validated against independent runs
    if sf_maps and tmaps and val_runs:
        tasks.extend(build_time_fit_tasks(
            detectors, epochs, candidates, val_runs, sf_maps, tmaps, output_dir / "time_fit"
        ))

    print(f"\nDispatching {len(tasks)} plotting tasks across {jobs} parallel worker process(es)...")

    t_start = time.time()
    saved_plots = []

    if jobs > 1 and len(tasks) > 1:
        with concurrent.futures.ProcessPoolExecutor(max_workers=jobs) as executor:
            futures = [executor.submit(execute_plotting_task, t) for t in tasks]
            for future in tqdm.tqdm(concurrent.futures.as_completed(futures), total=len(futures), desc="Rendering plots"):
                try:
                    label, result = future.result()
                    if result:
                        if isinstance(result, list):
                            saved_plots.extend(result)
                        else:
                            saved_plots.append(result)
                except Exception as err:
                    print(f"Error in plotting task: {err}")
    else:
        for t in tqdm.tqdm(tasks, desc="Rendering plots"):
            try:
                label, result = execute_plotting_task(t)
                if result:
                    if isinstance(result, list):
                        saved_plots.extend(result)
                    else:
                        saved_plots.append(result)
            except Exception as err:
                print(f"Error in plotting task: {err}")

    # Synchronize and render per-run trend plots across detectors
    if has_val:
        print("\nSynchronizing validation trend plots (val_trend_by_run) across detectors...")
        det_summaries = {}
        for det in detectors:
            sum_csv = dir_validation / f"val_summary_{det}.csv"
            if sum_csv.is_file():
                try:
                    df_rec = pd.read_csv(sum_csv)
                    if not df_rec.empty:
                        det_summaries[det] = df_rec
                except Exception as err:
                    print(f"Warning: Could not read {sum_csv}: {err}")

        if det_summaries:
            if val_ylim_residual is not None:
                global_ylim_residual = val_ylim_residual
            else:
                all_diff_lows = []
                all_diff_highs = []
                for df_rec in det_summaries.values():
                    all_diff_lows.extend((df_rec["mean_diff"] - df_rec["std_diff"]).tolist())
                    all_diff_highs.extend((df_rec["mean_diff"] + df_rec["std_diff"]).tolist())
                y_min_top = min(all_diff_lows) if all_diff_lows else -0.05
                y_max_top = max(all_diff_highs) if all_diff_highs else 0.05
                y_span_top = max(y_max_top - y_min_top, 0.02)
                global_ylim_residual = (y_min_top - 0.15 * y_span_top, y_max_top + 0.45 * y_span_top)

            if val_ylim_ratio is not None:
                global_ylim_ratio = val_ylim_ratio
            else:
                all_ratio_lows = []
                all_ratio_highs = []
                for df_rec in det_summaries.values():
                    all_ratio_lows.extend((df_rec["mean_ratio"] - df_rec["std_ratio"]).tolist())
                    all_ratio_highs.extend((df_rec["mean_ratio"] + df_rec["std_ratio"]).tolist())
                y_min_bot = min(all_ratio_lows) if all_ratio_lows else 0.95
                y_max_bot = max(all_ratio_highs) if all_ratio_highs else 1.05
                r_min = min(y_min_bot, 0.95)
                r_max = max(y_max_bot, 1.05)
                r_span = max(r_max - r_min, 0.10)
                global_ylim_ratio = (r_min - 0.08 * r_span, r_max + 0.45 * r_span)

            print(f"  Synchronized y-axis ranges ({len(det_summaries)} detector(s)):")
            print(f"    - Residual (CDB - Hist): [{global_ylim_residual[0]:.4f}, {global_ylim_residual[1]:.4f}]")
            print(f"    - Ratio (CDB / Hist):    [{global_ylim_ratio[0]:.4f}, {global_ylim_ratio[1]:.4f}]")

            for det, df_rec in det_summaries.items():
                trend_plots = render_val_trend_by_run(
                    det=det,
                    df_records=df_rec,
                    epochs=epochs,
                    output_dir=dir_validation,
                    ylim_residual=global_ylim_residual,
                    ylim_ratio=global_ylim_ratio,
                )
                saved_plots.extend(trend_plots)

    elapsed = time.time() - t_start
    print(f"\nSuccessfully rendered and saved {len(saved_plots)} plot files in {elapsed:.2f} seconds!")


def main():
    args = parse_arguments()

    if args.val_hist_list and not args.val_hist_list.is_file():
        sys.exit(f"Error: Validation hist list file not found: {args.val_hist_list}")

    epochs = load_epochs(args.epoch_file)
    detectors = [d.strip().upper() for d in args.detectors.split(",") if d.strip()]
    val_ylim_residual = parse_range(args.val_ylim_residual)
    val_ylim_ratio = parse_range(args.val_ylim_ratio)

    spatial_runs = None
    if args.spatial_runs:
        try:
            spatial_runs = [int(r.strip()) for r in args.spatial_runs.split(",") if r.strip()]
        except ValueError as err:
            sys.exit(f"Error parsing --spatial-runs: {err}")

    print("=" * 80)
    print(" ZSCrossCalib Validation Plotting")
    print("=" * 80)
    print(f"Summary CSV:       {args.summary_csv}")
    print(f"CDB Input Dir:     {args.input_dir}")
    print(f"Candidate Hist:    {args.hist_list}")
    if args.val_hist_list:
        print(f"Validation Hist:   {args.val_hist_list}")
    if val_ylim_residual:
        print(f"Val Res Y-Lim:     {val_ylim_residual}")
    if val_ylim_ratio:
        print(f"Val Ratio Y-Lim:   {val_ylim_ratio}")
    print(f"Output Plot Dir:   {args.output_dir}")
    print(f"Detectors:         {', '.join(detectors)}")
    print(f"Spatial Map Runs:  {', '.join(map(str, spatial_runs)) if spatial_runs else 'Auto (candidate runs)'}")
    print("-" * 80)

    # Automatically call extract_summary if CSV doesn't exist or --force-extract requested
    if args.summary_csv.is_file() and not args.force_extract:
        print(f"Loading existing summary CSV: {args.summary_csv}")
        df = pd.read_csv(args.summary_csv)
    else:
        if args.force_extract:
            print(f"--force-extract specified. Extracting metrics from {args.input_dir}...")
        else:
            print(f"Summary CSV not found at {args.summary_csv}. Calling extract_calib_summary...")
        df = extract_summary(
            input_dir=args.input_dir,
            output_file=args.summary_csv,
            epoch_file=args.epoch_file,
            jobs=args.jobs,
        )

    print(f"Loaded {len(df)} records across {df['run'].nunique()} unique runs.")

    args.output_dir.mkdir(parents=True, exist_ok=True)

    # Execute all plotting tasks in parallel across worker processes
    plot_all_validation_parallel(
        df=df,
        epochs=epochs,
        input_dir=args.input_dir,
        hist_list_path=args.hist_list,
        output_dir=args.output_dir,
        detectors=detectors,
        spatial_runs=spatial_runs,
        val_hist_list_path=args.val_hist_list,
        val_ylim_residual=val_ylim_residual,
        val_ylim_ratio=val_ylim_ratio,
        time_maps=not args.no_time_maps,
        jobs=args.jobs,
    )

    print("\n" + "=" * 80)
    print(f"All validation plots successfully generated in: {args.output_dir}")
    print(f"  - Trends:        {args.output_dir / 'trends'}")
    print(f"  - Dead Towers:   {args.output_dir / 'dead_towers'}")
    print(f"  - Distributions: {args.output_dir / 'distributions'}")
    print(f"  - Closure:       {args.output_dir / 'closure'}")
    print(f"  - Spatial Maps:  {args.output_dir / 'spatial_maps'}")
    print(f"  - SF Maps:       {args.output_dir / 'scale_factor_maps'}")
    if not args.no_time_maps:
        print(f"  - Time Maps:     {args.output_dir / 'time_maps'}")
        print(f"  - SF vs Time:    {args.output_dir / 'sf_vs_time'}")
        if args.val_hist_list:
            print(f"  - Time Fit:      {args.output_dir / 'time_fit'}")
    if args.val_hist_list:
        print(f"  - Validation:    {args.output_dir / 'validation'}")
        print(f"    * Combined:    {args.output_dir / 'validation'}")
        print(f"    * Residuals:   {args.output_dir / 'validation' / 'residuals'}")
        print(f"    * Ratios:      {args.output_dir / 'validation' / 'ratios'}")
        print(f"    * Spatial Maps:{args.output_dir / 'validation' / 'spatial_maps'}")
        print(f"  - SF Epoch Ratio:{args.output_dir / 'scale_factor_maps' / 'epoch_ratio'}")
    print("=" * 80)


if __name__ == "__main__":
    main()
