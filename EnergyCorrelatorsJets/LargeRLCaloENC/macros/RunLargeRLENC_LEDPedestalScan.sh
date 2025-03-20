#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n 
MYINSTALL=/sphenix/user/sgross/install_dir
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

runN=${1:-'42959'}
segment=${2:-'0'}
hcal_file=${3:-'/sphenix/user/sgross/LED_and_Pedestal_DST/macro/Dst_HCAL_led_42959.root'}
emcal_file=${4:-'/sphenix/user/sgross/LED_and_Pedestal_DST/macro/Dst_CEMC_led_42959.root'}
event=${5:-'0'}
skip=${6:-'0'}
/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/RunLargeRLENC_LEDPedestalScan.C\($runN,$segment,\"$hcal_file\",\"$emcal_file\",$event,$skip\)

