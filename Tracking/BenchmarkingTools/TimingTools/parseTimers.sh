#!/bin/sh

# This script parses output Fun4All macro logfiles and greps the tracking timing information from the logfiles
# In order for the timing information to show up in your macro, you must have:
# se->PrintTimer();
# at the end of your Fun4All macro, which prints the timers for all modules
# This script produces a txt file called time.txt which  the root macro AnalyzeTime.C is setup to parse
# The script takes two arguments:
# $0 the directory you want the output file to be produced in (e.g. $pwd)
# $1 the directory where the logfiles you want to grep exist


cd $1

echo "start of time " > time.txt
for infile in "$2*.out"; do
     #echo $infile
     grep -o -h 'MvtxClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'InttClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'TpcClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'MicromegasClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHMicromegasTpcTrackMatching_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'TpcClusterCleaner_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHActsSiliconSeeding_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHActsSiliconSeeding Acts seed time[^"]*' $infile >> time.txt
     grep -o -h 'PHCASeeding_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PrePropagatorPHTpcTrackSeedCircleFit_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHSimpleKFProp_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHTpcTrackSeedCircleFit_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHSiliconTpcTrackMatching_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHActsTrkFitter_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHSimpleVertexFinder_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHActsVertexPropagator_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHTpcClusterMover_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHTpcDeltaZCorrection_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o -h 'PHTpcResiduals_TOP: per event time (ms):[^"]*' $infile >> time.txt
done
