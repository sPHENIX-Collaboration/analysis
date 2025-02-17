#! /bin/bash

# if no argument is given, print usage
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <run number>"
    exit 1
fi

runnumber=$1

# create the directory for lists 
mkdir -p ./calolists
mkdir -p ./inttlists


# create the list of files for calo
# ls -1 /sphenix/lustre01/sphnxpro/physics/run2auau/calophysics/new_2024p007/run_00054200_00054300/DST_TRIGGERED_EVENT_run2auau_new_2024p007-000${runnumber}-*.root > ./calolists/calo_${runnumber}.list
ls /sphenix/lustre01/sphnxpro/physics/run2auau/calophysics/ana441_2024p007/run_00054200_00054300/DST_TRIGGERED_EVENT_run2auau_ana441_2024p007-000${runnumber}-*.root > ./calolists/calo_${runnumber}.list
# count the number of files in the list
NFILESCALO=$(cat ./calolists/calo_${runnumber}.list | wc -l)
echo "NFILESCALO = $NFILESCALO"
# loop over the files in the list and create one list per file
for ifile in $(seq 1 $NFILESCALO)
do
    # print line by line the list of files
    echo $(sed -n ${ifile}p ./calolists/calo_${runnumber}.list) > ./calolists/calo_${runnumber}_$((ifile-1)).list
done

# create the list of files for intt
# ls -1 /sphenix/lustre01/sphnxpro/physics/slurp/streaming/physics/new_2024p007/run_00054200_00054300/DST_INTT_EVENT_run2auau_new_2024p007-000${runnumber}-*.root > ./inttlists/intt_${runnumber}.list
ls /sphenix/lustre01/sphnxpro/production/run2auau/physics/ana464_2024p011_v001/DST_TRKR_CLUSTER/run_00054200_00054300/dst/DST_TRKR_CLUSTER_run2auau_ana464_2024p011_v001-00054280-*.root > ./inttlists/intt_${runnumber}.list
NFILESINTT=$(cat ./inttlists/intt_${runnumber}.list | wc -l)
echo "NFILESINTT = $NFILESINTT"
for ifile in $(seq 1 $NFILESINTT)
do
    echo $(sed -n ${ifile}p ./inttlists/intt_${runnumber}.list) > ./inttlists/intt_${runnumber}_$((ifile-1)).list
done