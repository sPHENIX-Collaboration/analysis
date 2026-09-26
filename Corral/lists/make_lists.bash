#!/bin/bash
# Chunk a Collect production into corral_m job lists (run this first when a new production lands).
#   Usage: lists/make_lists.bash <collect_dir> <nevt_per_list> [dataset]
#   e.g.   lists/make_lists.bash /rs/rs_grp_rhi/sPHENIX/ana532 1000000 ana532
# Output: lists/<dataset>/  list_NN.txt, list_all.txt (all lists, in list order: the single full-statistics
#         job), lists_summary.txt, manifest.txt, segment_entries.txt.  <dataset> defaults to the basename
#         of <collect_dir>; it is the name used everywhere (root|pdf|log/<dataset>/, corral_m -d <dataset>).
# Each list holds contiguous segments of one run, ~nevt_per_list events. Refuses to overwrite an
# existing list set (move or rename the old folder first).
if [ $# -lt 2 ] || [ $# -gt 3 ]; then
	echo "usage: $0 <collect_dir> <nevt_per_list> [dataset]"; exit 1
fi
CDIR=${1%/}
NEVT=$2
DS=${3:-$(basename $CDIR)}
HERE=$(cd "$(dirname "$(readlink -f "$0")")" && pwd)
OUT=$HERE/$DS
if [ ! -d "$CDIR" ]; then echo "no such directory: $CDIR"; exit 1; fi
if ls $OUT/list_*.txt >/dev/null 2>&1; then
	echo "$OUT already has lists -- move or rename it first (nothing was changed)"; exit 1
fi
mkdir -p $OUT
echo "counting events in $CDIR/outputCollect_*.root ..."
root -l -b -q "$HERE/count_entries.C(\"$CDIR\",\"$OUT/segment_entries.txt\")" 2>&1 | grep -E "files=|rror"
python3 $HERE/make_lists.py $OUT $NEVT $CDIR
NL=$(awk '$1=="nlists"{print $2}' $OUT/manifest.txt)
for ((i=0; i<NL; i++)); do cat $OUT/$(printf "list_%02d.txt" $i); done > $OUT/list_all.txt
echo "lists written to $OUT"
