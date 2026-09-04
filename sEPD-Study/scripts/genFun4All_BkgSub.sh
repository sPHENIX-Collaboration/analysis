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
do_flow=${8}
eta_calib_path=${9}
event_list_path=${10}
do_rcone=${11:-0}
do_mult=${12:-1}
do_neg_energy_threshold=${13:-1}
neg_energy_threshold=${14:--2.0}
submitDir=${15}

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

# Check if eta_calib_path is a path or a keyword / empty
if [[ -z "$eta_calib_path" || "$eta_calib_path" == "none" || "$eta_calib_path" == "default" ]]; then
    eta_calib_file=""
else
    eta_calib_file=$(basename "$eta_calib_path")
fi

# Check if event_list_path is a path or a keyword / empty
if [[ -z "$event_list_path" || "$event_list_path" == "none" || "$event_list_path" == "default" ]]; then
    event_list_file=""
else
    event_list_file=$(basename "$event_list_path")
fi

if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]
then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }

    echo "Reading inputs from: $input"

    cut -d ',' -f 1 "$input" > dst_calofit.list
    cut -d ',' -f 2 "$input" > dst_zdc.list
    cut -d ',' -f 3 "$input" > dst_sepd.list

    getinputfiles.pl --verbose --filelist dst_calofit.list || {
        echo "Error: getinputfiles.pl failed for dst_calofit.list at $(date) on $(hostname)" >&2
        mkdir -p "$submitDir/failures"
        echo "getinputfiles failure (dst_calofit) for $file on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
        exit 1
    }
    getinputfiles.pl --verbose --filelist dst_sepd.list || {
        echo "Error: getinputfiles.pl failed for dst_sepd.list at $(date) on $(hostname)" >&2
        mkdir -p "$submitDir/failures"
        echo "getinputfiles failure (dst_sepd) for $file on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
        exit 1
    }

    # Create/clear a temporary file for the basenames
    > dst_zdc_local.list

    while IFS= read -r file; do
        # Skip empty lines if there are any
        [ -z "$file" ] && continue

        # Copy the file to the current directory
        cp -v "$file" .

        # Extract just the filename and save it to our local list
        basename "$file" >> dst_zdc_local.list
    done < dst_zdc.list

    # Overwrite the original list with the basename-only list
    mv dst_zdc_local.list dst_zdc.list

    test -e "$input_calib" && cp -v "$input_calib" .
    if [[ -n "$eta_calib_path" && "$eta_calib_path" != "none" && "$eta_calib_path" != "default" ]]; then
        test -e "$eta_calib_path" && cp -v "$eta_calib_path" .
    fi
    if [[ -n "$event_list_path" && "$event_list_path" != "none" && "$event_list_path" != "default" ]]; then
        test -e "$event_list_path" && cp -v "$event_list_path" .
    fi
    ls -lah
else
    echo "condor scratch NOT set" >&2
    exit 1
fi

# print the environment - needed for debugging
printenv

mkdir -p "$run/hist" "$run/tree"

echo "Starting ROOT macro at $(date) on $(hostname)"
root -b -l -q "$f4a_macro(\"dst_calofit.list\", \"dst_zdc.list\", \"dst_sepd.list\", \"$calib_file\", \"$run/hist/$output\", \"$run/tree/$output_tree\", $do_flow, $nEvents, 0, 0, \"$dbtag\", \"$eta_calib_file\", \"$event_list_file\", $do_rcone, $do_mult, \"\", $do_neg_energy_threshold, $neg_energy_threshold)"

root_exit=$?
if [ $root_exit -ne 0 ]; then
    echo "Error: ROOT macro failed with exit code $root_exit at $(date) on $(hostname)! Aborting transfer." >&2
    mkdir -p "$submitDir/failures"
    echo "ROOT failure (exit code $root_exit) for $file on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit $root_exit
fi

echo "All Done and Transferring Files Back at $(date)"

# Define maximum retries and a counter
max_retries=5
count=0
success=0

while [ $count -lt $max_retries ]; do
    if cp -rv "$run" "$submitDir"; then
        success=1
        break
    else
        count=$((count + 1))
        echo "cp failed (likely GPFS lag). Retrying ($count/$max_retries) in 15 seconds..." >&2
        sleep 15
    fi
done

if [ $success -eq 0 ]; then
    echo "Error: cp failed permanently after $max_retries attempts at $(date)." >&2
    mkdir -p "$submitDir/failures"
    echo "CP transfer failure for $file on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit 1
fi

echo "Finished successfully at $(date)"
