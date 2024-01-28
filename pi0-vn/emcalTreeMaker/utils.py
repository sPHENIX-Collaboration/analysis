#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory for Fun4All_CaloTreeGen.')

f4a.add_argument('-i', '--run-list-dir', type=str, help='Directory of run lists', required=True)
f4a.add_argument('-e', '--executable', type=str, default='genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloTreeGen', help='Fun4All executable. Default: bin/Fun4All_CaloTreeGen')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=int, default=1, help='Memory (units of GB) to request per condor submission. Default: 2 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

pi0Ana = subparser.add_parser('pi0Ana', help='Create condor submission directory for pi0Analysis.')

pi0Ana.add_argument('-i', '--ntp-list', type=str, help='List of Ntuples', required=True)
pi0Ana.add_argument('-c', '--cuts', type=str, help='List of cuts', required=True)
pi0Ana.add_argument('-c2', '--csv', type=str, default="", help='CSV file with fitStats. Default: ""')
pi0Ana.add_argument('-c3', '--Q-vec-corr', type=str, default="", help='CSV file with Q vector corrections. Default: ""')
pi0Ana.add_argument('-e', '--script', type=str, default='genPi0Ana.sh', help='Job script to execute. Default: genPi0Ana.sh')
pi0Ana.add_argument('-b', '--executable', type=str, default='bin/pi0Ana', help='Executable. Default: bin/pi0Ana')
pi0Ana.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
pi0Ana.add_argument('-s', '--memory', type=int, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
pi0Ana.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

dummy = subparser.add_parser('dummy', help='Identify events per run from input text file.')
dummy.add_argument('-i', '--entries', type=str, help='Text file of all runs and events', required=True)

dummy2 = subparser.add_parser('dummy2', help='Create pi0Ana submission command for multiple runs.')
dummy2.add_argument('-i', '--fp', type=str, help='Text file of all runs and events', required=True)
dummy2.add_argument('-d', '--directory', type=str, default='.', help='output directory. Default: .')
dummy2.add_argument('-e', '--input-dir', type=str, default='.', help='Directory containing the <run>.list files. Default: .')
dummy2.add_argument('-j', '--jobs', type=str, default=999, help='Number of jobs. Default: 999')

args = parser.parse_args()

def process_dummy2():
    fp        = os.path.realpath(args.fp)
    input_dir = os.path.realpath(args.input_dir)
    output    = args.directory
    jobs      = args.jobs

    print(f'Input file: {fp}')
    print(f'Input dir: {input_dir}')
    print(f'Output Directory: {output}')
    print(f'Jobs: {jobs}')

    with open(fp) as file:
        for line in file:
            print(f'./utils.py pi0Ana -i {input_dir}/{line.split()[0]}.list -c cuts.txt -n {line.split()[1]} -j {jobs} -d {output}/{line.split()[0]} && ', end='')


def process_dummy():
    entries = os.path.realpath(args.entries)

    print(f'Input file: {entries}')

    df = pd.read_csv(entries, delimiter=':', names=['run','entries'])
    df.run = df.run.apply(lambda x: x.split('/')[6])
    print(df.groupby(by='run').sum())

def create_f4a_jobs():
    run_list_dir = os.path.realpath(args.run_list_dir)
    output_dir   = os.path.realpath(args.output)
    f4a          = os.path.realpath(args.f4a)
    executable   = os.path.realpath(args.executable)
    memory       = args.memory
    log          = args.log

    print(f'Run List Directory: {run_list_dir}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(executable, output_dir)

    for filename in os.listdir(run_list_dir):
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
                file.write(f'executable             = ../{os.path.basename(executable)}\n')
                file.write(f'arguments              = {output_dir}/{os.path.basename(f4a)} $(input_dst) output/qa-$(Process).root output/diphoton-$(Process).root\n')
                file.write(f'log                    = {log}\n')
                file.write('output                  = stdout/job-$(Process).out\n')
                file.write('error                   = error/job-$(Process).err\n')
                file.write(f'request_memory         = {memory}GB\n')
                file.write(f'queue input_dst from {filename}')

            print(f'cd {job_dir} && condor_submit genFun4All.sub && ', end='')
    print()

def create_pi0Ana_jobs():
    ntp_list   = os.path.realpath(args.ntp_list)
    cuts       = os.path.realpath(args.cuts)
    fitStats   = os.path.realpath(args.csv) if(args.csv != '') else ''
    Q_vec_corr = os.path.realpath(args.Q_vec_corr) if(args.Q_vec_corr != '') else ''
    script     = os.path.realpath(args.script)
    executable = os.path.realpath(args.executable)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    log        = args.log

    print(f'Run List: {ntp_list}')
    print(f'Cuts: {cuts}')
    print(f'FitStats: {fitStats}')
    print(f'Q Vector Correction: {Q_vec_corr}')
    print(f'Script: {script}')
    print(f'Executable: {executable}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(ntp_list, output_dir)
    shutil.copy(cuts, output_dir)
    if(fitStats != ''):
        shutil.copy(fitStats, output_dir)
    if(Q_vec_corr != ''):
        shutil.copy(Q_vec_corr, output_dir)

    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/output',exist_ok=True)

    cuts       = f'{output_dir}/{os.path.basename(cuts)}'
    fitStats   = f'{output_dir}/{os.path.basename(fitStats)}' if(fitStats != '') else r'\"\"'
    Q_vec_corr = f'{output_dir}/{os.path.basename(Q_vec_corr)}' if(Q_vec_corr != '') else r'\"\"'

    with open(f'{output_dir}/genPi0Ana.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(script)}\n')
        file.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {cuts} {fitStats} {Q_vec_corr} output/test-$(Process).root\n')
        file.write(f'log            = {log}\n')
        file.write( 'output         = stdout/job-$(Process).out\n')
        file.write( 'error          = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        file.write(f'queue input_ntp from {os.path.basename(ntp_list)}')

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
    if(args.command == 'pi0Ana'):
        create_pi0Ana_jobs()
    if(args.command == 'dummy'):
        process_dummy()
    if(args.command == 'dummy2'):
        process_dummy2()
