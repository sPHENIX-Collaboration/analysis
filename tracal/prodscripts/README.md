prodscripts - just a couple of bash scripts. 

Once you have a local copy, 
```
	cd prodscripts
```
then you can make a file list:
```
	./makelist_Catalog.bash
```
this will create and populate a subdirectory called ./LISTS/.

If you want to see what production tags actually have useful
data in them (and which DSTs are there in each one), you can:
```
	./findprod.bash
```
This script is keyed on run 53877. if there is a CLUSTERS, SEED, TRACKS, or CALO
DST with some production tag for which rootfiles exist for run 53877, this script
will find the production tags.

------------------------------------------------------

makelist_Catalog.bash - build list of 4-DSTs across all run2pp data for use in FFA
- the latest production tags are used, or you can change what combination of tags you want to use in the user-parameter section near the top of the script.
- loop over QM40 runs list, create 4 separate single-DST-type filelists for DST-types CALO, TRKR_CLUSTER, TRKR_SEED, TRKR_TRACKS
- loop through runs, then each of these 4 filelists, and find matching sets of 4 DST file names (over all runs & segments)
- write these DST filename quartets to ./LISTS/catlist_[date].list. This list can be used directly in condor. Each line of this file has the following format:
```
0, TRACKS-dst-file-name, SEED-dst-file-name, CLUSTER-dst-file-name, CALO-dst-file-name 
```
------------------------------------------------------

findprod.bash - find out what productions are available for a given DST-type. Search keyed on run 53877.
- use CreateDstList.pl to list all available production tag names
- loop through all production tags and the 4 DST-types, and call CreateDstList.pl for each tag and type to count the number of segment rootfiles available for run 53877.
- write to the screen the production name, the DST-type, and number of run 53877 segment rootfiles available.

------------------------------------------------------

Notes:
- the scripts actually use myCreateDstList.pl in repo, which is the same as the library version except with two fewer printf's (unnecessary)
- run 53877 has ~2600 tracking segments
- note that the number of events in DST_CALO is either 5:1 or 10:1 w.r.t. the number of events in the associated tracking DSTs.
	- ana468_2024p012_v001 CALO:TRACKS is  5:1  (525 CALO dsts for 53877)
	- ana492_2024p020_v005 CALO:TRACKS is 10:1  (259 CALO dsts for 53877)
	- ana502_2024p022_v001 CALO:TRACKS is 10:1  (259 CALO dsts for 53877)

------------------------------------------------------
