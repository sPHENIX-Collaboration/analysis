#!/bin/sh

#regenerate the table file,
#which is used by all the other table_* generation scripts.

#basically, table pairs an encoded stump like "100110160400"
#with its parameter tuple "e+ 1.6 4.0".
#that way, no other script needs to know how to do the mapping.

#also, table is generated with all possible parameter combinations.
#so, no other script needs to know
#what the basic initial lists were.. just all the combos.

rm table

flavors="e- e+"
pseudorapidities="3.2 2.4 1.6 0.8 0.0 -0.4 -1.6 -2.4 -3.2"
momenta="25. 21.2 8.2 4.0 2.4 1.6 1.1 .83 .64"

for flavor in $flavors; do
	for pseudorapidity in $pseudorapidities; do
		for momentum in $momenta; do
			if [ "$flavor" = "e-" ]; then
				printf "00011"
			elif [ "$flavor" = "e+" ]; then
				printf "10011"
			fi
			
			if [ "${pseudorapidity:0:1}" = "-" ]; then
				printf "1"
			else
				printf "0"
			fi
			printf "%s" $pseudorapidity | tr -d '.' | tr -d '-'
			
			printf "%05.2f" $momentum | tr -d '.'
			
			printf " %s %s %s\n" $flavor $pseudorapidity $momentum
		done
	done
done > table
