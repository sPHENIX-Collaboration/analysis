#!/bin/bash
# -----------------------------------------------------------------------------
# 'SwitchToCorrectBuild.sh'
# Derek Anderson
# 12.29.2022
#
# Use to switch to a particular
# build of sPHENIX core software.
# -----------------------------------------------------------------------------

# build needed
build="ana.322"

# run setup script
source /opt/sphenix/core/bin/sphenix_setup.sh -n $build

# end -------------------------------------------------------------------------
