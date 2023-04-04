#!/usr/bin/env python3
import numpy as np
# import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

create = subparser.add_parser('create')
status = subparser.add_parser('status')

create.add_argument('--executable', type=str, help='Job script to execute.', required=True)
create.add_argument('--macros', type=str, help='Directory of input macros. Directory containing Fun4All_G4_sPHENIX.C and G4Setup_sPHENIX.C.',required=True)
create.add_argument('-n', '--events', type=int, default=1, help='Number of events to generate. Default: 1.')
create.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: Current Directory.')
create.add_argument('-m', '--jobs-per-submission', type=int, default=20000, help='Maximum number of jobs per condor submission. Default: 20000.')
create.add_argument('-j', '--events-per-job', type=int, default=50, help='Number of events to generate per job. Default: 50.')
create.add_argument('--memory', type=int, default=10, help='Memory (units of GB) to request per condor submission. Default: 10 GB.')

status.add_argument('-d','--condor-dir', type=str, help='Condor submission directory.', required=True)

args = parser.parse_args()

def create_jobs():
    events              = args.events
    jobs_per_submission = args.jobs_per_submission
    output_dir          = os.path.abspath(args.output)
    executable          = os.path.abspath(args.executable)
    events_per_job      = args.events_per_job
    memory              = args.memory
    macros_dir          = os.path.abspath(args.macros)
    jobs                = events//events_per_job
    submissions         = jobs//jobs_per_submission
    # add one more submission if there is any remainder left
    if(jobs%jobs_per_submission):
        submissions += 1

    print(f'Events: {events}')
    print(f'Events per job: {events_per_job}')
    print(f'Jobs: {jobs}')
    print(f'Maximum jobs per condor submission: {jobs_per_submission}')
    print(f'Submissions: {submissions}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Output Directory: {output_dir}')
    print(f'Macros Directory: {macros_dir}')
    print(f'Executable: {executable}')
    print('-----------------------------------')

    os.makedirs(output_dir,exist_ok=True)
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
        file.write('transfer_input_files    = src/Fun4All_G4_sPHENIX.C, src/G4Setup_sPHENIX.C\n')
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
        shutil.copy(f'{macros_dir}/Fun4All_G4_sPHENIX.C', f'{submit_dir}/src')
        shutil.copy(f'{macros_dir}/G4Setup_sPHENIX.C', f'{submit_dir}/src')

        file_name = f'{submit_dir}/seed.txt'
        with open(file_name, mode="w") as file:
            for j in range(min(jobs,jobs_per_submission)):
                file.write(f'{j} {i*jobs_per_submission+100}\n')

        jobs -= min(jobs,jobs_per_submission)
        print(f'Written {file_name}')

def get_status():
    condor_dir = os.path.abspath(args.condor_dir)
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


if __name__ == '__main__':
    if(args.command == 'create'):
        create_jobs()
    elif(args.command == 'status'):
        get_status()
