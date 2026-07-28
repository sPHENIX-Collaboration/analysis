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
import sys
import csv
from pathlib import Path

def process_file(path):
    path = Path(path)
    if not path.exists():
        return None, None, None, f"File not found: {path}"

    try:
        run_number = int(path.name.split('.')[0])
        with uproot.open(path) as file:
            if "hLuminosity" in file:
                hist = file["hLuminosity"]
                lumi = hist.values()[0]

                title_str = None
                try:
                    title = hist.member("fYaxis").member("fTitle")
                    if title:
                        title_str = title.replace("#mu", "\\mu").replace("#", "\\")
                except Exception:
                    pass

                return run_number, lumi, title_str, None
            else:
                return None, None, None, f"hLuminosity not found in {path}"
    except Exception as e:
        return None, None, None, f"Error processing {path}: {e}"

def main():
    parser = argparse.ArgumentParser(description="Plot luminosity per run from a list of ROOT files.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing ROOT file paths (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the plots (default: current directory).")
    parser.add_argument("-n", "--name", "--plot-name", dest="name", type=str, default="luminosity_per_run", help="Base filename for output plots (default: luminosity_per_run).")
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

    results_data = []
    y_axis_title = "Luminosity"

    print(f"Found {len(file_list)} files to process.")

    # Use ProcessPoolExecutor to read files in parallel
    max_workers = min(os.cpu_count() or 4, 32)
    with concurrent.futures.ProcessPoolExecutor(max_workers=max_workers) as executor:
        results = list(tqdm.tqdm(executor.map(process_file, file_list), total=len(file_list)))

    for run_num, lumi, title_str, err in results:
        if err:
            print(err)
        elif run_num is not None:
            lumi_ub_inv = round(lumi * 1000, 4)
            results_data.append({'Run': run_num, 'Lumi': lumi, 'Lumi_ub_inv': lumi_ub_inv})
            if y_axis_title == "Luminosity" and title_str:
                y_axis_title = title_str

    if not results_data:
        print("No valid data found to plot.")
        return

    # Sort by run number to ensure the plot is ordered
    sorted_results = sorted(results_data, key=lambda x: x['Run'])

    all_runs = []
    all_lumis = []
    original_lumis = []

    for r in sorted_results:
        all_runs.append(r['Run'])
        original_lumi = r['Lumi']
        original_lumis.append(original_lumi)

        # Scale luminosities to inverse microbarns (x 10^3) for the plot
        plot_lumi = original_lumi * 1000
        all_lumis.append(plot_lumi)

    # Update y-axis title to reflect inverse microbarns and ensure valid LaTeX math mode
    if "nb" in y_axis_title:
        y_axis_title = y_axis_title.replace("nb^{-1}", r"$\mu\text{b}^{-1}$").replace("nb", r"$\mu$b")
    elif r"\mu" not in y_axis_title and "$" not in y_axis_title:
        y_axis_title = r"Luminosity [$\mu\text{b}^{-1}$]"
    else:
        # If \mu was fetched from ROOT but lacks $ for math mode, wrap it
        if r"\mu b^{-1}" in y_axis_title:
             y_axis_title = y_axis_title.replace(r"\mu b^{-1}", r"$\mu\text{b}^{-1}$")
        elif r"\mu" in y_axis_title and "$" not in y_axis_title:
             y_axis_title = y_axis_title.replace(r"\mu", r"$\mu$")

    # Plotting using ATLAS style
    hep.style.use("ATLAS")

    fig, ax = plt.subplots(figsize=(10, 6))

    label_data = rf"|z| < 10 cm & MBD Trig"
    ax.plot(all_runs, all_lumis, marker='o', markersize=4, linestyle='none', color='black', label=label_data)

    # Add a light dashed line for the average luminosity
    avg_plot_lumi = sum(all_lumis) / len(all_lumis) if all_lumis else 0
    ax.axhline(avg_plot_lumi, color='gray', linestyle='--', alpha=0.5, label='Average')
    ax.legend(loc='upper right', bbox_to_anchor=(1.0, 1.03), frameon=False)

    ax.set_xlabel("Run Number", labelpad=18)
    ax.set_ylabel(y_axis_title)
    ax.set_title(r"$|z| < 10$ cm and MBD Trigger")
    ax.set_ylim(bottom=0, top=7)

    # Calculate and display the total sum of luminosity and number of runs
    total_runs = len(original_lumis)
    total_lumi = sum(original_lumis)
    lumi_text = (
        f"Runs = {total_runs}\n"
        r"$\sum \mathcal{{L}} = {:,.2f} \text{{ nb}}^{{-1}}$".format(total_lumi)
    )
    ax.text(0.05, 0.95, lumi_text, transform=ax.transAxes, ha='left', va='top', fontsize=20)

    plt.tight_layout()
    plt.subplots_adjust(top=0.94, bottom=0.14)

    args.output_dir.mkdir(parents=True, exist_ok=True)

    csv_path = args.output_dir / f"{args.name}.csv"
    try:
        with csv_path.open('w', newline='') as csvfile:
            fieldnames = ['Run', 'Lumi_ub_inv']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames, lineterminator='\n')
            writer.writeheader()
            for row in sorted_results:
                writer.writerow({
                    'Run': row['Run'],
                    'Lumi_ub_inv': row['Lumi_ub_inv']
                })
        print(f"Saved detailed run summary to {csv_path}")
    except Exception as e:
        print(f"Error saving to CSV: {e}")

    pdf_path = args.output_dir / f"{args.name}.pdf"
    png_path = args.output_dir / f"{args.name}.png"
    plt.savefig(pdf_path)
    plt.savefig(png_path, dpi=300)
    print(f"Saved plots as {pdf_path} and {png_path}")

    print("-" * 88)
    print(f"Total Integrated Luminosity (|z| < 10 cm) for {total_runs} runs: {total_lumi:.6f} nb^-1")

if __name__ == "__main__":
    main()
