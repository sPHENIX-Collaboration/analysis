#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_macro=${1}
input_global=${2}
input_jet=${3}
input_g4hit=${4}
input_calo=${5}
input_calib=${6}
output=${7}
output_tree=${8}
nEvents=${9}
jet_pt_min=${10}
dbtag=${11}
submitDir=${12}

# Extract basenames
file_global=$(basename "$input_global")
file_jet=$(basename "$input_jet")
file_g4hit=$(basename "$input_g4hit")
file_calo=$(basename "$input_calo")
file_calib=$(basename "$input_calib")

# extract runnumber from file name
IFS='-' read -r p1 p2 p3 p4 p5 <<< "$file_global"
run=$(echo "$p4" | sed 's/^0*//') # Remove leading zeros using sed

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    cp -v "$input_global" "$input_jet" "$input_g4hit" "$input_calo" .
    test -e "$input_calib" && cp -v "$input_calib" .
    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run/hist" "$run/tree"

root -b -l -q "$f4a_macro(\"$file_global\", \"$file_jet\", \"$file_g4hit\", \"$file_calo\", \"$file_calib\", \"$run/hist/$output\", \"$run/tree/$output_tree\", $nEvents, $jet_pt_min, \"$dbtag\")"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
