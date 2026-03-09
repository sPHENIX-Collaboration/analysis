#!/bin/bash
#
# submit jobs to condor
# 
# $executable the program to execute (usually a shell script)
# $arg is what is passed to the run.cmd script
# $log is what the log file names are
# $user is the email to send to
#

# make sure log directory exists
TMPLOG=/tmp/${USER}/${PWD##*$USER/}/log
echo mkdir -p ${TMPLOG}
mkdir -p ${TMPLOG}
mkdir -p log

executable=runstarlight.cmd    # program to run
logfname=${executable%.cmd}

njobs=10   # num jobs to run

# submit njobs
for (( ijob=0; ijob<${njobs}; ijob++ ))
#cat redoruns | while read ijob
do
  echo "Submitting Run ${ijob}"

  log=${logfname}${ijob}    # log file names
  arg=${ijob}             # argument passed to executable

  echo "Queue" | condor_submit \
    -a "Executable=$PWD/${executable}" \
    -a "Arguments=${arg}" \
    -a "Log=${TMPLOG}/${log}.log" \
    -a "Output=$PWD/log/${log}.out" \
    -a "Error=$PWD/log/${log}.err" \
    -a "+Job_Type=\"cas\"" \
    -a "Initialdir=$PWD" \
    -a "GetEnv=True"

done

echo Condor submit logs are in $TMPLOG

