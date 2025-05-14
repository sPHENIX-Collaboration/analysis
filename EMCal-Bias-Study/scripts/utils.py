#!/usr/bin/env python3
"""
This module is a general utility to configure condor job submission for event combining, waveform fitting etc.
"""
import subprocess
import argparse
import os
import shutil
import pandas as pd

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

evtComb = subparser.add_parser('evtComb', help='Create condor submission directory for Event Combining.')
evtComb.add_argument('-i', '--run-list', type=str, help='Run list', required=True)
evtComb.add_argument('-i2', '--calib-dir', type=str, default='/sphenix/lustre01/sphnxpro/physics/emcal/led', help='Data Directory. Default: /sphenix/lustre01/sphnxpro/physics/emcal/led')
evtComb.add_argument('-e', '--executable', type=str, default='scripts/genEventCombine.sh', help='Job script to execute. Default: scripts/genEventCombine.sh')
evtComb.add_argument('-n', '--nSegments', type=int, default=16, help='Number of segments per run to combine. Default: 16')
evtComb.add_argument('-n2', '--nEvents', type=int, default=100, help='Minimum number of segments in each SEB. Default: 100')
evtComb.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
evtComb.add_argument('-s', '--memory', type=float, default=2, help='Memory (units of GB) to request per condor submission. Default: 2 GB.')
evtComb.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

