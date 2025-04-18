#!/bin/bash

nmax=20000
filecounter=0
echo `pwd`
for rn in `ls  lists/dst_calo_run2pp*.list | awk -F'.' '{print $1}' | awk -F'/' '{print $2}' | awk -F'-' '{print $2}'`; do
    rn=$(expr $rn + 0)
    nfile=`wc -l lists/dst_calo_run2pp-000${rn}.list | awk '{print $1}'`
    njob=$(( $nfile + 9 ))
    njob=$(( $njob / 10 ))
    filecounter=$(( $filecounter + $njob ))
    if [ $filecounter -gt $nmax ]; then
	break
    fi
    echo $rn $filecounter
    bash run_everything.sh $njob $rn $nfile `pwd`
done


