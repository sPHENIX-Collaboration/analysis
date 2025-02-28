#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.441

IsShuffle=$1
N_merged_files=$2
input_directory=$3
input_filename=$4

echo 
echo IsShuffle: ${IsShuffle}
echo N_merged_files: ${N_merged_files}
echo input_directory: ${input_directory}
echo input_filename: ${input_filename}


root.exe -l -b -q /sphenix/user/ChengWei/INTT/INTT_dNdeta_repo/NewCode2024/macros/RandomMerge.C\(${IsShuffle},${N_merged_files},\"${input_directory}\",\"${input_filename}\"\)

