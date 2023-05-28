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
pTlb=$2
nevnt=$3
l_calo_cluster=$4
l_truth_cluster=$5

echo Running: root.exe -q -l ../Fun4All_JetRhoMedian.C\(\"$out_name\",$pTlb,$nevnt,\"$l_calo_cluster\",\"$l_truth_cluster\",\"\",0\) 
root.exe -q -l ../Fun4All_JetRhoMedian.C\(\"$out_name\",$pTlb,$nevnt,\"$l_calo_cluster\",\"$l_truth_cluster\",\"\",0\) 

echo all done
