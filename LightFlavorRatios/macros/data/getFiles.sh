#!/bin/bash

run=$1

dirStart=${run:0:3}
dirEnd=$(($dirStart + 1))

runSpecies=run3oo
buildTag=ana534_2025p010_v001
dstType=DST_TRKR_SEED

directory=DIRECTORY
if [[ "${runSpecies}" == "run3auau" || "${runSpecies}" == "run3pp"  || "${runSpecies}" == "run3oo" ]]; then
  directory=/sphenix/lustre01/sphnxpro/production/${runSpecies}/physics/${buildTag}/${dstType}/run_000${dirStart}00_000${dirEnd}00/
fi
if [[ "${runSpecies}" == "run2pp" ]]; then
  directory=/sphenix/lustre01/sphnxpro/production/${runSpecies}/physics/${buildTag}/${dstType}/run_000${dirStart}00_000${dirEnd}00/dst/
fi

fileHeader=${dstType}_${runSpecies}_${buildTag}-000${run}-
filePath=${directory}${fileHeader}

totalLargeSegments=$(ls -1 ${filePath}*000.root | wc -l)
totalLargeSegments=$((${totalLargeSegments} - 1))

nTotal=1000 #10000 events per segment
nEvents=1000 # nEvents per job
nSkips=$((($nTotal / $nEvents) - 1))

outDir=fileLists

if [[ ! -d ${outDir} ]]; then
  mkdir -p ${outDir}
fi

for largeSegment in $(seq 0 $totalLargeSegments)
do
  largeSegment=$(printf "%02d" $largeSegment)
  fileNames=${filePath}${largeSegment}*.root

  nDSTs=$(ls -1 ${fileNames} | wc -l)
  nDSTs=$((${nDSTs} - 1))

  outFile=${outDir}/file_run${run}_${largeSegment}.list

  if [[ -f ${outFile} ]]; then
    rm ${outFile}
  fi

  echo "Writing output file ${outFile}"

  fileCounter=0
  for i in $(seq 0 $nDSTs)
  do
    smallSegment=$(printf "%03d" $fileCounter)
    #checkFile=${filePath}${smallSegment}.root
    checkFile=${filePath}${largeSegment}${smallSegment}.root
    while [[ ! -f ${checkFile} ]]
    do
      ((fileCounter++))
      smallSegment=$(printf "%03d" $fileCounter)
      #checkFile=${filePath}${smallSegment}.root
      checkFile=${filePath}${largeSegment}${smallSegment}.root
    done

    DST=$(realpath ${checkFile} | awk -F '/' '{print $NF}' ) 
    ((fileCounter++))

    for i in $(seq 0 $nSkips)
    do
      startEvent=$(($i*$nEvents))

      echo "${nEvents} ${DST} ${directory} ${startEvent}" >> ${outFile}
      #echo "${nEvents} ${DST} ${startEvent}" >> ${outFile}

    done
 
  done

done

