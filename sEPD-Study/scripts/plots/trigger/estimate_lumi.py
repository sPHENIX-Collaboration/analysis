#!/usr/bin/env python3
import argparse
import psycopg2
import sys
import csv
from pathlib import Path

import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
import mplhep as hep

# MBD cross section
SIGMA_MBD_BARNS = 6.324
# 1 barn = 1e6 microbarns
SIGMA_MBD_UB = SIGMA_MBD_BARNS * 1e6

def estimate_lumi_for_runs(conn, run_list, verbose=False):
    if not run_list:
        return 0.0

    try:
        with conn.cursor() as cur:
            query = """
                SELECT runnumber, index, raw, live, scaled
                FROM gl1_scalers
                WHERE runnumber IN %s AND index IN (12, 14);
            """
            cur.execute(query, (tuple(run_list),))
            rows = cur.fetchall()

            query_time = """
                SELECT runnumber, brtimestamp, ertimestamp
                FROM run
                WHERE runnumber IN %s;
            """
            cur.execute(query_time, (tuple(run_list),))
            time_rows = cur.fetchall()
            run_times = {row[0]: (row[1], row[2]) for row in time_rows}
    except Exception as e:
        print(f"Error querying runs: {e}")
        return 0.0

    # Organize rows by runnumber
    run_data = {}
    for row in rows:
        runnumber, idx, raw, live, scaled = row
        if runnumber not in run_data:
            run_data[runnumber] = {}
        run_data[runnumber][idx] = {'raw': raw, 'live': live, 'scaled': scaled}

    total_lumi = 0.0
    results = []
    failed_runs = 0

    for run in run_list:
        data = run_data.get(run)
        if not data:
            if verbose:
                print(f"Run {run:6d} | No scaler data found.")
            continue

        raw12, live12, scaled12 = 0, 0, 0
        raw14, live14, scaled14 = 0, 0, 0

        if 12 in data:
            raw12, live12, scaled12 = data[12]['raw'], data[12]['live'], data[12]['scaled']
        if 14 in data:
            raw14, live14, scaled14 = data[14]['raw'], data[14]['live'], data[14]['scaled']

        trig12_active = (scaled12 >= 1) and (live12 > 0)
        trig14_active = (scaled14 >= 1) and (live14 > 0)

        N_counts = 0
        method = ""

        if trig12_active:
            N_counts = scaled12
            method = "Trig 12"
        elif trig14_active and raw14 > 0:
            zfraction = raw12 / raw14
            N_counts = scaled14 * zfraction
            method = "Trig 14 w/ zfrac"
        else:
            failed_runs += 1
            if verbose:
                print(f"Run {run:6d} | Both Trig 12 and Trig 14 failed checks. Excluded.")
            continue

        lumi_ub_inv = N_counts / SIGMA_MBD_UB
        total_lumi += lumi_ub_inv

        results.append({
            'Run': run,
            'Trig12_Active': 'Yes' if trig12_active else 'No',
            'Trig14_Active': 'Yes' if trig14_active else 'No',
            'Method': method,
            'Lumi_ub_inv': round(lumi_ub_inv, 4),
            'Time': run_times.get(run, (None, None))[0],
            'EndTime': run_times.get(run, (None, None))[1]
        })

        if verbose:
            print(f"Run {run:6d} | Trig 12 Active: {'Yes' if trig12_active else 'No'} | Trig 14 Active: {'Yes' if trig14_active else 'No'} | Method: {method:16s} | Lumi (|z|<10cm) = {lumi_ub_inv:10.4f} ub^-1")

    return total_lumi, results, failed_runs

