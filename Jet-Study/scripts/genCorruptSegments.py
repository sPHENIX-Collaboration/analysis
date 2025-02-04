#!/usr/bin/env python3
import subprocess
import argparse
import os

parser = argparse.ArgumentParser()

parser.add_argument('-i', '--log-dir', type=str, default='/sphenix/data/data02/sphnxpro/run2pp/calologs/ana446_2024p007', help='Location of the log directory.')
parser.add_argument('-o', '--output', type=str, default='bad-calologs-ana446-2024p007.list', help='Output list file of all files that have an error.')
parser.add_argument('-o2', '--output-segments', type=str, default='bad-ana446-2024p007-segments.list', help='Output list file of all segments that have an error.')

args = parser.parse_args()

if __name__ == '__main__':
    log_dir         = os.path.realpath(args.log_dir)
    output          = os.path.realpath(args.output)
    output_segments = os.path.realpath(args.output_segments)

    print(f'Log Dir: {log_dir}')
    print(f'Output: {output}')
    print(f'Output Segments: {output_segments}')

    # remove output file before writing if it already exists
    if os.path.exists(output):
        os.remove(output)
        print(f'File {output} deleted successfully.')

    # generate the list of log files that contain Error
    command = f"""while read d; do
                    echo "Processing Dir: $d, $i"
                    /direct/sphenix+u/anarde/.cargo/bin/rg -l "Error" $d >> {output}
                    echo "logs: $(wc -l {output})"
                    i=$((i+1))
                done < <(readlink -f {log_dir}/*)"""
    subprocess.run(['bash','-c',command])

    # extract only the run-segments from the list of log files
    command = f'awk -F\'/\' \'{{print $NF}}\' {output} | cut -d"-" -f2,3 | cut -d "." -f1 | sort > {output_segments}'
    subprocess.run(['bash','-c',command])
