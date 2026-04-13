#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

jetAna_bin=${1}
input=${2}
input_f4a_qa=${3}
jet_pt_min=${4}
jet_eta_max=${5}
submitDir=${6}

# extract runnumber from file name
run=$(echo "$input" | grep -oP 'output/\K\d+(?=/tree)') # Remove leading zeros using sed
input_file=$(basename "$input")
input_f4a_qa_file=$(basename "$input_f4a_qa")

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    cp -v "$input" .
    cp -v "$input_f4a_qa" .
    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

$jetAna_bin "$input_file" "$input_f4a_qa_file" "$run" 0 "$jet_pt_min" "$jet_eta_max" "$run"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
