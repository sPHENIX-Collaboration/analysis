#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n 
MYINSTALL=/sphenix/user/sgross/install_dir
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

runN=${1:-'42959'}
segment=${2:-'0'}
hcal_file=${3:-'/sphenix/user/sgross/LED_and_Pedestal_DST/macro/Dst_HCAL_led_42959.root'}
emcal_file=${4:-'/sphenix/user/sgross/LED_and_Pedestal_DST/macro/Dst_CEMC_led_42959.root'}
event=${5:-'0'}
run_e2c_n=${6:-'5'}
run_e2c=true
if [[ $run_e2c_n -le 1 ]]; then
	run_e2c=false
fi
#/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/utils/bin/valgrind  --track-origins=yes --log-file=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/data_valdrind-out_1.txt /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/RunLargeRLENC_LEDPedestalScan.C\($runN,$segment,\"$hcal_file\",\"$emcal_file\",$event\)

/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/root-6.26.06.p01/bin/root.exe -x /sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/RunLargeRLENC_LEDPedestalScan.C\($runN,$segment,\"$hcal_file\",\"$emcal_file\",$event,$run_e2c\)


