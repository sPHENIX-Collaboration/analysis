#!/bin/tcsh -f

set outfilename=$1
set flavor=$2
set pseudorapidity=$3
set momentum=$4

root -l -q -b -x macro/make_g4.C\(\"data/g4/$outfilename\",\"$flavor\",$pseudorapidity,$momentum\)
