#!/bin/bash

source /sphenix/u/bseidlitz/setup.sh

echo "------------------setting up environment--------------------"
export Cur_dir=$(pwd)
echo "running area:" ${Cur_dir}
echo "-------------------------running----------------------------"
cd ${Cur_dir}
ls
root '' > notes.log



root -b "/sphenix/u/bseidlitz/work/cvpDev/analysis/EMCal_pi0_Calib_2023/macros/Fun4All_EMCal.C(0,\"inputdata.txt\",iteration,\"local_calib_copy.root\")"

echo "JOB COMPLETE!"
