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
f4aSim = subparser.add_parser('f4aSim', help='Create condor submission directory for Fun4All_CaloHotTowerSim.')
gen = subparser.add_parser('gen', help='Generate run lists.')
status = subparser.add_parser('status', help='Get status of Condor.')

f4a.add_argument('-i', '--run-list-dir', type=str, help='Directory of run lists', required=True)
f4a.add_argument('-i2', '--hot-tower-list', type=str, help='Hot Tower List', required=True)
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macro/Fun4All_CaloHotTower.C', help='Fun4All macro. Default: macro/Fun4All_CaloHotTower.C')
f4a.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloHotTower', help='Fun4All executable. Default: bin/Fun4All_CaloHotTower')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=0.5, help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-n', '--segments', type=int, default=10, help='Number of segments to process. Default: 10.')
# f4a.add_argument('-p', '--concurrency', type=int, default=10000, help='Max number of jobs running at once. Default: 10000.')

f4aSim.add_argument('-i', '--segment-list', type=str, help='Segment list', required=True)
f4aSim.add_argument('-e', '--executable', type=str, default='scripts/genFun4AllSim.sh', help='Job script to execute. Default: scripts/genFun4AllSim.sh')
f4aSim.add_argument('-m', '--macro', type=str, default='macro/Fun4All_CaloHotTowerSim.C', help='Fun4All macro. Default: macro/Fun4All_CaloHotTowerSim.C')
f4aSim.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4aSim.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloHotTowerSim', help='Fun4All executable. Default: bin/Fun4All_CaloHotTowerSim')
f4aSim.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4aSim.add_argument('-s', '--memory', type=float, default=0.7, help='Memory (units of GB) to request per condor submission. Default: 0.7 GB.')
f4aSim.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

gen.add_argument('-o', '--output', type=str, default='files', help='Output Directory. Default: files')
gen.add_argument('-t', '--ana-tag', type=str, default='ana446', help='ana tag. Default: ana446')
gen.add_argument('-n', '--events', type=int, default=500000, help='Minimum number of events. Default: 500k')

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
    n              = args.segments
    # p              = args.concurrency

    # concurrency_limit = 2308032

    print(f'Run List Directory: {run_list_dir}')
    print(f'Hot Tower List: {hot_tower_list}')
    print(f'Fun4All : {macro}')
    print(f'src: {src}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Segments: {n}')
    # print(f'Concurrency: {p}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(hot_tower_list, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)
    shutil.copy(executable, output_dir)

    i     = 0
    runs  = []

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

            # shutil.copy(f, job_dir)
            subprocess.run(['bash','-c',f'head -n {n} {f} > {filename}'],cwd=job_dir)

            with open(f'{job_dir}/genFun4All.sub', mode="w") as file:
                file.write(f'executable     = ../{os.path.basename(executable)}\n')
                file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_dst) {output_dir}/{os.path.basename(hot_tower_list)} output/test-$(Process).root\n')
                file.write(f'log            = {log}\n')
                file.write('output          = stdout/job-$(Process).out\n')
                file.write('error           = error/job-$(Process).err\n')
                file.write(f'request_memory = {memory}GB\n')
                file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
                # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
                # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
                file.write(f'queue input_dst from {filename}')

            runs.append(run)

    # print(f'xargs -L 1 -I {{}} bash -c \'cd {output_dir}/{{}} && condor_submit genFun4All.sub\' < {output_dir}/sub-{i}.txt')
            # arr[i%n] = arr[i%n] + f'cd {job_dir} && condor_submit genFun4All.sub && '
            i += 1

    np.savetxt(f'{output_dir}/runs.list',np.array(runs),fmt='%i')

    print(f'while read run; do cd {output_dir}/$run && condor_submit genFun4All.sub; done <{output_dir}/runs.list;')

def create_f4aSim_jobs():
    segment_list   = os.path.realpath(args.segment_list)
    output_dir     = os.path.realpath(args.output)
    f4a            = os.path.realpath(args.f4a)
    macro          = os.path.realpath(args.macro)
    src            = os.path.realpath(args.src)
    executable     = os.path.realpath(args.executable)
    memory         = args.memory
    log            = args.log

    print(f'Segment List: {segment_list}')
    print(f'Fun4All : {macro}')
    print(f'src: {src}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copy(segment_list, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)
    shutil.copy(executable, output_dir)

    try:
        os.symlink(f'{os.path.basename(segment_list)}',f'{output_dir}/jobs.list')
    except FileExistsError:
        print(f'Symlink {output_dir}/jobs.list already exists.')

    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/output',exist_ok=True)

    with open(f'{output_dir}/genFun4All.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_dst) output/test-$(Process).root {output_dir}/output\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(Process).out\n')
        file.write('error           = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        # file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
        file.write(f'queue input_dst from jobs.list')

def get_condor_status():
    hosts = [f'sphnxuser{x:02}' for x in range(1,9)]
    hosts += [f'sphnxsub{x:02}' for x in range(1,3)]

    print(f'hosts: {hosts}')

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

def create_run_lists():
    ana_tag   = args.ana_tag
    threshold = args.events
    output  = os.path.realpath(args.output)+'/'+ana_tag

    print(f'Tag: {ana_tag}')
    print(f'Threshold: {threshold}')
    print(f'Output: {output}')

    os.makedirs(output,exist_ok=True)

    print('Generating Bad Tower Maps Run List')
    subprocess.run(['bash','-c','find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap -name "*p0*" | cut -d \'-\' -f2 | cut -d c -f1 | sort | uniq > runs-hot-maps.list'],cwd=output)

    print(f'Generating {ana_tag}_2024p007 minimum statistics Run List')
    subprocess.run(['bash','-c',f'psql FileCatalog -c "select runnumber from datasets where dataset = \'{ana_tag}_2024p007\' and dsttype=\'DST_CALOFITTING_run2pp\' GROUP BY runnumber having SUM(events) >= {threshold} and runnumber > 46619 order by runnumber;" -At > runs-{ana_tag}.list'],cwd=output)

    print(f'Generating {ana_tag}_2024p007 minimum statistics Run List with Hot maps')
    subprocess.run(['bash','-c',f'join -t \',\' runs-{ana_tag}.list runs-hot-maps.list > runs-{ana_tag}-hot-maps.list'],cwd=output)

    print(f'Generating Timestamp Run List')
    subprocess.run(['bash','-c',f'psql -h sphnxdaqdbreplica -p 5432 -U phnxro daq -c "select runnumber, brtimestamp from run where runnumber > 46619 order by runnumber;" -At --csv > runs-timestamp.list'],cwd=output)

    print(f'Generating {ana_tag}_2024p007 Timestamp Run List')
    subprocess.run(['bash','-c',f'join -t \',\' runs-{ana_tag}.list runs-timestamp.list > runs-{ana_tag}-timestamp.list'],cwd=output)

    print('Run Stats')
    subprocess.run(['bash','-c','wc -l *'],cwd=output)

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
    if(args.command == 'f4aSim'):
        create_f4aSim_jobs()
    if(args.command == 'gen'):
        create_run_lists()
    if(args.command == 'status'):
        get_condor_status()
