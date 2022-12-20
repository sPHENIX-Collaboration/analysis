# BuildResonanceJetTaggingTree

* Change the path of the variable $MYINSTALL in the file run_MDC2reco.sh
* Run makeCondorJobs.py with parameters to get the files to run MDC2 simulations. My suggestion is:
  * python makeCondorJobs.py -i D0JETS --truth --calo --truth_table -t 100000 -f 1
  * -t: number of events
  * -i: D0JETS will use the D0-Jet production

* After running makeCondorJobs.py, go to the recent created directory condorJob, there you have the file myD0JETS.job that will submit the jobs to run on the number of events and production you selected. Just do:
  * condor_submit myD0JETS.job
