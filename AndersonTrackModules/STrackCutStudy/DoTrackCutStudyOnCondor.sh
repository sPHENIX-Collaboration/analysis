#!/bin/bash
# -----------------------------------------------------------------------------
# 'DoTrackCutStudyOnCondor.sh'
# Derek Anderson
# 03.23.2023
#
# Called by 'DoTrackCutStudyOnCondor.job'
# -----------------------------------------------------------------------------

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh
printenv

# run macro
root -b -q "DoTrackCutStudy.C(true)"

# end -------------------------------------------------------------------------

