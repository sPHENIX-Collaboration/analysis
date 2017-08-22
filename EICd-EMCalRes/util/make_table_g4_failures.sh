#!/bin/sh

#much of the time, make_g4 jobs fail.
#when this happens, the output ROOT file generally holds only 282 bytes of header.
#so, this script replaces the full table_g4 with the subset of arguments for jobs that failed.

#i have not proven mathematically that jobs will only fail with 282 bytes.
#when RC jobs fail, they generally leave files with 390 bytes.
#moral is, do not just trust this script!
#go through the full list of ROOT files manually and check for anything suspicious.

ls -l ../data/g4 | grep -e " 282 " | while read line; do
	#extract just the G4 file name from the "ls -l" output.
	
	filename="$(printf '%s' $line | sed 's/.*\(G4.*root\)/\1/')"
	
	#find the line with that file name in table_g4
	
	grep table_g4 -e $filename
done > table_g4-TEMPORARY

#copy the output the table_g4
#so that the condor job sees only the failed jobs.

mv table_g4-TEMPORARY table_g4
