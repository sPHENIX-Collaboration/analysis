# TowerInfoExample

## Building the module
* Load sPHENIX newest software:
  * source /opt/sphenix/core/bin/sphenix_setup.sh -n new
* Create the environmental variable MYINSTALL point to you installation directory
  * Ex.: export MYINSTALL=/sphenix/u/antoniosilva/myInstall (This is just an example, you have to change it accordingly to your directories)
* Load some more setup:
  * source $OPT_SPHENIX/bin/setup_local.sh $MYINSTALL
* Inside the TowerInfoExample directory, go to src
  * cd src
* Now create a build directory inside src
  * mkdir build
  * cd build
* Run autogen.sh from inside the build directory. Don't forget to use the entire path where it is located. Use the --prefix to pass the MYINSTALL variable
  * Ex.: /sphenix/u/antoniosilva/analysis/TowerInfoExample/src/autogen.sh --prefix=$MYINSTALL (This is just an example, you have to change it accordingly to your directories)
* Compile it:
  * make install

## Running the software
* After building the package, go back to the TowerInfoExample directory and run Fun4All_TowerBuilder.C
  * root Fun4All_TowerBuilder.C

* The Fun4All_TowerBuilder.C is ready to run over a few events of the Pythia 8 simulation (MDC2). This is minimum bias pp events at 200 GeV
* Information about energy and time of the towers should be printed when running the code

## IMPORTANT additional information
* SaveTowerInfo.cc will get information from old towers and save it in the new TowerInfo classes. This is just a way to get some tower information and save it to the node. Soon, this will be done by the new TowerBuilder class

* RunTowerInfo.cc will get the TowerInfoContainer from the node and loop over the towers. I believe this is the most important part of the example and will show how to access the tower information and how to use the TowerInfoContainer to get the stl maps containing the TowerInfo objects
