#!/bin/bash
#
#

if [[ $# -lt 1 ]]
then
  echo 'Usage: runupctrigstudy.cmd DST_CALO.root <nevents>'
  exit -1
fi

echo PWD=${PWD}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo HOST=`hostname`

ulimit -c 0	# no core files

# get dst fname
dst_calo_fname=$1

cd OUTPUT

nevents=0
if [[ $# -eq 2 ]]
then
  nevents=$2
fi

echo root.exe -b -q Fun4All_UPCTrigStudy.C\(${nevents},\"${dst_calo_fname}\"\)
root.exe -b -q Fun4All_UPCTrigStudy.C\(${nevents},\"${dst_calo_fname}\"\)

