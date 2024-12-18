#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
input=${2}
outputTree=${3}
outputQA=${4}
submitDir=${5}

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
 then
   cd $_CONDOR_SCRATCH_DIR
    # transfer the input file
    getinputfiles.pl $input
 else
   echo "condor scratch NOT set"
   exit -1
fi

echo "File Transferred: `readlink -f $input`"

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $outputTree $outputQA

echo "All Done and Transferring Files Back"
cp -v $outputQA $submitDir

echo "Finished"
