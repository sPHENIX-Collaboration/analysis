#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

initialSeed=$2
seed=$(($1+$initialSeed))
jobs=$3

root -b -l -q "Fun4All_G4_sPHENIX.C($jobs, $seed)"
mv G4sPHENIX_g4cemc_eval.root G4sPHENIX_g4cemc_eval-$1.root
