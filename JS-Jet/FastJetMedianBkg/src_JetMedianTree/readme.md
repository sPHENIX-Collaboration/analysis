To use this code:

 1. Make a build directory and compile the code
    ```
    $ mkdir build
    $ cd build
    $ ../autogen.sh [install_lib_location]
    $ make -j4 install
    $ cd ..
    ```
Steps 2-4 make the JetReco tree
 2. Get the input files:
 ```
   $ cd macro
   $ mkdir full_lists
   $ cd full_lists
   $ CreateFileList.pl -type 11 -embed -n 100000000 DST_BBC_G4HIT DST_CALO_CLUSTER DST_TRUTH_JET DST_TRUTH_G4HIT DST_GLOBAL
   $ cd ..
 ```
 3. Make an output directory to test the files, and modify the running scripts
 ```
   $ chmod a+x make_inp_lists.py test.sh condor.sh
   [modify condor.sh to work with your file install location; condor.sh will be called by test.sh and condor_r07.job]
   [modify test.sh, probably to run just a very few events]
   $ ./make_inp_lists.py 2 1
   $ cd jobs_2x1
   $ ../test.sh
   [inspect the outputs of the generated *.root test file]
   $ cd ..
```
 4. When you are happy with step 3., make a directory to run over all the input file lists. If, for instance, you want jobs
    that will run 50 input files per job, with however many jobs it takes to do them all then:
 ```
   $ ./make_inp_lists.py 50 -1
   [or, in this case, just `$ ./make_inp_lists.py 50` works fine, too]
   $ cd jobs_50xAll
   $ condor_submit ../condor_r04.job
```
 5. hadd together the output and process it
```
    $ cd plot
    $ hadd -r -k hadd_jobs_50xAll.root ../jobs_50xAll/*.root
    $ ln -s hadd_jobs_50xAll.root hadd_Sub1rhoA.root
    $ root -l doJES_Sub1rhoA.C
    [your output file doJES_Sub1rhoA.root contains the desired output data]

```
Steps 6-9 make the rho fluctuation tree
 6. Get the files for the HIJING run
```
    $ mkdir rhofluct_lists
    $ cd rhofluct_lists
    $ CreateFileList.pl -type 4 -n 1000000 DST_BBC_G4HIT DST_CALO_CLUSTER DST_GLOBAL
    $ cd ..
```
 7. Make a small test run:
 ```
    $ ./make_inp_lists.py 1 1 rhofluct_lists 
    $ chmod a+x rhofluct_test.sh rhofluct_condor.sh Fun4All_RhoFluct.C
    [modify rhofluct_test.sh rhofluct_condor.sh and Fun4All_RhoFluct.C to use
    your proper install directory and probably in the test file to fun only a few events]
    $ cd rhofluct_1x1
    $ ../rhofluct_test.sh
    [if the test file works well, you should also test out condor on this small sampple
    before running a full set]
    $ condor_submit ../condor_rhofluct.job
```
 8. After the tests in step 6. work well, run the full statistics
```
    $ ./make_inp_lists.py 250 -1 rhofluct_lists 
    $ cd rhofluct_250xAll
    $ condor_submit ../condor_rhofluct.job
```
 9. hadd together the output and process it
```
    $ cd plot
    $ hadd -r -k hadd_rhofluct_260xAll.root ../rhofluct_250xAll/*.root
    $ ln -s hadd_rhofluct_260xAll.root hadd_RhoFluct.root
    $ root -l dodecile_RhoFluct.C
    [your output file dodecile_RhoFluct.root contains the desired data]
```

