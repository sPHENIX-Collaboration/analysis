#!/bin/bash
F4A_file="Fun4All_RhoFluct.C"

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME} #(or /star/u/${LOGNAME} or /sphenix/u/${LOGNAME} or /eic/u/${LOGNAME})
 
source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $HOME/install_rhofluct
# source /opt/sphenix/core/bin/setup_local.sh $HOME/install_dual

# print the environment - needed for debugging
printenv

# this is how you run your Fun4All_G4_sPHENIX.C macro in batch: 
nevnt=$1
jet_R=$2
out_name=$3
calo_list=$4
bbc_list=$5
global_list=$6


# Define the source file and destination directory
if [ ! -e "$F4A_file" ]; then
    cp "../$F4A_file" "./"
fi


echo root.exe -q -l ../$F4A_file\($nevnt,$jet_R,\"$out_name\",\"$calo_list\",\"$bbc_list\",\"$global_list\",0\);
root.exe -q -l ../$F4A_file\($nevnt,$jet_R,\"$out_name\",\"$calo_list\",\"$bbc_list\",\"$global_list\",0\);

echo all done
