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
output=${4}
output_tree=${5}
nEvents=${6}
dbtag=${7}
do_event_plane_reco=${8}
submitDir=${9}

# extract runnumber from file name
file=$(basename "$input")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed
calib_file=$(basename "$input_calib")

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
then
    cd $_CONDOR_SCRATCH_DIR
    getinputfiles.pl --filelist $input
    test -e "$input_calib" && cp -v "$input_calib" .
    ls -lah
    ls DST*.root > test.list
else
    echo "condor scratch NOT set"
    exit -1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run/hist" "$run/tree"

$f4a_bin "test.list" "$run" "$calib_file" "$run/hist/$output" "$run/tree/$output_tree" "$nEvents" 0 0 "$dbtag" 1 "$do_event_plane_reco"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
