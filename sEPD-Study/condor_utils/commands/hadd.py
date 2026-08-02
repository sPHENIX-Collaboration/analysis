import time
import shutil
import sys
from pathlib import Path

from condor_utils.core.helpers import chunk_list
from condor_utils.core.manager import CondorJobManager
from condor_utils.cli import get_common_parser

def hadd_jobs(args):
    manager = CondorJobManager(args, job_name="hadd")
    manager.add_file_to_check(args.condor_script)
    manager.validate_paths()

    input_dir = Path(args.input_dir).resolve()
    if not input_dir.is_dir():
        manager.logger.critical(f'Dir: {input_dir} does not exist!')
        sys.exit(1)

    files_per_job = args.hadd_max

    manager.log_initialization({
        'Input Dir': input_dir,
        'Files Per Job': files_per_job,
    })

    manager.prepare_directories()

    partial_dir = manager.output_dir / 'partial'
    for subdir in ['stdout', 'error', 'output']:
        (partial_dir / subdir).mkdir(parents=True, exist_ok=True)

    manager.copy_dependencies()

    manager.logger.info("Starting STAGE 1: Partial Merging")

    run_dirs = [d for d in input_dir.iterdir() if d.is_dir()]
    job_counter = 0
    stage1_jobs_list = manager.output_dir / 'jobs_stage1.list'

    manager.logger.info(f'Run Dirs: {run_dirs}')

    with open(stage1_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name
            root_files = list(run_dir.glob('*.root'))
            if not root_files:
                root_files = list(run_dir.glob('hist/*.root'))

            if not root_files:
                manager.logger.warning(f"No root files found for run {run_name}, skipping.")
                continue

            if len(root_files) == 1:
                dst = manager.output_dir / 'output' / f"{run_name}.root"
                manager.logger.info(f"Run {run_name} has only 1 root file. Copying directly to final output.")
                shutil.copy(str(root_files[0]), str(dst))
                continue

            chunks = list(chunk_list(root_files, files_per_job))
            for i, chunk in enumerate(chunks):
                chunk_list_filename = manager.output_dir / 'files' / f'{run_name}_part_{i}.list'
                chunk_list_filename.parent.mkdir(exist_ok=True)

                with open(chunk_list_filename, mode='w', encoding='utf-8') as f_chunk:
                    for rfile in chunk:
                        f_chunk.write(f"{rfile}\n")

                partial_output_name = f"partial-{run_name}-{i}.root"
                f_list.write(f"{chunk_list_filename},{partial_output_name},{partial_dir}/output\n")
                job_counter += 1

    arguments = "$(list_file) $(out_name) $(out_dir)"
    manager.write_submit_file(arguments=arguments, sub_file_name="stage1.sub",
                              stdout_dir="partial/stdout", error_dir="partial/error", log_prefix="stage1")

    if job_counter > 0:
        manager.logger.info(f"Submitting {job_counter} partial merge jobs...")
        manager.finalize_submission(queue_arg="list_file,out_name,out_dir from jobs_stage1.list", sub_file_name="stage1.sub")

        while True:
            finished_files = len(list((partial_dir / 'output').glob('*.root')))
            if finished_files >= job_counter:
                manager.logger.info(f"Stage 1 Complete. {finished_files}/{job_counter} files created.")
                break

            manager.logger.info(f"Waiting for Stage 1... {finished_files}/{job_counter} done.")
            time.sleep(15)
    else:
        manager.logger.info("No partial merge jobs to submit for Stage 1.")

    manager.logger.info("Starting STAGE 2: Final Merging")

    stage2_jobs_list = manager.output_dir / 'jobs_stage2.list'
    stage2_counter = 0

    with open(stage2_jobs_list, mode='w', encoding='utf-8') as f_list:
        for run_dir in run_dirs:
            run_name = run_dir.name
            partial_files = list((partial_dir / 'output').glob(f'partial-{run_name}-*.root'))

            if not partial_files:
                continue

            if len(partial_files) == 1:
                src = partial_files[0]
                dst = manager.output_dir / 'output' / f"{run_name}.root"
                manager.logger.info(f"Run {run_name} has only 1 partial file. Moving directly to final.")
                shutil.move(str(src), str(dst))
                continue

            final_list_filename = manager.output_dir / 'files' / f'{run_name}_final.list'
            with open(final_list_filename, mode='w', encoding='utf-8') as f_final:
                for pfile in partial_files:
                    f_final.write(f"{pfile}\n")

            final_output_name = f"{run_name}.root"
            f_list.write(f"{final_list_filename},{final_output_name},{manager.output_dir}/output\n")
            stage2_counter += 1

    manager.write_submit_file(arguments=arguments, sub_file_name="stage2.sub", log_prefix="stage2")

    if stage2_counter > 0:
        manager.logger.info(f"Submitting {stage2_counter} final merge jobs...")
        manager.finalize_submission(queue_arg="list_file,out_name,out_dir from jobs_stage2.list", sub_file_name="stage2.sub")
        manager.logger.info("All jobs submitted.")
    else:
        manager.logger.info("No final merge jobs to submit for Stage 2.")

def setup_hadd_subparsers(subparsers):
    hadd_parser = subparsers.add_parser('hadd', help='hadd condor jobs.')
    hadd_parser.add_argument('-i', '--input-dir', type=str, required=True, help='Directory of runs to combine output.')
    hadd_parser.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    hadd_parser.add_argument('-n', '--hadd-max', type=int, default=10, help='Hadd Max at once. Default: 10')
    hadd_parser.add_argument('-s', '--memory', type=float, default=1.0, help='Memory (units of GB). Default: 1.0 GB.')
    hadd_parser.add_argument('-m', '--max-retries', type=int, default=3, help='Max Condor job retries on failure. Default: 3.')
    hadd_parser.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    hadd_parser.add_argument('-f', '--condor-script', type=str, default='scripts/genHadd.sh', help='Condor Script.')
    hadd_parser.set_defaults(func=hadd_jobs)
