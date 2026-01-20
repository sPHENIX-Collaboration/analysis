#!/usr/bin/env python3
"""
This module automates the Fun4All Condor Procedure
"""

import argparse
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
                    , '--calib', type=str
                    , required=True
                    , help='Q Vector Calibrations.')

f4a.add_argument('-i3'
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

f4a.add_argument('-s'
                    , '--memory', type=float
                    , default=4
                    , help='Memory (units of GB) to request per condor submission. Default: 4 GB.')

f4a.add_argument('-l'
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
    input_list = Path(args.input_list).resolve()
    calib_list = Path(args.calib).resolve()
    dbtag = args.dbtag
    events = args.events
    output_dir = Path(args.output_dir).resolve()
    log_file  = output_dir / 'log.txt'
    f4a_macro = Path(args.f4a_macro).resolve()
    f4a_bin = Path(args.f4a_bin).resolve()
    src_dir = Path(args.src_dir).resolve()
    condor_memory = args.memory
    condor_script = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()
    common_errors = Path(args.common_errors).resolve()

    # Create Dirs
    output_dir.mkdir(parents=True, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that files exists
    for f in [input_list, calib_list, condor_script, f4a_bin, f4a_macro, common_errors]:
        if not f.is_file():
            logger.critical(f'File: {f} does not exist!')
            sys.exit()

    # Ensure that directory exists
    if not src_dir.is_dir():
        logger.critical(f'Directory: {src_dir} does not exist!')
        sys.exit()

    # Compute the total number of DSTs in the list files
    total_files = int(subprocess.run(['bash','-c',f'cat {input_list} | xargs -I {{}} sh -c \'test -f "{{}}" && wc -l "{{}}"\' | awk \'{{sum += $1}} END {{print sum}}\''], capture_output=True, encoding='utf-8', check=False).stdout.strip())

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input DST List: {input_list}')
    logger.info(f'Calib List: {calib_list}')
    logger.info(f'Total DSTs: {total_files}')
    logger.info(f'Events to process per job: {events if events != 0 else "All"}')
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

    shutil.rmtree(condor_log_dir, ignore_errors=True)

    # Setup Condor Log Dir
    condor_log_dir.mkdir(parents=True, exist_ok=True)

    # Copy necessary files to the output directory
    shutil.copy(input_list, output_dir)
    shutil.copy(calib_list, output_dir)
    shutil.copy(f4a_macro, output_dir)
    f4a_bin = shutil.copy(f4a_bin, output_dir)
    shutil.copy(common_errors, output_dir)
    shutil.copytree(src_dir, output_dir / 'src', dirs_exist_ok=True)

    CONDOR_SUBMISSION_LIMIT = 20000

    files_per_job = math.ceil(total_files / CONDOR_SUBMISSION_LIMIT)
    logger.info(f'Files Per Job: {files_per_job}')

    files_dir = output_dir / 'files'
    files_dir.mkdir(parents=True, exist_ok=True)

    calib_map = {}
    # Calib List
    with open(calib_list, mode='r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            run = Path(line.split(',')[0]).parts[-2]
            logger.info(f'Processing: {line}, run: {run}')
            calib_map[run] = line

    jobs_file = output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    jobs_temp_file = output_dir / 'jobs-temp.list'
    jobs_temp_file.unlink(missing_ok=True)

    with open(input_list, mode='r', encoding='utf-8') as file:
        for line in file:
            line = line.strip()
            logger.info(f'Processing: {line}')
            file_stem = Path(line).stem

            command = f'split --lines {files_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
            run_command_and_log(command, logger, files_dir, False)

            command = f'readlink -f {files_dir}/{file_stem}* >> {jobs_temp_file.name}'
            run_command_and_log(command, logger, output_dir, False)

    with open(jobs_temp_file, mode='r', encoding='utf-8') as file_in, \
         open(jobs_file, mode='w', encoding='utf-8') as file_out:
        for line in file_in:
            line = Path(line.strip()).resolve()
            run = line.stem.split('-')[1].lstrip('0')
            if run in calib_map:
                file_out.write(f'{line},{calib_map[run]}\n')
            else:
                file_out.write(f'{line},none\n')

    # Remove temp file
    jobs_temp_file.unlink(missing_ok=True)

    # list of subdirectories to create
    subdirectories = ['stdout', 'error', 'output']

    # Loop through the list and create each one
    for subdir in subdirectories:
        shutil.rmtree(output_dir / subdir, ignore_errors=True)
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)

    shutil.copy(condor_script, output_dir)

    submit_file_content = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = {f4a_bin} $(input_dst) $(input_calib) $(input_sEPD_BadTowers) test-$(ClusterId)-$(Process).root tree-$(ClusterId)-$(Process).root {events} {dbtag} {output_dir}/output
        log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
        output         = stdout/job-$(ClusterId)-$(Process).out
        error          = error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'genFun4All.sub', mode='w', encoding='utf-8') as file:
        file.write(submit_file_content)

    command = f'cd {output_dir} && condor_submit genFun4All.sub -queue "input_dst,input_calib,input_sEPD_BadTowers from jobs.list"'
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
                logger.info(f'Stage: {calib}, Jobs Finished')
                break

            logger.info(f'Checking Condor Output Status: Jobs: {jobs} out of {total_jobs}, {jobs * 100 / total_jobs:0.2f} %')
            time.sleep(sleep_interval)

        command = f'hadd -n {files_per_hadd+1} {hadd_hist} output/*'
        run_command_and_log(command, logger, calib_dir)

# ----------------------------

jetAna = subparser.add_parser('jetAna', help='jetAna condor jobs.')

jetAna.add_argument('-i'
                    , '--input-list', type=str
                    , required=True
                    , help='List of TTrees to analyze.')

jetAna.add_argument('-j'
                    , '--jet-pt-min', type=float
                    , default=7
                    , help='Minimum Jet pT. Default: 7 [GeV]')

jetAna.add_argument('-j2'
                    , '--jet-eta-max', type=float
                    , default=0.9
                    , help='Maximum Jet eta. Default: 0.9')

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
                    , default=3
                    , help='Memory (units of GB) to request per condor submission. Default: 3 GB.')

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
    jet_pt_min     = args.jet_pt_min
    jet_eta_max    = args.jet_eta_max
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
    for f in [input_list, condor_script, jetAna_bin]:
        if not f.is_file():
            logger.critical(f'File: {f} does not exist!')
            sys.exit()

    # Print Logs
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input List: {input_list}')
    logger.info(f'Jet pT Min: {jet_pt_min} GeV')
    logger.info(f'Jet eta Max: {jet_eta_max}')
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

    shutil.copy(input_list, output_dir / 'jobs.list')
    shutil.copy(jetAna_macro, output_dir)
    jetAna_bin = shutil.copy(jetAna_bin, output_dir)
    shutil.copy(condor_script, output_dir)

    submit_file_content = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = {jetAna_bin} $(input_tree) {jet_pt_min} {jet_eta_max} {output_dir}/output
        log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
        output         = stdout/job-$(ClusterId)-$(Process).out
        error          = error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'genJetAna.sub', mode='w', encoding='utf-8') as file:
        file.write(submit_file_content)

    command = f'cd {output_dir} && condor_submit genJetAna.sub -queue "input_tree from jobs.list"'
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
                    , default=10
                    , help='Hadd Max at once. Default: 10')

