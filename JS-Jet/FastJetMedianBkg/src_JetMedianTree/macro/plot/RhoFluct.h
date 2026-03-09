//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct 24 09:50:41 2023 by ROOT version 6.26/06
// from TTree T/RhoFluct Tree
// found on file: hadd_RhoFluct.root
//////////////////////////////////////////////////////////

#ifndef RhoFluct_h
#define RhoFluct_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class RhoFluct {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Float_t         rho;
   Float_t         rho_sigma;
   Float_t         cent;
   Float_t         cent_mdb;
   Float_t         cent_epd;
   Float_t         impactparam;
   Float_t         emb_1TeV_phi;
   Float_t         emb_1TeV_eta;
   Bool_t          sub1_ismatched;
   Float_t         sub1JetPhi;
   Float_t         sub1JetEta;
   Float_t         sub1JetPt;
   Float_t         sub1Jet_delPt;
   Bool_t          rhoA_ismatched;
   Float_t         rhoAJetPhi;
   Float_t         rhoAJetEta;
   Float_t         rhoAJetPt;
   Float_t         rhoAJetArea;
   Float_t         rhoAJetPtLessRhoA;
   Float_t         rhoAJet_delPt;

   // List of branches
   TBranch        *b_rho;   //!
   TBranch        *b_rho_sigma;   //!
   TBranch        *b_cent;   //!
   TBranch        *b_cent_mdb;   //!
   TBranch        *b_cent_epd;   //!
   TBranch        *b_impactparam;   //!
   TBranch        *b_emb_1TeV_phi;   //!
   TBranch        *b_emb_1TeV_eta;   //!
   TBranch        *b_sub1_ismatched;   //!
   TBranch        *b_sub1JetPhi;   //!
   TBranch        *b_sub1JetEta;   //!
   TBranch        *b_sub1JetPt;   //!
   TBranch        *b_sub1Jet_delPt;   //!
   TBranch        *b_rhoA_ismatched;   //!
   TBranch        *b_rhoAJetPhi;   //!
   TBranch        *b_rhoAJetEta;   //!
   TBranch        *b_rhoAJetPt;   //!
   TBranch        *b_rhoAJetArea;   //!
   TBranch        *b_rhoAJetPtLessRhoA;   //!
   TBranch        *b_rhoAJet_delPt;   //!

   RhoFluct(TTree *tree=0);
   virtual ~RhoFluct();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(string);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   const std::array<float,11> IP_DEC{{
         0.,
         4.86409,
         6.84237,
         8.34927,
         9.61087,
         10.722 ,
         11.7247,
         12.6486,
         13.5242,
         14.4868,
         22.
   }};
   int get_ip_decile(float ip) {
         if (ip < 4.86409) { return 0; }
         else if (ip < 6.84237) { return 1; }
         else if (ip < 8.34927) { return 2; }
         else if (ip < 9.61087) { return 3; }
         else if (ip < 10.722)  { return 4; }
         else if (ip < 11.7247) { return 5; }
         else if (ip < 12.6486) { return 6; }
         else if (ip < 13.5242) { return 7; }
         else if (ip < 14.4868) { return 8; }
         else                   { return 9; }
   };

};

#endif

#ifdef RhoFluct_cxx
RhoFluct::RhoFluct(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("hadd_RhoFluct.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("hadd_RhoFluct.root");
      }
      f->GetObject("T",tree);

   }
   Init(tree);
}

RhoFluct::~RhoFluct()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t RhoFluct::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t RhoFluct::LoadTree(Long64_t entry)
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

void RhoFluct::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("rho", &rho, &b_rho);
   fChain->SetBranchAddress("rho_sigma", &rho_sigma, &b_rho_sigma);
   fChain->SetBranchAddress("cent", &cent, &b_cent);
   fChain->SetBranchAddress("cent_mdb", &cent_mdb, &b_cent_mdb);
   fChain->SetBranchAddress("cent_epd", &cent_epd, &b_cent_epd);
   fChain->SetBranchAddress("impactparam", &impactparam, &b_impactparam);
   fChain->SetBranchAddress("emb_1TeV_phi", &emb_1TeV_phi, &b_emb_1TeV_phi);
   fChain->SetBranchAddress("emb_1TeV_eta", &emb_1TeV_eta, &b_emb_1TeV_eta);
   fChain->SetBranchAddress("sub1_ismatched", &sub1_ismatched, &b_sub1_ismatched);
   fChain->SetBranchAddress("sub1JetPhi", &sub1JetPhi, &b_sub1JetPhi);
   fChain->SetBranchAddress("sub1JetEta", &sub1JetEta, &b_sub1JetEta);
   fChain->SetBranchAddress("sub1JetPt", &sub1JetPt, &b_sub1JetPt);
   fChain->SetBranchAddress("sub1Jet_delPt", &sub1Jet_delPt, &b_sub1Jet_delPt);
   fChain->SetBranchAddress("rhoA_ismatched", &rhoA_ismatched, &b_rhoA_ismatched);
   fChain->SetBranchAddress("rhoAJetPhi", &rhoAJetPhi, &b_rhoAJetPhi);
   fChain->SetBranchAddress("rhoAJetEta", &rhoAJetEta, &b_rhoAJetEta);
   fChain->SetBranchAddress("rhoAJetPt", &rhoAJetPt, &b_rhoAJetPt);
   fChain->SetBranchAddress("rhoAJetArea", &rhoAJetArea, &b_rhoAJetArea);
   fChain->SetBranchAddress("rhoAJetPtLessRhoA", &rhoAJetPtLessRhoA, &b_rhoAJetPtLessRhoA);
   fChain->SetBranchAddress("rhoAJet_delPt", &rhoAJet_delPt, &b_rhoAJet_delPt);
   Notify();
}

Bool_t RhoFluct::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void RhoFluct::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t RhoFluct::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef RhoFluct_cxx
