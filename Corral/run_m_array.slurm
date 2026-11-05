#!/bin/bash
# One array task = one job list: lists/<set>/list_NN.txt, NN = SLURM_ARRAY_TASK_ID (2 digits).
# Submitted by run_m_array.bash (which snapshots the binary and makes the output folders).
# Outputs: root/<set>/chunks/corral_NN.root, pdf/<set>/chunks/corral_NN.pdf; the log is
# log/<set>/chunks/corral_NN.log.
#   $1 = set (dataset = the list folder, e.g. ana532); $2 = output set (= $1, or <set>_<variant>); $3 = run string (optional)
SET=$1
OUT=${2:-$1}
CRS=$3
# CORRAL_DIR is exported by run_m_array.bash (sbatch runs a spooled copy of this script, so it cannot
# find the project directory from its own location)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_m_array.bash"}
NN=$(printf "%02d" ${SLURM_ARRAY_TASK_ID})
LIST=$PROJDIR/lists/$SET/list_$NN.txt
BIN=$PROJDIR/log/$OUT/chunks/corral.snapshot
NAME=corral_$NN
echo "hostname  = "$HOSTNAME
echo "set       = "$SET"   -> "$OUT"   run string '"$CRS"'"
echo "task      = "$NN"   (array job "$SLURM_ARRAY_JOB_ID")"
echo "list      = "$LIST"   ("$(wc -l < $LIST)" files)"
echo "binary    = "$BIN
TMPDIR=${TMPDIR:-/tmp}
WORK=$TMPDIR/corral_${SLURM_ARRAY_JOB_ID}_$NN
#---- the work folder (and the local input copy) is removed however the job ends: normal end, error, scancel or
#---- time limit (SIGTERM). Only a hard kill (SIGKILL) leaves it; the stale-folder cleanup below catches those.
trap 'cd /; rm -rf "$WORK"' EXIT
trap 'echo "SIGTERM -- cleaning $WORK"; exit 143' TERM
#---- stale folders of my own killed jobs on this node: corral_<jobid>_NN whose job is no longer in squeue
if squeue -h -u $USER -o %F > /dev/null 2>&1; then
	ALIVE=" $(squeue -h -u $USER -o %F | sort -u | tr '\n' ' ') "
	for d in $(find $TMPDIR -maxdepth 1 -user $USER -name 'corral_*_*' -type d 2>/dev/null); do
		j=$(basename $d | cut -d_ -f2)
		[[ "$ALIVE" == *" $j "* ]] || { echo "removing stale $d"; rm -rf "$d"; }
	done
fi
mkdir -p $WORK/root $WORK/pdf
cd $WORK || exit 1
pwd
cp -v $BIN ./corral
#---- README_IOTiming.md (2026-11-04): /rs is slow per read request, and corral reads every file 3-4 times (pre-passes
#---- + main loop). So copy the chunk's files to node-local disk once (4 copies in parallel), then run from there.
#---- Not enough free space -> the task FAILS (exit 3, "LOCALCOPY FAIL"); resubmit it (user 2026-11-04).
#---- CORRAL_LOCAL=0 reads /rs directly, as before.
RUNLIST=$LIST
if [ "${CORRAL_LOCAL:-1}" != "0" ]; then
	NEED=$(awk '{print $1}' $LIST | xargs du -cb | tail -1 | awk '{print $1}')
	FREE=$(df -B1 --output=avail $TMPDIR | tail -1)
	MARGIN=$((5*1024*1024*1024))
	echo "localcopy = $((NEED/1024/1024)) MB to copy, $((FREE/1024/1024)) MB free on $TMPDIR"
	if [ $((NEED+MARGIN)) -gt $FREE ]; then
		echo "LOCALCOPY FAIL: not enough space on $HOSTNAME:$TMPDIR (need $((NEED/1024/1024)) MB + 5 GB margin) -- resubmit this task"
		exit 3
	fi
	mkdir -p $WORK/in
	T0=$(date +%s)
	awk '{print $1}' $LIST | xargs -P 4 -I{} cp {} $WORK/in/ || { echo "LOCALCOPY FAIL: cp error -- resubmit this task"; exit 3; }
	GOT=$(du -cb $WORK/in/*.root | tail -1 | awk '{print $1}')
	[ "$GOT" == "$NEED" ] || { echo "LOCALCOPY FAIL: copied $GOT bytes, expected $NEED -- resubmit this task"; exit 3; }
	echo "localcopy = done in $(( $(date +%s)-T0 )) s"
	RUNLIST=$WORK/list_$NN.txt
	awk -v d=$WORK/in '{n=split($1,a,"/"); $1=d"/"a[n]; print}' $LIST > $RUNLIST
	#---- corral finds bad_segments.txt next to the -l list: point it at the production's real one
	if [ -z "$CORRAL_SEGVETO" ]; then
		PROD=$(basename $(dirname $LIST) | sed -E 's/^(ana[0-9]+).*/\1/')
		export CORRAL_SEGVETO=$PROJDIR/lists/$PROD/bad_segments.txt
	fi
	echo "runlist   = $RUNLIST   segveto = $CORRAL_SEGVETO"
fi
echo Starting..........................
time ./corral -n 0 -l $RUNLIST -o $NAME ${CRS:+-s $CRS}
cp -v ./root/$NAME.root $PROJDIR/root/$OUT/chunks/
cp -v ./pdf/$NAME.pdf   $PROJDIR/pdf/$OUT/chunks/
exit
