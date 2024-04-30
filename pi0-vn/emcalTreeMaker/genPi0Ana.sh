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
z=${4}
output=${5}
anaType=${6}
do_vn_calc=${7}
fitStats=${8}
Q_vec_corr=${9}
samples=${10}
cut_num=${11}
sigma=${12}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $cuts $z $output $anaType $do_vn_calc $fitStats $Q_vec_corr $samples $cut_num $sigma
