**![My GitHub:](https://github.com/FrancescoVassalli/SinglePhoton/)**
**Truth Level Analysis**
The truthconversion package uses truth information to identify photon conversions and saves information about the conversion to a TTree. The conversions are then associated with RawClusters and the TreeMaker class makes a second TTree with cluster and tower information. The macro titled cluster_burner will run the package. It takes a DST input file and a title for the output trees. A run number can also be designated. If the user does not want both TTrees that can be edited in the macro. Most of the interface should be handled with the macro however if one wishes to change what it recored in the TTree this should be done in TruthConversionEval.C which is heavily commented. 

**Reco Level Analysis**
Reco level analysis is not possible yet. I am implementing the cuts used in ATLAS however not all the cuts are implented and none of them are tuned. To tune them I am making 2 additional TTrees with the truthconversion package. One of the trees has the cut info for all the truth conversion signal and the other has background data. I use TMVA to tune the cuts. However I am leaving in feburary 2019 and may not have time to tune the cuts, but I will make the Trees. The unfinished recoconversion package is not yet included here but [here is the WIP](https://github.com/FrancescoVassalli/SinglePhoton/tree/master/PhotonConversion). One of the main things that needs to be done for the reco package is making a vertexing algorithm for 2 tracks with photons as the initial guess.


**Background on Photon Conversion**
- I have been using ![this ATLAS paper](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf) as a reference. The relevent information starts on page 136 of the pdf

**Conversion Efficiency** 
- I first studied the exiting efficiency of the sPHENIX simulation to recover converted photons. These two plots were made on 12/3/18 and describe the effiency relative to radius and pT for single photon events using SvtxTrack reconstruction with no calorimeter data. This is also known as the inside out method. 
![pT](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf)
![radius](https://github.com/FrancescoVassalli/SinglePhoton/files/2742039/4layerRecoR.pdf)
It may be useful to compare this to the atlas paper.
Note that my photons were 5-30 GeV uniform distribution while the ATLAS study was fixed 20GeV.

- This package was presented to the simulations group in https://indico.bnl.gov/event/5447/
