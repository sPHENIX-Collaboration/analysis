import math
import shutil
from pathlib import Path
from condor_utils.core.manager import CondorJobManager
from condor_utils.core.helpers import run_command_and_log, get_line_count, chunk_list
from condor_utils.cli import get_common_parser

def create_f4a_jobs(args):
    if "Fun4All_BkgSub" in args.f4a_macro:
        args.condor_script = 'scripts/genFun4All_BkgSub.sh'

    manager = CondorJobManager(args, job_name="F4A")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.calo_calib_macro)
    manager.add_file_to_check(args.HIJetReco_macro)
    manager.add_dir_to_check(args.src_dir)

    calib_list = Path(args.calib).resolve() if args.calib else None
    if calib_list:
        manager.add_file_to_check(calib_list)

    eta_calib = Path(args.eta_calib_path).resolve() if getattr(args, 'eta_calib_path', None) else None
    if eta_calib:
        manager.add_file_to_check(eta_calib)

    event_list = Path(args.event_list).resolve() if getattr(args, 'event_list', None) else None
    if event_list:
        manager.add_file_to_check(event_list)

    manager.validate_paths()

    init_log = {
        'Calib List': calib_list if calib_list else "Not Provided (Using default)",
        'Eta Calib': eta_calib if eta_calib else "Not Provided (Using default/empty)",
        'Event List': event_list if event_list else "Not Provided (Using all events)",
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Calo Calib Macro': Path(args.calo_calib_macro).resolve(),
        'HIJetReco Macro': Path(args.HIJetReco_macro).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    }
    if "Fun4All_BkgSub" in args.f4a_macro:
        init_log['Do RCone'] = bool(getattr(args, 'do_rcone', False))
        init_log['Do Mult'] = bool(getattr(args, 'do_mult', 1))
        init_log['Do Neg Energy Threshold'] = bool(getattr(args, 'do_neg_energy_threshold', 1))
        init_log['Neg Energy Threshold'] = float(getattr(args, 'neg_energy_threshold', -2.0))

    manager.log_initialization(init_log)

    files_dir = manager.prepare_directories()

    extra_files = [args.f4a_macro, args.calo_calib_macro, args.HIJetReco_macro]
    if calib_list:
        extra_files.append(calib_list)
    if eta_calib:
        extra_files.append(eta_calib)
    if event_list:
        extra_files.append(event_list)

    manager.copy_dependencies(extra_files=extra_files, extra_dirs=[args.src_dir])

    calib_map = {}
    if calib_list:
        for line in calib_list.read_text(encoding='utf-8').splitlines():
            line = line.strip()
            if not line:
                continue
            run = Path(line.split(',')[0]).parts[-2]
            manager.logger.info(f'Processing Calib: {line}, run: {run}')
            calib_map[run] = line

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)
    jobs_temp_file = manager.output_dir / 'jobs-temp.list'
    jobs_temp_file.unlink(missing_ok=True)

    for line in manager.input_list.read_text(encoding='utf-8').splitlines():
        line = line.strip()
        manager.logger.info(f'Processing: {line}')
        file_stem = Path(line).stem

        command = f'split --lines {args.dst_per_job} {line} -d -a 3 {file_stem}- --additional-suffix=.list'
        run_command_and_log(command, manager.logger, files_dir, False)

        command = f'realpath {files_dir}/{file_stem}* >> {jobs_temp_file.name}'
        run_command_and_log(command, manager.logger, manager.output_dir, False)

    with open(jobs_temp_file, mode='r', encoding='utf-8') as file_in, \
         open(jobs_file, mode='w', encoding='utf-8') as file_out:
        for line in file_in:
            line = Path(line.strip()).resolve()
            run = line.stem.split('-')[1].lstrip('0')
            if run in calib_map:
                file_out.write(f'{line},{calib_map[run]}\n')
            else:
                file_out.write(f'{line},default\n')

    jobs_temp_file.unlink(missing_ok=True)

    if "Fun4All_BkgSub" in args.f4a_macro:
        eta_calib_val = (manager.output_dir / eta_calib.name) if eta_calib else "none"
        event_list_val = (manager.output_dir / event_list.name) if event_list else "none"
        do_rcone_val = 1 if getattr(args, 'do_rcone', False) else 0
        do_mult_val = getattr(args, 'do_mult', 1)
        do_neg_energy_threshold_val = getattr(args, 'do_neg_energy_threshold', 1)
        neg_energy_threshold_val = getattr(args, 'neg_energy_threshold', -2.0)
        bkgsub_args = f"{args.do_flow} {eta_calib_val} {event_list_val} {do_rcone_val} {do_mult_val} {do_neg_energy_threshold_val} {neg_energy_threshold_val} "
    else:
        bkgsub_args = ""

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst) $(input_calib) test-$(ClusterId)-$(Process).root tree-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {bkgsub_args}{manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst,input_calib from jobs.list")

def create_f4a_zdc_jobs(args):
    manager = CondorJobManager(args, job_name="F4A ZDC")
    manager.add_file_to_check(args.f4a_macro)
    manager.validate_paths()

    manager.log_initialization({
        'Logging Interval': f"{args.log_interval} Events",
        'Fun4All Macro': Path(args.f4a_macro).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro])

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    all_dst_lines = []
    for list_path_str in manager.input_list.read_text(encoding='utf-8').splitlines():
        list_path_str = list_path_str.strip()
        if not list_path_str:
            continue

        sub_list = Path(list_path_str)
        if sub_list.is_file():
            all_dst_lines.extend(sub_list.read_text(encoding='utf-8').splitlines())
        else:
            manager.logger.warning(f"Sub-list file not found: {sub_list}")

    total_jobs = 0
    with open(jobs_file, mode='w', encoding='utf-8') as f_jobs:
        for i, chunk in enumerate(chunk_list(all_dst_lines, args.dst_per_job)):
            chunk_file = files_dir / f'chunk-{i:05d}.list'
            chunk_file.write_text("\n".join(chunk) + "\n", encoding='utf-8')
            f_jobs.write(f"{chunk_file.resolve()}\n")
            total_jobs += 1

    manager.logger.info(f"Total jobs prepared: {total_jobs}")

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst) {args.events} {args.dbtag} {args.log_interval} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def create_f4a_mc_jobs(args):
    manager = CondorJobManager(args, job_name="F4A MC")
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

