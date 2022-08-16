#!/bin/bash

source /cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.sh -n new.1

cd /sphenix/sim/sim01/sphnxpro/TrackingDailyBuild/
if [ ! -d "$(date +"%Y")" ]; then
 mkdir "$(date +"%Y")"
fi

cd "$(date +"%Y")"

if [ ! -d "$(date +"%d-%m")" ]; then
  mkdir "$(date +"%d-%m")"
fi
cd "$(date +"%d-%m")"

git clone git@github.com:sPHENIX-Collaboration/analysis.git

cd analysis/Tracking/BenchMarkingTools/DailyCronJobScripts/MinBias50kHzHijing
mkdir logfiles

condor_submit Run50kHzMBHijing.job
