#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "TChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TSystem.h"
#include "TROOT.h"

void D0TruthSeparator()
{
  string inDir = "/sphenix/user/rosstom/analysis/HF-Particle/KFParticle_sPHENIX/Run40Acceptance082922/";
  TString truthFilePath;
  TString KFPFilePath;

  Float_t K_mass = .493677;
  Float_t Pi_mass = .139570;
   
  // Initialize new file for saving the separated D0/D0bar information
  TFile *newfile = new TFile("Run40_D0_Separated_091922.root","recreate");
  TTree *D0_tree = new TTree("D0_tree","D0_tree");
  TTree *D0bar_tree = new TTree("D0bar_tree","D0bar_tree");
  TTree *Background_tree = new TTree("Background_tree","Background_tree");

  Float_t outTrue_mother_px, outTrue_mother_py, outTrue_mother_pz, outTrue_mother_pE;
  Float_t outTrue_mother_pT, outTrue_mother_eta;
  Int_t outTrue_mother_barcode, outTrue_mother_PDG_ID;
  Float_t outTrue_positive_px, outTrue_positive_py, outTrue_positive_pz, outTrue_positive_pE;
  Float_t outTrue_negative_px, outTrue_negative_py, outTrue_negative_pz, outTrue_negative_pE;
  Float_t outTrue_positive_eta, outTrue_negative_eta;
  Int_t outTrue_positive_PDG_ID, outTrue_negative_PDG_ID;
  Float_t outKFP_D0_px, outKFP_D0_py, outKFP_D0_pz, outKFP_D0_pE;
  Float_t outKFP_D0_mass;
  Float_t outKFP_D0_decayTime, outKFP_D0_decayLength, outKFP_D0_DIRA, outKFP_D0_IP, outKFP_D0_IPchi2;
  Float_t outKFP_D0_pseudorapidity, outKFP_D0_rapidity;
  Float_t outKFP_positive_px, outKFP_positive_py, outKFP_positive_pz, outKFP_positive_pE;
  Float_t outKFP_negative_px, outKFP_negative_py, outKFP_negative_pz, outKFP_negative_pE;
  Float_t outKFP_positive_p, outKFP_negative_p;
  Float_t outKFP_positive_pseudorapidity, outKFP_negative_pseudorapidity;
  Int_t outKFP_positive_charge, outKFP_negative_charge;
  Float_t outKFP_positive_IP, outKFP_negative_IP;
  Float_t outKFP_positive_IPchi2, outKFP_negative_IPchi2;
  Float_t outKFP_track_1_track_2_DCA;
  Float_t outKFP_KpPm_invm, outKFP_KmPp_invm;

  D0_tree->Branch("outTrue_mother_px", &outTrue_mother_px,"outTrue_mother_px/F");
  D0_tree->Branch("outTrue_mother_py", &outTrue_mother_py,"outTrue_mother_py/F");
  D0_tree->Branch("outTrue_mother_pz", &outTrue_mother_pz,"outTrue_mother_pz/F");
  D0_tree->Branch("outTrue_mother_pE", &outTrue_mother_pE,"outTrue_mother_pE/F");
  D0_tree->Branch("outTrue_mother_pT", &outTrue_mother_pT,"outTrue_mother_pT/F");
  D0_tree->Branch("outTrue_mother_eta", &outTrue_mother_eta,"outTrue_mother_eta/F");
  D0_tree->Branch("outTrue_mother_barcode", &outTrue_mother_barcode,"outTrue_mother_barcode/I");
  D0_tree->Branch("outTrue_mother_PDG_ID", &outTrue_mother_PDG_ID,"outTrue_mother_PDG_ID/I");
  D0_tree->Branch("outTrue_positive_px", &outTrue_positive_px,"outTrue_positive_px/F");
  D0_tree->Branch("outTrue_positive_py", &outTrue_positive_py,"outTrue_positive_py/F");
  D0_tree->Branch("outTrue_positive_pz", &outTrue_positive_pz,"outTrue_positive_pz/F");
  D0_tree->Branch("outTrue_positive_pE", &outTrue_positive_pE,"outTrue_positive_pE/F");
  D0_tree->Branch("outTrue_negative_px", &outTrue_negative_px,"outTrue_negative_px/F");
  D0_tree->Branch("outTrue_negative_py", &outTrue_negative_py,"outTrue_negative_py/F");
  D0_tree->Branch("outTrue_negative_pz", &outTrue_negative_pz,"outTrue_negative_pz/F");
  D0_tree->Branch("outTrue_negative_pE", &outTrue_negative_pE,"outTrue_negative_pE/F");
  D0_tree->Branch("outTrue_positive_eta", &outTrue_positive_eta,"outTrue_positive_eta/F");
  D0_tree->Branch("outTrue_negative_eta", &outTrue_negative_eta,"outTrue_negative_eta/F");
  D0_tree->Branch("outTrue_positive_PDG_ID", &outTrue_positive_PDG_ID,"outTrue_positive_PDG_ID/I");
  D0_tree->Branch("outTrue_negative_PDG_ID", &outTrue_negative_PDG_ID,"outTrue_negative_PDG_ID/I"); 
  D0_tree->Branch("outKFP_D0_px", &outKFP_D0_px,"outKFP_D0_px/F");
  D0_tree->Branch("outKFP_D0_py", &outKFP_D0_py,"outKFP_D0_py/F");
  D0_tree->Branch("outKFP_D0_pz", &outKFP_D0_pz,"outKFP_D0_pz/F");
  D0_tree->Branch("outKFP_D0_pE", &outKFP_D0_pE,"outKFP_D0_pE/F");
  D0_tree->Branch("outKFP_D0_mass", &outKFP_D0_mass,"outKFP_D0_mass/F");
  D0_tree->Branch("outKFP_D0_decayTime", &outKFP_D0_decayTime,"outKFP_D0_decayTime/F");
  D0_tree->Branch("outKFP_D0_decayLength", &outKFP_D0_decayLength,"outKFP_D0_decayLength/F");
  D0_tree->Branch("outKFP_D0_DIRA", &outKFP_D0_DIRA,"outKFP_D0_DIRA/F");
  D0_tree->Branch("outKFP_D0_IP", &outKFP_D0_IP,"outKFP_D0_IP/F");
  D0_tree->Branch("outKFP_D0_IPchi2", &outKFP_D0_IPchi2,"outKFP_D0_IPchi2/F");
  D0_tree->Branch("outKFP_D0_pseudorapidity", &outKFP_D0_pseudorapidity,"outKFP_D0_pseudorapidity/F");
  D0_tree->Branch("outKFP_D0_rapidity", &outKFP_D0_rapidity,"outKFP_D0_rapidity/F");
  D0_tree->Branch("outKFP_positive_px", &outKFP_positive_px,"outKFP_positive_px/F");
  D0_tree->Branch("outKFP_positive_py", &outKFP_positive_py,"outKFP_positive_py/F");
  D0_tree->Branch("outKFP_positive_pz", &outKFP_positive_pz,"outKFP_positive_pz/F");
  D0_tree->Branch("outKFP_positive_pE", &outKFP_positive_pE,"outKFP_positive_pE/F");
  D0_tree->Branch("outKFP_negative_px", &outKFP_negative_px,"outKFP_negative_px/F");
  D0_tree->Branch("outKFP_negative_py", &outKFP_negative_py,"outKFP_negative_py/F");
  D0_tree->Branch("outKFP_negative_pz", &outKFP_negative_pz,"outKFP_negative_pz/F");
  D0_tree->Branch("outKFP_negative_pE", &outKFP_negative_pE,"outKFP_negative_pE/F");
  D0_tree->Branch("outKFP_positive_p", &outKFP_positive_p,"outKFP_positive_p/F");
  D0_tree->Branch("outKFP_negative_p", &outKFP_negative_p,"outKFP_negative_p/F");
  D0_tree->Branch("outKFP_positive_pseudorapidity", &outKFP_positive_pseudorapidity,"outKFP_positive_pseudorapidity/F");
  D0_tree->Branch("outKFP_negative_pseudorapidity", &outKFP_negative_pseudorapidity,"outKFP_negative_pseudorapidity/F");
  D0_tree->Branch("outKFP_positive_charge", &outKFP_positive_charge,"outKFP_positive_charge/I");
  D0_tree->Branch("outKFP_negative_charge", &outKFP_negative_charge,"outKFP_negative_charge/I");
  D0_tree->Branch("outKFP_positive_IP", &outKFP_positive_IP,"outKFP_positive_IP/F");
  D0_tree->Branch("outKFP_negative_IP", &outKFP_negative_IP,"outKFP_negative_IP/F");
  D0_tree->Branch("outKFP_positive_IPchi2", &outKFP_positive_IPchi2,"outKFP_positive_IPchi2/F");
  D0_tree->Branch("outKFP_negative_IPchi2", &outKFP_negative_IPchi2,"outKFP_negative_IPchi2/F");
  D0_tree->Branch("outKFP_track_1_track_2_DCA", &outKFP_track_1_track_2_DCA,"outKFP_track_1_track_2_DCA/F");
  D0_tree->Branch("outKFP_KpPm_invm", &outKFP_KpPm_invm,"outKFP_KpPm_invm/F");
  D0_tree->Branch("outKFP_KmPp_invm", &outKFP_KmPp_invm,"outKFP_KmPp_invm/F");
 
  D0bar_tree->Branch("outTrue_mother_px", &outTrue_mother_px,"outTrue_mother_px/F");
  D0bar_tree->Branch("outTrue_mother_py", &outTrue_mother_py,"outTrue_mother_py/F");
  D0bar_tree->Branch("outTrue_mother_pz", &outTrue_mother_pz,"outTrue_mother_pz/F");
  D0bar_tree->Branch("outTrue_mother_pE", &outTrue_mother_pE,"outTrue_mother_pE/F");
  D0bar_tree->Branch("outTrue_mother_pT", &outTrue_mother_pT,"outTrue_mother_pT/F");
  D0bar_tree->Branch("outTrue_mother_eta", &outTrue_mother_eta,"outTrue_mother_eta/F");
  D0bar_tree->Branch("outTrue_mother_barcode", &outTrue_mother_barcode,"outTrue_mother_barcode/I");
  D0bar_tree->Branch("outTrue_mother_PDG_ID", &outTrue_mother_PDG_ID,"outTrue_mother_PDG_ID/I");
  D0bar_tree->Branch("outTrue_positive_px", &outTrue_positive_px,"outTrue_positive_px/F");
  D0bar_tree->Branch("outTrue_positive_py", &outTrue_positive_py,"outTrue_positive_py/F");
  D0bar_tree->Branch("outTrue_positive_pz", &outTrue_positive_pz,"outTrue_positive_pz/F");
  D0bar_tree->Branch("outTrue_positive_pE", &outTrue_positive_pE,"outTrue_positive_pE/F");
  D0bar_tree->Branch("outTrue_negative_px", &outTrue_negative_px,"outTrue_negative_px/F");
  D0bar_tree->Branch("outTrue_negative_py", &outTrue_negative_py,"outTrue_negative_py/F");
  D0bar_tree->Branch("outTrue_negative_pz", &outTrue_negative_pz,"outTrue_negative_pz/F");
  D0bar_tree->Branch("outTrue_negative_pE", &outTrue_negative_pE,"outTrue_negative_pE/F");
  D0bar_tree->Branch("outTrue_positive_eta", &outTrue_positive_eta,"outTrue_positive_eta/F");
  D0bar_tree->Branch("outTrue_negative_eta", &outTrue_negative_eta,"outTrue_negative_eta/F");
  D0bar_tree->Branch("outTrue_positive_PDG_ID", &outTrue_positive_PDG_ID,"outTrue_positive_PDG_ID/I");
  D0bar_tree->Branch("outTrue_negative_PDG_ID", &outTrue_negative_PDG_ID,"outTrue_negative_PDG_ID/I"); 
  D0bar_tree->Branch("outKFP_D0_px", &outKFP_D0_px,"outKFP_D0_px/F");
  D0bar_tree->Branch("outKFP_D0_py", &outKFP_D0_py,"outKFP_D0_py/F");
  D0bar_tree->Branch("outKFP_D0_pz", &outKFP_D0_pz,"outKFP_D0_pz/F");
  D0bar_tree->Branch("outKFP_D0_pE", &outKFP_D0_pE,"outKFP_D0_pE/F");
  D0bar_tree->Branch("outKFP_D0_mass", &outKFP_D0_mass,"outKFP_D0_mass/F");
  D0bar_tree->Branch("outKFP_D0_decayTime", &outKFP_D0_decayTime,"outKFP_D0_decayTime/F");
  D0bar_tree->Branch("outKFP_D0_decayLength", &outKFP_D0_decayLength,"outKFP_D0_decayLength/F");
  D0bar_tree->Branch("outKFP_D0_DIRA", &outKFP_D0_DIRA,"outKFP_D0_DIRA/F");
  D0bar_tree->Branch("outKFP_D0_IP", &outKFP_D0_IP,"outKFP_D0_IP/F");
  D0bar_tree->Branch("outKFP_D0_IPchi2", &outKFP_D0_IPchi2,"outKFP_D0_IPchi2/F");
  D0bar_tree->Branch("outKFP_D0_pseudorapidity", &outKFP_D0_pseudorapidity,"outKFP_D0_pseudorapidity/F");
  D0bar_tree->Branch("outKFP_D0_rapidity", &outKFP_D0_rapidity,"outKFP_D0_rapidity/F");
  D0bar_tree->Branch("outKFP_positive_px", &outKFP_positive_px,"outKFP_positive_px/F");
  D0bar_tree->Branch("outKFP_positive_py", &outKFP_positive_py,"outKFP_positive_py/F");
  D0bar_tree->Branch("outKFP_positive_pz", &outKFP_positive_pz,"outKFP_positive_pz/F");
  D0bar_tree->Branch("outKFP_positive_pE", &outKFP_positive_pE,"outKFP_positive_pE/F");
  D0bar_tree->Branch("outKFP_negative_px", &outKFP_negative_px,"outKFP_negative_px/F");
  D0bar_tree->Branch("outKFP_negative_py", &outKFP_negative_py,"outKFP_negative_py/F");
  D0bar_tree->Branch("outKFP_negative_pz", &outKFP_negative_pz,"outKFP_negative_pz/F");
  D0bar_tree->Branch("outKFP_negative_pE", &outKFP_negative_pE,"outKFP_negative_pE/F");
  D0bar_tree->Branch("outKFP_positive_p", &outKFP_positive_p,"outKFP_positive_p/F");
  D0bar_tree->Branch("outKFP_negative_p", &outKFP_negative_p,"outKFP_negative_p/F");
  D0bar_tree->Branch("outKFP_positive_pseudorapidity", &outKFP_positive_pseudorapidity,"outKFP_positive_pseudorapidity/F");
  D0bar_tree->Branch("outKFP_negative_pseudorapidity", &outKFP_negative_pseudorapidity,"outKFP_negative_pseudorapidity/F");
  D0bar_tree->Branch("outKFP_positive_charge", &outKFP_positive_charge,"outKFP_positive_charge/I");
  D0bar_tree->Branch("outKFP_negative_charge", &outKFP_negative_charge,"outKFP_negative_charge/I");
  D0bar_tree->Branch("outKFP_positive_IP", &outKFP_positive_IP,"outKFP_positive_IP/F");
  D0bar_tree->Branch("outKFP_negative_IP", &outKFP_negative_IP,"outKFP_negative_IP/F");
  D0bar_tree->Branch("outKFP_positive_IPchi2", &outKFP_positive_IPchi2,"outKFP_positive_IPchi2/F");
  D0bar_tree->Branch("outKFP_negative_IPchi2", &outKFP_negative_IPchi2,"outKFP_negative_IPchi2/F");
  D0bar_tree->Branch("outKFP_track_1_track_2_DCA", &outKFP_track_1_track_2_DCA,"outKFP_track_1_track_2_DCA/F");
  D0bar_tree->Branch("outKFP_KpPm_invm", &outKFP_KpPm_invm,"outKFP_KpPm_invm/F");
  D0bar_tree->Branch("outKFP_KmPp_invm", &outKFP_KmPp_invm,"outKFP_KmPp_invm/F");
  
  //Background_tree->Branch("outTrue_mother_px", &outTrue_mother_px,"outTrue_mother_px/F");
  //Background_tree->Branch("outTrue_mother_py", &outTrue_mother_py,"outTrue_mother_py/F");
  //Background_tree->Branch("outTrue_mother_pz", &outTrue_mother_pz,"outTrue_mother_pz/F");
  //Background_tree->Branch("outTrue_mother_pE", &outTrue_mother_pE,"outTrue_mother_pE/F");
  //Background_tree->Branch("outTrue_mother_pT", &outTrue_mother_pT,"outTrue_mother_pT/F");
  //Background_tree->Branch("outTrue_mother_eta", &outTrue_mother_eta,"outTrue_mother_eta/F");
  //Background_tree->Branch("outTrue_mother_barcode", &outTrue_mother_barcode,"outTrue_mother_barcode/I");
  //Background_tree->Branch("outTrue_track_1_px", &outTrue_track_1_px,"outTrue_track_1_px/F");
  //Background_tree->Branch("outTrue_track_1_py", &outTrue_track_1_py,"outTrue_track_1_py/F");
  //Background_tree->Branch("outTrue_track_1_pz", &outTrue_track_1_pz,"outTrue_track_1_pz/F");
  //Background_tree->Branch("outTrue_track_1_pE", &outTrue_track_1_pE,"outTrue_track_1_pE/F");
  //Background_tree->Branch("outTrue_track_2_px", &outTrue_track_2_px,"outTrue_track_2_px/F");
  //Background_tree->Branch("outTrue_track_2_py", &outTrue_track_2_py,"outTrue_track_2_py/F");
  //Background_tree->Branch("outTrue_track_2_pz", &outTrue_track_2_pz,"outTrue_track_2_pz/F");
  //Background_tree->Branch("outTrue_track_2_pE", &outTrue_track_2_pE,"outTrue_track_2_pE/F");
  //Background_tree->Branch("outTrue_track_1_eta", &outTrue_track_1_eta,"outTrue_track_1_eta/F");
  //Background_tree->Branch("outTrue_track_2_eta", &outTrue_track_2_eta,"outTrue_track_2_eta/F");
  //Background_tree->Branch("outTrue_track_1_PDG_ID", &outTrue_track_1_PDG_ID,"outTrue_track_1_PDG_ID/I");
  //Background_tree->Branch("outTrue_track_2_PDG_ID", &outTrue_track_2_PDG_ID,"outTrue_track_2_PDG_ID/I"); 
  Background_tree->Branch("outKFP_D0_px", &outKFP_D0_px,"outKFP_D0_px/F");
  Background_tree->Branch("outKFP_D0_py", &outKFP_D0_py,"outKFP_D0_py/F");
  Background_tree->Branch("outKFP_D0_pz", &outKFP_D0_pz,"outKFP_D0_pz/F");
  Background_tree->Branch("outKFP_D0_pE", &outKFP_D0_pE,"outKFP_D0_pE/F");
  Background_tree->Branch("outKFP_D0_mass", &outKFP_D0_mass,"outKFP_D0_mass/F");
  Background_tree->Branch("outKFP_D0_decayTime", &outKFP_D0_decayTime,"outKFP_D0_decayTime/F");
  Background_tree->Branch("outKFP_D0_decayLength", &outKFP_D0_decayLength,"outKFP_D0_decayLength/F");
  Background_tree->Branch("outKFP_D0_DIRA", &outKFP_D0_DIRA,"outKFP_D0_DIRA/F");
  Background_tree->Branch("outKFP_D0_IP", &outKFP_D0_IP,"outKFP_D0_IP/F");
  Background_tree->Branch("outKFP_D0_IPchi2", &outKFP_D0_IPchi2,"outKFP_D0_IPchi2/F");
  Background_tree->Branch("outKFP_D0_pseudorapidity", &outKFP_D0_pseudorapidity,"outKFP_D0_pseudorapidity/F");
  Background_tree->Branch("outKFP_D0_rapidity", &outKFP_D0_rapidity,"outKFP_D0_rapidity/F");
  Background_tree->Branch("outKFP_positive_px", &outKFP_positive_px,"outKFP_positive_px/F");
  Background_tree->Branch("outKFP_positive_py", &outKFP_positive_py,"outKFP_positive_py/F");
  Background_tree->Branch("outKFP_positive_pz", &outKFP_positive_pz,"outKFP_positive_pz/F");
  Background_tree->Branch("outKFP_positive_pE", &outKFP_positive_pE,"outKFP_positive_pE/F");
  Background_tree->Branch("outKFP_negative_px", &outKFP_negative_px,"outKFP_negative_px/F");
  Background_tree->Branch("outKFP_negative_py", &outKFP_negative_py,"outKFP_negative_py/F");
  Background_tree->Branch("outKFP_negative_pz", &outKFP_negative_pz,"outKFP_negative_pz/F");
  Background_tree->Branch("outKFP_negative_pE", &outKFP_negative_pE,"outKFP_negative_pE/F");
  Background_tree->Branch("outKFP_positive_p", &outKFP_positive_p,"outKFP_positive_p/F");
  Background_tree->Branch("outKFP_negative_p", &outKFP_negative_p,"outKFP_negative_p/F");
  Background_tree->Branch("outKFP_positive_pseudorapidity", &outKFP_positive_pseudorapidity,"outKFP_positive_pseudorapidity/F");
  Background_tree->Branch("outKFP_negative_pseudorapidity", &outKFP_negative_pseudorapidity,"outKFP_negative_pseudorapidity/F");
  Background_tree->Branch("outKFP_positive_charge", &outKFP_positive_charge,"outKFP_positive_charge/I");
  Background_tree->Branch("outKFP_negative_charge", &outKFP_negative_charge,"outKFP_negative_charge/I");
  Background_tree->Branch("outKFP_positive_IP", &outKFP_positive_IP,"outKFP_positive_IP/F");
  Background_tree->Branch("outKFP_negative_IP", &outKFP_negative_IP,"outKFP_negative_IP/F");
  Background_tree->Branch("outKFP_positive_IPchi2", &outKFP_positive_IPchi2,"outKFP_positive_IPchi2/F");
  Background_tree->Branch("outKFP_negative_IPchi2", &outKFP_negative_IPchi2,"outKFP_negative_IPchi2/F");
  Background_tree->Branch("outKFP_track_1_track_2_DCA", &outKFP_track_1_track_2_DCA,"outKFP_track_1_track_2_DCA/F");
  Background_tree->Branch("outKFP_KpPm_invm", &outKFP_KpPm_invm,"outKFP_KpPm_invm/F");
  Background_tree->Branch("outKFP_KmPp_invm", &outKFP_KmPp_invm,"outKFP_KmPp_invm/F");

  for (int i = 1; i < 237; ++i)
  {
    std::cout << "Starting File Number: " << i << std::endl;
    string fNumb;
    if (i < 10)
    {
      fNumb = "00" + std::to_string(i);
    }
    else if (i < 100)
    {
      fNumb = "0" + std::to_string(i);
    }
    else
    {
      fNumb = std::to_string(i);
    }

    string truthFile = "outputData_Run40Acceptance082922_00" + fNumb + ".root";
    string KFPFile = "outputData_Run40Acceptance082922_KFP_00" + fNumb + ".root";
    truthFilePath = inDir + truthFile;
    KFPFilePath = inDir + KFPFile;

    TFile *truthInput(0);
    truthInput = TFile::Open(truthFilePath);
    TTree *truthTree = (TTree*)truthInput->Get("QAG4SimulationTruthDecay");
    TFile *KFPInput(0);
    KFPInput = TFile::Open(KFPFilePath);
    TTree *recoTree = (TTree*)KFPInput->Get("DecayTree");

    Float_t truth_mother_px, truth_mother_py, truth_mother_pz, truth_mother_pE;
    Float_t truth_mother_pT, truth_mother_eta;
    Int_t truth_mother_barcode, truth_mother_PDG_ID;
    Float_t truth_track_1_px, truth_track_1_py, truth_track_1_pz, truth_track_1_pE;
    Float_t truth_track_2_px, truth_track_2_py, truth_track_2_pz, truth_track_2_pE;
    Float_t truth_track_1_eta, truth_track_2_eta;
    Int_t truth_track_1_PDG_ID, truth_track_2_PDG_ID;
    Float_t kfp_D0_px, kfp_D0_py, kfp_D0_pz, kfp_D0_pE;
    Float_t kfp_D0_mass;
    Float_t kfp_D0_decayTime, kfp_D0_decayLength, kfp_D0_DIRA, kfp_D0_IP, kfp_D0_IPchi2;
    Float_t kfp_D0_pseudorapidity, kfp_D0_rapidity;
    Float_t kfp_track_1_px, kfp_track_1_py, kfp_track_1_pz, kfp_track_1_pE;
    Float_t kfp_track_2_px, kfp_track_2_py, kfp_track_2_pz, kfp_track_2_pE;
    Float_t kfp_track_1_p, kfp_track_2_p;
    Float_t kfp_track_1_true_px, kfp_track_1_true_py, kfp_track_1_true_pz;
    Float_t kfp_track_2_true_px, kfp_track_2_true_py, kfp_track_2_true_pz;
    Float_t kfp_track_1_pseudorapidity, kfp_track_2_pseudorapidity;
    Int_t kfp_track_1_charge, kfp_track_2_charge;
    Float_t kfp_track_1_IP, kfp_track_2_IP;
    Float_t kfp_track_1_IPchi2, kfp_track_2_IPchi2;
    Float_t kfp_track_1_track_2_DCA;

    truthTree->SetBranchAddress("mother_px", &truth_mother_px);
    truthTree->SetBranchAddress("mother_py", &truth_mother_py);
    truthTree->SetBranchAddress("mother_pz", &truth_mother_pz);
    truthTree->SetBranchAddress("mother_pE", &truth_mother_pE);
    truthTree->SetBranchAddress("mother_pT", &truth_mother_pT);
    truthTree->SetBranchAddress("mother_eta", &truth_mother_eta);
    truthTree->SetBranchAddress("mother_barcode", &truth_mother_barcode);
    truthTree->SetBranchAddress("mother_PDG_ID", &truth_mother_PDG_ID);
    truthTree->SetBranchAddress("track_1_px", &truth_track_1_px);
    truthTree->SetBranchAddress("track_1_py", &truth_track_1_py);
    truthTree->SetBranchAddress("track_1_pz", &truth_track_1_pz);
    truthTree->SetBranchAddress("track_1_pE", &truth_track_1_pE);
    truthTree->SetBranchAddress("track_2_px", &truth_track_2_px);
    truthTree->SetBranchAddress("track_2_py", &truth_track_2_py);
    truthTree->SetBranchAddress("track_2_pz", &truth_track_2_pz);
    truthTree->SetBranchAddress("track_2_pE", &truth_track_2_pE);
    truthTree->SetBranchAddress("track_1_eta", &truth_track_1_eta);
    truthTree->SetBranchAddress("track_2_eta", &truth_track_2_eta);
    truthTree->SetBranchAddress("track_1_PDG_ID", &truth_track_1_PDG_ID);
    truthTree->SetBranchAddress("track_2_PDG_ID", &truth_track_2_PDG_ID);

    recoTree->SetBranchAddress("D0_px", &kfp_D0_px);
    recoTree->SetBranchAddress("D0_py", &kfp_D0_py);
    recoTree->SetBranchAddress("D0_pz", &kfp_D0_pz);
    recoTree->SetBranchAddress("D0_pE", &kfp_D0_pE);
    recoTree->SetBranchAddress("D0_mass", &kfp_D0_mass);
    recoTree->SetBranchAddress("D0_decayTime", &kfp_D0_decayTime);
    recoTree->SetBranchAddress("D0_decayLength", &kfp_D0_decayLength);
    recoTree->SetBranchAddress("D0_DIRA", &kfp_D0_DIRA);
    recoTree->SetBranchAddress("D0_IP", &kfp_D0_IP);
    recoTree->SetBranchAddress("D0_IPchi2", &kfp_D0_IPchi2);
    recoTree->SetBranchAddress("D0_pseudorapidity", &kfp_D0_pseudorapidity);
    recoTree->SetBranchAddress("D0_rapidity", &kfp_D0_rapidity);
    recoTree->SetBranchAddress("track_1_px", &kfp_track_1_px);
    recoTree->SetBranchAddress("track_1_py", &kfp_track_1_py);
    recoTree->SetBranchAddress("track_1_pz", &kfp_track_1_pz);
    recoTree->SetBranchAddress("track_1_pE", &kfp_track_1_pE);
    recoTree->SetBranchAddress("track_2_px", &kfp_track_2_px);
    recoTree->SetBranchAddress("track_2_py", &kfp_track_2_py);
    recoTree->SetBranchAddress("track_2_pz", &kfp_track_2_pz);
    recoTree->SetBranchAddress("track_2_pE", &kfp_track_2_pE);
    recoTree->SetBranchAddress("track_1_p", &kfp_track_1_p);
    recoTree->SetBranchAddress("track_2_p", &kfp_track_2_p);
    recoTree->SetBranchAddress("track_1_true_px", &kfp_track_1_true_px);
    recoTree->SetBranchAddress("track_1_true_py", &kfp_track_1_true_py);
    recoTree->SetBranchAddress("track_1_true_pz", &kfp_track_1_true_pz);
    recoTree->SetBranchAddress("track_2_true_px", &kfp_track_2_true_px);
    recoTree->SetBranchAddress("track_2_true_py", &kfp_track_2_true_py);
    recoTree->SetBranchAddress("track_2_true_pz", &kfp_track_2_true_pz);
    recoTree->SetBranchAddress("track_1_pseudorapidity", &kfp_track_1_pseudorapidity);
    recoTree->SetBranchAddress("track_2_pseudorapidity", &kfp_track_2_pseudorapidity);
    recoTree->SetBranchAddress("track_1_charge", &kfp_track_1_charge);
    recoTree->SetBranchAddress("track_2_charge", &kfp_track_2_charge);
    recoTree->SetBranchAddress("track_1_IP", &kfp_track_1_IP);
    recoTree->SetBranchAddress("track_2_IP", &kfp_track_2_IP);
    recoTree->SetBranchAddress("track_1_IPchi2", &kfp_track_1_IPchi2);
    recoTree->SetBranchAddress("track_2_IPchi2", &kfp_track_2_IPchi2);
    recoTree->SetBranchAddress("track_1_track_2_DCA", &kfp_track_1_track_2_DCA);
    
    vector<Int_t> reconstructedBarcodes;
    vector<Int_t> missedBarcodes;
    vector<int> usedRecoEntries;
  
    int minEntry;  
 
    for (int j = 0; j < truthTree->GetEntries(); ++j)
    {
      truthTree->GetEntry(j);

      outTrue_mother_px = 0; outTrue_mother_py = 0; outTrue_mother_pz = 0; outTrue_mother_pE = 0;
      outTrue_mother_pT = 0; outTrue_mother_eta = 0;
      outTrue_mother_barcode = 0; outTrue_mother_PDG_ID = 0;
      outTrue_positive_px = 0; outTrue_positive_py = 0; outTrue_positive_pz = 0; outTrue_positive_pE = 0;
      outTrue_negative_px = 0; outTrue_negative_py = 0; outTrue_negative_pz = 0; outTrue_negative_pE = 0;
      outTrue_positive_eta = 0; outTrue_negative_eta = 0;
      outTrue_positive_PDG_ID = 0; outTrue_negative_PDG_ID = 0;
      outKFP_D0_px = 0; outKFP_D0_py = 0; outKFP_D0_pz = 0; outKFP_D0_pE = 0;
      outKFP_D0_mass = 0;
      outKFP_D0_decayTime = 0; outKFP_D0_decayLength = 0; outKFP_D0_DIRA = 0; outKFP_D0_IP = 0; outKFP_D0_IPchi2 = 0;
      outKFP_D0_pseudorapidity = 0; outKFP_D0_rapidity = 0;
      outKFP_positive_px = 0; outKFP_positive_py = 0; outKFP_positive_pz = 0; outKFP_positive_pE = 0;
      outKFP_negative_px = 0; outKFP_negative_py = 0; outKFP_negative_pz = 0; outKFP_negative_pE = 0;
      outKFP_positive_p = 0; outKFP_negative_p = 0;
      outKFP_positive_pseudorapidity = 0; outKFP_negative_pseudorapidity = 0;
      outKFP_positive_charge = 0; outKFP_negative_charge = 0;
      outKFP_positive_IP = 0; outKFP_negative_IP = 0;
      outKFP_positive_IPchi2 = 0; outKFP_negative_IPchi2 = 0;
      outKFP_track_1_track_2_DCA = 0;
      outKFP_KpPm_invm = 0; outKFP_KmPp_invm = 0;

      bool matchPx_t1r1;
      bool matchPy_t1r1;
      bool matchPz_t1r1;
      bool matchPx_t1r2;
      bool matchPy_t1r2;
      bool matchPz_t1r2;
      bool matchPx_t2r1;
      bool matchPy_t2r1;
      bool matchPz_t2r1;
      bool matchPx_t2r2;
      bool matchPy_t2r2;
      bool matchPz_t2r2;
    
      bool match_t1r1_t2r2 = false;
      bool match_t2r1_t1r2 = false;
       
      for (int k = 0; k < recoTree->GetEntries(); ++k)
      {
        recoTree->GetEntry(k); 

        matchPx_t1r1 = (kfp_track_1_true_px == truth_track_1_px);
        matchPy_t1r1 = (kfp_track_1_true_py == truth_track_1_py);
        matchPz_t1r1 = (kfp_track_1_true_pz == truth_track_1_pz);
        matchPx_t1r2 = (kfp_track_2_true_px == truth_track_1_px);
        matchPy_t1r2 = (kfp_track_2_true_py == truth_track_1_py);
        matchPz_t1r2 = (kfp_track_2_true_pz == truth_track_1_pz);
        matchPx_t2r1 = (kfp_track_1_true_px == truth_track_2_px);
        matchPy_t2r1 = (kfp_track_1_true_py == truth_track_2_py);
        matchPz_t2r1 = (kfp_track_1_true_pz == truth_track_2_pz);
        matchPx_t2r2 = (kfp_track_2_true_px == truth_track_2_px);
        matchPy_t2r2 = (kfp_track_2_true_py == truth_track_2_py);
        matchPz_t2r2 = (kfp_track_2_true_pz == truth_track_2_pz); 
        
        if (matchPx_t1r1 && matchPy_t1r1 && matchPz_t1r1)
        {
          if (matchPx_t2r2 && matchPy_t2r2 && matchPz_t2r2)
          {
            std::cout << "Got a match" << std::endl;
            match_t1r1_t2r2 = true;
            minEntry = k;
            break;
          }
        }       
        else if (matchPx_t1r2 && matchPy_t1r2 && matchPz_t1r2)
        {
          if (matchPx_t2r1 && matchPy_t2r1 && matchPz_t2r1)
          {
            std::cout << "Got a match" << std::endl;
            match_t2r1_t1r2 = true;
            minEntry = k;
            break;
          }
        }       
      }
      if (match_t1r1_t2r2)
      {
        if (kfp_track_1_charge > 0 && kfp_track_2_charge < 0)
        {
          outTrue_mother_px = truth_mother_px; outTrue_mother_py = truth_mother_py; outTrue_mother_pz = truth_mother_pz; outTrue_mother_pE = truth_mother_pE;
          outTrue_mother_pT = truth_mother_pT; outTrue_mother_eta = truth_mother_eta;
          outTrue_mother_barcode = truth_mother_barcode; outTrue_mother_PDG_ID = truth_mother_PDG_ID;
          outTrue_positive_px = truth_track_1_px; outTrue_positive_py = truth_track_1_py; outTrue_positive_pz = truth_track_1_pz; outTrue_positive_pE = truth_track_1_pE;
          outTrue_negative_px = truth_track_2_px; outTrue_negative_py = truth_track_2_py; outTrue_negative_pz = truth_track_2_pz; outTrue_negative_pE = truth_track_2_pE;
          outTrue_positive_eta = truth_track_1_eta; outTrue_negative_eta = truth_track_2_eta;
          outTrue_positive_PDG_ID = truth_track_1_PDG_ID; outTrue_negative_PDG_ID = truth_track_2_PDG_ID;
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_positive_px = kfp_track_1_px; outKFP_positive_py = kfp_track_1_py; outKFP_positive_pz = kfp_track_1_pz; outKFP_positive_pE = kfp_track_1_pE;
          outKFP_negative_px = kfp_track_2_px; outKFP_negative_py = kfp_track_2_py; outKFP_negative_pz = kfp_track_2_pz; outKFP_negative_pE = kfp_track_2_pE;
          outKFP_positive_p = kfp_track_1_p; outKFP_negative_p = kfp_track_2_p;
          outKFP_positive_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_negative_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_positive_charge = kfp_track_1_charge; outKFP_negative_charge = kfp_track_2_charge;
          outKFP_positive_IP = kfp_track_1_IP; outKFP_negative_IP = kfp_track_2_IP;
          outKFP_positive_IPchi2 = kfp_track_1_IPchi2; outKFP_negative_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA; 

          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
 
          if (usedRecoEntries.size() > 0)
          {
            bool recoCandidateUsed = false;
            for (int ent : usedRecoEntries)
            {
              if (ent == minEntry)
	      {
                std::cout << "Candidate has already been used, skipping this candidate" << std::endl;
                recoCandidateUsed = true;
   	      } 
            }
            if (recoCandidateUsed) continue;
            else
            {
              usedRecoEntries.push_back(minEntry);
              // D0 or D0bar check here 
              if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
              else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
              else
              {
                Background_tree->Fill();
                std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
              }
            }         
          }
          else
          {
            usedRecoEntries.push_back(minEntry);

            // D0 or D0bar check here 
            if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
            else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
            else
            {
              Background_tree->Fill();
              std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
            }
          }
        }
        else if (kfp_track_1_charge < 0 && kfp_track_2_charge > 0)
        {
          outTrue_mother_px = truth_mother_px; outTrue_mother_py = truth_mother_py; outTrue_mother_pz = truth_mother_pz; outTrue_mother_pE = truth_mother_pE;
          outTrue_mother_pT = truth_mother_pT; outTrue_mother_eta = truth_mother_eta;
          outTrue_mother_barcode = truth_mother_barcode; outTrue_mother_PDG_ID = truth_mother_PDG_ID;
          outTrue_negative_px = truth_track_1_px; outTrue_negative_py = truth_track_1_py; outTrue_negative_pz = truth_track_1_pz; outTrue_negative_pE = truth_track_1_pE;
          outTrue_positive_px = truth_track_2_px; outTrue_positive_py = truth_track_2_py; outTrue_positive_pz = truth_track_2_pz; outTrue_positive_pE = truth_track_2_pE;
          outTrue_negative_eta = truth_track_1_eta; outTrue_positive_eta = truth_track_2_eta;
          outTrue_negative_PDG_ID = truth_track_1_PDG_ID; outTrue_positive_PDG_ID = truth_track_2_PDG_ID;
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_negative_px = kfp_track_1_px; outKFP_negative_py = kfp_track_1_py; outKFP_negative_pz = kfp_track_1_pz; outKFP_negative_pE = kfp_track_1_pE;
          outKFP_positive_px = kfp_track_2_px; outKFP_positive_py = kfp_track_2_py; outKFP_positive_pz = kfp_track_2_pz; outKFP_positive_pE = kfp_track_2_pE;
          outKFP_negative_p = kfp_track_1_p; outKFP_positive_p = kfp_track_2_p;
          outKFP_negative_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_positive_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_negative_charge = kfp_track_1_charge; outKFP_positive_charge = kfp_track_2_charge;
          outKFP_negative_IP = kfp_track_1_IP; outKFP_positive_IP = kfp_track_2_IP;
          outKFP_negative_IPchi2 = kfp_track_1_IPchi2; outKFP_positive_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA; 

          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
 
          if (usedRecoEntries.size() > 0)
          {
            bool recoCandidateUsed = false;
            for (int ent : usedRecoEntries)
            {
              if (ent == minEntry)
	      {
                std::cout << "Candidate has already been used, skipping this candidate" << std::endl;
                recoCandidateUsed = true;
   	      } 
            }
            if (recoCandidateUsed) continue;
            else
            {
              usedRecoEntries.push_back(minEntry);
              // D0 or D0bar check here 
              if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
              else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
              else
              {
                Background_tree->Fill();
                std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
              }
            }         
          }
          else
          {
            usedRecoEntries.push_back(minEntry);

            // D0 or D0bar check here 
            if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
            else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
            else
            {
              Background_tree->Fill();
              std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
            }
          }
        }
      } 
      else if (match_t2r1_t1r2)
      { 
        if (kfp_track_1_charge > 0 && kfp_track_2_charge < 0)
        {
          outTrue_mother_px = truth_mother_px; outTrue_mother_py = truth_mother_py; outTrue_mother_pz = truth_mother_pz; outTrue_mother_pE = truth_mother_pE;
          outTrue_mother_pT = truth_mother_pT; outTrue_mother_eta = truth_mother_eta;
          outTrue_mother_barcode = truth_mother_barcode; outTrue_mother_PDG_ID = truth_mother_PDG_ID;
          outTrue_negative_px = truth_track_1_px; outTrue_negative_py = truth_track_1_py; outTrue_negative_pz = truth_track_1_pz; outTrue_negative_pE = truth_track_1_pE;
          outTrue_positive_px = truth_track_2_px; outTrue_positive_py = truth_track_2_py; outTrue_positive_pz = truth_track_2_pz; outTrue_positive_pE = truth_track_2_pE;
          outTrue_negative_eta = truth_track_1_eta; outTrue_positive_eta = truth_track_2_eta;
          outTrue_negative_PDG_ID = truth_track_1_PDG_ID; outTrue_positive_PDG_ID = truth_track_2_PDG_ID;
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_positive_px = kfp_track_1_px; outKFP_positive_py = kfp_track_1_py; outKFP_positive_pz = kfp_track_1_pz; outKFP_positive_pE = kfp_track_1_pE;
          outKFP_negative_px = kfp_track_2_px; outKFP_negative_py = kfp_track_2_py; outKFP_negative_pz = kfp_track_2_pz; outKFP_negative_pE = kfp_track_2_pE;
          outKFP_positive_p = kfp_track_1_p; outKFP_negative_p = kfp_track_2_p;
          outKFP_positive_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_negative_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_positive_charge = kfp_track_1_charge; outKFP_negative_charge = kfp_track_2_charge;
          outKFP_positive_IP = kfp_track_1_IP; outKFP_negative_IP = kfp_track_2_IP;
          outKFP_positive_IPchi2 = kfp_track_1_IPchi2; outKFP_negative_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA; 

          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
 
          if (usedRecoEntries.size() > 0)
          {
            bool recoCandidateUsed = false;
            for (int ent : usedRecoEntries)
            {
              if (ent == minEntry)
	      {
                std::cout << "Candidate has already been used, skipping this candidate" << std::endl;
                recoCandidateUsed = true;
   	      } 
            }
            if (recoCandidateUsed) continue;
            else
            {
              usedRecoEntries.push_back(minEntry);
              // D0 or D0bar check here 
              if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
              else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
              else
              {
                Background_tree->Fill();
                std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
              }
            }         
          }
          else
          {
            usedRecoEntries.push_back(minEntry);

            // D0 or D0bar check here 
            if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
            else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
            else
            {
              Background_tree->Fill();
              std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
            }
          }
        }
        if (kfp_track_1_charge < 0 && kfp_track_2_charge > 0)
        {
          outTrue_mother_px = truth_mother_px; outTrue_mother_py = truth_mother_py; outTrue_mother_pz = truth_mother_pz; outTrue_mother_pE = truth_mother_pE;
          outTrue_mother_pT = truth_mother_pT; outTrue_mother_eta = truth_mother_eta;
          outTrue_mother_barcode = truth_mother_barcode; outTrue_mother_PDG_ID = truth_mother_PDG_ID;
          outTrue_positive_px = truth_track_1_px; outTrue_positive_py = truth_track_1_py; outTrue_positive_pz = truth_track_1_pz; outTrue_positive_pE = truth_track_1_pE;
          outTrue_negative_px = truth_track_2_px; outTrue_negative_py = truth_track_2_py; outTrue_negative_pz = truth_track_2_pz; outTrue_negative_pE = truth_track_2_pE;
          outTrue_positive_eta = truth_track_1_eta; outTrue_negative_eta = truth_track_2_eta;
          outTrue_positive_PDG_ID = truth_track_1_PDG_ID; outTrue_negative_PDG_ID = truth_track_2_PDG_ID;
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_negative_px = kfp_track_1_px; outKFP_negative_py = kfp_track_1_py; outKFP_negative_pz = kfp_track_1_pz; outKFP_negative_pE = kfp_track_1_pE;
          outKFP_positive_px = kfp_track_2_px; outKFP_positive_py = kfp_track_2_py; outKFP_positive_pz = kfp_track_2_pz; outKFP_positive_pE = kfp_track_2_pE;
          outKFP_negative_p = kfp_track_1_p; outKFP_positive_p = kfp_track_2_p;
          outKFP_negative_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_positive_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_negative_charge = kfp_track_1_charge; outKFP_positive_charge = kfp_track_2_charge;
          outKFP_negative_IP = kfp_track_1_IP; outKFP_positive_IP = kfp_track_2_IP;
          outKFP_negative_IPchi2 = kfp_track_1_IPchi2; outKFP_positive_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA; 

          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
 
          if (usedRecoEntries.size() > 0)
          {
            bool recoCandidateUsed = false;
            for (int ent : usedRecoEntries)
            {
              if (ent == minEntry)
	      {
                std::cout << "Candidate has already been used, skipping this candidate" << std::endl;
                recoCandidateUsed = true;
   	      } 
            }
            if (recoCandidateUsed) continue;
            else
            {
              usedRecoEntries.push_back(minEntry);
              // D0 or D0bar check here 
              if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
              else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
              else
              {
                Background_tree->Fill();
                std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
              }
            }         
          }
          else
          {
            usedRecoEntries.push_back(minEntry);

            // D0 or D0bar check here 
            if (outTrue_mother_PDG_ID == 421) D0_tree->Fill();  
            else if (outTrue_mother_PDG_ID == -421) D0bar_tree->Fill();
            else
            {
              Background_tree->Fill();
              std::cout << "ERROR: Should not get here if tracks match" <<std::endl;
            }
          }
        }
      }
    }
    for (int k = 0; k < recoTree->GetEntries(); ++k)
    {
      bool recoCandidateUsed = false;
      for (int ent : usedRecoEntries)
      {
        if (ent == k) recoCandidateUsed = true;
      }
      if (recoCandidateUsed == false)
      {
        recoTree->GetEntry(k); 
       
        if (kfp_track_1_charge > 0 && kfp_track_2_charge < 0)
        {
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_positive_px = kfp_track_1_px; outKFP_positive_py = kfp_track_1_py; outKFP_positive_pz = kfp_track_1_pz; outKFP_positive_pE = kfp_track_1_pE;
          outKFP_negative_px = kfp_track_2_px; outKFP_negative_py = kfp_track_2_py; outKFP_negative_pz = kfp_track_2_pz; outKFP_negative_pE = kfp_track_2_pE;
          outKFP_positive_p = kfp_track_1_p; outKFP_negative_p = kfp_track_2_p;
          outKFP_positive_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_negative_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_positive_charge = kfp_track_1_charge; outKFP_negative_charge = kfp_track_2_charge;
          outKFP_positive_IP = kfp_track_1_IP; outKFP_negative_IP = kfp_track_2_IP;
          outKFP_positive_IPchi2 = kfp_track_1_IPchi2; outKFP_negative_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA;
        
          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          
          Background_tree->Fill(); 
        } 
        else if (kfp_track_1_charge < 0 && kfp_track_2_charge > 0)
        {
          outKFP_D0_px = kfp_D0_px; outKFP_D0_py = kfp_D0_py; outKFP_D0_pz = kfp_D0_pz; outKFP_D0_pE = kfp_D0_pE;
          outKFP_D0_mass = kfp_D0_mass;
          outKFP_D0_decayTime = kfp_D0_decayTime; outKFP_D0_decayLength = kfp_D0_decayLength; outKFP_D0_DIRA = kfp_D0_DIRA; outKFP_D0_IP = kfp_D0_IP; outKFP_D0_IPchi2 = kfp_D0_IPchi2;
          outKFP_D0_pseudorapidity = kfp_D0_pseudorapidity; outKFP_D0_rapidity = kfp_D0_rapidity;
          outKFP_negative_px = kfp_track_1_px; outKFP_negative_py = kfp_track_1_py; outKFP_negative_pz = kfp_track_1_pz; outKFP_negative_pE = kfp_track_1_pE;
          outKFP_positive_px = kfp_track_2_px; outKFP_positive_py = kfp_track_2_py; outKFP_positive_pz = kfp_track_2_pz; outKFP_positive_pE = kfp_track_2_pE;
          outKFP_negative_p = kfp_track_1_p; outKFP_positive_p = kfp_track_2_p;
          outKFP_negative_pseudorapidity = kfp_track_1_pseudorapidity; outKFP_positive_pseudorapidity = kfp_track_2_pseudorapidity;
          outKFP_negative_charge = kfp_track_1_charge; outKFP_positive_charge = kfp_track_2_charge;
          outKFP_negative_IP = kfp_track_1_IP; outKFP_positive_IP = kfp_track_2_IP;
          outKFP_negative_IPchi2 = kfp_track_1_IPchi2; outKFP_positive_IPchi2 = kfp_track_2_IPchi2;
          outKFP_track_1_track_2_DCA = kfp_track_1_track_2_DCA;
        
          TVector3 P_p(outKFP_positive_px,outKFP_positive_py,outKFP_positive_pz);
          TVector3 N_p(outKFP_negative_px,outKFP_negative_py,outKFP_negative_pz);
          Float_t K_energy = sqrt(P_p.Mag2() + pow(K_mass,2));
          Float_t Pi_energy = sqrt(N_p.Mag2() + pow(Pi_mass,2));
          outKFP_KpPm_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          K_energy = sqrt(N_p.Mag2() + pow(K_mass,2));
          Pi_energy = sqrt(P_p.Mag2() + pow(Pi_mass,2));
          outKFP_KmPp_invm = sqrt(pow((K_energy + Pi_energy),2) - ((N_p+P_p).Mag2()));
          
          Background_tree->Fill(); 
        }
        else
        {
          std::cout << "WARNING: tracks not oppositely charged" << std::endl;
        } 
      }
    }
  }

  newfile->cd();
  D0_tree->Print();
  D0_tree->Write();
  D0bar_tree->Print();
  D0bar_tree->Write();
  Background_tree->Print();
  Background_tree->Write();
  newfile->Close(); 

  ifstream file2("Run40_D0_Separated_091922.root");
  if (file2.good())
  {
    string moveOutput = "mv Run40_D0_Separated_091922.root " + inDir;
    system(moveOutput.c_str());
  }

}
