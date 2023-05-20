#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create = subparser.add_parser('create', help='Create file lists.')
run    = subparser.add_parser('run', help='Run LEDTowerBuilder on the given file list.')

create.add_argument('-i', '--run-list', type=str, nargs='+' , help='List of run numbers.')
create.add_argument('-p', '--prdf-dir', type=str, default='/direct/sphenix+lustre01/sphnxpro/rawdata/commissioning/emcal/calib', help='Directory containing the prdf files. Default: /direct/sphenix+lustre01/sphnxpro/rawdata/commissioning/emcal/calib')
create.add_argument('-o', '--output-dir', type=str, default='files', help='Directory to store the file lists. Default: files')

run.add_argument('-i', '--file-list', type=str, help='File list containing prdfs to analyze.', required=True)
run.add_argument('-n', '--nevents', type=int, default = -1, help='Number of events to analyze. Default: -1 (analyze all)')
run.add_argument('-o', '--output', type=str, default = 'data/LEDTowerBuilder.root', help='Output root file. Default: data/LEDTowerBuilder.root')
run.add_argument('-m', '--max', type=int, default = 10000, help='Maximum number of events to analyze at once. Default: 10000')

args = parser.parse_args()

def create_file_list():
    run_list = args.run_list
    prdf_dir = os.path.abspath(args.prdf_dir)
    output_dir = os.path.abspath(args.output_dir)

    print(f'run list: {run_list}')
    print(f'prdf dir: {prdf_dir}')
    print(f'output dir: {output_dir}')

    for run in run_list:
        print(f'run: {run}')

        result = subprocess.run(['fd', run, prdf_dir], stdout=subprocess.PIPE, text=True)
        print(result.stdout)

        with open(f'{output_dir}/file-list-{run}.txt',mode='w') as fw:
            fw.write(result.stdout)

def run_analysis():
    file_list          = os.path.abspath(args.file_list)
    nevents            = args.nevents
    output             = args.output
    max_events_per_run = args.max

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
            if(total_events > nevents and nevents >= 0):
                break

    nevents = total_events if nevents == -1 else nevents
    print(f'total events: {total_events}')
    if(nevents != total_events):
        print(f'events to analyze: {nevents}')

    runs = int(np.ceil(nevents / max_events_per_run))
    max_events_per_run = min(max_events_per_run, nevents)

    log = os.path.basename(output).split('.')[0]
    output_dir = os.path.dirname(output)
    process_events = max_events_per_run
    skip = 0

    if(runs > 1):
        print(f'Runs: {runs}')
        print(f'Max events per run: {max_events_per_run}')

        merge_files = []
        for i in range(runs):
            subprocess.run(['echo', f'Run: {i}'])
            command = f"./bin/Fun4All_LEDTowerBuilder {process_events} {skip} {file_list} {output_dir}/test-{i}.root &> {output_dir}/log/log-test-{i}.txt &"
            print(command)
            skip += max_events_per_run
            process_events = min(max_events_per_run, nevents-(i+1)*max_events_per_run)
            merge_files.append(f'{output_dir}/test-{i}.root')

        print('hadd command: ')
        merge_files = ' '.join(merge_files)
        print(f'hadd -n 50 {output} {merge_files}')

    else:
        command = f"./bin/Fun4All_LEDTowerBuilder {process_events} {skip} {file_list} {output} &> {output_dir}/log/log-{log}.txt &"
        print(command)


    # command = f"root -b -l -q 'macro/Fun4All_LEDTowerBuilder.C({nevents}, \"{file_list}\", \"{output}\")'"
    # command = f"./bin/Fun4All_LEDTowerBuilder {nevents} {file_list} {output} &> data/log/log-{log}.txt &"
    # print(command)
    # subprocess.run(['root','-b','-l','-q',f'macro/Fun4All_LEDTowerBuilder.C({nevents}, \"{file_list}\", \"{output}\")'])


if __name__ == '__main__':
    if(args.command == 'create'):
        create_file_list()
    if(args.command == 'run'):
        run_analysis()
