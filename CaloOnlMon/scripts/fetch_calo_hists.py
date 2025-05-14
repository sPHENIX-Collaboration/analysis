#!/usr/bin/env python3
"""
This module extracts the different online monitoring histograms and combines them into one file
per each plot type.
"""
import argparse
import os
import subprocess
import sys

parser = argparse.ArgumentParser()

parser.add_argument('-i'
                    , '--run-start', type=int
                    , required=True
                    , help='Input start runnumber')

parser.add_argument('-o'
                    , '--output', type=str
                    , default='.'
                    , help='Output directory of plots.')

parser.add_argument('-d'
                    , '--dry-run', action='store_true'
                    , help='Enable Dry Run.')

args = parser.parse_args()

def execute_command(local_command, local_dry_run):
    """Executes a command and exits when an error is encountered."""
    if local_dry_run:
        print(f'COMMAND: {local_command}')
    else:
        result = subprocess.run(['bash','-c',local_command], check=False)
        if result.returncode != 0:
            print(f'Error in {local_command}')
            sys.exit()

if __name__ == '__main__':
    run_start = args.run_start
    output    = os.path.realpath(args.output)
    dry_run   = args.dry_run

    os.makedirs(output, exist_ok=True)

    print(f'Run Start: {run_start}')
    print(f'Output: {output}')
    print(f'Dry Run: {dry_run}')

    # Generate Run List
    command = ('psql -h sphnxdaqdbreplica daq -c '
               '"select runnumber from run '
               f'where runnumber >= {run_start} and runnumber != -1 and runtype != \'junk\' '
               f'and ertimestamp is not null order by runnumber;" -At > {output}/runs.list')

    execute_command(command, dry_run)

    hist_types = range(1,10)
    detectors = ['CEMC','IHCAL','OHCAL']


    ctr = 1
    for hist_type in hist_types:
        for detector in detectors:

            # IHCAL and OHCAL don't have hist types greater than 7
            if(hist_type > 7 and detector != 'CEMC'):
                continue

            print(f'Processing hist type: {hist_type}, Detector: {detector}, {ctr}/{len(hist_types)*len(detectors)-4}')

            command = (f'/direct/sphenix+u/anarde/.cargo/bin/rg -f {output}/runs.list '
                    f'<(/direct/sphenix+u/anarde/.cargo/bin/fd {detector}MONDRAW_{hist_type} /sphenix/WWW/run/2025/OnlMonHtml) '
                    '--no-filename | sort -t\'/\' -k9,9n '
                    f'| xargs /direct/sphenix+u/anarde/.local/bin/img2pdf --output {output}/{detector}MONDDRAW_{run_start}_{hist_type}.pdf')

            execute_command(command, dry_run)

            ctr += 1
