#!/bin/csh

#setup default new environment for job
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.csh -n


# input parameters
echo "input process num: " $1
echo "nEvents: " $2
set runno = $1
set nevents = $2
set strout = $3

echo "In directory: " 
pwd

echo "Run number: " $runno
echo "events " $nevents
echo "outputFile: " $strout
echo "Executing with build contained in " $OFFLINE_MAIN
echo "Executing on date $(date +"%d-%m-%Y")"

# Construct the G4Hits DST files to access. These are MinBias 50 kHz pile up AuAu
# events
set strembed0=`printf "DST_TRUTH_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000004-0"`
set strembed1=`printf "DST_TRKR_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000004-0"`
set strembed2=`printf "DST_CALO_G4HIT_sHijing_0_20fm_50kHz_bkg_0_20fm-0000000004-0"`

echo $strembed0
echo $strembed1
echo $strembed2

git clone -b QA-tracking-mbhijing git@github.com:sPHENIX-Collaboration/macros.git
cd macros/detectors/sPHENIX

echo "pwd is: "
pwd

root -b -q  'Fun4All_G4_sPHENIX.C('$nevents', '$runno', "'$strembed0'" ,  "'$strembed1'", "'$strembed2'", "'$strout'" )'
