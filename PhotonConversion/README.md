The truthconversion package uses truth information to identify photon conversions and saves information about the conversion to a TTree. The conversions are then associated with RawClusters and the TreeMaker class makes a second TTree with cluster and tower information. The macro titled cluster_burner will run the package. It takes a DST input file and a title for the output trees. A run number can also be designated. If the user does not want both TTrees that can be edited in the macro. 


**Background on Photon Conversion**
- I have been using ![this ATLAS paper](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf) as a reference. The relevent information starts on page 136 of the pdf

**Conversion Efficiency** 
- I first studied the exiting efficiency of the sPHENIX simulation to recover converted photons. These two plots were made on 12/3/18 and describe the effiency relative to radius and pT for single photon events using SvtxTrack reconstruction. This is also known as the inside out method. 
![pT](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecopT.pdf)
![radius](https://github.com/FrancescoVassalli/SinglePhoton/files/2742038/4layerRecoR.pdf)
It may be useful to compare this to the atlas paper.
Note that my photons were 5-30 GeV uniform distribution while the ATLAS study was fixed 20GeV.
