#! /bin/bash

#Run analysis over a set of runs, taken from csv file
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
./GetET.o "/sphenix/tg/tg01/jets/ahodges/run23_production/21518/DST-00021518-0000.root"
#runs=$( more $1 | cut -d "," -f2 )
#declare -a runnumbers 
#readarray -t runnumbers <<< $runs

#for ra in ${runnumbers[@]}; do 
#	ls /sphenix/lustre01/sphnxpro/commissioning/HCal/led/led_*"$ra"*  >> run_"$ra".txt
#done

	
