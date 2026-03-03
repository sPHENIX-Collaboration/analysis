#!/bin/bash                                                                                                               
export USER="$(id -u -n)"
export LOGNAME=${USER}
export HOME=/sphenix/u/${LOGNAME}/macros/detectors/sPHENIX/


runnumber=0
outDir=""
triggertype="MB"
dosubmit=false 
triggertype="MB"
prodtype="26"
condor_testfile="condor_blank.job"
verbose=false 
superverbose=false 
allsegments=false #run over all data
filedensity=25 #segments per job
nfiles=100
makedatalist=false
forcechunk=false

#Setting directories and ensuring they exist

make_condor_jobs()
{
	if [[ $nfiles -eq 0 ]]; then 
		nfiles=`wc -l < ${triggertype}_data/jet_density_${filedensity}.list`
	fi	
	for i in $(seq 0 ${nfiles}); do 
		j=$(( i+1 ))
		if [ $i -eq $nfiles ]; then 
			break
		fi
		condor_file="$(pwd)/condor_file_dir/condor_"$triggertype"_seg_"$i".job"
		condor_out_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".out"
		condor_err_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".err"
		condor_log_file=$(pwd)"/condor_file_dir/condor_"$triggertype"_seg_"$i".log"
		global=`sed "${j}q;d" ${triggertype}_data/global_density_${filedensity}.list`
		truth=`sed "${j}q;d" ${triggertype}_data/truth_density_${filedensity}.list`
		jet=`sed "${j}q;d" ${triggertype}_data/jet_density_${filedensity}.list`
		calo=`sed "${j}q;d" ${triggertype}_data/calo_density_${filedensity}.list`
		
		if [ "$vebose_mode" = true ]; then
			echo "Producing condor job file " $condor_file
		fi
		IFS=$'\n' read -d '' -r -a blanklines < $condor_testfile
		echo "${blanklines[0]}" > $condor_file 
		echo "${blanklines[1]}"$(pwd)"/run_VandySkimmerTruth.sh" >> $condor_file
		echo "${blanklines[2]}"$calo $truth $jet $global $outDir $MYINSTALL>> $condor_file
		echo "${blanklines[3]}"$condor_out_file >> $condor_file
		echo "${blanklines[4]}"$condor_err_file >> $condor_file
		echo "${blanklines[5]}"$condor_log_file >> $condor_file
		echo "${blanklines[6]} $outDir" >>$condor_file
		echo "${blanklines[7]}" >> $condor_file
		echo "${blanklines[8]}" >> $condor_file 
		echo "${blanklines[9]}" "   "  $USER >> $condor_file 
		echo "${blanklines[10]}" >> $condor_file
		echo "${blanklines[11]}" >> $condor_file
		echo "${blanklines[12]}" >> $condor_file
		echo "${blanklines[13]}" >> $condor_file
	done
}
make_home_dir()
{
	if [ ! -d ${HOME} ]; then 
	    if [[ "$verbose" == true ]]; then
		    echo "Home Directory doesn't exist. Creating now"
	    fi
	    mkdir -p ${HOME}
	fi
}

set_out_dir()
{
	if [[ "$outDir" == "" ]]; then	
	    outDir=/sphenix/tg/tg01/jets/${USER}/VandyDSTs/
	fi
	if [ ! -d ${outDir} ]; then 
	    if [[ "$verbose" == true ]]; then
		    echo "Output Directory doesn't exist. Creating now"
	    fi
	    mkdir -p ${outDir}
	fi
}

