#!/bin/bash
# -----------------------------------------------------------------------------
# 'RunNewTruthMatcherOnCondor.sh'
# Derek Anderson
# 08.10.2023
#
# Short script to run F4A driver macro
# to test new truth-track matcher. Called
# by 'RunNewTruthMatcherOnCondor.job'
#
# NOTE: make sure that 'G4setup_sPHENIX.C'
# is in the same directory
# -----------------------------------------------------------------------------

# parameters
nEvt=500
nSkip=0

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
export MYINSTALL=/sphenix/user/danderson/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
printenv

# run macro
root -b -q "Fun4All_RunNewTruthMatcher.C($nEvt, $nSkip, \"$1\")"

# end -------------------------------------------------------------------------
