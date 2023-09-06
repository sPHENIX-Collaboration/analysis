#! /bin/bash

#Run analysis over a set of runs, taken from csv file
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
./GetET "/sphenix/tg/tg01/jets/ahodges/run23_production/21518/DST-00021518-0000.root"
