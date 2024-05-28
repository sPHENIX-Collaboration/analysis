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
systematics=${5}
do_pi0=${6}
z=${7}
e=${8}
chi=${9}
sim=${10}
input_global=${11}
input_mbd=${12}
input_g4hits=${13}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $qa $diphoton $systematics $do_pi0 $z $e $chi $sim $input_global $input_mbd $input_g4hits
