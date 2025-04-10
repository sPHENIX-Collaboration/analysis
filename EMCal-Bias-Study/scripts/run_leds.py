#!/usr/bin/env python3
"""
This module handles running and controlling the EMCal LEDs.
Automatically records LED runs over various specified bias offset and LED pulse width ranges.
"""
import subprocess
import argparse
import os
import datetime
import sys
import numpy as np

parser = argparse.ArgumentParser()

parser.add_argument('-i'
                    , '--bias-var-dir', type=str
                    , default='/home/phnxrc/haggerty/emcal/apurva/03-25-25-bias'
                    , help='Bias Offset Variation Directory')

parser.add_argument('-l'
                    , '--log-dir', type=str
                    , default='/home/phnxrc/anarde/log'
                    , help='Log Directory. Default: /home/phnxrc/anarde/log')

parser.add_argument('-n'
                    , '--nevents', type=int
                    , default=1000
                    , help='Minimum events per Run. Default: 1000')

parser.add_argument('-p1'
                    , '--tp-start', type=int
                    , default=26, choices=range(10,51)
                    , help='LED Pulse Width (Start). Default: 26 ns')

parser.add_argument('-p2'
                    , '--tp-end', type=int
                    , default=32, choices=range(10,51)
                    , help='LED Pulse Width (End). Default: 32 ns')

parser.add_argument('-b1'
                    , '--bias-start', type=int
                    , default=-2000
                    , help='Bias Offset (Start). Default: -2000 mV')

parser.add_argument('-b2'
                    , '--bias-end', type=int
                    , default=2000
                    , help='Bias Offset (End). Default: 2000 mV')
parser.add_argument('-bs'
                    , '--bias-step', type=int
                    , default=100
                    , help='Bias Offset Step. Default: 100 mV')

parser.add_argument('-d'
                    , '--dry-run', action='store_true'
                    , help='Enable Dry Run.')

parser.add_argument('-a'
                    , '--record-default', action='store_false'
                    , help='Disable recording run with default offsets.')

args = parser.parse_args()

def tee_print(message, filename):
    """Prints a message to stdout and appends it to a file."""
    print(message)  # Print to stdout
    try:
        with open(filename, "a", encoding="utf-8") as f:  # Open file in append mode
            f.write(message + "\n")  # Write to file with a newline
    except OSError as e:
        print(f"Error writing to file {filename}: {e}")

def execute_command(local_command, local_dry_run):
    """Executes a command and exits when an error is encountered."""
    if local_dry_run:
        print(f'COMMAND: {local_command}')
    else:
        result = subprocess.run(['bash','-c',local_command], check=False)
        if result.returncode != 0:
            print(f'Error in {local_command}')
            sys.exit()

