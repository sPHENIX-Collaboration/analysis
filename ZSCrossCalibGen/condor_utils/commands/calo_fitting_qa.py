import os
from pathlib import Path

from condor_utils.core.helpers import resolve_repo_path
from condor_utils.core.manager import CondorJobManager


def create_calo_fitting_qa_jobs(args):
    """Sets up Condor directories, job lists, and submission files for Fun4All_CaloFittingQA."""
    manager = CondorJobManager(args, job_name="Fun4All_CaloFittingQA")

    # Resolve macros and scripts relative to cwd or repo root
    f4a_macro_path = resolve_repo_path(args.f4a_macro)
    calo_fitting_path = resolve_repo_path(args.calo_fitting_macro)
    condor_script_path = resolve_repo_path(args.condor_script)

    manager.add_file_to_check(f4a_macro_path)
    manager.add_file_to_check(calo_fitting_path)
    manager.add_file_to_check(condor_script_path)
    manager.validate_paths()

    default_build = Path(os.environ['OFFLINE_MAIN']).name if os.environ.get('OFFLINE_MAIN') else 'new'
    build_val = getattr(args, 'build', None) or default_build

    init_log = {
        'sPHENIX Build': build_val,
        'Fun4All Macro': f4a_macro_path.resolve(),
        'Calo Fitting Macro': calo_fitting_path.resolve(),
        'Condor Worker Script': condor_script_path.resolve(),
        'Execution Mode': 'Direct Submit' if args.submit else 'Dry Run (Generate scripts only)',
    }
    manager.log_initialization(init_log)

    manager.prepare_directories()

    # Stage dependencies into output_dir
    manager.copy_dependencies(
        extra_files=[f4a_macro_path, calo_fitting_path, condor_script_path]
    )
    # Copy rebuild.info and rebuild.log from release build dir into the job dir
    manager.copy_build_info(build_val)

    # Prepare jobs.list: each line is <seg_list_path>,<output_root_name>
    job_paths = manager.prepare_job_lists(jobs_file_name="jobs.list")
    if not job_paths:
        manager.logger.error("No valid jobs could be prepared. Submission aborted.")
        return

    # Condor arguments passed to genFun4All_CaloFittingQA.sh:
    # 1. Macro: Fun4All_CaloFittingQA.C
    # 2. $(input_seg_list)
    # 3. $(output_file)
    # 4. nEvents
    # 5. dbtag
    # 6. submitDir
    # 7. build
    arguments = (
        f"{f4a_macro_path.name} "
        f"$(input_seg_list) "
        f"$(output_file) "
        f"{args.events} "
        f"{args.dbtag} "
        f"{manager.output_dir} "
        f"{build_val}"
    )

    sub_file_name = "genFun4All_CaloFittingQA.sub"
    manager.write_submit_file(
        arguments=arguments,
        executable=condor_script_path.name,
        sub_file_name=sub_file_name,
    )

    manager.finalize_submission(
        queue_arg="input_seg_list,output_file from jobs.list",
        sub_file_name=sub_file_name,
        execute=args.submit,
    )
