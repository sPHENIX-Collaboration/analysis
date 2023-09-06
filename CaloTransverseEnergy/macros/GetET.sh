#! bin/bash
 
#####A file to run the generic version of the file, to be called by the condor ######
source /cvmfs/sphenix.sdcc.bnl.gov/gcc-12.1.0/opt/sphenix/core/bin/sphenix_setup.sh -n ana
root -x GetET.C($1) 
