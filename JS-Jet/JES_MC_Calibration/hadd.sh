#!/bin/bash

# Usage: ./hadd.sh [runindex]   (default 38; the trigger list is selected automatically)
runindex=${1:-38}
if [[ $runindex -eq 28 ]]; then
  trig_array=(0 5 12 20 30 40 50 60 80)
else
  trig_array=(12 20 30 40 50 60 80)
fi
runindex_array=($runindex)
echo "hadd: runindex=$runindex triggers=${trig_array[*]}"

for runindex in "${runindex_array[@]}"; do
    for trig in "${trig_array[@]}"; do
      if   [[ $trig -eq 0  ]]; then runtype="MB"
      elif [[ $trig -eq 5 ]]; then runtype="Jet5GeV"
      elif [[ $trig -eq 12 ]]; then runtype="Jet12GeV"
      elif [[ $trig -eq 20 ]]; then runtype="Jet20GeV"
      elif [[ $trig -eq 30 ]]; then runtype="Jet30GeV"
      elif [[ $trig -eq 40 ]]; then runtype="Jet40GeV"
      elif [[ $trig -eq 50 ]]; then runtype="Jet50GeV"
      elif [[ $trig -eq 60 ]]; then runtype="Jet60GeV"
      elif [[ $trig -eq 80 ]]; then runtype="Jet80GeV"
      fi

      OUTFILE="condor_sim/output_run${runindex}_${runtype}.root"

      if [ -f "$OUTFILE" ]; then
          echo "Removing existing $OUTFILE"
          rm -f "$OUTFILE"
      fi

      hadd -j -k "$OUTFILE" condor_sim/Run${runindex}_${runtype}/output_*.root

    done
done
