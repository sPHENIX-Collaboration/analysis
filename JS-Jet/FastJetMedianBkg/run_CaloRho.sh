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
echo 4: $4
echo 5: $5
echo 6: $6

root.exe -q -b macro/Fun4All_CaloJetRho.C\($1,$2,$3,$4,$5,$6\)

echo all done
