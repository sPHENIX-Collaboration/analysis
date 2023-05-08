//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Sep 21 14:05:21 2022 by ROOT version 6.26/06
// from TTree TreeClusterTower/Tree for cluster and tower information
// found on file: pi0ClusterAnan_pass0_FromDST.root
//////////////////////////////////////////////////////////

#ifndef cluster_h
#define cluster_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class cluster {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   std::vector<float>   *towerEtaCEnter;
   std::vector<float>   *towerPhiCenter;
   std::vector<float>   *towerEnergy;
   std::vector<float>   *clusterEta;
   std::vector<float>   *clusterPhi;
   std::vector<float>   *clusterE;
   std::vector<float>   *clusterChi2;
   std::vector<float>   *clusterProb;
   std::vector<float>   *clusterNTowers;

   // List of branches
   TBranch        *b_towerEtaCEnter;   //!
   TBranch        *b_towerPhiCenter;   //!
   TBranch        *b_towerEnergy;   //!
   TBranch        *b_clusterEta;   //!
   TBranch        *b_clusterPhi;   //!
   TBranch        *b_clusterE;   //!
   TBranch        *b_clusterChi2;   //!
   TBranch        *b_clusterProb;   //!
   TBranch        *b_clusterNTowers;   //!

   cluster(TTree *tree=0);
   virtual ~cluster();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef cluster_cxx
cluster::cluster(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("pass1_1M_out.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("pass1_1M_out.root");
      }
      f->GetObject("TreeClusterTower",tree);

   }
   Init(tree);
}

cluster::~cluster()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t cluster::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t cluster::LoadTree(Long64_t entry)
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

void cluster::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   towerEtaCEnter = 0;
   towerPhiCenter = 0;
   towerEnergy = 0;
   clusterEta = 0;
   clusterPhi = 0;
   clusterE = 0;
   clusterChi2 = 0;
   clusterProb = 0;
   clusterNTowers = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("towerEtaCEnter", &towerEtaCEnter, &b_towerEtaCEnter);
   fChain->SetBranchAddress("towerPhiCenter", &towerPhiCenter, &b_towerPhiCenter);
   fChain->SetBranchAddress("towerEnergy", &towerEnergy, &b_towerEnergy);
   fChain->SetBranchAddress("clusterEta", &clusterEta, &b_clusterEta);
   fChain->SetBranchAddress("clusterPhi", &clusterPhi, &b_clusterPhi);
   fChain->SetBranchAddress("clusterE", &clusterE, &b_clusterE);
   fChain->SetBranchAddress("clusterChi2", &clusterChi2, &b_clusterChi2);
   fChain->SetBranchAddress("clusterProb", &clusterProb, &b_clusterProb);
   fChain->SetBranchAddress("clusterNTowers", &clusterNTowers, &b_clusterNTowers);
   Notify();
}

Bool_t cluster::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void cluster::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t cluster::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef cluster_cxx
