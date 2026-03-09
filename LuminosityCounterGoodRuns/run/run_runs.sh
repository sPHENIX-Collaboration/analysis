#!/bin/bash

nmax=150000
filecounter=0
echo `pwd`
LISTDIR=lists
for rn in `cat listrunnumber.txt`; do #`ls  ${LISTDIR}/dst_calofitting_run2pp*.list | awk -F'.' '{print $1}' | awk -F'/' '{print $2}' | awk -F'-' '{print $2}'`; do #`cat fullgoodrunlist.list`; do
    rn=$(expr $rn + 0)
    nfile=`wc -l ${LISTDIR}/dst_calofitting-000${rn}.list | awk '{print $1}'`
    njob=$(( $nfile + 19 ))
    njob=$(( $njob / 20 ))
    filecounter=$(( $filecounter + $njob ))
    bash run_everything.sh $njob $rn $nfile `pwd`
    echo $rn $filecounter

    if [ $filecounter -gt $nmax ]; then
	break
    fi
    
done


