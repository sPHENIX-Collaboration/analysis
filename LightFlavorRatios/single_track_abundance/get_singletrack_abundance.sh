#!/bin/bash

INFILE=$1
DEDXFILE=$2
OUTFILE=$3

root -l -b -q get_singletrack_abundance.C\(\"${INFILE}\",\"${DEDXFILE}\",\"${OUTFILE}\"\)
