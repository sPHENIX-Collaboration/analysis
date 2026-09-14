#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/cutEfficiencyCorrection

root -l -q -b getCutEfficiency_systematics.C\(\"${BASEDIR}/cut_efficiency_correction.root\",\"${BASEDIR}/systematics_min/cut_efficiency_correction.root\",\"${BASEDIR}/systematics_max/cut_efficiency_correction.root\",\"${BASEDIR}/cut_efficiency_correction_syserr.root\",\"Lambda0\",\"K_S0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_{S}^{0}\"\)

root -l -q -b getCutEfficiency_systematics.C\(\"${BASEDIR}/cut_efficiency_correction_pos.root\",\"${BASEDIR}/systematics_min/cut_efficiency_correction_pos.root\",\"${BASEDIR}/systematics_max/cut_efficiency_correction_pos.root\",\"${BASEDIR}/cut_efficiency_correction_pos_syserr.root\",\"Lambda0\",\"K_S0\",\"\#Lambda\",\"K_{S}^{0}\"\)

root -l -q -b getCutEfficiency_systematics.C\(\"${BASEDIR}/cut_efficiency_correction_neg.root\",\"${BASEDIR}/systematics_min/cut_efficiency_correction_neg.root\",\"${BASEDIR}/systematics_max/cut_efficiency_correction_neg.root\",\"${BASEDIR}/cut_efficiency_correction_neg_syserr.root\",\"Lambda0\",\"K_S0\",\"\#bar\{\#Lambda\",\"K_{S}^{0}\"\)
