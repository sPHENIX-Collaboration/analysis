#!/bin/bash

# Configuration via positional arguments
# Usage: ./merge_lists.sh <calo_dir> <zdc_dir> <output_dir> <zdc_prefix>
CALO_DIR="${1:-./input_calo}"
ZDC_DIR="${2:-./input_zdc}"
OUTPUT_DIR="${3:-./output}"
ZDC_PREFIX="${4:-dst_zdc_raw}"

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

echo "Scanning $CALO_DIR for calofitting lists..."
echo "Matching against $ZDC_PREFIX files in $ZDC_DIR..."

# Loop through all calofitting lists in the input directory
for calo_list in "$CALO_DIR"/dst_calofitting-*.list; do

    # Guard against no files matching the pattern
    [ -e "$calo_list" ] || { echo "No dst_calofitting files found in $CALO_DIR"; exit 1; }

    # Extract the run number using parameter expansion
    # e.g., dst_calofitting-00066580.list -> 00066580
    base_name=$(basename "$calo_list")
    run_ext="${base_name#*-}"
    run_number="${run_ext%.*}"

    zdc_list="$ZDC_DIR/${ZDC_PREFIX}-${run_number}.list"
    out_list="$OUTPUT_DIR/dst_calofitting_zdc-${run_number}.list"

    # Ensure the matching secondary list actually exists before processing
    if [[ ! -f "$zdc_list" ]]; then
        echo "Skipping Run $run_number: Missing $ZDC_PREFIX list in $ZDC_DIR"
        continue
    fi

    echo "Processing Run $run_number..."

    # Use awk to match segments and write to the output folder
    awk -F'-' '{
        # $NF gets the last field after the final dash (e.g., "00000.root")
        # split separates the segment number from the ".root" extension
        split($NF, arr, ".")
        segment = arr[1]

        if (NR == FNR) {
            # NR == FNR is true only while reading the FIRST file (zdc_list)
            # Store the entire ZDC line in an array indexed by the segment number
            zdc_files[segment] = $0
        } else {
            # Now reading the SECOND file (calo_list)
            # Check if this segment exists in our stored ZDC array
            if (segment in zdc_files) {
                # If it matches, print both, separated by a comma
                print $0 "," zdc_files[segment]
            }
        }
    }' "$zdc_list" "$calo_list" > "$out_list"

done

echo "---"
echo "Done! Merged lists saved to: $OUTPUT_DIR"
