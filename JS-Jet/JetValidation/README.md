This package is designed to create jet performance plots using simulations or data. The code first runs over DSTs to create root trees containing the relevant jet information, then offline root macros are used to create and plot histograms.

# Tree Making
The tree making step is done using the JetValidation analysis module, run using the macro macro/Fun4All_JetVal.C. 

To run the tree making step (note if you are using a bash shell use instead the versions of the commands in parentheses):
1. Build the package in the [usual way](https://wiki.bnl.gov/sPHENIX/index.php/Example_of_using_DST_nodes#Building%20a%20package):

   * Make a build directory inside the src directory: 
  
         cd src
         mkdir build
         cd build
        
   * Setup the sPHENIX environment and install paths:

         source /opt/sphenix/core/bin/sphenix_setup.csh -n (source /opt/sphenix/core/bin/sphenix_setup.sh)
         setenv MYINSTALL ~/install (export MYINSTALL=~/install)
         source /opt/sphenix/core/bin/setup_local.csh $MYINSTALL (source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL)
        
   * Compile your code:

     	 /PATH_TO_YOUR_SOURCE_DIR/autogen.csh --prefix=$MYINSTALL (/PATH_TO_YOUR_SOURCE_DIR/autogen.sh --prefix=$MYINSTALL)
         make -j 4
         make install
         
2. Run the code using the Fun4All macro:
 
   * Go to the macro directory:
   
          cd ../../macro
          
   * Get some files to run on using CreateFileList.pl, for example to get 1000 events of pythia dijets embeded in minimum bias HIJING:
          
          CreateFileList.pl -n 1000 -type 11 -embed DST_CALO_CLUSTER DST_TRUTH_JET DST_GLOBAL
 
   * Test run using Fun4All. The Fun4All macro takes in input file lists for the truth jet and calo cluster DSTs and an output file name as inputs. For example, you can run:
          
          root -b -q 'Fun4All_JetVal.C("dst_truth_jet.list", "dst_calo_cluster.list", "dst_global.list", "output.root")'
   
   * This will create an output file containing all the necessary information for the histogram making.
   ## Options in JetValidation
   There are a few options you can specify in the Fun4All macro for the JetValidation module:
   * setPtRange(low, high): set the pt range of TRUTH jets you are interested in keeping in your tree
   * setEtaRange(low, high): set the eta range of TRUTH jets you are interested in keeping in your tree
   * doUnsub(flag): save also the pt of reconstructed jets before UE subtraction and the amount of Et subtracted (flag is set to 0 or 1)
   * doTruth(flag): save also the truth jet kinematics (set to 0 for running over data) 
   * doSeeds(flag): save also the seed jets (flag is set to 0 or 1)

# Histogram Making
Once you have made trees containing the relevant information, the next step is to make and plot histograms of the quantities of interest. The scripts for this are found in the offline/ directory.

### Jet_reso.C: 
Jet_reso.C is a root macro which takes in the trees from the previous step and creates histograms. It matches truth and reconstructed jets in our trees based on the distance between them and produces histograms of the jet response as well as histograms of the truth and reconstructed jet kinematics. The response is then fit to produce the jet energy scale and jet energy resolution. The histograms are all written out to an output root file. The default input is the file "output.root" produced in the macros directory, but this can be changed in the macro. You can also read in multiple output files (see the commented out loop over files for an example).

### draw_hists.C
draw_hists.C takes the histogram output from Jet_reso.C and draws nice versions of the plots, then saves them as pdfs. Currently it draws only the JES/JER and plots of the subtracted Et, but more will be added soon.

# Questions/suggestions
This package is a work in progress, so please send any questions, bugs, or suggestions for new features to Virginia Bailey (vbailey13@gsu.edu)
