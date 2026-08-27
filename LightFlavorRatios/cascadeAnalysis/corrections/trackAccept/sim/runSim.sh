#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

root.exe -l -q -b Fun4All_G4_sPHENIX_silicon.C\(100,$1\) 
