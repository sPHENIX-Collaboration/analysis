#!/bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh /sphenix/user/verkest/install
 
# print the environment - needed for debugging
printenv
echo 1: $1
echo 2: $2
echo 3: $3

root.exe -q -b macros/Fun4All_HCalJetPhiShift.C\($1,$2,$3\)

echo all done
