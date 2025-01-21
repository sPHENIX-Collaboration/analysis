#!/bin/bash
source /opt/sphenix/core/bin/sphenix_setup.sh -n 
#294
export ECCE=/sphenix/user/napple
export MYINSTALL=$ECCE/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

root.exe -q -b --web=off Fun4All_Ana_PPG09_Mod_Skim.C\(\"$1\",\"$2\",\"$3\"\)
