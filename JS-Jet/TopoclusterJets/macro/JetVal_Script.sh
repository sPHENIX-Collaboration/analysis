#!/bin/bash

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/JS-Jet/JetValidation/macro/
export MYINSTALL=/sphenix/u/ecroft/install/

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
 
 # print the environment - needed for debugging
printenv

#echo $dataFileList
#simFileList=/sphenix/user/bkimelman/analysis/CaloEmbedAnalysis/macro/fileLists/simLists/simDST_$(printf "%04d" $1).list
#globalList=/sphenix/user/bkimelman/simGlobalLists/simGlobalDST_$(printf "%04d" $1).list
#truthList=/sphenix/user/bkimelman/simTruthLists/simTruthDST_$(printf "%04d" $1).list
#dataFileList=/sphenix/user/bkimelman/Run23745_ana399_DSTs/Run23745_ana399_DST_$(printf "%04d" $1).list
#dataFileList=/sphenix/u/ecroft/JS-Jet/JetValidation/macro/testList.list
dataFileList=/sphenix/lustre01/sphnxpro/physics/slurp/caloy2test/run_00047400_00047500/DST_CALO_run2pp_new_2024p004-00047474-00000.root



#echo simFileList: $simFileList
#echo globalList: $globalList
#echo truthList: $truthList
echo dataFileList: $dataFileList

 # use this for sim
#root.exe -q -b Fun4All_EMJetVal.C\(\"$truthList\",\"$simFileList\",\"$globalList\",\"/sphenix/u/jamesj3j3/analysis/JS-Jet/JetValidation/macro/condorTest/EMTree_$(
#printf "%04d" $1).root\"\)

 
 # use this for data

root.exe -q -b Fun4All_JetVal.C\(\"\",\"$dataFileList\",\"\",\"/sphenix/user/ecroft/JetValTest.root\",10.0\)
 
echo all done
