#!/usr/bin/env python3

import uproot
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import mplhep as hep
import argparse
from pathlib import Path
import numpy as np
import sys
from scipy.optimize import curve_fit

def gaussian(x, a, mu, sigma):
    return a * np.exp(-0.5 * ((x - mu) / sigma)**2)

def plot_2d_hist(hist, output_dir, name, suffix=""):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(10, 8))

    # Get values and edges
    values, edges_x, edges_y = hist.to_numpy()
    errors = hist.errors()

    # Calculate sigmas for each channel
    sigmas = []
    fit_sigmas = []
    y_centers = (edges_y[:-1] + edges_y[1:]) / 2
    for i in range(values.shape[0]):
        counts = values[i, :]
        errs = errors[i, :]
        total_counts = np.sum(counts)
        if total_counts > 0:
            mean = np.sum(counts * y_centers) / total_counts
            variance = np.sum(counts * (y_centers - mean)**2) / total_counts
            if variance > 0:
                calc_sigma = np.sqrt(variance)
                sigmas.append(calc_sigma)

                # Fit Gaussian
                mask = errs > 0
                if np.sum(mask) >= 3:
                    xdata = y_centers[mask]
                    ydata = counts[mask]
                    sigma_data = errs[mask]

                    max_idx = np.argmax(ydata)
                    peak_y = xdata[max_idx]
                    peak_val = ydata[max_idx]

                    half_max = peak_val / 2.0
                    above_half = (ydata >= half_max)
                    if np.sum(above_half) > 1:
                        fwhm = xdata[above_half][-1] - xdata[above_half][0] + (edges_y[1] - edges_y[0])
                    else:
                        fwhm = 10.0
                    sigma_est = max(fwhm / 2.355, 1.0)

                    win = max(0.5 * fwhm, 4.0)
                    fit_mask = mask & (np.abs(y_centers - peak_y) <= win)
                    if np.sum(fit_mask) >= 3:
                        p0 = [peak_val, peak_y, sigma_est]
                        try:
                            popt, _ = curve_fit(gaussian, y_centers[fit_mask], counts[fit_mask], p0=p0, sigma=errs[fit_mask], absolute_sigma=True, bounds=([0, -np.inf, 0], [np.inf, np.inf, np.inf]))
                            fit_sigmas.append(abs(popt[2]))
                        except Exception:
                            pass

    # Create meshgrid for pcolormesh
    X, Y = np.meshgrid(edges_x, edges_y)

    # Set 0 values to NaN to avoid issues with log scale
    values = np.where(values == 0, np.nan, values)

    # Use LogNorm for logz
    mesh = ax.pcolormesh(X, Y, values.T, cmap='viridis', norm=mcolors.LogNorm())

    cbar = fig.colorbar(mesh, ax=ax)
    cbar.set_label("Counts")

    # Fetch title from the histogram
    title = hist.title
    if title:
        ax.set_title(title.strip())

    try:
        x_label = hist.axes[0].member('fTitle').strip()
    except Exception:
        x_label = "X"

    try:
        y_label = hist.axes[1].member('fTitle').strip()
    except Exception:
        y_label = "Y"

    ax.set_xlabel(x_label)
    ax.set_ylabel(y_label)
    ax.set_xlim(left=0)
    if "hcal" in name:
        ax.set_xlim(right=1536)
    elif "sepd" in name:
        ax.set_xlim(right=744)

    # # Text info for sPHENIX Internal
    # ax.text(0.05, 0.95, "sPHENIX Internal", transform=ax.transAxes, ha='left', va='top', fontsize=18, fontweight='bold')

    plt.tight_layout()

    image_dir = output_dir / "images"
    image_dir.mkdir(parents=True, exist_ok=True)

    png_path = image_dir / f"{name}{suffix}.png"
    plt.savefig(png_path, dpi=300)
    print(f"Saved plot as {png_path}")

    plt.close(fig)
    return sigmas, fit_sigmas

def plot_sigma_distributions(all_sigmas, output_dir, name="noise_sigma_distributions"):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(10, 8))

    colors = {
        "h2_ihcal_noise": "blue",
        "h2_ohcal_noise": "red",
        "h2_sepd_noise": "green"
    }
    labels = {
        "h2_ihcal_noise": "IHCal",
        "h2_ohcal_noise": "OHCal",
        "h2_sepd_noise": "sEPD"
    }

    all_vals = []
    for sigmas in all_sigmas.values():
        all_vals.extend(sigmas)

    if not all_vals:
        return

    min_val, max_val = min(all_vals), max(all_vals)
    bins = np.linspace(min_val, max_val, 100)

    for hist_name, sigmas in all_sigmas.items():
        if not sigmas:
            continue
        counts, bin_edges = np.histogram(sigmas, bins=bins)
        mean_sigma = np.mean(sigmas)
        base_label = labels.get(hist_name, hist_name)
        label_text = f"{base_label} (avg: {mean_sigma:.2f} ADC)"

        ax.stairs(counts, bin_edges, color=colors.get(hist_name, "black"), label=label_text, linewidth=2)

    ax.set_xlabel("Sigma (ADC)")
    ax.set_ylabel("Channels")
    ax.set_ylim(bottom=0)
    ax.set_title("Noise Sigma Distributions")
    ax.legend(frameon=False, fontsize=24)

    plt.tight_layout()

    image_dir = output_dir / "images"
    image_dir.mkdir(parents=True, exist_ok=True)

    png_path = image_dir / f"{name}.png"
    plt.savefig(png_path, dpi=300)
    print(f"Saved plot as {png_path}")

    plt.close(fig)

