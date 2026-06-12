#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.543 

export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}

# uncomment for local development
#export MYINSTALL=${HOME}/install
#export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH
#export ROOT_INCLUDE_PATH=$MYINSTALL/include:$ROOT_INCLUDE_PATH
#export PATH="$HOME/.local/bin:$PATH"
#
# source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

useScratch=false

#if ${useScratch}=true; then
#  this_script=$BASH_SOURCE
#  this_script=`readlink -f $this_script`
#  this_dir=`dirname $this_script`
#  echo rsyncing from $this_dir
#  echo running: $this_script $*
#  
#  if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]
#  then
#    cd $_CONDOR_SCRATCH_DIR
#    rsync -av $this_dir/* .
#  else
#    echo condor scratch NOT set
#    exit -1
#  fi
#fi

nEvents=$1
inDst=$2
outDir=$3
nSkip=$4

#if ${useScratch}=true; then
#  if [[ "${inDst}" == *.root ]]; then
#    getinputfiles.pl $inDst
#  elif [[ "${inDst}" == *.list ]]; then
#    getinputfiles.pl --filelist $inDst
#  fi
#fi

# print the environment - needed for debugging
#printenv

echo running: runHFreco.sh $*
root.exe -q -b /sphenix/user/rosstom/TrackingAnalysis/Physics_Val_TF/data/macro/Fun4All_HF.C\(${nEvents},\"${inDst}\",\"${outDir}\",${nSkip}\)
echo Script done
