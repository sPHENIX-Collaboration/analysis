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
    while IFS= read -r dst; do
        getinputfiles.pl $dst
        echo "File Transferred: `readlink -f $dst`"
    done < "$inputJET"
 else
   echo "condor scratch NOT set"
   exit -1
fi

# print the environment - needed for debugging
printenv

$exe $inputJET $output

echo "All Done and Transferring Files Back"
cp -v $output $submitDir

echo "Finished"
