#!/usr/bin/env bash

dir=${1}

while read run; do
    a=`ls $dir/$run/output | wc -l`
    b=`wc -l $dir/$run/dst*.list | cut -d " " -f1`

    if [[ $a != $b ]]; then
        echo "Run: $run, Processed: $a, Total: $b"
        ls $dir/$run/output | cut -d "-" -f2 | cut -d "." -f1 > $dir/$run/a.txt

        # initial clean up
        rm -f $dir/$run/done.txt

        # get a list of DSTs that have an output
        while read line; do
            line=$((line+1))
            sed -n "${line}p" $dir/$run/dst*.list >> $dir/$run/done.txt
        done < $dir/$run/a.txt

        # find the DSTs without an output
        comm -13 $dir/$run/done.txt $dir/$run/dst_calofitting_run2pp-000$run.list > $dir/$run/missing.txt

        # print list of DSTs without an output
        cat $dir/$run/missing.txt

        # clean up
        rm $dir/$run/a.txt
        rm $dir/$run/done.txt
    fi
done <$dir/runs.list;
