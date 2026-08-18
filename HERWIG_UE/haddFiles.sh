#!/bin/bash

#for MC in Herwig Pythia;
#do
    MC=Herwig
    for samp in MB Jet5 Jet12 Jet20 Jet30 Jet40 Jet50;
    do
        hadd -f /sphenix/tg/tg01/jets/bkimelman/Herwig_UE/${MC}_UE_Aug5_${samp}.root /sphenix/tg/tg01/jets/bkimelman/Herwig_UE/${MC}_rootFiles_Aug5_2026/${MC}${samp}_*.root
    done
#done