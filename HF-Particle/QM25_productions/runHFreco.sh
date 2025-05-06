#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.483

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

#export SPHENIX=${HOME}/sPHENIX
#export MYINSTALL=$SPHENIX/install
#export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
#export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
#
#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=$1

echo running: runHFreco.sh $*
root.exe -q -b Fun4All_HF_QM25.C\(${nEvents},\"$2\",\"$3\",$4\)
echo Script done
