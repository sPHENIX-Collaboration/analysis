#!/bin/tcsh -f

set infilename=$1
set outfilename=$2
set flavor=$3
set pseudorapidity=$4
set momentum=$5

root -l -q -b -x macro/g42rc.C\(\"data/g4/$infilename\",\"data/rc/$outfilename\",\"$flavor\",$pseudorapidity,$momentum\)
