#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection

hadd -d . -f -k -j ${BASEDIR}/merged_inclusive_parity.root ${BASEDIR}/inclusive_parity/hists*.root 
hadd -d . -f -k -j ${BASEDIR}/merged_lambda_only.root ${BASEDIR}/lambda_only/hists*.root 
hadd -d . -f -k -j ${BASEDIR}/merged_lambdabar_only.root ${BASEDIR}/lambdabar_only/hists*.root 

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_inclusive_parity.root\",\"${BASEDIR}/corrections/geo_acceptance_inclusive.root\",\"Lambda0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambda_only.root\",\"${BASEDIR}/corrections/geo_acceptance_lambdaonly.root\",\"Lambda0\",\"\#Lambda\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambdabar_only.root\",\"${BASEDIR}/corrections/geo_acceptance_lambdabaronly.root\",\"Lambda0\",\"\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)
