import sys, os
from os import environ
import argparse

parser = argparse.ArgumentParser(description='sPHENIX MDC2 Reco Job Creator')
parser.add_argument('-i', '--inputType', default="CHARM", help='Input type: PYTHIA8_PP_MB, HIJING_[0-20/0-4P88], CHARM[D0], BOTTOM[D0]')
parser.add_argument('-f', '--nFilesPerJob', default=50, type=int, help='Number of input files to pass to each job')
parser.add_argument('-t', '--nTotEvents', default=-1, type=int, help='Total number of events to run over')

args = parser.parse_args()

inputType = args.inputType.upper()

types = {'PYTHIA8_PP_MB' : 3, 'HIJING_0-20' : 4, 'HIJING_0-4P88' : 6, 'CHARM' : 7, 'BOTTOM' : 8, 'CHARMD0' : 9, 'BOTTOMD0' : 10}
if inputType not in types:
  print("The argument, {}, was not known. Use CHARM[D0] or BOTTOM[D0] instead.".format(args.type))
  sys.exit()

dstSets = ['DST_TRACKS', 'DST_VERTEX', 'DST_TRUTH', 'DST_CALO_CLUSTER', 'DST_TRKR_HIT', 'DST_BBC_G4HIT', 'DST_TRKR_G4HIT']

myShell = str(environ['SHELL'])
goodShells = ['/bin/bash', '/bin/tcsh']
if myShell not in goodShells:
    print("Your shell {} was not recognised".format(myShell))
    sys.exit()


def makeCondorJob():
    print("Creating condor submission files for {} production".format(inputType))
    inputFiles = []
    line = []
    for i in range(len(dstSets)):
        inputFiles.append(open("{0}.list".format(dstSets[i].lower()), "r"))
        line.append(inputFiles[i].readline())
    myOutputPath = os.getcwd()
    condorDir = "{}/condorJob".format(myOutputPath)
    os.makedirs("{}/log".format(condorDir), exist_ok=True)
    os.makedirs("{}/fileLists".format(condorDir), exist_ok=True)
    nJob = 0;
    while line[0]:
        listFile = []
        listFileGeneric = []
        for i in range(len(dstSets)):
          fileStart = "fileLists/productionFiles-{1}-{2}-".format(condorDir, inputType, dstSets[i].lower())
          listFile.append("{0}/{1}{2:05d}.list".format(condorDir, fileStart, nJob))
          listFileGeneric.append("$(condorDir)/{0}$INT(Process,%05d).list".format(fileStart))
          productionFilesToUse = open(listFile[i], "w")
          for j in range(0, args.nFilesPerJob):
              splitLine = line[i].split("/")
              fileName = splitLine[-1]
              productionFilesToUse.write(fileName)
              line[i] = inputFiles[i].readline()
        nJob += 1
    condorFileName = "{0}/my{1}.job".format(condorDir, inputType)
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("initialDir         = {}\n".format(myOutputPath))
    if myShell == '/bin/bash': condorFile.write("Executable         = $(initialDir)/run_HFreco.sh\n")
    if myShell == '/bin/tcsh': condorFile.write("Executable         = $(initialDir)/run_HFreco.csh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 1.8GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("condorDir          = $(initialDir)/condorJob\n")
    condorOutputInfo = "$(condorDir)/log/condor-{0}-$INT(Process,%05d)".format(inputType)
    condorFile.write("Output             = {0}.out\n".format(condorOutputInfo))
    condorFile.write("Error              = {0}.err\n".format(condorOutputInfo))
    condorFile.write("Log                = {0}.log\n".format(condorOutputInfo))
    condorFile.write("Arguments          = \"{}\"\n".format(' '.join(listFileGeneric)))
    condorFile.write("Queue {}\n".format(nJob))
    print("Submission setup complete!")
    print("This setup will submit {} subjobs".format(nJob))
    print("You can submit your job with the script:\n{}".format(condorFileName))
        
catalogCommand = "CreateFileList.pl -type {0} {1}".format(types[inputType], ' '.join(dstSets))
if args.nTotEvents != -1: catalogCommand += " -n {}".format(args.nTotEvents)
os.system(catalogCommand)
makeCondorJob()
