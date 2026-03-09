#!/bin/bash

SQLFILE="CheckProductionEventCombine.sql"
RUNLIST="transferred_runs.txt"

if [[ ! -f $SQLFILE ]] || [[ ! -f $RUNLIST ]]; then
  echo "Missing SQL file or transferred_runs.txt"
  exit 1
fi

psql -h sphnxproddbmaster.sdcc.bnl.gov -d Production -U argouser <<EOF
\echo Creating transferred_runs table
CREATE TEMP TABLE transferred_runs (run int);
\COPY transferred_runs FROM '$RUNLIST'

\echo Running classification SQL
\i $SQLFILE

-- Output 12 result files
\echo Writing TPC results...

\COPY (SELECT run FROM tpc_fully_finished ORDER BY run DESC) TO 'tpc_fully_finished.txt'
\COPY (SELECT run FROM tpc_segment0_finished ORDER BY run DESC) TO 'tpc_segment0_finished.txt'
\COPY (SELECT run FROM tpc_still_running ORDER BY run DESC) TO 'tpc_still_running.txt'

\echo Writing MVTX results...
\COPY (SELECT run FROM mvtx_fully_finished ORDER BY run DESC) TO 'mvtx_fully_finished.txt'
\COPY (SELECT run FROM mvtx_segment0_finished ORDER BY run DESC) TO 'mvtx_segment0_finished.txt'
\COPY (SELECT run FROM mvtx_still_running ORDER BY run DESC) TO 'mvtx_still_running.txt'

\echo Writing INTT results...
\COPY (SELECT run FROM intt_fully_finished ORDER BY run DESC) TO 'intt_fully_finished.txt'
\COPY (SELECT run FROM intt_segment0_finished ORDER BY run DESC) TO 'intt_segment0_finished.txt'
\COPY (SELECT run FROM intt_still_running ORDER BY run DESC) TO 'intt_still_running.txt'

\echo Writing GLOBAL results...
\COPY (SELECT run FROM fully_finished_global ORDER BY run DESC) TO 'fully_finished_global.txt'
\COPY (SELECT run FROM segment0_finished_global ORDER BY run DESC) TO 'segment0_finished_global.txt'
\COPY (SELECT run FROM still_running_global ORDER BY run DESC) TO 'still_running_global.txt'

EOF
