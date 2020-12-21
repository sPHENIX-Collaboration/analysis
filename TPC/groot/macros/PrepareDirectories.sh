#! /bin/sh

# After copying all the macro files into your own work directory,
# run this script to set up all the directories that the condor
#  stuff is expecting...

mkdir logs
mkdir scratch
mkdir ALL_JOBS
ln -s /sphenix/user/hemmick/data/fnal_June2018/ data
