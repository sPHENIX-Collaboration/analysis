import time
import shutil
import textwrap
from pathlib import Path
from condor_utils.core.logging import setup_logging
from condor_utils.core.helpers import run_command_and_log, chunk_list
import logging
import datetime
import sys

def hadd_jobs(args):
    input_dir      = Path(args.input_dir).resolve()
    output_dir     = Path(args.output_dir).resolve()
    files_per_job  = args.hadd_max
    log_file       = output_dir / 'log.txt'
    condor_memory  = args.memory
    condor_script  = Path(args.condor_script).resolve()
    condor_log_dir = Path(args.condor_log_dir).resolve()

    output_dir.mkdir(parents=True, exist_ok=True)
    partial_dir = output_dir / 'partial'
    partial_dir.mkdir(parents=True, exist_ok=True)

    logger = setup_logging(log_file, logging.DEBUG)

    if not input_dir.is_dir():
        logger.critical(f'Dir: {input_dir} does not exist!')
        sys.exit(1)

    if not condor_script.is_file():
        logger.critical(f'File: {condor_script} does not exist!')
        sys.exit(1)

    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input Dir: {input_dir}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Files Per Job: {files_per_job}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    for subdir in ['stdout', 'error', 'output']:
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)
        (partial_dir / subdir).mkdir(parents=True, exist_ok=True)

    condor_log_dir.mkdir(parents=True, exist_ok=True)

    logger.info("Starting STAGE 1: Partial Merging")

    run_dirs = [d for d in input_dir.iterdir() if d.is_dir()]
    job_counter = 0
    stage1_jobs_list = output_dir / 'jobs_stage1.list'

    logger.info(f'Run Dirs: {run_dirs}')

    with open(stage1_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name
            root_files = list(run_dir.glob('*.root'))
            if not root_files:
                root_files = list(run_dir.glob('hist/*.root'))

            if not root_files:
                logger.warning(f"No root files found for run {run_name}, skipping.")
                continue

            chunks = list(chunk_list(root_files, files_per_job))
            for i, chunk in enumerate(chunks):
                chunk_list_filename = output_dir / 'files' / f'{run_name}_part_{i}.list'
                chunk_list_filename.parent.mkdir(exist_ok=True)

                with open(chunk_list_filename, mode='w', encoding='utf-8') as f_chunk:
                    for rfile in chunk:
                        f_chunk.write(f"{rfile}\n")

                partial_output_name = f"partial-{run_name}-{i}.root"
                f_list.write(f"{chunk_list_filename},{partial_output_name},{partial_dir}/output\n")
                job_counter += 1

    shutil.copy(condor_script, output_dir)

    submit_content_s1 = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = $(list_file) $(out_name) $(out_dir)
        log            = {condor_log_dir}/stage1-$(ClusterId)-$(Process).log
        output         = {partial_dir}/stdout/job-$(ClusterId)-$(Process).out
        error          = {partial_dir}/error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    (output_dir / 'stage1.sub').write_text(submit_content_s1)

    logger.info(f"Submitting {job_counter} partial merge jobs...")
    command = f'cd {output_dir} && condor_submit stage1.sub -queue "list_file,out_name,out_dir from jobs_stage1.list"'
    run_command_and_log(command, logger, output_dir)

    while True:
        finished_files = len(list((partial_dir / 'output').glob('*.root')))
        if finished_files >= job_counter:
            logger.info(f"Stage 1 Complete. {finished_files}/{job_counter} files created.")
            break

        logger.info(f"Waiting for Stage 1... {finished_files}/{job_counter} done.")
        time.sleep(15)

    logger.info("Starting STAGE 2: Final Merging")

    stage2_jobs_list = output_dir / 'jobs_stage2.list'
    stage2_counter = 0

    with open(stage2_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name
            partial_files = list((partial_dir / 'output').glob(f'partial-{run_name}-*.root'))

            if not partial_files:
                continue

            if len(partial_files) == 1:
                src = partial_files[0]
                dst = output_dir / 'output' / f"{run_name}.root"
                logger.info(f"Run {run_name} has only 1 partial file. Moving directly to final.")
                shutil.move(str(src), str(dst))
                continue

            final_list_filename = output_dir / 'files' / f'{run_name}_final.list'
            with open(final_list_filename, mode='w', encoding='utf-8') as f_final:
                for pfile in partial_files:
                    f_final.write(f"{pfile}\n")

            final_output_name = f"{run_name}.root"
            f_list.write(f"{final_list_filename},{final_output_name},{output_dir}/output\n")
            stage2_counter += 1

    submit_content_s2 = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = $(list_file) $(out_name) $(out_dir)
        log            = {condor_log_dir}/stage2-$(ClusterId)-$(Process).log
        output         = {output_dir}/stdout/final-$(ClusterId)-$(Process).out
        error          = {output_dir}/error/final-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    (output_dir / 'stage2.sub').write_text(submit_content_s2)

    logger.info(f"Submitting {stage2_counter} final merge jobs...")
    command = f'cd {output_dir} && condor_submit stage2.sub -queue "list_file,out_name,out_dir from jobs_stage2.list"'
    run_command_and_log(command, logger, output_dir)
    logger.info("All jobs submitted.")

def setup_hadd_subparsers(subparsers):
    hadd_parser = subparsers.add_parser('hadd', help='hadd condor jobs.')
    hadd_parser.add_argument('-i', '--input-dir', type=str, required=True, help='Directory of runs to combine output.')
    hadd_parser.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    hadd_parser.add_argument('-n', '--hadd-max', type=int, default=10, help='Hadd Max at once. Default: 10')
    hadd_parser.add_argument('-s', '--memory', type=float, default=1.0, help='Memory (units of GB). Default: 1.0 GB.')
    hadd_parser.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    hadd_parser.add_argument('-f', '--condor-script', type=str, default='scripts/genHadd.sh', help='Condor Script.')
    hadd_parser.set_defaults(func=hadd_jobs)
