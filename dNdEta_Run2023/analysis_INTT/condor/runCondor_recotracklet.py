from optparse import OptionParser
import time
import os
import pwd
import sys
import re

def dir_empty(dir_path):
    return not any((True for _ in os.scandir(dir_path)))

if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("--filedesc", dest="filedesc", default='HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="File description")
    parser.add_option("--nEvents", dest="nEvents", default=200, help="Number of events per job")
    parser.add_option("--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("--drcut", dest="drcut", default=0.5, help="Delta R cut for tracklets")
    parser.add_option("--randomvtxz", dest="randomvtxz", action="store_true", default=False, help="Randomize vtx z (for geometric acceptance correction)")
    parser.add_option("--randomclusset", dest="randomclusset", default=0, help="Random cluster set (for systematic uncertainty)")
    parser.add_option("--clusadccutset", dest="clusadccutset", default=0, help="Cluster ADC cut set (for systematic uncertainty)")
    parser.add_option("--clusphisizecutset", dest="clusphisizecutset", default=0, help="Cluster phisize cut set (for systematic uncertainty)")
    parser.add_option("--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    isdata = opt.isdata
    filedesc = opt.filedesc
    nEvents = int(opt.nEvents)
    drcut = float(opt.drcut)
    nJob = int(opt.nJob)
    randomvtxz = opt.randomvtxz
    randomclusset = int(opt.randomclusset)
    clusadccutset = int(opt.clusadccutset)
    clusphisizecutset = int(opt.clusphisizecutset)
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]

    twolevelup = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir, os.path.pardir))
    infiledir = '{}/production/{}'.format(twolevelup, filedesc)
    print ('infiledir: {}'.format(infiledir))
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    tgspacedir = '/sphenix/tg/tg01/hf/hjheng/ppg02'
    finaloutfiledir = '{}/minitree/TrackletMinitree_{}/{}'.format(tgspacedir, filedesc, 'dRcut'+str(drcut).replace('.', 'p')+('_RandomVtxZ' if randomvtxz else '_NominalVtxZ')+('_RandomClusSet'+str(randomclusset))+('_clusAdcCutSet'+str(clusadccutset))+('_clusPhiSizeCutSet'+str(clusphisizecutset)))
    print ('finaloutfiledir: {}'.format(finaloutfiledir))
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_recotracklet/', exist_ok=True)
    if not dir_empty('./log_recotracklet/'):
        os.system('rm ./log_recotracklet/*')

    condorFileName = "submitCondor_recotracklet_{}_{}.job".format('data' if isdata else 'sim', 'dRcut'+str(drcut).replace('.', 'p')+('_RandomVtxZ' if randomvtxz else '_NominalVtxZ')+('_RandomClusSet'+str(randomclusset))+('_clusAdcCutSet'+str(clusadccutset))+('_clusPhiSizeCutSet'+str(clusphisizecutset)))
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_recotracklet.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n")
    if isdata:
        condorFile.write("request_memory       = 10GB\n")
    else:
        condorFile.write("request_memory       = 6GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("evtvtxmap          = {}/minitree/VtxEvtMap_{}/minitree_$(Extension).root\n".format(parentdir, filedesc))
    if isdata:
        condorFile.write("infilename         = /sphenix/tg/tg01/hf/hjheng/ppg02/dst/{}/ntuple_wEvtBcoDiff_$(Extension).root\n".format(filedesc))
    else:
        condorFile.write("infilename         = /sphenix/tg/tg01/hf/hjheng/ppg02/dst/{}/ntuple_$(Extension).root\n".format(filedesc))
    condorFile.write("outfilename        = {}/minitree_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("nevt               = {}\n".format(nEvents))
    condorFile.write("drcut              = {:.3g}\n".format(drcut))
    condorFile.write("randomvtxz         = {}\n".format(1 if randomvtxz else 0))
    condorFile.write("randomclusset      = {}\n".format(randomclusset))
    condorFile.write("clusadccutset      = {}\n".format(clusadccutset))
    condorFile.write("clusphisizecutset  = {}\n".format(clusphisizecutset))
    condorFile.write("Output             = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(evtvtxmap) $(infilename) $(outfilename) $(nevt) $(drcut) $(randomvtxz) $(randomclusset) $(clusadccutset) $(clusphisizecutset)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)