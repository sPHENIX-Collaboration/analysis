#! /bin/bash

# Find runs with a hot map in the CDB
hotmapslist="all_runs_with_hotmap.txt"
find /cvmfs/sphenix.sdcc.bnl.gov/calibrations/sphnxpro/cdb/CEMC_BadTowerMap/ -name '*cdb.root' | grep -E -o '[0-9]{5}cdb.root' | cut -d 'c' -f1 | sort | uniq > $hotmapslist

# Find intersection of GoldenFEM and hotmap run lists
outfile="emcal_withmaps.txt"
grep -Ff $hotmapslist "GoldenFEMrunList.txt" > $outfile
