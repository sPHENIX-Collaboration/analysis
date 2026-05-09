#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_macro=${1}
input=${2}
input_calib=${3}
output=${4}
output_tree=${5}
nEvents=${6}
dbtag=${7}
submitDir=${8}

# extract runnumber from file name
file=$(basename "$input")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed

# Check if input_calib is a path or a keyword
if [[ "$input_calib" == "default" ]]; then
    calib_file="default"
else
    calib_file=$(basename "$input_calib")
fi

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    cut -d ',' -f 1 "$input" > dst_calofit.list
    cut -d ',' -f 2 "$input" > dst_zdc.list
    getinputfiles.pl --verbose --filelist dst_calofit.list
    getinputfiles.pl --verbose --filelist dst_zdc.list
    test -e "$input_calib" && cp -v "$input_calib" .
    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run/hist" "$run/tree"

root -b -l -q "$f4a_macro(\"dst_calofit.list\", \"dst_zdc.list\", \"$calib_file\", \"$run/hist/$output\", \"$run/tree/$output_tree\", $nEvents, 0, 0, \"$dbtag\")"

echo "All Done and Transferring Files Back"
cp -rv "$run" "$submitDir"

echo "Finished"
