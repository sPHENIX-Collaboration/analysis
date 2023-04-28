#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create = subparser.add_parser('create')
run    = subparser.add_parser('run')

create.add_argument('-i', '--run-list', type=str, help='List of run numbers.', required=True)
create.add_argument('--prdf-dir', type=str, default='/direct/sphenix+lustre01/sphnxpro/commissioning/emcal/calib', help='Directory containing the prdf files. Default: /direct/sphenix+lustre01/sphnxpro/commissioning/emcal/calib')
create.add_argument('--output-dir', type=str, default='files', help='Directory to store the file lists. Default: files')

run.add_argument('-i', '--file-list', type=str, help='File list containing prdfs to analyze.', required=True)
run.add_argument('-n', '--nevents', type=int, default = -1, help='Number of events to analyze. Default: -1 (analyze all)')
run.add_argument('-o', '--output', type=str, default = 'data/LEDTowerBuilder.root', help='Output root file. Default: data/LEDTowerBuilder.root')

args = parser.parse_args()

def create_file_list():
    run_list = os.path.abspath(args.run_list)
    prdf_dir = os.path.abspath(args.prdf_dir)
    output_dir = os.path.abspath(args.output_dir)

    print(f'run list: {run_list}')
    print(f'prdf dir: {prdf_dir}')
    print(f'output dir: {output_dir}')

    with open(run_list) as f:
        for line in f:
            run = int(line)
            print(f'run: {run}')

            result = subprocess.run(['fd', str(run), prdf_dir], stdout=subprocess.PIPE, text=True)
            print(result.stdout)

            with open(f'{output_dir}/file-list-{run}.txt',mode='w') as fw:
                fw.write(result.stdout)

def run_analysis():
    file_list = os.path.abspath(args.file_list)
    nevents   = args.nevents
    output    = args.output

    print(f'file list: {file_list}')
    print(f'output: {output}')

    total_events = 0
    # Find total number of events in the file list
    with open(file_list) as f:
        for line in f:
            line = line.strip()
            command = f'dpipe -d n -s f -i {line}'
            dpipe_process = subprocess.run(command.split(), stdout=subprocess.PIPE)
            events = subprocess.run(['wc','-l'], input=dpipe_process.stdout, stdout=subprocess.PIPE)
            events = int(events.stdout.decode("utf-8"))

            total_events += events
            print(f'prdf: {line}, events: {events}')

    nevents = total_events if nevents == -1 else nevents
    print(f'total events: {total_events}')
    if(nevents != total_events):
        print(f'events to analyze: {nevents}')

    # command = f"root -b -l -q 'macro/Fun4All_LEDTowerBuilder.C({nevents}, \"{file_list}\", \"{output}\")'"
    log = os.path.basename(output).split('.')[0]
    command = f"./bin/Fun4All_LEDTowerBuilder {nevents} {file_list} {output} &> data/log-{log}.txt &"
    print(command)
    # subprocess.run(['root','-b','-l','-q',f'macro/Fun4All_LEDTowerBuilder.C({nevents}, \"{file_list}\", \"{output}\")'])


if __name__ == '__main__':
    if(args.command == 'create'):
        create_file_list()
    if(args.command == 'run'):
        run_analysis()
