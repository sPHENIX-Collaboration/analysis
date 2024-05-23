# SCorrelatorResponseMaker [Under Construction]

A class to match truth to reconstructed jets and their constituents. This produces a "response tree" which maps the truth jets/constituents onto their reconstructed counterparts. This tree is then used by the "SCorrelatorFolder" class to derive corrections for the reconstructed n-point energy correlation strength functions.

Much like the [SEnergyCorrelator](https://github.com/ruse-traveler/SEnergyCorrelator) class, this will be designed in such a way that it can run in two modes: a **"standalone"** mode in which the class can be run in a ROOT macro on already produced [input](https://github.com/ruse-traveler/SCorrelatorJetTree), and a **"complex"** mode in which the class is run as a module on the sPHENIX node tree. 
