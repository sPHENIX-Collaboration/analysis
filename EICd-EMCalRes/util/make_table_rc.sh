#!/bin/sh

#regenerate table_rc, which holds condor arguments for g42rc

#since this takes table_g4 as input,
#make sure that table_g4 is fully regenerated
#before running this script.

rm table_rc

while read g4filename flavor pseudorapidity momentum; do
	printf "%s " $g4filename
	printf "RC%s " ${g4filename:2}
	printf "%s %s %s\n" $flavor $pseudorapidity $momentum
done < table_g4 > table_rc
