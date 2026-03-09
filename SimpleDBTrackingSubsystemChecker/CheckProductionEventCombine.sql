-- ====== Expected name tables ======

-- TPC: ebdc00_0 to ebdc23_1 (48 total)
SELECT 'ebdc' || LPAD(i::text, 2, '0') || '_' || j AS name
INTO TEMP expected_names_tpc
FROM generate_series(0, 23) AS i,
     generate_series(0, 1) AS j;

-- MVTX: mvtx0 to mvtx5
SELECT 'mvtx' || i || '_0' AS name
INTO TEMP expected_names_mvtx
FROM generate_series(0, 5) AS i;

-- INTT: intt0 to intt7
SELECT 'intt' || i || '_0' AS name
INTO TEMP expected_names_intt
FROM generate_series(0, 7) AS i;

-- ====== Extract relevant DST entries ======

CREATE TEMP TABLE all_streams AS
SELECT 
  ps.run,
  ps.segment,
  ps.status,
  CASE
    WHEN ps.dsttype LIKE 'DST_STREAMING_EVENT_ebdc%' THEN regexp_replace(ps.dsttype, '^DST_STREAMING_EVENT_(ebdc\d{2}_\d)_run3auau$', '\1')
    WHEN ps.dsttype LIKE 'DST_STREAMING_EVENT_mvtx%' THEN regexp_replace(ps.dsttype, '^DST_STREAMING_EVENT_(mvtx\d+_0)_run3auau$', '\1')
    WHEN ps.dsttype LIKE 'DST_STREAMING_EVENT_intt%' THEN regexp_replace(ps.dsttype, '^DST_STREAMING_EVENT_(intt\d+_0)_run3auau$', '\1')
  END AS name
FROM production_status ps
WHERE ps.dsttype ~ '^DST_STREAMING_EVENT_(ebdc|mvtx|intt)'
  AND ps.dsttype LIKE '%run3auau'
  AND ps.run IN (SELECT run FROM transferred_runs);

-- ====== Run summaries ======

-- Generic summary function
CREATE TEMP TABLE run_summary_tpc AS
SELECT
  s.run,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.name IN (SELECT name FROM expected_names_tpc)) AS seg0_total,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_tpc)) AS seg0_finished,
  COUNT(DISTINCT s.name) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_tpc)) AS seg0_unique_names,
  COUNT(*) FILTER (WHERE s.status = 'running' AND s.name IN (SELECT name FROM expected_names_tpc)) AS running_count,
  COUNT(*) FILTER (WHERE s.status != 'finished' AND s.name IN (SELECT name FROM expected_names_tpc)) AS not_finished_count
FROM all_streams s GROUP BY s.run;

CREATE TEMP TABLE run_summary_mvtx AS
SELECT
  s.run,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.name IN (SELECT name FROM expected_names_mvtx)) AS seg0_total,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_mvtx)) AS seg0_finished,
  COUNT(DISTINCT s.name) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_mvtx)) AS seg0_unique_names,
  COUNT(*) FILTER (WHERE s.status = 'running' AND s.name IN (SELECT name FROM expected_names_mvtx)) AS running_count,
  COUNT(*) FILTER (WHERE s.status != 'finished' AND s.name IN (SELECT name FROM expected_names_mvtx)) AS not_finished_count
FROM all_streams s GROUP BY s.run;

CREATE TEMP TABLE run_summary_intt AS
SELECT
  s.run,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.name IN (SELECT name FROM expected_names_intt)) AS seg0_total,
  COUNT(*) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_intt)) AS seg0_finished,
  COUNT(DISTINCT s.name) FILTER (WHERE s.segment = 0 AND s.status = 'finished' AND s.name IN (SELECT name FROM expected_names_intt)) AS seg0_unique_names,
  COUNT(*) FILTER (WHERE s.status = 'running' AND s.name IN (SELECT name FROM expected_names_intt)) AS running_count,
  COUNT(*) FILTER (WHERE s.status != 'finished' AND s.name IN (SELECT name FROM expected_names_intt)) AS not_finished_count
