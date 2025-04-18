#! /bin/bash

submit=${1:-'test'}
nRunraw=${2:-'46431'}
nFile=${3:-'0'}
nevts=${4:-'0'}
i=0
nRun=$(printf "%08d" $nRunraw)
filename="data_lists/dst_calo_run2pp-"$nRun".list"
filenamefitting="data_lists/dst_calofitting_run2pp-"$nRun".list"
filenametrigger="data_lists/dst_triggered_event_run2pp-"$nRun".list"
filenamejet="data_lists/dst_jet_run2pp-"$nRun".list"
tFile=`wc -l < $filename`
if [[ $nFile -eq 0 ]]; then 
	nFile=$((tFile-1))
fi
if [[ $nFile -ge  $tFile ]]; then
	nFile=$((tFile-1))
fi
for i in $(seq 0 ${nFile}); do 
	j=$((i+1))
	fname="condor_files/data_condor_run_"$nRunraw$"_segment_"$i".job"
	data=`sed "${j}q;d" $filename` 	 
	datafitting=`sed "${j}q;d" $filenamefitting`
	datatrigger=`sed "${j}q;d" $filenametrigger`
	datajet=`sed "${j}q;d" $filenamejet`
	truthf="none"
	truthj="none"
	caloclusterf="none"
	globalf="none"
	
	echo "Universe 	        = vanilla " > $fname
	echo "Executable 	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/RunLargeRLENC.sh " >>$fname
	echo "Arguments         = ${data} ${datafitting} ${datatrigger} ${datajet} none ${truthj} ${caloclusterf} ${truthf} ${globalf} ${nevts} ${minpt}" >> $fname 
	echo "Output  	        = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/condor_files/data_condor_${nRunraw}_${i}.out " >> $fname
	echo "Error 		= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/condor_files/data_condor_${nRunraw}_${i}.err " >> $fname
	echo "Log  		= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/macros/condor_files/data_condor_${nRunraw}_${i}.log" >> $fname
	echo "Initialdir  	= /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/EnergyCorrelatorsJets/LargeRLCaloENC/root_output_threshold_scan" >> $fname
	echo "PeriodicHold 	= (NumJobStarts>=1 && JobStatus == 1)" >>$fname
	echo "accounting_group = group_phenix.u " >> $fname
	echo "accounting_group_user = sgross " >> $fname
	echo "request_memory = 4096MB " >> $fname
	echo "Priority = 90 ">> $fname
	echo "job_lease_duration = 3600" >> $fname
	echo "Queue 1" >> $fname 
if [[ $submit == "submit" ]]; then 
	condor_submit $fname
fi 		
done 
