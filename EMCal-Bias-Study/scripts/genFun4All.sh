#!/usr/bin/env bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
input_file=${2}
events=${3}
doAllWaveforms=${4}
submitDir=${5}

# extract runnumber from file name
file=$(basename "$input_file")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed
output="test-$run.root"

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   # transfer the input file(s)
   cp -v $input_file .
else
  echo "condor scratch NOT set"
  exit -1
fi

# print the environment - needed for debugging
printenv

$exe $file $output $events $doAllWaveforms

echo "All Done and Transferring Files Back"
cp -v $output $submitDir

echo "Finished"
