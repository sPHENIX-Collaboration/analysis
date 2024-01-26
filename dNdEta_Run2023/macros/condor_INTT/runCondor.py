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
    parser.add_option("-d", "--data", dest="data", action="store_true", default=False, help="Is data or simulation")
    parser.add_option("-g", "--generator", dest="generator", default="HIJING", help="Generator type (HIJING, EPOS, AMPT)")
    parser.add_option("-n", "--eventPerJob", dest="eventPerJob", default=200, help="Number of events per job")
    parser.add_option("-j", "--nJob", dest="nJob", default=400, help="Number of jobs (queues)")
    parser.add_option("-o", "--outputdir", dest="outputdir", default="HIJING_ana398_zvtx-20cm_dummyAlignParams", help="Output directory (under /sphenix/user/hjheng/TrackletAna/data/INTT/)")
    parser.add_option("-s", "--softwareversion", dest="softwareversion", default="new", help="Software version (new, ana.3xx)")
    parser.add_option("-c", "--submitcondor", dest="submitcondor", action="store_true", default=False, help="Submit condor jobs")

    (opt, args) = parser.parse_args()
    print('opt: {}'.format(opt))

    data = opt.data
    generator = opt.generator
    eventPerJob = int(opt.eventPerJob)
    nJob = int(opt.nJob)
    outputdir = opt.outputdir
    softwareversion = opt.softwareversion
    submitcondor = opt.submitcondor
    username = pwd.getpwuid(os.getuid())[0]
    # print ('username: {}'.format(username))
    # subdir = 'data' if data else 'sim/{}'.format(generator)
    ntupledest = '/sphenix/user/{}/TrackletAna/data/INTT/{}'.format(username,outputdir)
    os.makedirs(ntupledest, exist_ok=True)

    os.makedirs('./condorLog/', exist_ok=True)
    if not dir_empty('./condorLog/'):
        os.system('rm ./condorLog/*')
    
    condorFileName = "submitCondor_process{}.job".format('Data' if data else 'Sim{}'.format(generator))
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("initialDir         = /sphenix/user/{}/sPHENIXdNdEta/macros/\n".format(username))
    condorFile.write("Executable         = $(initialDir)/production_INTT.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("runData            = {}\n".format(1 if data else 0))
    condorFile.write("generator          = {}\n".format(generator))
    condorFile.write("nEvents            = {}\n".format(eventPerJob))
    condorFile.write("filename           = {}/ntuple_$(Process).root\n".format(ntupledest))
    condorFile.write("Output             = $(Initialdir)/condor_INTT/condorLog/process{}_$(Process).out\n".format('Data' if data else 'Sim{}'.format(generator)))
    condorFile.write("Error              = $(Initialdir)/condor_INTT/condorLog/process{}_$(Process).err\n".format('Data' if data else 'Sim{}'.format(generator)))
    condorFile.write("Log                = $(Initialdir)/condor_INTT/condorLog/process{}_$(Process).log\n".format('Data' if data else 'Sim{}'.format(generator)))
    condorFile.write("Arguments          = \"$(runData) $(generator) $(nEvents) $(filename) $(Process) $(softwareversion)\"\n")
    condorFile.write("Queue {}\n".format(nJob))
    condorFile.close() # Close the file before submitting the job

    if submitcondor:
        cmd = 'condor_submit ' + condorFileName
        os.system(cmd)