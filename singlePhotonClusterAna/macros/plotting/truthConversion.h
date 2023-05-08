//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Feb 26 00:47:52 2023 by ROOT version 6.26/06
// from TTree TreeConversion/Tree for Photon Conversion Info
// found on file: pass1_1M_out.root
//////////////////////////////////////////////////////////

#ifndef truthConversion_h
#define truthConversion_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class truthConversion {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<float>   *electronE;
   vector<float>   *electron_phi;
   vector<float>   *electron_eta;
   vector<float>   *positronE;
   vector<float>   *positron_phi;
   vector<float>   *positron_eta;
   vector<float>   *vtx_x;
   vector<float>   *vtx_y;
   vector<float>   *vtx_z;
   vector<float>   *vtx_t;
   vector<float>   *vtx_r;
   vector<bool>    *isConversionEvent;

   // List of branches
   TBranch        *b_electronE;   //!
   TBranch        *b_electron_phi;   //!
   TBranch        *b_electron_eta;   //!
   TBranch        *b_positronE;   //!
   TBranch        *b_positron_phi;   //!
   TBranch        *b_positron_eta;   //!
   TBranch        *b_vtx_x;   //!
   TBranch        *b_vtx_y;   //!
   TBranch        *b_vtx_z;   //!
   TBranch        *b_vtx_t;   //!
   TBranch        *b_vtx_r;   //!
   TBranch        *b_isConversionEvent;   //!

   truthConversion(TTree *tree=0);
   virtual ~truthConversion();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef truthConversion_cxx
truthConversion::truthConversion(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("pass1_1M_out.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("pass1_1M_out.root");
      }
      f->GetObject("TreeConversion",tree);

   }
   Init(tree);
}

truthConversion::~truthConversion()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t truthConversion::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t truthConversion::LoadTree(Long64_t entry)
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

void truthConversion::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   electronE = 0;
   electron_phi = 0;
   electron_eta = 0;
   positronE = 0;
   positron_phi = 0;
   positron_eta = 0;
   vtx_x = 0;
   vtx_y = 0;
   vtx_z = 0;
   vtx_t = 0;
   vtx_r = 0;
   isConversionEvent = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("electronE", &electronE, &b_electronE);
   fChain->SetBranchAddress("electron_phi", &electron_phi, &b_electron_phi);
   fChain->SetBranchAddress("electron_eta", &electron_eta, &b_electron_eta);
   fChain->SetBranchAddress("positronE", &positronE, &b_positronE);
   fChain->SetBranchAddress("positron_phi", &positron_phi, &b_positron_phi);
   fChain->SetBranchAddress("positron_eta", &positron_eta, &b_positron_eta);
   fChain->SetBranchAddress("vtx_x", &vtx_x, &b_vtx_x);
   fChain->SetBranchAddress("vtx_y", &vtx_y, &b_vtx_y);
   fChain->SetBranchAddress("vtx_z", &vtx_z, &b_vtx_z);
   fChain->SetBranchAddress("vtx_t", &vtx_t, &b_vtx_t);
   fChain->SetBranchAddress("vtx_r", &vtx_r, &b_vtx_r);
   fChain->SetBranchAddress("isConversionEvent", &isConversionEvent, &b_isConversionEvent);
   Notify();
}

Bool_t truthConversion::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void truthConversion::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t truthConversion::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef truthConversion_cxx
