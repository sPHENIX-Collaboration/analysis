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
useZDCInfo=${8}
zdc_cut=${9}
e=${10}
chi=${11}
sim=${12}
input_global=${13}
input_mbd=${14}
input_g4hits=${15}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $qa $diphoton $systematics $do_pi0 $z $useZDCInfo $zdc_cut $e $chi $sim $input_global $input_mbd $input_g4hits
