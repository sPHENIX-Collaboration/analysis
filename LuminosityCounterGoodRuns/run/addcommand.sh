#!/bin/bash
#UN=jocl
#source /opt/sphenix/core/bin/sphenix_setup.sh -n new
#source /opt/sphenix/core/bin/setup_local.sh /sphenix/user/jocl/projects/testinstall/
#export HOME=/sphenix/u/jocl

#rm anaoutput.txt
#rm -rf srces
for rn in `cat fullgoodrunlist.list`; do #op5mrad.list`; do #`cat fullgoodrunlist.list`; do

    #mkdir -p srces

    #for file in `ls -v /sphenix/tg/tg01/jets/${UN}/trigcount/${rn}/*`; do
    #	echo $file
	#cp $file ./srces
    #done
    echo $rn
    nfile=`ls /sphenix/tg/tg01/jets/jocl/trigcount/${rn} | wc -l`

    root -b -q -l "analyze.C(${rn},${nfile})" #>> anaoutput.txt

    mv triggeroutput_nblair_${rn}.root output/added_nblair/
    #rm -rf srces
done
