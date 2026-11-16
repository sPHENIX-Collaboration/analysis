#!/bin/bash

hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_lambda_MC.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/lambda*.root &
hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_Kshort_MC.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/Kshort*.root &
hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_lambda_MC_pos.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/lambda*.root &
hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_Kshort_MC_pos.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/Kshort*.root &
hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_lambda_MC_neg.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/lambda*.root &
hadd -d /sphenix/tg/tg01/hf/mjpeters/ -f -k -j /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/merged_Kshort_MC_neg.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/Kshort*.root &

