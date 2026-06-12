#!/bin/bash
run=$1
dirStart=${run:0:3}
dirEnd=$(($dirStart + 1))
runSpecies=run3pp
buildTag=ana538_2025p011_v001
dstType=DST_TRKR_TRACKS
directory=DIRECTORY
if [[ "${runSpecies}" == "run3auau" || "${runSpecies}" == "run3pp" ]]; then
  directory=/sphenix/lustre01/sphnxpro/production/${runSpecies}/physics/${buildTag}/${dstType}/run_000${dirStart}00_000${dirEnd}00/
fi
if [[ "${runSpecies}" == "run2pp" ]]; then
  directory=/sphenix/lustre01/sphnxpro/production/${runSpecies}/physics/${buildTag}/${dstType}/run_000${dirStart}00_000${dirEnd}00/dst/
fi
fileHeader=${dstType}_${runSpecies}_${buildTag}-000${run}-
filePath=${directory}${fileHeader}
totalLargeSegments=$(ls -1 ${filePath}*000.root | wc -l)
totalLargeSegments=$((${totalLargeSegments} - 1))
nTotal=1000
nEvents=1000
nSkips=$((($nTotal / $nEvents) - 1))

filesPerList=20

outDir=fileLists
if [[ ! -d ${outDir} ]]; then
  mkdir -p ${outDir}
fi

# Master list file to store all produced list file names
masterList=${outDir}/masterList_run${run}.list
if [[ -f ${masterList} ]]; then
  rm ${masterList}
fi

groupCounter=0
for largeSegment in $(seq 0 $totalLargeSegments)
do
  largeSegment=$(printf "%02d" $largeSegment)
  fileNames=${filePath}${largeSegment}*.root
  nDSTs=$(ls -1 ${fileNames} | wc -l)
  nDSTs=$((${nDSTs} - 1))

  fileCounter=0
  filesInCurrentGroup=0
  outFile=""

  for i in $(seq 0 $nDSTs)
  do
    smallSegment=$(printf "%03d" $fileCounter)
    checkFile=${filePath}${largeSegment}${smallSegment}.root
    while [[ ! -f ${checkFile} ]]
    do
      ((fileCounter++))
      smallSegment=$(printf "%03d" $fileCounter)
      checkFile=${filePath}${largeSegment}${smallSegment}.root
    done

    DST=$(realpath ${checkFile} | awk -F '/' '{print $NF}')

    # Start a new list file every 100 files
    if [[ $filesInCurrentGroup -eq 0 ]]; then
      groupSegment=$(printf "%04d" $groupCounter)
      outFile=${outDir}/${dstType}_${runSpecies}_${buildTag}-000${run}-${groupSegment}.list
      if [[ -f ${outFile} ]]; then
        rm ${outFile}
      fi
      echo "Writing output file ${outFile}"
      # Append the new list filename to the master list
      echo ${outFile} >> ${masterList}
    fi

    for j in $(seq 0 $nSkips)
    do
      startEvent=$(($j*$nEvents))
      echo "${directory}${DST}" >> ${outFile}
    done

    ((fileCounter++))
    ((filesInCurrentGroup++))

    # Reset group counter after every 10 files
    if [[ $filesInCurrentGroup -eq $filesPerList ]]; then
      filesInCurrentGroup=0
      ((groupCounter++))
    fi

  done
done

echo "Master list written to ${masterList}"
