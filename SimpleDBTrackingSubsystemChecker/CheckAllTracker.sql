WITH good_runs AS (

  SELECT runnumber
  FROM filelist
  WHERE (
    (filename LIKE '%@RUNTYPE@/%ebdc%' AND filename LIKE '%.evt') OR
    (filename LIKE '%@RUNTYPE@/%mvtx%' AND filename LIKE '%.evt') OR
    (filename LIKE '%@RUNTYPE@/%intt%' AND filename LIKE '%.evt')
  )

  GROUP BY runnumber
  HAVING
    COUNT(DISTINCT CASE
      WHEN filename LIKE '%TPC_ebdc%' AND filename LIKE '%-0000.evt'
      THEN SUBSTRING(filename FROM 'TPC_ebdc(\d{2}_\d)')
      END) = 48

  AND

    COUNT(DISTINCT CASE
      WHEN filename LIKE '%mvtx%' AND filename LIKE '%-0000.evt'
      THEN SUBSTRING(filename FROM 'mvtx(\d+)')
      END) = 6

  AND

    COUNT(DISTINCT CASE
      WHEN filename LIKE '%intt%' AND filename LIKE '%-0000.evt'
      THEN SUBSTRING(filename FROM 'intt(\d+)')
      END) = 8


  @TRANSFER_CONDITION@
)

SELECT
  r.runnumber,
  r.runtype,
  r.brtimestamp,
  r.ertimestamp,
  r.eventsinrun AS total_events,
  ROUND(EXTRACT(EPOCH FROM r.ertimestamp - r.brtimestamp)) AS duration_sec,
  ROUND(r.eventsinrun::numeric / NULLIF(EXTRACT(EPOCH FROM r.ertimestamp - r.brtimestamp), 0), 2) AS event_rate
FROM good_runs gr
JOIN run r ON r.runnumber = gr.runnumber
WHERE r.runtype = '@RUNTYPE@'
  AND r.eventsinrun > 0
  AND r.brtimestamp IS NOT NULL
  AND r.ertimestamp IS NOT NULL
  AND EXTRACT(EPOCH FROM r.ertimestamp - r.brtimestamp) >= 300
ORDER BY r.runnumber DESC;

