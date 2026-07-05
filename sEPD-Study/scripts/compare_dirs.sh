#!/bin/bash

# This script compares two directories to find files that share the exact same name but have different contents.
#  - Matches by name: It looks for files that exist in both Folder A and Folder B. It automatically ignores files that only exist in one folder or the other.
#  - Compares content efficiently: Instead of reading through every line of text, it checks the files byte-by-byte. The moment it finds a single mismatch, it stops reading that file and flags it.
#  - Outputs the differences: It prints out a clean list of just the filenames that don't match, giving you a quick summary of what has changed between the two locations.

# 1. Sanity check: Ensure exactly two arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <directory1> <directory2>" >&2
    exit 1
fi

dir1="$1"
dir2="$2"

# 2. Verify both inputs are actually directories
if [ ! -d "$dir1" ] || [ ! -d "$dir2" ]; then
    echo "Error: Both arguments must be valid directories." >&2
    exit 1
fi

echo "Files with the same name but different contents:"
echo "-----------------------------------------------"

# 3. Loop through files in the first directory
for file1 in "$dir1"/*; do
    # Ensure it's a regular file (skips subdirectories)
    [ -f "$file1" ] || continue

    # Extract just the filename using fast string manipulation (no slow basename fork)
    filename="${file1##*/}"
    file2="$dir2/$filename"

    # 4. Check if a file with the exact same name exists in directory 2
    if [ -f "$file2" ]; then
        # 'cmp -s' returns 0 if identical, 1 if different.
        # The '!' negates it so it runs if they differ.
        if ! cmp -s "$file1" "$file2"; then
            echo "$filename"
        fi
    fi
done
