#!/bin/bash

root -l -q -b Lambda_Kshort_ratio_MC.C | tee out.txt
root -l -q -b Lambda_Kshort_ratio_MC_pos.C | tee out_pos.txt
root -l -q -b Lambda_Kshort_ratio_MC_neg.C | tee out_neg.txt

root -l -q -b plot_results_MC.C
