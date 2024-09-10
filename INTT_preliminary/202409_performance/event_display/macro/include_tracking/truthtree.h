//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 26 00:22:22 2024 by ROOT version 6.26/06
// from TTree truth_tree/truth_tree
// found on file: tracking_1K_reverse_3sigma_cut.root
//////////////////////////////////////////////////////////

#ifndef truthtree_h
#define truthtree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"

class truthtree
{
public:
   TTree *fChain;  //! pointer to the analyzed TTree or TChain
   Int_t fCurrent; //! current Tree number in a TChain

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   int evt_truth;
   vector<double> *truthpx;
   vector<double> *truthpy;
   vector<double> *truthpz;
   vector<double> *truthpt;
   vector<double> *trutheta;
   vector<double> *truththeta;
   vector<double> *truthphi;
   vector<int> *truthpid;
   vector<int> *status;
   vector<double> *truthxvtx;
   vector<double> *truthyvtx;
   vector<double> *truthzvtx;
   vector<double> *truthzout;

   // List of branches
   TBranch *b_truthpx;    //!
   TBranch *b_truthpy;    //!
   TBranch *b_truthpz;    //!
   TBranch *b_truthpt;    //!
   TBranch *b_trutheta;   //!
   TBranch *b_truththeta; //!
   TBranch *b_truthphi;   //!
   TBranch *b_truthpid;   //!
   TBranch *b_status;     //!
   TBranch *b_evt_truth;  //!
   TBranch *b_truthxvtx;  //!
   TBranch *b_truthyvtx;  //!
   TBranch *b_truthzvtx;  //!
   TBranch *b_truthzout;  //!

   truthtree(TTree *tree = 0);
   virtual ~truthtree();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);
   virtual void Loop();
   virtual Bool_t Notify();
   virtual void Show(Long64_t entry = -1);
};

#endif

#ifdef truthtree_cxx
truthtree::truthtree(TTree *tree) : fChain(0)
{
   // if parameter tree is not specified (or zero), connect the file
   // used to generate this class and read the Tree.
   if (tree == 0)
   {
      TFile *f = (TFile *)gROOT->GetListOfFiles()->FindObject("tracking_1K_reverse_3sigma_cut.root");
      if (!f || !f->IsOpen())
      {
         f = new TFile("tracking_1K_reverse_3sigma_cut.root");
      }
      f->GetObject("truth_tree", tree);
   }
   Init(tree);
}

truthtree::~truthtree()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Int_t truthtree::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}
Long64_t truthtree::LoadTree(Long64_t entry)
{
   // Set the environment to read one entry
   if (!fChain)
      return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0)
      return centry;
   if (fChain->GetTreeNumber() != fCurrent)
   {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void truthtree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   truthpx = 0;
   truthpy = 0;
   truthpz = 0;
   truthpt = 0;
   trutheta = 0;
   truththeta = 0;
   truthphi = 0;
   truthpid = 0;
   status = 0;
   truthxvtx = 0;
   truthyvtx = 0;
   truthzvtx = 0;
   truthzout = 0;
   // Set branch addresses and branch pointers
   if (!tree)
      return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("truthpx", &truthpx, &b_truthpx);
   fChain->SetBranchAddress("truthpy", &truthpy, &b_truthpy);
   fChain->SetBranchAddress("truthpz", &truthpz, &b_truthpz);
   fChain->SetBranchAddress("truthpt", &truthpt, &b_truthpt);
   fChain->SetBranchAddress("trutheta", &trutheta, &b_trutheta);
   fChain->SetBranchAddress("truththeta", &truththeta, &b_truththeta);
   fChain->SetBranchAddress("truthphi", &truthphi, &b_truthphi);
   fChain->SetBranchAddress("truthpid", &truthpid, &b_truthpid);
   fChain->SetBranchAddress("status", &status, &b_status);
   fChain->SetBranchAddress("evt_truth", &evt_truth, &b_evt_truth);
   fChain->SetBranchAddress("truthxvtx", &truthxvtx, &b_truthxvtx);
   fChain->SetBranchAddress("truthyvtx", &truthyvtx, &b_truthyvtx);
   fChain->SetBranchAddress("truthzvtx", &truthzvtx, &b_truthzvtx);
   fChain->SetBranchAddress("truthzout", &truthzout, &b_truthzout);
   Notify();
}

Bool_t truthtree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void truthtree::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t truthtree::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
#endif // #ifdef truthtree_cxx
