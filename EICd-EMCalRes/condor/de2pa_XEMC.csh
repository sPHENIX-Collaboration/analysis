#!/bin/tcsh -f

set defilename=$1
set pafilename=$2
set qafilename=$3

root -l -q -b -x macro/de2pa.C\(\"data/de_XEMC/$defilename\",\"data/pa_XEMC/$pafilename\",\"data/qa1_XEMC/$qafilename\"\)
