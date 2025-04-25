#! /bin/bash
goalevents=${2:-'1000'}
filenumber=${3:-'0'}
filetag=$(printf "%06d" $filenumber)
triggervalue=${4:-'0.'}
outfile_name=${5:-' '}
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
export LHAPDF_DATA_PATH=$LHAPDF_DATA_PATH:/sphenix/user/sgross/sphenix_analysis/HerwigToHepMCProduction/config_files
export LHAPATH=$LHAPATH:/sphenix/user/sgross/sphenix_analysis/HerwigToHepMCProduction/config_files
echo $LHAPDF_DATA_PATH
seedn=$RANDOM
#echo $seedn
run_events=$goalevents
if [[ $triggervalue -gt 0 ]]; then 
	run_events=$(( run_events * 100 )) #worst case assume a 1% good event case
fi 

outfile_nameseed=$outfile_name"-S"$seedn"-"$filetag".hepmc"
outfile_namestem=$outfile_name
outfile_name=$outfile_name"-"$filetag".hepmc"
/cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/Herwig/bin/Herwig run $1 -N $run_events -t $filetag --seed=$seedn
echo $filetag
mv $outfile_namestem"-S"$seedn"-"$filetag".log" $outfile_namestem"-"$filetag".log"
mv $outfile_namestem"-S"$seedn"-"$filetag"-EvtGen.log" $outfile_namestem"-"$filetag"-EvtGen.log"
mv $outfile_namestem"-S"$seedn"-"$filetag".out" $outfile_namestem"-"$filetag".out"
mv $outfile_namestem"-S"$seedn"-"$filetag".tex" $outfile_namestem"-"$filetag".tex"
mv $outfile_nameseed $outfile_name
if [[ $triggervalue -gt -1 ]]; then 
	/sphenix/user/sgross/sphenix_analysis/HerwigToHepMCProduction/RunHerwigHepMCFilter.sh $outfile_name $triggervalue $goalevents
	rm $outfile_name
fi
 
