#! /bin/sh
LINE=`awk "NR==$1" CondorList.txt`
echo $LINE

#remove all space characters from the argument string to root
FIXEDLINE=`echo $LINE | sed "s/ //g"`
root -b -q groot100.C\($FIXEDLINE\);
