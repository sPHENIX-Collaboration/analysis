#!/bin/bash

export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n new

echo running: run_KFParticle.sh $*

echo arg1 \(file path\) : $1
echo running root.exe -q -b Fun4All_KFParticle_condorJob.C\(\"$1\"\)
root.exe -q -b Fun4All_KFParticle_condorJob.C\(\"$1\"\)
echo "script done"
