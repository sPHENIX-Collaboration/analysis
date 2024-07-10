//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Mar  3 01:33:24 2024 by ROOT version 6.26/06
// from TTree EventTree/EventTree
// found on file: /gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root
//////////////////////////////////////////////////////////

#ifndef ReadINTTDSTv1_h
#define ReadINTTDSTv1_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class ReadINTTDSTv1 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event_counter;
   ULong_t         INTT_BCO;
   Int_t           NClus_Layer1;
   Int_t           NClus;
   vector<int>     *ClusLayer;
   vector<float>   *ClusX;
   vector<float>   *ClusY;
   vector<float>   *ClusZ;
   vector<float>   *ClusR;
   vector<float>   *ClusPhi;
   vector<float>   *ClusEta;
   vector<unsigned int> *ClusAdc;
   vector<float>   *ClusPhiSize;
   vector<float>   *ClusZSize;
   vector<unsigned char> *ClusLadderZId;
   vector<unsigned char> *ClusLadderPhiId;
   vector<unsigned int> *ClusTrkrHitSetKey;
   vector<int>     *ClusTimeBucketId;

   // List of branches
   TBranch        *b_event_counter;   //!
   TBranch        *b_INTT_BCO;   //!
   TBranch        *b_NClus_Layer1;   //!
   TBranch        *b_NClus;   //!
   TBranch        *b_ClusLayer;   //!
   TBranch        *b_ClusX;   //!
   TBranch        *b_ClusY;   //!
   TBranch        *b_ClusZ;   //!
   TBranch        *b_ClusR;   //!
   TBranch        *b_ClusPhi;   //!
   TBranch        *b_ClusEta;   //!
   TBranch        *b_ClusAdc;   //!
   TBranch        *b_ClusPhiSize;   //!
   TBranch        *b_ClusZSize;   //!
   TBranch        *b_ClusLadderZId;   //!
   TBranch        *b_ClusLadderPhiId;   //!
   TBranch        *b_ClusTrkrHitSetKey;   //!
   TBranch        *b_ClusTimeBucketId;   //!

   ReadINTTDSTv1(TTree *tree=0);
   virtual ~ReadINTTDSTv1();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadINTTDSTv1_cxx
ReadINTTDSTv1::ReadINTTDSTv1(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/gpfs/mnt/gpfs02/sphenix/user/cdean/software/analysis/dNdEta_Run2023/macros/intt_run20869.root");
      }
      f->GetObject("EventTree",tree);

   }
   Init(tree);
}

ReadINTTDSTv1::~ReadINTTDSTv1()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadINTTDSTv1::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadINTTDSTv1::LoadTree(Long64_t entry)
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

void ReadINTTDSTv1::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   ClusLayer = 0;
   ClusX = 0;
   ClusY = 0;
   ClusZ = 0;
   ClusR = 0;
   ClusPhi = 0;
   ClusEta = 0;
   ClusAdc = 0;
   ClusPhiSize = 0;
   ClusZSize = 0;
   ClusLadderZId = 0;
   ClusLadderPhiId = 0;
   ClusTrkrHitSetKey = 0;
   ClusTimeBucketId = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("event_counter", &event_counter, &b_event_counter);
   fChain->SetBranchAddress("INTT_BCO", &INTT_BCO, &b_INTT_BCO);
   fChain->SetBranchAddress("NClus_Layer1", &NClus_Layer1, &b_NClus_Layer1);
   fChain->SetBranchAddress("NClus", &NClus, &b_NClus);
   fChain->SetBranchAddress("ClusLayer", &ClusLayer, &b_ClusLayer);
   fChain->SetBranchAddress("ClusX", &ClusX, &b_ClusX);
   fChain->SetBranchAddress("ClusY", &ClusY, &b_ClusY);
   fChain->SetBranchAddress("ClusZ", &ClusZ, &b_ClusZ);
   fChain->SetBranchAddress("ClusR", &ClusR, &b_ClusR);
   fChain->SetBranchAddress("ClusPhi", &ClusPhi, &b_ClusPhi);
   fChain->SetBranchAddress("ClusEta", &ClusEta, &b_ClusEta);
   fChain->SetBranchAddress("ClusAdc", &ClusAdc, &b_ClusAdc);
   fChain->SetBranchAddress("ClusPhiSize", &ClusPhiSize, &b_ClusPhiSize);
   fChain->SetBranchAddress("ClusZSize", &ClusZSize, &b_ClusZSize);
   fChain->SetBranchAddress("ClusLadderZId", &ClusLadderZId, &b_ClusLadderZId);
   fChain->SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId, &b_ClusLadderPhiId);
   fChain->SetBranchAddress("ClusTrkrHitSetKey", &ClusTrkrHitSetKey, &b_ClusTrkrHitSetKey);
   fChain->SetBranchAddress("ClusTimeBucketId", &ClusTimeBucketId, &b_ClusTimeBucketId);
   Notify();
}

Bool_t ReadINTTDSTv1::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadINTTDSTv1::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadINTTDSTv1::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadINTTDSTv1_cxx
