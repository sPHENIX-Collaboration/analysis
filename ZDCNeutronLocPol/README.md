This module calculates the raw square root asymmetries of forward neutrons in the ZDC/SMD. 

**Running standalone:** 
1) cd macros/
2) root.exe
3) `.x Fun4All_ZDCNeutronLocPol.C(<DST_file>,<output_file.root>,<startEvent>,<nEvents>)`

`<DST_file>`: use a raw triggered DST found in /sphenix/lustre01/sphnxpro/commissioning/slurp/calophysics/  
`<output_file>`: output root file that contains SMD hit trees  
`<startEvent>`: Event to start Fun4All at, default: 0  
`<nEvents>`: Number of events Fun4All will run over, default: 10000  


With the large number of events needed to detect a percent level asymmetry it becomes necessary to run over greater than 10^7 events. Each segment of the automatically produced DSTs contain 10000 events so we can utilize condor to speed up our workflow.

**Running with condor:**
1) `cd macros/`
2) In _run_smd.sh_, ensure that condor=true and fullrun=true
3) In _run_smd.sh_, set fname to the input DST with %04d where the segment number would be e.g. "/sphenix/lustre01/sphnxpro/commissioning/slurp/calophysics/run_00042700_00042800/DST_TRIGGERED_EVENT_run2pp_new_2024p001-00042797-%04d.root"
4) Ensure that the directory to which the output files are being written in ofname1 exists
5) In runsmd.job, set Queue = <number_of_jobs> in runsmd.job, default <number_of_jobs> = 100 (do not do too many more than this at one time)
6) `condor_submit runsmd.job`


Once you have your trees produced, `macros/drawAsym.C` will calculate and draw the asymmetries.


Contact dloom@umich.edu if you have any questions.