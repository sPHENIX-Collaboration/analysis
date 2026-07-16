#! /bin/bash

RUN=53046
RUN8=$(printf "%08d" "$RUN") 

BASE_calo="/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana468_2024p012_v001/DST_CALOFITTING/run_00053000_00053100/dst/DST_CALOFITTING_run2pp_ana468_2024p012_v001-${RUN8}-"
BASE_clus="/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana504_2024p023_v001/DST_TRKR_CLUSTER/run_00053000_00053100/dst/DST_TRKR_CLUSTER_run2pp_ana504_2024p023_v001-${RUN8}-"
BASE_seed="/sphenix/lustre01/sphnxpro/production/run2pp/physics/ana506_2024p023_v002/DST_TRKR_SEED/run_00053000_00053100/dst/DST_TRKR_SEED_run2pp_ana506_2024p023_v002-${RUN8}-"

OUT_alllist="./alllist/dst_run${RUN}.list"
: > "$OUT_alllist"
Nevt=50000

i=0
while [ $i -le 95 ]; do  #number of calo dst 
  printf -v I '%05d' "$i" 
  SUF="$I"
  OUT_calo="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/macro/makedata/list/calo/run${RUN}_calo_${I}.list"
  OUT_clus="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/macro/makedata/list/clus/run${RUN}_clus_${I}.list"
  OUT_seed="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/macro/makedata/list/seed/run${RUN}_seed_${I}.list"   
  OUT_condor="/sphenix/tg/tg01/commissioning/INTT/work/mahiro/SIliconCalo/run24pp/macro/makedata/condor/data/${RUN}/ana_${RUN}_${I}.root"   
  mkdir -p "$(dirname "$OUT_condor")"
  Sevt=$((Nevt*i))

  printf "%s%s.root\n" "${BASE_calo}" "${SUF}" > "$OUT_calo"
  : > "$OUT_clus"
  : > "$OUT_seed"
  l=0
    while [ $l -le 4 ]; do
      j=$((5*i+l))
      printf -v J '%05d' "$j"
      # SUF_trkr=$(printf "%05d" "$j")  
      printf "%s%s.root\n" "${BASE_clus}" "${J}" >> "$OUT_clus"
      printf "%s%s.root\n" "${BASE_seed}" "${J}" >> "$OUT_seed" 
      l=$((l+1))
    done
  printf "%s %s %s %s %s %s\n" "$Nevt" "$OUT_clus" "$OUT_seed" "$OUT_calo" "$OUT_condor" "$Sevt">> "$OUT_alllist" 

  i=$((i+1))
done

printf "$OUT_alllist "

echo "all done"