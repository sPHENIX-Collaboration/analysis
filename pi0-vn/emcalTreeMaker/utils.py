#!/usr/bin/env python3
import pandas as pd
import numpy as np
import subprocess
import argparse
import os
import shutil

parser = argparse.ArgumentParser()
subparser = parser.add_subparsers(dest='command')

f4a = subparser.add_parser('f4a', help='Create condor submission directory for Fun4All_CaloTreeGen.')

f4a.add_argument('-i', '--run-list-dir', type=str, help='Directory of run lists', required=True)
f4a.add_argument('-e', '--executable', type=str, default='genFun4All.sh', help='Job script to execute. Default: scripts/genFun4All.sh')
f4a.add_argument('-m', '--macro', type=str, default='macro/Fun4All_CaloTreeGen.C', help='Fun4All macro. Default: macro/Fun4All_CaloTreeGen.C')
f4a.add_argument('-m2', '--src', type=str, default='src', help='Directory Containing src files. Default: src')
f4a.add_argument('-b', '--f4a', type=str, default='bin/Fun4All_CaloTreeGen', help='Fun4All executable. Default: bin/Fun4All_CaloTreeGen')
f4a.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
f4a.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
f4a.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')
f4a.add_argument('-z', '--vtx-z', type=float, default=10, help='Event z-vertex cut. Default: 10 [cm]')
f4a.add_argument('-e2', '--clus-e-min', type=float, default=1, help='Minimum Cluster ECore. Default: 1 [GeV]')
f4a.add_argument('-c', '--clus-chi-max', type=float, default=4, help='Maximum Cluster Chi Squared. Default: 4')
f4a.add_argument('-a', '--do-pi0', type=int, default=1, help='Do pi0 Analysis. Default: True')
f4a.add_argument('-s2', '--isSim', type=int, default=0, help='Type Simulation. Default: False')
f4a.add_argument('-y', '--systematics', type=str, default='none', help='Systematics. Default: none')
f4a.add_argument('-z1', '--useZDCInfo', type=int, default=0, help='Use ZDC Info for MB classifier. Default: False')
f4a.add_argument('-z2', '--zdc-cut', type=float, default=40, help='Minimum ZDC Energy. Default: 40 [GeV]')

pi0Ana = subparser.add_parser('pi0Ana', help='Create condor submission directory for pi0Analysis.')

pi0Ana.add_argument('-i', '--ntp-list', type=str, help='List of Ntuples', required=True)
pi0Ana.add_argument('-c', '--cuts', type=str, help='List of cuts', required=True)
pi0Ana.add_argument('-a', '--anaType', type=int, default=1, help='Analysis Type. Default: 1.')
pi0Ana.add_argument('-p', '--do-vn-calc', type=int, default=0, help='Do vn analysis. Default: False')
pi0Ana.add_argument('-c2', '--csv', type=str, default="", help='CSV file with fitStats. Default: ""')
pi0Ana.add_argument('-c3', '--Q-vec-corr', type=str, default="", help='CSV file with Q vector corrections. Default: ""')
pi0Ana.add_argument('-z', '--vtx-z', type=float, default=10, help='Event z-vertex cut. Default: 10 [cm]')
pi0Ana.add_argument('-m', '--macro', type=str, default='macro/pi0Analysis.C', help='pi0Analysis macro. Default: macro/pi0Analysis.C')
pi0Ana.add_argument('-e', '--script', type=str, default='genPi0Ana.sh', help='Job script to execute. Default: genPi0Ana.sh')
pi0Ana.add_argument('-b', '--executable', type=str, default='bin/pi0Ana', help='Executable. Default: bin/pi0Ana')
pi0Ana.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
pi0Ana.add_argument('-u', '--subsamples', type=int, default=30, help='Number of subsamples for vn analysis. Default: 30')
pi0Ana.add_argument('-t', '--cut-num', type=int, default=0, help='Specific cut to use for vn analysis. Default: 0')
pi0Ana.add_argument('-g', '--sigma', type=float, default=2, help='Signal Window size in units of sigma. Default: 2')
pi0Ana.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
pi0Ana.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

