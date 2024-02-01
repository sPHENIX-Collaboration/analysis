from optparse import OptionParser
import time
import os
import pwd
import sys
import re

def dir_empty(dir_path):
    return not any((True for _ in os.scandir(dir_path)))

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
    # parser.add_option("-p", "--dirprefix", dest="dirprefix", default='ana398_zvtx-20cm_dummyAlignParams', help="Directory prefix")
    parser.add_option("-f", "--filedesc", dest="filedesc", default='HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="File description")
    # parser.add_option("-s", "--nset", dest="nset", default=400, help="Number of sets")
    # parser.add_option("-r", "--drcut", dest="drcut", default=0.5, help="Delta R cut for tracklets")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-r", "--drcut", dest="drcut", default=0.5, help="Delta R cut for tracklets")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    isdata = opt.isdata
    # dirprefix = opt.dirprefix
    filedesc = opt.filedesc
    # nset = int(opt.nset)
    # drcut = float(opt.drcut)
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    drcut = float(opt.drcut)
    username = pwd.getpwuid(os.getuid())[0]

    # subdir = 'data' if isdata else 'sim'
    finaloutfiledir = '/sphenix/user/{}/TrackletAna/analysis_INTT/plot/hists/{}/{}'.format(username, filedesc, 'dRcut'+str(drcut).replace('.', 'p'))
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_plottracklet/', exist_ok=True)
    if not dir_empty('./log_plottracklet/'):
        os.system('rm ./log_plottracklet/*')

    condorFileName = "submitCondor_plottracklet_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = /sphenix/user/{}/TrackletAna/analysis_INTT\n".format(username))
    condorFile.write("Executable         = $(InitialDir)/condor_plotTracklet.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("infilename         = /sphenix/user/{}/TrackletAna/minitree/INTT/TrackletMinitree_{}/{}/minitree_$(Process).root\n".format(username, filedesc, 'dRcut'+str(drcut).replace('.', 'p')))
    condorFile.write("outfilename        = {}/hists_$(Process).root\n".format(finaloutfiledir))
    condorFile.write("Output             = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(infilename) $(outfilename)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)

    # for i in range(nset):
    #     print('Set {}'.format(i))
    #     newfile = 'submitCondor_plotTracklet_set{}.job'.format(i)
    #     os.system('cp submitCondor_plotTracklet.job {}'.format(newfile))
    #     replacetext(newfile, 'ISDATA', '{}'.format(isdata))
    #     inputminitree = '/sphenix/user/mjpeters/analysis/dNdEta_Run2023/macros/dataTracklet_{}.root'.format(i) if isdata else '/sphenix/user/hjheng/TrackletAna/minitree/INTT/TrackletMinitree_{}/{}/TrackletAna_minitree_Set{}_dRcut{}.root'.format(dirprefix, subdir, i, str(drcut).replace('.', 'p'))
    #     outhistname = '/sphenix/user/hjheng/TrackletAna/analysis_INTT/plot/hists/{}/Hists_RecoTracklets_Set{}.root'.format(dirprefix, i)
    #     replacetext(newfile, 'INFILENAME', '{}'.format(inputminitree))
    #     replacetext(newfile, 'OUTFILENAME', '{}'.format(outhistname))
    #     replacetext(newfile, 'SET', '{}'.format(i))
    #     cmd = 'condor_submit ' + newfile
    #     os.system(cmd)