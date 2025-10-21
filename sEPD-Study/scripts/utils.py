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
import time
from pathlib import Path
import math
import textwrap

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

# ----------------------------

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

# ----------------------------

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

# ----------------------------

f4a = subparser.add_parser('f4a', help='Create condor submission directory.')

f4a.add_argument('-i'
                    , '--input-list', type=str
                    , required=True
                    , help='Input DST List.')

f4a.add_argument('-i2'
                    , '--dbtag', type=str
                    , default='newcdbtag'
                    , help='CDB Tag. Default: newcdbtag')

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
                    , help='Enable Official Event Plane Reco. Default: False.')

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

def create_f4a_jobs():
    """
    Create Fun4All Jobs
    """
    input_list = os.path.realpath(args.input_list)
    dbtag = args.dbtag
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

    f4a_bin = os.path.join(output_dir, os.path.basename(f4a_bin))

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input DST List: {input_list}')
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
            logger.info(f'Processing: {line}')
            file_stem = Path(line).stem

            command = f'split --lines {files_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
            run_command_and_log(command, logger, files_dir, False)

            command = f'readlink -f {files_dir}/{file_stem}* >> jobs.list'
            run_command_and_log(command, logger, output_dir, False)

    os.makedirs(f'{output_dir}/output', exist_ok=True)
    os.makedirs(f'{output_dir}/stdout', exist_ok=True)
    os.makedirs(f'{output_dir}/error', exist_ok=True)

    shutil.copy(condor_script, output_dir)

    with open(os.path.join(output_dir,'genFun4All.sub'), mode='w', encoding='utf-8') as file:
        file.write(f'executable    = {os.path.basename(condor_script)}\n')
        file.write(f'arguments     = {f4a_bin} $(input_dst) test-$(ClusterId)-$(Process).root tree-$(ClusterId)-$(Process).root {events} {dbtag} {int(do_ep)} {output_dir}/output\n')
        file.write(f'log           = {condor_log_dir}/job-$(ClusterId)-$(Process).log\n')
        file.write('output         = stdout/job-$(ClusterId)-$(Process).out\n')
        file.write('error          = error/job-$(ClusterId)-$(Process).err\n')
        file.write(f'request_memory = {condor_memory}GB\n')

    command = f'cd {output_dir} && condor_submit genFun4All.sub -queue "input_dst from jobs.list"'
    logger.info(command)

# ----------------------------

QVecCalib = subparser.add_parser('QVecCalib', help='Q-vector Calib condor jobs.')

QVecCalib.add_argument('-i'
                       , '--input-list', type=str
                       , required=True
                       , help='List of TTrees to analyze.')

QVecCalib.add_argument('-i2'
                       , '--QA-hist', type=str
                       , required=True
                       , help='QA Hist')

QVecCalib.add_argument('-i3'
                       , '--QVecAna', type=str
                       , choices=['DEFAULT', 'HALF', 'HALF1']
                       , default='DEFAULT'
                       , help='Q Vector Correction Type. Default: DEFAULT')

QVecCalib.add_argument('-p'
                       , '--files-per-hadd', type=int
                       , default=50
                       , help='Files Per hadd. Default: 50')

QVecCalib.add_argument('-p2'
                       , '--sleep-interval', type=int
                       , default=5
                       , help='Sleep Interval. Default: 5 seconds')

QVecCalib.add_argument('-f'
                       , '--macro', type=str
                       , default='macros/Q-vector-correctionv2.C'
                       , help='Q-Vector Correction Macro. Default: macros/Q-vector-correctionv2.C')

QVecCalib.add_argument('-f2'
                       , '--Qvec-bin', type=str
                       , default='bin/Q-vec-corrv2'
                       , help='Q-Vector Correction Bin. Default: bin/Q-vec-corrv2')

QVecCalib.add_argument('-o'
                       , '--output-dir', type=str
                       , default='scratch/test'
                       , help='Output Directory. Default: scratch/test')

