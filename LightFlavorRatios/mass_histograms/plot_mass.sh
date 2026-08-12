#!/bin/bash

SEGMENT=$1

INFILE_LAMBDA=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/ppi_reco/outputKFParticle_ppi_reco_${SEGMENT}.root
INFILE_KSHORT=/sphenix/tg/tg01/hf/mjpeters/lambdaKshortMB/lambdaKshort_20260422_DetroitMB_CR_2_mode_pTref_1p4/pipi_reco/outputKFParticle_pipi_reco_${SEGMENT}.root

OUTFILE_LAMBDA=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/lambda_${SEGMENT}.root
OUTFILE_KSHORT=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC/Kshort_${SEGMENT}.root

OUTFILE_LAMBDA_POS=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/lambda_${SEGMENT}.root
OUTFILE_KSHORT_POS=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_pos/Kshort_${SEGMENT}.root

OUTFILE_LAMBDA_NEG=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/lambda_${SEGMENT}.root
OUTFILE_KSHORT_NEG=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/mass_histogram_output/MC_neg/Kshort_${SEGMENT}.root

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA}\"\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT}\"\)

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA_POS}\",BinInfo::mass_bins_MC_pos\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT_POS}\",BinInfo::mass_bins_MC_pos\)

root -l -q -b plot_mass.C\(\"${INFILE_LAMBDA}\",\"Lambda0\",\"${OUTFILE_LAMBDA_NEG}\",BinInfo::mass_bins_MC_neg\)
root -l -q -b plot_mass.C\(\"${INFILE_KSHORT}\",\"K_S0\",\"${OUTFILE_KSHORT_NEG}\",BinInfo::mass_bins_MC_neg\)
