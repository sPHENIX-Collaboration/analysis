#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

# Arguments:
# $1: input_list_file (Text file containing full paths to .root files)
# $2: output_filename (e.g., partial-run-0.root or run-123.root)
# $3: output_dir      (Where to copy the final result)

input_list_file=${1}
output_filename=${2}
output_dir=${3}

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    echo "Reading inputs from: $input_list_file"
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# 1. Read the list file and copy those specific files to scratch
# This is safer than reading directly from GPFS/Lustre if network is busy
echo "Copying files from list..."
while read -r file; do
    cp -v "$file" .
done < "$input_list_file"

# 2. Run hadd
# We use *.root because we just copied them here.
# WARNING: Ensure the input_list_file doesn't contain the name of the output_filename!
echo "Running hadd..."
hadd -f -k "$output_filename" *.root

# 3. Transfer back
echo "Transferring output to $output_dir"

# Define maximum retries and a counter
max_retries=3
count=0
success=0

while [ $count -lt $max_retries ]; do
    if cp -v "$output_filename" "$output_dir"; then
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
