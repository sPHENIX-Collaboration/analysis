There are two different commpiled macros written:
    JetRhoMedian - for pythia jets alone, reconstructs truth jets, calo jets, and rho
                 - for pythia embedded in HIJING, also given centrality
    RhoMedianFluct - just gets centrality, rho, and error on embedded 100 GeV particle with 100 - (recon.-rho x A)

    There are two levels of embedded jets: those set for a min pt at 10 and a min pt at 30 GeV.
    2023.04.10 update: The jets set for the 30 GeV are really closer to 40 GeV. C. Pinkenburg is generating new 30 GeV ones now.

    The five directories, with the full_lists subdirectories, were made by hand. 
    Each full_lists directory was filled with dst file lists using the indicarted commands:
    (It is ok to not use sub a stupidly large number after -n; I just wanted all the events...)

```
.
├── A40_pp
│   ├── full_lists
         $ CreateFileList.pl -n 10000000000 -nopileup -type 11 DST_TRUTH_JET DST_CALO_CLUSTER
│   │   ├── dst_calo_cluster.list
│   │   └── dst_truth_jet.list
│   ├── Fun4All_JetRhoMedian.C
│   ├── condor.job
│   ├── condor.sh
│   └── test.sh
├── B_hijing
│   ├── full_lists
│   │     $ CreateFileList.pl -n 10000000000 -type 4 DST_CALO_CLUSTER DST_BBC_G4HIT
│   │   ├── dst_bbc_g4hit.list
│   │   └── dst_calo_cluster.list
│   ├── condor.job
│   ├── condor.sh
│   └── test.sh
└── C40_ppemb
    ├── condor.job
    ├── condor.sh
    ├── test.sh
    └── full_lists
         $ CreateFileList.pl -n 10000000000 -embed -type 19 DST_TRUTH_JET DST_CALO_CLUSTER DST_BBC_G4HIT
        ├── dst_bbc_g4hit.list
        ├── dst_calo_cluster.list
        └── dst_truth_jet.list
```

 CreateFileList.pl -n 10 -embed -nopileup -type 19

 Then in each directory (one level up from full_lists), the script ../make_inp_lists.py <n-flines-per-file> <n-files> 
 was run. It generated a new output directory named like (for n-lines-per-file=120 and n-files=100) jobs_120x100,
 in which the input lists have been divided into the  queue.list file which has been generated.

 The `Fun4All_*.C`, `condor.job` and `condor.sh` scripts are placed by hand in each file. They are run from inside one of the
 the `jobs_*x*` files with a command like the following:
    `jobs_120x100 $ condor_submit ../condor.job`


For example: 
To run the fastjet::JetMedianBackgroundEstimator code on the pp embedded in HIJING code, do the following,
   where the lines are like :
   `[pwdlocation] $ [commands] # [optional comment]`,
   where `[pwdlocation]` is the directory you should be in, and `[command]` is what you should type,
   and sometimes there are comments:

   Do this:
 - `[macros] $ cd ../src`
 - `[src] $ mkdir build`
 - `[src] $ cd build`
 - `[build] $ ../autogen.sh ${HOME}/install` # (or wherever your local install directory is located)
 - `[build] $ cd ../../macros/C40_ppemb/full_lists`
 - `[full_lists] $ CreateFileList.pl -n 10000000000 -embed -type 19 DST_TRUTH_JET DST_CALO_CLUSTER DST_BBC_G4HIT`
 - `[full_lists] $ cd ../..`
 - `[pp40_emb] $ ../make_inp_lists.py 2 2` # generates file `jobs_2x2` -- small sample to run over
 - `[pp40_emb] $ cd jobs_2x2`
 - `[jobs_2x2] $ ../test.sh` # runs a short job interactively and outputs `./output.root`
 - `[jobs_2x2] $ condor_submit ../condor.job` # submits the test jobs to condor.
 - `[jobs_2x2] $ cd ..`
 - `[pp40_emb] $ ../make_inp_lists.py 50` # creates a full run, with 50 input files per job
 - `[pp40_emb] $ cd jobs_50xAll` 
 - `[jobs_50xAll] $ condor_submit ../condor.job`

   And that's it! You have run all the jobs.

   A few notes:
    - This code outputs whole TTree's, which may be heavier than something that you want
   