def create_f4a_data_mc_jobs(args):
    manager = CondorJobManager(args, job_name="F4A Data MC")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.calo_calib_macro)
    manager.add_file_to_check(args.HIJetReco_macro)
    manager.add_dir_to_check(args.src_dir)

    calib_list = Path(args.calib).resolve() if args.calib else None
    if calib_list:
        manager.add_file_to_check(calib_list)

    manager.validate_paths()

    manager.log_initialization({
        'Calib List': calib_list if calib_list else "Not Provided (Using default)",
        'Jet pT Min': f"{args.jet_pt_min} GeV",
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Calo Calib Macro': Path(args.calo_calib_macro).resolve(),
        'HIJetReco Macro': Path(args.HIJetReco_macro).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()

    extra_files = [args.f4a_macro, args.calo_calib_macro, args.HIJetReco_macro]
    if calib_list:
        extra_files.append(calib_list)

    manager.copy_dependencies(extra_files=extra_files, extra_dirs=[args.src_dir])

    calib_map = {}
    if calib_list:
        for line in calib_list.read_text(encoding='utf-8').splitlines():
            line = line.strip()
            if not line:
                continue
            run = Path(line.split(',')[0]).parts[-2]
            manager.logger.info(f'Processing Calib: {line}, run: {run}')
            calib_map[run] = line

    jobs_file = manager.output_dir / 'jobs.list'
    jobs_file.unlink(missing_ok=True)

    with open(manager.input_list, mode='r', encoding='utf-8') as file_in, \
         open(jobs_file, mode='w', encoding='utf-8') as file_out:
        for line in file_in:
            line = line.strip()
            if not line:
                continue

            if not calib_map:
                file_out.write(f'{line},default\n')
            else:
                line_path = Path(line).resolve()
                try:
                    run = line_path.stem.split('-')[3].lstrip('0')
                    if run in calib_map:
                        file_out.write(f'{line},{calib_map[run]}\n')
                    else:
                        file_out.write(f'{line},default\n')
                except IndexError:
                    manager.logger.warning(f"Could not parse run number from {line}. Using default.")
                    file_out.write(f'{line},default\n')

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst_global) $(input_dst_jet) $(input_dst_g4hit) $(input_dst_calo) $(input_calib) test-$(ClusterId)-$(Process).root tree-$(ClusterId)-$(Process).root {args.events} {args.jet_pt_min} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst_global,input_dst_jet,input_dst_g4hit,input_dst_calo,input_calib from jobs.list")

def create_f4a_nobkgsub_jobs(args):
    manager = CondorJobManager(args, job_name="F4A NoBkgSub")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.calo_calib_macro)
    manager.add_file_to_check(args.NoBkgdSubJetReco_macro)
    manager.add_dir_to_check(args.src_dir)
    manager.validate_paths()

    manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Calo Calib Macro': Path(args.calo_calib_macro).resolve(),
        'NoBkgdSubJetReco Macro': Path(args.NoBkgdSubJetReco_macro).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()
    extra_files = [args.f4a_macro, args.calo_calib_macro, args.NoBkgdSubJetReco_macro]
    manager.copy_dependencies(extra_files=extra_files, extra_dirs=[args.src_dir])

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

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst) test-$(ClusterId)-$(Process).root tree-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def create_f4a_noise_jobs(args):
    manager = CondorJobManager(args, job_name="F4A Noise")
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

