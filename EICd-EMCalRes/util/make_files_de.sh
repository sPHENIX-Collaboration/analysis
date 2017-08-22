#!/bin/sh

#in order to generate DE files, rc2de requires a list of RC files.
#these lists are stored in the RCLI0 files.

#since each DE file is written by multiple rc2de runs, the macros need to be called in series.
#so, the rc2de.csh condor job file calls rc2de.C several times, with a different RCLI0 list each time.
#the RCLI1 list files keep track of which RCLI0 lists to use.

#also, since no one rc2de call is responsible for generating the DE file, i start empty DE files from here.

#if you got all that shit on the first pass, good for you.
#it is hard for me to keep all this in my mind, and i wrote it.
#..real sorry 'bout this.

rm ../data/rcli0/*
rm ../data/rcli1/*
rm ../data/de/*

while read filenamestub flavor pseudorapidity momentum; do
	#every loop... make rcli0 file
	
	filename_rcli_ZERO=$(printf '../data/rcli0/%s' $filenamestub)
	printf "" > $filename_rcli_ZERO
	for index in $(seq 0 9); do
		printf 'data/rc/RC%s%d.root\n' $filenamestub $index >> $filename_rcli_ZERO
	done
	
	#every so often... start rcli1 file, make DE file
	
	if [ "$momentum" = "25." ]; then
		filenamestub_truncated="${filenamestub%????}"
		
		filename_rcli_ONE=$(printf '../data/rcli1/%s' $filenamestub_truncated)
		printf "" > $filename_rcli_ONE
		
		filename_de=$(printf '../data/de/DE%s.root' $filenamestub_truncated)
		root -l -q -b -x make_file_de.C\(\"$filename_de\"\)
	fi
	
	#every loop... fill rcli1 file with new rcli0 file
	
	printf '%s %s\n' ${filename_rcli_ZERO##*/} $momentum >> $filename_rcli_ONE
done < table
