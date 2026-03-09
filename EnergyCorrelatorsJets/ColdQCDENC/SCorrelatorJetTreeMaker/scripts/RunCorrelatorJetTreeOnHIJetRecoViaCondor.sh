#!/bin/bash
# -----------------------------------------------------------------------------
# @file   RunCorrelatorJetTreeOnHIJetRecoViaCondor.sh
# @author Derek Anderson
# @date   09.23.2024
#
# Script to run correlator jet tree
# module via condor.
#
# Derived from code by Cameron Dean
# and Antonio Silva (thanks!!)
# -----------------------------------------------------------------------------

# grab home directory and set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL=/sphenix/u/danderson/install
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# add install path to LD_LIBRARY_PATH
echo $MYINSTALL
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$MYINSTALL/lib

# print library path for debugging
echo $LD_LIBRARY_PATH

# initialize no. of events to run and verbosity
nEvents=0
verbose=0

# create array of file lists
inputFiles="{"
for fileList in $@; do
  inputFiles+="\"${fileList}\","
done
inputFiles=${inputFiles::-1}
inputFiles+="}"

# run script
echo running: RunCorrelatorJetTreeOnHIJetRecoViaCondor.sh $*
root -b -q "Fun4All_RunJetTreeMakerOnHIJetRecoViaCondor.C($nEvents,$verbose,${inputFiles})"
echo Script done

# end -------------------------------------------------------------------------
