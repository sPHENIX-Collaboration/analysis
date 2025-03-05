#!/usr/bin/env bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

input_file=${1}
submitDir=${2}
run=$(basename $input_file | sed 's/[^0-9]*//g')
output="calib-000$run-0000.prdf"

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   # transfer the input file(s)
   while read fp; do
       cp -v $fp .
   done < "$input_file"
else
  echo "condor scratch NOT set"
  exit -1
fi

# print the environment - needed for debugging
printenv

eventcombiner -vpfi $output *.prdf

echo "All Done and Transferring Files Back"
cp -v $output $submitDir

echo "Finished"
