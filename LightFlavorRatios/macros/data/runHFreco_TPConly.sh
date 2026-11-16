#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new 

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

echo myinstall is $MYINSTALL
#
#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=$1
inputDST=$2
inputDir=$3
nSkip=$4

echo running: runHFreco.sh $*
root.exe -q -b Fun4All_HF_TPConly.C\(${nEvents},\"${inputDST}\",\"${inputDir}\",${nSkip}\)
echo Script done
