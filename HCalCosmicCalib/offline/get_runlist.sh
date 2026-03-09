#!/bin/bash

# Usage check
if [ $# -ne 2 ]; then
    echo "Usage: $0 MIN_RUN MAX_RUN"
    exit 1
fi

min_run=$1
max_run=$2

# Validate that both are integers
if ! [[ "$min_run" =~ ^[0-9]+$ && "$max_run" =~ ^[0-9]+$ ]]; then
    echo "Error: MIN_RUN and MAX_RUN must be integers."
    exit 1
fi

query="SELECT runnumber
       FROM run 
       WHERE runtype = 'cosmics'
       AND runnumber BETWEEN $min_run AND $max_run
       AND brtimestamp IS NOT NULL
       AND ertimestamp  IS NOT NULL
       AND ertimestamp > brtimestamp
       AND (ertimestamp - brtimestamp) > INTERVAL '10 minutes'
       ORDER BY runnumber;"

result=$(psql -h sphnxdaqdbreplica -d daq -t -A -F"    " -c "$query")

echo "$result" > runlist.txt

echo "Done. Run list has been saved to runlist.txt"