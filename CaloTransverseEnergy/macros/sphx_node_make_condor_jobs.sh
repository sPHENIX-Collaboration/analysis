#! /bin/bash 
#input is run number submit  
RUN=${1:-'21518'}
SUBMIT=${2:-'test'}
i=0
for infile in /sphenix/tg/tg01/jets/ahodges/run23_production/$RUN/DST-000$RUN-*; do 
	fname="condor_"$i"_run_"$RUN".job"
	touch $fname
	echo $fname
	
echo "Universe        = vanilla" > $fname
echo "Executable      = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/macros/GetET.sh" >> $fname
#	if ((i<10)); then 
#		echo "Arguments       = /sphenix/tg/tg01/jets/ahodges/run23_production/$RUN/DST-000$RUN-000$i.root " >> $fname 
#	else 
	echo "Arguments       = $infile " >> $fname
#	fi
echo "Output          = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_"$i"_run_"$RUN".out" >> $fname
echo "Error           =/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_"$i"_run_"$RUN".err" >> $fname
echo "Log             =/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_"$i"_run_"$RUN".log" >> $fname
echo "Initialdir      = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/src" >> $fname
echo "PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)" >> $fname
echo "accounting_group = group_sphenix.u" >> $fname
echo "accounting_group_user = sgross" >> $fname
echo "request_memory = 128192MB" >> $fname
echo "Priority = 90" >> $fname
echo "job_lease_duration = 3600" >> $fname
echo "Queue 1 " >> $fname


if [[ $2 -eq "submit" ]]; then 
	condor_submit $fname
fi
i=$(( i+1 ))
done
 	
