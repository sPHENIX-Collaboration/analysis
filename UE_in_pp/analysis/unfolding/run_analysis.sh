#!/bin/bash

set -e  # Exit on error
set -u  # Treat unset variables as error
USER='egm2153'

wait_for_condor_jobs() {
    echo "Waiting for Condor jobs to finish..."
    while true; do
        NUM=$(condor_q $USER | grep "^$USER" | wc -l)
        echo "  Remaining Condor jobs: $NUM"
        if [ "$NUM" -eq 0 ]; then
            echo "All Condor jobs completed."
            break
        fi
        sleep 60
    done
}

# Step 1: Run sim iteration 1
condor_submit analyze_sim_1_condor.sub
sleep 60
wait_for_condor_jobs

#cd analysis_sim_run21_output
#hadd -f -k output_dijet_sim_iter_1_jet10.root output_dijet_sim_iter_1_jet10_*.root
#hadd -f -k output_dijet_sim_iter_1_jet20.root output_dijet_sim_iter_1_jet20_*.root
#hadd -f -k output_dijet_sim_iter_1_jet30.root output_dijet_sim_iter_1_jet30_*.root
#hadd -f -k output_dijet_sim_iter_1_jet50.root output_dijet_sim_iter_1_jet50_*.root
#hadd -f -k output_dijet_sim_iter_1.root output_dijet_sim_iter_1_jet10.root output_dijet_sim_iter_1_jet20.root output_dijet_sim_iter_1_jet30.root output_dijet_sim_iter_1_jet50.root
#cd ..
#
## Step 2: Run sim iteration 2
#condor_submit analyze_sim_2_condor.sub
#sleep 60
#wait_for_condor_jobs
#
#cd analysis_sim_run21_output
#hadd -f -k output_dijet_sim_iter_2.root output_dijet_sim_iter_2_jet*_*.root
#cd ..
#
## Step 3: Run data analysis
#condor_submit analyze_data_condor.sub
#sleep 60
#wait_for_condor_jobs
#
#cd analysis_data_run21_output
#hadd -f -k output_data_dijet.root output_dijet_*.root
#cd ..
#
## Step 4: Get reweight matrix
#root -l -q -b 'get_reweightmatrix.C("analysis_sim_run21_output/output_dijet_sim_iter_2.root","analysis_data_run21_output/output_data_dijet.root","output_reweighted_respmatrix_run21_iter_2.root")'
#
## Step 5: Run sim iteration 3
#condor_submit analyze_sim_3_condor.sub
#sleep 60
#wait_for_condor_jobs
#
#cd analysis_sim_run21_output
#hadd -f -k output_dijet_sim_iter_3.root output_dijet_sim_iter_3_jet*_*.root
#cd ..
#
## Step 6: Run final ROOT scripts
#root -l -q -b 'do_closure.C("analysis_sim_run21_output/output_dijet_sim_iter_3.root","output_closure_sim_run21_iter_3_1000toys.root")'
#root -l -q -b 'do_unfold.C("analysis_sim_run21_output/output_dijet_sim_iter_3.root","analysis_data_run21_output/output_data_dijet.root")'
#hadd -f -k output_unfolded_data_run21_iter_3_1000toys.root output_unfolded_data_calib_dijet_*run21_iter_3_1000toys.root
#root -l -q -b 'plot_closure.C()'
#root -l -q -b 'plot_unfold.C()'
#root -l -q -b 'plot_result.C()'
#root -l -q -b 'plot_reweight.C()'

echo "All steps complete!"

