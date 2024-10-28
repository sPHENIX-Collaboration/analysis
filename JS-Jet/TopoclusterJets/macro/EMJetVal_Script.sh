#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/analysis/JS-Jet/JetValidation/macro/
export MYINSTALL=/sphenix/u/jamesj3j3/sPHENIX/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
 
 # print the environment - needed for debugging
printenv

#echo $dataFileList
#simFileList=/sphenix/user/bkimelman/analysis/CaloEmbedAnalysis/macro/fileLists/simLists/simDST_$(#printf "%04d" $1).list
#globalList=/sphenix/user/bkimelman/simGlobalLists/simGlobalDST_$(#printf "%04d" $1).list
#truthList=/sphenix/user/bkimelman/simTruthLists/simTruthDST_$(#printf "%04d" $1).list
dataFileList=/sphenix/user/bkimelman/Run23696_ana395_DSTs/Run23696_ana395_DST_$(printf "%04d" $1).list

#echo simFileList: $simFileList
#echo globalList: $globalList
#echo truthList: $truthList
echo dataFileList: $dataFileList

 # use this for sim
#root.exe -q -b Fun4All_EMJetVal.C\(\"$truthList\",\"$simFileList\",\"$globalList\",\"/sphenix/u/jamesj3j3/analysis/JS-Jet/JetValidation/macro/condorTest/EMTree_$(
#printf "%04d" $1).root\"\)

 
 # use this for data

root.exe -q -b Fun4All_EMJetVal.C\(\"\",\"$dataFileList\",\"$dataFileList\",\"/sphenix/u/jamesj3j3/analysis/JS-Jet/JetValidation/macro/condorTest/EMTree_$(printf "%04d" $1).root\"\)
 
echo all done
