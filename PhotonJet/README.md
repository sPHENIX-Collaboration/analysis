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
The isolation cone algorithm is by default used; one can change this with the function use_isocone_algorithm. The isolation algorithm details
are in the code, but it currently checks for reconstructed tracks and clusters within a cone size radius set by the user greater than 200 
MeV in pT.  

