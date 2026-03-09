#!/bin/tcsh

# This script parses output Fun4All macro logfiles and greps the tracking timing information from the logfiles
# In order for the timing information to show up in your macro, you must have:
# se->PrintTimer();
# at the end of your Fun4All macro, which prints the timers for all modules
# This script produces a txt file called time.txt which  the root macro AnalyzeTime.C is setup to parse
# The script takes two arguments:
# $0 the directory you want the output file to be produced in (e.g. $pwd)
# $1 the directory where the logfiles you want to grep exist

set globdot

cd $1

echo "start of time " > time.txt
foreach infile ($2/*)
    # Checks for a file with .out structure
    if ("`echo ${infile} | cut -d'.' -f2`" == "out" ) then
     echo $infile
     grep -o 'MvtxClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'InttClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'TpcClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'MicromegasClusterizer_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHMicromegasTpcTrackMatching_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'TpcClusterCleaner_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHActsSiliconSeeding_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHActsSiliconSeeding Acts seed time[^"]*' $infile >> time.txt
     grep -o 'PHCASeeding_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PrePropagatorPHTpcTrackSeedCircleFit_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHSimpleKFProp_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHTpcTrackSeedCircleFit_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHSiliconTpcTrackMatching_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHActsTrkFitter_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHSimpleVertexFinder_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHActsVertexPropagator_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHTpcClusterMover_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHTpcDeltaZCorrection_TOP: per event time (ms):[^"]*' $infile >> time.txt
     grep -o 'PHTpcResiduals_TOP: per event time (ms):[^"]*' $infile >> time.txt
    endif
end
