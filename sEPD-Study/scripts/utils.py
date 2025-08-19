#!/usr/bin/env python3
"""
This module automates the Fun4All Condor Procedure
"""

import argparse
import os
import sys
import subprocess
import datetime
import shutil
import logging
from pathlib import Path
import math
import re

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory.')

f4a.add_argument('-i'
                    , '--input-list', type=str
                    , required=True
                    , help='Input DST List.')

f4a.add_argument('-i2'
                    , '--dbtag', type=str
                    , default='newcdbtag'
                    , help='CDB Tag. Default: newcdbtag')

f4a.add_argument('-i3'
                    , '--q-vec-corr-list', type=str
                    , default=''
                    , help='Q vector correction file list. Default: ""')

f4a.add_argument('-o'
                    , '--output-dir', type=str
                    , default='scratch/test'
                    , help='Project Directory. Default: scratch/test')

f4a.add_argument('-n'
                    , '--events', type=int
                    , default=0
                    , help='Number of events to analyze. Default: All.')

f4a.add_argument('-e'
                    , '--do-event-plane'
                    , action='store_true'
                    , help='Do Official Event Plane Reco. Default: True.')

f4a.add_argument('-s'
                    , '--memory', type=float
                    , default=2
                    , help='Memory (units of GB) to request per condor submission. Default: 2 GB.')

f4a.add_argument('-l'
                    , '--log-file', type=str
                    , default='log.txt'
                    , help='Log File. Default: log.txt')

f4a.add_argument('-l2'
                    , '--condor-log-dir', type=str
                    , default='/tmp/anarde/dump'
                    , help='Condor Log Directory. Default: /tmp/anarde/dump')

f4a.add_argument('-f'
                    , '--f4a-macro', type=str
                    , default='macros/Fun4All_sEPD.C'
                    , help='Fun4All Macro. Default: macros/Fun4All_sEPD.C')

f4a.add_argument('-f2'
                    , '--src-dir', type=str
                    , default='src'
                    , help='Source Files Directory. Default: src')

f4a.add_argument('-f3'
                    , '--condor-script', type=str
                    , default='scripts/genFun4All.sh'
                    , help='Condor Script. Default: scripts/genFun4All.sh')

f4a.add_argument('-f4'
                    , '--common-errors', type=str
                    , default='files/common-errors.txt'
                    , help='Common Errors. Default: files/common-errors.txt')

f4a.add_argument('-b'
                    , '--f4a-bin', type=str
                    , default='bin/Fun4All_sEPD'
                    , help='Fun4All Bin. Default: bin/Fun4All_sEPD')

def setup_logging(log_file, log_level):
    """Configures the logging system to output to a file and console."""

    # Create a logger instance
    logger = logging.getLogger(__name__) # Use __name__ to get a logger specific to this module
    logger.setLevel(log_level)

    # Clear existing handlers to prevent duplicate output if run multiple times
    if logger.hasHandlers():
        logger.handlers.clear()

    # Create a formatter for log messages
    formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')

    # Create a FileHandler to save logs to a file
    file_handler = logging.FileHandler(log_file)
    file_handler.setLevel(log_level)
    file_handler.setFormatter(formatter)
    logger.addHandler(file_handler)

    # Create a StreamHandler to also output logs to the console (optional)
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.INFO) # Console might only show INFO and above
    console_handler.setFormatter(formatter)
    logger.addHandler(console_handler)

    return logger

