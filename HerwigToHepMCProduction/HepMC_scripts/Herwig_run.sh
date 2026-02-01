#! /bin/bash

goalevents=${2:-'1000'}
filenumber=${3:-'0'}
filetag=$(printf "%06d" $filenumber)
triggervalue=${4:-'0'}
photontriggervalue=${5:-'0'}
outfile_name=${6:-' '}
config_dir=${7:-$(pwd)}
LHADATA_path=${8:-''}
LHAPATH_path_path=${9:-''}
source /opt/sphenix/core/bin/sphenix_setup.sh -n ana
export LHAPDF_DATA_PATH=$LHADATA_path
export LHAPATH=$LHAPATH_path_path
seedn=$RANDOM
#echo $seedn
run_events=$goalevents
echo $goalevents
if [[ $triggervalue -gt 0 || $photontriggervalue -gt 0 ]]; then 
	run_events=$(( run_events * 100 )) #worst case assume a 1% good event case 
elif [[ $triggercalue -gt 30 ]]; then 
	run_events=$(( run_events * 1000 )) # based on know issue with 50 GeV jets
fi 
outfile_nameseed=$outfile_name"-S"$seedn"-"$filetag".hepmc"
outfile_namestem=$outfile_name
outfile_name=$outfile_name"-"$filetag".hepmc"
/cvmfs/sphenix.sdcc.bnl.gov/alma9.2-gcc-14.2.0/opt/sphenix/core/Herwig/bin/Herwig run $1 -N $run_events -t $filetag --seed=$seedn
echo $filetag
mv $outfile_namestem"-S"$seedn"-"$filetag".log" $outfile_namestem"-"$filetag".log"
mv $outfile_namestem"-S"$seedn"-"$filetag"-EvtGen.log" $outfile_namestem"-"$filetag"-EvtGen.log"
mv $outfile_namestem"-S"$seedn"-"$filetag".out" $outfile_namestem"-"$filetag".out"
mv $outfile_namestem"-S"$seedn"-"$filetag".tex" $outfile_namestem"-"$filetag".tex"
mv $outfile_nameseed $outfile_name
if [[ $triggervalue -gt -0 ]]; then 
	seg0="none"
	if [[ $filenumber -le 20 ]]; then 
		seg0=$outfile_namestem"-"$filetag".out"
	fi
	${config_dir}/../HerwigHepMCFilter/RunHerwigHepMCFilter.sh $outfile_name "jet" $triggervalue $goalevents $run_events $seg0 ${config_dir}/../HerwigHepMCFilter/
	rm $outfile_name
fi
if [[ $photontriggervalue -gt -0 ]]; then 
	seg0="none"
	if [[ $filenumber -le 20 ]]; then 
		seg0=$outfile_namestem"-"$filetag".out"
	fi
	${configdir}/../HerwigHepMCFilter/RunHerwigHepMCFilter.sh $outfile_name "photon" $photontriggervalue $goalevents $run_events $seg0 ${config_dir}/../HerwigHepMCFilter/
	rm $outfile_name
fi
 
