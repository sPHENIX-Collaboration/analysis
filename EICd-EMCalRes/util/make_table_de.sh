#!/bin/sh

#regenerate table_de, which hold condor arguments for rc2de

rm table_de

while read filenamestub flavor pseudorapidity momentum; do
	if [ "$momentum" = "25." ]; then
		filename_rcli_ONE=${filenamestub%????}
		filename_de=$(printf 'DE%s.root' $filename_rcli_ONE)
		printf '%s %s\n' $filename_rcli_ONE $filename_de
	fi
done < table > table_de
