#! /bin/bash

caloDSTlist=${1:-""}
truthDSTlist=${2:-""}
jetDSTlist=${3:-""}
globalDSTlist=${4:-""}
outDir=${5:-""}
installdir=${6:-$MYINSTALL}
thisDir=${7:-""}
Pythia_or_Herwig=${8:-'true'}
source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.542
source /opt/sphenix/core/bin/setup_local.sh $installdir

n_files=`wc -l < $caloDSTlist`
#/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/utils/bin/valgrind -s  --track-origins=yes --log-file=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/VandySkimmer/macros/data_valdrind-out_1.txt 
/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/root-6.32.06/bin/root -b -q ${thisDir}/Fun4All_VandySkimmerTruth.C\(\"$caloDSTlist\",\"$jetDSTlist\",\"$truthDSTlist\",\"$globalDSTlist\",\"$outDir\",\"$n_files\",\"$Pythia_or_Herwig\"\)

