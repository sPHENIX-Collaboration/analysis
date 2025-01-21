#!/bin/bash
source /opt/sphenix/core/bin/sphenix_setup.sh -n 

file="Full_ppGoldenRunList_Version1.list"
file2="list/list_runs_missing_bad_tower_maps.txt"
array=()
array2=()

echo "Starting Loop 1"

while read -r line; do
   array+=("$line")
done < "$file"

echo "Starting Loop 2"

while read -r line; do
   array2+=("$line")
done < "$file2"

echo "Starting Loop 3" 

result=()
for item1 in "${array[@]}"; do
    found=false
    for item2 in "${array2[@]}"; do
        if [[ "$item1" == "$item2" ]]; then
            found=true
            break
        fi
    done
    if ! $found; then
        result+=("$item1")
    fi
done

echo "Starting Loop 4" 

for element in "${result[@]}"; do
    echo "$element" >> NJA_Full_ppGoldenRunList_Version1.list
done

