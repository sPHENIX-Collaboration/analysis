import sys, os
from os import environ
import argparse

parser = argparse.ArgumentParser(description='sPHENIX MDC2 Reco Job Creator')
parser.add_argument('-i', '--inputType', default="JETS", help='Input type: PYTHIA8_PP_MB, HIJING_[0-20/0-4P88], CHARM[D0], BOTTOM[D0]')
parser.add_argument('-f', '--nFilesPerJob', default=30, type=int, help='Number of input files to pass to each job')
parser.add_argument('-t', '--nTotEvents', default=-1, type=int, help='Total number of events to run over')

args = parser.parse_args()

with open("Run_List_Generator/FileLists/Full_ppGoldenRunList_Version1.txt") as file_in:
    lines = [line.rstrip('\n') for line in file_in]
print(len(lines))

inputType = args.inputType.upper()

dstSets = ['DST_JETCALO', 'DST_JET']

myShell = str(environ['SHELL'])
goodShells = ['/bin/bash', '/bin/tcsh']
if myShell not in goodShells:
    print("Your shell {} was not recognised".format(myShell))
    sys.exit()

def makeCondorJob():
    print("Creating condor submission files for {} production".format(inputType))
    inputFiles = []
    line = []
    NFiles = []
 
    for i in range(len(lines)):
        f1 = open("Run_List_Generator/dst_list/{0}_run2pp-000{1}.list".format(dstSets[0].lower(), lines[i].lower()), "r")
        inputFiles.append(f1)
        line.append(inputFiles[i].readline())
        with open("Run_List_Generator/dst_list/{0}_run2pp-000{1}.list".format(dstSets[0].lower(), lines[i].lower()), "r") as f:
           for j, _ in enumerate(f):
              pass
        NFiles.append(j+1)

    myOutputPath = os.getcwd()
    condorDir = "{}/condorJob".format(myOutputPath)
    os.makedirs("{}/log".format(condorDir), exist_ok=True)
    os.makedirs("{}/fileLists".format(condorDir), exist_ok=True)
    
    nJob = 0;
    Current_Count = 0;
    while line[0]:
        listFile = []
        listFileGeneric = []
        fileStart = "fileLists/productionFiles-{1}-{2}-".format(condorDir, inputType, dstSets[0].lower(), lines[i].lower())
        fileStart2 = "fileLists/productionFiles-{1}-{2}-".format(condorDir, inputType, dstSets[1].lower(), lines[i].lower())
        with open('listRun.txt','w') as file:
           for i in range(len(lines)):
              Current_Count = 0;
              while(Current_Count < NFiles[i]*1/(args.nFilesPerJob)):
                 file.write('{0}\n'.format(lines[i].lower()))
                 listFile.append("{0}/{1}{2:05d}.list".format(condorDir, fileStart, nJob))
                 productionFilesToUse = open(listFile[nJob], "w")
                 for j in range(0, args.nFilesPerJob):
                    splitLine = line[i].split("/")
                    fileName = splitLine[-1]
                    productionFilesToUse.write(fileName)
                    line[i] = inputFiles[i].readline()
                    if ((Current_Count + 1) > NFiles[i]*1/(args.nFilesPerJob)) and (Current_Count*args.nFilesPerJob + j == NFiles[i]):
                       break 
                 nJob += 1 
                 Current_Count += 1
 
    listFileGeneric.append("$(condorDir)/{0}$INT(Process,%05d).list".format(fileStart))
    listFileGeneric.append("$(condorDir)/{0}$INT(Process,%05d).list".format(fileStart2))
    listFileGeneric.append("$INT(Process,%05d)")

    condorFileName = "{0}/my{1}.job".format(condorDir, inputType)
    condorFile = open("{}".format(condorFileName), "w")
    condorFile.write("Universe           = vanilla\n")
    condorFile.write("initialDir         = {}\n".format(myOutputPath))
    if myShell == '/bin/bash': condorFile.write("Executable         = $(initialDir)/run_Events.sh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 6144MB\n")
    condorFile.write("concurrency_limits = CONCURRENCY_LIMIT_DEFAULT:1000\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("condorDir          = $(initialDir)/condorJob\n")
    condorOutputInfo = "$(condorDir)/log/condor-{0}-$INT(Process,%05d)".format(inputType)
    condorFile.write("Output             = {0}.out\n".format(condorOutputInfo))
    condorFile.write("Error              = {0}.err\n".format(condorOutputInfo))
    condorFile.write("Log                = /tmp/condor-napple-$INT(Process,%05d).log\n")
    condorFile.write("Arguments          = \"{}\"\n".format(' '.join(listFileGeneric)))
    condorFile.write("Queue {}\n".format(nJob))
    print("Submission setup complete!")
    print("This setup will submit {} subjobs".format(nJob))
    print("You can submit your job with the script:\n{}".format(condorFileName))
        
makeCondorJob()
