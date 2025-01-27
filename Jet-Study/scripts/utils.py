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
gen = subparser.add_parser('gen', help='Generate run lists.')
status = subparser.add_parser('status', help='Get status of Condor.')

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

gen.add_argument('-o', '--output', type=str, default='files', help='Output Directory. Default: files')
gen.add_argument('-t', '--ana-tag', type=str, default='ana437', help='ana tag. Default: ana437')
gen.add_argument('-s', '--script', type=str, default='scripts/getGoodRunList.py', help='Good run generation script. Default: scripts/getGoodRunList.py')
gen.add_argument('-b', '--bad', type=str, default='files/bad-runs.list', help='List of known bad runs. Default: files/bad-runs.list')

args = parser.parse_args()

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
    print(f'Fun4All : {macro}')
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
    hosts = [f'sphnxuser{x:02}' for x in range(1,9)]
    hosts += [f'sphnxsub{x:02}' for x in range(1,3)]

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