def make_plot(results, total_lumi_ub, output_dir, plot_name="luminosity_per_run"):
    runs_trig12, lumis_trig12 = [], []
    runs_trig14, lumis_trig14 = [], []
    all_lumis = []

    for r in sorted(results, key=lambda x: x['Run']):
        lumi = r['Lumi_ub_inv']
        all_lumis.append(lumi)
        if r['Method'] == "Trig 12":
            runs_trig12.append(r['Run'])
            lumis_trig12.append(lumi)
        elif r['Method'] == "Trig 14 w/ zfrac":
            runs_trig14.append(r['Run'])
            lumis_trig14.append(lumi)

    # Plotting using ATLAS style
    hep.style.use("ATLAS")

    fig, ax = plt.subplots(figsize=(10, 6))

    sum_trig12_nb = sum(lumis_trig12) * 1e-3
    sum_trig14_nb = sum(lumis_trig14) * 1e-3

    if runs_trig12:
        label_12 = rf"Trig 12 ({len(runs_trig12)} runs, {sum_trig12_nb:.2f} $\text{{nb}}^{{-1}}$)"
        ax.plot(runs_trig12, lumis_trig12, marker='s', markersize=4, linestyle='none', color='tab:orange', label=label_12)
    if runs_trig14:
        label_14 = rf"Trig 14 (w/ zfrac) ({len(runs_trig14)} runs, {sum_trig14_nb:.2f} $\text{{nb}}^{{-1}}$)"
        ax.plot(runs_trig14, lumis_trig14, marker='o', markersize=4, linestyle='none', color='tab:blue', label=label_14)

    # Add a light dashed line for the average luminosity
    avg_lumi = sum(all_lumis) / len(all_lumis) if all_lumis else 0
    ax.axhline(avg_lumi, color='gray', linestyle='--', alpha=0.5, label='Average')
    ax.legend(loc='upper right', bbox_to_anchor=(1.0, 1.03), frameon=False)

    ax.set_xlabel("Run Number", labelpad=18)
    ax.set_ylabel(r"Luminosity [$\mu\text{b}^{-1}$]")
    ax.set_title(r"Database MBD Trigger")
    ax.set_ylim(bottom=0)

    # Calculate and display the total sum of luminosity in nb^-1
    total_runs = len(results)
    total_lumi_nb = total_lumi_ub * 1e-3
    lumi_text = (
        f"Runs = {total_runs}\n"
        r"$\sum \mathcal{{L}} = {:.2f} \text{{ nb}}^{{-1}}$".format(total_lumi_nb)
    )
    ax.text(0.05, 0.95, lumi_text, transform=ax.transAxes, ha='left', va='top', fontsize=20)

    plt.tight_layout()
    plt.subplots_adjust(top=0.94, bottom=0.14)

    output_dir.mkdir(parents=True, exist_ok=True)
    pdf_path = output_dir / f"{plot_name}.pdf"
    png_path = output_dir / f"{plot_name}.png"

    plt.savefig(pdf_path)
    plt.savefig(png_path, dpi=300)
    plt.close()
    print(f"Saved plots as {pdf_path} and {png_path}")

def make_cumulative_plot(results, output_dir, plot_name="luminosity_per_run"):
    # Filter out runs without a valid time
    valid_results = [r for r in results if r.get('Time') is not None]
    if not valid_results:
        return

    # Sort by time
    valid_results.sort(key=lambda x: x['Time'])

    times = []
    cumulative_lumi_nb = []

    current_lumi_nb = 0.0
    for r in valid_results:
        t_start = r['Time']
        t_end = r.get('EndTime')
        lumi = r['Lumi_ub_inv'] * 1e-3

        times.append(t_start)
        cumulative_lumi_nb.append(current_lumi_nb)

        if t_end is not None:
            times.append(t_end)
        else:
            times.append(t_start)

        current_lumi_nb += lumi
        cumulative_lumi_nb.append(current_lumi_nb)

        # Insert gap
        times.append(t_end if t_end else t_start)
        cumulative_lumi_nb.append(float('nan'))

    if not times:
        return

    hep.style.use("ATLAS")
    fig, ax = plt.subplots(figsize=(10, 6))

    ax.plot(times, cumulative_lumi_nb, linestyle='-', marker='', color='tab:blue', linewidth=4)

    import matplotlib.dates as mdates
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%b-%d'))

    ax.set_xlabel("Time", labelpad=4)
    ax.set_ylabel(r"Cumulative Luminosity [$\text{nb}^{-1}$]")
    ax.set_title(r"Database MBD Trigger")
    ax.set_ylim(bottom=0)

    # Filter out NaNs for xlim
    valid_times = [t for t in times if t is not None]
    if len(valid_times) > 1:
        ax.set_xlim(left=valid_times[0], right=valid_times[-1])

    total_runs = len(valid_results)
    lumi_text = (
        f"Runs = {total_runs}\n"
        r"Min. Bias |z| < 10 cm = {:.2f} $\text{{nb}}^{{-1}}$".format(current_lumi_nb)
    )
    ax.text(0.05, 0.95, lumi_text, transform=ax.transAxes, ha='left', va='top', fontsize=30)

    plt.tight_layout()
    plt.subplots_adjust(bottom=0.1)

    cum_plot_name = f"{plot_name}-cumulative"
    pdf_path = output_dir / f"{cum_plot_name}.pdf"
    png_path = output_dir / f"{cum_plot_name}.png"

    plt.savefig(pdf_path)
    plt.savefig(png_path, dpi=300)
    plt.close()
    print(f"Saved cumulative plots as {pdf_path} and {png_path}")

