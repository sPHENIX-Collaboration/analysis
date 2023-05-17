#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n ana.354
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

tag=$1
initialSeed=$2
seed=$(($tag+$initialSeed))
jobs=$3

# root -b -l -q "Fun4All_G4_sPHENIX.C($jobs, $seed, ".", "$tag")"
Fun4All_G4_sPHENIX $jobs $seed "." $tag
