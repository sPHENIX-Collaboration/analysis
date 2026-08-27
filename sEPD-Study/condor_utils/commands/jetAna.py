import sys
from pathlib import Path

from condor_utils.core.helpers import chunk_list
from condor_utils.core.manager import CondorJobManager
from condor_utils.cli import get_common_parser

def run_jet_jobs(args, version):
    manager = CondorJobManager(args, job_name=version)
    if version == "jetAna":
        manager.add_file_to_check(args.f4a_qa_list)
    manager.add_file_to_check(args.jetAna_macro)
    manager.add_file_to_check(args.jetAna_bin)

    manager.validate_paths()

    jet_pt_min      = args.jet_pt_min
    files_per_job   = args.files_per_job

    init_log = {
        'Jet pT Min': f"{jet_pt_min} GeV",
        'Files Per Job': files_per_job,
        'Jet Ana Macro': Path(args.jetAna_macro).resolve(),
        'Jet Ana Bin': Path(args.jetAna_bin).resolve(),
    }

    if version == "jetAna":
        jet_radius_type = args.jet_radius_type
        R_values = {2: 0.2, 3: 0.3}
        R_value = R_values.get(jet_radius_type)
        if R_value is not None:
            jet_eta_max = 1.1 - R_value
        else:
            manager.logger.critical(f'Invalid jet_radius_type: {jet_radius_type}')
            sys.exit(1)
        init_log['Input F4A QA List'] = Path(args.f4a_qa_list).resolve()
        init_log['Jet eta Max'] = jet_eta_max
        init_log['Jet Radius Type'] = jet_radius_type
    elif version == "jetAnav2":
        jet_eta_max = args.jet_eta_max
        init_log['Jet eta Max'] = jet_eta_max
    elif version == "jetAnav3":
        init_log['Do Iter'] = args.do_iter
        init_log['Do Mult'] = args.do_mult
        init_log['Do Unsub'] = args.do_unsub
        init_log['Do RCone'] = args.do_rcone
        init_log['Lead Jet pT Threshold'] = f"{args.lead_jet_pt_threshold} GeV"

    manager.log_initialization(init_log)

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

    if version == "jetAna":
        run_paths = [Path(l.strip()) for l in Path(args.f4a_qa_list).resolve().read_text(encoding='utf-8').splitlines()]
        run_map = {p.stem: str(p) for p in run_paths}

    jobs_list_file = manager.output_dir / 'jobs.list'
    jobs_list_file.unlink(missing_ok=True)
    total_jobs = 0

    with open(jobs_list_file, mode='w', encoding='utf-8') as f_jobs:
        for run_id, trees in run_trees.items():
            if version == "jetAna":
                if run_id not in run_map:
                    manager.logger.warning(f"Run {run_id} found in input trees but not in QA list. Skipping.")
                    continue
                qa_file = run_map[run_id]

            for i, chunk in enumerate(chunk_list(trees, files_per_job)):
                chunk_file = files_dir / f'{run_id}_part_{i}.list'
                chunk_file.write_text("\n".join(chunk) + "\n", encoding='utf-8')

                if version == "jetAna":
                    f_jobs.write(f"{chunk_file},{qa_file}\n")
                else:
                    f_jobs.write(f"{chunk_file}\n")
                total_jobs += 1

    manager.logger.info(f"Total jobs prepared: {total_jobs}")

    if version == "jetAna":
        arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) $(input_f4a_qa) {jet_pt_min} {jet_eta_max} {jet_radius_type} {manager.output_dir}/output"
        queue_arg = "input_tree_list,input_f4a_qa from jobs.list"
    elif version == "jetAnav2":
        arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) {jet_pt_min} {jet_eta_max} {manager.output_dir}/output"
        queue_arg = "input_tree_list from jobs.list"
    elif version == "jetAnav3":
        arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) {jet_pt_min} {manager.output_dir}/output {args.do_iter} {args.do_mult} {args.do_unsub} {args.do_rcone} {args.lead_jet_pt_threshold}"
        queue_arg = "input_tree_list from jobs.list"
    else:
        arguments = f"{manager.output_dir / Path(args.jetAna_bin).name} $(input_tree_list) {jet_pt_min} {manager.output_dir}/output"
        queue_arg = "input_tree_list from jobs.list"

    sub_file_name = f"{manager.condor_script.stem}.sub"
    manager.write_submit_file(arguments=arguments, sub_file_name=sub_file_name)
    manager.finalize_submission(queue_arg=queue_arg, sub_file_name=sub_file_name)

