#!/bin/bash

find /sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.376/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/ -type f -name *-0000.root | sort -k1 > dst_INTTdNdEta.list
# find /sphenix/tg/tg01/bulk/dNdeta_INTT_run2023/data/simulation/ana.376/HIJING/fullSim/magOff/detectorAligned/dstSet_00000/ -type f -name *.root | sort -k1 > dst_INTTdNdEta.list
sed -i -e '$a\' dst_INTTdNdEta.list