#!/bin/bash

INFILE=$1
OUTFILE=$2

root -l -b -q get_singletrack_abundance.C\(\"${INFILE}\",\"${OUTFILE}\"\)
