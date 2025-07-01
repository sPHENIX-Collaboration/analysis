# Running K-short reconstructions on real data

## Overview

These are the full scripts used for producing various resonance nTuples used for Quark Matter 25. It also contains K-short and D0 invariant mass fitters.

By default, only the K-short reconstruction is enabled but there are several other reconstructions that can be performed. Be aware that they will have significantly more background than the K-short and hence require more events processed to see anything. They are mainly used as examples to write your own reconstruction around.

## Test 1, running the macro

```Fun4All_HF_QM25.C``` is a top level macro that takes a **single** track DST and runs reconstructions with selections defined in ```HF_selections_QM25.C```. It makes output files, finds associated trigger and cluster DSTs for the track DST you read and handles all the file movements. It is set to run over 100 trigger frames and find K-short candidates. Run it with

```c++
root -l -q -b Fun4All_HF_QM25.C
```

As Fun4All processes, you will start to see information printed to the screen. As long as KFParticle's verbosity is > 0, it will print out each candidates information. You will get the particle masses, momentum and positions amongst other things. 

After it finishes, you should have a new folder called ```pipi_reco``` with a file ```outputKFParticle_pipi_reco_00053877_00000_00000.root``` containing the reconstructed candidates. Open this file in the root prompt then type
```
DecayTree->Draw("K_S0_mass")
```
to see the pipi invariant mass. There are several output branches in this file with information on the mother, daughters (including detector information like number of TPC states used by ACTS), global information such as the PV position or number of tracks associated to the PV, and trigger information

## Test 2, running with shell scripts

I normally find it easier to run KFParticle using shell scripts. Run the previous reconstruction using the command:

```bash
./runHFreco.sh 10000 DST_TRKR_TRACKS_run2pp_ana475_2024p017_v001-00053877-00000.root /sphenix/lustre01/sphnxpro/production/run2pp/physics/ana475_2024p017_v001/DST_TRKR_TRACKS/run_00053800_00053900/dst/ 0
```

The same track DST will be analysed, but this time we will run over 10000 trigger frames

## Test 3, fitting

After Fun4All has finished, the file should now contain 302 candidates. You can fit the selected candidates uses a RooFit macro in the folder ```K_S0```. Run it with
```bash
root -l -q -b K_S0_Fitter.C 
```

You should now have 3 files for the fit: a root macro, a PDF and a PNG. Feel free to use this fitter to develop further fitting codes.

## Further things that can be done

All previous instructions should be useful in getting people familiar with KFParticle and simple reconstructions that don't require much CPU resources. It is possible to run multiple reconstructions simultaneously in Fun4All, and to run over lots of data using condor. I won't go into much detail here to avoid oversaturating our condor resources but you can build a condor job list by using a shell script provided here
```bash
sh getFiles.sh 53877
```
which will get all track DSTs for run 53877. 

You can get as many condor lists as needed with this script. Then open ```submitCondorJobs.sh``` and alter ```runs``` to contain the runs you with to iterate over. After this, submit condor jobs with
```bash
sh submitCondorJobs.sh
```
