#!/bin/tcsh -f

set infilename=$1
set outfilename=$2

foreach line ("`cat data/rcli1/$infilename`")
	set argv=( $line )
	set filename="$1"
	set momentum="$2"
	set qafilename=`printf 'QA%s.pdf' $filename`
	root -l -q -b -x macro/rc2de.C\(\"data/rcli0/$filename\",\"data/de/$outfilename\",\"data/qa0/$qafilename\",$momentum\)
end
