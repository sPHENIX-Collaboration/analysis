#!/bin/bash

RN=$1

BASENAME="hadd_${RN}"
PREFIX="./subs"
SUBNAME="${BASENAME}.sub"

echo "executable = addcommand.sh" > $PREFIX/$SUBNAME
echo "concurrency_limits=CONCURRENCY_LIMIT_DEFAULT:500" >> $PREFIX/$SUBNAME
echo "arguments = ${RN} ${USER} ${3}" >> $PREFIX/$SUBNAME
echo "priority = 0" >> $PREFIX/$SUBNAME
echo "output = output/out/output_${BASENAME}_\$(Process).out" >> $PREFIX/$SUBNAME
echo "should_transfer_files   = IF_NEEDED" >> $PREFIX/$SUBNAME
echo "when_to_transfer_output = ON_EXIT" >> $PREFIX/$SUBNAME
echo "error = output/err/error_${BASENAME}_\$(Process).err" >> $PREFIX/$SUBNAME
echo "log = /tmp/${USER}_${BASENAME}.log" >> $PREFIX/$SUBNAME
echo "queue 1" >> $PREFIX/$SUBNAME

condor_submit $PREFIX/$SUBNAME
