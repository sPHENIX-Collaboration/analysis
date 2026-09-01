#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/geometricAcceptance
OUTDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection

hadd -d . -f -k -j ${BASEDIR}/merged_inclusive_parity.root ${BASEDIR}/Lambda0_geo_acceptance_0*.root
hadd -d . -f -k -j ${BASEDIR}/merged_lambda_only.root ${BASEDIR}/Lambda0_geo_acceptance_pos_*.root
hadd -d . -f -k -j ${BASEDIR}/merged_lambdabar_only.root ${BASEDIR}/Lambda0_geo_acceptance_neg_*.root
hadd -d . -f -k -j ${BASEDIR}/merged_kshort.root ${BASEDIR}/K_S0_geo_acceptance_*.root

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_inclusive_parity.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive.root\",\"Lambda0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambda_only.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly.root\",\"Lambda0\",\"\#Lambda\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambdabar_only.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly.root\",\"Lambda0\",\"\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
