#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunMatcherComparatorOnCondor.sh'
# Derek Anderson
# 02.01.2024
#
# short script to run RunMatcherComparator.C via condor
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q RunMatcherComparator.C

# end -------------------------------------------------------------------------
