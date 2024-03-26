import sys, os
from os import environ
import argparse

parser = argparse.ArgumentParser(description='sPHENIX MDC2 Reco Job Creator')
parser.add_argument('-i', '--inputType', default="HF_CHARM", help='Input type: PYTHIA8_PP_MB, HIJING_[0-20/0-4P88], HF_CHARM[D0], HF_BOTTOM[D0], JET_[10GEV/30GEV/PHOTON], SINGLE_PARTICLE')
parser.add_argument('-f', '--nFilesPerJob', default=5, type=int, help='Number of input files to pass to each job')
parser.add_argument('-t', '--nTotEvents', default=-1, type=int, help='Total number of events to run over')
parser.add_argument('-r', '--run', default=6, type=int, help='Production run to use')
parser.add_argument('--nopileup', help='Get data without pileup', action="store_true")
parser.add_argument('--truth', help='Enable truth DST reading', action="store_true")
parser.add_argument('--calo', help='Enable calo DST reading', action="store_true")
parser.add_argument('--trkr_hit', help='Enable tracker hit DST reading', action="store_true")
parser.add_argument('--bbc_g4hit', help='Enable BBC G4 hit DST reading', action="store_true")
parser.add_argument('--g4hit', help='Enable G4 hit DST reading', action="store_true")
parser.add_argument('--truth_table', help='Use DSTs for running tracking and making the truth/reco table', action="store_true")

args = parser.parse_args()

inputType = args.inputType.upper()

types = {'PYTHIA8_PP_MB' : 3, 'HIJING_0-20' : 4, 'HIJING_0-4P88' : 6, 'HF_CHARM' : 7, 'HF_BOTTOM' : 8, 'HF_CHARMD0' : 9, 'HF_BOTTOMD0' : 10
        , 'JET_30GEV' : 11, 'JET_10GEV' : 12, 'JET_PHOTON' : 13, 'SINGLE_PARTICLE' : 14 , 'D0JETS' : 16, 'D0JETS_5GEV' : 17, 'D0JETS_12GEV' : 18}
if inputType not in types:
  print("The argument, {}, was not known. Use --help to see available types".format(args.inputType))
  sys.exit()


dstSets = ['DST_TRACKS']
if args.truth:
    #args.g4hit = False
    dstSets.append('DST_TRUTH')
    #dstSets.append('DST_TRKR_G4HIT')
    dstSets.append('DST_TRACKSEEDS')
    dstSets.append('DST_TRKR_CLUSTER')
if args.calo: dstSets.append('DST_CALO_CLUSTER')
if args.trkr_hit: dstSets.append('DST_TRKR_HIT')
if args.bbc_g4hit:
    #args.g4hit = False
    dstSets.append('DST_BBC_G4HIT')
if args.g4hit: dstSets.append('G4Hits')
if args.truth_table:
    dstSets.append('DST_TRUTH_RECO')
    if args.truth == False: dstSets.append('DST_TRUTH')

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
    if myShell == '/bin/bash': condorFile.write("Executable         = $(initialDir)/run_MDC2reco.sh\n")
    if myShell == '/bin/tcsh': condorFile.write("Executable         = $(initialDir)/run_MDC2reco.csh\n")
    condorFile.write("PeriodicHold       = (NumJobStarts>=1 && JobStatus == 1)\n")
    condorFile.write("request_memory     = 4GB\n")
    condorFile.write("Priority           = 20\n")
    condorFile.write("job_lease_duration = 3600\n")
    condorFile.write("condorDir          = $(initialDir)/condorJob\n")
    condorOutputInfo = "$(condorDir)/log/condor-{0}-$INT(Process,%05d)".format(inputType)
    condorFile.write("Output             = {0}.out\n".format(condorOutputInfo))
    condorFile.write("Error              = {0}.err\n".format(condorOutputInfo))
    condorFile.write("Log                = /tmp/condor-{0}-$INT(Process,%05d).log\n".format(inputType))
    condorFile.write("Arguments          = \"{}\"\n".format(' '.join(listFileGeneric)))
    condorFile.write("Queue {}\n".format(nJob))
    print("Submission setup complete!")
    print("This setup will submit {} subjobs".format(nJob))
    print("You can submit your job with the script:\n{}".format(condorFileName))

catalogCommand = "CreateFileList.pl -run {0} -nop -type {1} {2}".format(args.run, types[inputType], ' '.join(dstSets))
if args.nTotEvents != -1: catalogCommand += " -n {}".format(args.nTotEvents)
if args.nopileup: catalogCommand += " -nopileup"
os.system(catalogCommand)
makeCondorJob()
