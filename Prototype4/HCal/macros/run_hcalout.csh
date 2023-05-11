#!/bin/tcsh

date

echo  inner HCAL 1st col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,0,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,0,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,0,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,0,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,0,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,5,0,\"HCALOUT\"\)

echo  inner HCAL 2nd col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,1,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,1,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,1,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,1,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,1,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,5,1,\"HCALOUT\"\)

echo  inner HCAL 3rd col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,2,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,2,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,2,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,2,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,2,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,5,2,\"HCALOUT\"\)

echo  inner HCAL 4th col
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,0,3,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,1,3,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,2,3,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,3,3,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,4,3,\"HCALOUT\"\)
root -l -b -q  Fun4All_TestBeam_HCAL_SampleFrac.C\(1000,5,3,\"HCALOUT\"\)

date

