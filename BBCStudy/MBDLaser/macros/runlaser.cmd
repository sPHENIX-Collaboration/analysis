#!/usr/bin/env bash
#
# runs MbdLaser analysis
#

if [[ $# -lt 1 ]]
then
  echo 'Usage: runlaser.cmd <DST_UNCALMBD_fname>'
  exit -1
fi

echo PWD=${PWD}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo HOST=`hostname`

ulimit -c 0	# no core files

input_fname=$1

if echo ${input_fname} | grep '.root$'
then
  #out_fname=`echo $input_fname | sed 's/^.*DST_UNCALMBD/LASER/'`
  out_fname=${input_fname##*/}
  out_fname=LASER_${out_fname}
  echo $out_fname

  #savedir=G4Hits${input_fname##*/G4Hits}
  #savedir=${savedir%/*}
  savedir=LASER
  mkdir -p $savedir

  out_fname=${savedir}/${out_fname}
else
  # this is assumed to be a list file
  out_fname=`head -1 ${input_fname} |  sed 's/^.*DST_UNCALMBD/LASER/' | sed 's/-[0-9]*.root/.root/'`

  # create subdirectory
  savedir=${input_fname%.root}
  savedir=${input_fname##*/}
  #savedir=`echo ${savedir} | sed 's:-:/:'`
  echo mkdir -p ${savedir}
  mkdir -p ${savedir}
  #cd ${savedir}

  out_fname=${savedir}/${out_fname}

fi
echo $out_fname

nevents=0
if [[ $# -eq 2 ]]
then
  nevents=$2
fi

# for bbc analysis
#ln -sf ../../Run_MbdLaser.C .
echo root.exe -b -q Run_MbdLaser.C\(\"${input_fname}\",\"${out_fname}\",${nevents}\)
root.exe -b -q Run_MbdLaser.C\(\"${input_fname}\",\"${out_fname}\",${nevents}\)

