#!/bin/bash

INFILE=$1
PARTICLE=$2
OUTFILE=$3

root -l -q -b plot_mass.C\(\"${INFILE}\",\"${PARTICLE}\",\"${OUTFILE}\"\)
