#!/usr/bin/env python3
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create = subparser.add_parser('create', help='Create condor submission directory.')
status = subparser.add_parser('status', help='Check the status of the condor submission.')
hadd   = subparser.add_parser('hadd', help='Merge completed condor jobs.')

create.add_argument('-e', '--executable', type=str, default='scripts/genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
create.add_argument('-a', '--macros', type=str, default='current/macro', help='Directory of input macros. Directory containing Fun4All_G4_sPHENIX.C and G4Setup_sPHENIX.C. Default: current/macro')
create.add_argument('-b', '--bin-dir', type=str, default='current/bin', help='Directory containing Fun4All_G4_sPHENIX executable. Default: current/bin')
create.add_argument('-n', '--events', type=int, default=1, help='Number of events to generate. Default: 1.')
create.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: Current Directory.')
create.add_argument('-m', '--jobs-per-submission', type=int, default=20000, help='Maximum number of jobs per condor submission. Default: 20000.')
create.add_argument('-j', '--events-per-job', type=int, default=100, help='Number of events to generate per job. Default: 100.')
create.add_argument('-s', '--memory', type=int, default=6, help='Memory (units of GB) to request per condor submission. Default: 6 GB.')
create.add_argument('-u', '--build-tag', type=str, default='unknown', help='Specify build tag. Ex: ana.xxx, Default: unknown')

status.add_argument('-d','--condor-dir', type=str, help='Condor submission directory.', required=True)

hadd.add_argument('-i','--job-dir-list', type=str, help='List of directories containing condor jobs to be merged.', required=True)
hadd.add_argument('-o','--output', type=str, default='test.root', help='Output root file. Default: test.root.')
hadd.add_argument('-n','--jobs-per-hadd', type=int, default=5000, help='Number of jobs to merge per hadd call. Default: 5000.')
hadd.add_argument('-j','--jobs-open', type=int, default=50, help='Number of jobs to load at once. Default: 50.')
hadd.add_argument('-m','--multiple-submit-dir', type=bool, default=False,help='If merging condor jobs over multiple directories. Default: False')

args = parser.parse_args()

def create_jobs():
    events              = args.events
    jobs_per_submission = args.jobs_per_submission
    output_dir          = os.path.realpath(args.output)
    bin_dir             = os.path.realpath(args.bin_dir)
    executable          = os.path.realpath(args.executable)
    events_per_job      = min(args.events_per_job, events)
    memory              = args.memory
    macros_dir          = os.path.realpath(args.macros)
    jobs                = events//events_per_job
    submissions         = int(np.ceil(jobs/jobs_per_submission))
    tag                 = args.build_tag

    print(f'Events: {events}')
    print(f'Events per job: {events_per_job}')
    print(f'Jobs: {jobs}')
    print(f'Maximum jobs per condor submission: {jobs_per_submission}')
    print(f'Submissions: {submissions}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Output Directory: {output_dir}')
    print(f'Macros Directory: {macros_dir}')
    print(f'Bin Directory: {bin_dir}')
    print(f'Executable: {executable}')
    print(f'Build Tag: {tag}')

    os.makedirs(output_dir,exist_ok=True)
    with open(f'{output_dir}/log.txt', mode='w') as file:
        file.write(f'Events: {events}\n')
        file.write(f'Events per job: {events_per_job}\n')
        file.write(f'Jobs: {jobs}\n')
        file.write(f'Maximum jobs per condor submission: {jobs_per_submission}\n')
        file.write(f'Submissions: {submissions}\n')
        file.write(f'Requested memory per job: {memory}GB\n')
        file.write(f'Output Directory: {output_dir}\n')
        file.write(f'Macros Directory: {macros_dir}\n')
        file.write(f'Bin Directory: {bin_dir}\n')
        file.write(f'Executable: {executable}\n')
        file.write(f'Build Tag: {tag}\n')

    # Generate condor submission file
    condor_file = f'{output_dir}/genFun4All.sub'
    with open(condor_file, mode="w") as file:
        file.write(f'executable             = bin/{os.path.basename(executable)}\n')
        file.write(f'arguments              = $(myPid) $(initialSeed) {events_per_job}\n')
        file.write('log                     = log/job-$(myPid).log\n')
        file.write('output                  = stdout/job-$(myPid).out\n')
        file.write('error                   = error/job-$(myPid).err\n')
        file.write(f'request_memory         = {memory}GB\n')
        file.write('should_transfer_files   = YES\n')
        file.write('when_to_transfer_output = ON_EXIT\n')
        # file.write('transfer_input_files    = src/Fun4All_G4_sPHENIX.C, src/G4Setup_sPHENIX.C\n')
        file.write('transfer_input_files    = bin/Fun4All_G4_sPHENIX\n')
        file.write('transfer_output_files   = G4sPHENIX_g4cemc_eval-$(myPid).root\n')
        file.write('transfer_output_remaps  = "G4sPHENIX_g4cemc_eval-$(myPid).root = output/G4sPHENIX_g4cemc_eval-$(myPid).root"\n')
        file.write('queue myPid,initialSeed from seed.txt')

    for i in range(submissions):
        print(f'Submission: {i}')

        submit_dir = f'{output_dir}/submission-{i}'
        print(f'Submission Directory: {submit_dir}')

        os.makedirs(f'{submit_dir}/stdout',exist_ok=True)
        os.makedirs(f'{submit_dir}/error',exist_ok=True)
        os.makedirs(f'{submit_dir}/log',exist_ok=True)
        os.makedirs(f'{submit_dir}/output',exist_ok=True)
        os.makedirs(f'{submit_dir}/bin',exist_ok=True)
        os.makedirs(f'{submit_dir}/src',exist_ok=True)

        shutil.copy(condor_file, submit_dir)
        shutil.copy(executable, f'{submit_dir}/bin')
        shutil.copy(f'{bin_dir}/Fun4All_G4_sPHENIX', f'{submit_dir}/bin')
        shutil.copy(f'{macros_dir}/Fun4All_G4_sPHENIX.C', f'{submit_dir}/src')
        shutil.copy(f'{macros_dir}/G4Setup_sPHENIX.C', f'{submit_dir}/src')

        file_name = f'{submit_dir}/seed.txt'
        with open(file_name, mode="w") as file:
            for j in range(min(jobs,jobs_per_submission)):
                file.write(f'{j} {i*jobs_per_submission+100}\n')

        jobs -= min(jobs,jobs_per_submission)
        print(f'Written {file_name}')

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

if __name__ == '__main__':
    if(args.command == 'create'):
        create_jobs()
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
