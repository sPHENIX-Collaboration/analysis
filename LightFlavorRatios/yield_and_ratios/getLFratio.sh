#!/bin/bash

#inFile=/sphenix/user/cdean/plots/Lambda0K_S0ratio/Lambda0ToK_S0_ratio.C
inFile=lambdaKsratio_pT.C

binWidth=0.28
binStart=0.2
halfBinWidth=$(echo ${binWidth}*0.5 | bc)

for (( i=1;i<=10;i++  ))
do
  xHigh=$(echo ${binStart}+$i*${binWidth} | bc)
  xValue=$(echo $xHigh-${halfBinWidth} | bc)
  xLow=$(echo $xHigh-${binWidth} | bc)
  yValue=($(grep SetBinContent\(${i} ${inFile} | awk -F ',' '{print $2}' | awk -F ')' '{print $1}'))
  yError=($(grep SetBinError\(${i} ${inFile} | awk -F ',' '{print $2}' | awk -F ')' '{print $1}'))

  echo "  sphenix_3run.push_back({${xValue}, ${xLow}, ${xHigh}, ${yValue}, ${yError}, 0, 0});"
done
