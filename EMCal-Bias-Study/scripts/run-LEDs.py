#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil
import datetime

parser = argparse.ArgumentParser()

parser.add_argument('-i', '--bias-var-dir', type=str, default='/home/phnxrc/haggerty/emcal/apurva/03-25-25-bias', help='Bias Offset Variation Directory')
parser.add_argument('-l', '--log-dir', type=str, default='/home/phnxrc/anarde/log', help='Log Directory. Default: /home/phnxrc/anarde/log')
parser.add_argument('-n', '--nevents', type=int, default=1000, help='Minimum events per Run. Default: 1000')
parser.add_argument('-p1', '--tp-start', type=int, default=26, choices=range(10,51), help='LED Pulse Width (Start). Default: 26 ns')
parser.add_argument('-p2', '--tp-end', type=int, default=32, choices=range(10,51), help='LED Pulse Width (End). Default: 32 ns')
parser.add_argument('-d', '--dry-run', action='store_true', help='Enable Dry Run.')

args = parser.parse_args()

def tee_print(message, filename):
    """Prints a message to stdout and appends it to a file."""
    print(message)  # Print to stdout
    try:
        with open(filename, "a") as f:  # Open file in append mode
            f.write(message + "\n")  # Write to file with a newline
    except Exception as e:
        print(f"Error writing to file {filename}: {e}")

def execute_command(command, dry_run):
    if(dry_run):
        print(f'COMMAND: {command}')
    else:
        result = subprocess.run(['bash','-c',command])
        if(result.returncode != 0):
            print(f'Error in {command}')
            exit

if __name__ == '__main__':
    bias_var_dir = os.path.realpath(args.bias_var_dir)
    dry_run      = args.dry_run
    current_date = str(datetime.date.today())
    log_dir      = os.path.realpath(args.log_dir) + '/' + current_date
    log_file     = f'{log_dir}/log.txt'
    nevents      = args.nevents
    run_time     = nevents // 100
    tp_start     = args.tp_start
    tp_end       = args.tp_end

    # Ensure the starting pulse width is not greater than ending pulse width
    if(tp_start > tp_end):
        parser.error(f'LED pulse width start: {tp_start} ns is greater than end: {tp_end} ns.')

    os.makedirs(log_dir,exist_ok=True)

    tee_print(f'##########################', log_file)
    tee_print(f'LOGGING: {str(datetime.datetime.now())}', log_file)
    tee_print(f'Bias Var Dir: {bias_var_dir}', log_file)
    tee_print(f'Log Dir: {log_dir}', log_file)
    tee_print(f'Minimum events per Run: {nevents}', log_file)
    tee_print(f'Recording time per Run: {run_time} seconds', log_file)
    tee_print(f'Dry Run: {dry_run}\n', log_file)

    # configure DAQ
    # RC setup
    command = f'~/samfred/quickscripts/rc_setup_local.sh 5 seb{{00..15}}'
    execute_command(command,dry_run)
    tee_print('rc local setup done', log_file)

    # GTM setup
    command = (f'gl1_gtm_client gtm_set_mode 5 0 && '
                'gl1_gtm_client gtm_load_modebits 5 /home/phnxrc/operations/gl1_gtm/EMCAL_pulse_100Hz.scheduler')
    execute_command(command,dry_run)
    tee_print('gl1_gtm setup done', log_file)

    # Set Data Type to LED
    command = f'rc_client rc_set_runtype led'
    execute_command(command,dry_run)
    tee_print('rc local setup done\n', log_file)

    bias_step     = 100 # mV DO NOT CHANGE
    bias_var_step = 500 # mV [Can be set to any multiples of 100 mV]
    bias_dir_step = -bias_var_step // bias_step

    bias_low  = -2000
    var_start = 40 # corresponds to 2000 mV
    var_end   = 0  # corresponds to -2000 mV

    bias_tuple = list(zip(range(var_start,var_end-1,bias_dir_step), [f'{bias_var_dir}/var-{var}' for var in range(var_start,var_end-1,bias_dir_step)]))
    bias_tuple = [('original','/home/phnxrc/haggerty/emcal/vop-1008')] + bias_tuple

    # loop over bias offsets
    for item in bias_tuple:
        bias = bias_low + (var_start - item[0]) * bias_step if isinstance(item[0],int) else item[0]
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
            command = (f'rc_client rc_begin -i && '                                       # START RUN
                        'sleep 2 && '                                                     # wait for a bit
                       f'rc_client rc_status > {log_dir}/log_tp_{tp}_bias_{bias}.txt && ' # record the run number information to LOG
                       f'sleep {run_time} && '                                            # wait until sufficient events have been recorded
                        'rc_client rc_end -i && '                                         # END RUN
                        'sleep 5')                                                        # wait for a bit before starting again

            execute_command(command, dry_run)
            tee_print(f'Run Recorded\n', log_file)
            ### END RUN ###

    ### CLEAN UP ###
    bias_dir = '/home/phnxrc/haggerty/emcal/vop-1008'
    command = (f'ln -sfn {bias_dir} /home/phnxrc/haggerty/emcal/config && '
                '~/haggerty/emcal/calcon/biasallsectorsfast.py && '
                'sleep 2 && '
                'rc_client rc_shutdown')
    execute_command(command, dry_run)
    tee_print(f'EMCal Configured with default offsets and shutdown rc_client', log_file)
