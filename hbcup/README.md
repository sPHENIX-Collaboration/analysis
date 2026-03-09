# Rough steps to generate simulations for a UPC analysis:

1. Generate starlight MC events, for all channels and decay modes that can contribute to the real data.
   Other event generators could be used.

2. Convert the starlight files to HEPMC so that they can be read by the sPHENIX simulation code.  

     StarLight_To_HepMc slight.out hepmc.dat

   You should of course replace hepmc.dat with a sensible name like starlight_cohjpsi_mumu.dat.

   The converter source code is in the sphenix repository at coresoftware/generators/sphstarlight/.
   (All of our code is in the sphenix repository so this will be assumed from now on).

3. Once you have the HEPMC files, you can now run the sphenix simulation on those events, as
   explained in the wiki and in the code documentation at

     https://github.com/sPHENIX-Collaboration/macros

   The output should be a G4sPHENIX.root file, which is the simulated DST.  It contains similar data
   to what you have from a DST from the real data.  Since this is a simulation, if you decide to, 
   you can also include all the "truth" information from the monte carlo event generator and geant output.

4. Analyze the DSTs just as you would a real DST.  The analysis code is at

     analysis/hbcup/

   The macro that drives the code is in macro/Fun4All_AnaUPC.C, and the source code for doing the analysis
   is src/AnaUPC.*

5. Derive background from hadronic collisions using Hijing.  One can get a low statistics estimate from MDC2 files,
   but adequate statistics can only be done from a custom production...  The analysis macro to use for this is
   Fun4All_hijbkg.C.
 
