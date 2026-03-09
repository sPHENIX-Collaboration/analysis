# IsotrackTreesAnalysis

Usage:

1. Copy src directory
2. Create a run folder named build directly outside of src directory
4. Issue "make" and then run the executable (./run_analysis)
5. There are many options for the executable, they are available by running ./run_analysis --help
6. Pipe in root files using flag --file /path/to/files after ./run_analysis

Add any new modules in the "processEvent(...)" and "processTrack(...)" functions.
A module with a single function should be added as a member function to the IsotrackTreesAnalysis class. The definition of the function should go in a separate \*.C file - it is not needed to include .h file.
