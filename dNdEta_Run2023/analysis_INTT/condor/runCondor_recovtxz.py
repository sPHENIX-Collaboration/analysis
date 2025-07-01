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
    dphicut = float(opt.dphicut)
    dcacut = float(opt.dcacut)
    debug = opt.debug
    makedemoplot = opt.makedemoplot
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]
    # subdir = 'data' if isdata else 'sim'

    twolevelup = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir, os.path.pardir))
    infiledir = '{}/production/{}'.format(twolevelup, filedesc)
    print ('infiledir: {}'.format(infiledir))
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    finaloutfiledir = '{}/minitree/VtxEvtMap_{}'.format(parentdir, filedesc)
    print ('finaloutfiledir: {}'.format(finaloutfiledir))
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_recovtxz/', exist_ok=True)
    if not dir_empty('./log_recovtxz/'):
        os.system('rm -rf ./log_recovtxz/')
    os.makedirs('./log_recovtxz/', exist_ok=True)

    # hadd files under bsresfilepath
    os.system('hadd -f -j 20 {}/minitree/BeamspotMinitree_{}/minitree_merged.root {}/minitree/BeamspotMinitree_{}/minitree_0*.root'.format(parentdir, filedesc, parentdir, filedesc))
    
    condorFileName = "submitCondor_recovtxz_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_recovtxz.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    # condorFile.write("concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("nevt               = {}\n".format(nevents))
    condorFile.write("dphicut            = {}\n".format(dphicut * (np.pi / 180.)))
    condorFile.write("dcacut             = {}\n".format(dcacut))
    condorFile.write("bsresfilepath      = {}/minitree/BeamspotMinitree_{}/\n".format(parentdir, filedesc))
    if isdata:
        condorFile.write("infilename         = /sphenix/tg/tg01/hf/hjheng/ppg02/dst/{}/ntuple_$(Extension).root\n".format(filedesc))
    else:
        condorFile.write("infilename         = /sphenix/tg/tg01/hf/hjheng/ppg02/dst/{}/ntuple_$(Extension).root\n".format(filedesc))
    condorFile.write("outfilename        = {}/minitree_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("demoplotpath       = ./plot/RecoPV_demo/RecoPV_{}/{}\n".format('data' if isdata else 'sim', filedesc))
    condorFile.write("debug              = {}\n".format(1 if debug else 0))
    condorFile.write("makedemoplot       = {}\n".format(1 if makedemoplot else 0))
    condorFile.write("Output             = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).err\n")
    # condorFile.write("Log                = $(Initialdir)/condor/log_recovtxz/condorlog_$(Process).log\n")
    condorFile.write("Log                = /tmp/condorlog_hjheng_recovtx_$(Process).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(nevt) $(dphicut) $(dcacut) $(bsresfilepath) $(infilename) $(outfilename) $(demoplotpath) $(debug) $(makedemoplot)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)