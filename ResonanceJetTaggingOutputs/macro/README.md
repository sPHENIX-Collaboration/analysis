# BuildResonanceJetTaggingTree

## Building the module
* Load sPHENIX newest software:
  * source /opt/sphenix/core/bin/sphenix_setup.sh -n new
* Create the environmental variable MYINSTALL point to you installation directory
  * Ex.: export MYINSTALL=/sphenix/u/antoniosilva/myInstall (This is just an example, you have to change it accordingly to your directories)
* Load some more setup:
  * source $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL
* Inside the ResonanceJetTaggingOutputs directory, create a build directory
  * mkdir build
  * cd build
* Run autogen.sh from inside the build directory. Don't forget to use the entire path where it is located. Use the --prefix to pass the MYINSTALL variable
  * Ex.: /sphenix/u/antoniosilva/analysis/ResonanceJetTaggingOutputs/autogen.sh --prefix=$MYINSTALL (This is just an example, you have to change it accordingly to your directories)
* Compile it:
  * make install

## Running the software

* Don't forget to load sPHENIX software: source /opt/sphenix/core/bin/sphenix_setup.sh -n new
* Change the path of the variable $MYINSTALL in the file run_MDC2reco.sh. It should point to your install directory
* In run_MDC2reco.sh, the $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL will be loaded
* Run makeCondorJobs.py with parameters to get the files to run MDC2 simulations. My suggestion is:
  * python makeCondorJobs.py -i D0JETS --truth --calo --truth_table -t 100000 -f 1
  * -t: number of events
  * -i: D0JETS will use the D0-Jet production

* After running makeCondorJobs.py, go to the recent created directory condorJob, there you have the file myD0JETS.job that will submit the jobs to run on the number of events and production you selected. Just do:
  * condor_submit myD0JETS.job
