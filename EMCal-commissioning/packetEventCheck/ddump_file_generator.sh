#!/bin/bash

if [ -z "$1" ]; then
    echo "Please provide a filename as a parameter."
    exit 1
fi

filename=$1
filename2=$2
percentile=$3

input_filename="$filename"

# Check if the file exists
if [ ! -f "$input_filename" ]; then
    echo "File '$input_filename' does not exist."
    exit 1
fi

output_filename="$filename2"
touch "$output_filename"


# Run dpipe and capture its output
length=$(dpipe -s f -d n -i "$input_filename" | wc -l)

# Check if the previous command had any output
if [ -n "$length" ]; then
    # Calculate % of the output and round it to the nearest integer
    percent=$(echo "$length * $percentile" | bc)	
    largest_integer=$(echo "scale=0; $percent/1" | bc)
      
for i in $(seq 6001 6128); do                                                            
echo "$(ddump -p $i -e $largest_integer $input_filename | grep "FEM Evt")" >> "$output_filename"  
echo $i
done

    echo "Event For checking: $percent"
    echo "Largest int: $largest_integer"
else
    echo "command had no output."
fi




