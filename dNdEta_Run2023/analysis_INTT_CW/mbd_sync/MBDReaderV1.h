//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Mar  1 14:53:33 2024 by ROOT version 6.26/06
// from TTree EventTree/EventTree
// found on file: /gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root
//////////////////////////////////////////////////////////

#ifndef MBDReaderV1_h
#define MBDReaderV1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class MBDReaderV1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   UShort_t        femclk;
   Bool_t          is_min_bias;
   Float_t         MBD_centrality;
   Float_t         MBD_z_vtx;
   Float_t         MBD_south_charge_sum;
   Float_t         MBD_north_charge_sum;
   Float_t         MBD_charge_sum;
   Float_t         MBD_charge_asymm;

   // List of branches
   TBranch        *b_femclk;   //!
   TBranch        *b_is_min_bias;   //!
   TBranch        *b_MBD_centrality;   //!
   TBranch        *b_MBD_z_vtx;   //!
   TBranch        *b_MBD_south_charge_sum;   //!
   TBranch        *b_MBD_north_charge_sum;   //!
   TBranch        *b_MBD_charge_sum;   //!
   TBranch        *b_MBD_charge_asymm;   //!

   MBDReaderV1(TTree *tree=0);
   virtual ~MBDReaderV1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MBDReaderV1_cxx
MBDReaderV1::MBDReaderV1(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/centrality_run20869.root");
      }
      f->GetObject("EventTree",tree);

   }
   Init(tree);
}

MBDReaderV1::~MBDReaderV1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MBDReaderV1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MBDReaderV1::LoadTree(Long64_t entry)
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

void MBDReaderV1::Init(TTree *tree)
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

   fChain->SetBranchAddress("femclk", &femclk, &b_femclk);
   fChain->SetBranchAddress("is_min_bias", &is_min_bias, &b_is_min_bias);
   fChain->SetBranchAddress("MBD_centrality", &MBD_centrality, &b_MBD_centrality);
   fChain->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx, &b_MBD_z_vtx);
   fChain->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum, &b_MBD_south_charge_sum);
   fChain->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum, &b_MBD_north_charge_sum);
   fChain->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum, &b_MBD_charge_sum);
   fChain->SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm, &b_MBD_charge_asymm);
   Notify();
}

Bool_t MBDReaderV1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MBDReaderV1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MBDReaderV1::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MBDReaderV1_cxx
