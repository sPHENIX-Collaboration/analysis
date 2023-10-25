#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME} #(or /star/u/${LOGNAME} or /sphenix/u/${LOGNAME} or /eic/u/${LOGNAME})
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
# source /opt/sphenix/core/bin/setup_local.sh $HOME/install
source /opt/sphenix/core/bin/setup_local.sh $HOME/install_dual

# print the environment - needed for debugging
printenv

# this is how you run your Fun4All_G4_sPHENIX.C macro in batch: 
jettruth_list=$1
calo_list=$2
global_list=$3
bbc_list=$4
out_name=$5

echo root.exe -q -l ../Fun4All_JetVal.C\(\"$jettruth_list\",\"$calo_list\",\"$global_list\",\"$bbc_list\",\"$out_name\"\);
root.exe -q -l ../Fun4All_JetVal.C\(\"$jettruth_list\",\"$calo_list\",\"$global_list\",\"$bbc_list\",\"$out_name\"\);

echo all done
