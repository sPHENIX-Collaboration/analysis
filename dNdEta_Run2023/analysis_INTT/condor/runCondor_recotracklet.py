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
    parser.add_option("-e", "--nEvents", dest="nEvents", default=200, help="Number of events per job")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="nJob")
    parser.add_option("-r", "--drcut", dest="drcut", default=0.5, help="Delta R cut for tracklets")
    parser.add_option("-s", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt)) 

    isdata = opt.isdata
    filedesc = opt.filedesc
    nEvents = int(opt.nEvents)
    drcut = float(opt.drcut)
    nJob = int(opt.nJob)
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]

    twolevelup = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir, os.path.pardir))
    infiledir = '{}/production/{}'.format(twolevelup, filedesc)
    print ('infiledir: {}'.format(infiledir))
    parentdir = os.path.abspath(os.path.join(os.path.dirname(__file__), os.path.pardir))
    finaloutfiledir = '{}/minitree/TrackletMinitree_{}/{}'.format(parentdir, filedesc, 'dRcut'+str(drcut).replace('.', 'p'))
    print ('finaloutfiledir: {}'.format(finaloutfiledir))
    os.makedirs(finaloutfiledir, exist_ok=True)

    os.makedirs('./log_recotracklet/', exist_ok=True)
    if not dir_empty('./log_recotracklet/'):
        os.system('rm ./log_recotracklet/*')

    condorFileName = "submitCondor_recotracklet_{}.job".format('data' if isdata else 'sim')
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("InitialDir         = {}\n".format(parentdir))
    condorFile.write("Executable         = $(InitialDir)/condor_recotracklet.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 6GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("Myindex            = $(Process)\n")
    condorFile.write("Extension          = $INT(Myindex,%05d)\n")
    condorFile.write("isdata             = {}\n".format(1 if isdata else 0))
    condorFile.write("evtvtxmap          = {}/minitree/VtxEvtMap_{}/minitree_$(Extension).root\n".format(parentdir, filedesc))
    condorFile.write("infilename         = {}/ntuple_$(Extension).root\n".format(infiledir))
    condorFile.write("outfilename        = {}/minitree_$(Extension).root\n".format(finaloutfiledir))
    condorFile.write("nevt               = {}\n".format(nEvents))
    condorFile.write("drcut              = {:.3g}\n".format(drcut))
    condorFile.write("Output             = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).out\n")
    condorFile.write("Error              = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).err\n")
    condorFile.write("Log                = $(Initialdir)/condor/log_recotracklet/condorlog_$(Process).log\n")
    condorFile.write("Arguments          = \"$(isdata) $(evtvtxmap) $(infilename) $(outfilename) $(nevt) $(drcut)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)