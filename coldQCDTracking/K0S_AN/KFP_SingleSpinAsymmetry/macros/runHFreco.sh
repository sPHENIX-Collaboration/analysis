#!/bin/bash

# source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.477  #for ana475

#source /opt/sphenix/core/bin/sphenix_setup.sh -n new
#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.503 #for now because new is broken by tpc revert
# source /opt/sphenix/core/bin/setup_local.sh /sphenix/u/dloomis/install
#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.495 


source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=/sphenix/user/schen4/life/K0S_reconstruction/KFP_singlespinasymmetries/build/install

export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

echo $ROOT_INCLUDE_PATH | sed -e "s/:/\n/g"


# export USER="$(id -u -n)"
# export LOGNAME=${USER}
# export HOME=/sphenix/u/${LOGNAME}

#export MYINSTALL=${HOME}/install
#export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
#export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=$1
runnumber=$2
process=$3
inDir=$4
OutDir=$5

echo running: runHFreco.sh $*
root.exe -l -q -b Run_Fun4All_KFP_SSA.C\(${nEvents},${runnumber},${process},\"${inDir}\",\"${OutDir}\"\)
echo Script done


# run 79514: 03540
# run 79515: 04790
# run 79516: 37823
# run 79526: 34799
# run 79528: 37369
# run 79429: 33589