def create_event_combine_jobs():
    """
    This function creates / configures the condor job submission for event combining
    the individual prdfs into one prdf per run.
    """
    run_list   = os.path.realpath(args.run_list)
    calib_dir  = os.path.realpath(args.calib_dir)
    output_dir = os.path.realpath(args.output)
    executable = os.path.realpath(args.executable)
    memory     = args.memory
    log        = args.log
    nSegments  = args.nSegments
    nEvents    = args.nEvents

    calib_dir_base  = os.path.basename(calib_dir)

    print(f'Run List: {run_list}')
    print(f'Calib Dir: {calib_dir}')
    print(f'Calib Dir Base: {calib_dir_base}')
    print(f'Output Directory: {output_dir}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'nSegments: {nSegments}')
    print(f'Mininum Number of Events per SEB: {nEvents}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(executable, output_dir)
    shutil.copy(run_list, output_dir)

    os.makedirs(f'{output_dir}/output',exist_ok=True)
    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/{calib_dir_base}',exist_ok=True)

    # get list of all PRDF files for the runs
    with open(run_list, encoding="utf-8") as fp:
        for run in fp:
            run = run.strip()
            print(f'run: {run}')
            calib_list = f'{calib_dir_base}/{calib_dir_base}-{run}.list'
            command = f'fd {run} {calib_dir} > {calib_list}'

            result = subprocess.run(['bash','-c',command], cwd=output_dir, check=False)
            if result.returncode != 0:
                print(f'Error in {command}')
                return

            # ensure there are correct number of segments
            segments = int(subprocess.run(['bash','-c',f'wc -l {calib_list}'], capture_output=True, encoding="utf-8", cwd=output_dir, check=False).stdout.split()[0])
            if segments != nSegments:
                print(f'WARNING: {run} has {segments} out of {nSegments}')

    # generate the job list
    command = f'readlink -f {calib_dir_base}/* > jobs.list'

    result = subprocess.run(['bash','-c',command], cwd=output_dir, check=False)
    if result.returncode != 0:
        print(f'Error in {command}')
        return

    with open(f'{output_dir}/genEventCombine.sub', mode="w", encoding="utf-8") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = $(input_run) {output_dir}/output {nEvents}\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(ClusterId)-$(Process).out\n')
        file.write('error           = error/job-$(ClusterId)-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        # file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
        # file.write(f'queue input_run from jobs.list')

    print('\nSubmission Command')
    print(f'rm -rf /tmp/anarde/dump && mkdir /tmp/anarde/dump && cd {output_dir} && condor_submit genEventCombine.sub -queue "input_run from jobs.list"')

f4a = subparser.add_parser('f4a', help='Create condor submission directory for Fun4All CaloFitting.')
f4a.add_argument('-i', '--runs-dir', type=str, help='Runs Directory', required=True)
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-b', '--f4a-bin', type=str, default='bin/Fun4All_Year2_Fitting', help='Fun4All executable. Default: bin/Fun4All_Year2_Fitting')
f4a.add_argument('-n', '--nEvents', type=int, default=0, help='Number of events to process. Default: 0 (All)')
f4a.add_argument('-w', '--all-waveforms', type=int, default=1, help='Do All Waveforms. Default: 1 (true)')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

def create_f4a_jobs():
    """
    This function creates / configures the condor job submission for waveform fitting the prdf
    and generate the histograms containing the waveforms and average ADC.
    """
    runs_dir   = os.path.realpath(args.runs_dir)
    output_dir = os.path.realpath(args.output)
    executable = os.path.realpath(args.executable)
    f4a_bin    = os.path.realpath(args.f4a_bin)
    doAllWaveforms = args.all_waveforms
    memory     = args.memory
    log        = args.log
    nEvents  = args.nEvents

    print(f'Runs Dir: {runs_dir}')
    print(f'Output Directory: {output_dir}')
    print(f'Executable: {executable}')
    print(f'f4a: {f4a_bin}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'nEvents: {nEvents if nEvents else "All"}')
    print(f'Do All Waveforms: {bool(doAllWaveforms)}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(executable, output_dir)
    shutil.copy(f4a_bin, output_dir)

    os.makedirs(f'{output_dir}/output',exist_ok=True)
    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)

    command = f'readlink -f {runs_dir}/* > jobs.list'
    result = subprocess.run(['bash','-c',command], cwd=output_dir, check=False)
    if result.returncode != 0:
        print(f'Error in {command}')
        return

    with open(f'{output_dir}/genFun4All.sub', mode="w", encoding="utf-8") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = {f4a_bin} $(input_run) {nEvents} {doAllWaveforms} {output_dir}/output\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(ClusterId)-$(Process).out\n')
        file.write('error           = error/job-$(ClusterId)-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        # file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
        # file.write(f'queue input_run from jobs.list')

    print('\nSubmission Command')
    print(f'rm -rf /tmp/anarde/dump && mkdir /tmp/anarde/dump && cd {output_dir} && condor_submit genFun4All.sub -queue "input_run from jobs.list"')

status = subparser.add_parser('status', help='Get status of Condor.')

def get_condor_status():
    """
    This function gets the status of condor jobs for each submission node: idle / running / held
    """
    hosts = [f'sphnxuser{x:02}' for x in range(1,9)]

    dt_all = []
    dt_user = []

    for host in hosts:
        print(f'Progress: {host}')

        a = subprocess.run(['bash','-c',f'ssh {host} "condor_q | tail -n 3 | head -n 2"'], capture_output=True, encoding="utf-8", check=False)
        total   = int(a.stdout.split('\n')[-3].split('jobs')[0].split(':')[1])
        idle    = int(a.stdout.split('\n')[-3].split('idle')[0].split(',')[-1])
        running = int(a.stdout.split('\n')[-3].split('running')[0].split(',')[-1])
        held    = int(a.stdout.split('\n')[-3].split('held')[0].split(',')[-1])

        dt_user.append({'host': host, 'total': total, 'idle': idle, 'running': running, 'held': held})

        total   = int(a.stdout.split('\n')[-2].split('jobs')[0].split(':')[1])
        idle    = int(a.stdout.split('\n')[-2].split('idle')[0].split(',')[-1])
        running = int(a.stdout.split('\n')[-2].split('running')[0].split(',')[-1])
        held    = int(a.stdout.split('\n')[-2].split('held')[0].split(',')[-1])

        dt_all.append({'host': host, 'total': total, 'idle': idle, 'running': running, 'held': held})

    print('All')
    print(pd.DataFrame(dt_all).to_string(index=False))

    print('User')
    print(pd.DataFrame(dt_user).to_string(index=False))

args = parser.parse_args()

if __name__ == '__main__':
    if args.command == 'evtComb':
        create_event_combine_jobs()
    if args.command == 'f4a':
        create_f4a_jobs()
    if args.command == 'status':
        get_condor_status()
