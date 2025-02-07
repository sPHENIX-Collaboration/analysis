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
    parser.add_option("-d", "--isdata", dest="isdata", action="store_true", default=False, help="Is data or simulation")
    parser.add_option("-f", "--filedesc", dest="filedesc", default='Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey', help="File description")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    isdata = opt.isdata
    filedesc = opt.filedesc
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    finaloutfiledir = '{}/plot/hists/{}/RecoVtx'.format(parentdir, filedesc)
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_plotrecovtx/', exist_ok=True)
    if not dir_empty('./log_plotrecovtx/'):
        os.system('rm ./log_plotrecovtx/*')

    condorFileName = "submitCondor_plotRecoVtx_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_plotRecoVtx.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:100\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("infilename         = {}/minitree/VtxEvtMap_{}/minitree_$(Extension).root\n".format(parentdir,filedesc))
    condorFile.write("outfilename        = {}/hists_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("Output             = $(Initialdir)/condor/log_plotrecovtx/condorlog_$(Extension).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_plotrecovtx/condorlog_$(Extension).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_plotrecovtx/condorlog_$(Extension).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(infilename) $(outfilename)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)