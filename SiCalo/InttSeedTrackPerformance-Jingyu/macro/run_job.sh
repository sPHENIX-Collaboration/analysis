#!/bin/bash
date

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
# Additional commands for my local environment
export SPHENIX=/sphenix/u/jzhang1
export MYINSTALL=$SPHENIX/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

cd /sphenix/user/jzhang1/INTT-EMCAL/InttSeedingTrackDev/InttSeedTrackPerformance/macro

runnumber=$2

echo "runnumber: $runnumber"

root.exe -b -q jyTrackPerformance.C\($runnumber\)
echo Script done

date