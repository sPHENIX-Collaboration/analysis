#! bin/bash

submit=${1:-'test'}
nFile=${2:-'0'}
nRuns=${3:-'0'}
i=0
if [[ $nFile -eq 0 ]]; then 
	$nfile = `wc -l < dst_truth_jet.list`
fi 
for i in {1..$nFile}; do 
	fname="condor_files/condor_segment_"$i",job" 	 
	truthf=sed "${i}q;d" dst_truth_reco.list
	truthj=sed "${i}q;d" dst_truth_jet.list
	g4hitf=sed "${i}q;d" dst_truth_g4hit.list
	caloclusterf=sed "${i}q;d" dst_calo_cluster.list
	globalf=sed "${i}q;d" dst_global.list
	
	echo " Universe 	= vanilla " > $fname
	echo " Executable 	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/CompareParticlesToTowers.sh " >>$fname
	echo " Arguments       = ${truthj} ${caloclusterf} ${truthf} ${global} ${caloclusterf} ${nRuns}" >> $fname 
	echo " Output  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/condor_${i}.out " >> $fname
	echo "Error 		=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/condor_${i}.err " >> $fname
	echo " Log  		=/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC/macros/condor_${i}.log" >> $fname
	echo "Initialdir  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/CalorimeterTowerENC" >> $fname
	echo "PeriodicHold 	= (NumJobStarts>=1 && JobStatus == 1)" >>$fname
	echo "accounting_group = group_phenix.u " >> $fname
	echo "accounting_group_user = sgross " >> $fname
	echo "request_memory = 32192MB " >> $fname
	echo "Priority = 90 ">> $fname
	echo" job_lease_duration = 3600" >> $fname
	echo "Queue 1" >> $fname 
if [[ $submit == "submit" ]]; then 
	condor_submit $fname
fi 		
done 
