!# /bin/bash
nRun=${1:-'0'}
submit=${2:-'test'}
nFile=${3:-'0'}
i=0
if [[ $nFile -eq 0 ]]; then 
	nFile=`wc -l < dst_truth_jet.list`
fi 
for i in $(seq 0 ${nFile}); do 
	fname="condor_files/condor_segment_"$i".job" 	 
	#truthf=`sed "${i}q;d" dst_truth_reco.list`
	truthj=`sed "${i}q;d" dst_truth_jet.list`
	#g4hitf=`sed "${i}q;d" dst_truth_g4hit.list`
	#caloclusterf=`sed "${i}q;d" dst_calo_cluster.list`
	#globalf=`sed "${i}q;d" dst_global.list`
	
	echo "Universe 	= vanilla " > $fname
	echo "Executable 	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/JS-Jet/Dijet_QA/macros/GenerateDijetQA.sh " >>$fname
	echo "Arguments       	= ${truthj} $nRun $i">> $fname 
	echo "Output  		= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/JS-Jet/Dijet_QA/CalorimeterTowerENC/macros/condor_files/condor_${i}.out " >> $fname
	echo "Error 		=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/JS-Jet/Dijet_QA/CalorimeterTowerENC/macros/condor_files/condor_${i}.err " >> $fname
	echo "Log  		=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/JS-Jet/Dijet_QA/CalorimeterTowerENC/macros/condor_files/condor_${i}.log" >> $fname
	echo "Initialdir  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/JS-Jet/Dijet_QA/root_output" >> $fname
	echo "PeriodicHold 	= (NumJobStarts>=1 && JobStatus == 1)" >>$fname
	echo "accounting_group = group_phenix.u " >> $fname
	echo "accounting_group_user = sgross " >> $fname
	echo "request_memory = 32192MB " >> $fname
	echo "Priority = 90 ">> $fname
	echo "job_lease_duration = 3600" >> $fname
	echo "Queue 1" >> $fname 
if [[ $submit == "submit" ]]; then 
	condor_submit $fname
fi 		
done 
