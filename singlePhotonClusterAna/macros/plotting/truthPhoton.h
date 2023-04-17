//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 21 14:04:41 2022 by ROOT version 6.26/06
// from TTree TreeTruthPhoton/Tree for truth photons
// found on file: pi0ClusterAnan_pass0_FromDST.root
//////////////////////////////////////////////////////////

#ifndef truthPhoton_h
#define truthPhoton_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class truthPhoton {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   std::vector<float>   *photonPhi;
   std::vector<float>   *photonEta;
   std::vector<float>   *photonE;

   // List of branches
   TBranch        *b_photonPhi;   //!
   TBranch        *b_photonEta;   //!
   TBranch        *b_photonE;   //!

   truthPhoton(TTree *tree=0);
   virtual ~truthPhoton();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef truthPhoton_cxx
truthPhoton::truthPhoton(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("pass1_1M_out.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("pass1_1M_out.root");
      }
      f->GetObject("TreeTruthPhoton",tree);

   }
   Init(tree);
}

truthPhoton::~truthPhoton()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t truthPhoton::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t truthPhoton::LoadTree(Long64_t entry)
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

void truthPhoton::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   photonPhi = 0;
   photonEta = 0;
   photonE = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("photonE", &photonE, &b_photonE);
   fChain->SetBranchAddress("photon_eta", &photonEta, &b_photonEta);
   fChain->SetBranchAddress("photon_phi", &photonPhi, &b_photonPhi);
   Notify();
}

Bool_t truthPhoton::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void truthPhoton::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t truthPhoton::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef truthPhoton_cxx
