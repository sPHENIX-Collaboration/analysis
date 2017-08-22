#!/bin/tcsh -f

set infilename=$1
set outfilename=$2

foreach line ("`cat data/rcli1_XEMC/$infilename`")
	set argv=( $line )
	set filename="$1"
	set momentum="$2"
	set qafilename=`printf 'QA%s.pdf' $filename`
	root -l -q -b -x macro/rc2de.C\(\"data/rcli0_XEMC/$filename\",\"data/de_XEMC/$outfilename\",\"data/qa0_XEMC/$qafilename\",$momentum\)
end
