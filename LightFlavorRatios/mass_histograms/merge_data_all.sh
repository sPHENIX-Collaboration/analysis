#!/bin/bash

hadd -f -k -j data_Kshort_mass.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/data/Kshort*.root
hadd -f -k -j data_phi_mass.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/data/phi*.root
hadd -f -k -j data_lambda_mass.root /sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/data/lambda*.root
