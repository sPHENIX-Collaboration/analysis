This module is meant to study the sphenix emcal pi0 reconstruction efficiency. 
Single pi0's are fired into the emcal over a range of energies (0.5-20)
The output from the Fun4All simulation is then analyzed by pi0Efficiency.cc/h
And output histograms are generated containing information on the reconstructed
invariant mass and energy with different minimum energy cuts. 

Truth level information on the parent pi0's energy as well as
the decay photons energy asymmetry and opening angle as a function of the truth
pi0 energy are also available

The offline directory contains a single macro that takes in the output file
as its sole input and compute the efficiency. 
