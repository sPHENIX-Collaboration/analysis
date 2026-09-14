#!/bin/bash

FILEBASE=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection

hadd -d . -f -k -j ${FILEBASE}/systematics_min/merged_cut_efficiency.root ${FILEBASE}/systematics_min/cut_efficiency_0*.root
hadd -d . -f -k -j ${FILEBASE}/systematics_min/merged_cut_efficiency_pos.root ${FILEBASE}/systematics_min/cut_efficiency_pos_0*.root
hadd -d . -f -k -j ${FILEBASE}/systematics_min/merged_cut_efficiency_neg.root ${FILEBASE}/systematics_min/cut_efficiency_neg_0*.root

hadd -d . -f -k -j ${FILEBASE}/systematics_max/merged_cut_efficiency.root ${FILEBASE}/systematics_max/cut_efficiency_0*.root
hadd -d . -f -k -j ${FILEBASE}/systematics_max/merged_cut_efficiency_pos.root ${FILEBASE}/systematics_max/cut_efficiency_pos_0*.root
hadd -d . -f -k -j ${FILEBASE}/systematics_max/merged_cut_efficiency_neg.root ${FILEBASE}/systematics_max/cut_efficiency_neg_0*.root

root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_min/merged_cut_efficiency.root\",\"${FILEBASE}/systematics_min/cut_efficiency_correction.root\",\"Lambda0\",\"K_S0\"\)
root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_min/merged_cut_efficiency_pos.root\",\"${FILEBASE}/systematics_min/cut_efficiency_correction_pos.root\",\"Lambda0\",\"K_S0\"\)
root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_min/merged_cut_efficiency_neg.root\",\"${FILEBASE}/systematics_min/cut_efficiency_correction_neg.root\",\"Lambda0\",\"K_S0\"\)

root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_max/merged_cut_efficiency.root\",\"${FILEBASE}/systematics_max/cut_efficiency_correction.root\",\"Lambda0\",\"K_S0\"\)
root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_max/merged_cut_efficiency_pos.root\",\"${FILEBASE}/systematics_max/cut_efficiency_correction_pos.root\",\"Lambda0\",\"K_S0\"\)
root -l -q -b getCutEfficiency.C\(\"${FILEBASE}/systematics_max/merged_cut_efficiency_neg.root\",\"${FILEBASE}/systematics_max/cut_efficiency_correction_neg.root\",\"Lambda0\",\"K_S0\"\)
