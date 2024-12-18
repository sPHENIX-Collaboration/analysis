#!/bin/bash
source /opt/sphenix/core/bin/sphenix_setup.sh -n 

python makeCondorJobs_jet.py
python makeCondorJobs_calo.py 
