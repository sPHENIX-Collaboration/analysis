#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

root.exe -l -q -b XiKS0_SingleBin_Calculator.C\(\"$1\",$2,$3\) 
