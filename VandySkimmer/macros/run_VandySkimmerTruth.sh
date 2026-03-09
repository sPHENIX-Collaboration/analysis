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
root -b -q ${thisDir}/Fun4All_VandySkimmerTruth.C\(\"$caloDSTlist\",\"$jetDSTlist\",\"$truthDSTlist\",\"$globalDSTlist\",\"$outDir\",\"$n_files\"\)