def plot_channel_projection(hist, channel, output_dir, name):
    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(10, 8))

    values, edges_x, edges_y = hist.to_numpy()
    errors = hist.errors()

    x_centers = (edges_x[:-1] + edges_x[1:]) / 2
    bin_idx = np.argmin(np.abs(x_centers - channel))

    counts = values[bin_idx, :]
    errs = errors[bin_idx, :]
    y_centers = (edges_y[:-1] + edges_y[1:]) / 2

    mask = errs > 0
    fit_text = f"Channel: {channel}"
    if np.sum(mask) >= 3:
        xdata = y_centers[mask]
        ydata = counts[mask]
        sigma_data = errs[mask]

        max_idx = np.argmax(ydata)
        peak_y = xdata[max_idx]
        peak_val = ydata[max_idx]

        half_max = peak_val / 2.0
        above_half = (ydata >= half_max)
        if np.sum(above_half) > 1:
            fwhm = xdata[above_half][-1] - xdata[above_half][0] + (edges_y[1] - edges_y[0])
        else:
            fwhm = 10.0
        sigma_est = max(fwhm / 2.355, 1.0)

        win = max(0.5 * fwhm, 4.0)
        fit_mask = mask & (np.abs(y_centers - peak_y) <= win)
        if np.sum(fit_mask) >= 3:
            p0 = [peak_val, peak_y, sigma_est]
            try:
                popt, _ = curve_fit(gaussian, y_centers[fit_mask], counts[fit_mask], p0=p0, sigma=errs[fit_mask], absolute_sigma=True, bounds=([0, -np.inf, 0], [np.inf, np.inf, np.inf]))
                fit_a, fit_mu, fit_sigma = popt[0], popt[1], abs(popt[2])

                x_fit = np.linspace(edges_y[0], edges_y[-1], 500)
                ax.plot(x_fit, gaussian(x_fit, fit_a, fit_mu, fit_sigma), color='red', linestyle='--', linewidth=2, label='Gaussian Fit')

                fit_text += f"\nMean: {fit_mu:.2f}\nSigma: {fit_sigma:.2f}"
            except Exception as e:
                print(f"Fit failed for {name} channel {channel}: {e}")

    ax.errorbar(y_centers[mask], counts[mask], yerr=errs[mask], fmt='o', color='black', label='Data', markersize=4, capsize=2)

    try:
        y_label = hist.axes[1].member('fTitle').strip()
    except Exception:
        y_label = "ADC"

    ax.set_xlabel(y_label)
    ax.set_ylabel("Counts")

    title_str = hist.title.strip() if hist.title else name
    ax.set_title(f"{title_str} - Channel {channel}")
    ax.set_xlim(left=-50, right=50)
    ax.set_ylim(bottom=0)
    ax.legend(frameon=False, fontsize=16)

    ax.text(0.05, 0.85, fit_text, transform=ax.transAxes, ha='left', va='top', fontsize=18)

    plt.tight_layout()

    image_dir = output_dir / "images"
    image_dir.mkdir(parents=True, exist_ok=True)

    png_path = image_dir / f"{name}_channel_{channel}.png"
    plt.savefig(png_path, dpi=300)
    print(f"Saved channel projection plot as {png_path}")

    plt.close(fig)

def main():
    parser = argparse.ArgumentParser(description="Plot 2D noise histograms from a ROOT file.")
    parser.add_argument("file", type=Path, help="Path to the input ROOT file")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")

    args = parser.parse_args()

    input_file = args.file
    if not input_file.exists():
        print(f"Error: File not found: {input_file}")
        sys.exit(1)

    hist_names = ["h2_ihcal_noise", "h2_ohcal_noise", "h2_sepd_noise"]
    channel_targets = {
        "h2_sepd_noise": 259,
        "h2_ihcal_noise": 517,
        "h2_ohcal_noise": 517
    }

    try:
        all_sigmas = {}
        all_fit_sigmas = {}
        with uproot.open(input_file) as f:
            for hist_name in hist_names:
                if hist_name in f:
                    hist = f[hist_name]
                    sigmas, fit_sigmas = plot_2d_hist(hist, args.output_dir, hist_name)
                    all_sigmas[hist_name] = sigmas
                    all_fit_sigmas[hist_name] = fit_sigmas

                    if hist_name in channel_targets:
                        plot_channel_projection(hist, channel_targets[hist_name], args.output_dir, hist_name)
                else:
                    print(f"Warning: Histogram {hist_name} not found in {input_file}")

        if all_sigmas:
            plot_sigma_distributions(all_sigmas, args.output_dir, "noise_sigma_distributions")
        if all_fit_sigmas:
            plot_sigma_distributions(all_fit_sigmas, args.output_dir, "noise_fit_sigma_distributions")
    except Exception as e:
        print(f"Error opening or processing {input_file}: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