def create_f4a_calofittingqa_jobs(args):
    manager = CondorJobManager(args, job_name="F4A CaloFittingQA")
    manager.add_file_to_check(args.f4a_macro)
    manager.validate_paths()

    manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro])

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

def create_f4a_sepdqa_jobs(args):
    manager = CondorJobManager(args, job_name="F4A sEPDQA")
    manager.add_file_to_check(args.f4a_macro)
    manager.add_file_to_check(args.calo_calib_macro)
    manager.add_dir_to_check(args.src_dir)
    manager.validate_paths()

    manager.log_initialization({
        'Fun4All Macro': Path(args.f4a_macro).resolve(),
        'Calo Calib Macro': Path(args.calo_calib_macro).resolve(),
        'Source Directory': Path(args.src_dir).resolve()
    })

    files_dir = manager.prepare_directories()
    manager.copy_dependencies(extra_files=[args.f4a_macro, args.calo_calib_macro], extra_dirs=[args.src_dir])

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

    arguments = f"{manager.output_dir / Path(args.f4a_macro).name} $(input_dst) tree-$(ClusterId)-$(Process).root {args.events} {args.dbtag} {manager.output_dir}/output"
    manager.write_submit_file(arguments=arguments)
    manager.finalize_submission(queue_arg="input_dst from jobs.list")

