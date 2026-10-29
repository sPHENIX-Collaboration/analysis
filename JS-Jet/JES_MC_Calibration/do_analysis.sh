#!/bin/bash
# Run the closure-test Gaussian fitting for every (z, eta) bin.
#
# Usage:
#   ./do_analysis.sh              # default: R = 0.4
#   ./do_analysis.sh 4            # one radius (index = 10 x R)
#   ./do_analysis.sh 2 4 6        # several radii
#
# For each radius: all z-vertex bins (0-10 signed z ranges from -200 to +200 cm,
# 11 = no-vertex), eta-integrated (eta = -1) plus each eta-fraction bin.
# The z loop range and eta bin counts must match Jet_resp_Def_closure.h.

radii=("$@")
if [ ${#radii[@]} -eq 0 ]; then radii=(4); fi

do_closure=1

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
  echo "===== R = 0.${r}  (${n_eta} eta bins) ====="
  for z in $(seq 0 11); do
    root -l -q -b "Jet_resp_histfitter.C(${r},${z},${do_closure})"
    for ((e = 0; e < n_eta; e++)); do
      root -l -q -b "Jet_resp_histfitter.C(${r},${z},${do_closure},${e})"
    done
  done
done

echo "All fits done."