truthPi0Ana = subparser.add_parser('truthPi0Ana', help='Create condor submission directory for truthPi0Analysis.')

truthPi0Ana.add_argument('-i', '--ntp-list', type=str, help='List of Ntuples', required=True)
truthPi0Ana.add_argument('-c', '--Q-vec-corr', type=str, help='CSV file with Q vector corrections.', required=True)
truthPi0Ana.add_argument('-a', '--anaType', type=int, default=1, help='Analysis Type. Default: 1.')
truthPi0Ana.add_argument('-z', '--vtx-z', type=float, default=10, help='Event z-vertex cut. Default: 10 [cm]')
truthPi0Ana.add_argument('-m', '--macro', type=str, default='macro/truthPi0Analysis.C', help='pi0Analysis macro. Default: macro/truthPi0Analysis.C')
truthPi0Ana.add_argument('-e', '--script', type=str, default='genTruthPi0Ana.sh', help='Job script to execute. Default: genTruthPi0Ana.sh')
truthPi0Ana.add_argument('-b', '--executable', type=str, default='bin/truthPi0Ana', help='Executable. Default: bin/truthPi0Ana')
truthPi0Ana.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
truthPi0Ana.add_argument('-u', '--subsamples', type=int, default=30, help='Number of subsamples for vn analysis. Default: 30')
truthPi0Ana.add_argument('-s', '--memory', type=float, default=0.5, help='Memory (units of GB) to request per condor submission. Default: 0.5 GB.')
truthPi0Ana.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

mix = subparser.add_parser('mix', help='Create condor submission directory for mixed event vn.')

mix.add_argument('-i', '--ntp-list', type=str, help='List of Ntuples', required=True)
mix.add_argument('-c', '--cuts', type=str, default='cuts.txt', help='List of cuts. Default: cuts.txt')
mix.add_argument('-c2', '--Q-vec-corr', type=str, default='Q-vec-corr-z-10.csv', help='CSV file with Q vector corrections. Default: Q-vec-corr-z-10.csv')
mix.add_argument('-m', '--macro', type=str, default='macro/mixedEvent.C', help='Mixed Event macro. Default: macro/mixedEvent.C')
mix.add_argument('-e', '--script', type=str, default='genMixedEvent.sh', help='Job script to execute. Default: genMixedEvent.sh')
mix.add_argument('-b', '--executable', type=str, default='bin/mixedEvent', help='Executable. Default: bin/mixedEvent')
mix.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
mix.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
mix.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

QVecCorr = subparser.add_parser('QVecCorr', help='Create condor submission directory for Q Vector Correction.')

QVecCorr.add_argument('-i', '--ntp-list', type=str, help='List of Ntuples', required=True)
QVecCorr.add_argument('-m', '--macro', type=str, default='macro/Q-vector-correction.C', help='Q Vector Correction macro. Default: macro/Q-vector-correction.C')
QVecCorr.add_argument('-z', '--vtx-z', type=float, default=10, help='Event z-vertex cut. Default: 10 [cm]')
QVecCorr.add_argument('-e', '--script', type=str, default='genQVecCorr.sh', help='Job script to execute. Default: genQVecCorr.sh')
QVecCorr.add_argument('-b', '--executable', type=str, default='bin/Q-vec-corr', help='Executable. Default: bin/Q-vec-corr')
QVecCorr.add_argument('-d', '--output', type=str, default='test', help='Output Directory. Default: ./test')
QVecCorr.add_argument('-s', '--memory', type=float, default=1, help='Memory (units of GB) to request per condor submission. Default: 1 GB.')
QVecCorr.add_argument('-l', '--log', type=str, default='/tmp/anarde/dump/job-$(ClusterId)-$(Process).log', help='Condor log file.')

dummy = subparser.add_parser('dummy', help='Identify events per run from input text file.')
dummy.add_argument('-i', '--entries', type=str, help='Text file of all runs and events', required=True)

