#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME} #(or /star/u/${LOGNAME} or /sphenix/u/${LOGNAME} or /eic/u/${LOGNAME})
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $HOME/install

# print the environment - needed for debugging
printenv

# this is how you run your Fun4All_G4_sPHENIX.C macro in batch: 
out_name=$1
nevnt=$2
l_calo_cluster=$3
l_bbc=$4

echo Running: root.exe -q -l ../Fun4All_RhoMedianFluct.C\(\"$out_name\",$nevnt,\"$l_calo_cluster\",\"$l_bbc\",0\) 
root.exe -q -l ../Fun4All_RhoMedianFluct.C\(\"$out_name\",$nevnt,\"$l_calo_cluster\",\"$l_bbc\",0\) 

echo all done
