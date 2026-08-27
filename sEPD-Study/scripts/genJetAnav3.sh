#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

jetAna_bin=${1}
input=${2}
jet_pt_min=${3}
submitDir=${4}
do_iter=${5:-1}
do_mult=${6:-1}
do_unsub=${7:-1}
do_rcone=${8:-1}
lead_jet_pt_threshold=${9:-100}

# extract runnumber from file name
run=$(head -n 1 "$input" | grep -oP '(?<=/)\d+(?=/tree/)')
input_file=$(basename "$input")

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    mkdir input

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

$jetAna_bin "$input_file" 0 "$jet_pt_min" "$run" 0 "$do_iter" "$do_mult" "$do_unsub" "$do_rcone" "$lead_jet_pt_threshold"

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
