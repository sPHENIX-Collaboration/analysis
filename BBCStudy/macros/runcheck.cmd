#!/usr/bin/env bash
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
#let line=$1+1
#dst_fname=`sed -n "${line}p" dst_bbc_g4hit.list`
#dst_truth_fname=`sed -n "${line}p" dst_truth.list`

dst_fname=$1

# create subdirectory
#savedir=`printf "bbc%02d" ${1}`
#savedir=${dst_fname%.root}
#savedir=`echo ${savedir} | sed 's:-:/:'`
#echo mkdir -p ${savedir}
#mkdir -p ${savedir}
#cd ${savedir}

if echo ${dst_fname} | grep '.root$'
then
  out_fname=`echo $dst_fname | sed 's/^.*DST/CHECK/'`
else
  out_fname=`head -1 ${dst_fname} |  sed 's/^.*DST/CHECK/' | sed 's/-[0-9]*.root/.root/'`
fi
echo $out_fname

nevents=0
if [[ $# -eq 2 ]]
then
  nevents=$2
fi

# for bbc analysis
#ln -sf ../../Run_BbcCheck.C .
root.exe -b -q Run_BbcCheck.C\(${nevents},\"${dst_fname}\",\"${out_fname}\"\)

