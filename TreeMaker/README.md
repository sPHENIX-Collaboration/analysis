# This is TreeMaker (needs a new name), an sPHENIX analysis module
# It copies cluster and jet objects and makes new ones for the purpose of eventually applying different energy scales based on constituents

## These are the main analysis files

File name | description
--------- | -----------
TreeMaker.C | class implementation file
TreeMaker.h | class header file
TreeMakerBackgrounds.C | gets tower backgrounds and sets class variables (for tree output, holdover from DVP's code)
TreeMakerCopyAndMakeClusters.C | copies existing cluster object and fills a new one, algorithm will go here
TreeMakerCopyAndMakeJets.C | copires existing jet objects and makes a new one, algorithm will go here
TreeMakerCreateNode.C | creates new nodes for CopyAndMake functions to fill
TreeMakerGetJets.C | gets jets and sets class variables (for tree output)
TreeMakerInitializations.C | initializes the tree and class variables
TreeMakerLinkDef.h | standard LinkDef (ROOT)
TreeMakerUseFastJet.C | applies fast jet algorithm to list of clusters, regular and modified

## Additional basic PHENIX universe files

File name | description
--------- | -----------
Makefile.am | standard makefile
autogen.sh | standard autogen file
configure.in | standard configure file

## Macros to test the code

File name | description
--------- | -----------
run_macro.C | macro to run this class
run_macro_2.C | additional macro

## Additional files

File name | description
--------- | -----------

