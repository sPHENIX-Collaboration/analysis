#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME} #(or /star/u/${LOGNAME} or /sphenix/u/${LOGNAME} or /eic/u/${LOGNAME})
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
# source /opt/sphenix/core/bin/setup_local.sh $HOME/install
source /opt/sphenix/core/bin/setup_local.sh $HOME/install_master

# print the environment - needed for debugging
printenv

# this is how you run your Fun4All_G4_sPHENIX.C macro in batch: 
nevnt=$1
jet_R=$2
out_name=$3
min_lead_truth_pt=$4
calo_list=$5
jettruth_list=$6
bbc_list=$7
global_list=$8
truthg4_list=$9
verbosity=$10

echo root.exe -q -l ../Fun4All_JetRhoMedian.C\($nevnt,$jet_R,\"$out_name\",$min_lead_truth_pt,\"$calo_list\",\"$jettruth_list\",\"$bbc_list\",\"$global_list\",\"$truthg4_list\",0\);
root.exe -q -l ../Fun4All_JetRhoMedian.C\($nevnt,$jet_R,\"$out_name\",$min_lead_truth_pt,\"$calo_list\",\"$jettruth_list\",\"$bbc_list\",\"$global_list\",\"$truthg4_list\",0\);

echo all done
