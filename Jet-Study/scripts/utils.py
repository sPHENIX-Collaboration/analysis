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
gen = subparser.add_parser('gen', help='Generate run lists.')
status = subparser.add_parser('status', help='Get status of Condor.')

f4a.add_argument('-i', '--run-list-dir', type=str, help='Directory of run lists', required=True)
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macros/Fun4All_JetVal.C', help='Fun4All macro. Default: macros/Fun4All_JetVal.C')
f4a.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_JetVal', help='Fun4All executable. Default: bin/Fun4All_JetVal')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=1.5, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-n', '--submissions', type=int, default=9, help='Number of submissions. Default: 9.')
# f4a.add_argument('-p', '--concurrency', type=int, default=10000, help='Max number of jobs running at once. Default: 10k.')

gen.add_argument('-o', '--output', type=str, default='files', help='Output Directory. Default: files')
gen.add_argument('-t', '--ana-tag', type=str, default='ana437', help='ana tag. Default: ana437')
gen.add_argument('-s', '--script', type=str, default='scripts/getGoodRunList.py', help='Good run generation script. Default: scripts/getGoodRunList.py')
gen.add_argument('-b', '--bad', type=str, default='files/bad-runs.list', help='List of known bad runs. Default: files/bad-runs.list')

args = parser.parse_args()

def create_f4a_jobs():
    run_list_dir   = os.path.realpath(args.run_list_dir)
    output_dir     = os.path.realpath(args.output)
    f4a            = os.path.realpath(args.f4a)
    macro          = os.path.realpath(args.macro)
    src            = os.path.realpath(args.src)
    executable     = os.path.realpath(args.executable)
    memory         = args.memory
    log            = args.log
    n              = args.submissions
    # p              = args.concurrency

    concurrency_limit = 2308032

    print(f'Run List Directory: {run_list_dir}')
    print(f'Fun4All : {macro}')
    print(f'src: {src}')
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
                file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_dst) output/tree-$(Process).root output/qa-$(Process).root {job_dir}/output\n')
                file.write(f'log            = {log}\n')
                file.write('output          = stdout/job-$(Process).out\n')
                file.write('error           = error/job-$(Process).err\n')
                file.write(f'request_memory = {memory}GB\n')
                file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
                file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
                # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
                file.write(f'queue input_dst from {filename}')

            arr[i%n] = arr[i%n] + f'{run}\n'
            i += 1

    i = 0
    for x in arr:
        with open(f'{output_dir}/sub-{i}.txt', mode="w") as file:
            file.write(x)

        print(f'xargs -L 1 -I {{}} bash -c \'cd {output_dir}/{{}} && condor_submit genFun4All.sub\' < {output_dir}/sub-{i}.txt')
        i += 1

def create_run_lists():
    ana_tag = args.ana_tag
    output  = os.path.realpath(args.output)+'/'+ana_tag
    gen_runs = os.path.realpath(args.script)
    bad_runs = os.path.realpath(args.bad)
    dst_tag  = 'DST_CALOFITTING_run2pp'

    print(f'Tag: {ana_tag}')
    print(f'DST: {dst_tag}')
    print(f'Output: {output}')
    print(f'Good Runs Script: {gen_runs}')
    print(f'Known Bad Runs: {bad_runs}')

    os.makedirs(output,exist_ok=True)

    print('Generating Good Run List')
    subprocess.run(f'{gen_runs}'.split(),cwd=output)

    print('Generating Bad Tower Maps Run List')
    subprocess.run(['bash','-c','find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap -name "*p0*" | cut -d \'-\' -f2 | cut -d c -f1 | sort | uniq > runs-hot-maps.list'],cwd=output)

    print(f'Generating {ana_tag} 2024p007 Run List')
    subprocess.run(['bash','-c',f'CreateDstList.pl --build {ana_tag} --cdb 2024p007 {dst_tag} --printruns > runs-{ana_tag}.list'],cwd=output)

    print('Generating Runs with MBD NS >= 1 and Jet X GeV triggers enabled')
    subprocess.run(['bash','-c',f'psql -h sphnxdaqdbreplica -p 5432 -U phnxro daq -c \'select runnumber from gl1_scaledown where runnumber > 46619 and scaledown10 != -1 and scaledown21 != -1 and scaledown22 != -1 and scaledown23 != -1 order by runnumber;\' -At > runs-trigger-all.list'],cwd=output)
    subprocess.run(['bash','-c',f'psql -h sphnxdaqdbreplica -p 5432 -U phnxro daq -c \'select runnumber from gl1_scaledown where runnumber > 46619 and scaledown10 != -1 and (scaledown21 != -1 or scaledown22 != -1 or scaledown23 != -1) order by runnumber;\' -At > runs-trigger-any.list'],cwd=output)

    print(f'Generating Good {ana_tag} 2024p007 Run List')
    subprocess.run(['bash','-c',f'comm -12 runList.txt runs-{ana_tag}.list > runs-{ana_tag}-good.list'],cwd=output)

    print(f'Generating Good {ana_tag} 2024p007 with Bad Tower Maps Run List')
    subprocess.run(['bash','-c',f'comm -12 runs-{ana_tag}-good.list runs-hot-maps.list > runs-{ana_tag}-good-maps.list'],cwd=output)

    print(f'Generating Good {ana_tag} 2024p007 with triggers')
    subprocess.run(['bash','-c',f'comm -12 runs-{ana_tag}-good-maps.list runs-trigger-all.list > runs-{ana_tag}-good-maps-trigger-all.list'],cwd=output)
    subprocess.run(['bash','-c',f'comm -12 runs-{ana_tag}-good-maps.list runs-trigger-any.list > runs-{ana_tag}-good-maps-trigger-any.list'],cwd=output)

    print('Remove any known bad runs')
    subprocess.run(['bash','-c',f'comm -23 runs-{ana_tag}-good-maps-trigger-all.list {bad_runs} >  runs-{ana_tag}-good-maps-trigger-all-clean.list'],cwd=output)
    subprocess.run(['bash','-c',f'comm -23 runs-{ana_tag}-good-maps-trigger-any.list {bad_runs} >  runs-{ana_tag}-good-maps-trigger-any-clean.list'],cwd=output)

    print('Run Stats')
    subprocess.run(['bash','-c','wc -l *'],cwd=output)

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
    if(args.command == 'gen'):
        create_run_lists()
    if(args.command == 'status'):
        get_condor_status()
