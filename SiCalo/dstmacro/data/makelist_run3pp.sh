#! /bin/bash

#79508 79510 79511 79512 79513 79514 79515
#79516 79523 79524 79525 79526 79527 79528
#79529 79530 79564 79565 79566 79567 79568
#79569 79570 79571 79572 80858 80859 80861
#80864 80865 80866 80878 80879 80880 80891
#80892 80893 80895 80901 80902 80904 80905
#80906 80908 80909 80910 80912 80915 81556
#81558 81559 81561 81566 81575 81577 81580
#81581 81583 81584 81585 81586 81600 81601
#81602 81603 81605 81607 81609 81610 81611

RUN=${1:-79516}

#RUN=79516
echo $RUN
#exit

RUN8=$(printf "%08d" "$RUN") 

RUN_RNG_B=$(( RUN / 100 * 100 ))
RUN_RNG_E=$(( ((RUN / 100)+1) * 100 ))
RUN_RNG=$(printf "%08d_%08d" "$RUN_RNG_B" "$RUN_RNG_E")

#CALO file = 100000 evt
#SEED file = 1000 evt

#x=78516
#y=$(( x / 100 * 100 ))

#echo $RUN_ROUNDING $RUN_ROUNDING1

CALO_VER="pro001_pcdb001_v001" 
SEED_VER="ana573_2026p003_v001"

BASE_LISTOUT="/sphenix/user/hachiya/myrepo/analysis/SiCalo/dstmacro/data/list_run3pp"
BASE_ANAOUT="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/run3pp"


BASE_calo="/sphenix/lustre01/sphnxpro/production/run3pp/physics/${CALO_VER}/DST_CALOFITTING/run_${RUN_RNG}/DST_CALOFITTING_run3pp_${CALO_VER}-${RUN8}-"
BASE_seed="/sphenix/lustre01/sphnxpro/production/run3pp/physics/${SEED_VER}/DST_TRKR_SEED/run_${RUN_RNG}/DST_TRKR_SEED_run3pp_${SEED_VER}-${RUN8}-"

#echo $BASE_calo
#echo $BASE_seed

OUT_alllist="./joblist/dst_run${RUN}.list"
: > "$OUT_alllist"

Nevt=100000

NCALO=$(ls ${BASE_calo}*.root | wc -l)
echo $NCALO

SEEDFILELIST="dst_trkr_seed-${RUN8}.list"

echo $SEEDFILELIST

#exit

 i=0
 while [ $i -lt $NCALO ]; do  #number of calo dst 
# while [ $i -le 1 ]; do  #number of calo dst 
   printf -v I '%05d' "$i" 
   SUF="$I"

   OUT_calo="${BASE_LISTOUT}/calo/${RUN}/run${RUN}_calo_${I}.list"
   OUT_seed="${BASE_LISTOUT}/seed/${RUN}/run${RUN}_seed_${I}.list"   

   OUTDIR_condor="${BASE_ANAOUT}/${RUN}"   
   OUT_condor="${OUTDIR_condor}/ana_${RUN}_${I}.root"   

   echo $OUT_calo
   echo $OUT_seed
#
#   echo $OUTDIR_condor
#   echo $OUT_condor   

   if [ ! -d "$(dirname $OUT_calo)" ]; then
       mkdir -p "$(dirname $OUT_calo)"
   fi

   if [ ! -d "$(dirname $OUT_seed)" ]; then
       mkdir -p "$(dirname $OUT_seed)"
   fi

   if [ ! -d "$OUTDIR_condor" ]; then
       mkdir -p "$OUTDIR_condor"
   fi

   Sevt=$((Nevt*i))

   CALO_file=$(printf "%s%s.root\n" "${BASE_calo}" "${SUF}")
   #echo $CALO_file


   # if calo dst exist, write it to the list file
   if [ -f "$CALO_file" ]; then
     printf "%s%s.root\n" "${BASE_calo}" "${SUF}" > "$OUT_calo"
   fi


   : > "$OUT_seed"
   l=0
     while [ $l -lt 100 ]; do
       j=$((100*i+l))

       printf -v J '%05d' "$j"
       # SUF_trkr=$(printf "%05d" "$j")  
       #printf "%s%s.root\n" "${BASE_seed}" "${J}" >> "$OUT_seed" 

       SEEDFILE=$(printf "%s%05d.root" "${BASE_seed}" "${j}")
       SEEDFILENAME=$(basename $SEEDFILE)
       #if [  -f $SEEDFILE ]; then
       if  grep -q "$SEEDFILENAME" $SEEDFILELIST; then
         #echo $SEEDFILE
         echo $SEEDFILE >> "$OUT_seed"
       fi

       l=$((l+1))
     done
   printf "%s %s %s %s %s\n" "$Nevt" "$OUT_seed" "$OUT_calo" "$OUT_condor" "$Sevt">> "$OUT_alllist" 
 
   i=$((i+1))
 done
 
 printf "$OUT_alllist "
 
 echo "all done"
