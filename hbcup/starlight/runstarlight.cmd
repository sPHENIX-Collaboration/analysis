#!/bin/bash
#
# condor executes runana.cmd 0, runana.cmd 1, etc.
#
echo PWD=${PWD}
echo LD_LIBRARY_PATH=${LD_LIBRARY_PATH}
echo HOST=`hostname`

ulimit -c 0	# no core files

# create subdirectory
dirname=`printf "PROD1_highW%04d" ${1}`
mkdir -p ${dirname}
cd ${dirname}

# Set a random seed
rand=$(od -vAn -N4 -t u4 < /dev/urandom)
cat ../PROD1_highW/slight.in | sed "s/RND_SEED.*/RND_SEED = ${rand}/" | sed 's/N_EVENTS.*/N_EVENTS = 100000/' > slight.in

#
# either compile starlight and put in your path, or
# use the default in the sPHENIX environment
#
starlight

cp -p ../log/runstarlight${1}.out output.txt
