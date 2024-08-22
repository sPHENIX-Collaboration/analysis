//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat May 11 03:49:02 2024 by ROOT version 6.26/06
// from TTree EventTree/EventTree
// found on file: ntuple_merged.root
//////////////////////////////////////////////////////////

#ifndef ReadF4ANtupleMC_h
#define ReadF4ANtupleMC_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include "vector"
#include "vector"
#include "vector"
#include "vector"
#include "vector"

class ReadF4ANtupleMC {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           event;
   Int_t           ncoll;
   Int_t           npart;
   Float_t         centrality_bimp;
   Float_t         centrality_impactparam;
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
   ULong_t         INTT_BCO;
   Int_t           NTruthVtx;
   Float_t         TruthPV_trig_x;
   Float_t         TruthPV_trig_y;
   Float_t         TruthPV_trig_z;
   Int_t           NHepMCFSPart;
   Int_t           signal_process_id;
   vector<float>   *HepMCFSPrtl_Pt;
   vector<float>   *HepMCFSPrtl_Eta;
   vector<float>   *HepMCFSPrtl_Phi;
   vector<float>   *HepMCFSPrtl_E;
   vector<int>     *HepMCFSPrtl_PID;
   vector<float>   *HepMCFSPrtl_prodx;
   vector<float>   *HepMCFSPrtl_prody;
   vector<float>   *HepMCFSPrtl_prodz;
   Int_t           NPrimaryG4P;
   vector<float>   *PrimaryG4P_Pt;
   vector<float>   *PrimaryG4P_Eta;
   vector<float>   *PrimaryG4P_Phi;
   vector<float>   *PrimaryG4P_E;
   vector<int>     *PrimaryG4P_PID;
   Int_t           NTruthLayers;
   vector<int>     *ClusTruthCKeys;
   vector<int>     *ClusNG4Particles;
   vector<int>     *ClusNPrimaryG4Particles;
   vector<int>     *TruthClusPhiSize;
   vector<int>     *TruthClusZSize;
   vector<int>     *TruthClusNRecoClus;
   vector<int>     *PrimaryTruthClusPhiSize;
   vector<int>     *PrimaryTruthClusZSize;
   vector<int>     *PrimaryTruthClusNRecoClus;
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

   // List of branches
   TBranch        *b_event;   //!
   TBranch        *b_ncoll;   //!
   TBranch        *b_npart;   //!
   TBranch        *b_centrality_bimp;   //!
   TBranch        *b_centrality_impactparam;   //!
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
   TBranch        *b_INTT_BCO;   //!
   TBranch        *b_NTruthVtx;   //!
   TBranch        *b_TruthPV_trig_x;   //!
   TBranch        *b_TruthPV_trig_y;   //!
   TBranch        *b_TruthPV_trig_z;   //!
   TBranch        *b_NHepMCFSPart;   //!
   TBranch        *b_signal_process_id;   //!
   TBranch        *b_HepMCFSPrtl_Pt;   //!
   TBranch        *b_HepMCFSPrtl_Eta;   //!
   TBranch        *b_HepMCFSPrtl_Phi;   //!
   TBranch        *b_HepMCFSPrtl_E;   //!
   TBranch        *b_HepMCFSPrtl_PID;   //!
   TBranch        *b_HepMCFSPrtl_prodx;   //!
   TBranch        *b_HepMCFSPrtl_prody;   //!
   TBranch        *b_HepMCFSPrtl_prodz;   //!
   TBranch        *b_NPrimaryG4P;   //!
   TBranch        *b_PrimaryG4P_Pt;   //!
   TBranch        *b_PrimaryG4P_Eta;   //!
   TBranch        *b_PrimaryG4P_Phi;   //!
   TBranch        *b_PrimaryG4P_E;   //!
   TBranch        *b_PrimaryG4P_PID;   //!
   TBranch        *b_NTruthLayers;   //!
   TBranch        *b_ClusTruthCKeys;   //!
   TBranch        *b_ClusNG4Particles;   //!
   TBranch        *b_ClusNPrimaryG4Particles;   //!
   TBranch        *b_TruthClusPhiSize;   //!
   TBranch        *b_TruthClusZSize;   //!
   TBranch        *b_TruthClusNRecoClus;   //!
   TBranch        *b_PrimaryTruthClusPhiSize;   //!
   TBranch        *b_PrimaryTruthClusZSize;   //!
   TBranch        *b_PrimaryTruthClusNRecoClus;   //!
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

   ReadF4ANtupleMC(TTree *tree=0);
   virtual ~ReadF4ANtupleMC();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ReadF4ANtupleMC_cxx
ReadF4ANtupleMC::ReadF4ANtupleMC(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("ntuple_merged.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("ntuple_merged.root");
      }
      f->GetObject("EventTree",tree);

   }
   Init(tree);
}

ReadF4ANtupleMC::~ReadF4ANtupleMC()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ReadF4ANtupleMC::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ReadF4ANtupleMC::LoadTree(Long64_t entry)
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

