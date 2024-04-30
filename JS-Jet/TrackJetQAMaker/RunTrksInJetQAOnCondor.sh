#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunTrksInJetQAOnCondor.sh'
# Derek Anderson
# 04.13.2024
#
# short script to run Fun4All_MakeTrksInJetQA.C via condor
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q "Fun4All_MakeTrksInJetQA.C"

# end -------------------------------------------------------------------------
