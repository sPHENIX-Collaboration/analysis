#! /bin/bash

RUN=53018
#RUN=53877
RUN8=$(printf "%08d" "$RUN") 

RUN_RNG_B=$(( RUN / 100 * 100 ))
RUN_RNG_E=$(( ((RUN / 100)+1) * 100 ))
RUN_RNG=$(printf "%08d_%08d" "$RUN_RNG_B" "$RUN_RNG_E")

#echo $RUN_RNG_B $RUN_RNG_E $RUN_RNG

# calo : allruns ana509_2024p022_v001
#        (old) allruns ana468_2024p012_v001
# clus : allruns ana517_2024p024_v001
#        (old)ana466_2024p012_v001 52077 52078
#        (old)ana504_2024p023_v001 53018 53046
#        (old)53741 53742 53743 53744 53756 53783 53821 53871 53877 53879
# seed:  ana475_2024p018_v001 52077 52078
#        ana506_2024p023_v002 53018 53046
#        ana506_2024p023_v001 53741 53742 53743 53744 53756 53783 53821 53871 53877 53879
#        (old) ana473_2024p016_v001 52077 52078

CALO_VER="ana509_2024p022_v001"
#CLUS_VER="ana517_2024p024_v001"
CLUS_VER="ana505_2024p023_v001"
SEED_VER="ana506_2024p023_v001"
if (( $RUN >= 52077 && $RUN <= 52078 )); then
  SEED_VER="ana475_2024p018_v001"
elif (( $RUN >= 53018 && $RUN <= 53046 )); then
  SEED_VER="ana506_2024p023_v002"
fi

echo $CALO_VER $CLUS_VER $SEED_VER

BASE_LISTOUT="/sphenix/user/hachiya/myrepo/analysis/SiCalo/dstmacro/data/list_run2pp"
BASE_ANAOUT="/sphenix/tg/tg01/commissioning/INTT/work/hachiya/SiCaloTrack/data/run2pp"

BASE_calo="/sphenix/lustre01/sphnxpro/production2/run2pp/physics/calofitting/${CALO_VER}/run_${RUN_RNG}/DST_CALOFITTING_run2pp_${CALO_VER}-${RUN8}-"
BASE_clus="/sphenix/lustre01/sphnxpro/production/run2pp/physics/${CLUS_VER}/DST_TRKR_CLUSTER/run_${RUN_RNG}/dst/DST_TRKR_CLUSTER_run2pp_${CLUS_VER}-${RUN8}-"
BASE_seed="/sphenix/lustre01/sphnxpro/production/run2pp/physics/${SEED_VER}/DST_TRKR_SEED/run_${RUN_RNG}/dst/DST_TRKR_SEED_run2pp_${SEED_VER}-${RUN8}-"

OUT_alllist="./joblist_run2pp/dst_run${RUN}.list"
: > "$OUT_alllist"

Nevt=100000
NCALO=$(ls ${BASE_calo}*.root | wc -l)
echo $NCALO

i=0
while [ $i -lt ${NCALO} ]; do  #number of calo dst 
#while [ $i -lt 1 ]; do  #number of calo dst 
  #printf -v I '%05d' "$i" 
  #SUF="$I"
  SUF=$(printf '%05d' "$i")
  echo $SUF

  OUT_calo="${BASE_LISTOUT}/calo/${RUN}/run${RUN}_calo_${SUF}.list"
  OUT_clus="${BASE_LISTOUT}/clus/${RUN}/run${RUN}_clus_${SUF}.list"
  OUT_seed="${BASE_LISTOUT}/seed/${RUN}/run${RUN}_seed_${SUF}.list"   

  OUTDIR_condor="${BASE_ANAOUT}/${RUN}"
  OUT_condor="${OUTDIR_condor}/ana_${RUN}_${SUF}.root"

  echo "outdir $OUTDIR_condor"

  OUT_calo_dir=$(dirname "${OUT_calo}")
  #echo $OUT_calo_dir
  if [ ! -d "$OUT_calo_dir" ]; then
    echo "mkdir -p $OUT_calo_dir"
    echo "mkdir -p $(dirname $OUT_clus)"
    echo "mkdir -p $(dirname $OUT_seed)"
    echo "mkdir -p $OUTDIR_condor"
    mkdir -p "$OUT_calo_dir"
    mkdir -p "$(dirname "$OUT_clus")"
    mkdir -p "$(dirname "$OUT_seed")"
    mkdir -p "$OUTDIR_condor"
  fi

  Sevt=$((Nevt*i))

  printf "%s%s.root\n" "${BASE_calo}" "${SUF}" > "$OUT_calo"
  : > "$OUT_clus"
  : > "$OUT_seed"
  l=0
    while [ $l -lt 10 ]; do
      j=$((10*i+l))

      CLUSFILE=$(printf "%s%05d.root" "${BASE_clus}" "${j}")
      if [  -f $CLUSFILE ]; then
        #echo $CLUSFILE
        echo $CLUSFILE >> "$OUT_clus"
      fi

      SEEDFILE=$(printf "%s%05d.root" "${BASE_seed}" "${j}")
      if [  -f $SEEDFILE ]; then
        #echo $SEEDFILE
        echo $SEEDFILE >> "$OUT_seed"
      fi

      #printf "%s%s.root\n" "${BASE_clus}" "${J}" >> "$OUT_clus"
      #printf "%s%s.root\n" "${BASE_seed}" "${J}" >> "$OUT_seed" 

      l=$((l+1))
    done
  printf "%s %s %s %s %s %s\n" "$Nevt" "$OUT_clus" "$OUT_seed" "$OUT_calo" "$OUT_condor" "$Sevt">> "$OUT_alllist" 

  i=$((i+1))
done

printf "$OUT_alllist "

echo "all done"
