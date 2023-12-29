#!/usr/bin/env bash
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}
export MYINSTALL="$HOME/Documents/sPHENIX/install"

source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

exe=${1}
events=${2}
seed=${3}
outDir=${4}
tag=${5}
outputFile=${6}

# print the environment - needed for debugging
printenv

mkdir -p $outDir
$exe $events $seed $outDir $tag $outputFile
