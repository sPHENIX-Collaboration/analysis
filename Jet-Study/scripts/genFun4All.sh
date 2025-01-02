#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
inputJET=${2}
output=${3}
submitDir=${4}

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   # transfer the input file
   getinputfiles.pl --filelist $inputJET
else
  echo "condor scratch NOT set"
  exit -1
fi

# extract runnumber from file name
file=$(basename "$inputJET")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p3" | sed 's/^0*//' | sed 's/\.[^.]*$//') # Remove leading zeros and extension

# print the environment - needed for debugging
printenv

mkdir -p $run

$exe $inputJET $run/$output

echo "All Done and Transferring Files Back"
cp -rv $run $submitDir

echo "Finished"
