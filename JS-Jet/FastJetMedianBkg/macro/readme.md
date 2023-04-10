There are two different commpiled macros written:
    JetRhoMedian - for pythia jets alone, reconstructs truth jets, calo jets, and rho
                 - for pythia embedded in HIJING, also given centrality
    RhoMedianFluct - just gets centrality, rho, and error on embedded 100 GeV particle with 100 - (recon.-rho x A)

    There are two levels of embedded jets: those set for a min pt at 10 and a min pt at 30 GeV.
    2023.04.10 update: The jets set for the 30 GeV are really closer to 40 GeV. C. Pinkenburg is generating new 30 GeV ones now.

    The five directories, with the full_lists subdirectories, were made by hand. 
    Each full_lists directory was filled with dst file lists using the indicarted commands:
    (It is ok to not use sub a stupidly large number after -n; I just wanted all the events...)
.
├── A10_pp
│   ├── full_lists
         $ CreateFileList.pl -n 10000000000 -nopileup -type 12 DST_TRUTH_JET DST_CALO_CLUSTER
│   │   ├── dst_calo_cluster.list
│   │   └── dst_truth_jet.list
├── A30_pp
│   ├── full_lists
         $ CreateFileList.pl -n 10000000000 -nopileup -type 11 DST_TRUTH_JET DST_CALO_CLUSTER
│   │   ├── dst_calo_cluster.list
│   │   └── dst_truth_jet.list
├── B_hijing
│   ├── full_lists
         $ CreateFileList.pl -n 10000000000 -type 4 DST_CALO_CLUSTER DST_BBC_G4HIT
│   │   ├── dst_bbc_g4hit.list
│   │   └── dst_calo_cluster.list
├── C10_ppemb
│   ├── full_lists
         $ CreateFileList.pl -n 10000000000 -embed -type 12 DST_TRUTH_JET DST_CALO_CLUSTER DST_BBC_G4HIT
│   │   ├── dst_bbc_g4hit.list
│   │   ├── dst_calo_cluster.list
│   │   └── dst_truth_jet.list
└── C30_ppemb
    └── full_lists
         $ CreateFileList.pl -n 10000000000 -embed -type 11 DST_TRUTH_JET DST_CALO_CLUSTER DST_BBC_G4HIT
        ├── dst_bbc_g4hit.list
        ├── dst_calo_cluster.list
        └── dst_truth_jet.list

 Then in each directory (one level up from full_lists), the script ../make_inp_lists.py <n-flines-per-file> <n-files> 
 was run. It generated a new output directory named like (for n-lines-per-file=120 and n-files=100) jobs_120x100,
 in which the input lists have been divided the queue.list file has been generated.

 The `Fun4All_*.C`, `condor.job` and `condor.sh` scripts are placed by hand in each file. They are run from inside one of the
 the `jobs_*x*` files with a command like the following:
    `jobs_120x100 $ condor_submit ../condor.job`
