#! /bin/bash

RUN=79516
RUN8=$(printf "%08d" "$RUN") 

RUN_ROUNDING=$(( RUN / 100 * 100 ))
RUN_ROUNDING1=$(( ((RUN / 100)+1) * 100 ))
#x=78516
#y=$(( x / 100 * 100 ))

#echo $RUN_ROUNDING $RUN_ROUNDING1

BASE_LISTOUT="/sphenix/user/hachiya/myrepo/analysis/SiCalo/dstmacro/data/list_run3pp"
BASE_ANAOUT="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/run3pp"


BASE_calo="/sphenix/lustre01/sphnxpro/production2/run3pp/physics/calofitting/new_newcdbtag_v008/run_000${RUN_ROUNDING}_000${RUN_ROUNDING1}/DST_CALOFITTING_run3pp_new_newcdbtag_v008-${RUN8}-"
BASE_seed="/sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_SEED/run_000${RUN_ROUNDING}_000${RUN_ROUNDING1}/DST_TRKR_SEED_run3pp_ana538_2025p011_v001-${RUN8}-"
#-- #          /sphenix/lustre01/sphnxpro/production2/run3pp/physics/calofitting/new_newcdbtag_v008/run_00079500_00079600/DST_CALOFITTING_run3pp_new_newcdbtag_v008-00079516-
#-- #           /sphenix/lustre01/sphnxpro/production/run3pp/physics/ana538_2025p011_v001/DST_TRKR_SEED/run_00079500_00079600/DST_TRKR_SEED_run3pp_ana538_2025p011_v001-00079516-37800.root
#-- #BASE_clus="/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana504_2024p023_v001/DST_TRKR_CLUSTER/run_00053000_00053100/dst/DST_TRKR_CLUSTER_run2pp_ana504_2024p023_v001-${RUN8}-"

#echo $BASE_calo
#echo $BASE_seed

OUT_alllist="./joblist/dst_run${RUN}.list"
: > "$OUT_alllist"

Nevt=100000
 

NCALO=378
#NCALO=5

 i=0
 while [ $i -le $NCALO ]; do  #number of calo dst 
   printf -v I '%05d' "$i" 
   SUF="$I"

   OUT_calo="${BASE_LISTOUT}/calo/run${RUN}_calo_${I}.list"
   OUT_seed="${BASE_LISTOUT}/seed/run${RUN}_seed_${I}.list"   
   #OUT_clus="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/macro/makedata/list/clus/run${RUN}_clus_${I}.list"

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
       printf "%s%s.root\n" "${BASE_seed}" "${J}" >> "$OUT_seed" 
       l=$((l+1))
     done
   printf "%s %s %s %s %s\n" "$Nevt" "$OUT_seed" "$OUT_calo" "$OUT_condor" "$Sevt">> "$OUT_alllist" 
 
   i=$((i+1))
 done
 
 printf "$OUT_alllist "
 
 echo "all done"
