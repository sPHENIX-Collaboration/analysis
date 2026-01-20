#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_bin=${1}
input=${2}
input_calib=${3}
input_sEPD_BadTowers=${4}
output=${5}
output_tree=${6}
nEvents=${7}
dbtag=${8}
submitDir=${9}

# extract runnumber from file name
file=$(basename "$input")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed
calib_file=$(basename "$input_calib")
sEPD_BadTowers_file=$(basename "$input_sEPD_BadTowers")

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
then
    cd $_CONDOR_SCRATCH_DIR
    getinputfiles.pl --filelist $input
    test -e "$input_calib" && cp -v "$input_calib" .
    test -e "$input_sEPD_BadTowers" && cp -v "$input_sEPD_BadTowers" .
    ls -lah
    ls DST*.root > test.list
else
    echo "condor scratch NOT set"
    exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run/hist" "$run/tree"

$f4a_bin "test.list" "$run" "$calib_file" "$sEPD_BadTowers_file" "$run/hist/$output" "$run/tree/$output_tree" "$nEvents" 0 0 "$dbtag" 1

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
