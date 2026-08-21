#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

sepdQA_bin=${1}
input=${2}
nEvents=${3:-0}
submitDir=${4}
verbosity=${5:-0}

# extract runnumber from file name or first line
run=$(head -n 1 "$input" | grep -oP '(?<=/)\d+(?=/tree/)' || head -n 1 "$input" | grep -oP '\d{5,8}')
if [ -z "$run" ]; then
    file=$(basename "$input")
    IFS='_' read -r p1 p2 <<< "$file"
    run=$(echo "$p1" | sed 's/^0*//')
fi
input_file=$(basename "$input")

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    mkdir -p input

    echo "Copying files from list: $input"
    cat "$input" | xargs -I {} -P 4 cp -v {} input/

    realpath input/* > "$input_file"

    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run"

$sepdQA_bin "$input_file" "$nEvents" "$run" "$verbosity"

if [ $? -ne 0 ]; then
    echo "Error: sEPD-QA binary failed! Aborting transfer." >&2
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
