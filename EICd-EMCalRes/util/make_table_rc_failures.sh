#!/bin/sh

#sometimes, g42rc jobs take a long time to run.
#killing them and re-submitting can make stuff go faster.
#so, this script replaces the full table_g4 with the subset of arguments for jobs that failed.

#also, sometimes, for no good reason,
#files are generated with small size (390 bytes).

#i have not proven mathematically that jobs will only fail in these way!
#moral is, do not just trust this script.
#go through the full list of ROOT files manually and check for anything suspicious.

while read name_file_g4 name_file_rc all_the_rest; do
	#check to see if the RC file already exists
	#if not, the job has been killed manually,
	#so add it to table_rc-TEMPORARY
	
	ls_line=$(ls -l ../data/rc | grep -e $name_file_rc)
	if [ "$?" != "0" ]; then
		printf '%s %s %s\n' $name_file_g4 $name_file_rc "$all_the_rest"
	fi
	
	#check to see if any RC files
	#contain only 390 bytes.
	#if so, their job probably failed.
	
	echo "$ls_line" | grep -q -e " 390 "
	if [ "$?" = "0" ]; then
		printf '%s %s %s\n' $name_file_g4 $name_file_rc "$all_the_rest"
		
		#also, remove 390-byte files.
		#otherwise, they do not get re-generated right.
		
		rm ../data/rc/$name_file_rc
	fi
	
	#check to see if any RC files
	#contain exactly 6689 bytes.
	#if so, their G4 file was not properly closed.
	
	echo "$ls_line" | grep -q -e " 6689 "
	if [ "$?" = "0" ]; then
		printf '%s %s %s\n' $name_file_g4 $name_file_rc "$all_the_rest"
		
		#echo a warning, 'cause the *real* failure here
		#is that the G4 job did not complete correctly.
		
		printf "***WARNING!*** $name_file_g4 has not been closed correctly and is corrupted. you must re-generate the file.\n"
	fi
done < table_rc >> table_rc-TEMPORARY

#copy the output to table_rc
#so that the condor job sees only the failed jobs.

mv table_rc-TEMPORARY table_rc
