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

/*******************************/
/*    A*E Scan for D0/D0bar    */
/* Thomas Marshall, UCLA, 2022 */
/*       rosstom@ucla.edu      */
/*******************************/

void D0EffScan()
{
  // input directory where .root files are from KFParticle and QAG4SimulationTruthDecay output
  string inDir = "/sphenix/user/rosstom/analysis/HF-Particle/KFParticle_sPHENIX/Run40Acceptance082922/";
  TString truthFilePath;
  TString KFPFilePath;

  Int_t nFiles = 237;  
  string fileModuleName = "Run40Acceptance082922";
 
  Float_t etaAccept = 0.5; // cut on each track abs(eta) to be within acceptance
  Float_t pTAccept = 0.2; // cut on each track pT to be within acceptance
   
  Float_t totalTruthEvents = 0;
  Float_t insideKinematic = 0;
  Float_t insideGeometric = 0;
  Float_t insideBoth = 0;
  Float_t totalReconstructed = 0;
 
  // pT binning for reconstruction efficiency measurement
  // Total number of truth events
  Float_t truthTotal_01 = 0;
  Float_t truthTotal_12 = 0;
  Float_t truthTotal_23 = 0;
  Float_t truthTotal_34 = 0;
  Float_t truthTotal_45 = 0;
  Float_t truthTotal_5 = 0;

  // Number of truth events within geometric/kinematic acceptance
  Float_t truthAccept_01 = 0;
  Float_t truthAccept_12 = 0;
  Float_t truthAccept_23 = 0;
  Float_t truthAccept_34 = 0;
  Float_t truthAccept_45 = 0;
  Float_t truthAccept_5 = 0;
  
  // Number of events that were reconstructed by KFParticle
  Float_t reconstructed_01 = 0;
  Float_t reconstructed_12 = 0;
  Float_t reconstructed_23 = 0;
  Float_t reconstructed_34 = 0;
  Float_t reconstructed_45 = 0;
  Float_t reconstructed_5 = 0;
    
  // eta binning for geometric acceptance measurement
  // Uses same naming conventions as above
  Float_t truthTotal_n11n1 = 0;
  Float_t truthTotal_n1n08 = 0;
  Float_t truthTotal_n08n06 = 0;
  Float_t truthTotal_n06n04 = 0;
  Float_t truthTotal_n04n02 = 0;
  Float_t truthTotal_n02n00 = 0;
  Float_t truthTotal_p11p1 = 0;
  Float_t truthTotal_p1p08 = 0;
  Float_t truthTotal_p08p06 = 0;
  Float_t truthTotal_p06p04 = 0;
  Float_t truthTotal_p04p02 = 0;
  Float_t truthTotal_p02p00 = 0;
  Float_t truthTotal_p11 = 0;
  Float_t truthTotal_n11 = 0;
    
  Float_t truthAccept_n11n1 = 0;
  Float_t truthAccept_n1n08 = 0;
  Float_t truthAccept_n08n06 = 0;
  Float_t truthAccept_n06n04 = 0;
  Float_t truthAccept_n04n02 = 0;
  Float_t truthAccept_n02n00 = 0;
  Float_t truthAccept_p11p1 = 0;
  Float_t truthAccept_p1p08 = 0;
  Float_t truthAccept_p08p06 = 0;
  Float_t truthAccept_p06p04 = 0;
  Float_t truthAccept_p04p02 = 0;
  Float_t truthAccept_p02p00 = 0;
  Float_t truthAccept_p11 = 0;
  Float_t truthAccept_n11 = 0;
  
  Float_t reconstructed_n11n1 = 0;
  Float_t reconstructed_n1n08 = 0;
  Float_t reconstructed_n08n06 = 0;
  Float_t reconstructed_n06n04 = 0;
  Float_t reconstructed_n04n02 = 0;
  Float_t reconstructed_n02n00 = 0;
  Float_t reconstructed_p11p1 = 0;
  Float_t reconstructed_p1p08 = 0;
  Float_t reconstructed_p08p06 = 0;
  Float_t reconstructed_p06p04 = 0;
  Float_t reconstructed_p04p02 = 0;
  Float_t reconstructed_p02p00 = 0;
  Float_t reconstructed_p11 = 0;
  Float_t reconstructed_n11 = 0;


  // Loop over output files from each subjob to keep reconstructed events with their corresponding truth events
  for (int i = 235; i < nFiles; ++i)
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

    string truthFile = "outputData_" + fileModuleName + "_00" + fNumb + ".root";
    string KFPFile = "outputData_" + fileModuleName + "_KFP_00" + fNumb + ".root";
    truthFilePath = inDir + truthFile;
    KFPFilePath = inDir + KFPFile;

    TFile *truthInput(0);
    truthInput = TFile::Open(truthFilePath);
    TTree *truthTree = (TTree*)truthInput->Get("QAG4SimulationTruthDecay");
    TFile *KFPInput(0);
    KFPInput = TFile::Open(KFPFilePath);
    TTree *recoTree = (TTree*)KFPInput->Get("DecayTree");

    Float_t truthMotherPx, truthMotherPy, truthMotherPz, truthMotherPe;
    Float_t truthT1Px, truthT1Py, truthT1Pz, truthT1Pe;
    Float_t truthT2Px, truthT2Py, truthT2Pz, truthT2Pe;
    Float_t truthT1Pt, truthT2Pt;
    Float_t truthT1Eta, truthT2Eta;
    Float_t recoMotherPx, recoMotherPy, recoMotherPz, recoMotherPe;
    Float_t KFPTruthT1Px, KFPTruthT1Py, KFPTruthT1Pz, KFPTruthT1Pe;
    Float_t KFPTruthT2Px, KFPTruthT2Py, KFPTruthT2Pz, KFPTruthT2Pe;
    Float_t truthMotherPt, truthMotherEta;
    Int_t truthMotherBarcode;
    Float_t recoMotherMass;

    truthTree->SetBranchAddress("mother_px", &truthMotherPx);
    truthTree->SetBranchAddress("mother_py", &truthMotherPy);
    truthTree->SetBranchAddress("mother_pz", &truthMotherPz);
    truthTree->SetBranchAddress("mother_pE", &truthMotherPe);
    truthTree->SetBranchAddress("mother_pT", &truthMotherPt);
    truthTree->SetBranchAddress("mother_eta", &truthMotherEta);
    truthTree->SetBranchAddress("mother_barcode", &truthMotherBarcode);
    truthTree->SetBranchAddress("track_1_px", &truthT1Px);
    truthTree->SetBranchAddress("track_1_py", &truthT1Py);
    truthTree->SetBranchAddress("track_1_pz", &truthT1Pz);
    truthTree->SetBranchAddress("track_1_pT", &truthT1Pt);
    truthTree->SetBranchAddress("track_2_px", &truthT2Px);
    truthTree->SetBranchAddress("track_2_py", &truthT2Py);
    truthTree->SetBranchAddress("track_2_pz", &truthT2Pz);
    truthTree->SetBranchAddress("track_2_pT", &truthT2Pt);
    truthTree->SetBranchAddress("track_1_eta", &truthT1Eta);
    truthTree->SetBranchAddress("track_2_eta", &truthT2Eta);

    recoTree->SetBranchAddress("D0_px", &recoMotherPx);
    recoTree->SetBranchAddress("D0_py", &recoMotherPy);
    recoTree->SetBranchAddress("D0_pz", &recoMotherPz);
    recoTree->SetBranchAddress("D0_pE", &recoMotherPe);
    recoTree->SetBranchAddress("D0_mass", &recoMotherMass);
    recoTree->SetBranchAddress("track_1_true_px", &KFPTruthT1Px);
    recoTree->SetBranchAddress("track_1_true_py", &KFPTruthT1Py);
    recoTree->SetBranchAddress("track_1_true_pz", &KFPTruthT1Pz);
    recoTree->SetBranchAddress("track_2_true_px", &KFPTruthT2Px);
    recoTree->SetBranchAddress("track_2_true_py", &KFPTruthT2Py);
    recoTree->SetBranchAddress("track_2_true_pz", &KFPTruthT2Pz);

    vector<Int_t> reconstructedBarcodes;
    vector<Int_t> missedBarcodes;
    vector<int> usedRecoEntries;
  
    Float_t min_deltaPx, min_deltaPy, min_deltaPz, min_deltaPe;
    int minEntry;  
    
    for (int j = 0; j < truthTree->GetEntries(); ++j)
    {
      truthTree->GetEntry(j);
      totalTruthEvents += 1;

      if (truthT1Pt > pTAccept && truthT2Pt > pTAccept) insideKinematic += 1;
      if (abs(truthT1Eta) < etaAccept && abs(truthT2Eta) < etaAccept) insideGeometric += 1;
      if (abs(truthT1Eta) < etaAccept && abs(truthT2Eta) < etaAccept && truthT1Pt > pTAccept && truthT2Pt > pTAccept) insideBoth += 1;  

      if (truthMotherPt < 1) truthTotal_01 += 1;      
      if (truthMotherPt < 2 && truthMotherPt >= 1) truthTotal_12 += 1;      
      if (truthMotherPt < 3 && truthMotherPt >= 2) truthTotal_23 += 1;      
      if (truthMotherPt < 4 && truthMotherPt >= 3) truthTotal_34 += 1;      
      if (truthMotherPt < 5 && truthMotherPt >= 4) truthTotal_45 += 1;      
      if (truthMotherPt >= 5) truthTotal_5 += 1;
 
      if (truthMotherEta < -1) truthTotal_n11 += 1;      
      if (truthMotherEta < -1 && truthMotherEta >= -1.1) truthTotal_n11n1 += 1;      
      if (truthMotherEta < -0.8 && truthMotherEta >= -1) truthTotal_n1n08 += 1;      
      if (truthMotherEta < -0.6 && truthMotherEta >= -0.8) truthTotal_n08n06 += 1;      
      if (truthMotherEta < -0.4 && truthMotherEta >= -0.6) truthTotal_n06n04 += 1;      
      if (truthMotherEta < -0.2 && truthMotherEta >= -0.4) truthTotal_n04n02 += 1;      
      if (truthMotherEta < 0.0 && truthMotherEta >= -0.2) truthTotal_n02n00 += 1;      
      if (truthMotherEta < 0.2 && truthMotherEta >= 0.0) truthTotal_p02p00 += 1;      
      if (truthMotherEta < 0.4 && truthMotherEta >= 0.2) truthTotal_p04p02 += 1;      
      if (truthMotherEta < 0.6 && truthMotherEta >= 0.4) truthTotal_p06p04 += 1;      
      if (truthMotherEta < 0.8 && truthMotherEta >= 0.6) truthTotal_p08p06 += 1;      
      if (truthMotherEta < 1.0 && truthMotherEta >= 0.8) truthTotal_p1p08 += 1;      
      if (truthMotherEta < 1.1 && truthMotherEta >= 1) truthTotal_p11p1 += 1;      
      if (truthMotherEta >= 1) truthTotal_p11 += 1;      

      if (abs(truthT1Eta) < etaAccept && abs(truthT2Eta) < etaAccept && truthT1Pt > pTAccept && truthT2Pt > pTAccept)
      {
        if (truthMotherPt < 1) truthAccept_01 += 1;
        if (truthMotherPt < 2 && truthMotherPt >= 1) truthAccept_12 += 1;
        if (truthMotherPt < 3 && truthMotherPt >= 2) truthAccept_23 += 1;
        if (truthMotherPt < 4 && truthMotherPt >= 3) truthAccept_34 += 1;
        if (truthMotherPt < 5 && truthMotherPt >= 4) truthAccept_45 += 1;
        if (truthMotherPt >= 5) truthAccept_5 += 1;

        if (truthMotherEta < -1) truthAccept_n11 += 1;
        if (truthMotherEta < -1 && truthMotherEta >= -1.1) truthAccept_n11n1 += 1;
        if (truthMotherEta < -0.8 && truthMotherEta >= -1) truthAccept_n1n08 += 1;
        if (truthMotherEta < -0.6 && truthMotherEta >= -0.8) truthAccept_n08n06 += 1;
        if (truthMotherEta < -0.4 && truthMotherEta >= -0.6) truthAccept_n06n04 += 1;
        if (truthMotherEta < -0.2 && truthMotherEta >= -0.4) truthAccept_n04n02 += 1;
        if (truthMotherEta < 0.0 && truthMotherEta >= -0.2) truthAccept_n02n00 += 1;
        if (truthMotherEta < 0.2 && truthMotherEta >= 0.0) truthAccept_p02p00 += 1;
        if (truthMotherEta < 0.4 && truthMotherEta >= 0.2) truthAccept_p04p02 += 1;
        if (truthMotherEta < 0.6 && truthMotherEta >= 0.4) truthAccept_p06p04 += 1;
        if (truthMotherEta < 0.8 && truthMotherEta >= 0.6) truthAccept_p08p06 += 1;
        if (truthMotherEta < 1.0 && truthMotherEta >= 0.8) truthAccept_p1p08 += 1;
        if (truthMotherEta < 1.1 && truthMotherEta >= 1) truthAccept_p11p1 += 1;
        if (truthMotherEta >= 1) truthAccept_p11 += 1;
      } 

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
    
      bool matches = false;
       
      for (int k = 0; k < recoTree->GetEntries(); ++k)
      {
        recoTree->GetEntry(k); 
        matchPx_t1r1 = (KFPTruthT1Px == truthT1Px);
        matchPy_t1r1 = (KFPTruthT1Py == truthT1Py);
        matchPz_t1r1 = (KFPTruthT1Pz == truthT1Pz);
        matchPx_t1r2 = (KFPTruthT2Px == truthT1Px);
        matchPy_t1r2 = (KFPTruthT2Py == truthT1Py);
        matchPz_t1r2 = (KFPTruthT2Pz == truthT1Pz);
        matchPx_t2r1 = (KFPTruthT1Px == truthT2Px);
        matchPy_t2r1 = (KFPTruthT1Py == truthT2Py);
        matchPz_t2r1 = (KFPTruthT1Pz == truthT2Pz);
        matchPx_t2r2 = (KFPTruthT2Px == truthT2Px);
        matchPy_t2r2 = (KFPTruthT2Py == truthT2Py);
        matchPz_t2r2 = (KFPTruthT2Pz == truthT2Pz);
        
        // check the truth information from KFParticle and QAG4SimulationTruthDecay to match a reconstructed candidate with its truth information
        // if match is found, that means a true D0->PiK event was reconstructed in KFParticle
        if (matchPx_t1r1 && matchPy_t1r1 && matchPz_t1r1)
        {
          if (matchPx_t2r2 && matchPy_t2r2 && matchPz_t2r2)
          {
            std::cout << "Got a match" << std::endl;
            matches = true;
            minEntry = k;
            break;
          }
        }       
        else if (matchPx_t1r2 && matchPy_t1r2 && matchPz_t1r2)
        {
          if (matchPx_t2r1 && matchPy_t2r1 && matchPz_t2r1)
          {
            std::cout << "Got a match" << std::endl;
            matches = true;
            minEntry = k;
            break;
          }
        }       
      }
      if (matches)
      {
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
            
            if (abs(truthT1Eta) < etaAccept && abs(truthT2Eta) < etaAccept && truthT1Pt > pTAccept && truthT2Pt > pTAccept)
            { 
              totalReconstructed += 1;

              if (truthMotherPt < 1) reconstructed_01 += 1;      
              if (truthMotherPt < 2 && truthMotherPt >= 1) reconstructed_12 += 1;      
              if (truthMotherPt < 3 && truthMotherPt >= 2) reconstructed_23 += 1;      
              if (truthMotherPt < 4 && truthMotherPt >= 3) reconstructed_34 += 1;      
              if (truthMotherPt < 5 && truthMotherPt >= 4) reconstructed_45 += 1;      
              if (truthMotherPt >= 5) reconstructed_5 += 1;
 
              if (truthMotherEta < -1.1) reconstructed_n11 += 1;      
              if (truthMotherEta < -1 && truthMotherEta >= -1.1) reconstructed_n11n1 += 1;      
              if (truthMotherEta < -0.8 && truthMotherEta >= -1) reconstructed_n1n08 += 1;      
              if (truthMotherEta < -0.6 && truthMotherEta >= -0.8) reconstructed_n08n06 += 1;      
              if (truthMotherEta < -0.4 && truthMotherEta >= -0.6) reconstructed_n06n04 += 1;      
              if (truthMotherEta < -0.2 && truthMotherEta >= -0.4) reconstructed_n04n02 += 1;      
              if (truthMotherEta < 0.0 && truthMotherEta >= -0.2) reconstructed_n02n00 += 1;      
              if (truthMotherEta < 0.2 && truthMotherEta >= 0.0) reconstructed_p02p00 += 1;      
              if (truthMotherEta < 0.4 && truthMotherEta >= 0.2) reconstructed_p04p02 += 1;      
              if (truthMotherEta < 0.6 && truthMotherEta >= 0.4) reconstructed_p06p04 += 1;      
              if (truthMotherEta < 0.8 && truthMotherEta >= 0.6) reconstructed_p08p06 += 1;      
              if (truthMotherEta < 1.0 && truthMotherEta >= 0.8) reconstructed_p1p08 += 1;      
              if (truthMotherEta < 1.1 && truthMotherEta >= 1) reconstructed_p11p1 += 1;      
              if (truthMotherEta >= 1) reconstructed_p11 += 1;      
            } 
          }         
        }
        else
        {
          usedRecoEntries.push_back(minEntry);

          if (abs(truthT1Eta) < etaAccept && abs(truthT2Eta) < etaAccept && truthT1Pt > pTAccept && truthT2Pt > pTAccept)
          { 
            totalReconstructed += 1;

            if (truthMotherPt < 1) reconstructed_01 += 1;      
            if (truthMotherPt < 2 && truthMotherPt >= 1) reconstructed_12 += 1;      
            if (truthMotherPt < 3 && truthMotherPt >= 2) reconstructed_23 += 1;      
            if (truthMotherPt < 4 && truthMotherPt >= 3) reconstructed_34 += 1;      
            if (truthMotherPt < 5 && truthMotherPt >= 4) reconstructed_45 += 1;      
            if (truthMotherPt >= 5) reconstructed_5 += 1;
 
            if (truthMotherEta < -1.1) reconstructed_n11 += 1;      
            if (truthMotherEta < -1 && truthMotherEta >= -1.1) reconstructed_n11n1 += 1;      
            if (truthMotherEta < -0.8 && truthMotherEta >= -1) reconstructed_n1n08 += 1;      
            if (truthMotherEta < -0.6 && truthMotherEta >= -0.8) reconstructed_n08n06 += 1;      
            if (truthMotherEta < -0.4 && truthMotherEta >= -0.6) reconstructed_n06n04 += 1;      
            if (truthMotherEta < -0.2 && truthMotherEta >= -0.4) reconstructed_n04n02 += 1;      
            if (truthMotherEta < 0.0 && truthMotherEta >= -0.2) reconstructed_n02n00 += 1;      
            if (truthMotherEta < 0.2 && truthMotherEta >= 0.0) reconstructed_p02p00 += 1;      
            if (truthMotherEta < 0.4 && truthMotherEta >= 0.2) reconstructed_p04p02 += 1;      
            if (truthMotherEta < 0.6 && truthMotherEta >= 0.4) reconstructed_p06p04 += 1;      
            if (truthMotherEta < 0.8 && truthMotherEta >= 0.6) reconstructed_p08p06 += 1;      
            if (truthMotherEta < 1.0 && truthMotherEta >= 0.8) reconstructed_p1p08 += 1;      
            if (truthMotherEta < 1.1 && truthMotherEta >= 1) reconstructed_p11p1 += 1;      
            if (truthMotherEta >= 1) reconstructed_p11 += 1;      
          } 
        }
      } 
    } 
  }
  std::cout << "truth mother pT efficiencies" << std::endl;
  std::cout << "0-1 GeV: " << reconstructed_01/truthAccept_01 << std::endl;
  std::cout << "1-2 GeV: " << reconstructed_12/truthAccept_12 << std::endl;
  std::cout << "2-3 GeV: " << reconstructed_23/truthAccept_23 << std::endl;
  std::cout << "3-4 GeV: " << reconstructed_34/truthAccept_34 << std::endl;
  std::cout << "4-5 GeV: " << reconstructed_45/truthAccept_45 << std::endl;
  std::cout << "5+ GeV: " << reconstructed_5/truthAccept_5 << std::endl;

  std::cout << "" << std::endl;
  std::cout << "Number of truth events w/in truth track acceptance" << std::endl;
  
  std::cout << "0-1 GeV: " << truthAccept_01 << std::endl;
  std::cout << "1-2 GeV: " << truthAccept_12 << std::endl;
  std::cout << "2-3 GeV: " << truthAccept_23 << std::endl;
  std::cout << "3-4 GeV: " << truthAccept_34 << std::endl;
  std::cout << "4-5 GeV: " << truthAccept_45 << std::endl;
  std::cout << "5+ GeV: " << truthAccept_5 << std::endl;
  
  std::cout << "" << std::endl;

  std::cout << "truth mother eta acceptance" << std::endl;
  std::cout << "eta < -1.1 : " << reconstructed_n11/truthAccept_n11 << std::endl;
  std::cout << "-1.1 <= eta < -1.0 : " << reconstructed_n11n1/truthAccept_n11n1 << std::endl;
  std::cout << "-1 <= eta < -0.8 : " << reconstructed_n1n08/truthAccept_n1n08 << std::endl;
  std::cout << "-0.8 <= eta < -0.6 : " << reconstructed_n08n06/truthAccept_n08n06 << std::endl;
  std::cout << "-0.6 <= eta < -0.4 : " << reconstructed_n06n04/truthAccept_n06n04 << std::endl;
  std::cout << "-0.4 <= eta < -0.2 : " << reconstructed_n04n02/truthAccept_n04n02 << std::endl;
  std::cout << "-0.2 <= eta < 0.0 : " << reconstructed_n02n00/truthAccept_n02n00 << std::endl;
  std::cout << "0.0 <= eta < 0.2 : " << reconstructed_p02p00/truthAccept_p02p00 << std::endl;
  std::cout << "0.2 <= eta < 0.4 : " << reconstructed_p04p02/truthAccept_p04p02 << std::endl;
  std::cout << "0.4 <= eta < 0.6 : " << reconstructed_p06p04/truthAccept_p06p04 << std::endl;
  std::cout << "0.6 <= eta < 0.8 : " << reconstructed_p08p06/truthAccept_p08p06 << std::endl;
  std::cout << "0.8 <= eta < 1.0 : " << reconstructed_p1p08/truthAccept_p1p08 << std::endl;
  std::cout << "1.0 <= eta < 1.1 : " << reconstructed_p11p1/truthAccept_p11p1 << std::endl;
  std::cout << "eta >= 1.1 : " << reconstructed_p11/truthAccept_p11 << std::endl;
  
  std::cout << "" << std::endl;
  std::cout << "Number of truth events w/in truth track acceptance" << std::endl;

  std::cout << "eta < -1.1 : " << truthAccept_n11 << std::endl;
  std::cout << "-1.1 <= eta < -1.0 : " << truthAccept_n11n1 << std::endl;
  std::cout << "-1 <= eta < -0.8 : " << truthAccept_n1n08 << std::endl;
  std::cout << "-0.8 <= eta < -0.6 : " << truthAccept_n08n06 << std::endl;
  std::cout << "-0.6 <= eta < -0.4 : " << truthAccept_n06n04 << std::endl;
  std::cout << "-0.4 <= eta < -0.2 : " << truthAccept_n04n02 << std::endl;
  std::cout << "-0.2 <= eta < 0.0 : " << truthAccept_n02n00 << std::endl;
  std::cout << "0.0 <= eta < 0.2 : " << truthAccept_p02p00 << std::endl;
  std::cout << "0.2 <= eta < 0.4 : " << truthAccept_p04p02 << std::endl;
  std::cout << "0.4 <= eta < 0.6 : " << truthAccept_p06p04 << std::endl;
  std::cout << "0.6 <= eta < 0.8 : " << truthAccept_p08p06 << std::endl;
  std::cout << "0.8 <= eta < 1.0 : " << truthAccept_p1p08 << std::endl;
  std::cout << "1.0 <= eta < 1.1 : " << truthAccept_p11p1 << std::endl;
  std::cout << "eta >= 1.1 : " << truthAccept_p11 << std::endl;

  std::cout << "Total Number of Truth Events: " << totalTruthEvents << std::endl;
  std::cout << "Fraction of events w/ truth track pT > " << pTAccept << " GeV: " << insideKinematic/totalTruthEvents << std::endl;
  std::cout << "Fraction of events w/ truth track |eta| < " << etaAccept << ": " << insideGeometric/totalTruthEvents << std::endl;
  std::cout << "Fraction of events w/ both: " << insideBoth/totalTruthEvents << std::endl;
  std::cout << "Total Integrated Efficiency: " << totalReconstructed/insideBoth << std::endl;
 
  const Int_t n_pT = 6;
  Double_t pT_bin[n_pT] = {0.5,1.5,2.5,3.5,4.5,5.5};
  Double_t pT_efficiency[n_pT] = {reconstructed_01/truthAccept_01,reconstructed_12/truthAccept_12,reconstructed_23/truthAccept_23,reconstructed_34/truthAccept_34,reconstructed_45/truthAccept_45,reconstructed_5/truthAccept_5};
  Double_t pT_bin_widths[n_pT] = {0.5,0.5,0.5,0.5,0.5,0.5};
  Double_t pT_efficiency_err[n_pT] = {sqrt(truthAccept_01)/truthAccept_01,sqrt(truthAccept_12)/truthAccept_12,sqrt(truthAccept_23)/truthAccept_23,sqrt(truthAccept_34)/truthAccept_34,sqrt(truthAccept_45)/truthAccept_45,sqrt(truthAccept_5)/truthAccept_5};

  TCanvas *c1 = new TCanvas("D0_Efficiency_pT", "D0_Efficiency_pT", 1800, 900);
  TGraphErrors* D0_Efficiency_pT = new TGraphErrors(n_pT, pT_bin, pT_efficiency, pT_bin_widths, pT_efficiency_err);
  D0_Efficiency_pT->SetTitle("D^{0} Reconstruction Efficiency vs Truth Mother p_{T};Truth p_{T} [GeV/c];Reconstruction Efficiency");
  D0_Efficiency_pT->SetLineColor(kBlue);
  D0_Efficiency_pT->GetXaxis()->SetLimits(0.,6.0);
  D0_Efficiency_pT->SetMinimum(0.);
  D0_Efficiency_pT->SetMaximum(1.0);
  D0_Efficiency_pT->SetMarkerStyle(22);
  D0_Efficiency_pT->SetMarkerColor(kBlue);
  D0_Efficiency_pT->SetMarkerSize(2);
  D0_Efficiency_pT->SetLineWidth(2);
  D0_Efficiency_pT->Draw("ACP");
  auto legend = new TLegend(0.1,0.7,0.48,0.9);
  legend->AddEntry("","#it{#bf{sPHENIX}} Simulation","");
  legend->AddEntry("", "p+p #rightarrow D^{0} (#pi^{+}K^{-}) or #bar{D}^{0} (#pi^{-}K^{+})", "");
  legend->AddEntry("", "#sqrt{s}=200 GeV", "");
  legend->AddEntry("", "True Track pT > 0.2 GeV, |#eta| < 0.5", "");
  legend->SetMargin(0);
  legend->Draw();
  c1->SaveAs("D0_Efficiency_pT.png");
  c1->Close();

  const Int_t n_eta = 14;
  Double_t eta_bin[n_eta] = {-1.15,-1.05,-0.9,-0.7,-0.5,-0.3,-0.1,0.1,0.3,0.5,0.7,0.9,1.05,1.15};
  Double_t eta_efficiency[n_eta] = {reconstructed_n11/truthAccept_n11, reconstructed_n11n1/truthAccept_n11n1, reconstructed_n1n08/truthAccept_n1n08, reconstructed_n08n06/truthAccept_n08n06,
                                   reconstructed_n06n04/truthAccept_n06n04, reconstructed_n04n02/truthAccept_n04n02, reconstructed_n02n00/truthAccept_n02n00, reconstructed_p02p00/truthAccept_p02p00,
                                   reconstructed_p04p02/truthAccept_p04p02, reconstructed_p06p04/truthAccept_p06p04, reconstructed_p08p06/truthAccept_p08p06, reconstructed_p1p08/truthAccept_p1p08,
                                   reconstructed_p11p1/truthAccept_p11p1, reconstructed_p11/truthAccept_p11};
  Double_t eta_bin_widths[n_eta] = {0.05,0.05,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.05,0.05};
  Double_t eta_efficiency_err[n_eta] = {sqrt(truthAccept_n11)/truthAccept_n11,sqrt(truthAccept_n11n1)/truthAccept_n11n1,sqrt(truthAccept_n1n08)/truthAccept_n1n08,sqrt(truthAccept_n08n06)/truthAccept_n08n06,
                                       sqrt(truthAccept_n06n04)/truthAccept_n06n04,sqrt(truthAccept_n04n02)/truthAccept_n04n02,sqrt(truthAccept_n02n00)/truthAccept_n02n00,sqrt(truthAccept_p02p00)/truthAccept_p02p00,
                                       sqrt(truthAccept_p04p02)/truthAccept_p04p02,sqrt(truthAccept_p06p04)/truthAccept_p06p04,sqrt(truthAccept_p08p06)/truthAccept_p08p06,sqrt(truthAccept_p1p08)/truthAccept_p1p08,
                                       sqrt(truthAccept_p11p1)/truthAccept_p11p1,sqrt(truthAccept_p11)/truthAccept_p11};

  TCanvas *c2 = new TCanvas("D0_Efficiency_eta", "D0_Efficiency_eta", 1800, 900);
  TGraphErrors* D0_Efficiency_eta = new TGraphErrors(n_eta, eta_bin, eta_efficiency, eta_bin_widths, eta_efficiency_err);
  D0_Efficiency_eta->SetTitle("D^{0} Reconstruction Efficiency vs Truth Mother #eta;Truth #eta;Reconstruction Efficiency");
  D0_Efficiency_eta->SetLineColor(kBlue);
  D0_Efficiency_eta->GetXaxis()->SetLimits(-1.25,1.25);
  D0_Efficiency_eta->SetMinimum(0.);
  D0_Efficiency_eta->SetMaximum(1.0);
  D0_Efficiency_eta->SetMarkerStyle(22);
  D0_Efficiency_eta->SetMarkerColor(kBlue);
  D0_Efficiency_eta->SetMarkerSize(2);
  D0_Efficiency_eta->SetLineWidth(2);
  D0_Efficiency_eta->Draw("ACP");
  auto legend2 = new TLegend(0.1,0.7,0.48,0.9);
  legend2->AddEntry("","#it{#bf{sPHENIX}} Simulation","");
  legend2->AddEntry("", "p+p #rightarrow D^{0} (#pi^{+}K^{-}) or #bar{D}^{0} (#pi^{-}K^{+})", "");
  legend2->AddEntry("", "#sqrt{s}=200 GeV", "");
  legend2->AddEntry("", "True Track pT > 0.2 GeV, |#eta| < 0.5", "");
  legend2->SetMargin(0);
  legend2->Draw();
  c2->SaveAs("D0_Efficiency_eta.png");
  c2->Close();
}
