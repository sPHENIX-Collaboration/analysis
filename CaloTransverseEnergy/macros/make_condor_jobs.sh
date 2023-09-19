#! /bin/bash 
#input is run number submit  
RUN=${1:-'21518'}
SUBMIT=${2:-'test'}
for i in {0..18}; do 
	fname="condor_$i.job"
	touch $fname
	
echo "Universe        = vanilla" > $fname
echo "Executable      = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/macros/GetET.sh" >> $fname
	if ((i<10)); then 
		echo "Arguments       = /sphenix/tg/tg01/jets/ahodges/run23_production/$RUN/DST-000$1-000$i.root " >> $fname 
	else 
	echo "Arguments       = /sphenix/tg/tg01/jets/ahodges/run23_production/$RUN/DST-000$RUN-00$i.root " >> $fname
	fi
echo "Output          = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_$i.out" >> $fname
echo "Error           =/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_$i.err" >> $fname
echo "Log             =/gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/running_dir/condor_$i.log" >> $fname
echo "Initialdir      = /gpfs/mnt/gpfs02/sphenix/user/sgross/sphenix_analysis/CaloTransverseEnergy/src" >> $fname
echo "PeriodicHold    = (NumJobStarts>=1 && JobStatus == 1)" >> $fname
echo "accounting_group = group_phenix.u" >> $fname
echo "accounting_group_user = sgross" >> $fname
echo "request_memory = 8192MB" >> $fname
echo "Priority = 90" >> $fname
echo "job_lease_duration = 3600" >> $fname
echo "Queue 1 " >> $fname


if [[ $2 -eq "submit" ]]; then 
	condor_submit $fname
fi
done
 	
