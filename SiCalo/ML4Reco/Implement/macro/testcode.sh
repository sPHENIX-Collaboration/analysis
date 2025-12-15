#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
# Additional commands for my local environment
export SPHENIX=/sphenix/u/jzhang1
export MYINSTALL=$SPHENIX/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

date

cd /sphenix/user/jzhang1/testcode4all/INTT-EMCAL/InttSeedingTrackDev/ML4Reco/Implement/macro/

root -b -q PtCalcMLTutorial.C

date
