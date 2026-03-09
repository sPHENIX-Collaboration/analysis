#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.441

input_directory=$1
input_filename=$2
segment_index=$3

echo 
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}
echo segment_index: ${segment_index}

root.exe -l -b -q /sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/macros/RandomMerge_region.C\(\"${input_directory}\",\"${input_filename}\",${segment_index}\)

