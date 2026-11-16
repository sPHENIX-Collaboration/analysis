#!/bin/bash

submissionFile=submit.job

listDir=fileLists

#QM runs
#runs="53877 53534 53631 53687 53716 53738 53739 53741 53742 53743 53744 53756 53783 53871 53876 53879" 

#IS/Spin runs  (already have 53877 for Kshort (include back in when starting to look at other resonances)
## submitted up to 53590
# first job (STREAMING ana478) 53018 53046 53079 53081 53194 53195 53196 53197 53494 53513 53517 53531 53532 53571 53578 53581 53586 53587 53590
# next jobs: (STREAMING ana479)  53631 53632 53652 53687 53716 53738 53739 53741 53742 53743 53744 53756 53783 53871 53876 53879
runs="53631 53632 53652 53687 53716 53738 53739 53741 53742 53743 53744 53756 53783 53871 53876 53879"

for run in $runs
do
  nFiles=$(($(ls -l ${listDir}/file_run${run}_*.list | wc -l) - 1))

  for i in  $(seq 0 $nFiles) 
  do
  
    i=$(printf "%03d" $i)
  
    currentOptionsFile=$(grep 'Queue' ${submissionFile} | awk '{print $NF}' | awk -F "/" '{print $NF}')
    newOptionsFile=file_run${run}_$i.list
  
    if [ -f ${listDir}/${newOptionsFile} ]; then
      echo "Current options file: ${currentOptionsFile}"
      echo "New options file:  ${newOptionsFile}s"
      sed -i -e "s/${currentOptionsFile}/${newOptionsFile}/g" ${submissionFile}
      condor_submit ${submissionFile}
    else
      echo "File ${newOptionsFile} is missing"
    fi
  
  done
done