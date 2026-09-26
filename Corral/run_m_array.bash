#!/bin/bash
# Submit corral_m over a chunked production as a SLURM job array (one task per job list).
#   Usage: ./run_m_array.bash <set> [array-range]
#     <set>          dataset = list folder under lists/, made by lists/make_lists.bash (e.g. ana532)
#     [array-range]  optional, e.g. 0-3 or 7 (default: all lists, 0..nlists-1)
# Snapshots the current corral_m to log/<set>/chunks/corral_m.snapshot so every task runs the same binary.
# Outputs: root/<set>/chunks/corral_m_NN.root, pdf/<set>/chunks/corral_m_NN.pdf, log/<set>/chunks/corral_m_NN.log
# project directory = $CORRAL_DIR if set, else the directory this script lives in (passed on to the tasks)
PROJDIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
export CORRAL_DIR=$PROJDIR
SET=$1
if [ -z "$SET" ] || [ ! -f $PROJDIR/lists/$SET/manifest.txt ]; then
	echo "usage: $0 <set> [array-range]   (no lists/$SET/manifest.txt -- run lists/make_lists.bash first)"; exit 1
fi
NL=$(awk '$1=="nlists"{print $2}' $PROJDIR/lists/$SET/manifest.txt)
RANGE=${2:-0-$((NL-1))}
mkdir -p $PROJDIR/root/$SET/chunks $PROJDIR/pdf/$SET/chunks $PROJDIR/log/$SET/chunks
cp -v $PROJDIR/corral_m $PROJDIR/log/$SET/chunks/corral_m.snapshot
echo "set $SET: $NL lists, submitting array $RANGE"
sbatch --partition=prip --time=2-23:59:59 \
	--array=$RANGE \
	--job-name=crm_$SET \
	--output=$PROJDIR/log/$SET/chunks/corral_m_%2a.log \
	--error=$PROJDIR/log/$SET/chunks/corral_m_%2a.log \
	--mem=8G \
	--export=ALL \
	$PROJDIR/run_m_array.slurm $SET
