import sys, os
from os import environ

myShell = str(environ['SHELL'])
goodShells = ['/bin/bash', '/bin/tcsh']
if myShell not in goodShells:
    print("Your shell {} was not recognised".format(myShell))
    sys.exit()

def makeCondorJob(quarkFilter):
    print("Creating condor submission files for {} production".format(quarkFilter))
    inputFileList = "dst_hf_" + quarkFilter.lower() + ".list"
    infile = open(inputFileList, "r")
    line = infile.readline()
    myOutputPath = os.getcwd()
    condorDir = "{}/condorJobs".format(myOutputPath)
    os.makedirs("{}/log".format(condorDir), exist_ok=True)
    os.makedirs("{}/fileLists".format(condorDir), exist_ok=True)
    submitScriptName = "{}/submitJobs.sh".format(condorDir)
    submitScript = open("{}".format(submitScriptName), "w")
    submitScript.write("#!/bin/bash\n")
    nFilesPerJobs = 10;
    if len(sys.argv) == 3: nFilesPerJobs = int(sys.argv[2])
    nJob = 0;
    while line:
        listFile = "{0}/fileLists/productionFiles-{1}-{2:05d}.list".format(condorDir, quarkFilter, nJob)
        productionFilesToUse = open(listFile, "w")
        for i in range(0, nFilesPerJobs):
            splitLine = line.split("/")
            fileName = splitLine[-1]
            productionFilesToUse.write(fileName)
            line = infile.readline()

        condorOutputInfo = "{0}/log/condor-{1}-{2:05d}".format(condorDir, quarkFilter, nJob)

        condorFileName = "condorJob_{}_{:05d}.job".format(quarkFilter, nJob)
        condorFile = open("{0}/{1}".format(condorDir, condorFileName), "w")
        condorFile.write("Universe        = vanilla\n")
        if myShell == '/bin/bash': condorFile.write("Executable      = {}/run_KFParticle.sh\n".format(myOutputPath))
        if myShell == '/bin/tcsh': condorFile.write("Executable      = {}/run_KFParticle.csh\n".format(myOutputPath))
        condorFile.write("Arguments       = \"{}\"\n".format(listFile))
        condorFile.write("Output          = {0}.out\n".format(condorOutputInfo))
        condorFile.write("Error           = {0}.err\n".format(condorOutputInfo))
        condorFile.write("Log             = {0}.log\n".format(condorOutputInfo))
        condorFile.write("Initialdir      = {}\n".format(myOutputPath))
        condorFile.write("PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)\n")
        condorFile.write("request_memory = 4GB\n")
        condorFile.write("Priority        = 20\n")
        condorFile.write("job_lease_duration = 3600\n")
        condorFile.write("Queue 1\n")
        submitScript.write("condor_submit {}\n".format(condorFileName))
        nJob += 1
    print("Condor submission files have been written to:\n{}/condorJobs".format(myOutputPath))
    print("This setup will submit {} jobs".format(nJob))
    print("You can submit your jobs with the script:\n{}".format(submitScriptName))

        
nArgs = len(sys.argv)

if nArgs == 1 or sys.argv[1].upper() == "CHARM":
    os.system("CreateFileList.pl -type 7 DST_HF_CHARM")
    makeCondorJob("CHARM")
elif sys.argv[1].upper() == "BOTTOM":
    os.system("CreateFileList.pl -type 8 DST_HF_BOTTOM")
    makeCondorJob("BOTTOM")
else:
    print("The argument, {}, was not known. Use CHARM or BOTTOM instead, followed by the number of files per job".format(sys.argv[1]))
