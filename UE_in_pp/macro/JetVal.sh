#!/bin/bash
# file name: firstcondor.sh

source /opt/sphenix/core/bin/sphenix_setup.sh -n
export HOME=/sphenix/u/egm2153
export MYINSTALL=$HOME/install
source $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL
export SPHENIX=$MYINSTALL
root -l -b -q "Fun4All_JetVal.C(${1},${2},${3},\"${4}\",\"${5}\",\"${6}\")"
