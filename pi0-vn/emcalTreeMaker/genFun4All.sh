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
e=${7}
chi=${8}
sim=${9}
input_global=${10}
input_g4hits=${11}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $qa $diphoton $do_pi0 $z $e $chi $sim $input_global $input_g4hits
