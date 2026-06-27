import sys
import shutil
import datetime
import textwrap
from pathlib import Path
import logging

from condor_utils.core.logging import setup_logging
from condor_utils.core.helpers import run_command_and_log, chunk_list

def jetAna_jobs(args):
    input_list      = Path(args.input_list).resolve()
    f4a_qa_list     = Path(args.f4a_qa_list).resolve()
    jet_pt_min      = args.jet_pt_min
    jet_radius_type = args.jet_radius_type
    files_per_job   = args.files_per_job
    output_dir      = Path(args.output_dir).resolve()
    jetAna_macro    = Path(args.jetAna_macro).resolve()
    jetAna_bin      = Path(args.jetAna_bin).resolve()
    log_file        = output_dir / 'log.txt'
    condor_memory   = args.memory
    condor_script   = Path(args.condor_script).resolve()
    condor_log_dir  = Path(args.condor_log_dir).resolve()

    output_dir.mkdir(parents=True, exist_ok=True)
    logger = setup_logging(log_file, logging.DEBUG)

    R_values = {2: 0.2, 3: 0.3}
    R_value = R_values.get(jet_radius_type)

    if R_value is not None:
        jet_eta_max = 1.1 - R_value
    else:
        logger.critical(f'Invalid jet_radius_type: {jet_radius_type}')
        sys.exit(1)

    for f in [input_list, f4a_qa_list, condor_script, jetAna_bin]:
        if not f.is_file():
            logger.critical(f'File: {f} does not exist!')
            sys.exit(1)

    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    logger.info(f'Input List: {input_list}')
    logger.info(f'Input F4A QA List: {f4a_qa_list}')
    logger.info(f'Jet pT Min: {jet_pt_min} GeV')
    logger.info(f'Jet eta Max: {jet_eta_max}')
    logger.info(f'Jet Radius Type: {jet_radius_type}')
    logger.info(f'Files Per Job: {files_per_job}')
    logger.info(f'Jet Ana Macro: {jetAna_macro}')
    logger.info(f'Jet Ana Bin: {jetAna_bin}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info(f'Condor Memory: {condor_memory} GB')
    logger.info(f'Condor Script: {condor_script}')
    logger.info(f'Condor Log Directory: {condor_log_dir}')

    if condor_log_dir.is_dir():
        shutil.rmtree(condor_log_dir)

    condor_log_dir.mkdir(parents=True, exist_ok=True)

    subdirectories = ['stdout', 'error', 'output', 'files']
    for subdir in subdirectories:
        (output_dir / subdir).mkdir(parents=True, exist_ok=True)

    shutil.copy(jetAna_macro, output_dir)
    jetAna_bin = shutil.copy(jetAna_bin, output_dir)
    shutil.copy(condor_script, output_dir)

    run_paths = [Path(l.strip()) for l in f4a_qa_list.read_text(encoding='utf-8').splitlines()]
    run_map = {p.stem: str(p) for p in run_paths}

    run_trees = {}
    input_lines = input_list.read_text(encoding='utf-8').splitlines()

    for line in input_lines:
        tree_path = Path(line.strip())
        run_id = tree_path.parent.parent.name

        if run_id not in run_trees:
            run_trees[run_id] = []

        run_trees[run_id].append(str(tree_path))

    jobs_list_file = output_dir / 'jobs.list'
    total_jobs = 0

    with open(jobs_list_file, mode='w', encoding='utf-8') as f_jobs:
        for run_id, trees in run_trees.items():
            if run_id not in run_map:
                logger.warning(f"Run {run_id} found in input trees but not in QA list. Skipping.")
                continue

            qa_file = run_map[run_id]

            for i, chunk in enumerate(chunk_list(trees, files_per_job)):
                chunk_file = output_dir / 'files' / f'{run_id}_part_{i}.list'
                chunk_file.write_text("\\n".join(chunk) + "\\n", encoding='utf-8')

                f_jobs.write(f"{chunk_file},{qa_file}\\n")
                total_jobs += 1

    logger.info(f"Total jobs prepared: {total_jobs}")

    submit_file_content = textwrap.dedent(f"""\
        executable     = {condor_script.name}
        arguments      = {jetAna_bin.name} $(input_tree_list) $(input_f4a_qa) {jet_pt_min} {jet_eta_max} {jet_radius_type} {output_dir}/output
        log            = {condor_log_dir}/job-$(ClusterId)-$(Process).log
        output         = stdout/job-$(ClusterId)-$(Process).out
        error          = error/job-$(ClusterId)-$(Process).err
        request_memory = {condor_memory}GB
    """)

    (output_dir / 'genJetAna.sub').write_text(submit_file_content)

    command = f'cd {output_dir} && condor_submit genJetAna.sub -queue "input_tree_list,input_f4a_qa from jobs.list"'
    logger.info(command)


def setup_jetAna_subparsers(subparsers, common_parser):
    jetAna = subparsers.add_parser('jetAna', help='jetAna condor jobs.')
    jetAna.add_argument('-i', '--input-list', type=str, required=True, help='List of TTrees to analyze.')
    jetAna.add_argument('-i2', '--f4a-qa-list', type=str, required=True, help='List of F4A QA.')
    jetAna.add_argument('-j', '--jet-pt-min', type=float, default=10, help='Minimum Jet pT. Default: 10 [GeV]')
    jetAna.add_argument('-j2', '--jet-radius-type', type=float, default=3, help='Jet Radius Type. Default: 3 (i.e. R = 0.3)')
    jetAna.add_argument('-p', '--files-per-job', type=int, default=100, help='Number of trees per job list. Default: 100')
    jetAna.add_argument('-f', '--jetAna-macro', type=str, default='macros/Jet-Ana.C', help='Jet-Ana Macro. Default: macros/Jet-Ana.C')
    jetAna.add_argument('-f2', '--jetAna-bin', type=str, default='bin/Jet-Ana', help='Jet-Ana Bin. Default: bin/Jet-Ana')
    jetAna.add_argument('-o', '--output-dir', type=str, default='scratch/test', help='Output Directory. Default: scratch/test')
    jetAna.add_argument('-s', '--memory', type=float, default=0.5, help='Memory (units of GB). Default: 0.5 GB.')
    jetAna.add_argument('-l', '--condor-log-dir', type=str, default='/tmp/anarde/dump', help='Condor Log Directory.')
    jetAna.add_argument('-f3', '--condor-script', type=str, default='scripts/genJetAna.sh', help='Condor Script.')
    jetAna.set_defaults(func=jetAna_jobs)
