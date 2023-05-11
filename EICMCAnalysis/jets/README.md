Compile:

g++ `root-config --cflags --glibs` -L$OFFLINE_MAIN/lib -I$OFFLINE_MAIN/include -leicsmear -lCGAL -lfastjet fill_jet_tree.C -ofill_jet_tree

Run:

./fill_jet_tree data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.root data/pythiaeRHIC/TREES/pythia.ep.20x250.RadCor=0.Q2gt1.50kevts.smear.root jettest.root

