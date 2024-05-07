//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sun Mar 10 16:08:43 2024 by ROOT version 6.26/06
// from TTree tree_eta/Tracklet eta info.
// found on file: ../../../../sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/DeltaR_TrackCounting_RecoZ_noMegaTrackRemoval_35k/INTT_final_hist_info.root
//////////////////////////////////////////////////////////

#ifndef ReadEta_v2_h
#define ReadEta_v2_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <iostream>
using namespace std;

class ReadEta_v2 {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           eID;
   Int_t           Evt_centrality_bin;
   Double_t        Evt_zvtx;
   Int_t           NTrueTrack;
   Int_t           NClus;
   vector<double>  *RecoTrack_eta_d;
   vector<double>  *RecoTrack_phi_d;
   vector<double>  *TrueTrack_eta_d;
   vector<double>  *TrueTrack_phi_d;
   Int_t           N2Clu_track;
   Int_t           N3Clu_track;
   Int_t           N4Clu_track;

   // List of branches
   TBranch        *b_eID;   //!
   TBranch        *b_Evt_centrality_bin;   //!
   TBranch        *b_Evt_zvtx;   //!
   TBranch        *b_NTrueTrack;   //!
   TBranch        *b_NClus;   //!
   TBranch        *b_RecoTrack_eta_d;   //!
   TBranch        *b_RecoTrack_phi_d;   //!
   TBranch        *b_TrueTrack_eta_d;   //!
   TBranch        *b_TrueTrack_phi_d;   //!
   TBranch        *b_N2Clu_track;   //!
   TBranch        *b_N3Clu_track;   //!
   TBranch        *b_N4Clu_track;   //!

   ReadEta_v2(TTree *tree=0);
   virtual ~ReadEta_v2();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadEta_v2_cxx

#endif // #ifdef ReadEta_v2_cxx
