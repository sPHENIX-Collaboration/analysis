#!/bin/bash


if [ $# -eq 1 ]; then
  RUNTYPE=$1
else
  echo "Usage:"
  echo "  $0 <Run type>"
  exit 1
fi
echo "Do full production checker on tracking $RUNTYPE data"
echo "=================================================="
echo "Doing transfer check..."
./runscript.sh CheckAllTracker.sql $RUNTYPE
echo "...Done"
echo "=================================================="
sleep 1
echo " " 
echo "=================================================="
echo "Doing event combiner check..."
./make_classified_list.sh
echo "...Done"
echo "=================================================="
sleep 1
echo " "
echo "=================================================="
echo "Doing track cluster DST check..."
psql -h sphnxproddbmaster.sdcc.bnl.gov -d Production -U argouser -f CheckTrkrClusterStatus.sql
echo "...Done"
echo "=================================================="
sleep 1
echo " "
echo " "
bash generate_summary_table.sh
