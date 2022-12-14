This module's primary purpose is to generate and store properties of clusters 
produced in the EMCal by single pi0's decaying to photons. There are two macros 
that take two separate types of inputs:

Fun4All_RunPi0ClusterAna.C: Takes file lists, specifically used is one created
by calling: CreateFileList.pl -type 14 -particle pi0 -pmin 2000 -pmax 20000

Fun4All_Pi0Gen_sPHENIX.C: Only takes the number of events and then uses the simple event generator to generate pi0's within the sPHENIX volume with the kinematic constraints provided by the user

Either way, the data from the pi0 decay is analyzed by the SubSysReco module 
pi0ClusterAna, which generates an ntuple with the following trees:

TreeTruthPi0 -> Show(0)
======> EVENT:0
 pi0E            = (vector<float>*)0x3749210
 pi0_phi         = (vector<float>*)0x2b597f0
 pi0_eta         = (vector<float>*)0x35b0270

TreeTruthPhoton -> Show(0)
======> EVENT:0
 pairAsym        = (vector<float>*)0x371fd30
 pairDeltaR      = (vector<float>*)0x378d710
 leadPhotonPhi   = (vector<float>*)0x2b5a1b0
 leadPhotonEta   = (vector<float>*)0x3790f50
 subleadPhotonPhi = (vector<float>*)0x35b8b80
 subleadPhotonEta = (vector<float>*)0x35afff0
 leadPhotonE     = (vector<float>*)0x3748ca0
 subLeadPhotonE  = (vector<float>*)0x3725150

TreeClusterTower -> Show(0)
======> EVENT:0
 towerEtaCEnter  = (vector<float>*)0x26860a0
 towerPhiCenter  = (vector<float>*)0x2c67c50
 towerEnergy     = (vector<float>*)0x2c59bf0
 clusterEta      = (vector<float>*)0x2c47230
 clusterPhi      = (vector<float>*)0x2c60760
 clusterE        = (vector<float>*)0x2c71640
 clusterChi2     = (vector<float>*)0x2c6d410
 clusterProb     = (vector<float>*)0x195dac0
 clusterNTowers  = (vector<float>*)0x1f5a9c0

Which contain information at the truth pi0, truth photon, and cluster+tower level
