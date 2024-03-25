This package is designed to subtract jets using the jet multiplicity method, the iterative method and the area method and saves resulting momentum to TTree.


# Building 
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
 
   * Test run using Fun4All. The Fun4All macro takes in input file lists for the truth jet and calo cluster DSTs. For example, you can run:
          
          root -b -q 'Fun4All_JetBkgd.C("dst_truth_jet.list", "dst_calo_cluster.list", "dst_global.list", "output.root", 0.4)'
   
   * This will create an output file containing all the necessary information for the histogram making.
   ## Options in JetBkgdSub
   There are a few options you can specify in the Fun4All macro for the JetBkgdSub module:
   * setPtRange(low, high): set the pt range of TRUTH jets you are interested in keeping in your tree
   * setEtaRange(low, high): set the eta range of TRUTH jets you are interested in keeping in your tree
   * add_input(string): add jet input to the tree. The string should be the name of the tower input.
   * doIterative(bool): Saves iterative background subtracted jets to the tree
   * doAreaSub(bool): Saves area background subtracted jets to the tree
   * doMultSub(bool): Saves multiplicity background subtracted jets to the tree
   * doTruth(bool): Saves truth jets to the tree
   * setMinRecoPt(float): Sets the minimum pt of the reconstructed jet to be saved to the tree
   


# Questions/suggestions
This package is a work in progress, so please send any questions, bugs, or suggestions for new features to Tanner Mengel (tmengel@vols.utk.edu)
