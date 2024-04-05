#define ReadEta_v3_cxx
#include "ReadEta_v3.h"
#include <iostream>
using namespace std;

ReadEta_v3::ReadEta_v3(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("INTT_final_hist_info.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("INTT_final_hist_info.root");
      }
      f->GetObject("tree_eta",tree);

   }
   Init(tree);
}

ReadEta_v3::~ReadEta_v3()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadEta_v3::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadEta_v3::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ReadEta_v3::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   RecoTrack_eta_d = 0;
   RecoTrack_phi_d = 0;
   TrueTrack_eta_d = 0;
   TrueTrack_phi_d = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eID", &eID, &b_eID);
   fChain->SetBranchAddress("Evt_centrality_bin", &Evt_centrality_bin, &b_Evt_centrality_bin);
   fChain->SetBranchAddress("Evt_zvtx", &Evt_zvtx, &b_Evt_zvtx);
   fChain->SetBranchAddress("True_zvtx", &True_zvtx, &b_True_zvtx);
   fChain->SetBranchAddress("NTrueTrack", &NTrueTrack, &b_NTrueTrack);
   fChain->SetBranchAddress("NClus", &NClus, &b_NClus);
   fChain->SetBranchAddress("RecoTrack_eta_d", &RecoTrack_eta_d, &b_RecoTrack_eta_d);
   fChain->SetBranchAddress("RecoTrack_phi_d", &RecoTrack_phi_d, &b_RecoTrack_phi_d);
   fChain->SetBranchAddress("TrueTrack_eta_d", &TrueTrack_eta_d, &b_TrueTrack_eta_d);
   fChain->SetBranchAddress("TrueTrack_phi_d", &TrueTrack_phi_d, &b_TrueTrack_phi_d);
   fChain->SetBranchAddress("N2Clu_track", &N2Clu_track, &b_N2Clu_track);
   fChain->SetBranchAddress("N3Clu_track", &N3Clu_track, &b_N3Clu_track);
   fChain->SetBranchAddress("N4Clu_track", &N4Clu_track, &b_N4Clu_track);
   Notify();
}

Bool_t ReadEta_v3::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadEta_v3::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadEta_v3::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

void ReadEta_v3::Loop()
{
//   In a ROOT session, you can do:
//      root> .L ReadEta_v3.C
//      root> ReadEta_v3 t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
   }
}
