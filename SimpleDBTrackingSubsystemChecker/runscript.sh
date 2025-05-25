#!/bin/bash

if [ $# -eq 1 ]; then
  SQL_FILE=$1
  psql -h sphnxdaqdbreplica -d daq -f "$SQL_FILE"

elif [ $# -eq 2 ]; then
  SQL_TEMPLATE=$1
  RUNTYPE=$2
  TEMP_SQL="temp_query.sql"

  sed "s/@RUNTYPE@/$RUNTYPE/g" "$SQL_TEMPLATE" > "$TEMP_SQL"
  psql -h sphnxdaqdbreplica -d daq -f "$TEMP_SQL"
  rm "$TEMP_SQL"

else
  echo "Usage:"
  echo "  $0 <sql_file>              # for all runs"
  echo "  $0 <sql_template> <run>    # for specific run"
  exit 1
fi

