#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

r_val="$1"
z_val="$2"
n_eta="$3"
do_closure="$4"

# Eta-integrated fit, then each eta-fraction bin
root -l -q -b "Jet_resp_histfitter.C(${r_val},${z_val},${do_closure})"
for ((e = 0; e < n_eta; e++)); do
  root -l -q -b "Jet_resp_histfitter.C(${r_val},${z_val},${do_closure},${e})"
done

echo "JOB COMPLETE!"
