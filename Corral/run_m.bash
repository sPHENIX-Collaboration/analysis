#!/bin/bash

# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the job)
export CORRAL_DIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
cd $CORRAL_DIR || exit 1

thisNEV=0
#thisNEV=100000000

#EXCL=dad1,dad2,dad3,dad4,dad5,dad6,dad7,dad8,hax9,hax1,hax2,hax3,hax4,hax5,hax6,hax7,hax8,hax9,hax10,hax11,hax12,hax13,hax14,hax15,hax16,hax17,hax18
#sbatch --partition=prip --time=2-23:59:59 --exclude=$EXCL \

sbatch --partition=prip --time=2-23:59:59 \
	--error=./log/run_m_%A.out \
	--output=./log/run_m_%A.out \
	--mem=8G \
	--export=ALL \
	run_m.slurm $thisNEV 

exit