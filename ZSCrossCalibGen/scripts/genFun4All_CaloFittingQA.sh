#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

f4a_macro=${1}
input_seg_list=${2}
output_file=${3}
nEvents=${4:-0}
dbtag=${5:-newcdbtag}
submitDir=${6}
build_arg=${7}

if [ -n "$build_arg" ]; then
    build="$build_arg"
elif [ -n "$OFFLINE_MAIN" ]; then
    build="$(basename "$OFFLINE_MAIN")"
else
    build="new"
fi

source /opt/sphenix/core/bin/sphenix_setup.sh -n "$build"

# print the environment - needed for debugging
printenv

# Verify Condor scratch directory
if [[ -n "$_CONDOR_SCRATCH_DIR" && -d "$_CONDOR_SCRATCH_DIR" ]]; then
    cd "$_CONDOR_SCRATCH_DIR" || { echo "Failed to cd to $_CONDOR_SCRATCH_DIR" >&2; exit 1; }
    echo "Running in Condor scratch directory: $_CONDOR_SCRATCH_DIR"
else
    echo "Error: _CONDOR_SCRATCH_DIR is not set or not a directory" >&2
    exit 1
fi

mkdir -p "$submitDir/failures"

# Copy macros from submitDir into scratch directory
macro_name=$(basename "$f4a_macro")
if [ -f "$submitDir/$macro_name" ]; then
    cp -v "$submitDir/$macro_name" .
fi
if [ -f "$submitDir/Calo_Fitting.C" ]; then
    cp -v "$submitDir/Calo_Fitting.C" .
fi

# Extract run and segment identifiers
seg_file_name=$(basename "$input_seg_list")
if [[ "$seg_file_name" =~ run([0-9]+) ]]; then
    run="${BASH_REMATCH[1]}"
elif [[ "$seg_file_name" =~ -([0-9]{5,})- ]]; then
    run="${BASH_REMATCH[1]}"
else
    run="run"
fi

if [[ "$seg_file_name" =~ seg([0-9]+) ]]; then
    seg="${BASH_REMATCH[1]}"
else
    seg="00000"
fi

echo "=========================================================="
echo "Job execution started at $(date) on $(hostname)"
echo "Run: $run | Segment: $seg"
echo "Input segment list: $input_seg_list"
echo "Output target: $output_file"
echo "Events to process: $nEvents | CDB Tag: $dbtag"
echo "=========================================================="

if [ ! -f "$input_seg_list" ]; then
    echo "Error: Input segment list '$input_seg_list' not found!" >&2
    echo "Missing segment list file: $input_seg_list on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit 1
fi

# Generate local list containing basenames of expected input files
> local_input.list
while IFS= read -r f || [ -n "$f" ]; do
    f_clean=$(echo "$f" | tr -d '[:space:]')
    [ -z "$f_clean" ] && continue
    [[ "$f_clean" =~ ^# ]] && continue
    basename "$f_clean" >> local_input.list
done < "$input_seg_list"

num_expected=$(wc -l < local_input.list)
echo "Segment list contains $num_expected files."

# Fetch files via getinputfiles.pl using --filelist
echo "Fetching files via getinputfiles.pl --filelist $input_seg_list..."
if ! getinputfiles.pl --verbose --filelist "$input_seg_list"; then
    echo "Error: getinputfiles.pl failed for $input_seg_list at $(date) on $(hostname)" >&2
    echo "getinputfiles failure for $seg_file_name on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    while IFS= read -r root_f; do
        rm -f "$root_f"
    done < local_input.list
    exit 1
fi

# Verify all expected files exist in scratch and are non-empty
all_found=1
missing_files=0
while IFS= read -r root_f; do
    if [ ! -s "$root_f" ]; then
        echo "Error: Fetched file $root_f is missing or empty (0 bytes)!" >&2
        all_found=0
        missing_files=$((missing_files + 1))
    fi
done < local_input.list

if [ $all_found -eq 0 ]; then
    echo "Error: $missing_files files missing after getinputfiles.pl for $seg_file_name!" >&2
    echo "Missing $missing_files files after getinputfiles for $seg_file_name on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    while IFS= read -r root_f; do
        rm -f "$root_f"
    done < local_input.list
    exit 1
fi

echo "All $num_expected files successfully fetched and verified in scratch directory."

mkdir -p "$run/hist"

# show current dir state
ls -lah

echo "Launching ROOT macro: $macro_name..."
root -b -l -q "${macro_name}($nEvents, \"local_input.list\", \"$run/hist/$output_file\", \"$dbtag\")"
root_exit=$?

if [ $root_exit -ne 0 ]; then
    echo "Error: ROOT macro failed with exit code $root_exit at $(date) on $(hostname)! Aborting transfer." >&2
    echo "ROOT failure (exit code $root_exit) for $seg_file_name on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit $root_exit
fi

if [ ! -s "$run/hist/$output_file" ]; then
    echo "Error: Output file '$run/hist/$output_file' was not created or is empty!" >&2
    echo "Empty/missing output file $output_file for $seg_file_name on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit 1
fi

echo "Macro completed successfully at $(date)."
echo "Output file size: $(ls -lh "$run/hist/$output_file" | awk '{print $5}')"
echo "Transferring output back to $submitDir/output..."

mkdir -p "$submitDir/output"

# Retry loop for copying output directory back to GPFS/Lustre
max_retries=5
count=0
success=0

while [ $count -lt $max_retries ]; do
    if cp -rv "$run" "$submitDir/output"; then
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
    echo "CP transfer failure for $seg_file_name on $(hostname) at $(date)" >> "$submitDir/failures/failure-log.txt"
    exit 1
fi

echo "Finished successfully at $(date)"
