#!/bin/bash
#
# 	use CreateDstList to get filelists for 4 different DST types for all QM40 runs
# 	and build output list where all 4 segments (CALO, CLUS, SEED, TRACKS) exist!
#		bill llope 20250810
#
leading_zero_fill () {
    printf "%0$1d\\n" "$2"
}

#---- begin user setup -----------------------------
#
RUNSLIST="./QM40runs.list"
#
OUTPUTDIR="./LISTS"
mkdir -p $OUTPUTDIR
#
DST_CALO="DST_CALO"
DST_CLUS="DST_TRKR_CLUSTER"
DST_SEED="DST_TRKR_SEED"
DST_TRAC="DST_TRKR_TRACKS"
prodCALO="ana502_2024p022_v001"
prodCLUS="ana494_2024p021_v001"
prodSEED="ana494_2024p021_v001"
prodTRAC="ana495_2024p021_v001"
listdirCALO=${OUTPUTDIR}"/DST_CALO_ana502"
listdirCLUS=${OUTPUTDIR}"/DST_TRKR_CLUSTER_ana494"
listdirSEED=${OUTPUTDIR}"/DST_TRKR_SEED_ana494"
listdirTRAC=${OUTPUTDIR}"/DST_TRKR_TRACKS_ana495"
#
#---- end user setup -------------------------------

datestr=$(date '+%Y%m%d')
OUTPUTLIST=${OUTPUTDIR}"/catlist_"${datestr}".txt"
echo output file: $OUTPUTLIST
rm -fv $OUTPUTLIST
touch $OUTPUTLIST
dst_CALO=`echo "${DST_CALO,,}"`		# make lowercase
dst_CLUS=`echo "${DST_CLUS,,}"`		# make lowercase
dst_SEED=`echo "${DST_SEED,,}"`		# make lowercase
dst_TRAC=`echo "${DST_TRAC,,}"`		# make lowercase

