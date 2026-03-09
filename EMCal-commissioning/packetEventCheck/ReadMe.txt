These scripts allow you to use the "ddump" command to return the event number recorded by each packet for an EMCal run. 
 ddump_file_generator.sh - requires 3 arguments: an input .prdf file (the data you wish to check for desynchronization) and an output text file (both strings)
			   The script will write the output from each packet on a new line in the text file. Finally, the third parameter is a decimal, the percentile of the
			   events you wish to look at: inputting 0.98 will look at the event 98% of the way through the file.
 modecalc.sh		-  given a file in the format outputted from ddump_file_generator as its only argument, modecalc determines the mode of the data, and the percentage of 
			   entries that match the mode. It also returns the packet numbers of all entries that do not match the mode. These are likely to be bad packets
			   and are entered into the existing text file	
check_desynch.sh        - runs the above two shell scripts in tandem. The output is a text file with the even recorded by each packet, the mode, the percentage of packets that 
			   record the mode, and the indices of the packets that do not.
