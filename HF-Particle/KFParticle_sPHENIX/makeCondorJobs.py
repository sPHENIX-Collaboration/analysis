import sys, os
from os import environ
import argparse

parser = argparse.ArgumentParser(description='sPHENIX HF Reco Job Creator')
parser.add_argument('-q', '--quarkFilter', default="CHARM", help='Input quark filter: charm or bottom')
parser.add_argument('-f', '--nFilesPerJob', default=50, type=int, help='Number of input files to pass to each job')
parser.add_argument('-t', '--nTotEvents', default=-1, type=int, help='Total number of events to run over')

args = parser.parse_args()

quarkFilter = args.quarkFilter.upper()

types = {'CHARM' : 7, 'BOTTOM' : 8}
if quarkFilter not in types:
  print("The argument, {}, was not known. Use CHARM or BOTTOM instead.".format(args.type))
  sys.exit()

inputFileTypes = ['DST_TRACKS', 'DST_TRUTH', 'DST_CALO_CLUSTER']

myShell = str(environ['SHELL'])
goodShells = ['/bin/bash', '/bin/tcsh']
if myShell not in goodShells:
    print("Your shell {} was not recognised".format(myShell))
    sys.exit()


def makeCondorJob():
    print("Creating condor submission files for {} production".format(quarkFilter))
    inputFiles = []
    line = []
    for i in range(len(inputFileTypes)):
        inputFiles.append(open("inputList_{0}.list".format(inputFileTypes[i].lower()), "r"))
        line.append(inputFiles[i].readline())
    myOutputPath = os.getcwd()
    condorDir = "{}/condorJob".format(myOutputPath)
    os.makedirs("{}/log".format(condorDir), exist_ok=True)
    os.makedirs("{}/fileLists".format(condorDir), exist_ok=True)
    condorFileName = "{0}/my{1}.job".format(condorDir, quarkFilter)
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe        = vanilla\n")
    if myShell == '/bin/bash': condorFile.write("Executable      = {}/run_HFreco.sh\n".format(myOutputPath))
    if myShell == '/bin/tcsh': condorFile.write("Executable      = {}/run_HFreco.csh\n".format(myOutputPath))
    condorFile.write("Initialdir      = {}\n".format(myOutputPath))
    condorFile.write("PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory = 4GB\n")
    condorFile.write("Priority        = 20\n")
    condorFile.write("job_lease_duration = 3600\n\n")
    nJob = 0;
    while line[0]:
        listFile = []
        for i in range(len(inputFileTypes)):
          listFile.append("{0}/fileLists/productionFiles-{1}-{2}-{3:05d}.list".format(condorDir, quarkFilter, inputFileTypes[i].lower(), nJob))
          productionFilesToUse = open(listFile[i], "w")
          for j in range(0, args.nFilesPerJob):
              splitLine = line[i].split("/")
              fileName = splitLine[-1]
              productionFilesToUse.write(fileName)
              line[i] = inputFiles[i].readline()

        condorOutputInfo = "{0}/log/condor-{1}-{2:05d}".format(condorDir, quarkFilter, nJob)
        condorFile.write("Arguments       = \"{}\"\n".format(' '.join(listFile)))
        condorFile.write("Output          = {0}.out\n".format(condorOutputInfo))
        condorFile.write("Error           = {0}.err\n".format(condorOutputInfo))
        condorFile.write("Log             = {0}.log\n".format(condorOutputInfo))
        condorFile.write("Queue\n\n")
        nJob += 1
    print("Submission setup complete!")
    print("This setup will submit {} subjobs".format(nJob))
    print("You can submit your job with the script:\n{}".format(condorFileName))
        
for inputFile in inputFileTypes:
  catalogCommand = "CreateFileList.pl -type {0} {1}".format(types[quarkFilter], inputFile)
  if args.nTotEvents != -1: catalogCommand += " -n {}".format(args.nTotEvents)
  os.system(catalogCommand)
  os.system("mv {0}.list inputList_{0}.list".format(inputFile.lower()))
makeCondorJob()
