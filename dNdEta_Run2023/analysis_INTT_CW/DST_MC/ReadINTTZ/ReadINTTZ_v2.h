//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Fri Feb 16 04:13:34 2024 by ROOT version 6.26/06
// from TTree tree_Z/INTT Z info.
// found on file: INTT_zvtx.root
//////////////////////////////////////////////////////////

#ifndef ReadINTTZ_v2_h
#define ReadINTTZ_v2_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class ReadINTTZ_v2 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain


   // Declaration of leaf types
   Int_t           eID;
   Long64_t        bco_full;
   Int_t           nclu_inner;
   Int_t           nclu_outer;
   Double_t        ES_zvtx;
   Double_t        ES_zvtxE;
   Double_t        ES_rangeL;
   Double_t        ES_rangeR;
   Int_t           ES_N_good;
   Double_t        ES_Width_density;
   Double_t        LB_Gaus_Mean_mean;
   Double_t        LB_Gaus_Mean_meanE;
   Double_t        LB_Gaus_Mean_chi2;
   Double_t        LB_Gaus_Mean_width;
   Double_t        LB_Gaus_Width_width;
   Double_t        LB_Gaus_Width_offset;
   Double_t        LB_Gaus_Width_size_width;
   Double_t        LB_geo_mean;
   Bool_t          good_zvtx_tag;
   Int_t           mid_cut_Ngroup;
   Double_t        mid_cut_peak_width;
   Double_t        mid_cut_peak_ratio;
   Int_t           LB_cut_Ngroup;
   Double_t        LB_cut_peak_width;
   Double_t        LB_cut_peak_ratio;
   Double_t        MC_true_zvtx;
   Int_t           Centrality_bin;

   // List of branches
   TBranch        *b_eID;   //!
   TBranch        *b_bco_full;   //!
   TBranch        *b_nclu_inner;   //!
   TBranch        *b_nclu_outer;   //!
   TBranch        *b_ES_zvtx;   //!
   TBranch        *b_ES_zvtxE;   //!
   TBranch        *b_ES_rangeL;   //!
   TBranch        *b_ES_rangeR;   //!
   TBranch        *b_ES_N_good;   //!
   TBranch        *b_ES_Width_density;   //!
   TBranch        *b_LB_Gaus_Mean_mean;   //!
   TBranch        *b_LB_Gaus_Mean_meanE;   //!
   TBranch        *b_LB_Gaus_Mean_chi2;   //!
   TBranch        *b_LB_Gaus_Mean_width;   //!
   TBranch        *b_LB_Gaus_Width_width;   //!
   TBranch        *b_LB_Gaus_Width_offset;   //!
   TBranch        *b_LB_Gaus_Width_size_width;   //!
   TBranch        *b_LB_geo_mean;   //!
   TBranch        *b_good_zvtx_tag;   //!
   TBranch        *b_mid_cut_Ngroup;   //!
   TBranch        *b_mid_cut_peak_width;   //!
   TBranch        *b_mid_cut_peak_ratio;   //!
   TBranch        *b_LB_cut_Ngroup;   //!
   TBranch        *b_LB_cut_peak_width;   //!
   TBranch        *b_LB_cut_peak_ratio;   //!
   TBranch        *b_MC_true_zvtx;   //!
   TBranch        *b_Centrality_bin;   //!

   ReadINTTZ_v2(TTree *tree=0);
   virtual ~ReadINTTZ_v2();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadINTTZ_v2_cxx
ReadINTTZ_v2::ReadINTTZ_v2(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("INTT_zvtx.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("INTT_zvtx.root");
      }
      f->GetObject("tree_Z",tree);

   }
   Init(tree);
}

ReadINTTZ_v2::~ReadINTTZ_v2()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadINTTZ_v2::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadINTTZ_v2::LoadTree(Long64_t entry)
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

void ReadINTTZ_v2::Init(TTree *tree)
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

   fChain->SetBranchAddress("eID", &eID, &b_eID);
   fChain->SetBranchAddress("bco_full", &bco_full, &b_bco_full);
   fChain->SetBranchAddress("nclu_inner", &nclu_inner, &b_nclu_inner);
   fChain->SetBranchAddress("nclu_outer", &nclu_outer, &b_nclu_outer);
   fChain->SetBranchAddress("ES_zvtx", &ES_zvtx, &b_ES_zvtx);
   fChain->SetBranchAddress("ES_zvtxE", &ES_zvtxE, &b_ES_zvtxE);
   fChain->SetBranchAddress("ES_rangeL", &ES_rangeL, &b_ES_rangeL);
   fChain->SetBranchAddress("ES_rangeR", &ES_rangeR, &b_ES_rangeR);
   fChain->SetBranchAddress("ES_N_good", &ES_N_good, &b_ES_N_good);
   fChain->SetBranchAddress("ES_Width_density", &ES_Width_density, &b_ES_Width_density);
   fChain->SetBranchAddress("LB_Gaus_Mean_mean", &LB_Gaus_Mean_mean, &b_LB_Gaus_Mean_mean);
   fChain->SetBranchAddress("LB_Gaus_Mean_meanE", &LB_Gaus_Mean_meanE, &b_LB_Gaus_Mean_meanE);
   fChain->SetBranchAddress("LB_Gaus_Mean_chi2", &LB_Gaus_Mean_chi2, &b_LB_Gaus_Mean_chi2);
   fChain->SetBranchAddress("LB_Gaus_Mean_width", &LB_Gaus_Mean_width, &b_LB_Gaus_Mean_width);
   fChain->SetBranchAddress("LB_Gaus_Width_width", &LB_Gaus_Width_width, &b_LB_Gaus_Width_width);
   fChain->SetBranchAddress("LB_Gaus_Width_offset", &LB_Gaus_Width_offset, &b_LB_Gaus_Width_offset);
   fChain->SetBranchAddress("LB_Gaus_Width_size_width", &LB_Gaus_Width_size_width, &b_LB_Gaus_Width_size_width);
   fChain->SetBranchAddress("LB_geo_mean", &LB_geo_mean, &b_LB_geo_mean);
   fChain->SetBranchAddress("good_zvtx_tag", &good_zvtx_tag, &b_good_zvtx_tag);
   fChain->SetBranchAddress("mid_cut_Ngroup", &mid_cut_Ngroup, &b_mid_cut_Ngroup);
   fChain->SetBranchAddress("mid_cut_peak_width", &mid_cut_peak_width, &b_mid_cut_peak_width);
   fChain->SetBranchAddress("mid_cut_peak_ratio", &mid_cut_peak_ratio, &b_mid_cut_peak_ratio);
   fChain->SetBranchAddress("LB_cut_Ngroup", &LB_cut_Ngroup, &b_LB_cut_Ngroup);
   fChain->SetBranchAddress("LB_cut_peak_width", &LB_cut_peak_width, &b_LB_cut_peak_width);
   fChain->SetBranchAddress("LB_cut_peak_ratio", &LB_cut_peak_ratio, &b_LB_cut_peak_ratio);
   fChain->SetBranchAddress("MC_true_zvtx", &MC_true_zvtx, &b_MC_true_zvtx);
   fChain->SetBranchAddress("Centrality_bin", &Centrality_bin, &b_Centrality_bin);
   Notify();
}

Bool_t ReadINTTZ_v2::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadINTTZ_v2::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadINTTZ_v2::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadINTTZ_v2_cxx
