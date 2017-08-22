#!/bin/sh

#regenerate table_de_XEMC, which hold condor arguments for rc2de_XEMC

rm table_de_XEMC

for filename_rcli1 in $(ls -1 ../data/rcli1_XEMC); do
	filename_de=$(printf 'DE%s.root' $filename_rcli1)
	printf "%s %s\n" $filename_rcli1 $filename_de
done > table_de_XEMC
