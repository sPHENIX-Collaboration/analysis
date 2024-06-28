#!/bin/bash
# -----------------------------------------------------------------------------
# 'DoStandaloneCorrelatorCalculationOnCondor.sh'
# Derek Anderson
# 03.20.2024
#
# short script to run DoStandaloneCorrelatorCalculation.cxx via condor
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q "DoStandaloneCorrelatorCalculation.cxx(true)"

# end -------------------------------------------------------------------------
