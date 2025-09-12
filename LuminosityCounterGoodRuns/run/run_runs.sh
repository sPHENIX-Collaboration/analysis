#!/bin/bash

nmax=15000
filecounter=0
echo `pwd`
LISTDIR=lists
for rn in `cat fullgoodrunlist.list`; do #`ls  ${LISTDIR}/dst_calofitting_run2pp*.list | awk -F'.' '{print $1}' | awk -F'/' '{print $2}' | awk -F'-' '{print $2}'`; do #`cat fullgoodrunlist.list`; do
    rn=$(expr $rn + 0)
    nfile=`wc -l ${LISTDIR}/dst_calofitting-000${rn}.list | awk '{print $1}'`
    njob=$(( $nfile + 19 ))
    njob=$(( $njob / 20 ))
    filecounter=$(( $filecounter + $njob ))
    if [ $filecounter -gt $nmax ]; then
	break
    fi
    echo $rn $filecounter
    bash run_everything.sh $njob $rn $nfile `pwd`
done


