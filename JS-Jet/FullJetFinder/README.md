This package is designed to full jet analysis trees using simulations or data. The code first runs over DSTs to create root trees containing the relevant jet information, then offline root macros are used to create and plot histograms.

# Tree Making
The tree making step is done using the FullJetFinder analysis module, run using the macro macro/Fun4All_FullJetFinder.C. 

To run the tree making step (note if you are using a bash shell use instead the versions of the commands in parentheses):
1. Build the package in the [usual way](https://wiki.bnl.gov/sPHENIX/index.php/Example_of_using_DST_nodes#Building%20a%20package):

   * Make a build directory inside the src directory: 
  
         cd src
         mkdir build
         cd build
        
   * Setup the sPHENIX environment and install paths:

         source /opt/sphenix/core/bin/sphenix_setup.csh -n (source /opt/sphenix/core/bin/sphenix_setup.sh)
         setenv MYINSTALL ~/install (export MYINSTALL=~/install)
         source /opt/sphenix/core/bin/setup_local.csh $MYINSTALL (source /opt/sphenix/core/bin/setup_local.sh)
        
   * Compile your code and rerun the startup steps:

     	 /PATH_TO_YOUR_SOURCE_DIR/autogen.csh --prefix=$MYINSTALL (/PATH_TO_YOUR_SOURCE_DIR/autogen.sh --prefix=$MYINSTALL)
         make -j 4
         make install
         source /opt/sphenix/core/bin/sphenix_setup.csh -n (source /opt/sphenix/core/bin/sphenix_setup.sh)
         setenv MYINSTALL ~/install (export MYINSTALL=~/install)
         source /opt/sphenix/core/bin/setup_local.csh $MYINSTALL (source /opt/sphenix/core/bin/setup_local.sh)
         
2. Run the code using the Fun4All macro driver script:
 
   * Go to the macro directory:
   
          cd ../../macro

   * Update the paths in run_MDC2reco.sh macros in the beginning, so we can run on Condor
          
   * Get some files to run on using makeCondorJobs.py, for example like (replacing N with the latest simulation run):

          python makeCondorJobs.py -r 11 -i JET_30GEV -o /mypath/JET30_r11_30GeV --nopileup
          
          where the -o specifies path for the output
          
   * Test run run_MDC2reco, where the first argument is the output path and the next are path to DST_TRACKS, DST_TRUTH, DST_CALO_CLUSTER and DST_GLOBAL in any order. For example, you can run:
          
          ./run_MDC2reco.sh /mypath/JET30_r11_30GeV /mypath/JET30_r11_30GeV/condorJob/fileLists/productionFiles-JET_30GEV-dst_tracks-00000.list /mypath/JET30_r11_30GeV/condorJob/fileLists/productionFiles-JET_30GEV-dst_truth-00000.list /mypath/JET30_r11_30GeV/condorJob/fileLists/productionFiles-JET_30GEV-dst_calo_cluster-00000.list /mypath/condorJob/fileLists/productionFiles-JET_30GEV-dst_global-00000.list
   
       in order to submit jobs to condor in folder condorJob execute the myJET_30GEV.job as condor_submit myJET_30GEV.job