#!/bin/bash
# Submit Finalize for a dataset as a SLURM job: it runs in a scratch dir on the worker node and copies its
# outputs to where they belong, so nothing is copied by hand.
#   Usage: ./run_finalize.bash <dataset> [Finalize_<tokens>] [<dataset> ...]
#     <dataset>          the -d dataset, e.g. ana573_795xx, ana573 (all runs), ana573_795xx_nocm (a variant)
#     Finalize_<tokens>  optional Finalize run string (default Finalize), e.g. Finalize_fz16; applies to every
#                        dataset named after it
#   e.g. ./run_finalize.bash ana573_795xx ana573_8156x ana573_8158x ana573     (four jobs)
# Outputs (<name> = corral_<runstring>, e.g. corral_Finalize, corral_Finalize_fz16):
#   root/<dataset>/<name>.root, pdf/<dataset>/<name>.pdf, log/<dataset>/<name>.log
# The binary is snapshotted to log/<dataset>/<name>.snapshot (a rebuild cannot change a pending job).
# FINALIZE_NEXPMIN / FINALIZE_MFRAC in the environment are passed on to the job (--export=ALL).
# MEM= / TIME= override the defaults (4G, 2 h; a 49-pair-type ana573 run group took ~5 min and 0.7 GB).
# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the job)
export CORRAL_DIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
[ $# -lt 1 ] && { echo "usage: $0 <dataset> [Finalize_<tokens>] [<dataset> ...]"; exit 1; }
#---- keep off the hax* and rpr* nodes (as run_m*.bash); EXCLUDE="..." replaces the list, EXCLUDE= allows every node
EXCLUDE=${EXCLUDE-hax[1-18],rpr[1-24]}
TIME=${TIME-2:00:00}
MEM=${MEM-4G}
FRS=Finalize
for ARG in "$@"; do
	if [[ $ARG == Finalize* ]]; then FRS=$ARG; continue; fi
	SET=$ARG
	if [ ! -f $CORRAL_DIR/lists/$SET/manifest.txt ]; then echo "skip $SET: no lists/$SET/manifest.txt"; continue; fi
	if ! ls $CORRAL_DIR/root/$SET/chunks/*.root >/dev/null 2>&1; then echo "skip $SET: no root/$SET/chunks/*.root"; continue; fi
	NAME=corral_$FRS
	mkdir -p $CORRAL_DIR/root/$SET $CORRAL_DIR/pdf/$SET $CORRAL_DIR/log/$SET
	cp $CORRAL_DIR/corral $CORRAL_DIR/log/$SET/$NAME.snapshot
	echo -n "$SET  $FRS  -> root|pdf|log/$SET/$NAME.*   "
	sbatch --partition=prip --time=$TIME ${EXCLUDE:+--exclude=$EXCLUDE} \
		--job-name=fin_$SET \
		--output=$CORRAL_DIR/log/$SET/$NAME.log \
		--error=$CORRAL_DIR/log/$SET/$NAME.log \
		--mem=$MEM \
		--export=ALL \
		$CORRAL_DIR/run_finalize.slurm $SET $FRS
done
