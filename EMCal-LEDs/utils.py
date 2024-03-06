#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create  = subparser.add_parser('create', help='Create file lists.')
run     = subparser.add_parser('run', help='Run LEDTowerBuilder on the given file.')
evtDisp = subparser.add_parser('evtDisp', help='Create event display (json) given prdf and event number.')

create.add_argument('-i', '--run-list', type=str, nargs='+' , help='List of run numbers.')
create.add_argument('-p', '--prdf-dir', type=str, default='/direct/sphenix+lustre01/sphnxpro/rawdata/commissioning/emcal/calib', help='Directory containing the prdf files. Default: /direct/sphenix+lustre01/sphnxpro/rawdata/commissioning/emcal/calib')
create.add_argument('-o', '--output-dir', type=str, default='files', help='Directory to store the file lists. Default: files')

run.add_argument('-i', '--input-file', type=str, help='PRDF file to analyze', required=True)
run.add_argument('-n', '--nevents', type=int, help='Total number of events to process', required=True)
run.add_argument('-j', '--events-per-job', type=int, default=100, help='Total number of events per job to process. Default: 100')
run.add_argument('-m', '--macro', type=str, default='macro/Fun4All_LEDTowerBuilder.C', help='LEDTowerBuilder macro. Default: macro/Fun4All_LEDTowerBuilder.C')
run.add_argument('-e', '--script', type=str, default='genLEDAna.sh', help='Job script to execute. Default: genLEDAna.sh')
run.add_argument('-b', '--executable', type=str, default='bin/Fun4All_LEDTowerBuilder', help='Executable. Default: bin/Fun4All_LEDTowerBuilder')
run.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
run.add_argument('-s', '--memory', type=float, default=0.5, help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')
run.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

evtDisp.add_argument('-i', '--prdf', type=str, help='Prdfs to analyze.', required=True)
evtDisp.add_argument('-r', '--run', type=str, help='Run number.', required=True)
evtDisp.add_argument('-n', '--event', type=str, default = '1', help='Event number to use. Default: 1.')
evtDisp.add_argument('-o', '--output', type=str, help='Output json file.')

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
    input_file     = os.path.realpath(args.input_file)
    macro          = os.path.realpath(args.macro)
    script         = os.path.realpath(args.script)
    executable     = os.path.realpath(args.executable)
    output_dir         = os.path.realpath(args.output)
    nevents        = args.nevents
    events_per_job = args.events_per_job
    memory         = args.memory
    log            = args.log

    print(f'input: {input_file}')
    print(f'nevents: {nevents}')
    print(f'Events per job: {events_per_job}')
    print(f'macro: {macro}')
    print(f'script: {script}')
    print(f'executable: {executable}')
    print(f'output: {output_dir}')
    print(f'memory: {memory}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(macro, output_dir)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)

    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/output',exist_ok=True)

    jobs = nevents // events_per_job if (nevents % events_per_job == 0) else nevents // events_per_job + 1
    skip = 0
    with open(f'{output_dir}/jobs.list', mode='w') as file:
        for i in range(jobs):
            file.write(f'{skip}\n')
            skip += events_per_job

    with open(f'{output_dir}/genFun4All.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(script)}\n')
        file.write(f'arguments      = {os.path.basename(executable)} {input_file} output/test-$(Process).root {events_per_job} $(skip)\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(Process).out\n')
        file.write('error           = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        file.write(f'queue skip from jobs.list')

def event_display():
    prdf_input   = os.path.abspath(args.prdf)
    run          = args.run
    event        = args.event
    output       = args.output

    if(output is None):
        output = f'event-display/run-{run}-event-{event}.json'

    print(f'prdf: {prdf_input}')
    print(f'run: {run}')
    print(f'event: {event}')
    print(f'output: {output}')
    print(f'display only: {display_only}')

    # isolate the specified event from the prdf
    command = f'eventcombiner -v -i -e {event} -n 1 -f -p data/temp/test-{run}-{event}.prdf {prdf_input}'
    print(f'command: {command.split()}')
    subprocess.run(command.split())

    # create temp file list containing the prdf
    with open('files/test.txt','w') as f:
        f.write(f'data/temp/test-{run}-{event}.prdf\n')

    # running the LEDTowerBuilder over the prdf
    command = f'bin/Fun4All_LEDTowerBuilder 1 files/test.txt data/temp test'
    print(f'command: {command.split()}')
    subprocess.run(command.split())

    # running the event-display which generates the json output
    command = f'bin/event-display {run} {event} data/temp/test-CEMC.root {output} data/temp/test-HCALIN.root data/temp/test-HCALOUT.root'
    print(f'command: {command.split()}')
    subprocess.run(command.split())

if __name__ == '__main__':
    if(args.command == 'create'):
        create_file_list()
    if(args.command == 'run'):
        run_analysis()
    if(args.command == 'evtDisp'):
        event_display()
