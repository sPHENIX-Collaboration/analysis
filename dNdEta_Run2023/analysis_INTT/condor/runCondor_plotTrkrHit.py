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
    parser.add_option("-f", "--filedesc", dest="filedesc", default='HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams', help="File description")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    filedesc = opt.filedesc
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]
    
    twolevelup = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir, os.path.pardir))
    infiledir = '{}/production/{}'.format(twolevelup, filedesc)
    print ('infiledir: {}'.format(infiledir))
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    finaloutfiledir = '{}/plot/hists/{}/TrkrHit'.format(parentdir, filedesc)
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_plotTrkrHit/', exist_ok=True)
    if not dir_empty('./log_plotTrkrHit/'):
        os.system('rm ./log_plotTrkrHit/*')

    condorFileName = "submitCondor_plottrkrhit.job"
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_plotTrkrHit.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    condorFile.write("infilename         = {}/ntuple_$(Extension).root\n".format(infiledir))
    condorFile.write("outfilename        = {}/hists_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("Output             = $(Initialdir)/condor/log_plotTrkrHit/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_plotTrkrHit/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_plotTrkrHit/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(infilename) $(outfilename)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)