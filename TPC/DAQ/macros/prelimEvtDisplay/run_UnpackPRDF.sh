source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export MYINSTALL=/sphenix/u/rosstom/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=0

for fileList in $@
do
  root.exe -q -b Fun4All_TPC_UnpackPRDF.C\($nEvents,\"${fileList}\"\)
done

echo Script done
