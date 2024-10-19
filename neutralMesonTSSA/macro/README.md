# General workflow

## Creating a good run list
1. EmcalGetGoodRuns.py --> checks run triage database for 'golden' EMCal runs --> emcal_goodruns.txt
2. Create dst_calo\* lists for each run using CreateDstList.pl
3. CreateHotMapList.py --> creates per-job filelists (1 DST per file, 1 file per run) for condor jobs
4. condor_submit condor_hotmapcheck.job --> runs jobs to produce log files included whether a hot tower map exists in the CDB
5. CreateHotMapLists.py --> reads log files from above, selects runs with available hot tower map --> emcal_withmaps.txt
6. CheckRunsForSpin.C --> checks Spin DB for valid spin info --> validspinruns.txt

## Running neutralMesonTSSA jobs
7. Use CreateDstList.pl again to create dst_calo\* lists for only the runs in validspinruns.txt (remove old dst_calo\* files before doing this)
8. CreateJobDSTLists.py --> creates per-job file lists for condor
9. condor_submit condor_neutalmeson.job

Note -- you will likely need to change hard-coded paths in the scripts for creating file lists... or at least mkdir -p to create the paths first
