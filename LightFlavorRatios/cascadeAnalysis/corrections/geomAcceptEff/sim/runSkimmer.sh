#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

#Args:
# $1 = type (Kshort or Lambda)
# $2 = truth or reco tree (true for truth)
# $3 = N files (default is all) 
root.exe -l -q -b Xi_evaluator_skimmer.C 
