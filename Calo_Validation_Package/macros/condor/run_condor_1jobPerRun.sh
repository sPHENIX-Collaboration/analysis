#!/usr/bin/bash

export TargetDir="$PWD"/condorout

if [ -d ${TargetDir} ]; then
  if [ -d ${TargetDir}/OutDir0 ]; then
    rm -rf ${TargetDir}/OutDir*
  fi
else
  mkdir ${TargetDir}
fi

i=0
while read dir; do 
  li=$(printf "%04d" $i)
  
  mkdir ${TargetDir}/OutDir$i
  export WorkDir="${TargetDir}/OutDir$i"
  echo "WorkDir:" ${WorkDir}
  
  pushd ${WorkDir}

  for filepath in /sphenix/lustre01/sphnxpro/commissioning/DST_ana.387_2023p003/DST_CALOR-000"$dir"-*.root
  do
  cat >>input.txt<< EOF
  $file
  EOF
  done

    
  
  cp -v "$PWD"/../../CondorRun.sh CondorRunJob$li.sh
  
  chmod +x CondorRunJob$li.sh
  
  
  cat >>ff.sub<< EOF
+JobFlavour                   = "workday"
transfer_input_files          = ${WorkDir}/CondorRunJob$li.sh, ${WorkDir}/input.txt
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
  
    i=$((i+1))
done < runList.txt # redirect the input of the
