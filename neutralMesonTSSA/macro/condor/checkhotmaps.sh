#!/bin/bash

echo "Sourcing sPHENIX setup scripts"
source /opt/sphenix/core/bin/sphenix_setup.sh new -n
source $OPT_SPHENIX/bin/setup_local.sh /sphenix/user/gmattson/install
nProcess=$1

cd /sphenix/user/gmattson/analysis/neutralMesonTSSA/macro/condor
infile="job_filelists/hotmapcheck/job-$nProcess.list"
echo "DST file is $infile"
outfiledir="/sphenix/tg/tg01/coldqcd/gmattson/neutralMesonTSSA/pass1/dummyhists"
mkdir -p "$outfiledir"
outfile="$outfiledir/dummyhists-$nProcess.root"
echo "Output hists will be stored in $outfile"

# For 2024 RD
time root -b -q ../Fun4All_checkHotMaps.C'(1, "'$infile'", "'$outfile'")'
