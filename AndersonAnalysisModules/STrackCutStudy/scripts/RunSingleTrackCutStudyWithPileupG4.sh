#!/bin/bash
# 'RunSingleTrackCutStudyWithPileupG4.sh'
# Derek Anderson
# 12.03.2022
#
# Short script to run the F4A macro
# (with pileup) for the Track Cut
# Study. Called by 'RunTrackCutStudyG4.job'
#
# NOTE: make sure that 'DisplayOn.C'
# and 'G4setup_sPHENIX.C' are in
# the same directory as you're
# running this in!

# set up environment
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n
printenv

# fixed parameters
nSkip=0

# run macro
root -b -q "Fun4All_G4_sPHENIX_ForTrackCutStudy_EmbedScanOff.C($1, \"$2\", \"$4\", \"$3\", $nSkip)"

# end -------------------------------------------------------------------------
