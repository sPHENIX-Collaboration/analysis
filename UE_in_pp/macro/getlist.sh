#!/bin/bash

DST_CALO_LIST="run22_mb_dst_calo_cluster.list"
DST_TRUTH_LIST="run22_mb_dst_truthinfo.list"
DST_GLOBAL_LIST="run22_mb_dst_global.list"
DST_TRUTH_JET_LIST="run22_mb_dst_truth_jet.list"
DST_TRACKS_LIST="run22_mb_dst_tracks.list"

# Run SQL queries and redirect results to files
psql -h sphnxdbmaster.sdcc.bnl.gov -d FileCatalog -t -A -q -F" " --command "SELECT filename FROM datasets WHERE runnumber = 22 AND dsttype = 'DST_CALO_CLUSTER' AND filename LIKE '%pythia8_Detroit-0000000022%' ORDER BY segment;" > "$DST_CALO_LIST"
psql -h sphnxdbmaster.sdcc.bnl.gov -d FileCatalog -t -A -q -F" " --command "SELECT filename FROM datasets WHERE runnumber = 22 AND dsttype = 'DST_TRUTH' AND filename LIKE '%pythia8_Detroit-0000000022%' ORDER BY segment;" > "$DST_TRUTH_LIST"
psql -h sphnxdbmaster.sdcc.bnl.gov -d FileCatalog -t -A -q -F" " --command "SELECT filename FROM datasets WHERE runnumber = 22 AND dsttype = 'DST_GLOBAL' AND filename LIKE '%pythia8_Detroit-0000000022%' ORDER BY segment;" > "$DST_GLOBAL_LIST"
psql -h sphnxdbmaster.sdcc.bnl.gov -d FileCatalog -t -A -q -F" " --command "SELECT filename FROM datasets WHERE runnumber = 22 AND dsttype = 'DST_TRUTH_JET' AND filename LIKE '%pythia8_Detroit-0000000022%' ORDER BY segment;" > "$DST_TRUTH_JET_LIST"
psql -h sphnxdbmaster.sdcc.bnl.gov -d FileCatalog -t -A -q -F" " --command "SELECT filename FROM datasets WHERE runnumber = 22 AND dsttype = 'DST_TRACKS' AND filename LIKE '%pythia8_Detroit-0000000022%' ORDER BY segment;" > "$DST_TRACKS_LIST"