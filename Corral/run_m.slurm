#!/bin/bash
#SBATCH -N 1
#SBATCH -t 2-23:59:59

#	called by slurm by run_m.bash

NEV=$1
# RUNSTRING=$2
echo "hostname  = "$HOSTNAME
echo "nev       = "$NEV

# CORRAL_DIR is exported by run_m.bash (sbatch runs a spooled copy of this script)
export PROJDIR=${CORRAL_DIR:?"CORRAL_DIR not set -- submit with run_m.bash"}/
echo "TMPDIR        = "$TMPDIR
echo "PROJDIR       = "$PROJDIR
export JOBSCR=$TMPDIR

pwd 
export JOBNUMA=`echo ${SLURM_JOBID} | awk -F. '{print $NR"\n"}'`
# echo "JOBNUMA = "$JOBNUMA
k=${#JOBNUMA} 
echo "Digits in JOBNUMA = "$k
if [ $k -lt 3 ]; then
	export JOBNUM=000000$JOBNUMA
elif [ $k -lt 4 ]; then
	export JOBNUM=00000$JOBNUMA
elif [ $k -lt 5 ]; then
	export JOBNUM=0000$JOBNUMA
elif [ $k -lt 6 ]; then
	export JOBNUM=000$JOBNUMA
elif [ $k -lt 7 ]; then
	export JOBNUM=00$JOBNUMA
elif [ $k -lt 8 ]; then
	export JOBNUM=0$JOBNUMA
elif [ $k -lt 9 ]; then
	export JOBNUM=$JOBNUMA
fi
# echo "JOBID     = "$SLURM_JOBID
echo "JOBNUM    = "$JOBNUM
# export JOBID=`echo $JOBNUM`

cd $JOBSCR
pwd
mkdir corral_${JOBNUM}
cd corral_${JOBNUM}
pwd
cp -v $PROJDIR/corral_m .
mkdir root
mkdir pdf
ls -lasF

export rsd=`xxd -l 3 -ps -c 10 /dev/urandom` 
export rsdn=`printf "%i" 0x$rsd` 
echo "rsdn      = "$rsdn

#--------------------------------------
echo Starting..........................
# 2026-09-22 (README_SplitTracks.md sec 14.4): NO RunString at all, by explicit
# design decision (user's own framing: "running code with no runstring should
# be our present best effort"). Both split-track scans (flat SKF, sec
# 13.8.14-13.8.19; MVTX-led SKF, sec 14.1) are done, and the validated result
# (SL<0.60, MVTX-led SKF>=0.20, i.e. SiSplitScore -- fluct_common.h/sec 13.9)
# is now the actual code default (corral_utils.cxx), same as the full
# 15-pairtype table (PairTypes.h). This script previously passed an explicit
# RunString token on the "never trust a silent code default" principle -- that
# principle now points the other way: the code default itself is the single
# source of truth for "present best effort," and a hardcoded token here would
# be the thing that goes stale next time the recommendation is revisited.
# Output lands as root/corral_m.root, pdf/corral_m.pdf (no RunString suffix).
time ./corral_m -n $NEV
cp -v ./root/*.root $PROJDIR/root/
cp -v ./pdf/*.pdf $PROJDIR/pdf/
#--------------------------------------

cd $JOBSCR
rm -rf corral_${JOBNUM}

exit





