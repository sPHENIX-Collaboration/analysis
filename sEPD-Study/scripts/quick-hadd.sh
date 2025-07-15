#!/usr/bin/env bash

input=${1}
output=${2}
log_file=${3}

mkdir -p "$output"
rm -f "$log_file"

ls "$input" | while read run; do
    echo "Processing: $run"
    hadd -n 51 "$output/test-$run.root" "$input/$run"/*.root | tee --append "$log_file"
done
