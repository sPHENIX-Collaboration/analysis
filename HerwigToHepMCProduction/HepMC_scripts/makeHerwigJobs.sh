#! /bin/bash
verbose_mode=false
events=1000000
nfiles=1000
density=10000 #events / file
dosubmit=false
triggertype="MB" 
triggervalue="0."
configfile="MB.in"
configdir="$(pwd)/../config_files"
condor_testfile="condor_blank.job"
minseg=0
user=`id -u -n`
make_condor_jobs()
{
	for i in $(seq 0 ${nfiles}); do
		j=$(( i + minseg )) 
		condor_file="$(pwd)/condor_file_dir/condor_"$triggertype"_"$j".job"
		condor_out_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_"$j".out"
		condor_err_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_"$j".err"
		condor_log_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_"$j".log"
		if [ "$vebose_mode" = true ]; then
			echo "Producing condor job file " $condor_file
		fi
		IFS=$'\n' read -d '' -r -a blanklines < $condor_testfile
		echo "${blanklines[0]}" > $condor_file 
		echo "${blanklines[1]}"$(pwd)"/Herwig_run.sh" >> $condor_file
		echo "${blanklines[2]}"$configfile $density $j $triggervalue "/sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_"$triggertype"/Herwig_"$triggertype>> $condor_file
		echo "${blanklines[3]}"$condor_out_file >> $condor_file
		echo "${blanklines[4]}"$condor_err_file >> $condor_file
		echo "${blanklines[5]}"$condor_log_file >> $condor_file
		echo "${blanklines[6]} /sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_"$triggertype >>$condor_file
		echo "${blanklines[7]}" >> $condor_file
		echo "${blanklines[8]}" >> $condor_file #set for me right now
		echo "${blanklines[9]}" "   "  $user >> $condor_file 
		echo "${blanklines[10]}" >> $condor_file
		echo "${blanklines[11]}" >> $condor_file
		echo "${blanklines[12]}" >> $condor_file
		echo "${blanklines[13]}" >> $condor_file
	done		
}
submit_condor_jobs(){
	#if submit just get all files in the expected job type
	for n in $(seq 0 ${nfiles}); do 
	 	i="condor_file_dir/condor_"$triggertype"_"$n".job" 
		condor_submit $i 
	done
}
has_argument(){
	[[ ("$1" == *=* && -n ${1#*=}) || ( ! -z "$2" && "$2" != -*) ]]
}

extract_argument() {
	echo "${2:-${1#*=}}"
}

set_config()
{
	#Need to use the .run files, can ammend to use the .in files  but that adds unnecessary computational time
	if [ "$triggertype" = "MB" ]; then 
		configfile="${configdir}/Herwig_MB.run"
	elif [ "$triggertype" = "Jet10" ]; then
		configfile="${configdir}/Herwig_Jet10.run"
		triggervalue="10"
	elif [ "$triggertype" = "Jet20" ]; then
		configfile="${configdir}/Herwig_Jet20.run"
		triggervalue="20"
	elif [ "$triggertype" = "Jet30" ]; then 
		configfile="${configdir}/Herwig_Jet30.run"
		triggervalue="30"
	elif [ "$triggertype" = "PhotonJet5" ]; then 
		configfile="${configdir}/Herwig_PhotonJet5.run"
	elif [ "$triggertype" = "PhotonJet10" ]; then 
		configfile="${configdir}/Herwig_PhotonJet10.run"
	elif [ "$triggertype" = "PhotonJet20" ]; then 
		configfile="${configdir}/Herwig_PhotonJet20.run"
	else
		configfile="${configdir}/Herwig_MB.run" #use as default value
	fi
}

find_config()
{
	if [[ "$configfile" == *"Herwig_"* ]];then
		filetag="Herwig_"
		triggertype="${configfile#*$filetag}"
	else 
		triggertype="$configfile"
	fi
	if [[ "$triggertype" == *".run"* ]];then
		filetype=".run"
		triggertype="${triggertype%$filetype*}"
	fi
}
handle_options(){
	while [ $# -gt 0 ]; do
	case $1 in 
		-h | --help)
			echo "Options for Herwig job creation script"
			echo "$0 [OPTIONS]"
			echo "This script runs Herwig to create HepMC files given an input configuration"
			echo "  "
			echo " -h, --help	Display this help message"
			echo " -v, --verbose	Enable verbose job creation (Default false) "
			echo " -N, --events  	Number of events to generate (Default 1M) "
			echo " -n, --perfile	Number of events per file (Default 1k) "
			echo " -s, --submit	Make and submit condor jobs (Default false)"
			echo " -t, --trigger	Input type (MB, Jet10, Jet20, Jet30, PhotonJet5, PhotonJet10, PhotonJet20) (Default MB)"
			echo " -j, --jetcut	Add a Jet cut filter [Integer GeV] (Default None) "
			echo " -i, --input 	Specify new input file (Default blank)"
			echo " -f, --first	Specify a first segment number (Default 0)"
			exit 0 
			;;
		-v | --verbose)
			verbose_mode=true
			shift
			;;
		-n | --perfile*) 
			if has_argument $@; then 
				density=$(extract_argument $@)
				nfiles=$(( events / density ))
			fi
			shift
			shift
			;;
		-N | --events*)
			if has_argument $@; then 
				events=$(extract_argument $@) 
				nfiles=$(( events / density ))
				if [ "$verbose_mode" = true ]; then
					echo "Run " $events " events"
					echo " This will generate " $nfiles " output hepmc files"
				fi
				nfiles=$(( nfiles - 1 ))
			fi
			shift
			shift
			;;
		-s | --submit) 
			dosubmit=true
			shift			
			;;
		-t | --trigger*)
			if has_argument $@; then 
				triggertype=$(extract_argument $@)
				set_config
				if [ "$verbose_mode" = true ]; then
					echo "Trigger type: " $triggertype 
					echo "Config file: " $configfile
					echo "Config dir: " $configdir
				fi
			fi
			shift
			shift
			;;
		-j | --jetcut*)
			if has_argument $@; then 
				triggervalue=$(extract_argument $@)
				if [ "$verbose_mode" = true ]; then
					echo "Jet cut value: " $triggervalue
				fi
			fi
			shift
			shift
			;;
		-i | --input*)
			if has_argument $@; then
				configfile=$(extract_argument $@)
				find_config
				if [ "$verbose_mode" = true ]; then
					echo "Trigger type: " $triggertype 
					echo "Config file: " $configfile
				fi
			fi
			shift
			shift
			;;
		-f | --first*)
			if has_argument $@; then 
				minseg=$(extract_argument $@)
				if [ "$verbose_mode" = true ]; then 
					echo "First segement number: " $minseg
				fi
			fi
			shift 
			shift		
		;;
		*) 
			echo "Invalid option: $1 "
			exit 1
			;;
		esac
	done
}

handle_options "$@"
if [ ! -d "/sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_"$triggertype ]; then 
	mkdir -p "/sphenix/tg/tg01/jets/sgross/HerwigHepMC/Herwig_"$triggertype; 
fi 
make_condor_jobs 
if [ "$dosubmit" = true ]; then
	submit_condor_jobs
fi

