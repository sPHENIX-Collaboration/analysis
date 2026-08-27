#!/bin/sh

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export MYINSTALL=/sphenix/user/rosstom/install/
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH

#echo "Events "$1
#echo "outFileName "$2
#echo "outDir "$3
#echo "Process ID "$4

#FUN4ALL_DEFAULT_INPUTFILE="https://www.phenix.bnl.gov/WWW/publish/phnxbld/sPHENIX/files/sPHENIX_G4Hits_sHijing_9-11fm_00000_00010.root"
#FUN4ALL_DEFAULT_EMBEDFILE="https://www.phenix.bnl.gov/WWW/publish/phnxbld/sPHENIX/files/sPHENIX_G4Hits_sHijing_9-11fm_00000_00010.root"
#FUN4ALL_DEFAULT_SKIP=0
#RUNNUMBER=1

root.exe -q -b Fun4All_HFG.C\(\"$1\"\,\"$2\"\)

#root -l -b -q Fun4All_G4_sPHENIX.C\($1,\"$FUN4ALL_DEFAULT_INPUTFILE\",\"$2\",\"$FUN4ALL_DEFAULT_EMBEDFILE\",$FUN4ALL_DEFAULT_SKIP,\"$3\",$RUNNUMBER,\"$4\"\)

echo "Finished running"
