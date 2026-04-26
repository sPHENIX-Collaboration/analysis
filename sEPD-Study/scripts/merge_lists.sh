#!/bin/bash

# Define your input and output directories here
# You can also pass them as arguments: ./merge_lists.sh <input_dir> <output_dir>
INPUT_DIR="${1:-./input}"
OUTPUT_DIR="${2:-./output}"

# Create the output directory if it doesn't exist
mkdir -p "$OUTPUT_DIR"

# Loop through all calofitting lists in the input directory
for calo_list in "$INPUT_DIR"/dst_calofitting-*.list; do

    # Guard against no files matching the pattern
    [ -e "$calo_list" ] || { echo "No dst_calofitting files found in $INPUT_DIR"; exit 1; }

    # Extract the run number using parameter expansion
    # e.g., dst_calofitting-00066580.list -> 00066580
    base_name=$(basename "$calo_list")
    run_ext="${base_name#*-}"
    run_number="${run_ext%.*}"

    zdc_list="$INPUT_DIR/dst_zdc_raw-${run_number}.list"
    out_list="$OUTPUT_DIR/dst_calofitting_zdc-${run_number}.list"

    # Ensure the matching ZDC list actually exists before processing
    if [[ ! -f "$zdc_list" ]]; then
        echo "Skipping Run $run_number: Missing corresponding ZDC list."
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

echo "Done! Merged lists saved to: $OUTPUT_DIR"
