#!/bin/bash

KS_INFILE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Kshort_run3pp_looseCuts_20260608_bco_skimmed.root
LAMBDA_INFILE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/bcoSkimmedDataSample/Lambda_run3pp_looseCuts_20260608_bco_skimmed.root

root -l -q -b plot_mass.C\(\"${KS_INFILE}\",\"Kshort_data.root\",StandardCuts::data_K_S0_cuts,BinInfo::K_S0_data_mass_bins\)
root -l -q -b plot_mass.C\(\"${LAMBDA_INFILE}\",\"Lambda_data.root\",StandardCuts::data_Lambda0_cuts,BinInfo::Lambda0_data_mass_bins\)

#root -l -q -b plot_mass.C\(\"${KS_INFILE}\",\"Kshort_data_pos.root\",StandardCuts::data_K_S0_cuts,BinInfo::K_S0_data_mass_bins\)
root -l -q -b plot_mass.C\(\"${LAMBDA_INFILE}\",\"Lambda_data_pos.root\",StandardCuts::data_Lambda0_pos_cuts,BinInfo::Lambda0_data_mass_bins\)

#root -l -q -b plot_mass.C\(\"${KS_INFILE}\",\"Kshort_data_neg.root\",StandardCuts::data_K_S0_cuts,BinInfo::K_S0_data_mass_bins\)
root -l -q -b plot_mass.C\(\"${LAMBDA_INFILE}\",\"Lambda_data_neg.root\",StandardCuts::data_Lambda0_neg_cuts,BinInfo::Lambda0_data_mass_bins\)
