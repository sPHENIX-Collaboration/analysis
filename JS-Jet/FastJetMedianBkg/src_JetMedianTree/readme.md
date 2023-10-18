To use this code:

 1. Make a build directory and compile the code
    ```
    $ mkdir build
    $ cd build
    $ ../autogen.sh [install_lib_location]
    $ make -j4 install
    $ cd ..
    ```
 2. Get the input files:
 ```
   $ cd macro/full_lists
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
   [inspect the outputs of the generated test file]
   $ cd ..
```
 4. When you are happy with step 3., make a directly to run over all the input file lists. If, for instance, you want jobs
    that will run 500 input files, with 10 files per job, then do:
 ```
   $ ./make_inp_lists.py 10 500
   $ cd jobs_10x500
   $ condor_submit ../condor_r04.job
```
