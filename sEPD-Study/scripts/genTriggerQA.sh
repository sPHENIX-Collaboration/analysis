#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

f4a_macro=${1}
input=${2}
output=${3}
nEvents=${4}
dbtag=${5}
submitDir=${6}

# extract runnumber from file name
file=$(basename "$input")
IFS='-' read -r p1 p2 p3 <<< "$file"
run=$(echo "$p2" | sed 's/^0*//') # Remove leading zeros using sed

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    cut -d ',' -f 1 "$input" > dst_calofit.list
    getinputfiles.pl --verbose --filelist dst_calofit.list
    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

root -b -l -q "$f4a_macro(\"dst_calofit.list\", \"$run/$output\", $nEvents, \"$dbtag\")"

if [ $? -ne 0 ]; then
    echo "Error: ROOT macro crashed! Aborting transfer." >&2
    exit 1
fi

echo "All Done and Transferring Files Back"

# Define maximum retries and a counter
max_retries=3
count=0
success=0

while [ $count -lt $max_retries ]; do
    if cp -rv "$run" "$submitDir"; then
        success=1
        break
    else
        count=$((count + 1))
        echo "cp failed (likely GPFS lag). Retrying ($count/$max_retries) in 2 seconds..."
        sleep 2
    fi
done

if [ $success -eq 0 ]; then
    echo "Error: cp failed permanently after $max_retries attempts."
    exit 1
fi

echo "Finished"
