#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/analysis/JS-Jet/JetValidation/macro/
export MYINSTALL=/sphenix/u/jamesj3j3/sPHENIX/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.401
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

# print the environment - needed for debugging
printenv

dataFileList=/sphenix/user/bkimelman/Run23745_ana399_DSTs/Run23745_ana399_DST_$(printf "%04d" $1).list
echo dataFileList: $dataFileList


#root.exe -b -q Fun4All_EventSelection.C\(\"$dataFileList\",\"\",\"/sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/macro/Feb20_2024_bkgdTest/JetBkgdSub_tree_$(printf "%04d" $1)\",0.4\)

#root.exe -q -b Fun4All_EventSelection.C\(" ",\"$dataFileList\",\"/sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/macro/Feb20_2024_bkgdTest/JetBkgdSub_tree_$(printf "%04d" $1).root\",0.4\)

root.exe -q -b Fun4All_EventSelection.C\(\"$dataFileList\",\"/sphenix/user/jamesj3j3/analysis/JS-Jet/JetValidation/macro/Feb20_2024_bkgdTest/JetBkgdSub_tree_$(printf "%04d" $1).root\"\)


echo all done
