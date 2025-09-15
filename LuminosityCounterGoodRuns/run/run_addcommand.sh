#!/bin/bash

NFILE=`cat fullgoodrunlist.list | wc -l`

BASENAME="condor_add"
PREFIX="./subs"
SUBNAME="${BASENAME}.sub"

echo "executable = addcommand.sh" > $PREFIX/$SUBNAME
#echo "concurrency_limits=CONCURRENCY_LIMIT_DEFAULT:1000" >> $PREFIX/$SUBNAME
echo "request_memory = 3072MB" >> $PREFIX/$SUBNAME
echo "arguments = \$(Process)" >> $PREFIX/$SUBNAME
echo "output = output/out/output_add_${BASENAME}_\$(Process).out" >> $PREFIX/$SUBNAME
echo "should_transfer_files   = IF_NEEDED" >> $PREFIX/$SUBNAME
echo "when_to_transfer_output = ON_EXIT" >> $PREFIX/$SUBNAME
echo "error = output/out/output_add_${BASENAME}_\$(Process).out" >> $PREFIX/$SUBNAME
echo "log = /tmp/${USER}_${BASENAME}.log" >> $PREFIX/$SUBNAME
echo "queue ${NFILE}" >> $PREFIX/$SUBNAME

condor_submit $PREFIX/$SUBNAME