mkdir -p $listdirCALO && rm -f $listdirCALO/*.list
mkdir -p $listdirCLUS && rm -f $listdirCLUS/*.list
mkdir -p $listdirSEED && rm -f $listdirSEED/*.list
mkdir -p $listdirTRAC && rm -f $listdirTRAC/*.list

./myCreateDstList.pl --tag $prodCALO --list $RUNSLIST $DST_CALO
mv ${dst_CALO}*.list $listdirCALO
echo $prodCALO $RUNSLIST $DST_CALO -- `ls -1 $listdirCALO/*.list | wc -l` runs found

./myCreateDstList.pl --tag $prodCLUS --list $RUNSLIST $DST_CLUS
mv ${dst_CLUS}*.list $listdirCLUS
echo $prodCLUS $RUNSLIST $DST_CLUS -- `ls -1 $listdirCLUS/*.list | wc -l` runs found

./myCreateDstList.pl --tag $prodSEED --list $RUNSLIST $DST_SEED
mv ${dst_SEED}*.list $listdirSEED
echo $prodSEED $RUNSLIST $DST_SEED -- `ls -1 $listdirSEED/*.list | wc -l` runs found

./myCreateDstList.pl --tag $prodTRAC --list $RUNSLIST $DST_TRAC
mv ${dst_TRAC}*.list $listdirTRAC
echo $prodTRAC $RUNSLIST $DST_TRAC -- `ls -1 $listdirTRAC/*.list | wc -l` runs found

#----- now loop over runs in RUNSLIST and build list of segments where all 4 DSTs exist
echo Starting...
numfoundsegtotal=0
while IFS= read -r runnum; do
	#echo doing $runnum
	numfoundseg=0
	paddedrunnum=`leading_zero_fill 8 $runnum`
    for segment in $(seq 0 3000); do 		#!!!!!!!!! keep an eye on max # of segments/run! 
    	paddedsegment=`leading_zero_fill 5 $segment`
		#
		found_CALO=false
		found_CLUS=false
		found_SEED=false
		found_TRAC=false
		#
 		segment2="$((segment / 10))"				# if 10 tracking files per calo file
 		paddedsegment2=`leading_zero_fill 5 $segment2`
	    fileCALO=${DST_CALO}_run2pp_${prodCALO}-${paddedrunnum}-${paddedsegment2}.root
		filelist=${listdirCALO}/${dst_CALO}-${paddedrunnum}.list
		if grep -Fxq "$fileCALO" "$filelist"; then
			found_CALO=true
	    fi
	    #
	    fileCLUS=${DST_CLUS}_run2pp_${prodCLUS}-${paddedrunnum}-${paddedsegment}.root
		filelist=${listdirCLUS}/${dst_CLUS}-${paddedrunnum}.list
		if grep -Fxq "$fileCLUS" "$filelist"; then
			found_CLUS=true
	    fi
	    #
	    fileSEED=${DST_SEED}_run2pp_${prodSEED}-${paddedrunnum}-${paddedsegment}.root
		filelist=${listdirSEED}/${dst_SEED}-${paddedrunnum}.list
		if grep -Fxq "$fileSEED" "$filelist"; then
			found_SEED=true
	    fi
	    #
	    fileTRAC=${DST_TRAC}_run2pp_${prodTRAC}-${paddedrunnum}-${paddedsegment}.root
		filelist=${listdirTRAC}/${dst_TRAC}-${paddedrunnum}.list
		if grep -Fxq "$fileTRAC" "$filelist"; then
			found_TRAC=true
	    fi
	    #
		if [[ "$found_CALO" == "true" 
		   && "$found_CLUS" == "true" 
		   && "$found_SEED" == "true" 
		   && "$found_TRAC" == "true" ]]; then
			#echo Found all 4..  $runnum $segment $filesearch
			numfoundseg=$((numfoundseg+1))
			numfoundsegtotal=$((numfoundsegtotal+1))
		    #       trac      seed      clus      calo
		    echo 0,$fileTRAC,$fileSEED,$fileCLUS,$fileCALO >> $OUTPUTLIST
		fi
	    #
    done
	echo found $numfoundseg segments in run $runnum, $numfoundsegtotal total
done < QM40runs.list

exit


# #---- print available tags
# #CreateDstList.pl --printtags --dataset run2pp
# #---- make list for one run
# #CreateDstList.pl DST_CALO --run 53877 --tag ana468_2024p012_v001
# 
# #---- fixed build DSTs...
# #CreateDstList.pl --tag ana502_2024p022_v001 DST_CALO --dataset run2pp --printruns
# CreateDstList.pl --tag ana502_2024p022_v001 --list QM40runs.list DST_CALO
# mv dst_calo-*.list LISTS_TrackFitting/DST_CALO_ana502
# 
# #CreateDstList.pl --tag ana494_2024p021_v001 DST_TRKR_CLUSTER --dataset run2pp --printruns
# CreateDstList.pl --tag ana494_2024p021_v001 --list QM40runs.list DST_TRKR_CLUSTER
# mv dst_trkr_cluster-*.list LISTS_TrackFitting/DST_TRKR_CLUSTER_ana494
# 
# #CreateDstList.pl --tag ana494_2024p021_v001 DST_TRKR_SEED --dataset run2pp --printruns
# CreateDstList.pl --tag ana494_2024p021_v001 --list QM40runs.list DST_TRKR_SEED
# mv dst_trkr_seed-*.list LISTS_TrackFitting/DST_TRKR_SEED_ana494
# 
# #CreateDstList.pl --tag ana495_2024p021_v001 DST_TRKR_TRACKS --dataset run2pp --printruns
# CreateDstList.pl --tag ana495_2024p021_v001 --list QM40runs.list DST_TRKR_TRACKS
# mv dst_trkr_tracks-*.list LISTS_TrackFitting/DST_TRKR_TRACKS_ana495

