#!/bin/bash

source /sphenix/u/bseidlitz/setup.sh

echo "------------------setting up environment--------------------"
export Cur_dir=$(pwd)
echo "running area:" ${Cur_dir}
echo "-------------------------running----------------------------"
cd ${Cur_dir}
ls
root '' > notes.log


# Get the file path from filedir.txt
if [[ -f filedir.txt ]]; then
  file_path=$(cat filedir.txt)
else
  echo "filedir.txt does not exist in the current directory. Exiting."
  exit 1
fi

# Run the ROOT macro with the file path as the second argument
root "/sphenix/u/bseidlitz/work/cvpDev/analysis/Calo_Validation_Package/macros//Fun4All_Calo.C(100,\"$file_path\")"

echo "JOB COMPLETE!"
