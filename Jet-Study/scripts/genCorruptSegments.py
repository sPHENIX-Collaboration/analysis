#!/usr/bin/env python3
import subprocess
import argparse
import os

parser = argparse.ArgumentParser()

parser.add_argument('-i', '--log-dir', type=str, default='/sphenix/data/data02/sphnxpro/run2pp/calologs/ana446_2024p007', help='Location of the log directory.')
parser.add_argument('-d', '--dataset', type=str, default='ana446_2024p007', help='Production Dataset.')
parser.add_argument('-d2', '--dsttype', type=str, default='DST_TRIGGERED_EVENT_run2pp', help='Production Dst Type.')
parser.add_argument('-o', '--output', type=str, default='bad-calologs-ana446-2024p007.list', help='Output list file of all files that have an error.')
parser.add_argument('-o2', '--output-segments', type=str, default='bad-ana446-2024p007-segments.list', help='Output list file of all segments that have an error.')
parser.add_argument('-o3', '--output-dir', type=str, default='bad-segments', help='Output directory to save all files.')

args = parser.parse_args()

if __name__ == '__main__':
    log_dir         = os.path.realpath(args.log_dir)
    dataset         = args.dataset
    dsttype         = args.dsttype
    output          = args.output
    output_segments = args.output_segments
    output_dir      = os.path.realpath(args.output_dir)

    print(f'Log Dir: {log_dir}')
    print(f'Dataset: {dataset}')
    print(f'DST Type: {dsttype}')
    print(f'Output: {output}')
    print(f'Output Segments: {output_segments}')
    print(f'Output Directory: {output_dir}')

    os.makedirs(output_dir,exist_ok=True)
    # remove output file before writing if it already exists
    if os.path.exists(f'{output_dir}/{output}'):
        os.remove(f'{output_dir}/{output}')
        print(f'File {output} deleted successfully.')

    # generate the list of log files that contain Error
    command = f"""while read d; do
                    echo "Processing Dir: $d, $i"
                    /direct/sphenix+u/anarde/.cargo/bin/rg -l "Error" $d >> {output}
                    echo "logs: $(wc -l {output})"
                    i=$((i+1))
                done < <(readlink -f {log_dir}/*)"""
    subprocess.run(['bash','-c',command],cwd=output_dir)

    # sort the log file
    command = f'sort {output} -o {output}'
    subprocess.run(['bash','-c',command],cwd=output_dir)

    # extract only the run-segments from the list of log files
    command = f'awk -F\'/\' \'{{print $NF}}\' {output} | cut -d"-" -f2,3 | cut -d "." -f1 | sort > {output_segments}'
    subprocess.run(['bash','-c',command],cwd=output_dir)

    # get the list of produced DST
    command = f'psql FileCatalog -c "select filename from datasets where dataset = \'{dataset}\' and dsttype=\'{dsttype}\';" -At | cut -d "-" -f2,3 | cut -d"." -f1 | sort > {dsttype}_{dataset}.list'
    subprocess.run(['bash','-c',command],cwd=output_dir)

    # find the list of segments that overlap between containing Error and produced
    produced_segments = f'{os.path.splitext(output_segments)[0]}-produced.list'
    command = f'comm -12 {dsttype}_{dataset}.list {output_segments} > {produced_segments}'
    subprocess.run(['bash','-c',command],cwd=output_dir)

    # find the list of segments that overlap between containing Error and produced
    produced_logs = f'{os.path.splitext(output)[0]}-produced.list'
    command = f'/direct/sphenix+u/anarde/.cargo/bin/rg -Ff {produced_segments} {output} > {produced_logs}'
    subprocess.run(['bash','-c',command],cwd=output_dir)
