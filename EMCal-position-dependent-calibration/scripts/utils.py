#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a        = subparser.add_parser('f4a', help='Create condor submission directory for single photon sample.')
status     = subparser.add_parser('status', help='Check the status of the condor submission.')
hadd       = subparser.add_parser('hadd', help='Merge completed condor jobs.')
validation = subparser.add_parser('vd', help='Create condor submission directory for validation.')

f4a.add_argument('-n', '--events', type=int, help='Number of events to generate.', required=True)
f4a.add_argument('-j', '--events-per-job', type=int, default=200, help='Number of events to generate per job. Default: 200.')
f4a.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macro/new/Fun4All_G4_sPHENIX.C', help='Fun4All macro. Default: macro/new/Fun4All_G4_sPHENIX.C')
f4a.add_argument('-m2', '--src', type=str, default='src/validation', help='Directory Containing src files. Default: src/validation')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_G4_sPHENIX', help='Fun4All executable. Default: bin/Fun4All_G4_sPHENIX')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=3, help='Memory (units of GB) to request per condor submission. Default: 3 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

status.add_argument('-d','--condor-dir', type=str, help='Condor submission directory.', required=True)

hadd.add_argument('-i','--job-dir-list', type=str, help='List of directories containing condor jobs to be merged.', required=True)
hadd.add_argument('-o','--output', type=str, default='test.root', help='Output root file. Default: test.root.')
hadd.add_argument('-n','--jobs-per-hadd', type=int, default=5000, help='Number of jobs to merge per hadd call. Default: 5000.')
hadd.add_argument('-j','--jobs-open', type=int, default=50, help='Number of jobs to load at once. Default: 50.')
hadd.add_argument('-m','--multiple-submit-dir', type=bool, default=False,help='If merging condor jobs over multiple directories. Default: False')

