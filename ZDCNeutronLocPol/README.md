This module calculates the raw square root asymmetries of forward neutrons in the ZDC/SMD. 

**Running standalone:** 
1) `cd macros/`
2) `root.exe`
3) `.x Fun4All_ZDCNeutronLocPol.C(<DST_file>,<output_file>,<startEvent>,<nEvents>)`

`<DST_file>`: use a raw triggered DST found in _/sphenix/lustre01/sphnxpro/commissioning/slurp/calophysics/_ or _/sphenix/lustre01/sphnxpro/physics/slurp/calophysics/_
`<output_file>`: output root file that contains SMD hit trees  
`<startEvent>`: Event to start Fun4All at, default: 0  
`<nEvents>`: Number of events Fun4All will run over, default: 10000  

**Running with condor:**  
To detect a percent level asymmetry it becomes necessary to run over greater than 10^7 events. Each segment of the automatically produced DSTs contain 10000 events so we can utilize condor to speed up our workflow.  

1) `cd macros/`  
2) In run_smd.sh, ensure that condor=true and fullrun=true  
3) In run_smd.sh, set fname to the input DST with %04d where the segment number would be e.g. _"DST_TRIGGERED_EVENT_run2pp_new_2024p001-00042797-%04d.root"_  
4) Ensure that the directory in ofname1 exists  
5) In runsmd.job, change Initialdir to your directory  
5) In runsmd.job, set Queue <number_of_jobs>, default <number_of_jobs> = 500 (do not do too many more than this at one time)  
6) `condor_submit runsmd.job`

**Square root asymmetries:**  
Once you have your trees produced, the raw square root asymmetries are calculated and drawn by:  
1) `root.exe`
2) `.x drawAsym.C(<tree_file>,<storenumber>,<runnumber>)`

`<tree_file>`: root output file from Fun4All containing SMD hit trees  
`<storenumber>`: store number, default: 34485  
`<runnumber>`: run number, default: 42796  


**Example results:**  
The asymmetries of Runs 42796+42797 from store 34485 can be seen in `macros/output`