FROM all_streams s GROUP BY s.run;

-- ====== Individual classification ======

-- TPC
SELECT run INTO TEMP tpc_fully_finished FROM run_summary_tpc WHERE seg0_total = 48 AND seg0_finished = 48 AND seg0_unique_names = 48 AND running_count = 0 AND not_finished_count = 0;
SELECT run INTO TEMP tpc_segment0_finished FROM run_summary_tpc WHERE seg0_finished = 48;
SELECT run INTO TEMP tpc_still_running FROM run_summary_tpc WHERE running_count > 0 AND not_finished_count > 0;

-- MVTX
SELECT run INTO TEMP mvtx_fully_finished FROM run_summary_mvtx WHERE seg0_total = 6 AND seg0_finished = 6 AND seg0_unique_names = 6 AND running_count = 0 AND not_finished_count = 0;
SELECT run INTO TEMP mvtx_segment0_finished FROM run_summary_mvtx WHERE seg0_finished = 6;
SELECT run INTO TEMP mvtx_still_running FROM run_summary_mvtx WHERE running_count > 0 AND not_finished_count > 0;

-- INTT
SELECT run INTO TEMP intt_fully_finished FROM run_summary_intt WHERE seg0_total = 8 AND seg0_finished = 8 AND seg0_unique_names = 8 AND running_count = 0 AND not_finished_count = 0;
SELECT run INTO TEMP intt_segment0_finished FROM run_summary_intt WHERE seg0_finished = 8;
SELECT run INTO TEMP intt_still_running FROM run_summary_intt WHERE running_count > 0 AND not_finished_count > 0;

-- ====== Global AND classification ======

SELECT run INTO TEMP fully_finished_global
FROM tpc_fully_finished
INTERSECT
SELECT run FROM mvtx_fully_finished
INTERSECT
SELECT run FROM intt_fully_finished;

SELECT run INTO TEMP segment0_finished_global
FROM tpc_segment0_finished
INTERSECT
SELECT run FROM mvtx_segment0_finished
INTERSECT
SELECT run FROM intt_segment0_finished;

SELECT run INTO TEMP still_running_global
FROM tpc_still_running
UNION
SELECT run FROM mvtx_still_running
UNION
SELECT run FROM intt_still_running;

--SELECT run FROM still_running_global ORDER BY run DESC \g /dev/null
-- TPC
CREATE TEMP VIEW ordered_tpc_fully_finished AS SELECT run FROM tpc_fully_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_tpc_segment0_finished AS SELECT run FROM tpc_segment0_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_tpc_still_running AS SELECT run FROM tpc_still_running ORDER BY run DESC;

-- MVTX
CREATE TEMP VIEW ordered_mvtx_fully_finished AS SELECT run FROM mvtx_fully_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_mvtx_segment0_finished AS SELECT run FROM mvtx_segment0_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_mvtx_still_running AS SELECT run FROM mvtx_still_running ORDER BY run DESC;

-- INTT
CREATE TEMP VIEW ordered_intt_fully_finished AS SELECT run FROM intt_fully_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_intt_segment0_finished AS SELECT run FROM intt_segment0_finished ORDER BY run DESC;
CREATE TEMP VIEW ordered_intt_still_running AS SELECT run FROM intt_still_running ORDER BY run DESC;

-- GLOBAL
CREATE TEMP VIEW ordered_fully_finished_global AS SELECT run FROM fully_finished_global ORDER BY run DESC;
CREATE TEMP VIEW ordered_segment0_finished_global AS SELECT run FROM segment0_finished_global ORDER BY run DESC;
CREATE TEMP VIEW ordered_still_running_global AS SELECT run FROM still_running_global ORDER BY run DESC \g /dev/null
