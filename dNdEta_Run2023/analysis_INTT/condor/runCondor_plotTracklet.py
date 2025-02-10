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
    parser.add_option("-f", "--filedesc", dest="filedesc", default='HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="File description")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-r", "--drcut", dest="drcut", default=0.5, help="Delta R cut for tracklets")
    parser.add_option("--randomclusset", dest="randomclusset", default=0, help="Random cluster set (for systematic uncertainty)")
    parser.add_option("--clusadccutset", dest="clusadccutset", default=0, help="Cluster ADC cut set (for systematic uncertainty)")
    parser.add_option("--clusphisizecutset", dest="clusphisizecutset", default=0, help="Cluster phisize cut set (for systematic uncertainty)")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    isdata = opt.isdata
    filedesc = opt.filedesc
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    drcut = float(opt.drcut)
    randomclusset = int(opt.randomclusset)
    clusadccutset = int(opt.clusadccutset)
    clusphisizecutset = int(opt.clusphisizecutset)
    username = pwd.getpwuid(os.getuid())[0]

    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    # subdir = 'data' if isdata else 'sim'
    finaloutfiledir = '{}/plot/hists/{}/{}'.format(parentdir, filedesc, 'dRcut'+str(drcut).replace('.', 'p')+'_NominalVtxZ'+('_RandomClusSet'+str(randomclusset))+('_clusAdcCutSet'+str(clusadccutset))+('_clusPhiSizeCutSet'+str(clusphisizecutset)))
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_plottracklet/', exist_ok=True)
    if not dir_empty('./log_plottracklet/'):
        os.system('rm ./log_plottracklet/*')

    condorFileName = "submitCondor_plottracklet_{}_{}.job".format('data' if isdata else 'sim', 'dRcut'+str(drcut).replace('.', 'p')+'_NominalVtxZ'+('_RandomClusSet'+str(randomclusset))+('_clusAdcCutSet'+str(clusadccutset))+('_clusPhiSizeCutSet'+str(clusphisizecutset)))
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_plotTracklet.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    # condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("infilename         = /sphenix/tg/tg01/hf/hjheng/ppg02/minitree/TrackletMinitree_{}/{}/minitree_$(Extension).root\n".format(filedesc, 'dRcut'+str(drcut).replace('.', 'p')+'_NominalVtxZ'+('_RandomClusSet'+str(randomclusset))+('_clusAdcCutSet'+str(clusadccutset))+('_clusPhiSizeCutSet'+str(clusphisizecutset))))
    condorFile.write("outfilename        = {}/hists_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("Output             = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_plottracklet/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(infilename) $(outfilename)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)