#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n 
MYINSTALL=/sphenix/user/sgross/install_dir
echo $MYINSTALL
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

data_dst=${1:-'none'}
truth_jet_dst=${2:-'none'}
calo_reco_dst=${3:-'none'}
truth_reco_dst=${4:-'none'}
global_reco_dst=${5:-'none'}
runs=${6:-'0'}
minpt=${7:='1.0'}

/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLENC/macros/LargeRLENC.C\(\"$data_dst\",\"$truth_jet_dst\",\"$calo_reco_dst\",\"$truth_reco_dst\",\"$global_reco_dst\",\"$runs\",\"$minpt\"\)

