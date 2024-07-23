#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory for Fun4All_CaloHotTower.')

f4a.add_argument('-i', '--run-list-dir', type=str, help='Directory of run lists', required=True)
f4a.add_argument('-i2', '--hot-tower-list', type=str, help='Hot Tower List', required=True)
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macro/Fun4All_CaloHotTower.C', help='Fun4All macro. Default: macro/Fun4All_CaloHotTower.C')
f4a.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloHotTower', help='Fun4All executable. Default: bin/Fun4All_CaloHotTower')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=0.5, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-n', '--submissions', type=int, default=9, help='Number of submissions. Default: 1.')
f4a.add_argument('-p', '--concurrency', type=int, default=5000, help='Max number of jobs running at once. Default: 5000.')

args = parser.parse_args()

def create_f4a_jobs():
    run_list_dir   = os.path.realpath(args.run_list_dir)
    hot_tower_list = os.path.realpath(args.hot_tower_list)
    output_dir     = os.path.realpath(args.output)
    f4a            = os.path.realpath(args.f4a)
    macro          = os.path.realpath(args.macro)
    src            = os.path.realpath(args.src)
    executable     = os.path.realpath(args.executable)
    memory         = args.memory
    log            = args.log
    n              = args.submissions
    p              = args.concurrency

    concurrency_limit = 2308032

    print(f'Run List Directory: {run_list_dir}')
    print(f'Hot Tower List: {hot_tower_list}')
    print(f'Fun4All : {macro}')
    print(f'src: {src}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Submissions: {n}')
    print(f'Concurrency: {p}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(hot_tower_list, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)
    shutil.copy(executable, output_dir)

    i     = 0
    arr   = ['']*n

    for filename in os.listdir(run_list_dir):
        print(f'Processing: {filename}, i: {i}')
        if(filename.endswith('list')):
            f = os.path.join(run_list_dir, filename)
            run = int(filename.split('-')[1].split('.')[0])
            job_dir = f'{output_dir}/{run}'

            os.makedirs(job_dir,exist_ok=True)
            os.makedirs(f'{job_dir}/stdout',exist_ok=True)
            os.makedirs(f'{job_dir}/error',exist_ok=True)
            os.makedirs(f'{job_dir}/output',exist_ok=True)

            shutil.copy(f, job_dir)

            with open(f'{job_dir}/genFun4All.sub', mode="w") as file:
                file.write(f'executable     = ../{os.path.basename(executable)}\n')
                file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_dst) {output_dir}/{os.path.basename(hot_tower_list)} output/test-$(Process).root\n')
                file.write(f'log            = {log}\n')
                file.write('output          = stdout/job-$(Process).out\n')
                file.write('error           = error/job-$(Process).err\n')
                file.write(f'request_memory = {memory}GB\n')
                file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
                file.write(f'queue input_dst from {filename}')

            arr[i%n] = arr[i%n] + f'cd {job_dir} && condor_submit genFun4All.sub && '
            i += 1

    for x in arr:
        print(x)

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
