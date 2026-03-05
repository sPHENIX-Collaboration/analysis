#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_bin=${1}
input_global=${2}
input_jet=${3}
input_g4hit=${4}
input_calib=${5}
input_sEPD_BadTowers=${6}
output=${7}
nEvents=${8}
dbtag=${9}
submitDir=${10}

# Extract basenames
file_global=$(basename "$input_global")
file_jet=$(basename "$input_jet")
file_g4hit=$(basename "$input_g4hit")
file_calib=$(basename "$input_calib")
file_bad_towers=$(basename "$input_sEPD_BadTowers")

# extract runnumber from file name
IFS='-' read -r p1 p2 p3 p4 p5 <<< "$file_global"
run=$(echo "$p4" | sed 's/^0*//') # Remove leading zeros using sed

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
then
    cd $_CONDOR_SCRATCH_DIR
    cp -v "$input_global" "$input_jet" "$input_g4hit" .
    test -e "$input_calib" && cp -v "$input_calib" .
    test -e "$input_sEPD_BadTowers" && cp -v "$input_sEPD_BadTowers" .
    ls -lah
else
    echo "condor scratch NOT set"
    exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

$f4a_bin "$file_global" "$file_jet" "$file_g4hit" "$file_calib" "$file_bad_towers" "$run/$output" "$nEvents" "$dbtag"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
