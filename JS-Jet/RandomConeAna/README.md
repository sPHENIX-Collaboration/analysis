This package is designed to construct random cones from tower components for area, iterative, and multiplicity subtracted cones. There are 3 modes:
       * Random Cones:
              Randomly places $\eta$ $\phi$ cone with R = 0.4
       * Randomized correlations
              Cones with all tower $\eta$ $\phi$ scrambbled within acceptance
       * Avoid Leading jet
              Cones that are atleast $\Delta R$ = 1.4 away from leading truth jet in the event


# Building 
1. Build the package in the [usual way](https://wiki.bnl.gov/sPHENIX/index.php/Example_of_using_DST_nodes#Building%20a%20package):

   * Make a build directory inside the src/RandomCones directory: 
  
         cd src/RandomCones
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

   * Re-source the sPHENIX environment:

         source /opt/sphenix/core/bin/sphenix_setup.csh -n (source /opt/sphenix/core/bin/sphenix_setup.sh)
         setenv MYINSTALL ~/install (export MYINSTALL=~/install)
         source /opt/sphenix/core/bin/setup_local.csh $MYINSTALL (source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL)

   * Make a build directory inside the src/TowerRho directory:

         cd src/TowerRho
         mkdir build
         cd build

   * Repeat steps 2, 3, and 4 for the TowerRho package.

   * Make a build directory inside the src directory:

         cd src
         mkdir build
         cd build
       
   * Repeat steps 2, 3, and 4 for the RandomConeTree package.

         
2. Run the code using the Fun4All macro:
 
   * Go to the macro directory:
   
          cd ../../macro
          
   * Get some files to run on using CreateFileList.pl, for example to get 1000 events of pythia dijets embeded in minimum bias HIJING:
          
          CreateFileList.pl -n 1000 -type 11 -embed DST_CALO_CLUSTER DST_GLOBAL
 
   * Test run using Fun4All. The Fun4All macro takes in input file lists for the global and calo cluster DSTs. For example, you can run:
          
          root -b -q -l 'Fun4All.C("dst_calo_cluster.list", "dst_global.list", "output.root")'
   
   * This will create an output file containing all the necessary information for the histogram making.
   
   ## Options in RandomConeAna
   There are a few options you can specify in the Fun4All macro for the RandomConeAna module:
   * setEventSelection(low, high): Sets the pT range of the leading R = 0.4 truth jet for event weighting MDC2 data. (If you don't want to use this then don't setEventSelection). Defualt is not to do event selection.
   * addWeight(low, high): Set event weight to be added to output tree. Useful for MDC2 data. Default is not to add a weight to output tree. 
   * add_input(string): add jet input to the module. The string should be the name of the tower input. Will add raw cone pT to the output tree
   * add_iter_input(bool): add iterativativly subtracted jet tower input to the tree. The string should be the name of the sub1 tower input. Will add iter cone pT to the output tree
   * set_user_seed(bool): Seeds the random number generator for selecting cone $\eta$ $\phi$  and randomizing tower corrdinates. Defualt is seeded by TTimeStamp. 
   


# Questions/suggestions
This package is a work in progress, so please send any questions, bugs, or suggestions for new features to Tanner Mengel (tmengel@vols.utk.edu)
