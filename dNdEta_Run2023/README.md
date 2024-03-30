# Run2023 dNdEta analysis with the INTT data
Updated: 2024-03-30

## Analysis ntuple production
The Fun4All analysis module is located in the `dNdEtaINTT` directory.  

The Fun4All steering macro `Fun4All_G4_sPHENIX.C` is located in the `macros` directory. To locally test the code, first make sure the bash script `production_INTT.sh` has the correct permission
```bash
chmod 755 production_INTT.sh
```
Then run the bash script 
```bash
./production_INTT.sh 0 HIJING 1 testNtuple.root 0 new
```
The arguments are as follows:
- The 1st argument is to indicate whether to use the INTT data DST file (0 for no, 1 for yes)
- The 2nd argument is the generator name (HIJING/EPOS/AMPT); this argument is irrelavent if the 1st argument is set to 1
- The 3rd argument is the number of events to be processed
- The 4th argument is the output file name
- The 5th argument is the process (for the condor job)
- The 6th argument is the F4A software version (new, ana.xxx)

For batch production, the python script `runCondor.py` under the `condor_INTT` is used. 
```
python runCondor.py --help
Usage: runCondor.py ver [options -h]

Options:
  -h, --help            show this help message and exit
  -d, --data            Is data or simulation
  -g GENERATOR, --generator=GENERATOR
                        Generator type (HIJING, EPOS, AMPT)
  -n EVENTPERJOB, --eventPerJob=EVENTPERJOB
                        Number of events per job
  -j NJOB, --nJob=NJOB  Number of jobs (queues)
  -o OUTPUTDIR, --outputdir=OUTPUTDIR
                        Output directory (under
                        /sphenix/user/hjheng/TrackletAna/data/INTT/)
  -s SOFTWAREVERSION, --softwareversion=SOFTWAREVERSION
                        Software version (new, ana.3xx)
  -c, --submitcondor    Submit condor jobs
```
Examples to run the script are
- Data, Run20869
```bash
python runCondor.py --data --eventPerJob 1000 --nJob 551 --outputdir data_Run20869 --softwareversion ana.382 --submitcondor
```
- Simulation, HIJING
```bash
python runCondor.py --generator HIJING --eventPerJob 200 --nJob 400 --outputdir HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams --softwareversion ana.398 --submitcondor
```
- Simulation, EPOS
```bash
python runCondor.py --generator EPOS --eventPerJob 400 --nJob 200 --outputdir EPOS_ana399_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams --softwareversion ana.399  --submitcondor
```
- Simulation, AMPT
```bash
python runCondor.py --generator AMPT --eventPerJob 800 --nJob 100 --outputdir AMPT_ana400_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams --softwareversion ana.400 --submitcondor
```

## Analysis codes & scripts
The analysis codes are located in the `analysis_INTT` and `analysis_INTT_CW` directory.

### For `analysis_INTT`:
First, make the executables by
```bash
make
```
which (for now) should produce the executables `TrackletAna`, `plotTracklets`, `plotCluster`, `BeamspotReco`, `INTTVtxZ`, and `Corrections`. 

### For `analysis_INTT_CW`
It is detailed in the folder


