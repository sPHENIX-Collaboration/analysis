#!/bin/bash

export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n new

#export SPHENIX=/sphenix/u/${USER}/sPHENIX
#export SPHENIX=$HOME/sPHENIX
export MYINSTALL=/sphenix/u/rosstom/install
#export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
#export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
#
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=10

inputFiles="{"
for fileList in $@
do
  inputFiles+="\"${fileList}\","
done
inputFiles=${inputFiles::-1}
inputFiles+="}"
echo running: run_TrackSelectionKFP.sh $*
root.exe -q -b CommAnaTrackSelection.C\(${inputFiles},$nEvents\)
echo Script done
