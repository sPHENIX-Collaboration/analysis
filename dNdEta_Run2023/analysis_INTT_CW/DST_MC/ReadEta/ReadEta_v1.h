//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Mar  7 07:29:22 2024 by ROOT version 6.26/06
// from TTree tree_eta/Tracklet eta info.
// found on file: INTT_final_hist_info.root
//////////////////////////////////////////////////////////

#ifndef ReadEta_v1_h
#define ReadEta_v1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class ReadEta_v1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           eID;
   Int_t           Evt_centrality_bin;
   Double_t        Evt_zvtx;
   vector<double>  *Track_eta_d;
   vector<int>     *Track_eta_i;
   vector<double>  *Track_delta_phi_d;
   Int_t           N2Clu_track;
   Int_t           N3Clu_track;
   Int_t           N4Clu_track;

   // List of branches
   TBranch        *b_eID;   //!
   TBranch        *b_Evt_centrality_bin;   //!
   TBranch        *b_Evt_zvtx;   //!
   TBranch        *b_Track_eta_d;   //!
   TBranch        *b_Track_eta_i;   //!
   TBranch        *b_Track_delta_phi_d;   //!
   TBranch        *b_N2Clu_track;   //!
   TBranch        *b_N3Clu_track;   //!
   TBranch        *b_N4Clu_track;   //!

   ReadEta_v1(TTree *tree=0);
   virtual ~ReadEta_v1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadEta_v1_cxx
ReadEta_v1::ReadEta_v1(TTree *tree) : fChain(0) 
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

ReadEta_v1::~ReadEta_v1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadEta_v1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadEta_v1::LoadTree(Long64_t entry)
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

void ReadEta_v1::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   Track_eta_d = 0;
   Track_eta_i = 0;
   Track_delta_phi_d = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("eID", &eID, &b_eID);
   fChain->SetBranchAddress("Evt_centrality_bin", &Evt_centrality_bin, &b_Evt_centrality_bin);
   fChain->SetBranchAddress("Evt_zvtx", &Evt_zvtx, &b_Evt_zvtx);
   fChain->SetBranchAddress("Track_eta_d", &Track_eta_d, &b_Track_eta_d);
   fChain->SetBranchAddress("Track_eta_i", &Track_eta_i, &b_Track_eta_i);
   fChain->SetBranchAddress("Track_delta_phi_d", &Track_delta_phi_d, &b_Track_delta_phi_d);
   fChain->SetBranchAddress("N2Clu_track", &N2Clu_track, &b_N2Clu_track);
   fChain->SetBranchAddress("N3Clu_track", &N3Clu_track, &b_N3Clu_track);
   fChain->SetBranchAddress("N4Clu_track", &N4Clu_track, &b_N4Clu_track);
   Notify();
}

Bool_t ReadEta_v1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadEta_v1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadEta_v1::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadEta_v1_cxx
