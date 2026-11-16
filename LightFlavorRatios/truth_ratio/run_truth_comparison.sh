#!/bin/bash

mkdir -p plots/yields
mkdir -p plots/ratios
mkdir -p plots/closure
mkdir -p plots/corrections

root -l -q -b plot_truth_comparison.C
