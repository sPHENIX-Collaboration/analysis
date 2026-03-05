#! /bin/bash

caloDSTlist=${1:-""}
truthDSTlist=${2:-""}
jetDSTlist=${3:-""}
globalDSTlist=${4:-""}
outDir=${5:-""}
installdir=${6:-$MYINSTALL}
thisDir=${7:-""}
source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $installdir

n_files=`wc -l < $caloDSTlist`
/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/utils/bin/valgrind -s --track-origins=yes --log-file=valgrind.txt /cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/root-6.32.06/bin/root -b -q ${thisDir}/Fun4All_VandySkimmerTruth.C\(\"$caloDSTlist\",\"$jetDSTlist\",\"$truthDSTlist\",\"$globalDSTlist\",\"$outDir\",\"$n_files\"\)