def run_command_and_log(command, logger, current_dir = '.', do_logging = True, description="Executing command"):
    """
    Runs an external command using subprocess and logs its stdout, stderr, and return code.
    """
    if do_logging:
        logger.info(f"{description}: '{command}'")

    try:
        # subprocess.run is the recommended high-level API
        # capture_output=True: captures stdout and stderr
        # text=True: decodes output as text (usually UTF-8)
        # check=False: do NOT raise an exception for non-zero exit codes immediately.
        #              We want to log stderr even on failure before deciding to raise.
        result = subprocess.run(['bash','-c',command], cwd=current_dir, capture_output=True, text=True, check=False)

        # Log stdout if any
        if result.stdout and do_logging:
            # Using logger.debug allows capturing even verbose outputs
            logger.debug(f"  STDOUT from '{command}':\n{result.stdout.strip()}")

        # Log stderr if any
        if result.stderr:
            # Using logger.error for stderr, as it often indicates problems
            logger.error(f"  STDERR from '{command}':\n{result.stderr.strip()}")

        if do_logging:
            # Log the return code
            logger.info(f"  Command exited with code: {result.returncode}")

        # You can choose to raise an exception here if the command failed
        if result.returncode != 0:
            logger.error(f"Command failed: '{command}' exited with non-zero code {result.returncode}")
            # Optionally, raise an error to stop execution
            # raise subprocess.CalledProcessError(result.returncode, command, output=result.stdout, stderr=result.stderr)
            return False
        return True

    except FileNotFoundError:
        logger.critical(f"Error: Command '{command}' not found. Is it in your PATH?")
        return False
    except Exception as e:
        logger.critical(f"An unexpected error occurred while running '{command}': {e}")
        return False

