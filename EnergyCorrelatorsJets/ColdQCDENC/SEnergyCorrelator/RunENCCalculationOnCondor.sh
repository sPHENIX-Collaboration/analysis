#!/bin/bash
# -----------------------------------------------------------------------------
# \file   RunENCCalculationOnCondor.sh
# \author Derek Anderson
# \date   03.20.2024
#
# short script to run DoStandaloneCorrelatorCalculation.cxx via condor
# -----------------------------------------------------------------------------

# set up environment
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

# run macro
root -b -q "RunENCCalculation.cxx(true)"

# end -------------------------------------------------------------------------
