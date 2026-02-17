#! /bin/bash
verbose_mode=false 
allsegments=false
nfiles=100
dosubmit=false 
user=`id -u -n`
triggertype="MB"
prodtype="26"
condor_testfile="condor_blank.job"

#build the condor jobs
make_condor_jobs()
{
	if [ ! -d $(pwd)"/../"${triggertype}"_output" ]; then 
		if [ "$verbose_mode" = true ]; then 
			echo "data directory doesn't exist for ${triggertype}, fixing now"
		fi 
		
		mkdir $(pwd)"/../"${triggertype}"_output"
	fi 
	if [[ $nfiles -eq 0 ]]; then 
		nfiles=`wc -l < ${triggertype}_data/herwig_files.list`
	fi	
	for i in $(seq 0 ${nfiles}); do 
		j=$(( i+1 ))
		condor_file="$(pwd)/condor_file_dir/condor_"$triggertype"_seg_"$i".job"
		condor_out_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".out"
		condor_err_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".err"
		condor_log_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".log"
		herwig_hepmc=`sed "${j}q;d" ${triggertype}_data/herwig_files.list`
		pythia_truth=`sed "${j}q;d" ${triggertype}_data/pythia_truth.list`
		pythia_jet=`sed "${j}q;d" ${triggertype}_data/pythia_jet.list`

		if [ "$vebose_mode" = true ]; then
			echo "Producing condor job file " $condor_file
		fi
		IFS=$'\n' read -d '' -r -a blanklines < $condor_testfile
		echo "${blanklines[0]}" > $condor_file 
		echo "${blanklines[1]}"$(pwd)"/RunHerwigProductionQA.sh" >> $condor_file
		echo "${blanklines[2]}"$i $triggertype $herwig_hepmc $pythia_truth $pythia_jet >> $condor_file
		echo "${blanklines[3]}"$condor_out_file >> $condor_file
		echo "${blanklines[4]}"$condor_err_file >> $condor_file
		echo "${blanklines[5]}"$condor_log_file >> $condor_file
		echo "${blanklines[6]} $(pwd)/../${triggertype}_output" >>$condor_file
		echo "${blanklines[7]}" >> $condor_file
		echo "${blanklines[8]}" >> $condor_file 
		echo "${blanklines[9]}" "   "  $user >> $condor_file 
		echo "${blanklines[10]}" >> $condor_file
		echo "${blanklines[11]}" >> $condor_file
		echo "${blanklines[12]}" >> $condor_file
		echo "${blanklines[13]}" >> $condor_file
	done		
}

submit_condor_jobs(){
	#if submit just get all files in expected job type
	for n in $(seq 0 ${nfiles}); do 
		i=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$n".job"
		condor_submit $i
	done
}

has_argument(){
	[[ ("$1" == *=* && -n ${1#*=}) || ( ! -z "$2" && "$2" != -*) ]]
}

extract_argument() {
	echo "${2:-${1#*=}}"
}

handle_options()
{
	while [ $# -gt 0 ]; do 
		case $1 in 
			-h | --help)
				echo "Options for Herwig/Pythia Comparisons at the HepMC level"
			       	echo "$0 [OPTIONS]"
				echo "This script runs a comparison QA between produced Herwig HepMC files and existant Pythia productions"
				echo " "
				echo " -h, --help 	Display this message"
				echo " -v, --verbose	Enable verbose job creation (Default false) "
				echo " -s, --submit 	Submit condor jobs (default false)"
				echo " -g,  --get 	Makes the filelist for Herwig & Pythia before running (default false)"
				echo " -t, --type 	Which production "
				echo "			MB (default)"
			       	echo "			Jets:Jet5, Jet15, Jet20, Jet30, Jet40, Jet50"
			        echo "			PhotonJets: PhotonJet5, PhotonJet10"
				echo " "
				echo " -n, --number 	How many files to test against (default 100, 0 is all) "
				exit 0 
				;;
			-v | --verbose)
				verbose_mode=true
				shift
				;;
			-s | --submit)
				dosubmit=true
				shift
				;;
			-g | --get)
				makedatalist=true
				shift
				;;
			-t | --type) 
				triggertype=$(extract_argument $@)
				shift
				shift
				;;
			-n | --number) 
				nfiles=$(extract_argument $@)
				shift 
				shift 
				;;
			*)
				echo "Invalid option: $1 "
				exit 1
				::
		esac
	done
}
converttriggertype()
{
	if [ "${triggertype}" = "MB" ]; then
		prodtype=26
	elif [ "${triggertype}" = "Jet5" ]; then
		prodtype=36
	elif [ "${triggertype}" = "Jet10" ]; then
		prodtype=12
	elif [ "${triggertype}" = "Jet15" ]; then
		prodtype=33
	elif [ "${triggertype}" = "Jet20" ]; then
		prodtype=21
	elif [ "${triggertype}" = "Jet30" ]; then
		prodtype=11
	elif [ "${triggertype}" = "Jet40" ]; then
		prodtype=19
	elif [ "${triggertype}" = "Jet50" ]; then
		prodtype=34
	elif [ "${triggertype}" = "PhotonJet5" ]; then
		prodtype=27
	elif [ "${triggertype}" = "PhotonJet10" ]; then
		prodtype=28
	elif [ "${triggertype}" = "PhotonJet10" ]; then
		prodtype=29
	fi
}
handle_options "$@"
if [ "$verbose_mode" = true ]; then 
	echo "Running over ${nfiles} segement(s)"
fi 
if [ "$makedatalist" = true ]; then
	base_dir=$(pwd)
	if [ "$verbose_mode" = true ]; then 
		echo "Checking if data directory exists for ${triggertype}"
	fi 
	if [ ! -d ${triggertype}"_data" ]; then 
		if [ "$verbose_mode" = true ]; then 
			echo "data directory doesn't exist for ${triggertype}, fixing now"
		fi 
		
		mkdir -p ${triggertype}"_data"
	fi 
	if [ "$verbose_mode" = true ]; then 
		echo "Create Herwig Datalist for ${triggertype}"
	fi 
	ls /sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_${triggertype}/*.hepmc > ${triggertype}_data/herwig_files.list
	if [ "$verbose_mode" = true ]; then 
		echo "Create Pythia Datalist for ${triggertype}"
	fi 
	cd ${triggertype}_data
	converttriggertype 
	CreateFileList.pl -nopileup -type ${prodtype} -run 28 G4Hits DST_TRUTH_JET
	mv g4hits.list pythia_truth.list
	mv dst_truth_jet.list pythia_jet.list
	cd ${base_dir}
fi
make_condor_jobs
if [ "$dosubmit" = true ]; then
	submit_condor_jobs
fi


