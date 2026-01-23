#! /bin/bash
#export MYINSTALL=$MYINSTALL
#source /opt/sphenix/core/bin/sphenix_setup.sh -n ana
#source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

input_file=${1:-''}
trigger_type=${2:-'jet'}
trigger=${3:-'10.'}
n_events=${4:-'1000'}
n_gen=${5:-'100000'}
seg_filename=${6:-"none"}
this_dir=${7:-''}
root.exe -x -b -q   ${this_dir}RunHerwigHepMCFilter.C\(\"$input_file\",\"$trigger_type\",\"$trigger\",$n_events,$n_gen,\"$seg_filename\"\) 
