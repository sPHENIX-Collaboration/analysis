#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_bin=${1}
input=${2}
output=${3}
nEvents=${4}
dbtag=${5}
submitDir=${6}

# extract runnumber from file name
file=$(basename "$input")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
   getinputfiles.pl --filelist $input
   ls -lah *.root
   ls *.root > test.list
 else
   echo "condor scratch NOT set"
   exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

$f4a_bin "test.list" "$run" "$run/$output" "$nEvents" "$dbtag"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