void ReadF4ANtupleMC::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   HepMCFSPrtl_Pt = 0;
   HepMCFSPrtl_Eta = 0;
   HepMCFSPrtl_Phi = 0;
   HepMCFSPrtl_E = 0;
   HepMCFSPrtl_PID = 0;
   HepMCFSPrtl_prodx = 0;
   HepMCFSPrtl_prody = 0;
   HepMCFSPrtl_prodz = 0;
   PrimaryG4P_Pt = 0;
   PrimaryG4P_Eta = 0;
   PrimaryG4P_Phi = 0;
   PrimaryG4P_E = 0;
   PrimaryG4P_PID = 0;
   ClusTruthCKeys = 0;
   ClusNG4Particles = 0;
   ClusNPrimaryG4Particles = 0;
   TruthClusPhiSize = 0;
   TruthClusZSize = 0;
   TruthClusNRecoClus = 0;
   PrimaryTruthClusPhiSize = 0;
   PrimaryTruthClusZSize = 0;
   PrimaryTruthClusNRecoClus = 0;
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
   fChain->SetBranchAddress("ncoll", &ncoll, &b_ncoll);
   fChain->SetBranchAddress("npart", &npart, &b_npart);
   fChain->SetBranchAddress("centrality_bimp", &centrality_bimp, &b_centrality_bimp);
   fChain->SetBranchAddress("centrality_impactparam", &centrality_impactparam, &b_centrality_impactparam);
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
   fChain->SetBranchAddress("INTT_BCO", &INTT_BCO, &b_INTT_BCO);
   fChain->SetBranchAddress("NTruthVtx", &NTruthVtx, &b_NTruthVtx);
   fChain->SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x, &b_TruthPV_trig_x);
   fChain->SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y, &b_TruthPV_trig_y);
   fChain->SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z, &b_TruthPV_trig_z);
   fChain->SetBranchAddress("NHepMCFSPart", &NHepMCFSPart, &b_NHepMCFSPart);
   fChain->SetBranchAddress("signal_process_id", &signal_process_id, &b_signal_process_id);
   fChain->SetBranchAddress("HepMCFSPrtl_Pt", &HepMCFSPrtl_Pt, &b_HepMCFSPrtl_Pt);
   fChain->SetBranchAddress("HepMCFSPrtl_Eta", &HepMCFSPrtl_Eta, &b_HepMCFSPrtl_Eta);
   fChain->SetBranchAddress("HepMCFSPrtl_Phi", &HepMCFSPrtl_Phi, &b_HepMCFSPrtl_Phi);
   fChain->SetBranchAddress("HepMCFSPrtl_E", &HepMCFSPrtl_E, &b_HepMCFSPrtl_E);
   fChain->SetBranchAddress("HepMCFSPrtl_PID", &HepMCFSPrtl_PID, &b_HepMCFSPrtl_PID);
   fChain->SetBranchAddress("HepMCFSPrtl_prodx", &HepMCFSPrtl_prodx, &b_HepMCFSPrtl_prodx);
   fChain->SetBranchAddress("HepMCFSPrtl_prody", &HepMCFSPrtl_prody, &b_HepMCFSPrtl_prody);
   fChain->SetBranchAddress("HepMCFSPrtl_prodz", &HepMCFSPrtl_prodz, &b_HepMCFSPrtl_prodz);
   fChain->SetBranchAddress("NPrimaryG4P", &NPrimaryG4P, &b_NPrimaryG4P);
   fChain->SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt, &b_PrimaryG4P_Pt);
   fChain->SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta, &b_PrimaryG4P_Eta);
   fChain->SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi, &b_PrimaryG4P_Phi);
   fChain->SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E, &b_PrimaryG4P_E);
   fChain->SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID, &b_PrimaryG4P_PID);
   fChain->SetBranchAddress("NTruthLayers", &NTruthLayers, &b_NTruthLayers);
   fChain->SetBranchAddress("ClusTruthCKeys", &ClusTruthCKeys, &b_ClusTruthCKeys);
   fChain->SetBranchAddress("ClusNG4Particles", &ClusNG4Particles, &b_ClusNG4Particles);
   fChain->SetBranchAddress("ClusNPrimaryG4Particles", &ClusNPrimaryG4Particles, &b_ClusNPrimaryG4Particles);
   fChain->SetBranchAddress("TruthClusPhiSize", &TruthClusPhiSize, &b_TruthClusPhiSize);
   fChain->SetBranchAddress("TruthClusZSize", &TruthClusZSize, &b_TruthClusZSize);
   fChain->SetBranchAddress("TruthClusNRecoClus", &TruthClusNRecoClus, &b_TruthClusNRecoClus);
   fChain->SetBranchAddress("PrimaryTruthClusPhiSize", &PrimaryTruthClusPhiSize, &b_PrimaryTruthClusPhiSize);
   fChain->SetBranchAddress("PrimaryTruthClusZSize", &PrimaryTruthClusZSize, &b_PrimaryTruthClusZSize);
   fChain->SetBranchAddress("PrimaryTruthClusNRecoClus", &PrimaryTruthClusNRecoClus, &b_PrimaryTruthClusNRecoClus);
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
   Notify();
}

Bool_t ReadF4ANtupleMC::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void ReadF4ANtupleMC::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ReadF4ANtupleMC::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ReadF4ANtupleMC_cxx
