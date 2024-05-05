#!/usr/bin/bash

export TargetDir="$PWD"/condorout


if [ -d ${TargetDir} ]; then
  if [ -n "$(ls -A ${TargetDir}/OutDir*)" ]; then
    rm -rf ${TargetDir}/OutDir*
  fi
else
  mkdir ${TargetDir}
fi

  
  export listfile="dst_calo_cluster.list"
  export listfile2="g4hits.list"
  rm $listfile
  rm $listfile2

  CreateFileList.pl -type 14  -run 13 -particle pi0 -pmin 200 -pmax 10000 DST_CALO_CLUSTER G4Hits
  
  #number of jobs
  j=500

  tot_files=$( cat ${listfile} | wc -l )
  echo "total files: $tot_files"
  rem=$(( $tot_files%$j ))
  files_per_job=$(( $tot_files/$j ))
  njob=$j
  if [ $rem -ne 0 ]; then
    files_per_job=$(( $files_per_job+1 ))
  fi
  rem2=$(( $tot_files%$files_per_job ))
  njob=$(( $tot_files/$files_per_job ))
  if [ $rem2 -ne 0 ]; then
    njob=$(( ($tot_files/$files_per_job)+1 ))
  fi
  echo "files per job: $files_per_job"
  echo "njob: $njob"


  for((q=0;q<$njob;q++));
  do

    mkdir ${TargetDir}/OutDir$q
    export WorkDir="${TargetDir}/OutDir$q"
    echo "WorkDir:" ${WorkDir}

    start_file=$(( $q*$files_per_job+1 ))
    end_file=$(( $start_file+$files_per_job-1 ))
    echo "start file: $start_file   end file: $end_file"

    sed -n $start_file\,${end_file}p ${listfile} > tmp.txt
    sed -n $start_file\,${end_file}p ${listfile2} > tmp2.txt
    mv tmp.txt ${WorkDir}/inputdata.txt
    mv tmp2.txt ${WorkDir}/inputdatahits.txt
    
    pushd ${WorkDir}

    
    cp -v "$PWD"/../../CondorRun.sh CondorRunJob$li.sh
    cp "$PWD"/../../../Fun4All_EMCal_sp.C .

    chmod +x CondorRunJob$li.sh
        
    
    cat >>ff.sub<< EOF
+JobFlavour                   = "workday"
transfer_input_files          = ${WorkDir}/CondorRunJob$li.sh, ${WorkDir}/inputdata.txt,${WorkDir}/Fun4All_EMCal_sp.C,${WorkDir}/inputdatahits.txt
Executable                    = CondorRunJob$li.sh
request_memory                = 10GB
Universe                      = vanilla
Notification                  = Never
GetEnv                        = True
Priority                      = +12
Output                        = condor.out
Error                         = condor.err
Log                           = /tmp/condor$li.log
Notify_user                   = bs3402@columbia.edu

Queue
EOF

    condor_submit ff.sub
    popd
  
done

