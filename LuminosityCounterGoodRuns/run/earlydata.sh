#!/bin/bash

UN=$4
source /opt/sphenix/core/bin/sphenix_setup.sh new

source /opt/sphenix/core/bin/setup_local.sh /sphenix/user/jocl/projects/testinstall/

export HOME=/sphenix/u/$UN

if [[ ! -z "$_CONDOR_SCRATCH_DIR" && -d $_CONDOR_SCRATCH_DIR ]]; then
    cd $_CONDOR_SCRATCH_DIR
else
    echo condor scratch NOT set
    exit -1
fi
SUBDIR=${1}
STARTN=$(( $1 * 20 ))
for i in {1..20}; do
    UPLN=$(( $STARTN + $i ))
    mkdir -p trigout
    mkdir -p $SUBDIR
    mkdir -p lists
    mkdir -p /sphenix/user/jocl/projects/trigcount_files/${2}/
    mkdir -p ./dsts/$2/${2}_${UPLN}
    #cp -r $5/run_earlydata.C .
    cp -r $5/run_earlydata_2.C .
    cp -r $5/lists/dst_calofitting-000${2}.list ./lists/${2}.list
    cat ./lists/$2.list
    DSTFILE=`sed -n "${UPLN}"p "./lists/${2}.list"`
    echo $DSTFILE
    if [ -z "${DSTFILE}" ]; then
	exit 0
    fi
    FULLPATH=`psql FileCatalog -t -c "select full_file_path from files where lfn = '${DSTFILE}';"`
    echo $FULLPATH
    cp $FULLPATH ./$DSTFILE
    ls -larth
    mv $DSTFILE ./dsts/$2/${2}_${UPLN}.root
    root -b -q -l 'run_earlydata_2.C('${UPLN}',0,'${2}','${3}')'
    #root -b -q -l 'temp_run.C(1000,"'$DSTFILE'","trigout/tempout_'${2}'_'${UPLN}'_hist.root","trigout/tempout_'${2}'_'${UPLN}'_tree.root","ProdA_2024")'
    cp trigout/* /sphenix/user/jocl/projects/trigcount_files/${2}/
    rm trigout/*
done
