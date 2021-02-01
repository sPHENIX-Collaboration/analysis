import sys, os

def makeCondorJob(quarkFilter):
    print("Creating condor submission files for {} production".format(quarkFilter))
    inputFileList = "dst_hf_" + quarkFilter.lower() + ".list"
    infile = open(inputFileList, "r")
    line = infile.readline()
    myOutputPath = os.getcwd()
    while line:
        splitLine = line.split("/")
        fileName = splitLine[-1]
        fileName = fileName.replace('\n', '')
        productionNumber = line[-11: -6]

        os.makedirs("{}/condorJobs/log".format(myOutputPath), exist_ok=True)
        condorOutputInfo = "{0}/condorJobs/log/condor-{1}-{2}".format(myOutputPath, quarkFilter, productionNumber)

        condorFileName = open("condorJobs/condorJob_" + quarkFilter + "_" + productionNumber + ".job", "w")
        condorFileName.write("Universe        = vanilla\n")
        condorFileName.write("Executable      = {}/run_KFParticle.sh\n".format(myOutputPath))
        condorFileName.write("Arguments       = \"/sphenix/sim/sim01/sphnxpro/MDC1/HF_pp200_signal/data {}\"\n".format(fileName))
        condorFileName.write("Output          = {0}.out\n".format(condorOutputInfo))
        condorFileName.write("Error           = {0}.err\n".format(condorOutputInfo))
        condorFileName.write("Log             = {0}.log\n".format(condorOutputInfo))
        condorFileName.write("Initialdir      = {}\n".format(myOutputPath))
        condorFileName.write("PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)\n")
        condorFileName.write("request_memory = 4GB\n")
        condorFileName.write("Priority        = 20\n")
        condorFileName.write("job_lease_duration = 3600\n")
        condorFileName.write("Queue 1\n")
        line = infile.readline()
    print("Condor submission files have been written to {}/condorJobs".format(myOutputPath))
        
nArgs = len(sys.argv)

if nArgs == 1 or sys.argv[1].upper() == "CHARM":
    os.system("CreateFileList.pl -type 7 DST_HF_CHARM")
    makeCondorJob("CHARM")
elif sys.argv[1].upper() == "BOTTOM":
    os.system("CreateFileList.pl -type 8 DST_HF_BOTTOM")
    makeCondorJob("BOTTOM")
else:
    print("The argument, {}, was not known".format(sys.argv[1]))
