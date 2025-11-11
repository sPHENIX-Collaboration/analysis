#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

input=${1}
hadd_max=${2}
submitDir=${3}

# extract runnumber from input dir
run=$(basename "$input")

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
then
    cd $_CONDOR_SCRATCH_DIR

    # Create a single target directory for the files
    mkdir "$run"

    echo "Attempting to copy old structure: $input/*.root"
    # Try copying the "old" structure first.
    # 1>/dev/null 2>&1 silences "No such file or directory" errors.
    cp -v "$input"/*.root "$run" 1>/dev/null 2>&1

    # Check if any .root files were *actually* copied.
    # We use 'find' and 'head' to check for *any* file.
    if ! find "$run" -name "*.root" -print -quit | grep -q .; then
        echo "Old structure not found or no .root files."
        echo "Attempting to copy new structure: $input/hist/*.root"
        
        # If no files, try copying the "new" structure
        # This only copies files from /hist, ignoring /tree
        cp -v "$input"/hist/*.root "$run" 1>/dev/null 2>&1
    fi
    
    # Check again. If still no files, then we have a problem.
    if ! find "$run" -name "*.root" -print -quit | grep -q .; then
        echo "ERROR: No .root files found in $input/*.root OR $input/hist/*.root. Exiting."
        exit 1
    fi

else
    echo "condor scratch NOT set"
    exit -1
fi

# print the environment - needed for debugging
printenv

hadd -n "$hadd_max" "test-$run.root" "$run"/*.root

echo "All Done and Transferring Files Back"

cp -v "test-$run.root" "$submitDir"

echo "Finished"
