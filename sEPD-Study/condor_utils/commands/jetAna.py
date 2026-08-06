import sys
from pathlib import Path

from condor_utils.core.helpers import chunk_list
from condor_utils.core.manager import CondorJobManager
from condor_utils.cli import get_common_parser

def jetAna_jobs(args):
    manager = CondorJobManager(args, job_name="jetAna")
    manager.add_file_to_check(args.f4a_qa_list)
    manager.add_file_to_check(args.jetAna_macro)
    manager.add_file_to_check(args.jetAna_bin)

    manager.validate_paths()

    jet_pt_min      = args.jet_pt_min
    jet_radius_type = args.jet_radius_type
    files_per_job   = args.files_per_job

    R_values = {2: 0.2, 3: 0.3}
    R_value = R_values.get(jet_radius_type)
    if R_value is not None:
        jet_eta_max = 1.1 - R_value
    else:
        manager.logger.critical(f'Invalid jet_radius_type: {jet_radius_type}')
        sys.exit(1)

    manager.log_initialization({
        'Input F4A QA List': Path(args.f4a_qa_list).resolve(),
        'Jet pT Min': f"{jet_pt_min} GeV",
        'Jet eta Max': jet_eta_max,
        'Jet Radius Type': jet_radius_type,
        'Files Per Job': files_per_job,
        'Jet Ana Macro': Path(args.jetAna_macro).resolve(),
        'Jet Ana Bin': Path(args.jetAna_bin).resolve(),
    })

    files_dir = manager.prepare_directories()

    manager.copy_dependencies(extra_files=[args.jetAna_macro, args.jetAna_bin])

    run_paths = [Path(l.strip()) for l in Path(args.f4a_qa_list).resolve().read_text(encoding='utf-8').splitlines()]
    run_map = {p.stem: str(p) for p in run_paths}

    run_trees = {}
    input_lines = manager.input_list.read_text(encoding='utf-8').splitlines()

    for line in input_lines:
        tree_path = Path(line.strip())
        run_id = tree_path.parent.parent.name
        if run_id not in run_trees:
            run_trees[run_id] = []
        run_trees[run_id].append(str(tree_path))

    jobs_list_file = manager.output_dir / 'jobs.list'
    jobs_list_file.unlink(missing_ok=True)
    total_jobs = 0

    with open(jobs_list_file, mode='w', encoding='utf-8') as f_jobs:
        for run_id, trees in run_trees.items():
            if run_id not in run_map:
                manager.logger.warning(f"Run {run_id} found in input trees but not in QA list. Skipping.")
                continue

            qa_file = run_map[run_id]

            for i, chunk in enumerate(chunk_list(trees, files_per_job)):
                chunk_file = files_dir / f'{run_id}_part_{i}.list'
                chunk_file.write_text("\n".join(chunk) + "\n", encoding='utf-8')

                f_jobs.write(f"{chunk_file},{qa_file}\n")
                total_jobs += 1

    manager.logger.info(f"Total jobs prepared: {total_jobs}")

    arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) $(input_f4a_qa) {jet_pt_min} {jet_eta_max} {jet_radius_type} {manager.output_dir}/output"
    sub_file_name = f"{manager.condor_script.stem}.sub"
    manager.write_submit_file(arguments=arguments, sub_file_name=sub_file_name)
    manager.finalize_submission(queue_arg="input_tree_list,input_f4a_qa from jobs.list", sub_file_name=sub_file_name)


