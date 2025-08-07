#!/bin/bash

# Define files
ALL_RUNS_FILE="all_runs.txt"
TRANSFERRED_FILE="transferred_runs.txt"
CLUSTER_FILE="trkr_cluster_summary.txt"

# All 12 classification files (assumed already exist)
FILES=(
  "tpc_fully_finished.txt"
  "tpc_segment0_finished.txt"
  "tpc_still_running.txt"
  "mvtx_fully_finished.txt"
  "mvtx_segment0_finished.txt"
  "mvtx_still_running.txt"
  "intt_fully_finished.txt"
  "intt_segment0_finished.txt"
  "intt_still_running.txt"
  "fully_finished_global.txt"
  "segment0_finished_global.txt"
  "still_running_global.txt"
)

# Summary
echo "==================== Production Summary ===================="
printf "%-30s %5s\n" "Category" "Count"
echo "------------------------------------------------------------"

# Total available
total_all=$(wc -l < "$ALL_RUNS_FILE")
printf "%-30s %5d\n" "Total available runs" "$total_all"

# Transferred runs
total_transferred=$(wc -l < "$TRANSFERRED_FILE")
printf "%-30s %5d\n" "Fully transferred runs" "$total_transferred"

echo " "
echo "------------------Event combined DSTs-----------------------"
# Now for each category
for file in "${FILES[@]}"; do
  label=$(basename "$file" .txt | sed 's/_/ /g' | sed 's/\b\(.\)/\u\1/g')  # format nicely
  count=$(wc -l < "$file" 2>/dev/null || echo 0)
  printf "%-30s %5d\n" "$label" "$count"
done

echo " "
echo "--------------------Cluster DSTs-------------------------"
if [[ -f "$CLUSTER_FILE" ]]; then
  runs_with_finished=$(awk '$2 > 0 { count++ } END { print count+0 }' "$CLUSTER_FILE")
  runs_with_90_failed=$(awk '$4 > 90 { count++ } END { print count+0 }' "$CLUSTER_FILE")
  runs_with_50_finished=$(awk '$2 > 50 { count++ } END { print count+0 }' "$CLUSTER_FILE")

  printf "%-30s %5d\n" "Number of runs with at least one segment finished" "$runs_with_finished"
  printf "%-30s %5d\n" "Number of runs with >90% failed" "$runs_with_90_failed"
  printf "%-30s %5d\n" "Number of runs with >50% finished" "$runs_with_50_finished"
else
  printf "%-30s %5s\n" "TRKR_CLUSTER stats" "File missing"
fi

echo "============================================================"

