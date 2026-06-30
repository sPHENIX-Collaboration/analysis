#!/bin/bash

# Configuration via positional arguments
# Usage: ./merge_lists.sh <calo_dir> <zdc_dir> <sepd_dir> <output_dir> <zdc_prefix> <sepd_prefix>
CALO_DIR="${1:-./input_calo}"
ZDC_DIR="${2:-./input_zdc}"
SEPD_DIR="${3:-./input_sepd}"
OUTPUT_DIR="${4:-./output}"
ZDC_PREFIX="${5:-dst_zdc_raw}"
SEPD_PREFIX="${6:-dst_sepd_raw}"

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

echo "Scanning $CALO_DIR for calofitting lists..."
echo "Matching against $ZDC_PREFIX files in $ZDC_DIR..."
echo "Matching against $SEPD_PREFIX files in $SEPD_DIR..."

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
    sepd_list="$SEPD_DIR/${SEPD_PREFIX}-${run_number}.list"
    out_list="$OUTPUT_DIR/dst_calofitting_zdc_sepd-${run_number}.list"

    # Ensure the matching secondary lists actually exist before processing
    if [[ ! -f "$zdc_list" ]]; then
        echo "Skipping Run $run_number: Missing $ZDC_PREFIX list in $ZDC_DIR"
        continue
    fi
    if [[ ! -f "$sepd_list" ]]; then
        echo "Skipping Run $run_number: Missing $SEPD_PREFIX list in $SEPD_DIR"
        continue
    fi

    echo "Processing Run $run_number..."

    # Use awk to match segments and write to the output folder
    awk -F'-' '{
        # $NF gets the last field after the final dash (e.g., "00000.root")
        # split separates the segment number from the ".root" extension
        split($NF, arr, ".")
        segment = arr[1]

        if (FILENAME == ARGV[1]) {
            # File 1: ZDC list
            zdc_files[segment] = $0
        } else if (FILENAME == ARGV[2]) {
            # File 2: sEPD list
            sepd_files[segment] = $0
        } else {
            # Now reading the THIRD file (calo_list)
            # Check if this segment exists in our stored ZDC and sEPD arrays
            if (segment in zdc_files && segment in sepd_files) {
                # If it matches both, print all three, separated by a comma
                print $0 "," zdc_files[segment] "," sepd_files[segment]
            }
        }
    }' "$zdc_list" "$sepd_list" "$calo_list" > "$out_list"

done

echo "---"
echo "Done! Merged lists saved to: $OUTPUT_DIR"
