#! /bin/bash

caloDSTlist=${1:-""}
truthDSTlist=${2:-""}
jetDSTlist=${3:-""}
globalDSTlist=${4:-""}
outDir=${5:-""}
installdir=${6:-$MYINSTALL}

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $installdir

n_files=wc -l $caloDSTlist
root -b -q Fun4All_VandySkimmer.C\(\"$caloDSTlist\",\"$jetDSTlist\",\"$truthDSTlist\",\"$globalDSTList\",\"$outDir\"\)

