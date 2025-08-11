prodscripts - just a couple of bash scripts. 

------------------------------------------------------

makelist_Catalog.bash - build list of 4-DSTs across all run2pp data for use in FFA
- loop over QM40 runs list, create 4 separate single-DST-type filelists for DST-types CALO, TRKR_CLUSTER, TRKR_SEED, TRKR_TRACKS
- loop through runs, then each of these 4 filelists, and find matching sets of 4 DST file names (over all runs & segments)
- write these DST filename quartets to ./LISTS/catlist_[date].list. This list can be used directly in condor.

------------------------------------------------------

findprod.bash - find out what productions are available for a given DST-type. Search keyed on run 53877.
- use CreateDstList.pl to list all available production tag names
- loop through all production tags and the 4 DST-types, and call CreateDstList.pl for each tag and type to count the number of segment rootfiles available for run 53877.
- write to the screen the production name, the DST-type, and number of run 53877 segment rootfiles available.

------------------------------------------------------

Notes:
- run 53877 has ~2600 segments (10000 trigger frames per segment)
- note that the number of events in DST_CALO is either 5:1 or 10:1 w.r.t. the number of events in the associated tracking DSTs.
	- ana468_2024p012_v001 CALO:TRACKS is  5:1  (525 CALO dsts for 53877)
	- ana492_2024p020_v005 CALO:TRACKS is 10:1  (259 CALO dsts for 53877)
	- ana502_2024p022_v001 CALO:TRACKS is 10:1  (259 CALO dsts for 53877)

------------------------------------------------------
