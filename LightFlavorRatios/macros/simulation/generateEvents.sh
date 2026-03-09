#!/bin/bash

#source /opt/sphenix/core/bin/sphenix_setup.sh -n new

#export USER="$(id -u -n)"
#export LOGNAME=${USER}
#export HOME=/sphenix/u/${LOGNAME}

#export SPHENIX=${HOME}/sPHENIX
#export MYINSTALL=$SPHENIX/install
#export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
#export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
#
#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

process=$1
nEvents=$2
channel=$3
outDir=$4

root.exe -q -b Fun4All_HFG.C\(\"${process}\",${nEvents},\"${channel}\",\"${outDir}\"\)
echo Script done
