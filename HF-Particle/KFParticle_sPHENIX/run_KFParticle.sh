#!/usr/bin/bash
export HOME=/sphenix/u/${LOGNAME}
source /opt/sphenix/core/bin/sphenix_setup.sh -n mdc1.5

echo running: run_KFParticle.sh $*

echo arg1 \(file path\) : $1
echo arg2 \(file name\) : $2
echo running root.exe -q -b Fun4All_KFParticle_singleFile.C\(\"$1\",\"$2\"\)
root.exe -q -b Fun4All_KFParticle_singleFile.C\(\"$1\",\"$2\"\)
echo "script done"
