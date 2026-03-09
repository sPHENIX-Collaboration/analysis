-- Step 1: Create a table of transferred runs
CREATE TEMP TABLE transferred_runs (run INT);
\COPY transferred_runs FROM 'transferred_runs.txt'

-- Step 2: Extract relevant DST_TRKR_CLUSTER segments
CREATE TEMP TABLE trkr_cluster_segments AS
SELECT
  ps.run,
  ps.segment,
  ps.status
FROM production_status ps
WHERE ps.dsttype = 'DST_TRKR_CLUSTER_run3auau'
  AND ps.run IN (SELECT run FROM transferred_runs);

-- Step 3: Get per-run segment counts and percentages
CREATE TEMP TABLE trkr_cluster_summary AS
SELECT
  r.run,
  COUNT(*) AS total_segments,
  COUNT(*) FILTER (WHERE ps.status = 'finished') * 100.0 / COUNT(*) AS percent_finished,
  COUNT(*) FILTER (WHERE ps.status = 'running')  * 100.0 / COUNT(*) AS percent_running,
  COUNT(*) FILTER (WHERE ps.status = 'failed')   * 100.0 / COUNT(*) AS percent_failed
FROM transferred_runs r
JOIN trkr_cluster_segments ps ON ps.run = r.run

GROUP BY r.run
ORDER BY r.run DESC;

-- Step 4: Handle runs with no segments (fill 0%)
-- This is automatically handled above using LEFT JOIN, NULLs become 0 in COUNT.

-- Step 5: Output the result
\COPY (SELECT run, COALESCE(ROUND(percent_finished, 1), 0.0) AS finished_percent, COALESCE(ROUND(percent_running, 1), 0.0) AS running_percent, COALESCE(ROUND(percent_failed, 1), 0.0) AS failed_percent FROM trkr_cluster_summary ORDER BY run DESC) TO 'trkr_cluster_summary.txt' WITH DELIMITER ' ';

