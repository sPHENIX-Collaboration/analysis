#!/bin/sh

#setup default new environment for job
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.sh -n


# input parameters
echo "input process num: " $1
echo "nEvents: " $2
runno=$1
nevents=$2
strout=$3

echo "In directory: " 
pwd

echo "Run number: " $runno
echo "events " $nevents
echo "outputFile: " $strout
echo "Executing with build contained in " $OFFLINE_MAIN
echo "Executing on date and time $(date +"%d-%m-%Y-%T")"

# Construct the G4Hits DST files to access. These are MinBias 50 kHz pile up AuAu
# events
strembed0="DST_TRUTH_G4HIT_pythia8_Charm_3MHz-0000000040-0"
strembed1="DST_TRKR_G4HIT_pythia8_Charm_3MHz-0000000040-0"
strembed2="DST_CALO_G4HIT_pythia8_Charm_3MHz-0000000040-0"

echo $strembed0
echo $strembed1
echo $strembed2


cd macros/detectors/sPHENIX

echo "pwd is: "
pwd

root -b -q  'Fun4All_G4_sPHENIX.C('$nevents', '$runno', "'$strembed0'" ,  "'$strembed1'", "'$strembed2'", "'$strout'" )'

mv $strout*.root ../../../../../../../../MinBias3MHzStreamingppOutput/
cd ../../../
mv logfiles/$strout.out ../../../../../MinBias3MHzStreamingppOutput/