def jetAnav2_jobs(args):
    manager = CondorJobManager(args, job_name="jetAnav2")
    manager.add_file_to_check(args.jetAna_macro)
    manager.add_file_to_check(args.jetAna_bin)

    manager.validate_paths()

    jet_pt_min      = args.jet_pt_min
    jet_eta_max     = args.jet_eta_max
    files_per_job   = args.files_per_job

    manager.log_initialization({
        'Jet pT Min': f"{jet_pt_min} GeV",
        'Jet eta Max': jet_eta_max,
        'Files Per Job': files_per_job,
        'Jet Ana Macro': Path(args.jetAna_macro).resolve(),
        'Jet Ana Bin': Path(args.jetAna_bin).resolve(),
    })

    files_dir = manager.prepare_directories()

    manager.copy_dependencies(extra_files=[args.jetAna_macro, args.jetAna_bin])

    run_trees = {}
    input_lines = manager.input_list.read_text(encoding='utf-8').splitlines()

    for line in input_lines:
        tree_path = Path(line.strip())
        run_id = tree_path.parent.parent.name
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

    arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) {jet_pt_min} {jet_eta_max} {manager.output_dir}/output"
    sub_file_name = f"{manager.condor_script.stem}.sub"
    manager.write_submit_file(arguments=arguments, sub_file_name=sub_file_name)
    manager.finalize_submission(queue_arg="input_tree_list from jobs.list", sub_file_name=sub_file_name)


def setup_jetAna_subparsers(subparsers):
    jetAna = subparsers.add_parser('jetAna', help='jetAna condor jobs.')
    jetAna.add_argument('-i', '--input-list', type=str, required=True, help='List of TTrees to analyze.')
    jetAna.add_argument('-i2', '--f4a-qa-list', type=str, required=True, help='List of F4A QA.')
    jetAna.add_argument('-j', '--jet-pt-min', type=float, default=10, help='Minimum Jet pT. Default: 10 [GeV]')
    jetAna.add_argument('-j2', '--jet-radius-type', type=float, default=3, help='Jet Radius Type. Default: 3 (i.e. R = 0.3)')
    jetAna.add_argument('-p', '--files-per-job', type=int, default=100, help='Number of trees per job list. Default: 100')
    jetAna.add_argument('-f', '--jetAna-macro', type=str, default='macros/Jet-Ana.C', help='Jet-Ana Macro. Default: macros/Jet-Ana.C')
    jetAna.add_argument('-f2', '--jetAna-bin', type=str, default='bin/Jet-Ana', help='Jet-Ana Bin. Default: bin/Jet-Ana')
    jetAna.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    jetAna.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB). Default: 1 GB.')
    jetAna.add_argument('-m', '--max-retries', type=int, default=3, help='Max Condor job retries on failure. Default: 3.')
    jetAna.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    jetAna.add_argument('-f3', '--condor-script', type=str, default='scripts/genJetAna.sh', help='Condor Script.')
    jetAna.add_argument('-f4', '--common-errors', type=str, default='files/common-errors.txt', help='Common Errors.')
    jetAna.set_defaults(func=jetAna_jobs)

    jetAnav2 = subparsers.add_parser('jetAnav2', help='jetAnav2 condor jobs.')
    jetAnav2.add_argument('-i', '--input-list', type=str, required=True, help='List of TTrees to analyze.')
    jetAnav2.add_argument('-j', '--jet-pt-min', type=float, default=10, help='Minimum Jet pT. Default: 10 [GeV]')
    jetAnav2.add_argument('-e', '--jet-eta-max', type=float, default=0.9, help='Maximum Jet eta. Default: 0.9')
    jetAnav2.add_argument('-p', '--files-per-job', type=int, default=100, help='Number of trees per job list. Default: 100')
    jetAnav2.add_argument('-f', '--jetAna-macro', type=str, default='macros/Jet-Anav2.C', help='Jet-Ana Macro. Default: macros/Jet-Anav2.C')
    jetAnav2.add_argument('-f2', '--jetAna-bin', type=str, default='bin/Jet-Anav2', help='Jet-Ana Bin. Default: bin/Jet-Anav2')
    jetAnav2.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    jetAnav2.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB). Default: 1 GB.')
    jetAnav2.add_argument('-m', '--max-retries', type=int, default=3, help='Max Condor job retries on failure. Default: 3.')
    jetAnav2.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    jetAnav2.add_argument('-f3', '--condor-script', type=str, default='scripts/genJetAnav2.sh', help='Condor Script.')
    jetAnav2.add_argument('-f4', '--common-errors', type=str, default='files/common-errors.txt', help='Common Errors.')
    jetAnav2.set_defaults(func=jetAnav2_jobs)
