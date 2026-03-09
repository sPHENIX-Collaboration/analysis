//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May  9 01:28:41 2024 by ROOT version 6.26/06
// from TTree EventTree/EventTree
// found on file: Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root
//////////////////////////////////////////////////////////

#ifndef ReadF4ANtupleData_h
#define ReadF4ANtupleData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class ReadF4ANtupleData {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           event;
   ULong_t         INTT_BCO;
   Int_t           NTrkrhits;
   vector<unsigned short> *TrkrHitRow;
   vector<unsigned short> *TrkrHitColumn;
   vector<unsigned char> *TrkrHitLadderZId;
   vector<unsigned char> *TrkrHitLadderPhiId;
   vector<unsigned char> *TrkrHitLayer;
   vector<unsigned short> *TrkrHitADC;
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
   // Int_t           event;
   UShort_t        clk;
   UShort_t        femclk;
   Bool_t          is_min_bias;
   Bool_t          is_min_bias_wozdc;
   Float_t         MBD_centrality;
   Float_t         MBD_z_vtx;
   Short_t         MBD_south_npmt;
   Short_t         MBD_north_npmt;
   Float_t         MBD_south_charge_sum;
   Float_t         MBD_north_charge_sum;
   Float_t         MBD_charge_sum;
   Float_t         MBD_charge_asymm;

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_INTT_BCO;   //!
   TBranch        *b_NTrkrhits;   //!
   TBranch        *b_TrkrHitRow;   //!
   TBranch        *b_TrkrHitColumn;   //!
   TBranch        *b_TrkrHitLadderZId;   //!
   TBranch        *b_TrkrHitLadderPhiId;   //!
   TBranch        *b_TrkrHitLayer;   //!
   TBranch        *b_TrkrHitADC;   //!
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
   // TBranch        *b_event;   //!
   TBranch        *b_clk;   //!
   TBranch        *b_femclk;   //!
   TBranch        *b_is_min_bias;   //!
   TBranch        *b_is_min_bias_wozdc;   //!
   TBranch        *b_MBD_centrality;   //!
   TBranch        *b_MBD_z_vtx;   //!
   TBranch        *b_MBD_south_npmt;   //!
   TBranch        *b_MBD_north_npmt;   //!
   TBranch        *b_MBD_south_charge_sum;   //!
   TBranch        *b_MBD_north_charge_sum;   //!
   TBranch        *b_MBD_charge_sum;   //!
   TBranch        *b_MBD_charge_asymm;   //!

   ReadF4ANtupleData(TTree *tree=0);
   virtual ~ReadF4ANtupleData();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadF4ANtupleData_cxx
ReadF4ANtupleData::ReadF4ANtupleData(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey.root");
      }
      f->GetObject("EventTree",tree);

   }
   Init(tree);
}

ReadF4ANtupleData::~ReadF4ANtupleData()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadF4ANtupleData::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadF4ANtupleData::LoadTree(Long64_t entry)
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

void ReadF4ANtupleData::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   TrkrHitRow = 0;
   TrkrHitColumn = 0;
   TrkrHitLadderZId = 0;
   TrkrHitLadderPhiId = 0;
   TrkrHitLayer = 0;
   TrkrHitADC = 0;
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

   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("INTT_BCO", &INTT_BCO, &b_INTT_BCO);
   fChain->SetBranchAddress("NTrkrhits", &NTrkrhits, &b_NTrkrhits);
   fChain->SetBranchAddress("TrkrHitRow", &TrkrHitRow, &b_TrkrHitRow);
   fChain->SetBranchAddress("TrkrHitColumn", &TrkrHitColumn, &b_TrkrHitColumn);
   fChain->SetBranchAddress("TrkrHitLadderZId", &TrkrHitLadderZId, &b_TrkrHitLadderZId);
   fChain->SetBranchAddress("TrkrHitLadderPhiId", &TrkrHitLadderPhiId, &b_TrkrHitLadderPhiId);
   fChain->SetBranchAddress("TrkrHitLayer", &TrkrHitLayer, &b_TrkrHitLayer);
   fChain->SetBranchAddress("TrkrHitADC", &TrkrHitADC, &b_TrkrHitADC);
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
//    fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("clk", &clk, &b_clk);
   fChain->SetBranchAddress("femclk", &femclk, &b_femclk);
   fChain->SetBranchAddress("is_min_bias", &is_min_bias, &b_is_min_bias);
   fChain->SetBranchAddress("is_min_bias_wozdc", &is_min_bias_wozdc, &b_is_min_bias_wozdc);
   fChain->SetBranchAddress("MBD_centrality", &MBD_centrality, &b_MBD_centrality);
   fChain->SetBranchAddress("MBD_z_vtx", &MBD_z_vtx, &b_MBD_z_vtx);
   fChain->SetBranchAddress("MBD_south_npmt", &MBD_south_npmt, &b_MBD_south_npmt);
   fChain->SetBranchAddress("MBD_north_npmt", &MBD_north_npmt, &b_MBD_north_npmt);
   fChain->SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum, &b_MBD_south_charge_sum);
   fChain->SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum, &b_MBD_north_charge_sum);
   fChain->SetBranchAddress("MBD_charge_sum", &MBD_charge_sum, &b_MBD_charge_sum);
   fChain->SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm, &b_MBD_charge_asymm);
   Notify();
}

Bool_t ReadF4ANtupleData::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadF4ANtupleData::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadF4ANtupleData::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadF4ANtupleData_cxx
