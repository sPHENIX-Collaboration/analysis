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

Fun4All_CaloTreeGen $input $qa $ntp

echo "finished job"
echo "transferring output files"

cp $qa $ntp $output

echo "Done"
