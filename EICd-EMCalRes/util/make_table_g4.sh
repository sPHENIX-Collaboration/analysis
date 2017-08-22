#!/bin/sh

#regenerate table_g4, which hold condor arguments for make_g4

rm table_g4

while read filenamestub flavor pseudorapidity momentum; do
	for index in $(seq 0 9); do
		printf "G4%s%d.root %s %s %s\n" $filenamestub $index $flavor $pseudorapidity $momentum
	done
done < table > table_g4
