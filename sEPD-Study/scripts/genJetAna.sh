#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

jetAna_bin=${1}
input=${2}
jet_pt_min=${3}
jet_eta_max=${4}
submitDir=${5}

# extract runnumber from file name
run=$(echo "$input" | grep -oP 'output/\K\d+(?=/tree)') # Remove leading zeros using sed
input_file=$(basename "$input")

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
then
    cd $_CONDOR_SCRATCH_DIR
    cp -v "$input" .
    ls -lah
else
    echo "condor scratch NOT set"
    exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

$jetAna_bin "$input_file" "$run" 0 "$jet_pt_min" "$jet_eta_max" "$run"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
