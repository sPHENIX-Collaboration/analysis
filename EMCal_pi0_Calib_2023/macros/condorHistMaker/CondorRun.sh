#!/bin/bash

source /sphenix/u/bseidlitz/setup.sh

echo "------------------setting up environment--------------------"
export Cur_dir=$(pwd)
echo "running area:" ${Cur_dir}
echo "-------------------------running----------------------------"
cd ${Cur_dir}
ls
root '' > notes.log



root -b "Fun4All_EMCal_Hist.C(0,\"inputdata.txt\")"

echo "JOB COMPLETE!"
