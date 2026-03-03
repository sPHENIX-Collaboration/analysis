#! /bin/bash

caloDSTlist=${1:-""}
truthDSTlist=${2:-""}
jetDSTlist=${3:-""}
globalDSTlist=${4:-""}
outDir=${5:-""}
n_files=wc -l $caloDSTlist
root -b -q Fun4All_VandySkimmer.C\(\"$caloDSTlist\",\"$jetDSTlist\",\"$truthDSTlist\",\"$globalDSTList\",\"$outDir\"\)

