#!/bin/bash
#SBATCH -N 1
#SBATCH -t 2-23:59:59

#	called by slurm by run_m.bash

SET=$1
NEV=$2
echo "hostname  = "$HOSTNAME
echo "dataset   = "$SET
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
# No RunString: the code defaults are the validated best effort.
time ./corral_m -n $NEV -l $PROJDIR/lists/$SET/list_all.txt
cp -v ./root/corral_m.root $PROJDIR/root/$SET/
cp -v ./pdf/corral_m.pdf   $PROJDIR/pdf/$SET/
#--------------------------------------

cd $JOBSCR
rm -rf corral_${JOBNUM}

exit





