#!/bin/bash

filename="$1"  # Path to the input text file

# Check if the file exists
if [ ! -f "$filename" ]; then
    echo "File $filename does not exist."
    exit 1
fi

# Array to store the first numbers from each column
numbers=()

# Read lines from the file
while IFS= read -r line; do
    # Check if the line starts with "FEM Evt nr:"
    if [[ $line == "FEM Evt nr:"* ]]; then
        # Extract the first number from each column using awk
        first_number=$(echo "$line" | awk '{print $5}')
      #echo $first_number 
       # Check if the extracted number is not empty
        if [[ ! -z "$first_number" ]]; then
            # Add the number to the array
            numbers+=("$first_number")
	else
	   numbers+=("-1")
		#echo $first_number
	   # echo "added"	
        fi
    fi
done < "$filename"
#echo $numbers
declare -A count

for num in "${numbers[@]}"; do
    ((count[$num]++))		
    #echo $num	
done

mode=""
maxCount=0


for num in "${!count[@]}"; do
       echo $num
    if ((count[$num] > maxCount)); then
        maxCount=${count[$num]}
        mode=$num
    fi
done

notEqualCount=0

# Iterate over the numbers again to count the non-mode numbers
for num in "${numbers[@]}"; do
    if [[ $num -ne $mode ]]; then
        ((notEqualCount++))
    fi
done
total=${#numbers[@]}
percentage=$(bc <<< "scale=4; (($total - $notEqualCount) / $total)*100")



# Calculate the mode of the numbers array using awk
#mode=$(printf "%s\n" "${numbers[@]}" | awk 'NR==FNR{a[$1]++;next}a[$1] > max{max=a[$1];mode=$1}END{print mode}')

echo "Mode: $mode"
echo "Percentage: $percentage"

notModeIndices=()

for i in "${!numbers[@]}"; do
    if [[ ${numbers[i]} -ne $mode ]]; then
        notModeIndices+=("$i")
    fi
done

echo "Indices not equal to mode: ${notModeIndices[@]}"

echo "Mode: $mode" >> "$filename"
echo "Percentage: $percentage" >> "$filename"
echo "Indices not equal to mode: ${notModeIndices[@]}" >> "$filename"
