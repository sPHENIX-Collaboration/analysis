Toweridmacro
- Takes in a run from the infile (supplied as argument)
- Iterates through each event (supplied as argument)
- For each tower that exceeds the adc cut(supplied as argument) in an event, the tower registers a "hit" and its frequency is incremented
- A histogram is filled with the total number of hits for each tower throughout the total number of events
- The mode and Standard Deviation of the histogram are calculated. A cutoff frequency is determined by adding a set number of standard deviations (sigmas: supplied as argument) to the mode
- All towers that exceed (record more hits than) the cutoff frequency are listed by channel number in a TTree 
- The histogram containing the hits for each tower, and the TTree with hot channels are written to an outfile (supplied as argument)
 
