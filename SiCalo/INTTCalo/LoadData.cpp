#include "LoadData.h"

void LoadData(string filename) {
  TFile *file = TFile::Open(filename.c_str(),"READ");
  if(!file || file->IsZombie()) return;

  // connect to the TTrees in the nDST file
  evtTree       = dynamic_cast<TTree*>(file->Get("evtTree"));
  caloTree      = dynamic_cast<TTree*>(file->Get("caloTree"));
  SiClusTree    = dynamic_cast<TTree*>(file->Get("SiClusTree"));
  SiClusAllTree = dynamic_cast<TTree*>(file->Get("SiClusAllTree"));
  trackTree     = dynamic_cast<TTree*>(file->Get("trackTree"));
  topoTree      = dynamic_cast<TTree*>(file->Get("TopoClusTree"));

  //data in evtTree
  evtTree->SetBranchAddress("evt",&evt);
  evtTree->SetBranchAddress("xgvtx",&xgvtx);
  evtTree->SetBranchAddress("ygvtx",&ygvtx);
  evtTree->SetBranchAddress("zgvtx",&zgvtx);

  //data in caloTree
  caloTree->SetBranchAddress("energy",&energy);
  caloTree->SetBranchAddress("phi",&emc_phi);
  caloTree->SetBranchAddress("eta",&emc_eta);
  caloTree->SetBranchAddress("r",&emc_r);
  caloTree->SetBranchAddress("z",&emc_z);
  caloTree->SetBranchAddress("Calo_tower_ieta",&emc_tower_ieta);
  caloTree->SetBranchAddress("Calo_tower_iphi",&emc_tower_iphi);
  caloTree->SetBranchAddress("Calo_tower_e",&emc_tower_e);
  caloTree->SetBranchAddress("Calo_tower_x",&emc_tower_x);
  caloTree->SetBranchAddress("Calo_tower_y",&emc_tower_y);
  caloTree->SetBranchAddress("Calo_tower_z",&emc_tower_z);
  caloTree->SetBranchAddress("Calo_tower_r",  &emc_tower_r);
  caloTree->SetBranchAddress("Calo_tower_phi",&emc_tower_eta);
  caloTree->SetBranchAddress("Calo_tower_eta",&emc_tower_phi);
  caloTree->SetBranchAddress("Calo_tower_time",&emc_tower_time);
  
  /*
  caloTree->SetBranchAddress("Calo_Clus_energy",&energy);
  caloTree->SetBranchAddress("Calo_Clus_phi",&emc_phi);
  caloTree->SetBranchAddress("Calo_Clus_r",&emc_r);
  caloTree->SetBranchAddress("Calo_Clus_z",&emc_z);
  */
  
  //data in topoClusTree
  topoTree->SetBranchAddress("clus_e",&top_e);
  topoTree->SetBranchAddress("clus_x",&top_x);
  topoTree->SetBranchAddress("clus_y",&top_y);
  topoTree->SetBranchAddress("clus_z",&top_z);
  topoTree->SetBranchAddress("clus_r",&top_r);
  topoTree->SetBranchAddress("clus_eta",&top_eta);
  topoTree->SetBranchAddress("clus_z",&top_z);
  topoTree->SetBranchAddress("clus_phi",&top_phi);
  topoTree->SetBranchAddress("clus_emcal_e",&emc_e);
  topoTree->SetBranchAddress("clus_ihcal_e",&ihc_e);
  topoTree->SetBranchAddress("clus_ohcal_e",&ohc_e);

  //data in SiClusTree
  SiClusTree->SetBranchAddress("Siclus_layer",&layer);
  SiClusTree->SetBranchAddress("Siclus_trackid",&trkid);
  SiClusTree->SetBranchAddress("Siclus_x",&si_x);
  SiClusTree->SetBranchAddress("Siclus_y",&si_y);
  SiClusTree->SetBranchAddress("Siclus_z",&si_z);

  //data in SiClusAllTree
  SiClusAllTree->SetBranchAddress("Siclus_layer",&a_layer);
  SiClusAllTree->SetBranchAddress("Siclus_trackid",&a_trkid);
  SiClusAllTree->SetBranchAddress("Siclus_x",&a_si_x);
  SiClusAllTree->SetBranchAddress("Siclus_y",&a_si_y);
  SiClusAllTree->SetBranchAddress("Siclus_z",&a_si_z);
  
  //data in trackTree
  trackTree->SetBranchAddress("eta0",&eta0);
  trackTree->SetBranchAddress("phi0",&phi0);
  trackTree->SetBranchAddress("pt0",&pt0);
  trackTree->SetBranchAddress("x0",&vx0);
  trackTree->SetBranchAddress("y0",&vy0);
  trackTree->SetBranchAddress("z0",&vz0);
}

void GetEvent(int i) {
  evtTree->GetEntry(i);
  caloTree->GetEntry(i);
  trackTree->GetEntry(i);
  SiClusTree->GetEntry(i);
  SiClusAllTree->GetEntry(i);
  topoTree->GetEntry(i);
}

