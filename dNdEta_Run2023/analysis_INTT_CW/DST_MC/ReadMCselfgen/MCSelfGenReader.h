//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May  2 09:00:27 2024 by ROOT version 6.26/06
// from TTree tree/INTT clu
// found on file: merged_file.root
//////////////////////////////////////////////////////////

#ifndef MCSelfGenReader_h
#define MCSelfGenReader_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"

class MCSelfGenReader {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           NClus;
   vector<int>     *system;
   vector<int>     *layer;
   vector<int>     *adc;
   vector<double>  *X;
   vector<double>  *Y;
   vector<double>  *Z;
   vector<int>     *size;
   vector<int>     *phi_size;
   vector<int>     *Z_size;
   Double_t        TruthPV_x;
   Double_t        TruthPV_y;
   Double_t        TruthPV_z;
   Int_t           NPrimaryG4P;
   vector<float>   *PrimaryG4P_Pt;
   vector<float>   *PrimaryG4P_Eta;
   vector<float>   *PrimaryG4P_Phi;
   vector<float>   *PrimaryG4P_E;
   vector<int>     *PrimaryG4P_PID;

   // List of branches
   TBranch        *b_NClus;   //!
   TBranch        *b_system;   //!
   TBranch        *b_layer;   //!
   TBranch        *b_adc;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Y;   //!
   TBranch        *b_Z;   //!
   TBranch        *b_size;   //!
   TBranch        *b_phi_size;   //!
   TBranch        *b_Z_size;   //!
   TBranch        *b_TruthPV_x;   //!
   TBranch        *b_TruthPV_y;   //!
   TBranch        *b_TruthPV_z;   //!
   TBranch        *b_NPrimaryG4P;   //!
   TBranch        *b_PrimaryG4P_Pt;   //!
   TBranch        *b_PrimaryG4P_Eta;   //!
   TBranch        *b_PrimaryG4P_Phi;   //!
   TBranch        *b_PrimaryG4P_E;   //!
   TBranch        *b_PrimaryG4P_PID;   //!

   MCSelfGenReader(TTree *tree=0);
   virtual ~MCSelfGenReader();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef MCSelfGenReader_cxx
MCSelfGenReader::MCSelfGenReader(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("merged_file.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("merged_file.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

MCSelfGenReader::~MCSelfGenReader()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MCSelfGenReader::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MCSelfGenReader::LoadTree(Long64_t entry)
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

void MCSelfGenReader::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   system = 0;
   layer = 0;
   adc = 0;
   X = 0;
   Y = 0;
   Z = 0;
   size = 0;
   phi_size = 0;
   Z_size = 0;
   PrimaryG4P_Pt = 0;
   PrimaryG4P_Eta = 0;
   PrimaryG4P_Phi = 0;
   PrimaryG4P_E = 0;
   PrimaryG4P_PID = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("NClus", &NClus, &b_NClus);
   fChain->SetBranchAddress("system", &system, &b_system);
   fChain->SetBranchAddress("layer", &layer, &b_layer);
   fChain->SetBranchAddress("adc", &adc, &b_adc);
   fChain->SetBranchAddress("X", &X, &b_X);
   fChain->SetBranchAddress("Y", &Y, &b_Y);
   fChain->SetBranchAddress("Z", &Z, &b_Z);
   fChain->SetBranchAddress("size", &size, &b_size);
   fChain->SetBranchAddress("phi_size", &phi_size, &b_phi_size);
   fChain->SetBranchAddress("Z_size", &Z_size, &b_Z_size);
   fChain->SetBranchAddress("TruthPV_x", &TruthPV_x, &b_TruthPV_x);
   fChain->SetBranchAddress("TruthPV_y", &TruthPV_y, &b_TruthPV_y);
   fChain->SetBranchAddress("TruthPV_z", &TruthPV_z, &b_TruthPV_z);
   fChain->SetBranchAddress("NPrimaryG4P", &NPrimaryG4P, &b_NPrimaryG4P);
   fChain->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt, &b_PrimaryG4P_Pt);
   fChain->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta, &b_PrimaryG4P_Eta);
   fChain->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi, &b_PrimaryG4P_Phi);
   fChain->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E, &b_PrimaryG4P_E);
   fChain->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID, &b_PrimaryG4P_PID);
   Notify();
}

Bool_t MCSelfGenReader::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MCSelfGenReader::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MCSelfGenReader::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MCSelfGenReader_cxx
