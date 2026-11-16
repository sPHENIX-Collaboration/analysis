#!/bin/bash

submissionFile=myCondor.job

listDir=fileLists

#runs="82420"
#runs="79514 79515 79516"
#runs="53877 53534 53631 53687 53716 53744 53756 53876"
runs="79516"

for run in $runs
do
  nFiles=$(($(ls -l ${listDir}/file_run${run}_*.list | wc -l) - 1))

  for i in  $(seq 0 $nFiles) 
  do
  
    i=$(printf "%02d" $i)
  
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
