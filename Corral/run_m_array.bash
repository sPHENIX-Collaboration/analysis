#!/bin/bash
# Submit corral over a chunked production as a SLURM job array (one task per job list).
#   Usage: ./run_m_array.bash <set> [array-range]
#     <set>          dataset = list folder under lists/, made by lists/make_lists.bash (e.g. ana532)
#     [array-range]  optional, e.g. 0-3 or 7 (default: all lists, 0..nlists-1)
#   CORRAL_VARIANT=<name> CORRAL_RS=<runstring> ./run_m_array.bash <set> ...   a VARIANT of the set (README_SplitTracks573, Final state):
#     the same lists, run with -s <runstring>, outputs under <set>_<name>/ (lists/<set>_<name> is made as a symlink to
#     lists/<set>, so Finalize runs unchanged with -d <set>_<name>). e.g. CORRAL_VARIANT=raw CORRAL_RS=noLS_noulstest_... ./run_m_array.bash ana573_795xx
#     (not VAR/RS: the login profile already sets RS=/rs/rs_grp_rhi)
# Snapshots the current corral to log/<out>/chunks/corral.snapshot so every task runs the same binary.
# Outputs (<out> = <set>, or <set>_<CORRAL_VARIANT>): root/<out>/chunks/corral_NN.root, pdf/<out>/chunks/corral_NN.pdf,
#   log/<out>/chunks/corral_NN.log
# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the tasks)
PROJDIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
export CORRAL_DIR=$PROJDIR
SET=$1
if [ -z "$SET" ] || [ ! -f $PROJDIR/lists/$SET/manifest.txt ]; then
	echo "usage: $0 <set> [array-range]   (no lists/$SET/manifest.txt -- run lists/make_lists.bash first)"; exit 1
fi
NL=$(awk '$1=="nlists"{print $2}' $PROJDIR/lists/$SET/manifest.txt)
RANGE=${2:-0-$((NL-1))}
OUT=$SET
VAR=$CORRAL_VARIANT
CRS=$CORRAL_RS
if [ -n "$CRS" ] && [ -z "$VAR" ]; then echo "CORRAL_RS='$CRS' needs a CORRAL_VARIANT=<name> (outputs go to <set>_<name>/)"; exit 1; fi
if [ -n "$VAR" ]; then
	[ -z "$CRS" ] && echo "variant $VAR: the code defaults (no run string), e.g. a new code version next to the old outputs"
	OUT=${SET}_$VAR
	[ -e $PROJDIR/lists/$OUT ] || ln -s $SET $PROJDIR/lists/$OUT
fi
mkdir -p $PROJDIR/root/$OUT/chunks $PROJDIR/pdf/$OUT/chunks $PROJDIR/log/$OUT/chunks
cp -v $PROJDIR/corral $PROJDIR/log/$OUT/chunks/corral.snapshot
echo "set $SET -> $OUT: $NL lists, run string '${CRS}', submitting array $RANGE"
#---- keep off the hax* and rpr* nodes (user, 2026-09-28: hax18 stalled reading /rs); EXCLUDE="..." replaces the
#---- list (sbatch --exclude syntax), EXCLUDE= (empty) allows every node
EXCLUDE=${EXCLUDE-hax[1-18],rpr[1-24]}
sbatch --partition=prip --time=2-23:59:59 ${EXCLUDE:+--exclude=$EXCLUDE} \
	--array=$RANGE \
	--job-name=crm_$OUT \
	--output=$PROJDIR/log/$OUT/chunks/corral_%2a.log \
	--error=$PROJDIR/log/$OUT/chunks/corral_%2a.log \
	--mem=8G \
	--export=ALL \
	$PROJDIR/run_m_array.slurm $SET $OUT $CRS
