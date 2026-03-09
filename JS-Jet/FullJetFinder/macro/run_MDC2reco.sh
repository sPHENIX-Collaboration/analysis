#!/bin/bash

export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL=/sphenix/u/jkvapil/sw/install
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL

nEvents=0

inputFiles="{"
for fileList in "${@:2}"
do
  inputFiles+="\"${fileList}\","
done
inputFiles=${inputFiles::-1}
inputFiles+="}"
echo running: Fun4All_FullJetFinder.sh $*
root.exe -q -b Fun4All_FullJetFinder.C\(\"$1\",${inputFiles},$nEvents\)
echo Script done
