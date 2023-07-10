#!/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/python3

'''
Input:
    req: local directory full_lists/
    usage:
        make_inp_lists.py [num files total] [num files per list]
    output:
        new_dir: new directory named 'in_lists_[num files total]_by_[num files per list]'
        input files lists: [new_dir]/dst_*_[num files per list]_[0,1,...]
        new queue file: [queue_[num file total]_by_[num files per list].list]
'''

from glob import glob
from sys import argv
import os
from os import path


def main():
    lines_per_output = 1 # lines per output
    try:
        lines_per_output = argv[1]
    except:
        pass

    n_files = -1
    try:
        n_files = int(argv[2])
    except:
        pass

    if n_files != -1:
        odir = f'jobs_{lines_per_output}x{n_files}'
    else:
        odir = f'jobs_{lines_per_output}xAll'

    if path.isdir(odir):
        print(f'replacing contents of {odir}');
        for f in glob(f'{odir}/*'):
            os.remove(f)
    else:
        os.mkdir(odir)

    queue = open(f'{odir}/queue.list','w')

    # determine which input files will be divided
    in_files = []
    in_tags  = []
    n_file = -1
    if (len(glob('full_lists/dst_*.list')) == 0):
        print("fatal error: no lists in full_lists")
        exit()

    for file in glob('full_lists/dst_*.list'):
        in_files .append(open(file,'r'))
        in_tags .append(file.split('/')[-1][:-5])

    try:
        while True:
            n_file += 1
            if n_files != -1 and n_file == n_files:
                break
            o_files = []
            # queue.write(f'out_{n_file}.root, ')
            first = True
            for tag in in_tags:
                name = f'{tag}_{n_file}.list'
                if first:
                    queue.write(f'{name}')
                    first = False
                else:
                    queue.write(f', {name}')
                o_files.append(open(f'{odir}/{name}','w'))
            queue.write('\n')
            for n in range (int(lines_per_output)):
                for i_file, o_file in zip(in_files, o_files):
                    line = i_file.readline()
                    if not line:
                        for file in o_files:
                            o_file.close()
                        raise StopIteration
                    o_file.write(line)
            for file in o_files:
                o_file.close()
    except StopIteration:
        pass
    if n_files == -1:
        with open(f'{odir}/last_list_is_{n_file}','w') as fout:
            fout.write(f'There are {n_file+2} lists')

    queue.close()
    



if __name__ == '__main__':
    main()
