import sys
import shutil
import datetime
from pathlib import Path
import logging

from condor_utils.core.logging import setup_logging
from condor_utils.core.helpers import run_command_and_log, get_line_count, count_total_lines, sort_files_by_lines

def setup_data(args):
    zdc_calib_dir = Path(args.zdc_calib_dir).resolve() if args.zdc_calib_dir else None
    zdc_calib_prefix = args.zdc_calib_prefix
    tag        = args.tag
    output_dir = Path(args.output_dir).resolve()
    merge_script = Path(args.merge_script).resolve()
    log_file = output_dir / 'log.txt'

    output_dir.mkdir(parents=True, exist_ok=True)

    if log_file.is_file():
        try:
            log_file.unlink()
            print(f"File '{log_file}' successfully removed.")
        except OSError as e:
            print(f"Error: {e.strerror}")

    logger = setup_logging(log_file, logging.DEBUG)

    if not merge_script.is_file():
        logger.critical(f'File: {merge_script} does not exist!')
        sys.exit(1)

    if zdc_calib_dir and not zdc_calib_dir.is_dir():
        logger.critical(f'Dir: {zdc_calib_dir} does not exist!')
        sys.exit(1)

    logger.info('#'*40)
    logger.info(f'LOGGING: {datetime.datetime.now()}')
    if zdc_calib_dir:
        logger.info(f'ZDC Calib Dir: {zdc_calib_dir}')
        logger.info(f'ZDC Calib Prefix: {zdc_calib_prefix}')
    logger.info(f'tag: {tag}')
    logger.info(f'Output Directory: {output_dir}')
    logger.info(f'Log File: {log_file}')
    logger.info('#'*40)

    run3auau_time_min_5 = output_dir / 'run3auau-time-min-5.list'
    current_runs = get_line_count(run3auau_time_min_5)

    command = """psql -h sphnxdaqdbreplica daq -c "select runnumber from run where runtype = 'physics' and runnumber between 66457 and 78954 and ertimestamp-brtimestamp > interval '5 minutes' order by runnumber;" -At > run3auau-time-min-5.list"""
    run_command_and_log(command, logger, output_dir, False)

    new_runs = get_line_count(run3auau_time_min_5)
    diff_runs_frac = (new_runs-current_runs)*100/current_runs if current_runs != 0 else 0

    logger.info(f'Runs: Before: {current_runs}, After: {new_runs}, Change: {diff_runs_frac:.2f} %')
    logger.info(f'Path: {run3auau_time_min_5}')

    dst_dir = output_dir / f'run3auau-{tag}'
    dst_dir_merged = output_dir / f'run3auau-merged-{tag}'

    current_segments = 0
    if dst_dir_merged.is_dir():
        try:
            current_segments = count_total_lines(dst_dir_merged)
            shutil.rmtree(dst_dir_merged)
            shutil.rmtree(dst_dir)
            logger.info(f"Directory '{dst_dir_merged}' successfully deleted.")
        except OSError as e:
            logger.critical(f"Error: {dst_dir_merged} : {e.strerror}")
            sys.exit(1)

    dst_dir.mkdir(parents=True, exist_ok=True)
    dst_dir_merged.mkdir(parents=True, exist_ok=True)

    command = f'CreateDstList.pl --tag {tag} --list ../run3auau-time-min-5.list DST_CALOFITTING DST_ZDC_RAW DST_SEPD_RAW'
    run_command_and_log(command, logger, dst_dir)

    command = f'{merge_script} {dst_dir} {dst_dir} {dst_dir} {dst_dir_merged}'
    run_command_and_log(command, logger, output_dir)

    if zdc_calib_dir:
        dst_dir_zdc_calib_merged = output_dir / f'run3auau-merged-zdc-calib-{tag}'
        shutil.rmtree(dst_dir_zdc_calib_merged)
        dst_dir_zdc_calib_merged.mkdir(parents=True, exist_ok=True)
        command = f'{merge_script} {dst_dir} {zdc_calib_dir} {dst_dir} {dst_dir_zdc_calib_merged} {zdc_calib_prefix}'
        run_command_and_log(command, logger, output_dir)

    new_segments = count_total_lines(dst_dir_merged)
    diff_segments_frac = (new_segments-current_segments)*100/current_segments if current_segments != 0 else 0
    logger.info(f'Total Segments: Before: {current_segments}, After: {new_segments}, Change: {diff_segments_frac:.2f} %')

    run3auau_dsts_time_min_5 = output_dir / f'run3auau-{tag}-time-min-5.list'
    current_runs_dsts = get_line_count(run3auau_dsts_time_min_5)

    command = f"ls run3auau-merged-{tag} | cut -d- -f2 | cut -d. -f1 | awk '{{x=$0+0;print x}}' | sort | uniq > run3auau-{tag}-time-min-5.list"
    run_command_and_log(command, logger, output_dir, False)

    new_runs_dsts = get_line_count(run3auau_dsts_time_min_5)
    diff_runs_frac = (new_runs_dsts-current_runs_dsts)*100/current_runs_dsts if current_runs_dsts != 0 else 0

    logger.info(f'Runs: Before: {current_runs_dsts}, After: {new_runs_dsts}, Change: {diff_runs_frac:.2f} %')
    logger.info(f'Path: {run3auau_dsts_time_min_5}')

    sort_files_by_lines(dst_dir_merged, output_dir / f'run3auau-{tag}.list')

    command = f"sed -E 's/.*-0*([1-9][0-9]*)\\.list$/\\1/' run3auau-{tag}.list > runs.list"
    run_command_and_log(command, logger, output_dir, False)


def setup_data_subparsers(subparsers):
    data_parser = subparsers.add_parser('data', help='Update file lists.')
    data_parser.add_argument('-i', '--zdc-calib-dir', type=str, default=None, help='ZDC Calib DST Dir. Default: None (optional)')
    data_parser.add_argument('-i2', '--zdc-calib-prefix', type=str, default='dst_zdc_calib', help='ZDC Calib DST List Prefix.')
    data_parser.add_argument('-t', '--tag', type=str, default='pro001_pcdb001_v001', help='Tag.')
    data_parser.add_argument('-o', '--output-dir', type=str, default='files/run3auau', help='Output Directory.')
    data_parser.add_argument('-s', '--merge-script', type=str, default='scripts/merge_lists.sh', help='Merge Lists Script.')
    data_parser.set_defaults(func=setup_data)
