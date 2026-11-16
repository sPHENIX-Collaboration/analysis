#!/bin/bash

BASEDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/closureTestSample/geometricAcceptance
OUTDIR=/sphenix/tg/tg01/hf/mjpeters/LightFlavorProduction/geometricAcceptanceCorrection

# central values
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_inclusive_parity.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive.root\",\"Lambda0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambda_only.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly.root\",\"Lambda0\",\"\#Lambda\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/merged_lambdabar_only.root\",\"${BASEDIR}/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly.root\",\"Lambda0\",\"\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

# loosest geometric acceptance thresholds
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_minthreshold/merged_inclusive_parity.root\",\"${BASEDIR}/systematics_minthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive_minthreshold.root\",\"Lambda0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_minthreshold/merged_lambda_only.root\",\"${BASEDIR}/systematics_minthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly_minthreshold.root\",\"Lambda0\",\"\#Lambda\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_minthreshold/merged_lambdabar_only.root\",\"${BASEDIR}/systematics_minthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly_minthreshold.root\",\"Lambda0\",\"\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

# tightest geometric acceptance thresholds
root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_maxthreshold/merged_inclusive_parity.root\",\"${BASEDIR}/systematics_maxthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive_maxthreshold.root\",\"Lambda0\",\"\(\#Lambda+\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_maxthreshold/merged_lambda_only.root\",\"${BASEDIR}/systematics_maxthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly_maxthreshold.root\",\"Lambda0\",\"\#Lambda\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

root -l -q -b get_geoAcceptance.C\(\"${BASEDIR}/systematics_maxthreshold/merged_lambdabar_only.root\",\"${BASEDIR}/systematics_maxthreshold/merged_kshort.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly_maxthreshold.root\",\"Lambda0\",\"\#bar\{\#Lambda\}\)\",\"K_S0\",\"K_\{S\}^\{0\}\"\)

# systematic uncertainty calculation
root -l -q -b get_geoAcceptance_syserr.C\(\"${OUTDIR}/corrections/geo_acceptance_inclusive.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive_minthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive_maxthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_inclusive_syserr.root\"\)

root -l -q -b get_geoAcceptance_syserr.C\(\"${OUTDIR}/corrections/geo_acceptance_lambdaonly.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly_minthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly_maxthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdaonly_syserr.root\"\)

root -l -q -b get_geoAcceptance_syserr.C\(\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly_minthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly_maxthreshold.root\",\"${OUTDIR}/corrections/geo_acceptance_lambdabaronly_syserr.root\"\)