QVecCalib.add_argument('-s'
                       , '--memory', type=float
                       , default=0.5
                       , help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')

QVecCalib.add_argument('-l'
                       , '--condor-log-dir', type=str
                       , default='/tmp/anarde/dump'
                       , help='Condor Log Directory. Default: /tmp/anarde/dump')

QVecCalib.add_argument('-f3'
                       , '--condor-script', type=str
                       , default='scripts/genQVecCalib.sh'
                       , help='Condor Script. Default: scripts/genQVecCalib.sh')

def QVecCalib_jobs():
    """
    QVecCalib condor jobs
    """
    input_list     = Path(args.input_list).resolve()
    total_jobs = int(subprocess.run(['bash','-c',f'wc -l {input_list}'], capture_output=True, encoding='utf-8', check=False).stdout.strip().split()[0])
    QA_hist        = Path(args.QA_hist).resolve()
    QVecAna        = args.QVecAna
    files_per_hadd = args.files_per_hadd
    sleep_interval = args.sleep_interval
    output_dir     = Path(args.output_dir).resolve()
    QVecCalib_macro = Path(args.macro).resolve()
    QVecCalib_bin   = Path(args.Qvec_bin).resolve()
    log_file       = output_dir / 'log.txt'
    condor_memory  = args.memory
    condor_script  = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()

    # Create Dirs
    output_dir.mkdir(parents=True, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that files exists
    for f in [input_list, QA_hist, condor_script]:
        if not f.is_file():
            logger.critical(f'File: {f} does not exist!')
            sys.exit()

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input File: {input_list}')
    logger.info(f'Jobs: {total_jobs}')
    logger.info(f'Files per hadd: {files_per_hadd}')
    logger.info(f'Sleep Interval: {sleep_interval}')
    logger.info(f'QA Hist: {QA_hist}')
    logger.info(f'Q Vec Ana: {QVecAna}')
    logger.info(f'Q Vec Calib Macro: {QVecCalib_macro}')
    logger.info(f'Q Vec Calib Bin: {QVecCalib_bin}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    if condor_log_dir.is_dir():
        shutil.rmtree(condor_log_dir)

    # Setup Condor Log Dir
    condor_log_dir.mkdir(parents=True, exist_ok=True)

    # list of subdirectories to create
    subdirectories = ['stdout', 'error', 'output']
    calib_types = ['ComputeRecentering', 'ApplyRecentering', 'ApplyFlattening']
    calib_hists = ['test-recentering.root', 'test-flattening.root', 'test-QVecCalib.root']

    # Loop through the list and create each one
    for calib in calib_types:
        for subdir in subdirectories:
            (output_dir / calib / subdir).mkdir(parents=True, exist_ok=True)

    input_list = shutil.copy(input_list, output_dir)
    shutil.copy(QVecCalib_macro, output_dir)
    QVecCalib_bin = shutil.copy(QVecCalib_bin, output_dir)
    QA_hist = shutil.copy(QA_hist, output_dir)
    condor_script = shutil.copy(condor_script, output_dir)

    for idx, (calib, hist) in enumerate(zip(calib_types, calib_hists)):
        calib_dir = output_dir / calib
        hadd_hist = calib_dir / hist
        calib_hist = output_dir / calib_types[idx-1] / calib_hists[idx-1] if idx != 0 else 'none'

        submit_file_content = textwrap.dedent(f"""\
            executable     = {condor_script}
            arguments      = {QVecCalib_bin} $(input_tree) {QA_hist} {calib_hist} {QVecAna} {calib} {calib_dir}/output
            log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
            output         = stdout/job-$(ClusterId)-$(Process).out
            error          = error/job-$(ClusterId)-$(Process).err
            request_memory = {condor_memory}GB
        """)

        with open(calib_dir / 'genQVecCalib.sub', mode='w', encoding='utf-8') as file:
            file.write(submit_file_content)

        command = f'condor_submit genQVecCalib.sub -queue "input_tree from {input_list}"'
        run_command_and_log(command, logger, calib_dir, False)

        while True:
            jobs = int(subprocess.run(['bash','-c','ls output | wc -l'], capture_output=True, encoding='utf-8', cwd=calib_dir, check=False).stdout.strip())

            if jobs == total_jobs:
                logger.info(f'Stage: {calib}, Jobs Finished at {datetime.datetime.now()}')
                break

            logger.info(f'Checking Condor Output Status: {datetime.datetime.now()}, Jobs: {jobs} out of {total_jobs}, {jobs * 100 / total_jobs:0.2f} %')
            time.sleep(sleep_interval)

        command = f'hadd -n {files_per_hadd+1} {hadd_hist} output/*'
        run_command_and_log(command, logger, calib_dir)

# ----------------------------

jetAna = subparser.add_parser('jetAna', help='jetAna condor jobs.')

jetAna.add_argument('-i'
                    , '--input-list', type=str
                    , required=True
                    , help='List of TTrees to analyze.')

jetAna.add_argument('-i2'
                    , '--calib', type=str
                    , required=True
                    , help='Q Vector Calibrations')

jetAna.add_argument('-f'
                    , '--jetAna-macro', type=str
                    , default='macros/Jet-Ana.C'
                    , help='Jet-Ana Macro. Default: macros/Jet-Ana.C')

jetAna.add_argument('-f2'
                    , '--jetAna-bin', type=str
                    , default='bin/Jet-Ana'
                    , help='Jet-Ana Bin. Default: bin/Jet-Ana')

jetAna.add_argument('-o'
                    , '--output-dir', type=str
                    , default='scratch/test'
                    , help='Output Directory. Default: scratch/test')

jetAna.add_argument('-s'
                    , '--memory', type=float
                    , default=0.5
                    , help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')

jetAna.add_argument('-l'
                    , '--condor-log-dir', type=str
                    , default='/tmp/anarde/dump'
                    , help='Condor Log Directory. Default: /tmp/anarde/dump')

jetAna.add_argument('-f3'
                    , '--condor-script', type=str
                    , default='scripts/genJetAna.sh'
                    , help='Condor Script. Default: scripts/genJetAna.sh')

def jetAna_jobs():
    """
    jetAna condor jobs
    """
    input_list     = Path(args.input_list).resolve()
    calib_file     = Path(args.calib).resolve()
    output_dir     = Path(args.output_dir).resolve()
    jetAna_macro   = Path(args.jetAna_macro).resolve()
    jetAna_bin     = Path(args.jetAna_bin).resolve()
    log_file       = output_dir / 'log.txt'
    condor_memory  = args.memory
    condor_script  = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()

    # Create Dirs
    output_dir.mkdir(parents=True, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that files exists
    for f in [input_list, calib_file, condor_script]:
        if not f.is_file():
            logger.critical(f'File: {f} does not exist!')
            sys.exit()

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input File: {input_list}')
    logger.info(f'Calib: {calib_file}')
    logger.info(f'Jet Ana Macro: {jetAna_macro}')
    logger.info(f'Jet Ana Bin: {jetAna_bin}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    if condor_log_dir.is_dir():
        shutil.rmtree(condor_log_dir)

    # Setup Condor Log Dir
    condor_log_dir.mkdir(parents=True, exist_ok=True)

    # list of subdirectories to create
    subdirectories = ['stdout', 'error', 'output']

    # Loop through the list and create each one
    for subdir in subdirectories:
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)

    shutil.copy(input_list, output_dir)
    shutil.copy(jetAna_macro, output_dir)
    jetAna_bin = shutil.copy(jetAna_bin, output_dir)
    calib_file = shutil.copy(calib_file, output_dir)
    shutil.copy(condor_script, output_dir)

    submit_file_content = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = {jetAna_bin} $(input_tree) {calib_file} {output_dir}/output
        log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
        output         = stdout/job-$(ClusterId)-$(Process).out
        error          = error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'genJetAna.sub', mode='w', encoding='utf-8') as file:
        file.write(submit_file_content)

    command = f'cd {output_dir} && condor_submit genJetAna.sub -queue "input_tree from {input_list.name}"'
    logger.info(command)

# ----------------------------

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
    input_dir     = Path(args.input_dir).resolve()
    output_dir     = Path(args.output_dir).resolve()
    hadd_max       = args.hadd_max
    log_file       = output_dir / 'log.txt'
    condor_memory  = args.memory
    condor_script  = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()

    # Create Dirs
    output_dir.mkdir(parents=True, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that paths exists
    if not input_dir.is_dir():
        logger.critical(f'Dir: {input_dir} does not exist!')
        sys.exit()

    if not condor_script.is_file():
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

    if condor_log_dir.is_dir():
        shutil.rmtree(condor_log_dir)

    # Setup Condor Log Dir
    condor_log_dir.mkdir(parents=True, exist_ok=True)

    # list of subdirectories to create
    subdirectories = ['stdout', 'error', 'output']

    # Loop through the list and create each one
    for subdir in subdirectories:
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)

    shutil.copy(condor_script, output_dir)

    command = f'readlink -f {input_dir}/* > jobs.list'
    run_command_and_log(command, logger, output_dir, False)

    submit_file_content = textwrap.dedent(f"""\
        executable     = {os.path.basename(condor_script)}
        arguments      = $(input_dir) {hadd_max} {output_dir}/output
        log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
        output         = stdout/job-$(ClusterId)-$(Process).out
        error          = error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'genHadd.sub', mode='w', encoding='utf-8') as file:
        file.write(submit_file_content)

    command = f'cd {output_dir} && condor_submit genHadd.sub -queue "input_dir from jobs.list"'
    logger.info(command)

# ----------------------------

data = subparser.add_parser('data', help='Update file lists.')

data.add_argument('-t'
                    , '--tag', type=str
                    , default='new_newcdbtag_v008'
                    , help='Tag. Default: new_newcdbtag_v008')

data.add_argument('-o'
                    , '--output-dir', type=str
                    , default='files/run3auau'
                    , help='Output Directory. Default: files/run3auau')

def get_line_count(file_path: str) -> int:
    """
    Checks if a file exists at the given path and, if so, returns the number of lines.

    Args:
        file_path: A string representing the path to the file.

    Returns:
        The number of lines in the file if it exists.
        Returns 0 if the file does not exist.
    """
    path = Path(file_path)

    if not path.is_file():
        print(f"Error: File '{file_path}' not found.")
        return 0

    # Efficiently count lines using a generator expression
    with open(path, 'r', encoding='utf-8') as f:
        line_count = sum(1 for line in f)

    return line_count

def find_file_with_most_lines(directory: str):
    """
    Identifies the file with the most lines in a given directory.

    Args:
        directory: The path to the directory as a string.

    Returns:
        A tuple containing the filename and line count, or (None, 0) if no files were found.
    """
    # Create a Path object for the directory
    dir_path = Path(directory)

    if not dir_path.is_dir():
        print(f"Error: Directory '{directory}' not found.")
        return None, 0

    max_lines = 0
    file_with_most_lines = None

    # Use iterdir() to get an iterator over the directory contents
    for path_object in dir_path.iterdir():
        # Check if the path points to a file
        if path_object.is_file():
            try:
                # Open the file using the Path object and count lines efficiently
                with path_object.open('r', encoding='utf-8', errors='ignore') as f:
                    line_count = sum(1 for line in f)

                # Update max_lines and the filename if a new maximum is found
                if line_count > max_lines:
                    max_lines = line_count
                    file_with_most_lines = path_object.name

            except IOError as e:
                print(f"Could not read file {path_object.name}: {e}")

    return file_with_most_lines, max_lines

def count_total_lines(directory_path: str) -> int:
    """
    Counts the total number of lines across all files in a given directory.

    Args:
        directory_path: The path to the directory as a string.

    Returns:
        The total number of lines, or 0 if the directory does not exist.
    """
    path_obj = Path(directory_path)

    if not path_obj.is_dir():
        print(f"Error: Directory '{directory_path}' not found.")
        return 0

    total_lines = 0

    # Iterate through all items in the directory
    for item in path_obj.iterdir():
        # Check if the item is a regular file
        if item.is_file():
            try:
                # Open the file using the Path object and count lines efficiently
                with item.open('r', encoding='utf-8', errors='ignore') as f:
                    total_lines += sum(1 for line in f)
            except IOError as e:
                print(f"Could not read file {item.name}: {e}")

    return total_lines

def setup_data():
    """
    Setup input data lists
    """
    tag        = args.tag
    output_dir = Path(args.output_dir).resolve()
    log_file = output_dir / 'log.txt'

    output_dir.mkdir(parents=True, exist_ok=True)

    if log_file.is_file():
        try:
            log_file.unlink()
            print(f"File '{log_file}' successfully removed.")
        except OSError as e:
            # Handle potential permission errors
            print(f"Error: {e.strerror}")

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'tag: {tag}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info('#'*40)

    run3auau_time_min_5 = output_dir / 'run3auau-time-min-5.list'
    current_runs = get_line_count(run3auau_time_min_5)

    # Get list of all runnumbers
    # Type = physics
    # Duration >= 5 minutes
    command = 'psql -h sphnxdaqdbreplica daq -c "select runnumber from run where runtype = \'physics\' and runnumber >= 66457 and ertimestamp-brtimestamp > interval \'5 minutes\' order by runnumber;" -At > run3auau-time-min-5.list'
    run_command_and_log(command, logger, output_dir, False)

    new_runs = get_line_count(run3auau_time_min_5)
    diff_runs_frac = (new_runs-current_runs)*100/current_runs if current_runs != 0 else 0

    logger.info(f'Runs: Before: {current_runs}, After: {new_runs}, Change: {diff_runs_frac:.2f} %')
    logger.info(f'Path: {run3auau_time_min_5}')

    # Generate list of DST_CALOFITTING from the above run list
    dst_dir = output_dir / f'run3auau-{tag}'

    if dst_dir.is_dir():
        try:
            current_segments = count_total_lines(dst_dir)
            shutil.rmtree(dst_dir)
            logger.info(f"Directory '{dst_dir}' successfully deleted.")
        except OSError as e:
            logger.critical(f"Error: {dst_dir} : {e.strerror}")
            sys.exit()

    dst_dir.mkdir(parents=True, exist_ok=True)

    command = f'CreateDstList.pl --tag {tag} --list ../run3auau-time-min-5.list DST_CALOFITTING'
    run_command_and_log(command, logger, dst_dir)

    new_segments = count_total_lines(dst_dir)
    diff_segments_frac = (new_segments-current_segments)*100/current_segments if current_segments != 0 else 0
    logger.info(f'Total Segments: Before: {current_segments}, After: {new_segments}, Change: {diff_segments_frac:.2f} %')

    # Write list of runs with DST_CALOFITTING to file
    run3auau_dsts_time_min_5 = output_dir / f'run3auau-{tag}-time-min-5.list'

    current_runs_dsts = get_line_count(run3auau_dsts_time_min_5)

    command = f'ls run3auau-{tag} | cut -d- -f2 | cut -d. -f1 | awk \'{{x=$0+0;print x}}\' | sort | uniq > run3auau-{tag}-time-min-5.list'
    run_command_and_log(command, logger, output_dir, False)

    new_runs_dsts = get_line_count(run3auau_dsts_time_min_5)
    diff_runs_frac = (new_runs_dsts-current_runs_dsts)*100/current_runs_dsts if current_runs_dsts != 0 else 0

    logger.info(f'Runs: Before: {current_runs_dsts}, After: {new_runs_dsts}, Change: {diff_runs_frac:.2f} %')
    logger.info(f'Path: {run3auau_dsts_time_min_5}')

    # Compile the List of Lists of DST_CALOFITTING
    command = f'readlink -f run3auau-{tag}/* > run3auau-{tag}.list'
    run_command_and_log(command, logger, output_dir, False)

    biggest_run, segments = find_file_with_most_lines(dst_dir)
    logger.info(f'Biggest Run: {biggest_run}, Segments: {segments}')

    # Compile the List of Lists of DST_CALOFITTING for testing
    command = f'readlink -f {dst_dir / biggest_run} > run3auau-{tag}-test.list'
    run_command_and_log(command, logger, output_dir, False)

args = parser.parse_args()

if __name__ == "__main__":
    if args.command == 'f4a':
        create_f4a_jobs()

    if args.command == 'QVecCalib':
        QVecCalib_jobs()

    if args.command == 'jetAna':
        jetAna_jobs()

    if args.command == 'hadd':
        hadd_jobs()

    if args.command == 'data':
        setup_data()
