#!/bin/bash


export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL=/sphenix/u/tmengel/installDir
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL


nEvents=-1

inputFiles="{"
for fileList in $@
do
  inputFiles+="\"${fileList}\","
done
inputFiles=${inputFiles::-1}
inputFiles+="}"
echo running: run_HFreco.sh $*
root.exe -q -b Fun4All_MDC2reco.C\(${inputFiles},$nEvents,ResonanceJetTagging::TAG::D0\)
echo Script done
