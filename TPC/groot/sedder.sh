#! /bin/sh

for FILE in `ls -1 *.h`
do
    sed s/chimera/groot/g $FILE > outputs/$FILE
done

for FILE in `ls -1 *.C`
do
    sed s/chimera/groot/g $FILE > outputs/$FILE
done
