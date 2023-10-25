#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME} #(or /star/u/${LOGNAME} or /sphenix/u/${LOGNAME} or /eic/u/${LOGNAME})
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $HOME/install
# source /opt/sphenix/core/bin/setup_local.sh $HOME/install_master

# print the environment - needed for debugging
printenv

# this is how you run your Fun4All_G4_sPHENIX.C macro in batch: 
nevnt=$2
jet_R=$3
out_name=$4
min_lead_truth_pt=$5
calo_list=$6
jettruth_list=$7
bbc_list=$8
global_list=$9
truthg4_list=${10}
verbosity=${11}

echo root.exe -q -l $1\($nevnt,$jet_R,\"$out_name\",$min_lead_truth_pt,\"$calo_list\",\"$jettruth_list\",\"$bbc_list\",\"$global_list\",\"$truthg4_list\",0\);
root.exe -q -l $1\($nevnt,$jet_R,\"$out_name\",$min_lead_truth_pt,\"$calo_list\",\"$jettruth_list\",\"$bbc_list\",\"$global_list\",\"$truthg4_list\",0\);

echo all done
