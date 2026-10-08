#!/bin/bash
# Chunk a Collect production into corral job lists (run this first when a new production lands).
#   Usage: lists/make_lists.bash <collect_dir> <nevt_per_list> [dataset] [runs]
#   e.g.   lists/make_lists.bash /rs/rs_grp_rhi/sPHENIX/ana532 1000000 ana532
#          lists/make_lists.bash /rs/rs_grp_rhi/sPHENIX/ana573 10000000 ana573_8158x 81584,81585,81586
#   [runs] = comma-separated run numbers to keep (default: every run in <collect_dir>). One production split by
#   run group: ana573_795xx (79514-79516), ana573_8156x (81566), ana573_8158x (81584-81586); plain ana573 = ALL runs.
#   If lists/<dataset>/segment_entries.txt already exists (and no lists), it is used as is -- no recount; e.g. for
#   ana573: cat the run-group segment_entries.txt files into lists/ana573/segment_entries.txt first.
# Output: lists/<dataset>/  list_NN.txt, list_all.txt (all lists, in list order: the single full-statistics
#         job), lists_summary.txt, manifest.txt, segment_entries.txt.  <dataset> defaults to the basename
#         of <collect_dir>; it is the name used everywhere (root|pdf|log/<dataset>/, corral -d <dataset>).
# Each list holds contiguous segments of one run, ~nevt_per_list events. Refuses to overwrite an
# existing list set (move or rename the old folder first).
if [ $# -lt 2 ] || [ $# -gt 4 ]; then
	echo "usage: $0 <collect_dir> <nevt_per_list> [dataset] [runs]"; exit 1
fi
CDIR=${1%/}
NEVT=$2
DS=${3:-$(basename $CDIR)}
RUNS=$4
HERE=$(cd "$(dirname "$(readlink -f "$0")")" && pwd)
OUT=$HERE/$DS
if [ ! -d "$CDIR" ]; then echo "no such directory: $CDIR"; exit 1; fi
if ls $OUT/list_*.txt >/dev/null 2>&1; then
	echo "$OUT already has lists -- move or rename it first (nothing was changed)"; exit 1
fi
mkdir -p $OUT
if [ -s $OUT/segment_entries.txt ]; then
	echo "using the existing $OUT/segment_entries.txt (no recount)"
else
	echo "counting events in $CDIR/outputCollect_*.root ${RUNS:+(runs $RUNS)} ..."
	root -l -b -q "$HERE/count_entries.C(\"$CDIR\",\"$OUT/segment_entries.txt\",\"$RUNS\")" 2>&1 | grep -E "files=|rror"
fi
python3 $HERE/make_lists.py $OUT $NEVT $CDIR
NL=$(awk '$1=="nlists"{print $2}' $OUT/manifest.txt)
for ((i=0; i<NL; i++)); do cat $OUT/$(printf "list_%02d.txt" $i); done > $OUT/list_all.txt
echo "lists written to $OUT"
