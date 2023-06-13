This package is designed to create histograms for the jet constituents and jet energy fractions for each calorimeter layer.  

# Histogram Making
Histograms are made with the JetNConsituents analysis module, run using the macro macro/Fun4All_JetNconst.C. 

To run the tree making step (note if you are using a bash shell use instead the versions of the commands in parentheses):
1. Build the package in the [usual way](https://wiki.bnl.gov/sPHENIX/index.php/Example_of_using_DST_nodes#Building%20a%20package):

   * Make a build directory inside the src directory: 
  
         cd src
         mkdir build
         cd build
        
   * Setup the sPHENIX environment and install paths:

         source /opt/sphenix/core/bin/sphenix_setup.sh -n
         export MYINSTALL=~/install 
         source /opt/sphenix/core/bin/setup_local.sh $MYINSTALL
        
   * Compile your code:

     	 /PATH_TO_YOUR_SOURCE_DIR/autogen.sh --prefix=$MYINSTALL 
         make -j4 
         make install
         
2. Run the code using the Fun4All macro:
           
   * Test run using Fun4All. The Fun4All macro takes in input file lists for the truth jet and calo cluster DSTs and an output file name as inputs. For example, you can run:
          
          root -b -q 'Fun4All_JetNconst.C("dst_calo_cluster.list", "output.root")'
   
   * This will create an output file containing histograms showing the total number of constituents in each jet, the number of constituents in each calorimeter layer, and the energy fraction in each calorimeter layer. The histograms are saved in a root file called output.root.


# Making Histograms Pretty
Once you have made the TFile containing the histograms, you can save these as pdfs in the SphenixStyle using plot_histos.C. How to run:
        root -b -q 'plot_histos.C("output.root", "0.4")'
    
    * The first argument is the name of the root file containing the histograms. The second argument is the jet radius. This is used to label the histograms. The output pdfs are saved in the directory where you run the macro. 


# Questions/suggestions
Please send any questions, bugs, or suggestions for new features to Tanner Mengel (tmengel@vols.utk.edu)
