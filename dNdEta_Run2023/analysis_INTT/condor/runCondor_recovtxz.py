from optparse import OptionParser
import time
import os
import pwd
import sys
import re
import numpy as np

def dir_empty(dir_path):
    return not any((True for _ in os.scandir(dir_path)))


if __name__ == '__main__':
    parser = OptionParser(usage="usage: %prog ver [options -h]")
    parser.add_option("-d", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("-f", "--filedesc", dest="filedesc", default='ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="File description")
    parser.add_option("-n", "--nevents", dest="nevents", default=400, help="Number of events to process")
    parser.add_option("-x", "--vtxx", dest="vtxx", default=-0.04, help="Vertex x")
    parser.add_option("-y", "--vtxy", dest="vtxy", default=0.24, help="Vertex y")
    parser.add_option("-p", "--dphicut", dest="dphicut", default=0.03, help="Delta phi cut [degree]")
    parser.add_option("-c", "--dcacut", dest="dcacut", default=0.15, help="DCA cut [cm]")
    parser.add_option("-g", "--debug", dest="debug", action="store_true", default=False, help="Debug mode")
    parser.add_option("-m", "--makedemoplot", dest="makedemoplot", action="store_true", default=False, help="Make demo plots")                      
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    isdata = opt.isdata
    filedesc = opt.filedesc
    nevents = int(opt.nevents)
    vtxx = float(opt.vtxx)
    vtxy = float(opt.vtxy)
    dphicut = float(opt.dphicut)
    dcacut = float(opt.dcacut)
    debug = opt.debug
    makedemoplot = opt.makedemoplot
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]
    # subdir = 'data' if isdata else 'sim'
    infiledir = '/sphenix/user/{}/TrackletAna/data/INTT/{}'.format(username, filedesc)
    finaloutfiledir = '/sphenix/user/{}/TrackletAna/minitree/INTT/VtxEvtMap_{}'.format(username, filedesc)
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_recovtxz/', exist_ok=True)
    if not dir_empty('./log_recovtxz/'):
        os.system('rm ./log_recovtxz/*')

    
    condorFileName = "submitCondor_recovtxz_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = /sphenix/user/{}/TrackletAna/analysis_INTT\n".format(username))
    condorFile.write("Executable         = $(InitialDir)/condor_recovtxz.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 6GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("nevt               = {}\n".format(nevents))
    condorFile.write("avgvtxx            = {}\n".format(vtxx))
    condorFile.write("avgvtxy            = {}\n".format(vtxy))
    condorFile.write("dphicut            = {}\n".format(dphicut * (np.pi / 180.)))
    condorFile.write("dcacut             = {}\n".format(dcacut))
    condorFile.write("infilename         = {}/ntuple_$(Process).root\n".format(infiledir))
    condorFile.write("outfilename        = {}/minitree_$(Process).root\n".format(finaloutfiledir))
    condorFile.write("demoplotpath       = ./plot/RecoPV_demo/RecoPV_{}/{}\n".format('data' if isdata else 'sim', filedesc))
    condorFile.write("debug              = {}\n".format(1 if debug else 0))
    condorFile.write("makedemoplot       = {}\n".format(1 if makedemoplot else 0))
    condorFile.write("Output             = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(nevt) $(avgvtxx) $(avgvtxy) $(dphicut) $(dcacut) $(infilename) $(outfilename) $(demoplotpath) $(debug) $(makedemoplot)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)