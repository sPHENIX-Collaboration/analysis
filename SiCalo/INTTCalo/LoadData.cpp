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
  evtTree->SetBranchAddress("scaledtrig",&scaledtrig);

  //data in caloTree
  caloTree->SetBranchAddress("energy",&energy);
  caloTree->SetBranchAddress("phi",&emc_phi);
  caloTree->SetBranchAddress("eta",&emc_eta);
  caloTree->SetBranchAddress("r",&emc_r);
  caloTree->SetBranchAddress("z",&emc_z);
  caloTree->SetBranchAddress("prob",&emc_prob);
  caloTree->SetBranchAddress("Calo_tower_ieta",&emc_tower_ieta);
  caloTree->SetBranchAddress("Calo_tower_iphi",&emc_tower_iphi);
  caloTree->SetBranchAddress("Calo_tower_e",&emc_tower_e);
  caloTree->SetBranchAddress("Calo_tower_x",&emc_tower_x);
  caloTree->SetBranchAddress("Calo_tower_y",&emc_tower_y);
  caloTree->SetBranchAddress("Calo_tower_z",&emc_tower_z);
  caloTree->SetBranchAddress("Calo_tower_r",  &emc_tower_r);
  caloTree->SetBranchAddress("Calo_tower_phi",&emc_tower_phi);
  caloTree->SetBranchAddress("Calo_tower_eta",&emc_tower_eta);
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
  topoTree->SetBranchAddress("clus_emcal_e",&top_emc_e);
  topoTree->SetBranchAddress("clus_ihcal_e",&top_ihc_e);
  topoTree->SetBranchAddress("clus_ohcal_e",&top_ohc_e);

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
  trackTree->SetBranchAddress("x0",&vtrk_x0);
  trackTree->SetBranchAddress("y0",&vtrk_y0);
  trackTree->SetBranchAddress("z0",&vtrk_z0);
  trackTree->SetBranchAddress("px0",&vtrk_px0);
  trackTree->SetBranchAddress("py0",&vtrk_py0);
  trackTree->SetBranchAddress("pz0",&vtrk_pz0);
  trackTree->SetBranchAddress("eta0",&vtrk_eta0);
  trackTree->SetBranchAddress("phi0",&vtrk_phi0);
  trackTree->SetBranchAddress("pt0",&vtrk_pt0);
  trackTree->SetBranchAddress("chi2ndf",&vtrk_chi2ndf);
  trackTree->SetBranchAddress("dxy",&vtrk_dxy);
  trackTree->SetBranchAddress("dz",&vtrk_dz);
  trackTree->SetBranchAddress("charge",&vtrk_charge);
  trackTree->SetBranchAddress("nmaps",&vtrk_nmaps);
  trackTree->SetBranchAddress("nintt",&vtrk_nintt);
  trackTree->SetBranchAddress("innerintt",&vtrk_innerintt);
  trackTree->SetBranchAddress("outerintt",&vtrk_outerintt);
  trackTree->SetBranchAddress("x_proj_emc",&vtrk_x_proj_emc);
  trackTree->SetBranchAddress("y_proj_emc",&vtrk_y_proj_emc);
  trackTree->SetBranchAddress("z_proj_emc",&vtrk_z_proj_emc);
  trackTree->SetBranchAddress("px_proj_emc",&vtrk_px_proj_emc);
  trackTree->SetBranchAddress("py_proj_emc",&vtrk_py_proj_emc);
  trackTree->SetBranchAddress("pz_proj_emc",&vtrk_pz_proj_emc);
  trackTree->SetBranchAddress("eta_proj_emc",&vtrk_eta_proj_emc);
  trackTree->SetBranchAddress("phi_proj_emc",&vtrk_phi_proj_emc);
  trackTree->SetBranchAddress("pt_proj_emc",&vtrk_pt_proj_emc);
  trackTree->SetBranchAddress("track_id_truth",&vtrk_track_id_truth);
  trackTree->SetBranchAddress("sicalo_pt",&vtrk_sicalo_pt);
  trackTree->SetBranchAddress("sicalo_phi",&vtrk_sicalo_phi);
  trackTree->SetBranchAddress("sicalo_eta",&vtrk_sicalo_eta);
  trackTree->SetBranchAddress("sicalo_dphi",&vtrk_sicalo_dphi);
  trackTree->SetBranchAddress("sicalo_dz",&vtrk_sicalo_dz);
  trackTree->SetBranchAddress("sicalo_emc_x",&vtrk_sicalo_emc_x);
  trackTree->SetBranchAddress("sicalo_emc_y",&vtrk_sicalo_emc_y);
  trackTree->SetBranchAddress("sicalo_emc_z",&vtrk_sicalo_emc_z);
  trackTree->SetBranchAddress("sicalo_emc_r",&vtrk_sicalo_emc_r);
  trackTree->SetBranchAddress("sicalo_emc_phi",&vtrk_sicalo_emc_phi);
  trackTree->SetBranchAddress("sicalo_emc_energy",&vtrk_sicalo_emc_energy);
  trackTree->SetBranchAddress("sicalo_emc_chi2",&vtrk_sicalo_emc_chi2);
  trackTree->SetBranchAddress("sicalo_emc_prob",&vtrk_sicalo_emc_prob);
  
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
  //  xBC=-0.018;
  //  yBC=0.126;
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

