#!/bin/bash
#
# condor executes runana.cmd 0, runana.cmd 1, etc.
#

if [[ $# -lt 1 ]]
then
  echo 'Usage: runana.cmd jobno [nevents=0]'
  exit -1
fi

echo PWD=${PWD}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo HOST=`hostname`

ulimit -c 0	# no core files

# get dst fname
let line=$1+1
dst_fname=`sed -n "${line}p" dst_bbc_g4hit.list`
dst_truth_fname=`sed -n "${line}p" dst_truth.list`

# create subdirectory
#savedir=`printf "bbc%02d" ${1}`
savedir=${dst_fname%.root}
savedir=`echo ${savedir} | sed 's:-:/:'`
echo mkdir -p ${savedir}
mkdir -p ${savedir}
cd ${savedir}

pwd

nevents=0
if [[ $# -eq 2 ]]
then
  nevents=$2
fi

# for bbc analysis
ln -sf ../../Run_BBCStudy.C .
root.exe -b -q Run_BBCStudy.C\(${nevents},\"${dst_fname}\",\"${dst_truth_fname}\"\)

