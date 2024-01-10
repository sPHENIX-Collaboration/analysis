These are the instructions for running the code to generate lists of noisy towers 
and generate the corresponding CDBTTrees

Build code:

mkdir build
cd build
$PWD/../src/autogen.sh --prefix=${Whatever you've named your install environment variable}

head to macros directory

Directory structure for condor
     mkdir out out_hadd cdbMaps err err_hadd submission submission_hadd mkdir fileLists log log_hadd output

cd fileLists

Create file lists for data of interest

Create file list of filelists

ls -1 *.list > fileListofLists.list

cd ../

makeSingleFileList.csh

runHotTowermacroMultistage.csh fileLists/fileListofLists.list sigma

This will submit a condor job for each segment of each run to be processed individually.
Then it will hadd the output files after pass 0
Then it will submit a job per run, which is actually making the bad towers, don't delete the output files from the previous step!
Then it writes the cdbttrees to the cdbMaps directory. These are immediately ready for upload. 

Where sigma is the desired cutoff. The code currently employs it symmetrically but it can be asymmetric as well.

Toweridmacro
- Takes in a run from the infile (supplied as argument)
- Iterates through each event (supplied as argument)
- For each tower that exceeds the adc cut(supplied as argument) in an event, the tower registers a "hit" and its frequency is incremented
- A histogram is filled with the total number of hits for each tower throughout the total number of events
- The mode and Standard Deviation of the histogram are calculated. A cutoff frequency is determined by adding a set number of standard deviations (sigmas: supplied as argument) to the mode
- All towers that exceed (record more hits than) the cutoff frequency are listed by channel number in a TTree 
- The histogram containing the hits for each tower, and the TTree with hot channels are written to an outfile (supplied as argument)

Fun4All_EMCalBadTowerID.C
nEvents - Number of events to process
inFile - input file
cdbname - name of cdbttree output file
adccut_sg - minimum adc value for blocks containing sg-type fibers
adccut_k - minimum adc value for blocks containing k-type fibers
sigmas_lo - number of standard deviations below  mean before a tower is cold
sigmas_hi - number of standard deviations above mean before tower is noisy
SG_f - fiducial cut on tower hit frequency for sg-type fibers
Kur_f - fiducial cut on tower hit frequency for k-type fibers
region_f - fiducial cut on region hit frequency
pass - iteration
 
