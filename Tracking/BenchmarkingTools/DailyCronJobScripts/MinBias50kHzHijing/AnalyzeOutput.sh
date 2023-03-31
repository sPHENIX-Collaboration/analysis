#!/bin/sh

source /cvmfs/sphenix.sdcc.bnl.gov/gcc-8.3/opt/sphenix/core/bin/sphenix_setup.sh -n 
HOME_DIR="/sphenix/sim/sim01/sphnxpro/TrackingDailyBuild/$(date +"%Y")/$(date +"%d-%m")/MinBias50kHzHijingOutput"
cd $HOME_DIR
YESTERDAY_DIR="/sphenix/sim/sim01/sphnxpro/TrackingDailyBuild/$(date +"%"Y)/$(date -d 'yesterday 13:00' +'%d-%m')/MinBias50kHzHijingOutput"
# creates time.txt file with aggregated timing information
~/git/analysis/Tracking/BenchmarkingTools/TimingTools/parseTimers.sh $HOME_DIR $HOME_DIR

#creates timingoutfile.root containing histogramed timing information
root ~/git/analysis/Tracking/BenchmarkingTools/TimingTools/AnalyzeTime.C\(\"$HOME_DIR/time.txt\",\"$HOME_DIR/../timingoutfile.root\"\)

#Aggregate job file output
cd $HOME_DIR
hadd -k MinBias50kHzHijingDailyBuild_qa.root MinBias50kHzHijing_dailybuild_*_qa.root

hadd -k MinBias50kHzHijingDailyBuild_g4svtx.root MinBias50kHzHijing_dailybuild_*_g4svtx_eval.root


# run qa macros and save histograms in home dir
root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/MvtxQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../MvtxComparisons.root\"\)

root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/InttQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../InttComparisons.root\"\)

root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/TpcQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../TpcComparisons.root\"\)

root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/TpotQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../TpotComparisons.root\"\)

root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/TrackingQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../TrackingComparisons.root\"\)

root ~/git/analysis/Tracking/BenchmarkingTools/PhysicsPerformanceTools/VertexingQA.C\(\"$YESTERDAY_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/MinBias50kHzHijingDailyBuild_qa.root\",\"$HOME_DIR/../VertexingComparisons.root\"\)
