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
    parser.add_option("-d", "--isdata", dest="isdata", action="store_true", default=False, help="Is data")
    parser.add_option("-i", "--infiledir", dest="infiledir", default='/sphenix/user/hjheng/TrackletAna/data/INTT/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="Input file")
    parser.add_option("-o", "--outfiledir", dest="outfiledir", default='BeamspotMinitree_HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="Output file directory (under /sphenix/user/hjheng/TrackletAna/minitree/INTT/)")
    parser.add_option("-p", "--dphicut", dest="dphicut", default=0.05, help="Delta phi cut")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    isdata = opt.isdata
    infiledir = opt.infiledir
    outfiledir = opt.outfiledir
    dphicut = float(opt.dphicut)
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    # username = pwd.getpwuid(os.getuid())[0]
    # print ('username: {}'.format(username))
    # subdir = 'data' if isdata else 'sim'
    # get parent directory of this file
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    finaloutfiledir = '{}/minitree/{}'.format(parentdir, outfiledir)
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_beamspot/', exist_ok=True)
    if not dir_empty('./log_beamspot/'):
        os.system('rm ./log_beamspot/*')

    
    condorFileName = "submitCondor_BeamspotReco_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_BeamspotReco.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    if isdata:
        condorFile.write("inputfile          = {}/ntuple_wEvtBcoDiff_$(Extension).root\n".format(infiledir))
    else:
        condorFile.write("inputfile          = {}/ntuple_$(Extension).root\n".format(infiledir))
    condorFile.write("dphicut            = {}\n".format(dphicut))
    condorFile.write("outputfile         = {}/minitree_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("Output             = $(Initialdir)/condor/log_beamspot/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_beamspot/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_beamspot/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(inputfile) $(outputfile) $(dphicut)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)