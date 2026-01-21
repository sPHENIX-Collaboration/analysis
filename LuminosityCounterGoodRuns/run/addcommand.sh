#!/bin/bash
#UN=jocl
source /opt/sphenix/core/bin/sphenix_setup.sh -n new
source /opt/sphenix/core/bin/setup_local.sh /sphenix/user/jocl/projects/testinstall/
export HOME=/sphenix/u/jocl

#rm anaoutput.txt
#rm -rf srces
#for rn in `tail -n 24 listrunnumber.txt | head -n 19`; do #op5mrad.list`; do #`cat listrunnumber.txt`; do
line=$(( $1 + 1 ))
rn=`head -n $line listrunnumber.txt | tail -n 1`
    #mkdir -p srces

    #for file in `ls -v /sphenix/tg/tg01/jets/${UN}/trigcount/${rn}/*`; do
    #	echo $file
	#cp $file ./srces
    #done
echo $rn
nfile=`ls /sphenix/user/jocl/projects/trigcount_files/${rn}/*clt* | wc -l`

root -b -q -l "analyze.C(${rn},${nfile},1)" #>> anaoutput.txt

mv triggeroutput_nblair_${rn}_clt.root output/added_509/
    #rm -rf srces
#done
