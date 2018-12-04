#!/bin/tcsh

date

ddump -t 9 -p 943 /gpfs/mnt/gpfs04/sphenix/data/data02/sphenix/t1044/fnal/beam/beam_0000$2-0000.prdf > /gpfs/mnt/gpfs04/sphenix/user/xusun/software/data/beam/ShowerCalibAna/$1/run$2_emcal.jpg
ddump -t 9 -p 940 /gpfs/mnt/gpfs04/sphenix/data/data02/sphenix/t1044/fnal/beam/beam_0000$2-0000.prdf > /gpfs/mnt/gpfs04/sphenix/user/xusun/software/data/beam/ShowerCalibAna/$1/run$2_hcal.jpg
