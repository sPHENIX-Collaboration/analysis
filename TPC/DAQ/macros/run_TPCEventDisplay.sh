source /opt/sphenix/core/bin/sphenix_setup.sh -n new
export MYINSTALL=/sphenix/u/rosstom/install
source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL

nEvents=2
outDir="/sphenix/user/rosstom/test/testFiles/"

for fileList in $@
do
  root.exe -q -b Fun4All_TPC_EventDisplayPrep.C\($nEvents,\"$outDir\",\"${fileList}\"\)
done

adcCut=0
outFile="/sphenix/user/rosstom/test/TPCEventDisplay_10616.json"
runName="pedestal-00010616"

root.exe -q -b TPCEventDisplay.C\($adcCut,\"$outFile\",\"$outDir\",\"$runName\"\)

echo Script done
