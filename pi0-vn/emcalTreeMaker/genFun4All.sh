#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
input=${2}
qa=${3}
diphoton=${4}
do_pi0=${5}
z=${6}
sim=${7}
input_global=${8}
input_g4hits=${9}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $qa $diphoton $do_pi0 $z $sim $input_global $input_g4hits
