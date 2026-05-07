#!/bin/bash

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <parent_directory> <output_filename>"
    echo "Example: $0 /sphenix/tg/tg01/commissioning/.../jet20 my_list.list"
    exit 1
fi

PARENT_DIR=$(realpath "$1")
OUTPUT_FILE=$2

# Enable nullglob so that non-matching patterns expand to nothing
shopt -s nullglob

echo "Scanning $PARENT_DIR for OutDir folders..."

# Open a subshell to pipe the loop output directly into sort
for d in "$PARENT_DIR"/OutDir*; do
    # Assign matches to arrays
    g=("$d"/DST_GLOBAL*)
    t=("$d"/DST_TRUTH_JET*)
    s=("$d"/DST_TRUTH_G4HIT*)
    k=("$d"/DST_CALO_*)

    # Verify that all four file types exist in this directory
    if [[ -n ${g[0]} && -n ${t[0]} && -n ${s[0]} && -n ${k[0]} ]]; then
        echo "${g[0]},${t[0]},${s[0]},${k[0]}"
    fi
done | sort -V > "$OUTPUT_FILE"

echo "Success: Found files and saved to $OUTPUT_FILE"
