#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
input=${2}
Q_vec_corr=${3}
z=${4}
output=${5}
anaType=${6}
samples=${7}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $Q_vec_corr $z $output $anaType $samples
