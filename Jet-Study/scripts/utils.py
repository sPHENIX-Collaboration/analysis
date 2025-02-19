#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory for Fun4All_JetVal.')
f4a.add_argument('-i', '--run-list', type=str, help='Run list', required=True)
f4a.add_argument('-i2', '--run-list-jet-dir', type=str, default='files/dst-jet', help='Directory for DST JET files')
f4a.add_argument('-i3', '--run-list-jet-calo-dir', type=str, default='files/dst-jet-calo', help='Directory for DST JETCALO files')
f4a.add_argument('-i4', '--single', action='store_true')
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macros/Fun4All_JetValv2.C', help='Fun4All macro. Default: macros/Fun4All_JetValv2.C')
f4a.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_JetValv2', help='Fun4All executable. Default: bin/Fun4All_JetValv2')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=2, help='Memory (units of GB) to request per condor submission. Default: 2 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-n', '--jobs', type=int, default=20000, help='Number of jobs per submission. Default: 20k.')
f4a.add_argument('-t', '--ana-tag', type=str, default='ana462_2024p010_v001', help='ana tag. Default: ana462_2024p010_v001')

def create_f4a_jobs():
    run_list              = os.path.realpath(args.run_list)
    jobs_list             = run_list
    run_list_jet_dir      = os.path.realpath(args.run_list_jet_dir)
    run_list_jet_calo_dir = os.path.realpath(args.run_list_jet_calo_dir)
    output_dir            = os.path.realpath(args.output)
    f4a                   = os.path.realpath(args.f4a)
    macro                 = os.path.realpath(args.macro)
    src                   = os.path.realpath(args.src)
    executable            = os.path.realpath(args.executable)
    ana_tag               = args.ana_tag
    memory                = args.memory
    log                   = args.log
    jobs                  = args.jobs
    single                = args.single
    # p                   = args.concurrency

    concurrency_limit = 2308032

    runs = -1
    jpr = -1

    if(not single):
        runs = int(subprocess.run(['bash','-c',f'wc -l {run_list}'],capture_output=True,text=True).stdout.split(' ')[0])
        jpr = jobs // runs

    print(f'Run List: {run_list}')
    print(f'Run List Jet Dir: {run_list_jet_dir}')
    print(f'Run List Jet Calo Dir: {run_list_jet_calo_dir}')
    print(f'Fun4All: {macro}')
    print(f'Ana Tag: {ana_tag}')
    print(f'src: {src}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Single: {single}')
    if(not single):
        print(f'Jobs per submission: {jobs}')
        print(f'Number of Runs: {runs}')
        print(f'Number of Jobs per Run: {jpr}')
    # print(f'Concurrency: {p}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)
    shutil.copy(executable, output_dir)
    shutil.copy(run_list, output_dir)

    os.makedirs(f'{output_dir}/output',exist_ok=True)
    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)

    if(not single):
        os.makedirs(f'{output_dir}/jobs',exist_ok=True)
        jobs_list = 'jobs.list'

        if os.path.exists(f'{output_dir}/{jobs_list}'):
            os.remove(f'{output_dir}/{jobs_list}')
            print(f'File {output_dir}/{jobs_list} deleted successfully.')

        with open(run_list) as fp:
            for run in fp:
                run = run.strip()

                print(f'Processing: {run}')
                # ensure that run exists
                if not os.path.exists(f'{run_list_jet_dir}/dst_jet_run2pp-{int(run):08}.list'):
                    print(f'Missing: {run}')
                    continue

                # get common segments from both files
                command = f'comm -12 <(cut -d"-" -f3 {run_list_jet_dir}/dst_jet_run2pp-{int(run):08}.list | cut -d "." -f1 | sort) <(cut -d"-" -f3 {run_list_jet_calo_dir}/dst_jetcalo_run2pp-{int(run):08}.list | cut -d "." -f1 | sort)'

                segments = subprocess.run(['bash','-c',command], capture_output=True, encoding="utf-8").stdout.strip().split('\n')

                ctr = 0
                arr1 = [[] for _ in range(jpr)]
                arr2 = [[] for _ in range(jpr)]

                for segment in segments:
                    dst_jet     = f'DST_JET_run2pp_{ana_tag}-{int(run):08}-{segment}.root'
                    dst_jetcalo = f'DST_JETCALO_run2pp_{ana_tag}-{int(run):08}-{segment}.root'
                    arr1[ctr%jpr].append(dst_jet)
                    arr2[ctr%jpr].append(dst_jetcalo)
                    ctr += 1

                ctr = 0
                with open(f'{output_dir}/{jobs_list}',mode='a') as sp:
                    for i in range(len(arr1)):
                        if(not arr1[i]):
                            break

                        file_jet = f'{output_dir}/jobs/dst_jet_run2pp-{ctr:02}-{int(run):08}.list'
                        np.savetxt(file_jet, np.array(arr1[i]), fmt='%s')

                        file_jetcalo = f'{output_dir}/jobs/dst_jetcalo_run2pp-{ctr:02}-{int(run):08}.list'
                        np.savetxt(file_jetcalo, np.array(arr2[i]),fmt='%s')

                        sp.write(f'{os.path.realpath(file_jet)},{os.path.realpath(file_jetcalo)}\n')
                        ctr += 1

    with open(f'{output_dir}/genFun4All.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(input_dst) $(input_dstcalo) test-$(Process).root {output_dir}/output\n')
        file.write(f'log            = {log}\n')
        file.write('output          = stdout/job-$(Process).out\n')
        file.write('error           = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        # file.write(f'PeriodicHold   = (NumJobStarts>=1 && JobStatus == 1)\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n')
        # file.write(f'concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:{int(np.ceil(concurrency_limit/p))}\n')
        file.write(f'queue input_dst,input_dstcalo from {os.path.basename(jobs_list)}')

gen = subparser.add_parser('gen', help='Generate run lists.')
gen.add_argument('-o', '--output', type=str, default='files', help='Output Directory. Default: files')
gen.add_argument('-r', '--run-min', type=int, default=47289, help='Minimum Run Number. Default: 47289')
gen.add_argument('-r2', '--events-min', type=int, default=1000000, help='Minimum Events in Run. Default: 1M')
gen.add_argument('-d', '--dataset', type=str, default='ana462_2024p010_v001', help='Production Dataset. Default: ana462_2024p010_v001')
gen.add_argument('-d2', '--dsttype', type=str, default='DST_CALO_run2pp', help='Production DST Type. Default: DST_CALO_run2pp')

def create_run_lists():
    output  = os.path.realpath(args.output)
    run_min = args.run_min
    events_min = args.events_min
    dataset = args.dataset
    dsttype = args.dsttype

    print(f'Output: {output}')
    print(f'Dataset: {dataset}')
    print(f'Dst Type: {dsttype}')
    print(f'Minimum Run Number: {run_min}')
    print(f'Minimum Events in Run: {events_min}')

    os.makedirs(output,exist_ok=True)

    # Get list of all runs with magnet on
    print('Generating Magnet On Run List')
    command = f'psql -h sphnxdaqdbreplica daq -c "select runnumber from magnet_info where magnet_on = \'true\' and runnumber >= {run_min} order by runnumber;" -At > runs-magnet-on.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs with MBD N&S >= 1 Enabled
    print('Generating MBD N&S >= 1 Enabled Run List')
    command = f'psql -h sphnxdaqdbreplica daq -c "select runnumber from gl1_scaledown where scaledown10 != -1 and runnumber >= {run_min};" -At > runs-MBD-NS-1-trigger-on.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs marked as golden in EMCal, IHCal and OHCal
    print('Generating Golden Run List')
    command = f'psql Production -h sphnxproddbmaster.sdcc.bnl.gov -c "select runnumber from goodruns WHERE (emcal_auto).runclass = \'GOLDEN\' and (ohcal_auto).runclass = \'GOLDEN\' and (ihcal_auto).runclass = \'GOLDEN\' and runnumber >= {run_min} order by runnumber;" -At > runs-golden.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs that contain a bad tower map
    print('Generating Bad Tower Maps Run List')
    command = f'find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap -name "*p0*" | cut -d \'-\' -f2 | cut -d c -f1 | sort | uniq > runs-hot-maps.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs with MBD Live time greater than 80%
    print('Generating MBD Live time 80% Run List')
    command = f'psql -h sphnxdaqdbreplica daq -c "select runnumber from gl1_scalers where index = 10 and runnumber >= {run_min} and live*100/raw >= 80 order by runnumber;" -At > runs-MBD-live-time-80.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs at least 1M events
    print('Generating Minimum 1M events Run List')
    command = f'psql FileCatalog -c "select runnumber from datasets where dsttype = \'{dsttype}\' and dataset = \'{dataset}\' and runnumber >= {run_min} group by runnumber having sum(events) >= {events_min};" -At > runs-events-min-{events_min}.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    # Get list of all runs at least 5 minutes duration
    print('Generating Minimum 5 Minute Duration Run List')
    command = f'psql -h sphnxdaqdbreplica daq -c "select runnumber from run where runnumber >= {run_min} and ertimestamp - brtimestamp >= interval \'5 minutes\' order by runnumber;" -At > runs-time-min-5.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    print('Generating Good Run List')
    good_runs = f'runs-good-{dsttype}-{dataset}.list'
    command = f'comm -12 runs-hot-maps.list <(comm -12 runs-MBD-NS-1-trigger-on.list <(comm -12 runs-magnet-on.list <(comm -12 runs-MBD-live-time-80.list <(comm -12 runs-golden.list runs-events-min-{events_min}.list)))) > {good_runs}'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    print('Good Runs Less than 5 Minutes')
    command = f'comm -23 {good_runs} runs-time-min-5.list > {os.path.splitext(good_runs)[0]}-without-time-min-5.list'
    result = subprocess.run(['bash','-c',command],cwd=output)
    if(result.returncode != 0):
        print(f'Error in {command}')
        return

    print('Run Stats')
    subprocess.run(['bash','-c','wc -l *.list'],cwd=output)

status = subparser.add_parser('status', help='Get status of Condor.')

def get_condor_status():
    hosts = [f'sphnxuser{x:02}' for x in range(1,9)]

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

args = parser.parse_args()

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
    if(args.command == 'gen'):
        create_run_lists()
    if(args.command == 'status'):
        get_condor_status()
