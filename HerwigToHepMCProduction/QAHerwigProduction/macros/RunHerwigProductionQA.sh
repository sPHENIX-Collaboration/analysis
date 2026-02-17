#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n 
MYINSTALL=/sphenix/user/sgross/install_dir
echo $MYINSTALL
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

seg=${1:-'0'}
trigger_type=${2:-"MB"}
herwig_file=${3:-''}
pythia_truth=${4:-''}
pythia_jet=${5:-''}

echo "Running Herwig check"
#/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/utils/bin/valgrind -s --track-origins=yes --log-file=$(pwd)/data_valgrind-out_1.txt 
/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/root-6.32.06/bin/root.exe -x -q $(pwd)/../macros/RunHerwigProductionQA.C\(${seg},\"${trigger_type}\",\"herwig\",\"$herwig_file\"\)
echo "Running Pythia check"
#/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/utils/bin/valgrind -s --track-origins=yes --log-file=$(pwd)/data_valgrind-out_1.txt 
/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/root-6.32.06/bin/root.exe -x -q $(pwd)/../macros/RunHerwigProductionQA.C\(${seg},\"${trigger_type}\",\"pythia\",\"$pythia_truth\",\"$pythia_jet\"\)
echo "All Done"
