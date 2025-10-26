#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

input=${1}
hadd_max=${2}
submitDir=${3}

# extract runnumber from input dir
run=$(basename "$input")

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   cp -rv "$input" "$run"
   ls -lah
 else
   echo "condor scratch NOT set"
   exit -1
fi

# print the environment - needed for debugging
printenv

hadd -n "$hadd_max" "test-$run.root" "$run"/*.root

echo "All Done and Transferring Files Back"

cp -v "test-$run.root" "$submitDir"

echo "Finished"
