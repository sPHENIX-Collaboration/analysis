#! /bin/bash
 
#####A file to run the generic version of the file, to be called by the condor ######
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
export MYINSTALL=/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/src/build/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
#source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana

root -x /sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/macros/GetET.C\(\"$1\"\) 
