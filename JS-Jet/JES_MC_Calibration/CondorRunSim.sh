#!/bin/bash

source /opt/sphenix/core/bin/sphenix_setup.sh -n new

trig="$1"
nseg="$2"
iseg="$3"
do_closure="$4"
runindex="$5"

# Run the ROOT macro with the combined string as an argument
root -l -q -b "Jet_resp_histmaker.C(${trig},${nseg},${iseg},${do_closure},${runindex})"

echo "JOB COMPLETE!"
