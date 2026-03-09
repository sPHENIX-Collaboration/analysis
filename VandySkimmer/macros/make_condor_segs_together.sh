#!/bin/bash

count=0
nRuns=`cat "/sphenix/u/bkimelman/analysis/wholeEventEECs/macros/queue_all.list" | wc -l`

echo "nRuns: $nRuns"

queueFile=queue_all_segs_jobOf25.list

for run in $(cat "/sphenix/u/bkimelman/analysis/wholeEventEECs/macros/queue_all.list");
do
    echo "working on run $run, $count / 1563"
    
    runShort=$(($run/100))

    nFiles=`ls /sphenix/lustre01/sphnxpro/production2/run2pp/physics/jetskimmer/ana521_2025p007_v001/run_000${runShort}00_000$((runShort+1))00/DST_JETCALO_run2pp_ana521_2025p007_v001-000${run}-*.root | wc -l`

    touch $queueFile
    for (( f=0; f<$nFiles; f+=25 ))
    do
        echo ${run}, $((f/25)) >> $queueFile
    done

    count=$((count+1))
done

