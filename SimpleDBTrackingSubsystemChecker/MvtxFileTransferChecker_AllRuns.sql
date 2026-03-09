SELECT runnumber
FROM filelist
WHERE filename LIKE '%dryrun/dryrun_mvtx%'
  AND filename LIKE '%.evt'
GROUP BY runnumber
HAVING
  COUNT(DISTINCT CASE WHEN filename LIKE '%0000.evt' THEN SUBSTRING(filename FROM 'dryrun_mvtx(\d+)') END) = 6
  AND
  COUNT(*) = SUM(CASE WHEN transferred_to_sdcc THEN 1 ELSE 0 END)
ORDER BY runnumber;

SELECT COUNT(*) AS total_good_runs FROM (
  SELECT runnumber
  FROM filelist
  WHERE filename LIKE '%dryrun/dryrun_mvtx%'
    AND filename LIKE '%.evt'
  GROUP BY runnumber
  HAVING
    COUNT(DISTINCT CASE WHEN filename LIKE '%0000.evt' THEN SUBSTRING(filename FROM 'dryrun_mvtx(\d+)') END) = 6
    AND COUNT(*) = SUM(CASE WHEN transferred_to_sdcc THEN 1 ELSE 0 END)
) AS good_runs;

