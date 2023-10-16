#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create = subparser.add_parser('create', help='Create condor submission directory.')

create.add_argument('-i', '--run-list', type=str, help='List of runs', required=True)
create.add_argument('-e', '--executable', type=str, default='genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
create.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloTreeGen', help='Fun4All executable. Default: Fun4All_CaloTreeGen')
create.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
create.add_argument('-s', '--memory', type=int, default=2, help='Memory (units of GB) to request per condor submission. Default: 2 GB.')
create.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

args = parser.parse_args()

def create_jobs():
    run_list   = os.path.realpath(args.run_list)
    output_dir = os.path.realpath(args.output)
    f4a        = os.path.realpath(args.f4a)
    executable = os.path.realpath(args.executable)
    memory     = args.memory
    log        = args.log

    print(f'Run List: {run_list}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(executable, output_dir)

    with open(run_list) as file_list:
        for run in file_list:
            run = run.strip()
            job_dir = f'{output_dir}/{os.path.basename(run)}'
            os.makedirs(job_dir,exist_ok=True)
            os.makedirs(f'{job_dir}/stdout',exist_ok=True)
            os.makedirs(f'{job_dir}/error',exist_ok=True)
            os.makedirs(f'{job_dir}/output',exist_ok=True)

            # create file list for each submission
            with open(f'{job_dir}/jobs.txt', mode='w') as file:
                for segment in os.listdir(run):
                    file.write(f'{run}/{segment}\n')

            with open(f'{job_dir}/genFun4All.sub', mode="w") as file:
                file.write(f'executable             = ../{os.path.basename(executable)}\n')
                file.write(f'arguments              = $(input) output/qa-$(Process).root output/ntp-$(Process).root {job_dir}/output {output_dir}/{os.path.basename(f4a)}\n')
                file.write(f'log                    = {log}\n')
                file.write('output                  = stdout/job-$(Process).out\n')
                file.write('error                   = error/job-$(Process).err\n')
                file.write(f'request_memory         = {memory}GB\n')
                file.write('queue input from jobs.txt')

            print(f'cd {job_dir} && condor_submit genFun4All.sub')

if __name__ == '__main__':
    if(args.command == 'create'):
        create_jobs()
