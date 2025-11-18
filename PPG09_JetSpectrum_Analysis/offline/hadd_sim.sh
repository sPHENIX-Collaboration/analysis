#!/bin/bash
mkdir -p output_sim_hadd
rm output_sim_hadd/*

radius_index=(4)

for ((r=0; r<${#radius_index[@]}; r++)); do
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_MB.root output_sim/output_r0${radius_index[$r]}_MB_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet5GeV.root output_sim/output_r0${radius_index[$r]}_Jet5GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet10GeV.root output_sim/output_r0${radius_index[$r]}_Jet10GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet15GeV.root output_sim/output_r0${radius_index[$r]}_Jet15GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet20GeV.root output_sim/output_r0${radius_index[$r]}_Jet20GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet30GeV.root output_sim/output_r0${radius_index[$r]}_Jet30GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet50GeV.root output_sim/output_r0${radius_index[$r]}_Jet50GeV_*
    hadd -j -k output_sim_hadd/output_sim_r0${radius_index[$r]}_Jet70GeV.root output_sim/output_r0${radius_index[$r]}_Jet70GeV_*
done
