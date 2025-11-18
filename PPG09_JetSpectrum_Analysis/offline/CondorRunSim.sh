#!/bin/bash

source /sphenix/u/hj2552/setup.sh

runtype="$1"
nSeg="$2"
iSeg="$3"
radius_value="$4"

echo "Run type: ${runtype}, nSeg: ${nSeg}, iSeg: ${iSeg}, radius: ${radius_value}"

root -l -q -b "analysis_sim.C(\"${runtype}\", ${nSeg}, ${iSeg}, ${radius_value})"

echo "JOB COMPLETE!"
