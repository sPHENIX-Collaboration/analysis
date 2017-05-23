## Basic intro and how to get started

This analysis package is designed to be run over with the main sPHENIX G4 macro, Fun4All_G4_sPHENIX.C. When written, it was made to be run over dedicated PYTHIA direct photon-jet events that were output into HEPMC format, although in principle any input that works with Fun4All_G4_sPHENIX.C should work. 

In order to run the macro and include the PhotonJet analysis package, the following steps should be taken:

1. Execute the usual `autogen.sh` and `make install` building of the PhotonJet package, per the usual sPHENIX way.

2. Include the following lines of code in the Fun4All_G4_sPHENIX.C macro:
```
  gSystem->Load("libPhotonJet.so");
  PhotonJet * photjet = new PhotonJet("outputfilename.root");
  se->registerSubsystem(photjet);
```
where se is the Fun4AllServer instantiated at the beginning of the macro.

The output is then "outputfilename.root" and contains several trees which have the relevant information. The tree names should be reasonably self explanatory. Note that truth information for the photon-jet correlated pairs is located directly in that tree, i.e. in the tree "isophoton-jets" the truth jet pt for a given entry is _truthjetpt and the reconstructed jet pt is _recojetpt.

Of course one can add whatever they would like to the file PhotonJet.C to add leaves to these trees or any other thing they see fit. Happy photon-jetting!



## More detailed things (will continue adding as code develops)

There are a few public functions that one can set automatically, for example the minimum photon pT cut or the isolation cone size. 
The isolation cone algorithm is by default used; one can change this with the function use_isocone_algorithm. The isolation algorithm details are in the code, but it currently checks for reconstructed tracks and clusters within a cone size radius set by the user greater than 200 MeV in pT. One can also change the cone size used for the jets. The default size is R=0.3, and the DSTnodes can accomodate R=0.2 to R=0.8 in increments of 0.1.

There are two different truth trees; one is produced with the node "PHG4TruthInfoContainer" and the other is from the raw HEPMC files, i.e. the tree "truthtree" contains all of the PYTHIA output which includes parton info while the "truth_g4particles" is a more condensed truth tree without the parton information.


Additional functionality has been added to look at forward jets. Tracked jet capabilities were added as well; one can set the use of these in their macro. 

##Information on using the Trigger Emulator
A trigger emulator was added by the UC Boulder group under offline/packages/trigger/. Currently as of the end of May 2017 this is not included in the sPHENIX nightly build, so this package must be built locally by the individual if trigger capabilities are desired. Additionally the two files from this packge CaloTriggerInfo.h and CaloTriggerInfo.C must be included in the PhotonJet directory, as they are included in the header of PhotonJet.C. The PhotonJet Makefile won't compile without the local compilation of the trigger emulator package as it searches for these libraries. There is also a flag in the PhotonJet package which can be set to false if trigger capabilities are not required; this bypasses all triggering nodes/functions.  

