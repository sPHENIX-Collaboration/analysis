#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

input=${1}
qa=${2}
ntp=${3}
output=${4}
exe=${5}

# print the environment - needed for debugging
printenv

mkdir -p output
$exe $input $qa $ntp
