#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

nFiles=500
startFile=$1

root.exe -l -q -b parseEvaluators.C\(\"Kshort\",${nFiles},${startFile}\)
root.exe -l -q -b parseEvaluators.C\(\"Lambda0\",${nFiles},${startFile}\)
