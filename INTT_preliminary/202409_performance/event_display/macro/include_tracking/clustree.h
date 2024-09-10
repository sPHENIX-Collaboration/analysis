//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 26 00:22:48 2024 by ROOT version 6.26/06
// from TTree clus_tree/clus_tree
// found on file: tracking_1K_reverse_3sigma_cut.root
//////////////////////////////////////////////////////////

#ifndef clustree_h
#define clustree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class clustree
{
public:
   TTree *fChain;  //! pointer to the analyzed TTree or TChain
   Int_t fCurrent; //! current Tree number in a TChain

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<double> *x_out;
   vector<double> *y_out;
   vector<double> *z_out;
   vector<double> *r_out;
   vector<double> *phi_out;
   vector<double> *theta_out;

   vector<double> *x_in;
   vector<double> *y_in;
   vector<double> *z_in;
   vector<double> *r_in;
   vector<double> *phi_in;
   vector<double> *theta_in;

   // List of branches
   TBranch *b_x_out;     //!
   TBranch *b_y_out;     //!
   TBranch *b_z_out;     //!
   TBranch *b_r_out;     //!
   TBranch *b_phi_out;   //!
   TBranch *b_theta_out; //!
   TBranch *b_x_in;     //!
   TBranch *b_y_in;     //!
   TBranch *b_z_in;     //!
   TBranch *b_r_in;     //!
   TBranch *b_phi_in;   //!
   TBranch *b_theta_in; //!

   clustree(TTree *tree = 0);
   virtual ~clustree();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);
   virtual void Loop();
   virtual Bool_t Notify();
   virtual void Show(Long64_t entry = -1);
};

#endif

#ifdef clustree_cxx
clustree::clustree(TTree *tree) : fChain(0)
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
      f->GetObject("clus_tree", tree);
   }
   Init(tree);
}

clustree::~clustree()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Int_t clustree::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}
Long64_t clustree::LoadTree(Long64_t entry)
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

void clustree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   x_out = 0;
   y_out = 0;
   z_out = 0;
   r_out = 0;
   phi_out = 0;
   theta_out = 0;

   x_in = 0;
   y_in = 0;
   z_in = 0;
   r_in = 0;
   phi_in = 0;
   theta_in = 0;
   // Set branch addresses and branch pointers
   if (!tree)
      return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("x_out", &x_out, &b_x_out);
   fChain->SetBranchAddress("y_out", &y_out, &b_y_out);
   fChain->SetBranchAddress("z_out", &z_out, &b_z_out);
   fChain->SetBranchAddress("r_out", &r_out, &b_r_out);
   fChain->SetBranchAddress("phi_out", &phi_out, &b_phi_out);
   fChain->SetBranchAddress("theta_out", &theta_out, &b_theta_out);

   fChain->SetBranchAddress("x_in", &x_in, &b_x_in);
   fChain->SetBranchAddress("y_in", &y_in, &b_y_in);
   fChain->SetBranchAddress("z_in", &z_in, &b_z_in);
   fChain->SetBranchAddress("r_in", &r_in, &b_r_in);
   fChain->SetBranchAddress("phi_in", &phi_in, &b_phi_in);
   fChain->SetBranchAddress("theta_in", &theta_in, &b_theta_in);
   Notify();
}

Bool_t clustree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void clustree::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t clustree::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
#endif // #ifdef clustree_cxx
