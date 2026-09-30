#!/bin/bash
# Submit the single full-statistics corral job for a dataset (all its events in one job; this is also
# the reference Finalize compares against).
#   Usage: ./run_m.bash <dataset> [nev]     (nev: events, default 0 = all)
# Reads lists/<dataset>/list_all.txt (made by lists/make_lists.bash).
#   CORRAL_VARIANT=<name> CORRAL_RS=<runstring> ./run_m.bash <dataset>   a VARIANT (as run_m_array.bash): outputs under <dataset>_<name>/
# Snapshots the current corral to log/<out>/corral.snapshot (a rebuild cannot change a pending job).
# Outputs (<out> = <dataset>, or <dataset>_<CORRAL_VARIANT>): root/<out>/corral.root, pdf/<out>/corral.pdf, log/<out>/corral.log
# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the job)
export CORRAL_DIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
cd $CORRAL_DIR || exit 1

SET=$1
thisNEV=${2:-0}
if [ -z "$SET" ] || [ ! -f $CORRAL_DIR/lists/$SET/list_all.txt ]; then
	echo "usage: $0 <dataset> [nev]   (no lists/$SET/list_all.txt -- run lists/make_lists.bash first)"; exit 1
fi
OUT=$SET
VAR=$CORRAL_VARIANT
CRS=$CORRAL_RS
if [ -n "$CRS" ] && [ -z "$VAR" ]; then echo "CORRAL_RS='$CRS' needs a CORRAL_VARIANT=<name> (outputs go to <set>_<name>/)"; exit 1; fi
if [ -n "$VAR" ]; then
	[ -z "$CRS" ] && echo "variant $VAR: the code defaults (no run string), e.g. a new code version next to the old outputs"
	OUT=${SET}_$VAR
	[ -e $CORRAL_DIR/lists/$OUT ] || ln -s $SET $CORRAL_DIR/lists/$OUT
fi
mkdir -p $CORRAL_DIR/root/$OUT $CORRAL_DIR/pdf/$OUT $CORRAL_DIR/log/$OUT
cp -v $CORRAL_DIR/corral $CORRAL_DIR/log/$OUT/corral.snapshot

#EXCL=dad1,dad2,dad3,dad4,dad5,dad6,dad7,dad8,hax9,hax1,hax2,hax3,hax4,hax5,hax6,hax7,hax8,hax9,hax10,hax11,hax12,hax13,hax14,hax15,hax16,hax17,hax18
#sbatch --partition=prip --time=2-23:59:59 --exclude=$EXCL \

#---- keep off the hax* and rpr* nodes (user, 2026-09-28: hax18 stalled reading /rs); EXCLUDE="..." replaces the
#---- list (sbatch --exclude syntax), EXCLUDE= (empty) allows every node
EXCLUDE=${EXCLUDE-hax[1-18],rpr[1-24]}
#---- the single full-statistics job (user, 2026-09-28): ana573 is 642M events, ~13 h at 61 min / 50M, memory
#---- ~1.3 GB + 8 MB per M events (~6.6 GB): defaults 7 days and 16G; TIME= / MEM= override them
TIME=${TIME-6-23:59:59}
MEM=${MEM-16G}
sbatch --partition=prip --time=$TIME ${EXCLUDE:+--exclude=$EXCLUDE} \
	--job-name=crm1_$OUT \
	--error=./log/$OUT/corral.log \
	--output=./log/$OUT/corral.log \
	--mem=$MEM \
	--export=ALL \
	run_m.slurm $SET $thisNEV $OUT $CRS

exit