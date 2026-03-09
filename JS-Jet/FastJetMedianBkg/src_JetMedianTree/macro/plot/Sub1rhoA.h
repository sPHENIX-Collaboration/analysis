//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Oct 19 12:42:52 2023 by ROOT version 6.26/06
// from TTree T/JetRhoMedian Tree
// found on file: hadd_Sub1rhoA.root
//////////////////////////////////////////////////////////

#ifndef Sub1rhoA_h
#define Sub1rhoA_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class Sub1rhoA {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Float_t         rho;
   Float_t         rho_sigma;
   Float_t         cent_mdb;
   Float_t         cent_epd;
   Float_t         impactparam;
   vector<float>   *TruthJetEta;
   vector<float>   *TruthJetPhi;
   vector<float>   *TruthJetPt;
   vector<float>   *sub1JetEta;
   vector<float>   *sub1JetPhi;
   vector<float>   *sub1JetPt;
   vector<float>   *rhoAJetEta;
   vector<float>   *rhoAJetPhi;
   vector<float>   *rhoAJetPt;
   vector<float>   *rhoAJetPtLessRhoA;
   vector<float>   *rhoAJetArea;

   // List of branches
   TBranch        *b_rho;   //!
   TBranch        *b_rho_sigma;   //!
   TBranch        *b_cent_mdb;   //!
   TBranch        *b_cent_epd;   //!
   TBranch        *b_impactparam;   //!
   TBranch        *b_TruthJetEta;   //!
   TBranch        *b_TruthJetPhi;   //!
   TBranch        *b_TruthJetPt;   //!
   TBranch        *b_sub1JetEta;   //!
   TBranch        *b_sub1JetPhi;   //!
   TBranch        *b_sub1JetPt;   //!
   TBranch        *b_rhoAJetEta;   //!
   TBranch        *b_rhoAJetPhi;   //!
   TBranch        *b_rhoAJetPt;   //!
   TBranch        *b_rhoAJetPtLessRhoA;   //!
   TBranch        *b_rhoAJetArea;   //!

   Sub1rhoA(TTree *tree=0);
   virtual ~Sub1rhoA();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(string inp="");
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Sub1rhoA_cxx
Sub1rhoA::Sub1rhoA(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("hadd_Sub1rhoA.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("hadd_Sub1rhoA.root");
      }
      f->GetObject("T",tree);

   }
   Init(tree);
}

Sub1rhoA::~Sub1rhoA()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Sub1rhoA::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Sub1rhoA::LoadTree(Long64_t entry)
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

void Sub1rhoA::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TruthJetEta = 0;
   TruthJetPhi = 0;
   TruthJetPt = 0;
   sub1JetEta = 0;
   sub1JetPhi = 0;
   sub1JetPt = 0;
   rhoAJetEta = 0;
   rhoAJetPhi = 0;
   rhoAJetPt = 0;
   rhoAJetPtLessRhoA = 0;
   rhoAJetArea = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("rho_sigma", &rho_sigma, &b_rho_sigma);
   fChain->SetBranchAddress("cent_mdb", &cent_mdb, &b_cent_mdb);
   fChain->SetBranchAddress("cent_epd", &cent_epd, &b_cent_epd);
   fChain->SetBranchAddress("impactparam", &impactparam, &b_impactparam);
   fChain->SetBranchAddress("TruthJetEta", &TruthJetEta, &b_TruthJetEta);
   fChain->SetBranchAddress("TruthJetPhi", &TruthJetPhi, &b_TruthJetPhi);
   fChain->SetBranchAddress("TruthJetPt", &TruthJetPt, &b_TruthJetPt);
   fChain->SetBranchAddress("sub1JetEta", &sub1JetEta, &b_sub1JetEta);
   fChain->SetBranchAddress("sub1JetPhi", &sub1JetPhi, &b_sub1JetPhi);
   fChain->SetBranchAddress("sub1JetPt", &sub1JetPt, &b_sub1JetPt);
   fChain->SetBranchAddress("rhoAJetEta", &rhoAJetEta, &b_rhoAJetEta);
   fChain->SetBranchAddress("rhoAJetPhi", &rhoAJetPhi, &b_rhoAJetPhi);
   fChain->SetBranchAddress("rhoAJetPt", &rhoAJetPt, &b_rhoAJetPt);
   fChain->SetBranchAddress("rhoAJetPtLessRhoA", &rhoAJetPtLessRhoA, &b_rhoAJetPtLessRhoA);
   fChain->SetBranchAddress("rhoAJetArea", &rhoAJetArea, &b_rhoAJetArea);
   Notify();
}

Bool_t Sub1rhoA::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Sub1rhoA::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Sub1rhoA::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Sub1rhoA_cxx