hadd.add_argument('-s'
                    , '--memory', type=float
                    , default=3
                    , help='Memory (units of GB) to request per condor submission. Default: 3 GB.')

hadd.add_argument('-l'
                    , '--condor-log-dir', type=str
                    , default='/tmp/anarde/dump'
                    , help='Condor Log Directory. Default: /tmp/anarde/dump')

hadd.add_argument('-f'
                    , '--condor-script', type=str
                    , default='scripts/genHadd.sh'
                    , help='Condor Script. Default: scripts/genHadd.sh')

# Helper to chunk list into smaller lists
def chunk_list(input_list, size):
    """Yield successive size-sized chunks from input_list."""
    for i in range(0, len(input_list), size):
        yield input_list[i:i + size]

def hadd_jobs():
    """
    Two-Stage Hierarchical hadd
    """
    input_dir      = Path(args.input_dir).resolve()
    output_dir     = Path(args.output_dir).resolve()
    files_per_job  = args.hadd_max
    log_file       = output_dir / 'log.txt'
    condor_memory  = args.memory
    condor_script  = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()

    # Create Dirs
    output_dir.mkdir(parents=True, exist_ok=True)
    partial_dir = output_dir / 'partial' # Intermediate files go here
    partial_dir.mkdir(parents=True, exist_ok=True)

    # Initialize the logger
    logger = setup_logging(log_file, logging.DEBUG)

    # Ensure that paths exists
    if not input_dir.is_dir():
        logger.critical(f'Dir: {input_dir} does not exist!')
        sys.exit()

    if not condor_script.is_file():
        logger.critical(f'File: {condor_script} does not exist!')
        sys.exit()

    # Print Input Args
    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input Dir: {input_dir}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Files Per Job: {files_per_job}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    # Standard subdirectories
    for subdir in ['stdout', 'error', 'output']:
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)
        (partial_dir / subdir).mkdir(parents=True, exist_ok=True)

    if condor_log_dir.is_dir():
        shutil.rmtree(condor_log_dir)

    # Setup Condor Log Dir
    condor_log_dir.mkdir(parents=True, exist_ok=True)

    # ---------------------------------------------------------
    # STAGE 1: SUBMIT PARTIAL MERGES
    # ---------------------------------------------------------
    logger.info("Starting STAGE 1: Partial Merging")

    # Find all run directories
    run_dirs = [d for d in input_dir.iterdir() if d.is_dir()]

    job_counter = 0
    stage1_jobs_list = output_dir / 'jobs_stage1.list'

    logger.info(f'Run Dirs: {run_dirs}')

    with open(stage1_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name

            # Find all ROOT files in this run directory
            # Tries both structures you had in your original script
            root_files = list(run_dir.glob('*.root'))
            if not root_files:
                root_files = list(run_dir.glob('hist/*.root'))

            if not root_files:
                logger.warning(f"No root files found for run {run_name}, skipping.")
                continue

            # Split into chunks (e.g., 400 files -> 40 chunks of 10)
            chunks = list(chunk_list(root_files, files_per_job))

            for i, chunk in enumerate(chunks):
                # Create a list file for this specific job
                chunk_list_filename = output_dir / 'files' / f'{run_name}_part_{i}.list'
                chunk_list_filename.parent.mkdir(exist_ok=True)

                with open(chunk_list_filename, mode='w', encoding='utf-8') as f_chunk:
                    for rfile in chunk:
                        f_chunk.write(f"{rfile}\n")

                # Output filename: partial-68099-0.root
                partial_output_name = f"partial-{run_name}-{i}.root"

                # Write to Condor Job List:
                # input_list_file, output_filename, output_destination
                f_list.write(f"{chunk_list_filename},{partial_output_name},{partial_dir}/output\n")
                job_counter += 1

    shutil.copy(condor_script, output_dir)

    # Generate Submit File for Stage 1
    submit_content_s1 = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = $(list_file) $(out_name) $(out_dir)
        log            = {condor_log_dir}/stage1-$(ClusterId)-$(Process).log
        output         = {partial_dir}/stdout/job-$(ClusterId)-$(Process).out
        error          = {partial_dir}/error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'stage1.sub', mode='w', encoding='utf-8') as f:
        f.write(submit_content_s1)

    # Submit Stage 1
    logger.info(f"Submitting {job_counter} partial merge jobs...")
    command = f'cd {output_dir} && condor_submit stage1.sub -queue "list_file,out_name,out_dir from jobs_stage1.list"'
    run_command_and_log(command, logger, output_dir)

    # Monitor Stage 1 (Wait Loop)
    while True:
        # Simple check: count files in partial/output vs expected jobs
        # Ideally query condor_q, but file counting is robust enough for this scale
        finished_files = len(list((partial_dir / 'output').glob('*.root')))

        if finished_files >= job_counter:
            logger.info(f"Stage 1 Complete. {finished_files}/{job_counter} files created.")
            break

        logger.info(f"Waiting for Stage 1... {finished_files}/{job_counter} done.")
        time.sleep(15) # Check every 15 seconds

    # ---------------------------------------------------------
    # STAGE 2: FINAL MERGE
    # ---------------------------------------------------------
    logger.info("Starting STAGE 2: Final Merging")

    stage2_jobs_list = output_dir / 'jobs_stage2.list'
    stage2_counter = 0

    with open(stage2_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name

            # Find the partial files we just created for this run
            # They match pattern: partial-RUNNAME-*.root in partial_dir/output
            partial_files = list((partial_dir / 'output').glob(f'partial-{run_name}-*.root'))

            if not partial_files:
                continue

            # --- CONTINGENCY FOR FEW FILES ---
            if len(partial_files) == 1:
                # If there is only 1 partial file, we don't need a Stage 2 Condor job.
                # Just move it to the final directory and rename it.
                src = partial_files[0]
                dst = output_dir / 'output' / f"{run_name}.root"

                logger.info(f"Run {run_name} has only 1 partial file. Moving directly to final.")
                shutil.move(str(src), str(dst))
                continue # Skip the rest of the loop for this run
            # ---------------------------------

            # Create list file for final merge (only if > 1 partial file)
            final_list_filename = output_dir / 'files' / f'{run_name}_final.list'
            with open(final_list_filename, mode='w', encoding='utf-8') as f_final:
                for pfile in partial_files:
                    f_final.write(f"{pfile}\n")

            final_output_name = f"{run_name}.root"

            # Write to Condor Job List
            f_list.write(f"{final_list_filename},{final_output_name},{output_dir}/output\n")
            stage2_counter += 1

    # Generate Submit File for Stage 2
    submit_content_s2 = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = $(list_file) $(out_name) $(out_dir)
        log            = {condor_log_dir}/stage2-$(ClusterId)-$(Process).log
        output         = {output_dir}/stdout/final-$(ClusterId)-$(Process).out
        error          = {output_dir}/error/final-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    with open(output_dir / 'stage2.sub', mode='w', encoding='utf-8') as f:
        f.write(submit_content_s2)

    # Submit Stage 2
    logger.info(f"Submitting {stage2_counter} final merge jobs...")
    command = f'cd {output_dir} && condor_submit stage2.sub -queue "list_file,out_name,out_dir from jobs_stage2.list"'
    run_command_and_log(command, logger, output_dir)
    logger.info("All jobs submitted.")

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
