#!/bin/bash
# Chunk a Collect production into corral_m job lists (run this first when a new production lands).
#   Usage: lists/make_lists.bash <collect_dir> <nevt_per_list>
#   e.g.   lists/make_lists.bash /rs/rs_grp_rhi/sPHENIX/run_ecuts_cf 1000000
# Output: lists/<basename of collect_dir>/  list_NN.txt, lists_summary.txt, manifest.txt, segment_entries.txt
# Each list holds contiguous segments of one run, ~nevt_per_list events. Refuses to overwrite an
# existing list set (move or rename the old folder first).
if [ $# -ne 2 ]; then
	echo "usage: $0 <collect_dir> <nevt_per_list>"; exit 1
fi
CDIR=${1%/}
NEVT=$2
HERE=$(cd "$(dirname "$(readlink -f "$0")")" && pwd)
OUT=$HERE/$(basename $CDIR)
if [ ! -d "$CDIR" ]; then echo "no such directory: $CDIR"; exit 1; fi
if ls $OUT/list_*.txt >/dev/null 2>&1; then
	echo "$OUT already has lists -- move or rename it first (nothing was changed)"; exit 1
fi
mkdir -p $OUT
echo "counting events in $CDIR/outputCollect_*.root ..."
root -l -b -q "$HERE/count_entries.C(\"$CDIR\",\"$OUT/segment_entries.txt\")" 2>&1 | grep -E "files=|rror"
python3 $HERE/make_lists.py $OUT $NEVT $CDIR
echo "lists written to $OUT"
