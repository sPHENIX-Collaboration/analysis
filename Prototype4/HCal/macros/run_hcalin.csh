#!/bin/tcsh

date

echo  inner HCAL 1st col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,0,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,0,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,0,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,0,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,0,\"HCALIN\"\)

echo  inner HCAL 2nd col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,1,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,1,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,1,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,1,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,1,\"HCALIN\"\)

echo  inner HCAL 3rd col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,2,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,2,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,2,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,2,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,2,\"HCALIN\"\)

echo  inner HCAL 4th col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,3,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,3,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,3,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,3,\"HCALIN\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,3,\"HCALIN\"\)

date

