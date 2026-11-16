#!/bin/bash
mkdir -p condor_sim/out/
mkdir -p condor_sim/err/

chmod +x "CondorRunSim.sh"
> ListCondorRunSim.csv

segperjob=20

# Usage: ./run_condor_sim.sh [runindex] [closure_mode]
#   runindex:     28 or 38 (default 38)
#   closure_mode: 0 = first pass, 1 = closure test, 2 = full-chain closure (default 1;
#                 mode 2 needs calib_r0R.root AND JES_Calibration_r0R.root in offline/)
# The trigger list is selected automatically: Run 28 has MB..Jet80, Run 38 has Jet12-80.
runindex=${1:-38}
do_closure=${2:-1}
if [[ $runindex -eq 28 ]]; then
  trig_array=(0 5 12 20 30 40 50 60 80)
else
  trig_array=(12 20 30 40 50 60 80)
fi
runindex_array=($runindex)
echo "run_condor_sim: runindex=$runindex closure_mode=$do_closure triggers=${trig_array[*]}"

for runindex in "${runindex_array[@]}"; do
  for trig in "${trig_array[@]}"; do
    if   [[ $trig -eq 0  ]]; then runtype="MB"
    elif [[ $trig -eq 5  ]]; then runtype="Jet5GeV"
    elif [[ $trig -eq 12 ]]; then runtype="Jet12GeV"
    elif [[ $trig -eq 20 ]]; then runtype="Jet20GeV"
    elif [[ $trig -eq 30 ]]; then runtype="Jet30GeV"
    elif [[ $trig -eq 40 ]]; then runtype="Jet40GeV"
    elif [[ $trig -eq 50 ]]; then runtype="Jet50GeV"
    elif [[ $trig -eq 60 ]]; then runtype="Jet60GeV"
    elif [[ $trig -eq 80 ]]; then runtype="Jet80GeV"
    fi
    echo "RunIndex: $runindex, Trigger: $runtype"
    mkdir -p condor_sim/Run${runindex}_${runtype}/
    rm condor_sim/Run${runindex}_${runtype}/*
    rm condor_sim/out/condor_${trig}*
    rm condor_sim/err/condor_${trig}*
    # Input trees: the MBD-vertex simulation production (must match Jet_resp_histmaker.C)
    treepath="/sphenix/tg/tg01/jets/hanpuj/Run${runindex}_MBDz/single"
    nfiles=$(ls "$treepath"/output_${runtype}_*.root 2>/dev/null | wc -l)
    if [[ $nfiles -eq 0 ]]; then
        echo "ERROR: no input files found: ${treepath}/output_${runtype}_*.root"
        exit 1
    fi
    # Segment range = 0 .. highest index present (NOT the file count): failed condor
    # jobs leave gaps in the index sequence, and counting files would drop the tail.
    # The histmaker skips missing indices itself.
    maxidx=$(ls "$treepath"/output_${runtype}_*.root | sed -E 's/.*_([0-9]+)\.root$/\1/' | sort -n | tail -1)
    totalseg=$((maxidx + 1))
    echo "  ${nfiles} files, highest index ${maxidx} -> ${totalseg} segments"
    total_condorjobs=$(( (totalseg + segperjob - 1) / segperjob ))
    for ((i=0; i<total_condorjobs; i++)); do
        if [[ $(((i+1)*segperjob)) -gt $totalseg ]]; then
            nseg=$((totalseg - i*segperjob))
        else
            nseg=$segperjob
        fi
        iseg=$((i*segperjob))
        echo "$trig,$nseg,$iseg,$do_closure,$runindex" >> ListCondorRunSim.csv
    done
  done
done

echo "Arguement list formed!"

condor_submit RunCondorSim.sub
