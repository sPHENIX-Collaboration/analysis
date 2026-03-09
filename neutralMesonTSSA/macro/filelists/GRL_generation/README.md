Steps to create a good run list:

1. $ python3 GoldenCaloRunListGenerator.py
--> creates runNumbers_all_p007.txt, runNumbers_with_AtLeast_500kEvents.txt, GoldenEmcalRunList.txt, GoldenIHCalRunList.txt, GoldenOHCalRunList.txt, GoldenCalorimeterRunList.txt

2. $ GenerateEventNumberList.sh
--> creates EventNumberList_GoldenEmcalRuns.txt

3. $ root -b -q GoldenGL1RunListGenerator.C
--> creates GoldenGL1RunList.txt

4. $ GenerateClockList.sh
--> creates lists under ./list_allFEM_clock

5. $ root -b -q GoldenFEMrunListGenerator.C
--> creates GoldenFEMrunList.txt

6. Hot tower map check
    a) $ cp GoldenFEMrunList.txt ../RD/2024/Nov7 && cd ../RD/2024/Nov7
    b) $ CreateDstList.pl --build ana437 --cdb 2024p007 --list ../../../GRL_generation/GoldenFEMrunList.txt DST_CALO_run2pp
        --> creates DST lists in ../RD/2024/Nov7
    c) $ cd ../../../..
    d) $ rm condor/job_filelists/hotmapcheck/*.list
    e) $ python3 CreateHotMapLists.py   --> creates per-job files in condor/job_filelists/hotmapcheck
    f) $ cd condor
    g) $ rm out/hotmaps/*.out
    h) Edit condor_hotmapcheck.job to submit the correct number of jobs ( = ls job_filelists/hotmapcheck | wc -l)
    i) $ condor_submit condor_hotmapcheck.job
    j) Rerun $ python3 CreateHotMapLists.py   --> Scans job log files and creates emcal_withmaps.txt

    UPDATE: do this instead
    a) $ ./getHotMapRuns.sh	--> creates all_runs_with_hotmap.txt and emcal_withmaps.txt

7. $ root -b -q CheckRunsForSpin.C'("emcal_withmaps.txt")'   --> creates validspinruns.txt, spinQAlog.root and spinQAlog.pdf

validspinruns.txt is the final good run list

