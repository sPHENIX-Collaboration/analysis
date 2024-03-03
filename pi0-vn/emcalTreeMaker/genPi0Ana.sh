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
do_vn_calc=${6}
fitStats=${7}
Q_vec_corr=${8}
samples=${9}
cut_num=${10}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $cuts $z $output $do_vn_calc $fitStats $Q_vec_corr $samples $cut_num
