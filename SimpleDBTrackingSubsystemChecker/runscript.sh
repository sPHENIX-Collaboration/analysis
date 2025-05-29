#!/bin/bash

if [ $# -eq 1 ]; then
  SQL_FILE=$1
  psql -h sphnxdaqdbreplica -d daq -f "$SQL_FILE"

elif [ $# -eq 2 ]; then
  SQL_TEMPLATE=$1
  RUNTYPE=$2
  TEMP_SQL_1="temp_with_transfer.sql"
  TEMP_SQL_2="temp_without_transfer.sql"

  sed -e "s/@RUNTYPE@/$RUNTYPE/g" \
    -e "s|@TRANSFER_CONDITION@|AND COUNT(*) = SUM(CASE WHEN transferred_to_sdcc THEN 1 ELSE 0 END)|" \
    "$SQL_TEMPLATE" > "$TEMP_SQL_1"

  sed -e "s/@RUNTYPE@/$RUNTYPE/g" \
    -e "s|@TRANSFER_CONDITION@||" \
    "$SQL_TEMPLATE" > "$TEMP_SQL_2"

  echo "Running with transferred_to_sdcc condition..."
  psql -h sphnxdaqdbreplica -d daq -f "$TEMP_SQL_1" | tee output_with_transfer.log
  grep -E '^\s*[0-9]+' output_with_transfer.log | awk '{print $1}' > transferred_runs.txt

  echo "Running without transferred_to_sdcc condition..."
  psql -h sphnxdaqdbreplica -d daq -f "$TEMP_SQL_2" | tee output_all_runs.log
  grep -E '^\s*[0-9]+' output_all_runs.log | awk '{print $1}' > all_runs.txt

  rm $TEMP_SQL_1
  rm $TEMP_SQL_2

else
  echo "Usage:"
  echo "  $0 <sql_file>              # for all runs"
  echo "  $0 <sql_template> <run>    # for specific run"
  exit 1
fi