def f4a_read_q_vec_hists(file_name, logger):
    """ Reads list of Q vector corrections into a dictionary."""
    q_vec_hists_dict = {}
    with open(file_name, mode='r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            run_match = re.search(r'-(\d+)\.', line)

            if run_match:
                runnumber = int(run_match.group(1))
            else:
                logger.info(f'Error! Cannot extract runnumber from: {line}, skipping')
                continue

            q_vec_hists_dict[runnumber] = line

    return q_vec_hists_dict

def create_f4a_jobs():
    """
    Create Fun4All Jobs
    """
    input_list = os.path.realpath(args.input_list)
    dbtag = args.dbtag
    q_vec_corr_list = os.path.realpath(args.q_vec_corr_list) if args.q_vec_corr_list else ''
    events = args.events
    do_ep = args.do_event_plane
    output_dir = os.path.realpath(args.output_dir)
    log_file  = os.path.join(output_dir, args.log_file)
    f4a_macro = os.path.realpath(args.f4a_macro)
    f4a_bin = os.path.realpath(args.f4a_bin)
    src_dir = os.path.realpath(args.src_dir)
    condor_memory = args.memory
    condor_script = os.path.realpath(args.condor_script)
    condor_log_dir = os.path.realpath(args.condor_log_dir)
    common_errors = os.path.realpath(args.common_errors)

    # Create Dirs
    os.makedirs(output_dir, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that paths exists
    if not os.path.isfile(input_list):
        logger.critical(f'File: {input_list} does not exist!')
        sys.exit()

    if q_vec_corr_list and not os.path.isfile(q_vec_corr_list):
        logger.critical(f'File: {q_vec_corr_list} does not exist!')
        sys.exit()

    if not os.path.isfile(f4a_bin):
        logger.critical(f'File: {f4a_bin} does not exist!')
        sys.exit()

    if not os.path.isfile(f4a_macro):
        logger.critical(f'File: {f4a_macro} does not exist!')
        sys.exit()

    if not os.path.isfile(condor_script):
        logger.critical(f'File: {condor_script} does not exist!')
        sys.exit()

    if not os.path.isfile(common_errors):
        logger.critical(f'File: {common_errors} does not exist!')
        sys.exit()

    if not os.path.exists(src_dir):
        logger.critical(f'Directory: {src_dir} does not exist!')
        sys.exit()

    # Compute the total number of DSTs in the list files
    total_files = int(subprocess.run(['bash','-c',f'cat {input_list} | xargs -I {{}} sh -c \'test -f "{{}}" && wc -l "{{}}"\' | awk \'{{sum += $1}} END {{print sum}}\''], capture_output=True, encoding='utf-8', check=False).stdout.strip())

    # Setup Condor Log Dir
    os.makedirs(condor_log_dir, exist_ok=True)

    if os.path.exists(condor_log_dir):
        shutil.rmtree(condor_log_dir)
        os.makedirs(condor_log_dir, exist_ok=True)

    # Copy necessary files to the output directory
    shutil.copy(input_list, output_dir)
    shutil.copy(f4a_macro, output_dir)
    shutil.copy(f4a_bin, output_dir)
    shutil.copy(common_errors, output_dir)
    shutil.copytree(src_dir, os.path.join(output_dir,'src'), dirs_exist_ok=True)
    if q_vec_corr_list:
        q_vec_hists_dict = f4a_read_q_vec_hists(q_vec_corr_list, logger)
        shutil.copy(q_vec_corr_list, output_dir)

    f4a_bin = os.path.join(output_dir, os.path.basename(f4a_bin))

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input DST List: {input_list}')
    logger.info(f'Input Q-vector correction list: {q_vec_corr_list}')
    logger.info(f'Total DSTs: {total_files}')
    logger.info(f'Events to process per job: {events if events != 0 else "All"}')
    logger.info(f'Do Event Plane Reco: {do_ep}')
    logger.info(f'DB Tag: {dbtag}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Fun4All Macro: {f4a_macro}')
    logger.info(f'Fun4All Bin: {f4a_bin}')
    logger.info(f'Source Directory: {src_dir}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')
    logger.info(f'Common Errors File: {common_errors}')

    CONDOR_SUBMISSION_LIMIT = 20000

    files_per_job = math.ceil(total_files / CONDOR_SUBMISSION_LIMIT)
    logger.info(f'Files Per Job: {files_per_job}')

    files_dir = os.path.join(output_dir, 'files')
    os.makedirs(files_dir, exist_ok=True)

    jobs_file = os.path.join(output_dir, 'jobs.list')
    if os.path.exists(jobs_file):
        os.remove(jobs_file)
        logger.info(f"File '{jobs_file}' deleted successfully.")

    with open(input_list, mode='r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            run_match = re.search(r'-(\d+)\.', line)

            q_vec_hist_exists = False

            if run_match:
                runnumber = int(run_match.group(1))
                if q_vec_corr_list and runnumber in q_vec_hists_dict:
                    q_vec_hist = q_vec_hists_dict[runnumber]
                    q_vec_hist_exists = True
            else:
                logger.info(f'Error! Cannot extract runnumber from: {line}, skipping')
                continue

            logger.info(f'Processing: {runnumber}, {os.path.basename(line)}')
            file_stem = Path(line).stem

            command = f'split --lines {files_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
            run_command_and_log(command, logger, files_dir, False)

            command = f'readlink -f {files_dir}/{file_stem}* > jobs-temp.list'
            run_command_and_log(command, logger, output_dir, False)

            if q_vec_hist_exists:
                command = f'sed -i \'s#$#,{q_vec_hist}#\' jobs-temp.list'
                run_command_and_log(command, logger, output_dir, False)
            else:
                command = 'sed -i \'s#$#,none#\' jobs-temp.list'
                run_command_and_log(command, logger, output_dir, False)

            command = 'cat jobs-temp.list >> jobs.list'
            run_command_and_log(command, logger, output_dir, False)

    os.remove(os.path.join(output_dir, 'jobs-temp.list'))

    os.makedirs(f'{output_dir}/output', exist_ok=True)
    os.makedirs(f'{output_dir}/stdout', exist_ok=True)
    os.makedirs(f'{output_dir}/error', exist_ok=True)

    shutil.copy(condor_script, output_dir)

    with open(os.path.join(output_dir,'genFun4All.sub'), mode='w', encoding='utf-8') as file:
        file.write(f'executable    = {os.path.basename(condor_script)}\n')
        file.write(f'arguments     = {f4a_bin} $(input_dst) test-$(ClusterId)-$(Process).root {events} {dbtag} {int(do_ep)} $(input_q_vec_hist) {output_dir}/output\n')
        file.write(f'log           = {condor_log_dir}/job-$(ClusterId)-$(Process).log\n')
        file.write('output         = stdout/job-$(ClusterId)-$(Process).out\n')
        file.write('error          = error/job-$(ClusterId)-$(Process).err\n')
        file.write(f'request_memory = {condor_memory}GB\n')

    command = f'cd {output_dir} && condor_submit genFun4All.sub -queue "input_dst,input_q_vec_hist from jobs.list"'
    logger.info(command)

hadd = subparser.add_parser('hadd', help='hadd condor jobs.')

hadd.add_argument('-i'
                    , '--input-dir', type=str
                    , required=True
                    , help='Directory of runs to combine output.')

hadd.add_argument('-o'
                    , '--output-dir', type=str
                    , default='scratch/test'
                    , help='Output Directory. Default: scratch/test')

hadd.add_argument('-n'
                    , '--hadd-max', type=int
                    , default=51
                    , help='Hadd Max at once. Default: 51')

hadd.add_argument('-s'
                    , '--memory', type=float
                    , default=0.5
                    , help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')

hadd.add_argument('-l'
                    , '--log-file', type=str
                    , default='log.txt'
                    , help='Log File. Default: log.txt')

hadd.add_argument('-l2'
                    , '--condor-log-dir', type=str
                    , default='/tmp/anarde/dump'
                    , help='Condor Log Directory. Default: /tmp/anarde/dump')

hadd.add_argument('-f'
                    , '--condor-script', type=str
                    , default='scripts/genHadd.sh'
                    , help='Condor Script. Default: scripts/genHadd.sh')

def hadd_jobs():
    """
    hadd condor jobs
    """
    input_dir     = os.path.realpath(args.input_dir)
    output_dir     = os.path.realpath(args.output_dir)
    hadd_max       = args.hadd_max
    log_file       = os.path.join(output_dir, args.log_file)
    condor_memory  = args.memory
    condor_script  = os.path.realpath(args.condor_script)
    condor_log_dir = os.path.realpath(args.condor_log_dir)

    # Create Dirs
    os.makedirs(output_dir, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that paths exists
    if not os.path.isdir(input_dir):
        logger.critical(f'Dir: {input_dir} does not exist!')
        sys.exit()

    if not os.path.isfile(condor_script):
        logger.critical(f'File: {condor_script} does not exist!')
        sys.exit()

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input Dir: {input_dir}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Hadd Max: {hadd_max}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    # Setup Condor Log Dir
    os.makedirs(condor_log_dir, exist_ok=True)

    if os.path.exists(condor_log_dir):
        shutil.rmtree(condor_log_dir)
        os.makedirs(condor_log_dir, exist_ok=True)

    os.makedirs(f'{output_dir}/output', exist_ok=True)
    os.makedirs(f'{output_dir}/stdout', exist_ok=True)
    os.makedirs(f'{output_dir}/error', exist_ok=True)

    shutil.copy(condor_script, output_dir)

    command = f'readlink -f {input_dir}/* > jobs.list'
    run_command_and_log(command, logger, output_dir, False)

    with open(os.path.join(output_dir,'genHadd.sub'), mode='w', encoding='utf-8') as file:
        file.write(f'executable    = {os.path.basename(condor_script)}\n')
        file.write(f'arguments     = $(input_dir) {hadd_max} {output_dir}/output\n')
        file.write(f'log           = {condor_log_dir}/job-$(ClusterId)-$(Process).log\n')
        file.write('output         = stdout/job-$(ClusterId)-$(Process).out\n')
        file.write('error          = error/job-$(ClusterId)-$(Process).err\n')
        file.write(f'request_memory = {condor_memory}GB\n')

    command = f'cd {output_dir} && condor_submit genHadd.sub -queue "input_dir from jobs.list"'
    logger.info(command)

args = parser.parse_args()

if __name__ == "__main__":
    if args.command == 'f4a':
        create_f4a_jobs()

    if args.command == 'hadd':
        hadd_jobs()
