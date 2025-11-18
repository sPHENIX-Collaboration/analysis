#!/bin/bash

radius_index=(4)

for ((r=0; r<${#radius_index[@]}; r++)); do
    rm output_data_r0${radius_index[$r]}.root
    hadd -j -k output_data_r0${radius_index[$r]}.root output_data/output_r0${radius_index[$r]}_*.root
done
