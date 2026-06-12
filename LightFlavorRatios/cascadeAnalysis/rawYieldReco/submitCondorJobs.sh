#!/bin/bash

submissionFile=myCondor.job

listDir=fileLists

#runs="79509 79510 79511 79512 79513 79514 79515 79516 79528 79529 79530 79570 79571 79572 79600"
#runs="79507 79524 79525 79526 79563 79565 79567 79568 79593 79594 79595 79596 79597 79598 79599 79614 79617 79627 79652 79653 79656 79660 79707 79708 79709 79711 79712"
#runs="79507 79524 79525 79526 79563 79565 79567"
#runs="79568 79593 79594 79595 79596 79597 79598 79599"
#runs="79614 79617 79627 79652 79653 79656 79660 79707 79708 79709 79711 79712"
runs="795079 79510 79511 79512 79513 79514 79515 79516 79528 79529 79530 79570 79571 79572 79600"
# DST_TRKR_TRACKS_run3pp_ana538_2025p011_v001-00079509-0029.list

for run in $runs
do
  currentOptionsFile=$(grep 'Queue' ${submissionFile} | awk '{print $NF}' | awk -F "/" '{print $NF}')
  newOptionsFile=masterList_run${run}.list
  
  if [ -f ${listDir}/${newOptionsFile} ]; then
    echo "Current options file: ${currentOptionsFile}"
    echo "New options file:  ${newOptionsFile}s"
    sed -i -e "s/${currentOptionsFile}/${newOptionsFile}/g" ${submissionFile}
    condor_submit ${submissionFile}
  else
    echo "File ${newOptionsFile} is missing"
  fi

done
