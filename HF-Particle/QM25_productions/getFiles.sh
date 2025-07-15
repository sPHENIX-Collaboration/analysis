#!/bin/bash

run=$1

dirStart=${run:0:3}
dirEnd=$(($dirStart + 1))

buildTag=ana475_2024p017_v001

directory=/sphenix/lustre01/sphnxpro/production/run2pp/physics/${buildTag}/DST_TRKR_TRACKS/run_000${dirStart}00_000${dirEnd}00/dst/

fileHeader=DST_TRKR_TRACKS_run2pp_${buildTag}-000${run}-
filePath=${directory}${fileHeader}

totalLargeSegments=1 #$(ls -1 ${filePath}*00.root | wc -l)
totalLargeSegments=$((${totalLargeSegments} - 1))

nTotal=10000 #10000 events per segment
nEvents=10000 # nEvents per job
nSkips=$((($nTotal / $nEvents) - 1))

outDir=fileLists

if [[ ! -d ${outDir} ]]; then
  mkdir -p ${outDir}
fi

for largeSegment in $(seq 0 $totalLargeSegments)
do
  fileNames=${filePath}${largeSegment}*.root

  largeSegment=$(printf "%03d" $largeSegment)

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

    smallSegment=$(printf "%05d" $fileCounter)
    checkFile=${filePath}${smallSegment}.root
    while [[ ! -f ${checkFile} ]]
    do
      ((fileCounter++))
      smallSegment=$(printf "%05d" $fileCounter)
      checkFile=${filePath}${smallSegment}.root
    done

    DST=$(realpath ${checkFile} | awk -F '/' '{print $NF}' ) 
    ((fileCounter++))

    for i in $(seq 0 $nSkips)
    do
      startEvent=$(($i*$nEvents))

      echo "${nEvents} ${DST} ${directory} ${startEvent}" >> ${outFile}

    done
 
  done

done
