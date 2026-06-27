import math
from pathlib import Path
from condor_utils.core.manager import CondorJobManager
from condor_utils.core.helpers import run_command_and_log

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
        
    arguments = f"{Path(args.f4a_macro).resolve()} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
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
        
    arguments = f"{Path(args.f4a_bin).resolve()} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
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
        
    arguments = f"{Path(args.f4a_bin).resolve()} $(input_dst) test-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def setup_qa_subparsers(subparsers, common_parser):
    # trigger_qa
    trigger_qa = subparsers.add_parser('trigger_qa', parents=[common_parser], help='Create condor submission directory.')
    trigger_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_TriggerQA.C', help='Fun4All Macro.')
    trigger_qa.set_defaults(
        dst_per_job=2,
        memory=1.0,
        condor_script='scripts/genTriggerQA.sh',
        func=create_trigger_qa_jobs
    )
    
    # calo_qa
    calo_qa = subparsers.add_parser('calo_qa', parents=[common_parser], help='Create condor submission directory.')
    calo_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_CaloQA.C', help='Fun4All Macro.')
    calo_qa.add_argument('-b', '--f4a-bin', type=str, default='bin/Fun4All_CaloQA', help='Fun4All Bin.')
    calo_qa.set_defaults(
        memory=2.0,
        condor_script='scripts/genFun4All_CaloQA.sh',
        func=create_calo_qa_jobs
    )
    
    # centrality_qa
    centrality_qa = subparsers.add_parser('centrality_qa', parents=[common_parser], help='Create condor submission directory.')
    centrality_qa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_CentralityQA.C', help='Fun4All Macro.')
    centrality_qa.add_argument('-b', '--f4a-bin', type=str, default='bin/Fun4All_CentralityQA', help='Fun4All Bin.')
    centrality_qa.set_defaults(
        memory=2.0,
        condor_script='scripts/genFun4All_CentralityQA.sh',
        func=create_centrality_qa_jobs
    )
