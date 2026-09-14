#!/bin/bash

root -l -q -b Lambda_Kshort_ratio_data_binned.C | tee out_data.txt
root -l -q -b Lambda_Kshort_ratio_data_pos_binned.C | tee out_data_pos.txt
root -l -q -b Lambda_Kshort_ratio_data_neg_binned.C | tee out_data_neg.txt

root -l -q -b plot_results_data.C
