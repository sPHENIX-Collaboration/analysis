#!/usr/bin/env python3
"""
This module extracts the bias, led width, and runnumber from the logs.
Writes these fields to a csv file.
"""
import subprocess
import argparse
import os
import sys

parser = argparse.ArgumentParser()

parser.add_argument('-i'
                    , '--log-dir', type=str
                    , required=True
                    , help='Logs Directory')

parser.add_argument('-o'
                    , '--output', type=str
                    , default='run-info.csv'
                    , help='Output Log File. Default: run-info.csv')

args = parser.parse_args()

def execute_command(local_command):
    """Executes a command and exits when an error is encountered."""
    result = subprocess.run(['bash','-c',local_command], check=False, stdout=subprocess.PIPE)
    if result.returncode != 0:
        print(f'Error in {local_command}')
        sys.exit()

    return result.stdout.decode('utf-8').strip()

if __name__ == '__main__':
    log_dir        = os.path.realpath(args.log_dir)
    output         = os.path.realpath(args.output)

    os.makedirs(os.path.dirname(output), exist_ok=True)

    print(f'Log Dir: {log_dir}')
    print(f'Output: {output}')

    RUN_LOG_PATTERN = 'log_tp_'

    with open(output, mode='w', encoding='utf-8') as file:
        file.write('run,width,offset\n')
        # Loop over all items in the directory.
        for filename in os.listdir(log_dir):
            file_path = os.path.join(log_dir, filename)

            # Ensure we're processing only files (not directories, symlinks, etc.)
            if os.path.isfile(file_path) and filename.startswith(RUN_LOG_PATTERN):
                # Open the file and process it line by line
                command = f"awk '{{print $2}}' {file_path} | sort | uniq"

                run = execute_command(command)
                width = filename.split('_')[2] # units [ns]
                offset = filename.split('_')[4].split('.')[0]

                file.write(f'{run},{width},{offset}\n')

    # sort output file by the run number
    command = f"{{ head -n 1 {output}; tail -n +2 {output} | sort -t',' -k1,1; }} > {output}.tmp && mv {output}.tmp {output}"
    execute_command(command)
