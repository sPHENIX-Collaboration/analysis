#!/bin/bash

source /cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.sh -n

cd /sphenix/sim/sim01/sphnxpro/TrackingDailyBuild/

if [ ! -d "$(date +"%Y")" ]; then
 mkdir "$(date +"%Y")"
fi

cd "$(date +"%Y")"

if [ ! -d "$(date +"%d-%m")" ]; then
  mkdir "$(date +"%d-%m")"
fi
cd "$(date +"%d-%m")"

mkdir MinBias3MHzStreamingppOutput

git clone https://github.com/sPHENIX-Collaboration/analysis.git MinBias3MHzStreamingpp

cd MinBias3MHzStreamingpp/Tracking/BenchmarkingTools/DailyCronJobScripts/MinBias3MHzStreamingpp

mkdir logfiles

git clone -b QA-tracking-streamingpp https://github.com/sPHENIX-Collaboration/macros.git

condor_submit Run3MHzMBStreamingpp.job