if __name__ == '__main__':
    bias_var_dir   = os.path.realpath(args.bias_var_dir)
    dry_run        = args.dry_run
    CURRENT_DATE   = str(datetime.date.today())
    log_dir        = os.path.realpath(args.log_dir) + '/' + CURRENT_DATE
    log_file       = f'{log_dir}/log.txt'
    nevents        = args.nevents
    run_time       = nevents // 100
    tp_start       = args.tp_start
    tp_end         = args.tp_end
    bias_start     = args.bias_start
    bias_end       = args.bias_end
    bias_step      = args.bias_step
    record_default = args.record_default

    ### Defaults START -- DO NOT CHANGE ###
    BIAS_MIN = -2000         # mV
    BIAS_MAX = 2000          # mV
    BIAS_STEP_DEFAULT = -100 # mV
    ### Defaults END   -- DO NOT CHANGE ###

    # Ensure the starting pulse width is not greater than ending pulse width
    if tp_start > tp_end:
        parser.error(f'LED pulse width start: {tp_start} ns is greater than end: {tp_end} ns.')

    # Ensure that bias offsets are within acceptable range
    if bias_start < BIAS_MIN or bias_end > BIAS_MAX:
        parser.error('Bounds for bias range exceed default.'
        f' Bias Min: {BIAS_MIN} mV and Max: {BIAS_MAX} mV.')

    # Ensure that the beginning isn't bigger than the end.
    # Leads to an empty range of bias offsets.
    if bias_start > bias_end:
        parser.error(f'Bias Start: {bias_start} mV > End: {bias_end} mV.')

    # Ensure that the bias step is a multiple of the default bias step.
    if bias_step % BIAS_STEP_DEFAULT != 0:
        parser.error(f'Bias step is not a multiple of {BIAS_STEP_DEFAULT}.')

    os.makedirs(log_dir,exist_ok=True)

    tee_print('##########################', log_file)
    tee_print(f'LOGGING: {str(datetime.datetime.now())}', log_file)
    tee_print(f'Bias Var Dir: {bias_var_dir}', log_file)
    tee_print(f'Log Dir: {log_dir}', log_file)
    tee_print(f'Minimum events per Run: {nevents}', log_file)
    tee_print(f'Recording time per Run: {run_time} seconds', log_file)
    tee_print(f'Bias Offset Start: {bias_start} mV', log_file)
    tee_print(f'Bias Offset End: {bias_end} mV', log_file)
    tee_print(f'Bias Offset Step: {bias_step} mV', log_file)
    tee_print(f'LED pulse width Start: {tp_start} ns', log_file)
    tee_print(f'LED pulse width End: {tp_end} ns', log_file)
    tee_print(f'Record Default: {record_default}', log_file)
    tee_print(f'Dry Run: {dry_run}\n', log_file)

    # configure DAQ
    # RC setup
    COMMAND = '~/samfred/quickscripts/rc_setup_local.sh 5 seb{00..15}'
    execute_command(COMMAND,dry_run)
    tee_print('rc local setup done', log_file)

    # GTM setup
    COMMAND = ('gl1_gtm_client gtm_set_mode 5 0 && '
               'gl1_gtm_client gtm_load_modebits 5 '
               '/home/phnxrc/operations/gl1_gtm/EMCAL_pulse_100Hz.scheduler')
    execute_command(COMMAND,dry_run)
    tee_print('gl1_gtm setup done', log_file)

    # Set Data Type to LED
    COMMAND = 'rc_client rc_set_runtype led'
    execute_command(COMMAND,dry_run)
    tee_print('rc local setup done\n', log_file)

    # Configure the loop over bias offsets
    bias_vec = ((np.arange(bias_start,bias_end+1,bias_step)-BIAS_MAX)//BIAS_STEP_DEFAULT).tolist()
    bias_tuple = list(zip(bias_vec, [f'{bias_var_dir}/var-{var}' for var in bias_vec]))

    # Add the default offset to the list of run configurations if not disabled explicity
    if record_default:
        bias_tuple = [('original','/home/phnxrc/haggerty/emcal/vop-1008')] + bias_tuple

    # loop over bias offsets
    for item in bias_tuple:
        bias = BIAS_MAX + item[0] * BIAS_STEP_DEFAULT if isinstance(item[0],int) else item[0]
        bias_dir = item[1]

        command = (f'ln -sfn {bias_dir} /home/phnxrc/haggerty/emcal/config && '
                    '~/haggerty/emcal/calcon/biasallsectorsfast.py')
        execute_command(command, dry_run)
        tee_print(f'EMCal Configured with bias offsets {bias} mV', log_file)

        # loop over LED pulse widths
        for tp in range(tp_start,tp_end+1):
            command = f'ssh opc0 "python3 ~/haggerty/emcal/calcon/ledtest.py {tp}"'
            execute_command(command,dry_run)
            tee_print(f'LEDs configured for pulse width = {tp} ns', log_file)

            ### START RUN ###
            command = (
                        # START RUN
                        f'rc_client rc_begin -i && '
                        # wait for a bit
                        'sleep 2 && '
                        # record the run number information to LOG
                        f'rc_client rc_status > {log_dir}/log_tp_{tp}_bias_{bias}.txt && '
                        # wait until sufficient events have been recorded
                        f'sleep {run_time} && '
                        # END RUN
                        'rc_client rc_end -i && '
                        # wait for a bit before starting again
                        'sleep 5'
                      )

            execute_command(command, dry_run)
            tee_print('Run Recorded\n', log_file)
            ### END RUN ###

    ### CLEAN UP ###
    BIAS_DIR = '/home/phnxrc/haggerty/emcal/vop-1008'
    command = (f'ln -sfn {BIAS_DIR} /home/phnxrc/haggerty/emcal/config && '
                '~/haggerty/emcal/calcon/biasallsectorsfast.py && '
                'sleep 2 && '
                'rc_client rc_shutdown')
    execute_command(command, dry_run)
    tee_print('EMCal Configured with default offsets and shutdown rc_client', log_file)
