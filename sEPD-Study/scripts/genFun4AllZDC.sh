#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

f4a_macro=${1}
input_list=${2}
nEvents=${3}
dbtag=${4}
logInterval=${5}
submitDir=${6}

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }

    # Check if the list file exists in the scratch dir
    if [[ ! -f "$input_list" ]]; then
        echo "Error: Input list file '$input_list' not found!" >&2
        exit 1
    fi

    # print the environment - needed for debugging
    printenv

    echo "Fetching all DST files at once..."
    # Transfer all files in the list upfront
    cut -d ',' -f 2 "$input_list" > dst_zdc.list
    getinputfiles.pl --verbose --filelist dst_zdc.list
    ls -lah

    # Loop over each line (DST file) in the input list to process them
    while IFS= read -r current_dst || [[ -n "$current_dst" ]]; do
        # Skip empty lines
        if [[ -z "$current_dst" ]]; then continue; fi

        # Ensure we are just using the filename in case the list had full paths
        local_file=$(basename "$current_dst")

        echo "--------------------------------------------------------"
        echo "Processing: $local_file"

        # Run the macro on the current local DST
        root -b -l -q "$f4a_macro(\"$local_file\", \"output\", $nEvents, \"$dbtag\", $logInterval)"

    done < dst_zdc.list

else
    echo "condor scratch NOT set" >&2
    exit 1
fi

echo "All Done and Transferring Files Back"
cp -rv output/. "$submitDir"

echo "Finished"
