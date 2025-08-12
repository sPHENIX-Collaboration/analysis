#!/bin/bash
#
# 	use CreatDstList.pl to search for all productions that have produced files for run 53877
#		bill llope 20250810
#
leading_zero_fill () {
    printf "%0$1d\\n" "$2"
}
DOIT() {
	prodname=$1
	DST=$2
	OUTDIR=$3
	dst=`echo "${DST,,}"`		# make lowercase
	if [ -e $dst-00053877.list ]; then
	  rm -fq $dst-00053877.list
	fi
	myCreateDstList.pl --tag $prodname --run 53877 $DST
	if [ -e $dst-00053877.list ]; then
		mv $dst-00053877.list $OUTDIR/prodlist/${prodname}_${DST}_53877.list
		numl=$(wc -l < $OUTDIR/prodlist/${prodname}_${DST}_53877.list)
		printf "%19s\t%-16s\t%4d\n" $prodname $DST $numl
	fi	
}

OUTPUTDIR="./LISTS"
mkdir -p $OUTPUTDIR/prodlist/
datestr=$(date '+%Y%m%d')

CreateDstList.pl --printtags --dataset run2pp > $OUTPUTDIR/catprod.txt

#----- now loop over runs in RUNSLIST and build list of segments where all 4 DSTs exist
echo Starting...
while IFS= read -r prodname; do
	#echo Start $prodname ------------------------------
	#
	DOIT $prodname "DST_TRKR_CLUSTER" $OUTPUTDIR
	DOIT $prodname "DST_TRKR_SEED" $OUTPUTDIR
	DOIT $prodname "DST_TRKR_TRACKS" $OUTPUTDIR
	DOIT $prodname "DST_CALO" $OUTPUTDIR
	#
done < $OUTPUTDIR/catprod.txt
 
exit

