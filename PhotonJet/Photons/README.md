# Photon package

This analysis package has two modules associated with it. Photons identifies single photons and Forward_pi0s identifies netural pions in either the fsPHENIX arm or central arm (despite it's name, it can do both arms).

It can run with the default Fun4All_G4_sPHENIX.C or Fun4All_G4_fsPHENIX.C macro. The packages are only designed to run for single (or multi) particle simulations. The output trees are simply trees with either single photons or pi0 characteristics, with the truth information stored in the tree.

The analysis packages can be used with the following steps:

1. Execute the usual `autogen.sh` and `make install` building of the Photons package, per the usual sPHENIX way.

2. Include the following lines of code in the Fun4All_G4_sPHENIX.C macro:
```
  gSystem->Load("libPhotons.so");
  Photons *photons = new Photons("outputfilename.root");
  se->registerSubsystem(photons);
```
where se is the Fun4AllServer instantiated at the beginning of the macro. 

The outputfile will have the name "outputfilename.root" and have several trees in it, which are named based on their contents. 

## Embedding capabilities

The package was modified to run in HIJING embedded backgrounds. One simply needs to turn the switch on in the package:
```
photons->use_embedding(1); //1 == use embedding, 0 == don't use it
```
One may need to also set the appropriate embedding flags in the actual Fun4All macro as well