validation.add_argument('-i', '--file-list', type=str, help='List of files', required=True)
validation.add_argument('-e', '--executable', type=str, default='scripts/genFun4Allv2.sh', help='Job script to execute. Default: scripts/genFun4Allv2.sh')
validation.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloTreeGen', help='Fun4All executable. Default: bin/Fun4All_CaloTreeGen')
validation.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
validation.add_argument('-s', '--memory', type=int, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
validation.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

args = parser.parse_args()

def f4a_jobs():
    events         = args.events
    events_per_job = args.events_per_job
    executable     = os.path.realpath(args.executable)
    f4a            = os.path.realpath(args.f4a)
    output_dir     = os.path.realpath(args.output)
    memory         = args.memory
    log            = args.log
    macro          = os.path.realpath(args.macro)
    src            = os.path.realpath(args.src)
    jobs           = events // events_per_job

    print(f'Jobs: {jobs}')
    print(f'Events per job: {events_per_job}')
    print(f'Events: {events}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Output Directory: {output_dir}')
    print(f'Executable: {executable}')
    print(f'f4a: {f4a}')
    print(f'Condor log file: {log}')
    print(f'src: {src}')
    print(f'macro: {macro}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(executable, output_dir)
    shutil.copy(f4a, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)

    submissions = jobs // 20000

    with open(f'{output_dir}/log.txt', mode='w') as file:
        file.write(f'Jobs: {jobs}\n')
        file.write(f'Events per job: {events_per_job}\n')
        file.write(f'Events: {events}\n')
        file.write(f'Requested memory per job: {memory}GB\n')
        file.write(f'Output Directory: {output_dir}\n')
        file.write(f'Executable: {executable}\n')
        file.write(f'f4a: {f4a}\n')
        file.write(f'Condor log file: {log}\n')
        file.write(f'src: {src}\n')
        file.write(f'macro: {macro}\n')

    for i in range(submissions):
        os.makedirs(f'{output_dir}/{i}',exist_ok=True)
        os.makedirs(f'{output_dir}/{i}/stdout',exist_ok=True)
        os.makedirs(f'{output_dir}/{i}/error',exist_ok=True)
        os.makedirs(f'{output_dir}/{i}/output',exist_ok=True)

        with open(f'{output_dir}/{i}/genFun4All.sub', mode="w") as file:
            file.write(f'executable     = ../{os.path.basename(executable)}\n')
            file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} {events_per_job} 0 output $(Process) test.root\n')
            file.write(f'log            = {log}\n')
            file.write( 'output         = stdout/job-$(Process).out\n')
            file.write( 'error          = error/job-$(Process).err\n')
            file.write(f'request_memory = {memory}GB\n')
            file.write(f'queue 20000')

        print(f'cd {output_dir}/{i} && condor_submit genFun4All.sub')

def get_status():
    condor_dir = os.path.realpath(args.condor_dir)
    submit_dirs = next(os.walk(condor_dir))[1]
    print(f'Condor Directory: {condor_dir}')
    jobs_done_total = 0
    total = 0
    for submit_dir in submit_dirs:
        jobs_done = len(os.listdir(f'{condor_dir}/{submit_dir}/output'))
        jobs_total = len(os.listdir(f'{condor_dir}/{submit_dir}/log'))
        if(jobs_total != 0):
            print(f'Condor submission dir: {condor_dir}/{submit_dir}, done: {jobs_done}, {jobs_done/jobs_total*100:.2f} %')
            jobs_done_total += jobs_done
            total += jobs_total

    if(total != 0):
        print(f'Total jobs done: {jobs_done_total}, {jobs_done_total/total*100:.2f} %')

def hadd(jobs_dir):
    output        = os.path.realpath(args.output)
    jobs_per_hadd = args.jobs_per_hadd
    jobs_open     = args.jobs_open+1
    print(f'jobs directory: {jobs_dir}')
    print(f'output: {output}')
    print(f'jobs per hadd: {jobs_per_hadd}')
    print(f'jobs open at once: {jobs_open-1}')

    jobs = os.listdir(jobs_dir)
    jobs = [f'{jobs_dir}/{job}' for job in jobs]

    total_jobs = len(jobs)
    hadd_calls = int(np.ceil(total_jobs/jobs_per_hadd))

    print(f'total jobs: {total_jobs}')
    print(f'hadd calls: {hadd_calls}')

    for i in range(hadd_calls):
        subprocess.run(['echo', '#######################'])
        subprocess.run(['echo', f'working on hadd: {i}'])
        command = f'hadd -a -n {jobs_open} {output}'.split()
        i_start = jobs_per_hadd*i
        i_end = min(jobs_per_hadd*(i+1), total_jobs)
        subprocess.run(['echo', f'i_start: {i_start}, i_end: {i_end}'])
        command.extend(jobs[i_start:i_end])
        subprocess.run(command)
        subprocess.run(['echo', f'done with hadd: {i}'])
        subprocess.run(['echo', '#######################'])

def create_validation_jobs():
    file_list  = os.path.realpath(args.file_list)
    executable = os.path.realpath(args.executable)
    f4a        = os.path.realpath(args.f4a)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    log        = args.log

    print(f'File List: {file_list}')
    print(f'Executable: {executable}')
    print(f'f4a: {f4a}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(executable, output_dir)
    shutil.copy(f4a, output_dir)
    shutil.copy(file_list, output_dir)

    os.makedirs(f'{output_dir}/stdout',exist_ok=True)
    os.makedirs(f'{output_dir}/error',exist_ok=True)
    os.makedirs(f'{output_dir}/output',exist_ok=True)

    with open(f'{output_dir}/genFun4All.sub', mode="w") as file:
        file.write(f'executable     = {os.path.basename(executable)}\n')
        file.write(f'arguments      = {output_dir}/{os.path.basename(f4a)} $(dst_calo_cluster) $(g4hits) output/test-$(Process).root\n')
        file.write(f'log            = {log}\n')
        file.write( 'output         = stdout/job-$(Process).out\n')
        file.write( 'error          = error/job-$(Process).err\n')
        file.write(f'request_memory = {memory}GB\n')
        file.write(f'queue dst_calo_cluster, g4hits from {os.path.basename(file_list)}')

if __name__ == '__main__':
    if(args.command == 'f4a'):
        f4a_jobs()
    elif(args.command == 'status'):
        get_status()
    elif(args.command == 'hadd'):
        if(args.multiple_submit_dir):
            job_dir_list  = os.path.realpath(args.job_dir_list)
            with open(job_dir_list) as f:
                for jobs_dir in f:
                    jobs_dir = jobs_dir.strip()
                    hadd(jobs_dir)
        else:
            job_dir = args.job_dir_list
            hadd(jobs_dir)

    elif(args.command == 'vd'):
        create_validation_jobs()
