#!/bin/bash

for n in {0..399}; do
    echo `printf "%05d" $n`
    # find /sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.382/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/ -type f -name *-`printf "%05d" $n`.root | sort -k1 > dst_INTTdNdEta_sim_$n.list
    find /sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.388/HIJING/fullSim/magOff/detectorAligned/dstSet_00001/ -type f -name *-`printf "%05d" $n`.root | sort -k1 > dst_INTTdNdEta_sim_$n.list
    sed -i -e '$a\' dst_INTTdNdEta_sim_$n.list
done

# find /sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/data/run_00020869/ana.382/beam_intt_combined-dst-00020869-0000.root > dst_INTTdNdEta_data.list
# sed -i -e '$a\' dst_INTTdNdEta_data.list