def setup_f4a_subparsers(subparsers):
    f4a = subparsers.add_parser('f4a', parents=[get_common_parser()], help='Create condor submission directory.')
    f4a.add_argument('-i2_calib', '--calib', type=str, default=None, help='Q Vector Calibrations. (Optional)')
    f4a.add_argument('-i3_calib', '--eta-calib-path', '--eta-calib-direct-path', dest='eta_calib_path', type=str, default=None, help='Direct path to eta-shape calibration file. (Optional)')
    f4a.add_argument('-el', '--event-list', dest='event_list', type=str, default=None, help='Direct path to event list file. (Optional)')
    f4a.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_sEPD.C', help='Fun4All Macro.')
    f4a.add_argument('-f5', '--calo-calib-macro', type=str, default='macros/Calo_Calib.C', help='Calo_Calib Macro.')
    f4a.add_argument('-f6', '--HIJetReco-macro', type=str, default='macros/HIJetReco.C', help='HIJetReco Macro.')
    f4a.add_argument('--do-flow', type=int, default=3, help='Flow modulation configuration for JetReco/QA (default=3)')
    f4a.add_argument('--do-rcone', action='store_true', default=False, help='Enable random cone validation.')
    f4a.add_argument('--do-mult', type=int, default=1, choices=[0, 1], help='Enable multiple subtraction (default=1).')
    f4a.add_argument('--do-neg-energy-threshold', type=int, default=1, choices=[0, 1], help='Enable negative energy threshold rejection (default=1).')
    f4a.add_argument('--neg-energy-threshold', type=float, default=-2.0, help='Negative energy threshold cut in GeV (default=-2.0).')
    f4a.set_defaults(memory=1.5, condor_script='scripts/genFun4All.sh', func=create_f4a_jobs)

    f4a_zdc = subparsers.add_parser('f4a_zdc', parents=[get_common_parser()], help='Create condor submission directory.')
    f4a_zdc.add_argument('-n3', '--log-interval', type=int, default=10000, help='Logging Event Frequency. Default: 10000.')
    f4a_zdc.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_ZDC.C', help='Fun4All Macro.')
    f4a_zdc.set_defaults(memory=0.5, condor_script='scripts/genFun4AllZDC.sh', func=create_f4a_zdc_jobs)

    f4a_mc = subparsers.add_parser('f4a_mc', parents=[get_common_parser()], help='Create condor submission directory.')
    f4a_mc.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_sEPD_MC.C', help='Fun4All Macro.')
    f4a_mc.set_defaults(dbtag='MDC2', dst_per_job=500, memory=0.5, condor_script='scripts/genFun4All_MC.sh', func=create_f4a_mc_jobs)

    f4a_data_mc = subparsers.add_parser('f4a_data_mc', parents=[get_common_parser()], help='Create condor submission directory.')
    f4a_data_mc.add_argument('-i2_calib', '--calib', type=str, default=None, help='Q Vector Calibrations. (Optional)')
    f4a_data_mc.add_argument('-c', '--jet-pt-min', type=float, default=10, help='Jet pT min cut. Default: 10 GeV.')
    f4a_data_mc.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_sEPD_DataMC.C', help='Fun4All Macro.')
    f4a_data_mc.add_argument('-f5', '--calo-calib-macro', type=str, default='macros/Calo_Calib.C', help='Calo_Calib Macro.')
    f4a_data_mc.add_argument('-f6', '--HIJetReco-macro', type=str, default='macros/HIJetReco.C', help='HIJetReco Macro.')
    f4a_data_mc.set_defaults(memory=0.5, condor_script='scripts/genFun4All_DataMC.sh', func=create_f4a_data_mc_jobs)

    f4a_nobkgsub = subparsers.add_parser('f4a_nobkgsub', parents=[get_common_parser()], help='Create condor submission directory for NoBkgSub.')
    f4a_nobkgsub.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_NoBkgSub.C', help='Fun4All Macro.')
    f4a_nobkgsub.add_argument('-f5', '--calo-calib-macro', type=str, default='macros/Calo_Calib.C', help='Calo_Calib Macro.')
    f4a_nobkgsub.add_argument('-f6', '--NoBkgdSubJetReco-macro', type=str, default='macros/NoBkgdSubJetReco.C', help='NoBkgdSubJetReco Macro.')
    f4a_nobkgsub.set_defaults(memory=1.5, condor_script='scripts/genFun4All_NoBkgSub.sh', func=create_f4a_nobkgsub_jobs)

    f4a_noise = subparsers.add_parser('f4a_noise', parents=[get_common_parser()], help='Create condor submission directory for Noise.')
    f4a_noise.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_Noise.C', help='Fun4All Macro.')
    f4a_noise.set_defaults(memory=1.5, condor_script='scripts/genFun4All_Noise.sh', func=create_f4a_noise_jobs)

    f4a_calofittingqa = subparsers.add_parser('f4a_calofittingqa', parents=[get_common_parser()], help='Create condor submission directory for CaloFittingQA.')
    f4a_calofittingqa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_sEPD_CaloFittingQA.C', help='Fun4All Macro.')
    f4a_calofittingqa.set_defaults(memory=1.5, condor_script='scripts/genFun4All_CaloFittingQA.sh', func=create_f4a_calofittingqa_jobs)

    f4a_sepdqa = subparsers.add_parser('f4a_sepdqa', parents=[get_common_parser()], help='Create condor submission directory for sEPDQA.')
    f4a_sepdqa.add_argument('-f', '--f4a-macro', type=str, default='macros/Fun4All_sEPDQA.C', help='Fun4All Macro.')
    f4a_sepdqa.add_argument('-f5', '--calo-calib-macro', type=str, default='macros/Calo_Calib.C', help='Calo_Calib Macro.')
    f4a_sepdqa.set_defaults(memory=1.5, condor_script='scripts/genFun4All_sEPDQA.sh', func=create_f4a_sepdqa_jobs)
