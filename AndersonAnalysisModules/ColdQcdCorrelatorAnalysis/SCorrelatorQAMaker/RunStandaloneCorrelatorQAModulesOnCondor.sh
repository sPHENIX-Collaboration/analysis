#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunStandaloneCorrelatorQAModulesOnCondor.sh'
# Derek Anderson
# 03.11.2024
#
# short script to run RunStandaloneCorrelatorQAModules.C via condor
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q RunStandaloneCorrelatorQAModules.cxx

# end -------------------------------------------------------------------------
