#!/bin/bash
# Submit the single full-statistics corral_m job for a dataset (all its events in one job; this is also
# the reference Finalize compares against).
#   Usage: ./run_m.bash <dataset> [nev]     (nev: events, default 0 = all)
# Reads lists/<dataset>/list_all.txt (made by lists/make_lists.bash).
# Outputs: root/<dataset>/corral_m.root, pdf/<dataset>/corral_m.pdf, log/<dataset>/corral_m.log
# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the job)
export CORRAL_DIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
cd $CORRAL_DIR || exit 1

SET=$1
thisNEV=${2:-0}
if [ -z "$SET" ] || [ ! -f $CORRAL_DIR/lists/$SET/list_all.txt ]; then
	echo "usage: $0 <dataset> [nev]   (no lists/$SET/list_all.txt -- run lists/make_lists.bash first)"; exit 1
fi
mkdir -p $CORRAL_DIR/root/$SET $CORRAL_DIR/pdf/$SET $CORRAL_DIR/log/$SET

#EXCL=dad1,dad2,dad3,dad4,dad5,dad6,dad7,dad8,hax9,hax1,hax2,hax3,hax4,hax5,hax6,hax7,hax8,hax9,hax10,hax11,hax12,hax13,hax14,hax15,hax16,hax17,hax18
#sbatch --partition=prip --time=2-23:59:59 --exclude=$EXCL \

sbatch --partition=prip --time=2-23:59:59 \
	--job-name=crm1_$SET \
	--error=./log/$SET/corral_m.log \
	--output=./log/$SET/corral_m.log \
	--mem=8G \
	--export=ALL \
	run_m.slurm $SET $thisNEV

exit