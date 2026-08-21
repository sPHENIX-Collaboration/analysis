import math
from pathlib import Path
from condor_utils.core.manager import CondorJobManager
from condor_utils.core.helpers import run_command_and_log, chunk_list
from condor_utils.cli import get_common_parser

def create_trigger_qa_jobs(args):
    manager = CondorJobManager(args, job_name="Trigger QA")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_dir_to_check(args.src_dir)
    manager.validate_paths()

    manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro], extra_dirs=[args.src_dir])

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    for line in manager.input_list.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        manager.logger.info(f'Processing: {line}')
        file_stem = Path(line).stem

        command = f'split --lines {args.dst_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
        run_command_and_log(command, manager.logger, files_dir, False)

        command = f'realpath {files_dir}/{file_stem}* >> {jobs_file.name}'
        run_command_and_log(command, manager.logger, manager.output_dir, False)

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def create_calo_qa_jobs(args):
    manager = CondorJobManager(args, job_name="Calo QA")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.f4a_bin)
    manager.add_dir_to_check(args.src_dir)
    manager.validate_paths()

    total_files = manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Fun4All Bin': Path(args.f4a_bin).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro, args.f4a_bin], extra_dirs=[args.src_dir])

    CONDOR_SUBMISSION_LIMIT = 100000
    files_per_job = math.ceil(total_files / CONDOR_SUBMISSION_LIMIT)
    manager.logger.info(f'Files Per Job: {files_per_job}')

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    for line in manager.input_list.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        manager.logger.info(f'Processing: {line}')
        file_stem = Path(line).stem

        command = f'split --lines {files_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
        run_command_and_log(command, manager.logger, files_dir, False)

        command = f'realpath {files_dir}/{file_stem}* >> {jobs_file.name}'
        run_command_and_log(command, manager.logger, manager.output_dir, False)

    arguments = f"{manager.output_dir / Path(args.f4a_bin).name} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def create_centrality_qa_jobs(args):
    manager = CondorJobManager(args, job_name="Centrality QA")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.f4a_bin)
    manager.add_dir_to_check(args.src_dir)
    manager.validate_paths()

    total_files = manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Fun4All Bin': Path(args.f4a_bin).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro, args.f4a_bin], extra_dirs=[args.src_dir])

    CONDOR_SUBMISSION_LIMIT = 100000
    files_per_job = math.ceil(total_files / CONDOR_SUBMISSION_LIMIT)
    manager.logger.info(f'Files Per Job: {files_per_job}')

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    for line in manager.input_list.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        manager.logger.info(f'Processing: {line}')
        file_stem = Path(line).stem

        command = f'split --lines {files_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
        run_command_and_log(command, manager.logger, files_dir, False)

        command = f'realpath {files_dir}/{file_stem}* >> {jobs_file.name}'
        run_command_and_log(command, manager.logger, manager.output_dir, False)

    arguments = f"{manager.output_dir / Path(args.f4a_bin).name} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def create_sepd_qa_jobs(args):
    manager = CondorJobManager(args, job_name="sEPD QA")
    manager.add_file_to_check(args.sepd_macro)
    manager.add_file_to_check(args.sepd_bin)
    manager.validate_paths()

    files_per_job = args.files_per_job

    manager.log_initialization({
        'Files Per Job': files_per_job,
        'Events': args.events if args.events else "All",
        'Verbosity': args.verbosity,
        'sEPD QA Macro': Path(args.sepd_macro).resolve(),
        'sEPD QA Bin': Path(args.sepd_bin).resolve(),
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.sepd_macro, args.sepd_bin])

    run_trees = {}
    input_lines = manager.input_list.read_text(encoding='utf-8').splitlines()

    for line in input_lines:
        line = line.strip()
        if not line:
            continue
        tree_path = Path(line)
        if tree_path.parent.name == "tree":
            run_id = tree_path.parent.parent.name
        else:
            run_id = tree_path.parent.name
        if run_id not in run_trees:
            run_trees[run_id] = []
        run_trees[run_id].append(str(tree_path))

    jobs_list_file = manager.output_dir / 'jobs.list'
    jobs_list_file.unlink(missing_ok=True)
    total_jobs = 0

    with open(jobs_list_file, mode='w', encoding='utf-8') as f_jobs:
        for run_id, trees in run_trees.items():
            for i, chunk in enumerate(chunk_list(trees, files_per_job)):
                chunk_file = files_dir / f'{run_id}_part_{i}.list'
                chunk_file.write_text("\n".join(chunk) + "\n", encoding='utf-8')
                f_jobs.write(f"{chunk_file}\n")
                total_jobs += 1

    manager.logger.info(f"Total jobs prepared: {total_jobs}")

    arguments = f"{manager.output_dir / Path(args.sepd_bin).name} $(input_tree_list) {args.events} {manager.output_dir}/output {args.verbosity}"
    queue_arg = "input_tree_list from jobs.list"

    sub_file_name = f"{manager.condor_script.stem}.sub"
    manager.write_submit_file(arguments=arguments, sub_file_name=sub_file_name)
    manager.finalize_submission(queue_arg=queue_arg, sub_file_name=sub_file_name)

def setup_qa_subparsers(subparsers):
    # trigger_qa
    trigger_qa = subparsers.add_parser('trigger_qa', parents=[get_common_parser()], help='Create condor submission directory.')
    trigger_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_TriggerQA.C', help='Fun4All Macro.')
    trigger_qa.set_defaults(
        dst_per_job=4,
        memory=1.0,
        condor_script='scripts/genTriggerQA.sh',
        func=create_trigger_qa_jobs
    )

    # calo_qa
    calo_qa = subparsers.add_parser('calo_qa', parents=[get_common_parser()], help='Create condor submission directory.')
    calo_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_CaloQA.C', help='Fun4All Macro.')
    calo_qa.add_argument('-b', '--f4a-bin', type=str, default='bin/Fun4All_CaloQA', help='Fun4All Bin.')
    calo_qa.set_defaults(
        memory=2.0,
        condor_script='scripts/genFun4All_CaloQA.sh',
        func=create_calo_qa_jobs
    )

    # centrality_qa
    centrality_qa = subparsers.add_parser('centrality_qa', parents=[get_common_parser()], help='Create condor submission directory.')
    centrality_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_CentralityQA.C', help='Fun4All Macro.')
    centrality_qa.add_argument('-b', '--f4a-bin', type=str, default='bin/Fun4All_CentralityQA', help='Fun4All Bin.')
    centrality_qa.set_defaults(
        memory=2.0,
        condor_script='scripts/genFun4All_CentralityQA.sh',
        func=create_centrality_qa_jobs
    )

    # sepd_qa
    sepd_qa = subparsers.add_parser('sepd_qa', parents=[get_common_parser()], help='Create condor submission directory for sEPD QA.')
    sepd_qa.add_argument('-f', '--sepd-macro', type=str, default='macros/sEPD-QA.C', help='sEPD-QA Macro. Default: macros/sEPD-QA.C')
    sepd_qa.add_argument('-b', '--sepd-bin', type=str, default='bin/sEPD-QA', help='sEPD-QA Bin. Default: bin/sEPD-QA')
    sepd_qa.add_argument('-p', '--files-per-job', type=int, default=100, help='Number of trees per job list. Default: 100')
    sepd_qa.add_argument('-v', '--verbosity', type=int, default=0, help='Verbosity. Default: 0')
    sepd_qa.set_defaults(
        memory=1.0,
        condor_script='scripts/gensEPDQA.sh',
        func=create_sepd_qa_jobs
    )
