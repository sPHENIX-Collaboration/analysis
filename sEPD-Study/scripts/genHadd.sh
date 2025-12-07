#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# Arguments:
# $1: input_list_file (Text file containing full paths to .root files)
# $2: output_filename (e.g., partial-run-0.root or run-123.root)
# $3: output_dir      (Where to copy the final result)

input_list_file=${1}
output_filename=${2}
output_dir=${3}

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]; then
    cd $_CONDOR_SCRATCH_DIR
else
    echo "condor scratch NOT set"
    exit -1
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
cp -v "$output_filename" "$output_dir"

echo "Finished"
