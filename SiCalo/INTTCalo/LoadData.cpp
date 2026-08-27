#include <iostream>
#include <string>
#include "LoadData.h"

void LoadData(string filename) {
  TFile *file = TFile::Open(filename.c_str(),"READ");

  if(!file || file->IsZombie()) {
    cout << "Failed to open "<<filename.c_str();
    cout <<" QUIT!" <<endl;
    exit(0);
  }
  
  // connect to the TTrees in the nDST file
  evtTree       = dynamic_cast<TTree*>(file->Get("evtTree"));
  caloTree      = dynamic_cast<TTree*>(file->Get("caloTree"));
  SiClusTree    = dynamic_cast<TTree*>(file->Get("SiClusTree"));
  SiClusAllTree = dynamic_cast<TTree*>(file->Get("SiClusAllTree"));
  trackTree     = dynamic_cast<TTree*>(file->Get("trackTree"));
  topoTree      = dynamic_cast<TTree*>(file->Get("TopoClusTree"));
  truthTree     = dynamic_cast<TTree*>(file->Get("truthTree"));

  if(evtTree == nullptr) {
    cout << "There is no evtTree. QUIT"<<endl;
    exit(0);
  }		    
  if(truthTree == nullptr) {
    cout << "No truth info" <<endl;
  }
    
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
  //  SiClusAllTree->SetBranchAddress("Siclus_trackid",&a_trkid);
  SiClusAllTree->SetBranchAddress("Siclus_x",&a_si_x);
  SiClusAllTree->SetBranchAddress("Siclus_y",&a_si_y);
  SiClusAllTree->SetBranchAddress("Siclus_z",&a_si_z);
  SiClusAllTree->SetBranchAddress("Siclus_t",&a_si_t);
  
  //data in trackTree
  trackTree->SetBranchAddress("eta0",&eta0);
  trackTree->SetBranchAddress("phi0",&phi0);
  trackTree->SetBranchAddress("pt0",&pt0);
  trackTree->SetBranchAddress("x0",&vx0);
  trackTree->SetBranchAddress("y0",&vy0);
  trackTree->SetBranchAddress("z0",&vz0);

  //Simulaiton: data in truthTree
  if(truthTree != nullptr) {
    cout << "This is simulation. Load truth data"<<endl;
    truthTree->SetBranchAddress("truth_pid",&truth_pid);
    truthTree->SetBranchAddress("truth_px",&truth_px);
    truthTree->SetBranchAddress("truth_py",&truth_py);
    truthTree->SetBranchAddress("truth_pz",&truth_pz);
    truthTree->SetBranchAddress("truth_e",&truth_e);
    truthTree->SetBranchAddress("truth_x",&truth_x);
    truthTree->SetBranchAddress("truth_y",&truth_y);
    truthTree->SetBranchAddress("truth_z",&truth_z);
    truthTree->SetBranchAddress("truth_vtx_x",&truth_vtx_x);
    truthTree->SetBranchAddress("truth_vtx_y",&truth_vtx_y);
    truthTree->SetBranchAddress("truth_vtx_z",&truth_vtx_z);
  }

  // Set Beam center
  // For run53876
  xBC=-0.018;
  yBC=0.126;
  // For simulaiton data
  if(truthTree !=nullptr) {
    xBC = 0.0;
    yBC = 0.0;
  }
  cout << "Beam Center (xBC,yBC) = ("<<xBC<<","<<yBC<<")"<<endl;
}

void GetEvent(int i) {
  evtTree->GetEntry(i);
  caloTree->GetEntry(i);
  trackTree->GetEntry(i);
  SiClusTree->GetEntry(i);
  SiClusAllTree->GetEntry(i);
  topoTree->GetEntry(i);

  if(truthTree != nullptr) {
    //this is simulaiton
    truthTree->GetEntry(i);
  }
}

