#!/bin/bash

NFILE=$1
RN=$2

BASENAME="condor_${RN}"
PREFIX="./subs"
SUBNAME="${BASENAME}.sub"

echo "executable = earlydata.sh" > $PREFIX/$SUBNAME
echo "concurrency_limits=CONCURRENCY_LIMIT_DEFAULT:500" >> $PREFIX/$SUBNAME
echo "arguments = \$(Process) ${RN} ${3} ${USER} ${4}" >> $PREFIX/$SUBNAME
echo "output = output/out/output_${BASENAME}_\$(Process).out" >> $PREFIX/$SUBNAME
echo "should_transfer_files   = IF_NEEDED" >> $PREFIX/$SUBNAME
echo "when_to_transfer_output = ON_EXIT" >> $PREFIX/$SUBNAME
echo "error = output/err/error_${BASENAME}_\$(Process).err" >> $PREFIX/$SUBNAME
echo "log = /tmp/${USER}_${BASENAME}.log" >> $PREFIX/$SUBNAME
echo "queue ${NFILE}" >> $PREFIX/$SUBNAME

condor_submit $PREFIX/$SUBNAME