get_dst_list()
{
	base_dir=$(pwd)
	if [ "$verbose" = true ]; then 
		echo "Checking if data directory exists for ${triggertype}"
	fi 
	if [ ! -d ${triggertype}"_data" ]; then 
		if [ "$verbose" = true ]; then 
			echo "data directory doesn't exist for ${triggertype}, fixing now"
		fi 
		
		mkdir -p ${triggertype}"_data"
	fi 
	if [ "$verbose" = true ]; then 
		echo "Create DST for ${triggertype}"
	fi
	cd ${triggertype}_data
	converttriggertype 
	CreateFileList.pl -nopileup -type ${prodtype} -run 28 G4Hits DST_TRUTH_JET DST_CALO_CLUSTER DST_GLOBAL
	mv g4hits.list truth.list
	mv dst_truth_jet.list jet.list
	mv dst_calo_cluster.list calo.list
	mv dst_global.list global.list
	cd ${base_dir}
}
chunk_dst_list()
{
	base_dir=$(pwd)
	cd ${triggertype}_data
	if [ "$verbose" = true ]; then 
		echo "Checking if lookup file and file list exist for a per job density of ${filedensity} exists"
	fi 
	listdir=lists_${filedensity}_per_file
	if [ ! -d ${listdir} ]; then
		mkdir -p $listdir
	fi 
	if [ ! -f truth_density_${filedensity}.list ]; then 
		forcechunk=true
	fi
	if [ "$forcechunk" = true ]; then 
		if [ "$verbose" = true ]; then 
			echo "Creating file lists and lookup files"
		fi
	       	echo "HERE"	
		if [ -f truth_density_${filedensity}.list ]; then 
			rm truth_density_${filedensity}.list
			rm jet_density_${filedensity}.list
			rm calo_density_${filedensity}.list
			rm global_density_${filedensity}.list
			rm ${listdir}/*
		fi 
		touch truth_density_${filedensity}.list
		touch jet_density_${filedensity}.list
		touch calo_density_${filedensity}.list
		touch global_density_${filedensity}.list

		nChunks=0
		Nseg=`wc -l  < truth.list`
		nSegsUsed=0
		while [ $nSegsUsed -le $Nseg ]; do 
			nStop=$(( nSegsUsed + filedensity - 1 )) 
			truthChunk=$(pwd)/${listdir}/truth_seg_${nSegsUsed}_to_${nStop}.list
			jetChunk=$(pwd)/${listdir}/jet_seg_${nSegsUsed}_to_${nStop}.list
			caloChunk=$(pwd)/${listdir}/calo_seg_${nSegsUsed}_to_${nStop}.list
			globalChunk=$(pwd)/${listdir}/global_seg_${nSegsUsed}_to_${nStop}.list
			if [ "$superverbose" = true ]; then 
				echo "Building the following files"
				echo " Truth data (g4Hits): " $truthChunk
				echo " Jet data : " $jetChunk
				echo " Calo data : " $caloChunk
				echo " Global data: " $globalChunk
			fi
			touch ${truthChunk}
			touch ${jetChunk}
			touch ${caloChunk}
			touch ${globalChunk}
			
			echo ${truthChunk} >> truth_density_${filedensity}.list
			echo ${jetChunk} >> jet_density_${filedensity}.list
			echo ${caloChunk} >> calo_density_${filedensity}.list
			echo ${globalChunk} >> global_density_${filedensity}.list
			
			for i in $(seq 0 $filedensity); do
				line0=$(( n * filedensity ))
				j=$(( line0 + i + 1 ))
				if [[ $j -gt $Nseg || $j -gt $nStop ]]; then
					break
				fi
				truth=`sed "${j}q;d" truth.list`
				jet=`sed "${j}q;d" jet.list`
				calo=`sed "${j}q;d" calo.list`
				global=`sed "${j}q;d" global.list`
				
				echo ${truth} >> ${truthChunk}	
				echo ${jet} >> ${jetChunk}	
				echo ${calo} >> ${caloChunk}	
				echo ${global} >> ${globalChunk}	
				nSegsUsed=$(( nSegsUsed + 1 ))
			done
			nChunks=$(( nChunks + 1 ))
			if [[ $nSegsUsed -gt $Nseg ]]; then
			       break
			fi 	       
		done	
	fi
	cd ${base_dir}
}
submit_condor_jobs(){
	#if submit just get all files in expected job type
	for n in $(seq 0 ${nfiles}); do 
		if [ $n -ge ${nfiles} ]; then
			break
		fi	
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
				echo "Options for MC running of Vandy Skimmer"
			       	echo "$0 [OPTIONS]"
				echo "This script run the vandy skimmer over MC files"
				echo " "
				echo " -h, --help 	Display this message"
				echo " -v, --verbose	Enable verbose job creation (Default false) "
				echo " -s, --submit 	Submit condor jobs (default false)"
				echo " -g,  --get 	Makes the filelist for selected sample before running (default false)"
				echo " -c,  --chunk 	Forces the recreation of the data chunks (default off, on if -g is called"
				echo " "
				echo " -t, --type 	Which trigger type "
				echo "			MB (default)"
			       	echo "			Jets:Jet5, Jet15, Jet20, Jet30, Jet40, Jet50"
			        echo "			PhotonJets: PhotonJet5, PhotonJet10"
				echo " "
				echo " -G, --generator 	Which generator "
				echo "			Pythia (default), Herwig "
				echo " "
				echo " -n, --number 	How many files per chunk  (default 25, 0 is all) "
				echo " "
				echo " -j, -jobs	How many chunks/jobs to submit/make (default 100, 0 is all)"
				echo " "
				exit 0 
				;;
			-v | --verbose)
				verbose=true
				shift
				;;
			-V )
				superverbose=true
				verbose=true
				shift
				;;
			-s | --submit)
				dosubmit=true
				shift
				;;
			-g | --get)
				makedatalist=true
				forcechunk=true
				shift
				;;
			-c | --chunk)
				forcechunk=true
				shift
				;;
			-t | --type) 
				triggertype=$(extract_argument $@)
				shift
				shift
				;;
			-n | --number) 
				filedensity=$(extract_argument $@)
				shift 
				shift 
				;;
			-G | --generator) 
				gen=$(extract_argument $@)
				shift
				shift
				;;
			-o | --outdir)
				outDir=$(extract_argument $@)
				shift
				shift
				;;
			-j | --jobs) 
				nfiles=$(extract_argument $@)
				if [ "$verbose" = true ]; then 
					echo "Will submit ${nfiles} jobs with ${filedensity} segment(s) each"
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
converttriggertype()
{
	if [ "${triggertype}" = "MB" ]; then
		prodtype=26
	elif [ "${triggertype}" = "Jet5" ]; then
		prodtype=36
	elif [ "${triggertype}" = "Jet10" ]; then
		prodtype=12
	elif [ "${triggertype}" = "Jet12" ]; then
		prodtype=39
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
make_home_dir
set_out_dir
if [ "$verbose" = true ]; then 
	echo "Running over ${nfiles} segement(s)"
fi 
if [ "$makedatalist" = true ]; then
	get_dst_list
fi
chunk_dst_list
make_condor_jobs
if [ "$dosubmit" = true ]; then
	submit_condor_jobs
fi

