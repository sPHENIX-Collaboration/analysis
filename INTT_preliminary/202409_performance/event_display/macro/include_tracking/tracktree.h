//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Jan 26 00:22:09 2024 by ROOT version 6.26/06
// from TTree track_tree/track_tree
// found on file: tracking_1K_reverse_3sigma_cut.root
//////////////////////////////////////////////////////////

#ifndef tracktree_h
#define tracktree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"

class tracktree
{
public:
   TTree *fChain;  //! pointer to the analyzed TTree or TChain
   Int_t fCurrent; //! current Tree number in a TChain

   // Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   vector<double> *phi_track;
   vector<double> *theta_track;
   Int_t ntrack;
   int evt_track;
   vector<double> *dca_2d;
   vector<double> *dca_z;
   vector<double> *phi_tracklets;
   vector<double> *theta_tracklets;
   vector<int> *is_deleted;
   vector<int> *dca_range_out;
   vector<int> *dca2d_range_out;
   vector<int> *dcaz_range_out;
   vector<double> *z_tracklets_out;
   vector<double> *pT;
   vector<double> *pT_truth;

   Double_t x_vertex;
   Double_t y_vertex;
   Double_t z_vertex;

   // List of branches
   TBranch *b_phi_track;           //!
   TBranch *b_theta_track;         //!
   TBranch *b_ntrack;              //!
   TBranch *b_evt_track;           //!
   TBranch *b_dca_2d;              //!
   TBranch *b_dca_z;               //!
   TBranch *b_phi_tracklets;       //!
   TBranch *b_theta_tracklets;     //!
   TBranch *b_is_deleted;          //!
   TBranch *b_dca_range_out;       //!
   TBranch *b_dca2d_range_out;     //!
   TBranch *b_dcaz_range_out;      //!
   TBranch *b_z_tracklets_out;     //!
   TBranch *b_pT;     //!
   TBranch *b_pT_truth;     //!
   TBranch *b_x_vertex;            //!
   TBranch *b_y_vertex;            //!
   TBranch *b_z_vertex;            //!

   tracktree(TTree *tree = 0);
   virtual ~tracktree();
   virtual Int_t Cut(Long64_t entry);
   virtual Int_t GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void Init(TTree *tree);
   virtual void Loop();
   virtual Bool_t Notify();
   virtual void Show(Long64_t entry = -1);
};

#endif

#ifdef tracktree_cxx
tracktree::tracktree(TTree *tree) : fChain(0)
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
      f->GetObject("track_tree", tree);
   }
   Init(tree);
}

tracktree::~tracktree()
{
   if (!fChain)
      return;
   delete fChain->GetCurrentFile();
}

Int_t tracktree::GetEntry(Long64_t entry)
{
   // Read contents of entry.
   if (!fChain)
      return 0;
   return fChain->GetEntry(entry);
}
Long64_t tracktree::LoadTree(Long64_t entry)
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

void tracktree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   phi_track = 0;
   theta_track = 0;
   dca_2d = 0;
   dca_z = 0;
   phi_tracklets = 0;
   theta_tracklets = 0;
   is_deleted = 0;
   dca_range_out = 0;
   dca2d_range_out = 0;
   dcaz_range_out = 0;
   z_tracklets_out = 0;
   pT = 0;
   pT_truth = 0;
   // Set branch addresses and branch pointers
   if (!tree)
      return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("phi_track", &phi_track, &b_phi_track);
   fChain->SetBranchAddress("theta_track", &theta_track, &b_theta_track);
   fChain->SetBranchAddress("ntrack", &ntrack, &b_ntrack);
   fChain->SetBranchAddress("evt_track", &evt_track, &b_evt_track);
   fChain->SetBranchAddress("dca_2d", &dca_2d, &b_dca_2d);
   fChain->SetBranchAddress("dca_z", &dca_z, &b_dca_z);
   fChain->SetBranchAddress("phi_tracklets", &phi_tracklets, &b_phi_tracklets);
   fChain->SetBranchAddress("theta_tracklets", &theta_tracklets, &b_theta_tracklets);
   fChain->SetBranchAddress("is_deleted", &is_deleted, &b_is_deleted);
   fChain->SetBranchAddress("dca_range_out", &dca_range_out, &b_dca_range_out);
   fChain->SetBranchAddress("dca2d_range_out", &dca2d_range_out, &b_dca2d_range_out);
   fChain->SetBranchAddress("dcaz_range_out", &dcaz_range_out, &b_dcaz_range_out);
   fChain->SetBranchAddress("z_tracklets_out", &z_tracklets_out, &b_z_tracklets_out);
   fChain->SetBranchAddress("pT", &pT, &b_pT);
   fChain->SetBranchAddress("pT_truth", &pT_truth, &b_pT_truth);
   fChain->SetBranchAddress("x_vertex", &x_vertex, &b_x_vertex);
   fChain->SetBranchAddress("y_vertex", &y_vertex, &b_y_vertex);
   fChain->SetBranchAddress("z_vertex", &z_vertex, &b_z_vertex);
   Notify();
}

Bool_t tracktree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void tracktree::Show(Long64_t entry)
{
   // Print contents of entry.
   // If entry is not specified, print current entry
   if (!fChain)
      return;
   fChain->Show(entry);
}
Int_t tracktree::Cut(Long64_t entry)
{
   // This function may be called from Loop.
   // returns  1 if entry is accepted.
   // returns -1 otherwise.
   return 1;
}
#endif // #ifdef tracktree_cxx
