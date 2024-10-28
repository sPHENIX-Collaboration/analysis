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

dst_fname=$1

if echo ${dst_fname} | grep '.root$'
then
  #out_fname=`echo $dst_fname | sed 's/^.*DST/CHECK/'`
  out_fname=${dst_fname##*/}
  out_fname=CHK_${out_fname}
echo $out_fname

  #savedir=G4Hits${dst_fname##*/G4Hits}
  #savedir=${savedir%/*}
  savedir=CHECK
  mkdir -p $savedir

  out_fname=${savedir}/${out_fname}
else
  # this is assumed to be a list file

  # create subdirectory
  savedir=CHECK
  echo mkdir -p ${savedir}
  mkdir -p ${savedir}

  out_fname=`head -1 ${dst_fname} |  sed 's/^.*DST/CHECK/' | sed 's/-[0-9]*.root/.root/'`
  out_fname=${savedir}/${out_fname}

fi
echo $out_fname

nevents=0
if [[ $# -eq 2 ]]
then
  nevents=$2
fi

# for bbc analysis
#ln -sf ../../Run_BbcCheck.C .
echo root.exe -b -q Run_BbcCheck.C\(${nevents},\"${dst_fname}\",\"${out_fname}\"\)
root.exe -b -q Run_BbcCheck.C\(${nevents},\"${dst_fname}\",\"${out_fname}\"\)

