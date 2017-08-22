#!/bin/tcsh -f

set defilename=$1
set pafilename=$2
set qafilename=$3

root -l -q -b -x macro/de2pa.C\(\"data/de/$defilename\",\"data/pa/$pafilename\",\"data/qa1/$qafilename\"\)