def jetAna_jobs(args):
    run_jet_jobs(args, "jetAna")

def jetAnav2_jobs(args):
    run_jet_jobs(args, "jetAnav2")

def jetAnav3_jobs(args):
    run_jet_jobs(args, "jetAnav3")

def add_common_args(parser, name_suffix):
    parser.add_argument('-i', '--input-list', type=str, required=True, help='List of TTrees to analyze.')
    parser.add_argument('-j', '--jet-pt-min', type=float, default=10, help='Minimum Jet pT. Default: 10 [GeV]')
    parser.add_argument('-p', '--files-per-job', type=int, default=100, help='Number of trees per job list. Default: 100')
    parser.add_argument('-f', '--jetAna-macro', type=str, default=f'macros/Jet-{name_suffix}.C', help=f'Jet-Ana Macro. Default: macros/Jet-{name_suffix}.C')
    parser.add_argument('-f2', '--jetAna-bin', type=str, default=f'bin/Jet-{name_suffix}', help=f'Jet-Ana Bin. Default: bin/Jet-{name_suffix}')
    parser.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    parser.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB). Default: 1 GB.')
    parser.add_argument('-m', '--max-retries', type=int, default=3, help='Max Condor job retries on failure. Default: 3.')
    parser.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    parser.add_argument('-f3', '--condor-script', type=str, default=f'scripts/genJet{name_suffix}.sh', help='Condor Script.')
    parser.add_argument('-f4', '--common-errors', type=str, default='files/common-errors.txt', help='Common Errors.')

def setup_jetAna_subparsers(subparsers):
    jetAna = subparsers.add_parser('jetAna', help='jetAna condor jobs.')
    add_common_args(jetAna, "Ana")
    jetAna.add_argument('-i2', '--f4a-qa-list', type=str, required=True, help='List of F4A QA.')
    jetAna.add_argument('-j2', '--jet-radius-type', type=float, default=3, help='Jet Radius Type. Default: 3 (i.e. R = 0.3)')
    jetAna.set_defaults(func=jetAna_jobs)

    jetAnav2 = subparsers.add_parser('jetAnav2', help='jetAnav2 condor jobs.')
    add_common_args(jetAnav2, "Anav2")
    jetAnav2.add_argument('-e', '--jet-eta-max', type=float, default=0.9, help='Maximum Jet eta. Default: 0.9')
    jetAnav2.set_defaults(func=jetAnav2_jobs)

    jetAnav3 = subparsers.add_parser('jetAnav3', help='jetAnav3 condor jobs.')
    add_common_args(jetAnav3, "Anav3")
    jetAnav3.add_argument('--do-iter', type=int, default=1, help='Do iter. Default: 1')
    jetAnav3.add_argument('--do-mult', type=int, default=1, help='Do mult. Default: 1')
    jetAnav3.add_argument('--do-unsub', type=int, default=1, help='Do unsub. Default: 1')
    jetAnav3.add_argument('--do-rcone', type=int, default=1, help='Do rcone. Default: 1')
    jetAnav3.add_argument('--lead-jet-pt-threshold', type=float, default=100.0, help='Leading Jet pT Threshold for CSV saving. Default: 100 [GeV]')
    jetAnav3.set_defaults(func=jetAnav3_jobs)
