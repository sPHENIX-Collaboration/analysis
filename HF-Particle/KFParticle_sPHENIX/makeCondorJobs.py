import sys, os

def makeCondorJob(quarkFilter):
    print("Creating condor submission files for {} production".format(quarkFilter))
    inputFileList = "dst_hf_" + quarkFilter.lower() + ".list"
    infile = open(inputFileList, "r")
    line = infile.readline()
    myOutputPath = os.getcwd()
    condorDir = "{}/condorJobs".format(myOutputPath)
    os.makedirs("{}/log".format(condorDir), exist_ok=True)
    submitScriptName = "{}/submitJobs.sh".format(condorDir)
    submitScript = open("{}".format(submitScriptName), "w")
    submitScript.write("#!/bin/bash\n")
    while line:
        splitLine = line.split("/")
        fileName = splitLine[-1]
        fileName = fileName.replace('\n', '')
        productionNumber = line[-11: -6]

        condorOutputInfo = "{0}/log/condor-{1}-{2}".format(condorDir, quarkFilter, productionNumber)

        condorFileName = "condorJob_".format(condorDir) + quarkFilter + "_" + productionNumber + ".job"
        condorFile = open("{0}/{1}".format(condorDir, condorFileName), "w")
        condorFile.write("Universe        = vanilla\n")
        condorFile.write("Executable      = {}/run_KFParticle.sh\n".format(myOutputPath))
        condorFile.write("Arguments       = \"/sphenix/sim/sim01/sphnxpro/MDC1/HF_pp200_signal/data {}\"\n".format(fileName))
        condorFile.write("Output          = {0}.out\n".format(condorOutputInfo))
        condorFile.write("Error           = {0}.err\n".format(condorOutputInfo))
        condorFile.write("Log             = {0}.log\n".format(condorOutputInfo))
        condorFile.write("Initialdir      = {}\n".format(myOutputPath))
        condorFile.write("PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)\n")
        condorFile.write("request_memory = 4GB\n")
        condorFile.write("Priority        = 20\n")
        condorFile.write("job_lease_duration = 3600\n")
        condorFile.write("Queue 1\n")
        line = infile.readline()
        submitScript.write("condor_submit {}\n".format(condorFileName))
    print("Condor submission files have been written to:\n{}/condorJobs".format(myOutputPath))
    print("You can submit your jobs with the script:\n{}".format(submitScriptName))

        
nArgs = len(sys.argv)

if nArgs == 1 or sys.argv[1].upper() == "CHARM":
    os.system("CreateFileList.pl -type 7 DST_HF_CHARM")
    makeCondorJob("CHARM")
elif sys.argv[1].upper() == "BOTTOM":
    os.system("CreateFileList.pl -type 8 DST_HF_BOTTOM")
    makeCondorJob("BOTTOM")
else:
    print("The argument, {}, was not known. Use CHARM or BOTTOM instead".format(sys.argv[1]))