dummy2 = subparser.add_parser('dummy2', help='Create pi0Ana submission command for multiple runs.')
dummy2.add_argument('-i', '--fp', type=str, help='Text file of all runs and events', required=True)
dummy2.add_argument('-d', '--directory', type=str, default='.', help='output directory. Default: .')
dummy2.add_argument('-e', '--input-dir', type=str, default='.', help='Directory containing the <run>.list files. Default: .')
dummy2.add_argument('-j', '--jobs', type=str, default=999, help='Number of jobs. Default: 999')

sigmaCheck = subparser.add_parser('sigmaCheck', help='Ensure sigma is increasing across the different types.')
sigmaCheck.add_argument('-i', '--fit-stats', type=str, help='List of Fit Stats', required=True)

vn = subparser.add_parser('vn', help='vn Combine')
vn.add_argument('-i', '--vn-files', type=str, help='List of vn files', required=True)

systematics = subparser.add_parser('systematics', help='Systematics Calculations')
systematics.add_argument('-i', '--files', type=str, help='List of vn files', required=True)
systematics.add_argument('-o', '--output', type=str, default='.', help='Output directory for the systematics csv. Default: .')

args = parser.parse_args()

def process_dummy2():
    fp        = os.path.realpath(args.fp)
    input_dir = os.path.realpath(args.input_dir)
    output    = args.directory
    jobs      = args.jobs

    print(f'Input file: {fp}')
    print(f'Input dir: {input_dir}')
    print(f'Output Directory: {output}')
    print(f'Jobs: {jobs}')

    with open(fp) as file:
        for line in file:
            print(f'./utils.py pi0Ana -i {input_dir}/{line.split()[0]}.list -c cuts.txt -n {line.split()[1]} -j {jobs} -d {output}/{line.split()[0]} && ', end='')

def process_dummy():
    entries = os.path.realpath(args.entries)

    print(f'Input file: {entries}')

    df = pd.read_csv(entries, delimiter=':', names=['run','entries'])
    df.run = df.run.apply(lambda x: x.split('/')[6])
    print(df.groupby(by='run').sum())

def process_sigmaCheck():
    fitStats = os.path.realpath(args.fit_stats)

    print(f'fitStats : {fitStats}')

    with open(fitStats) as file:
        fit_type = 'A'
        # Create an empty dataframe
        df = pd.DataFrame()

        ctr = 0

        for line in file:
            line = line.rstrip() # remove \n from the end of the string
            print(f'type: {fit_type}, {line}')

            df1 = pd.read_csv(line,usecols=['GaussSigma'])

            df[fit_type] = df1

            fit_type = chr(ord(fit_type)+1)

            if(ctr):
                print(df[df.iloc[:,ctr] < df.iloc[:,ctr-1]].iloc[:,[ctr-1,ctr]])

            ctr += 1

def process_systematics():
    files  = os.path.realpath(args.files)
    output = args.output

    print(f'input: {files}')
    print(f'outputDir: {output}')

    df = pd.read_csv(files)

    df_vn  = pd.DataFrame()
    df_rel = pd.DataFrame()
    df_abs = pd.DataFrame()

    # load each vn into a df
    for index, row in df.iterrows():
        # add v2 column to the df
        df_vn[row['tag']] = pd.read_csv(row['file'], usecols=['v2'])

        # add reference values
        # add v2 bg column
        if(row['tag'] == 'reference'):
            df_rel['v2']    = df_vn['reference']
            df_abs['v2']    = df_vn['reference']
            df_vn['bg-0.4'] = pd.read_csv(row['file'], usecols=['v2_type_4'])
            continue

        # relative uncertainty
        df_rel[row['tag']] = ((df_vn['reference']-df_vn[row['tag']])/df_vn['reference']).abs()

        # absolute uncertainty
        df_abs[row['tag']] = df_rel[row['tag']]*df_vn['reference'].abs()

    df_vn = df_vn[['reference','SYST1CEMC','SYST2CEMC','SYST3DCEMC','SYST3UCEMC','SYST4CEMC','sigma-1.5','bg-0.4']]

    # background systematic
    df_rel['bg-0.4'] = ((df_vn['reference']-df_vn['bg-0.4'])/df_vn['reference']).abs()
    df_abs['bg-0.4'] = df_rel['bg-0.4']*df_vn['reference'].abs()

    # compute total EMCal systematic by adding each column in quadrature
    df_rel['EMCal'] = df_rel.pow(2).iloc[:,1:6].sum(1).pow(0.5)
    df_abs['EMCal'] = df_abs.pow(2).iloc[:,1:6].sum(1).pow(0.5)

    # compute total systematic by adding each column in quadrature
    df_rel['total'] = df_rel.pow(2).iloc[:,1:8].sum(1).pow(0.5)
    df_abs['total'] = df_abs.pow(2).iloc[:,1:8].sum(1).pow(0.5)

    print('v2')
    print(df_vn)

    print('Relative')
    print(df_rel)

    print('Absolute')
    print(df_abs)

    # write dataframes to csv
    df_vn.to_csv(f'{output}/vn.csv', index=False)
    df_rel.round(5).to_csv(f'{output}/vn-relative.csv', index=False)
    df_abs.round(5).to_csv(f'{output}/vn-absolute.csv', index=False)

