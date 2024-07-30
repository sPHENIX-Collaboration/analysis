#! /bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n
export MYINSTALL=/sphenix/user/sgross/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

root -b -q CompareParticlesToTowers.C\( $1, $2, $3, $4 \)
