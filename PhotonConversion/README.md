This is intened to run on RCF
**![My GitHub:](https://github.com/FrancescoVassalli/SinglePhoton/)**
The attached word doc has additional info
**Truth Level Analysis**
The truthconversion package has TruthConversionEval which uses truth information to identify photon conversions and saves information about the conversion to a TTree. The conversions are then associated with RawClusters. The ConversionClusterTreeMaker class makes a second TTree with cluster and tower information from the clusters identified by TruthConversionEval. The macro titled cluster_burner will run the package. It takes a DST input file and a title for the output file. A run number can also be designated. If the user does not want TTrees that can be edited in the macro. Most of the interface should be handled with the macro however if one wishes to change what variables are recoreded in the TTree this should be done in TruthConversionEval.C which is heavily commented. 

**Reco Level Analysis**
Reco level analysis is not possible yet. I am implementing the tracking cuts used in ATLAS  as well as additional calorimeter cuts. Not all the cuts are implented and they all need to be tuned. To tune them I have made additional TTrees with the truthconversion package. The Train folder has TMVA macros for tuning. It may require ![reading about TMVA](https://root.cern.ch/download/doc/tmva/TMVAUsersGuide.pdf) if you are unfamiliar. The trees have the cut info for all the truth conversion signal and seperatly there are trees of background data. One of the main things that needs to be done for the reco package is making a vertexing algorithm for 2 tracks with photons as the initial guess. The current code included in recoconversion represent outlines are not functional.

**Background on Photon Conversion**
- I have been using ![this ATLAS paper](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf) as a reference. The relevent information starts on page 136 of the pdf

**Conversion Efficiency** 
- I first studied the exiting efficiency of the sPHENIX simulation to recover converted photons. These two plots were made on 12/3/18 and describe the effiency relative to radius and pT for single photon events using SvtxTrack reconstruction with no calorimeter data. This is also known as the inside out method. 
![pT](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf)
![radius](https://github.com/FrancescoVassalli/SinglePhoton/files/2742039/4layerRecoR.pdf)
It may be useful to compare this to the atlas paper.
Note that my photons were 5-30 GeV uniform distribution while the ATLAS study was fixed 20GeV.

- This package was presented to the simulations group in https://indico.bnl.gov/event/5447/

**Macros**
 cluster2Plotter.C:example root macro for plotting some cluster information from the truth TTree
 backgroundPlotter.C:example root macro for plotting data from a background tree.
 cluster_burner.C: macro to run the truth package, needs to be linked to your G4 macros
