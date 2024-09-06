#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n
MYINSTALL=/sphenix/user/sgross/install_dir
echo $MYINSTALL
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
runs=${7:-'0'}

#/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/utils/bin/valgrind  --track-origins=yes --log-file=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/valdrind-out.txt 
/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/CompareParticlesToTowers.C\(\"$1\",\"$2\",\"$3\",\"$4\",\"$5\",\"$6\",\"$runs\"\)
#the structre is truth jet, calorimer tower file, truth reco and global files then the number of events, defaults to all events