def main():
    parser = argparse.ArgumentParser(description="Estimate luminosity for a list of runs.")
    parser.add_argument("-f", "--file", type=Path, help="Path to a text file containing run numbers (one per line).")
    parser.add_argument("-o", "--output-dir", type=Path, default=Path("."), help="Directory to save the CSV and plots (default: current directory).")
    parser.add_argument("-n", "--name", "--plot-name", dest="name", type=str, default="luminosity_per_run", help="Base filename for output CSV and plots (default: luminosity_per_run).")
    parser.add_argument("-v", "--verbose", action="store_true", help="Print detailed information for each run in stdout.")
    parser.add_argument("runs", nargs="*", type=int, help="List of run numbers")
    args = parser.parse_args()

    run_list = []
    if args.runs:
        run_list.extend(args.runs)

    if args.file:
        try:
            with args.file.open('r') as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith('#'):
                        run_list.append(int(line))
        except Exception as e:
            print(f"Error reading file {args.file}: {e}")
            sys.exit(1)

    if not run_list:
        print("Error: You must provide at least one run number or a file containing run numbers.")
        parser.print_help()
        sys.exit(1)

    try:
        # Connect to sphnxdaqdbreplica database 'daq'
        # Often a specific user isn't needed if the environment is set up for ident/peer authentication.
        conn = psycopg2.connect(
            host="sphnxdaqdbreplica",
            database="daq"
        )
    except psycopg2.Error as e:
        print(f"Failed to connect to the database. Error:\n{e}")
        sys.exit(1)

    if args.verbose:
        print("-" * 88)
    total_lumi, results, failed_runs = estimate_lumi_for_runs(conn, run_list, verbose=args.verbose)

    if results:
        # Ensure output directory exists
        try:
            args.output_dir.mkdir(parents=True, exist_ok=True)
        except Exception as e:
            print(f"Error creating output directory {args.output_dir}: {e}")
            sys.exit(1)

        csv_path = args.output_dir / f"{args.name}.csv"
        try:
            with csv_path.open('w', newline='') as csvfile:
                fieldnames = ['Run', 'Trig12_Active', 'Trig14_Active', 'Method', 'Lumi_ub_inv', 'Time', 'EndTime']
                writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
                writer.writeheader()
                for row in results:
                    writer.writerow(row)
            print(f"-> Saved detailed run estimates to {csv_path}")
        except Exception as e:
            print(f"Error saving to CSV: {e}")

        make_plot(results, total_lumi, args.output_dir, args.name)
        make_cumulative_plot(results, args.output_dir, args.name)

    print("-" * 88)
    n_trig12 = sum(1 for r in results if r['Method'] == "Trig 12")
    n_trig14 = sum(1 for r in results if r['Method'] == "Trig 14 w/ zfrac")
    n_missing = len(run_list) - len(results) - failed_runs

    lumi_trig12_ub = sum(r['Lumi_ub_inv'] for r in results if r['Method'] == "Trig 12")
    lumi_trig14_ub = sum(r['Lumi_ub_inv'] for r in results if r['Method'] == "Trig 14 w/ zfrac")

    lumi_trig12_nb = lumi_trig12_ub * 1e-3
    lumi_trig14_nb = lumi_trig14_ub * 1e-3
    total_lumi_nb = total_lumi * 1e-3

    pct_trig12 = (lumi_trig12_ub / total_lumi * 100) if total_lumi > 0 else 0.0
    pct_trig14 = (lumi_trig14_ub / total_lumi * 100) if total_lumi > 0 else 0.0

    print("Trigger Usage Summary:")
    print(f"  - Trig 12:                   {n_trig12} run(s) | Lumi: {lumi_trig12_nb:.6f} nb^-1 ({pct_trig12:.2f}%)")
    print(f"  - Trig 14 (w/ zfrac):        {n_trig14} run(s) | Lumi: {lumi_trig14_nb:.6f} nb^-1 ({pct_trig14:.2f}%)")
    if failed_runs > 0:
        print(f"  - Failed Both Checks:        {failed_runs} run(s) (Excluded)")
    if n_missing > 0:
        print(f"  - No Scaler Data Found:      {n_missing} run(s)")
    print(f"Total Integrated Luminosity (|z| < 10 cm) for {len(results)} runs: {total_lumi_nb:.6f} nb^-1")

if __name__ == "__main__":
    main()
