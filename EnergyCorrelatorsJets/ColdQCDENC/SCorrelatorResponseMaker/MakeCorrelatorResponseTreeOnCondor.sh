#!/bin/bash
# -----------------------------------------------------------------------------
# @file   MakeCorrelatorResponseTreeOnCondor.sh
# @author Derek Anderson
# @date   05.16.2024
#
# short script to run 'MakeCorrelatorResponseTree.cxx' macro via condor
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q "MakeCorrealtorResponseTree.cxx(true)"

# end -------------------------------------------------------------------------
