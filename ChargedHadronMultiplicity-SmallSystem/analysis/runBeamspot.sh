#! /bin/bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n new

export MYINSTALL=$HOME/install
export LD_LIBRARY_PATH=$MYINSTALL/lib:$LD_LIBRARY_PATH

source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

runNumber="${1:?Invalid run number}"

root -b -q "beamspot_2DJointMedian.C(${runNumber})"

echo all done
