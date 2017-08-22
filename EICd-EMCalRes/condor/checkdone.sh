#!/bin/sh

for f in log/*; do
	grep $f -q -e "terminated"
	if [ $? != 0 ]; then
		printf "${f#*/} has not finished!\n"
	fi
done
