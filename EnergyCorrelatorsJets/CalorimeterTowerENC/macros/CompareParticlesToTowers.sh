#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n
MYINSTALL=/sphenix/user/sgross/install_dir
echo $MYINSTALL
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
runs=${5:-'0'}

root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/CompareParticlesToTowers.C\(\"$1\",\"$2\",\"$3\",\"$4\",\"$runs\"\)
#the structre is truth jet, calorimer tower file, truth reco and global files then the number of events, defaults to all events
