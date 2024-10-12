#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory.')
status = subparser.add_parser('status', help='Get status of Condor.')

f4a.add_argument('-i', '--run-list', type=str, help='Run List', required=True)
f4a.add_argument('-e', '--executable', type=str, default='scripts/genStatus.sh', help='Job script to execute. Default: scripts/genStatus.sh')
f4a.add_argument('-m', '--macro', type=str, default='macros/genStatus.C', help='Analysis macro. Default: macros/genStatus.C')
f4a.add_argument('-b', '--f4a', type=str, default='bin/genStatus', help='Analysis executable. Default: bin/genStatus')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=0.2, help='Memory (units of GB) to request per condor submission. Default: 0.2 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-n', '--submissions', type=int, default=1, help='Number of submissions. Default: 1.')

args = parser.parse_args()

def create_f4a_jobs():
    run_list   = os.path.realpath(args.run_list)
    executable = os.path.realpath(args.executable)
    macro      = os.path.realpath(args.macro)
    f4a        = os.path.realpath(args.f4a)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    log        = args.log
    n          = args.submissions

    concurrency_limit = 2308032

    print(f'Run List: {run_list}')
    print(f'Fun4All : {macro}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Submissions: {n}')
    # print(f'Concurrency: {p}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(run_list, output_dir)

    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/output',exist_ok=True)

    with open(f'{output_dir}/genStatus.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_run) {output_dir}/output\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(Process).out\n')
        file.write('error           = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
        file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
        file.write(f'queue input_run from {os.path.basename(run_list)}')

def get_condor_status():
    hosts = [f'sphnx{x:02}' for x in range(1,9)]
    hosts.append('sphnxdev01')

    dt_all = []
    dt_user = []

    for host in hosts:
        print(f'Progress: {host}')

        a = subprocess.run(['bash','-c',f'ssh {host} "condor_q | tail -n 3 | head -n 2"'], capture_output=True, encoding="utf-8")
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

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
    if(args.command == 'status'):
        get_condor_status()
