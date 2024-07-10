//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Mar 28 09:22:24 2024 by ROOT version 6.26/06
// from TChain tree/
//////////////////////////////////////////////////////////

#ifndef ReadEvtXY_Chain_h
#define ReadEvtXY_Chain_h

#include <iostream>
#include <fstream>
using namespace std;

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.
#include <vector>

class ReadEvtXY_Chain {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           eID;
   Int_t           NClus;
   Long64_t        bco_full;
   Double_t        true_vtx_x;
   Double_t        true_vtx_y;
   Double_t        true_vtx_z;
   vector<double>  *reco_vtx_x;
   vector<double>  *reco_vtx_y;
   Double_t        reco_vtx_z;
   Double_t        reco_vtx_z_width;
   vector<double>  *reco_vtx_x_stddev;
   vector<double>  *reco_vtx_y_stddev;
   vector<double>  *binwidth_x;
   vector<double>  *binwidth_y;

   // List of branches
   TBranch        *b_eID;   //!
   TBranch        *b_NClus;   //!
   TBranch        *b_bco_full;   //!
   TBranch        *b_true_vtx_x;   //!
   TBranch        *b_true_vtx_y;   //!
   TBranch        *b_true_vtx_z;   //!
   TBranch        *b_reco_vtx_x;   //!
   TBranch        *b_reco_vtx_y;   //!
   TBranch        *b_reco_vtx_z;   //!
   TBranch        *b_reco_vtx_z_width;   //!
   TBranch        *b_reco_vtx_x_stddev;   //!
   TBranch        *b_reco_vtx_y_stddev;   //!
   TBranch        *b_binwidth_x;   //!
   TBranch        *b_binwidth_y;   //!

   ReadEvtXY_Chain(string mother_folder_directory = "", string input_file_list="");
   virtual ~ReadEvtXY_Chain();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t GetEntries();
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

protected:
   string mother_folder_directory;
   string input_file_list;
   vector<string> file_list;
   void read_list();

};

#endif
