from optparse import OptionParser
import time
import os
import sys
import re

# Ref: https://www.geeksforgeeks.org/how-to-search-and-replace-text-in-a-file-in-python/
def replacetext(filename, search_text, replace_text):
    with open(filename, 'r+') as f:
        file = f.read()
        file = re.sub(search_text, replace_text, file)
        f.seek(0)
        f.write(file)
        f.truncate()
    # return 'Text {} replaced by {}'.format(search_text, replace_text)


if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -n]")
    parser.add_option("-d", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("-n", "--eventperset", dest="eventperset", default=200, help="Number of events per set")
    parser.add_option("-s", "--nset", dest="nset", default=400, help="Number of sets")
    parser.add_option("-o", "--outputdir", dest="outputdir", default="ana382_zvtx-20cm", help="Output directory (under /sphenix/user/hjheng/TrackletAna/data/INTT/)")

    (opt, args) = parser.parse_args()

    isdata = opt.isdata
    eventperset = int(opt.eventperset)
    nset = int(opt.nset)
    outputdir = opt.outputdir
    subdir = 'data' if isdata else 'sim'
    os.makedirs('/sphenix/user/hjheng/TrackletAna/data/INTT/{}/'.format(outputdir)+subdir, exist_ok=True)

    os.makedirs('./condor_out/', exist_ok=True)
    os.makedirs('./condor_out/out/', exist_ok=True)
    os.makedirs('./condor_out/err/', exist_ok=True)
    os.makedirs('./condor_out/log/', exist_ok=True)
    os.system('rm condor_set*.job')
    os.system('rm ./condor_out/out/*')
    os.system('rm ./condor_out/err/*')
    os.system('rm ./condor_out/log/*')

    for i in range(nset):
        print('Run Fun4All, file list set {}'.format(i))
        newfile = 'condor_set{}.job'.format(i)
        os.system('cp condor.job {}'.format(newfile))
        replacetext(newfile, 'RUNDATA', '{}'.format(1 if isdata else 0))
        replacetext(newfile, 'NEVENTS', '{}'.format(eventperset))
        replacetext(newfile, 'INPUTFILELISTIDX', '{}'.format(i))
        replacetext(newfile, 'FILENAME', '/sphenix/user/hjheng/TrackletAna/data/INTT/{}/{}/INTTRecoClusters_{}_set{}.root'.format(outputdir,subdir,subdir,i))
        cmd = 'condor_submit ' + newfile
        os.system(cmd)