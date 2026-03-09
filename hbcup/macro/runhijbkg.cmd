#!/bin/bash
#
# condor executes runana.cmd 0, runana.cmd 1, etc.
#

if [[ $# -lt 1 ]]
then
  echo 'Usage: runhijbkg.cmd jobno [nevents=0]'
  exit -1
fi

echo PWD=${PWD}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo HOST=`hostname`

ulimit -c 0	# no core files

# get dst fname
let line=$1+1

# Kludge
if [[ $PWD == "/phenix/u/chiu/sphenix/sphenix_bbc/offline/analysis/hbcup/workarea" ]]
then
 let line=$1+20001
fi

dst_truth_fname=`sed -n "${line}p" dst_truth.list`

# create subdirectory
savedir=${dst_truth_fname%.root}
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
ln -sf ../../Fun4All_hijbkg.C .
root.exe -b -q Fun4All_hijbkg.C\(${nevents},\"${dst_truth_fname}\"\)

