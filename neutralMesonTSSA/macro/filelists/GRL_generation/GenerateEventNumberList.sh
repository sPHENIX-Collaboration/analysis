#!/bin/bash
# This script processes a list of run numbers and retrieves event numbers

# Define the input file containing the list of run numbers
input_file="GoldenEmcalRunList.txt"

# Define the output file where the event numbers will be saved
output_file="EventNumberList_GoldenEmcalRuns.txt"

# Empty the output file before starting (if the file already exists, its contents will be deleted)
> "$output_file"

# Read each runnumber from the input file and process it one by one
while IFS= read -r runnumber; do
    # For each runnumber, execute the psql command to query event numbers from database
    # The -h option specifies the host (sphnxdaqdbreplica)
    # The -t and -A options format the output to remove extra lines and formatting
    # The -F" " option sets the field separator to a space
    # The result of the query is appended to the output file
    echo "Run # $runnumber"
    psql -h sphnxdaqdbreplica daq -t -A -F" " -c "SELECT * FROM event_numbers WHERE runnumber = $runnumber;" >> "$output_file"
done < "$input_file"

# After all runnumbers have been processed, print a message indicating the task is complete
echo "All runnumbers processed. Results saved in $output_file"

