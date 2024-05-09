#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
input=${2}
cuts=${3}
isSim=${4}
z=${5}
output=${6}
anaType=${7}
do_vn_calc=${8}
fitStats=${9}
Q_vec_corr=${10}
samples=${11}
cut_num=${12}
sigma=${13}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $cuts $isSim $z $output $anaType $do_vn_calc $fitStats $Q_vec_corr $samples $cut_num $sigma