def process_vn():
    vn_files = os.path.realpath(args.vn_files)

    print(f'vn : {vn_files}')

    indices = 18

    vn_err_array = [9999]*indices
    vn_val_array = [0]*indices
    vn_type      = ['A']*indices

    with open(vn_files) as file:
        fit_type = 'A'

        for line in file:
            line = line.rstrip() # remove \n from the end of the string

            with open(line) as file2:

                ctr = 0
                for line2 in file2:
                    if(ctr == 0):
                        ctr += 1
                        continue

                    ctr += 1

                    line2 = line2.rstrip() # remove \n from the end of the string
                    vn_idx = int(line2.split(',')[0])
                    vn_val = float(line2.split(',')[1])
                    vn_err = float(line2.split(',')[2])

                    if(vn_err < vn_err_array[vn_idx]):
                       vn_err_array[vn_idx] = vn_err
                       vn_val_array[vn_idx] = vn_val
                       vn_type[vn_idx]    = fit_type

                       # print(vn_idx, vn_val, vn_err)

            fit_type = chr(ord(fit_type)+1)

    print('Type,v2,v2_err')
    for i in range(indices):
        print(f'{vn_type[i]},{vn_val_array[i]},{vn_err_array[i]}')


def create_f4a_jobs():
    run_list_dir = os.path.realpath(args.run_list_dir)
    output_dir   = os.path.realpath(args.output)
    f4a          = os.path.realpath(args.f4a)
    macro        = os.path.realpath(args.macro)
    src          = os.path.realpath(args.src)
    executable   = os.path.realpath(args.executable)
    memory       = args.memory
    log          = args.log
    do_pi0       = args.do_pi0
    z            = args.vtx_z
    isSim        = args.isSim
    clus_e       = args.clus_e_min
    clus_chi     = args.clus_chi_max
    systematics  = args.systematics
    useZDCInfo   = args.useZDCInfo
    zdc_cut      = args.zdc_cut

    print(f'Simulation : {isSim}')
    print(f'Fun4All : {macro}')
    print(f'src: {src}')
    print(f'Do pi0 Analysis: {do_pi0}')
    print(f'Vtx z max: {z} cm')
    print(f'Use ZDC Info: {useZDCInfo}')
    print(f'ZDC Cut: {zdc_cut} [GeV]')
    print(f'Cluster Minimum ECore: {clus_e} GeV')
    print(f'Cluster Maximum Chi2: {clus_chi} GeV')
    print(f'Run List Directory: {run_list_dir}')
    print(f'Systematics: {systematics}')
    print(f'Output Directory: {output_dir}')
    print(f'Bin: {f4a}')
    print(f'Executable: {executable}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(f4a, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copytree(src, f'{output_dir}/src', dirs_exist_ok=True)
    shutil.copy(executable, output_dir)

    for filename in os.listdir(run_list_dir):
        if(filename.endswith('list')):
            f = os.path.join(run_list_dir, filename)
            run = int(filename.split('-')[-1].split('.')[0])
            job_dir = f'{output_dir}/{run}'

            os.makedirs(job_dir,exist_ok=True)
            os.makedirs(f'{job_dir}/stdout',exist_ok=True)
            os.makedirs(f'{job_dir}/error',exist_ok=True)
            os.makedirs(f'{job_dir}/output',exist_ok=True)

            shutil.copy(f, job_dir)

            with open(f'{job_dir}/genFun4All.sub', mode="w") as file:
                file.write(f'executable     = ../{os.path.basename(executable)}\n')
                # simulation
                if(isSim):
                    file.write(f'arguments  = {output_dir}/{os.path.basename(f4a)} $(input_dst) output/qa-$(Process).root output/diphoton-$(Process).root {systematics} {do_pi0} {z} {useZDCInfo} {zdc_cut} {clus_e} {clus_chi} {isSim} $(input_global) $(input_mbd) $(input_g4hits)\n')
                # data
                else:
                    file.write(f'arguments  = {output_dir}/{os.path.basename(f4a)} $(input_dst) output/qa-$(Process).root output/diphoton-$(Process).root {systematics} {do_pi0} {z} {useZDCInfo} {zdc_cut} {clus_e} {clus_chi}\n')

                file.write(f'log            = {log}\n')
                file.write('output          = stdout/job-$(Process).out\n')
                file.write('error           = error/job-$(Process).err\n')
                file.write(f'request_memory = {memory}GB\n')
                file.write('concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n')
                if(isSim):
                    file.write(f'queue input_dst, input_global, input_mbd, input_g4hits from {filename}')
                else:
                    file.write(f'queue input_dst from {filename}')

            print(f'cd {job_dir} && condor_submit genFun4All.sub && ', end='')
    print()

def create_pi0Ana_jobs():
    ntp_list   = os.path.realpath(args.ntp_list)
    cuts       = os.path.realpath(args.cuts)
    macro      = os.path.realpath(args.macro)
    fitStats   = os.path.realpath(args.csv) if(args.csv != '') else ''
    Q_vec_corr = os.path.realpath(args.Q_vec_corr) if(args.Q_vec_corr != '') else ''
    script     = os.path.realpath(args.script)
    executable = os.path.realpath(args.executable)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    z          = args.vtx_z
    log        = args.log
    do_vn_calc = args.do_vn_calc
    samples    = args.subsamples
    cut_num    = args.cut_num
    sigma      = args.sigma
    anaType    = args.anaType

    print(f'Macro: {macro}')
    print(f'Run List: {ntp_list}')
    print(f'Cuts: {cuts}')
    print(f'Do vn calc: {do_vn_calc}')
    print(f'Samples: {samples}')
    print(f'Cut num: {cut_num}')
    print(f'sigma: {sigma}')
    print(f'FitStats: {fitStats}')
    print(f'Q Vector Correction: {Q_vec_corr}')
    print(f'Vtx z max: {z} cm')
    print(f'Script: {script}')
    print(f'Executable: {executable}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Analysis Type: {anaType}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(ntp_list, output_dir)
    shutil.copy(cuts, output_dir)
    shutil.copy(macro, output_dir)
    if(do_vn_calc):
        shutil.copy(fitStats, output_dir)
        shutil.copy(Q_vec_corr, output_dir)
        fitStats   = f'{output_dir}/{os.path.basename(fitStats)}'
        Q_vec_corr = f'{output_dir}/{os.path.basename(Q_vec_corr)}'

    cuts = f'{output_dir}/{os.path.basename(cuts)}'

    sub = ''
    with open(ntp_list) as file:
        for line in file:
            line = line.rstrip() # remove \n from the end of the string
            run = os.path.splitext(os.path.basename(line))[0] # extract the run number from the file name

            print(f'Run: {run}')
            os.makedirs(f'{output_dir}/{run}/stdout',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/error',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/output',exist_ok=True)

            shutil.copy(line, f'{output_dir}/{run}')

            with open(f'{output_dir}/{run}/genPi0Ana.sub', mode="w") as file2:
                file2.write(f'executable     = ../{os.path.basename(script)}\n')
                if(do_vn_calc):
                    file2.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {cuts} {z} output/test-$(Process).root {anaType} {do_vn_calc} {fitStats} {Q_vec_corr} {samples} {cut_num} {sigma}\n')
                else:
                    file2.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {cuts} {z} output/test-$(Process).root {anaType}\n')
                file2.write(f'log            = {log}\n')
                file2.write( 'output         = stdout/job-$(Process).out\n')
                file2.write( 'error          = error/job-$(Process).err\n')
                file2.write(f'request_memory = {memory}GB\n')
                file2.write('concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n')
                file2.write(f'queue input_ntp from {os.path.basename(line)}')

            sub = f'{sub} cd {output_dir}/{run} && condor_submit genPi0Ana.sub &&'

    # print condor submission command
    print(sub)

def create_truthPi0Ana_jobs():
    ntp_list   = os.path.realpath(args.ntp_list)
    macro      = os.path.realpath(args.macro)
    Q_vec_corr = os.path.realpath(args.Q_vec_corr)
    script     = os.path.realpath(args.script)
    executable = os.path.realpath(args.executable)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    z          = args.vtx_z
    log        = args.log
    samples    = args.subsamples
    anaType    = args.anaType

    print(f'Macro: {macro}')
    print(f'Run List: {ntp_list}')
    print(f'Samples: {samples}')
    print(f'Q Vector Correction: {Q_vec_corr}')
    print(f'Vtx z max: {z} cm')
    print(f'Script: {script}')
    print(f'Executable: {executable}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')
    print(f'Analysis Type: {anaType}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(ntp_list, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copy(Q_vec_corr, output_dir)
    Q_vec_corr = f'{output_dir}/{os.path.basename(Q_vec_corr)}'

    sub = ''
    with open(ntp_list) as file:
        for line in file:
            line = line.rstrip() # remove \n from the end of the string
            run = os.path.splitext(os.path.basename(line))[0] # extract the run number from the file name

            print(f'Run: {run}')
            os.makedirs(f'{output_dir}/{run}/stdout',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/error',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/output',exist_ok=True)

            shutil.copy(line, f'{output_dir}/{run}')

            with open(f'{output_dir}/{run}/genTruthPi0Ana.sub', mode="w") as file2:
                file2.write(f'executable     = ../{os.path.basename(script)}\n')
                file2.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {Q_vec_corr} {z} output/test-$(Process).root {anaType} {samples}\n')
                file2.write(f'log            = {log}\n')
                file2.write( 'output         = stdout/job-$(Process).out\n')
                file2.write( 'error          = error/job-$(Process).err\n')
                file2.write(f'request_memory = {memory}GB\n')
                file2.write('concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n')
                file2.write(f'queue input_ntp from {os.path.basename(line)}')

            sub = f'{sub} cd {output_dir}/{run} && condor_submit genTruthPi0Ana.sub &&'

    # print condor submission command
    print(sub)

def create_mixedEvent_jobs():
    ntp_list   = os.path.realpath(args.ntp_list)
    cuts       = os.path.realpath(args.cuts)
    macro      = os.path.realpath(args.macro)
    Q_vec_corr = os.path.realpath(args.Q_vec_corr)
    script     = os.path.realpath(args.script)
    executable = os.path.realpath(args.executable)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    log        = args.log

    print(f'Macro: {macro}')
    print(f'Run List: {ntp_list}')
    print(f'Cuts: {cuts}')
    print(f'Q Vector Correction: {Q_vec_corr}')
    print(f'Script: {script}')
    print(f'Executable: {executable}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(ntp_list, output_dir)
    shutil.copy(cuts, output_dir)
    shutil.copy(macro, output_dir)
    shutil.copy(Q_vec_corr, output_dir)

    cuts       = f'{output_dir}/{os.path.basename(cuts)}'
    Q_vec_corr = f'{output_dir}/{os.path.basename(Q_vec_corr)}'

    sub = ''
    with open(ntp_list) as file:
        for line in file:
            line = line.rstrip() # remove \n from the end of the string
            run = os.path.splitext(os.path.basename(line))[0] # extract the run number from the file name

            print(f'Run: {run}')
            os.makedirs(f'{output_dir}/{run}/stdout',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/error',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/output',exist_ok=True)

            # shutil.copy(line, f'{output_dir}/{run}')
            with open(f'{output_dir}/{run}/{run}.list', mode='w') as file2:
                file2.write(f'{line}\n')

            with open(f'{output_dir}/{run}/genMixedEvent.sub', mode="w") as file2:
                file2.write(f'executable     = ../{os.path.basename(script)}\n')
                file2.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {cuts} {Q_vec_corr} output/test.root\n')
                file2.write(f'log            = {log}\n')
                file2.write( 'output         = stdout/job.out\n')
                file2.write( 'error          = error/job.err\n')
                file2.write(f'request_memory = {memory}GB\n')
                file2.write('concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n')
                file2.write(f'queue input_ntp from {run}.list')

            sub = f'{sub} cd {output_dir}/{run} && condor_submit genMixedEvent.sub &&'

    # print condor submission command
    print(sub)

def create_QVecCorr_jobs():
    ntp_list   = os.path.realpath(args.ntp_list)
    macro      = os.path.realpath(args.macro)
    script     = os.path.realpath(args.script)
    executable = os.path.realpath(args.executable)
    output_dir = os.path.realpath(args.output)
    memory     = args.memory
    log        = args.log
    z          = args.vtx_z

    print(f'Macro: {macro}')
    print(f'Run List: {ntp_list}')
    print(f'Vtx z max: {z} cm')
    print(f'Script: {script}')
    print(f'Executable: {executable}')
    print(f'Output Directory: {output_dir}')
    print(f'Requested memory per job: {memory}GB')
    print(f'Condor log file: {log}')

    os.makedirs(output_dir,exist_ok=True)
    shutil.copy(script, output_dir)
    shutil.copy(executable, output_dir)
    shutil.copy(ntp_list, output_dir)
    shutil.copy(macro, output_dir)

    sub = ''
    with open(ntp_list) as file:
        for line in file:
            line = line.rstrip() # remove \n from the end of the string
            run = os.path.splitext(os.path.basename(line))[0] # extract the run number from the file name

            print(f'Run: {run}')
            os.makedirs(f'{output_dir}/{run}/stdout',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/error',exist_ok=True)
            os.makedirs(f'{output_dir}/{run}/output',exist_ok=True)

            # shutil.copy(line, f'{output_dir}/{run}')
            with open(f'{output_dir}/{run}/{run}.list', mode='w') as file2:
                file2.write(f'{line}\n')

            with open(f'{output_dir}/{run}/genQVecCorr.sub', mode="w") as file2:
                file2.write(f'executable     = ../{os.path.basename(script)}\n')
                file2.write(f'arguments      = {output_dir}/{os.path.basename(executable)} $(input_ntp) {z} output/Q-vec-corr-{run}.root output/Q-vec-corr-{run}.csv\n')
                file2.write(f'log            = {log}\n')
                file2.write( 'output         = stdout/job.out\n')
                file2.write( 'error          = error/job.err\n')
                file2.write(f'request_memory = {memory}GB\n')
                file2.write('concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n')
                file2.write(f'queue input_ntp from {run}.list')

            sub = f'{sub} cd {output_dir}/{run} && condor_submit genQVecCorr.sub &&'

    # print condor submission command
    print(sub)

if __name__ == '__main__':
    if(args.command == 'f4a'):
        create_f4a_jobs()
    if(args.command == 'pi0Ana'):
        create_pi0Ana_jobs()
    if(args.command == 'truthPi0Ana'):
        create_truthPi0Ana_jobs()
    if(args.command == 'mix'):
        create_mixedEvent_jobs()
    if(args.command == 'QVecCorr'):
        create_QVecCorr_jobs()
    if(args.command == 'dummy'):
        process_dummy()
    if(args.command == 'dummy2'):
        process_dummy2()
    if(args.command == 'sigmaCheck'):
        process_sigmaCheck()
    if(args.command == 'vn'):
        process_vn()
    if(args.command == 'systematics'):
        process_systematics()
