#!/bin/bash

root -l -q -b Lambda_Kshort_ratio_NN_binned.C | tee out_NN.txt
root -l -q -b Lambda_Kshort_ratio_NN_pos_binned.C | tee out_NN_pos.txt
root -l -q -b Lambda_Kshort_ratio_NN_neg_binned.C | tee out_NN_neg.txt

root -l -q -b plot_results_NN.C
