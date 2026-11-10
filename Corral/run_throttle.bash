#!/bin/bash
# Keep about MAXRUN corral chunks running at once, shared among several SLURM job arrays (e.g. one array per run,
# submitted with run_m_array.bash). Every 10 min it sets each array that still has pending tasks to
# ArrayTaskThrottle = MAXRUN / (number of such arrays), at least 2, so slots freed by arrays that are running out
# go to the others. It exits when all the arrays are done.
#   Usage: ./run_throttle.bash <maxrun> <array jobid> [<array jobid> ...]
#   e.g.   ./run_throttle.bash 28 40977609 40977610 40977611
# It submits itself as a small SLURM job (1 CPU, 100 MB), so it keeps working after you log out.
# Log: log/throttle/throttle_<first jobid>.log
# Why: corral reads its inputs from the shared filesystem (/rs) once per chunk (run_m_array.slurm copies them to
# the node first). Too many chunks copying at once starves it; 28 at once was fine on 2026-11-04 (~4-5 min per copy).
PROJDIR=${CORRAL_DIR:-$(cd "$(dirname "$(readlink -f "$0")")" && pwd)}
MAXRUN=$1; shift
IDS="$*"
if [ -z "$MAXRUN" ] || [ -z "$IDS" ] || ! [[ "$MAXRUN" =~ ^[0-9]+$ ]]; then
	echo "usage: $0 <maxrun> <array jobid> [<array jobid> ...]"; exit 1
fi
#---- not inside SLURM yet: submit this script as its own job and return
if [ -z "$THROTTLE_INJOB" ]; then
	mkdir -p $PROJDIR/log/throttle
	FIRST=$(echo $IDS | awk '{print $1}')
	EXCLUDE=${EXCLUDE-hax[1-18],rpr[1-24]}
	THROTTLE_INJOB=1 sbatch --partition=prip --time=6-23:59:59 --mem=100M --cpus-per-task=1 \
		${EXCLUDE:+--exclude=$EXCLUDE} --job-name=throttle_$FIRST --export=ALL \
		--output=$PROJDIR/log/throttle/throttle_$FIRST.log $(readlink -f "$0") $MAXRUN $IDS
	exit
fi
CSV=$(echo $IDS | tr ' ' ',')
echo "$(date) throttle: at most ~$MAXRUN running, arrays $IDS"
while true; do
	[ -z "$(squeue -h -j $CSV 2>/dev/null)" ] && { echo "$(date +%H:%M) all arrays done"; exit 0; }
	ACT=""; for j in $IDS; do [ -n "$(squeue -h -j $j -t PD 2>/dev/null)" ] && ACT="$ACT $j"; done
	NRUN=$(squeue -h -j $CSV -t R 2>/dev/null | wc -l)
	n=$(echo $ACT | wc -w)
	if [ $n -gt 0 ]; then
		T=$(( MAXRUN / n )); [ $T -lt 2 ] && T=2
		for j in $ACT; do scontrol update JobId=$j ArrayTaskThrottle=$T > /dev/null 2>&1; done
		echo "$(date +%H:%M) running=$NRUN arrays-with-pending=$n throttle=$T"
	else
		echo "$(date +%H:%M) running=$NRUN, nothing pending"
	fi
	sleep 600
done
