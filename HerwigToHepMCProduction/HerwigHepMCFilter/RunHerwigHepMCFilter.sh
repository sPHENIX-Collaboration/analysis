#! /bin/bash
export MYINSTALL=/sphenix/user/sgross/install_dir
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
input_file=${1:-''}
jet_trigger=${2:-'10.'}
n_events=${3:-'1000'}
root.exe -x -b  /sphenix/user/sgross/coresoftware/generators/Herwig/HerwigHepMCFilter/RunHerwigHepMCFilter.C\(\"$input_file\",\"$jet_trigger\",$n_events\) 
