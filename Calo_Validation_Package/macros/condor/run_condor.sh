#!/usr/bin/bash

export TargetDir="$PWD"/condorout

if [ -d ${TargetDir}/OutDir0 ]; then
  rm -rf ${TargetDir}/OutDir*
else
  mkdir ${TargetDir}
fi

i=0
while read dir; do # read each line of run list
  for filepath in /sphenix/lustre01/sphnxpro/commissioning/DST_ana.386_2023p003/DST_CALOR_ana.386_2023p003-000"$dir"-*.root; do
    if [ ! -f "$filepath" ]; then
      continue
    fi
    
    li=$(printf "%04d" $i)
  
    mkdir ${TargetDir}/OutDir$i
    export WorkDir="${TargetDir}/OutDir$i"
    echo "WorkDir:" ${WorkDir}
  
    pushd ${WorkDir}
  
    cp -v "$PWD"/../../CondorRun.sh CondorRunJob$li.sh
  
    chmod +x CondorRunJob$li.sh
  
    echo "$filepath" > filedir.txt
  
    cat >>ff.sub<< EOF
+JobFlavour                   = "workday"
transfer_input_files          = ${WorkDir}/CondorRunJob$li.sh, ${WorkDir}/filedir.txt
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
  done
done < runList.txt # redirect the input of the
