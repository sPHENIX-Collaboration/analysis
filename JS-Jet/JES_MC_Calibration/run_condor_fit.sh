#!/bin/bash
# Submit the closure histfitter step (Jet_resp_histfitter.C over all (radius, z, eta)
# cases) as condor jobs: one job per (radius, z bin), running the eta-integrated fit
# plus every eta-fraction bin. Condor replacement for ./do_analysis.sh.
#
# Usage:
#   ./run_condor_fit.sh              # default: all radii 2 3 4 5 6 7 8
#   ./run_condor_fit.sh 4            # one radius (index = 10 x R)
#   ./run_condor_fit.sh 2 4 6       # several radii
#
# Prerequisite: output_combined.root (from Jet_resp_histcombiner.C).
# Old output_gaus files for the selected radii are removed first so stale fits
# cannot mask empty inputs.

mkdir -p condor_fit/out/
mkdir -p condor_fit/err/
mkdir -p output_gaus/

chmod +x "CondorRunFit.sh"
> ListCondorRunFit.csv

radii=("$@")
if [ ${#radii[@]} -eq 0 ]; then radii=(2 3 4 5 6 7 8); fi

do_closure=1
n_zbins=12   # z bins 0-10 signed + 11 no-vertex; must match Jet_resp_Def_closure.h

# Eta bin count per radius; must match get_n_etafracbin() in Jet_resp_Def_closure.h.
get_n_etafracbin() {
  case $1 in
    2) echo 6 ;;
    3) echo 6 ;;
    4) echo 6 ;;
    5) echo 5 ;;
    6) echo 4 ;;
    7) echo 3 ;;
    8) echo 3 ;;
    *) echo 6 ;;
  esac
}

for r in "${radii[@]}"; do
  n_eta=$(get_n_etafracbin "$r")
  echo "R = 0.${r}: ${n_zbins} z bins x (1 + ${n_eta}) eta cases"
  rm -f output_gaus/output_gaus_r0${r}_z*.root
  rm -f condor_fit/out/condor_r0${r}_*
  rm -f condor_fit/err/condor_r0${r}_*
  for ((z = 0; z < n_zbins; z++)); do
    echo "$r,$z,$n_eta,$do_closure" >> ListCondorRunFit.csv
  done
done

echo "Argument list formed!"

condor_submit RunCondorFit.sub
