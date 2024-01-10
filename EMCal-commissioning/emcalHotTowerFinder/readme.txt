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
 
