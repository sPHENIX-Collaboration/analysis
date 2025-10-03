//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Sat Nov 23 01:L:33 2024 by ROOT version 6.26/06
// from TTree tree/sPHENIX info.
// found on file: ana439_Electron_2GeV_00150.root
//////////////////////////////////////////////////////////

#ifndef InttSeedTrackPerformance_h
#define InttSeedTrackPerformance_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include <TMath.h>
#include "Fit/Fitter.h"
#include <Math/Functor.h>

#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TF1.h>
#include <TProfile.h>
#include <TProfile2D.h>

#include "TArc.h"

#include "SPHTracKuma.h"
#include "InttSeedTracking.h"

// Header file for the classes stored in the TTree if any.
#include <vector>

// struct hitStruct{
// 	Double_t r;
//    Double_t z;
// 	Double_t phi;
//    Double_t eta;
//    Double_t pt;
//    Double_t energy;
// };

class InttSeedTrackPerformance {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

// Fixed size dimensions of array or collections stored in the TTree if any.

   // Declaration of leaf types
   Int_t           trk_NClus;
   std::vector<int>     *trk_system;
   std::vector<int>     *trk_layer;
   std::vector<int>     *trk_adc;
   std::vector<double>  *trk_X;
   std::vector<double>  *trk_Y;
   std::vector<double>  *trk_Z;
   std::vector<int>     *trk_size;
   std::vector<int>     *trk_phi_size;
   std::vector<int>     *trk_Z_size;

   Int_t           nTowers;
   std::vector<int>     *tower_system;
   std::vector<double>  *tower_X;
   std::vector<double>  *tower_Y;
   std::vector<double>  *tower_Z;
   std::vector<double>  *tower_Eta;
   std::vector<double>  *tower_Phi;
   std::vector<double>  *tower_Eta_test;
   std::vector<double>  *tower_Phi_test;
   std::vector<double>  *tower_Eta_bin;
   std::vector<double>  *tower_Phi_bin;
   std::vector<double>  *tower_edep;

   Int_t           nCaloClus;
   std::vector<int>     *caloClus_system;
   std::vector<double>  *caloClus_X;
   std::vector<double>  *caloClus_Y;
   std::vector<double>  *caloClus_Z;
   std::vector<double>  *caloClus_R;
   std::vector<double>  *caloClus_Phi;
   std::vector<double>  *caloClus_edep;

   Int_t           NTruthVtx;
   Float_t         TruthPV_trig_x;
   Float_t         TruthPV_trig_y;
   Float_t         TruthPV_trig_z;
   Int_t           NPrimaryG4P;
   std::vector<float>   *PrimaryG4P_Pt;
   std::vector<float>   *PrimaryG4P_Eta;
   std::vector<float>   *PrimaryG4P_Phi;
   std::vector<float>   *PrimaryG4P_E;
   std::vector<int>     *PrimaryG4P_PID;
   std::vector<bool>    *PrimaryG4P_isChargeHadron;

   // List of branches
   TBranch        *b_trk_NClus;   //!
   TBranch        *b_trk_system;   //!
   TBranch        *b_trk_layer;   //!
   TBranch        *b_trk_adc;   //!
   TBranch        *b_trk_X;   //!
   TBranch        *b_trk_Y;   //!
   TBranch        *b_trk_Z;   //!
   TBranch        *b_trk_size;   //!
   TBranch        *b_trk_phi_size;   //!
   TBranch        *b_trk_Z_size;   //!

   TBranch        *b_nTowers;   //!
   TBranch        *b_tower_system;   //!
   TBranch        *b_tower_X;   //!
   TBranch        *b_tower_Y;   //!
   TBranch        *b_tower_Z;   //!
   TBranch        *b_tower_Eta;   //!
   TBranch        *b_tower_Phi;   //!
   TBranch        *b_tower_Eta_test;   //!
   TBranch        *b_tower_Phi_test;   //!
   TBranch        *b_tower_Eta_bin;   //!
   TBranch        *b_tower_Phi_bin;   //!
   TBranch        *b_tower_edep;   //!

   TBranch        *b_nCaloClus;   //!
   TBranch        *b_caloClus_system;   //!
   TBranch        *b_caloClus_X;   //!
   TBranch        *b_caloClus_Y;   //!
   TBranch        *b_caloClus_Z;   //!
   TBranch        *b_caloClus_R;   //!
   TBranch        *b_caloClus_Phi;   //!
   TBranch        *b_caloClus_edep;   //!

   TBranch        *b_NTruthVtx;   //!
   TBranch        *b_TruthPV_trig_x;   //!
   TBranch        *b_TruthPV_trig_y;   //!
   TBranch        *b_TruthPV_trig_z;   //!

   TBranch        *b_NPrimaryG4P;   //!
   TBranch        *b_PrimaryG4P_Pt;   //!
   TBranch        *b_PrimaryG4P_Eta;   //!
   TBranch        *b_PrimaryG4P_Phi;   //!
   TBranch        *b_PrimaryG4P_E;   //!
   TBranch        *b_PrimaryG4P_PID;   //!
   TBranch        *b_PrimaryG4P_isChargeHadron;   //!

   InttSeedTrackPerformance(TTree *tree=0, std::string fInputName="", std::string fOutputName="", Int_t runNum=0);
   virtual ~InttSeedTrackPerformance();
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(Int_t runNum);


   // Kuma Members ======
   std::string fDir = "";

   std::string fInput = "";
   std::string fOutput = "";
   TFile *oFile;

   Int_t qaEvent = 1;
   Int_t pubEvNum = 0;

   bool m_bDecayEvent = false;

   std::vector<Int_t> m_vTargetEvents;

   std::vector<tracKuma> m_tracks;

   std::vector<hitStruct > m_TruthParticle;

   std::vector<hitStruct > m_fMvtxHits;
   std::vector<hitStruct > m_sMvtxHits;
   std::vector<hitStruct > m_tMvtxHits;

   std::vector<hitStruct > m_iInttHits;
   std::vector<hitStruct > m_oInttHits;

   std::vector<hitStruct > m_TpcHits;

   std::vector<hitStruct > m_emcalHits;
   std::vector<hitStruct > m_iHCalHits;
   std::vector<hitStruct > m_oHCalHits;

   std::vector<std::vector<hitStruct > > m_vTruthParticle;

   std::vector<std::vector<hitStruct > > m_viInttHits;
   std::vector<std::vector<hitStruct > > m_voInttHits;

   std::vector<std::vector<hitStruct > > m_vTpcHits;

   std::vector<std::vector<hitStruct > > m_vemcalHits;
   std::vector<std::vector<hitStruct > > m_viHCalHits;
   std::vector<std::vector<hitStruct > > m_voHCalHits;
   


   TH2D* m_HINTTHitMap;
   TH2D* m_HGlobalHitMap;

   TProfile2D* m_HECalPhiVsE;
   TProfile2D* m_HIHCalPhiVsE;
   TProfile2D* m_HOHCalPhiVsE;

   TH1D* m_HMatchCalEVsHighestCalE;

   TH2D* m_HdPhiInttdPhiECal;
   TH2D* m_HTruthPtVsdPhiInttdPhiECal;
   
   TH2D* m_HTruthPtVsdPhiIIntt;
   TH2D* m_HTruthPtVsdPhiOIntt;

   TH3D* m_HTruthPtVsSagittaPt_rough;
   TH3D* m_HTruthPtVsSagittaPt_VtxIntt;
   TH3D* m_HTruthPtVsSagittaPt_InttEmcal;
   TH3D* m_HTruthPtVsSagittaPt_VtxInttEmcal;
   TH3D* m_HTruthPtVsSagittaPt_MvtxInttEmcal;
   TH3D* m_HTruthPtVsSagittaPt_VtxMvtxInttEmcal;

   TH3D* m_HTruthPtVsFitFuncPt_IInttOInttEmcal;

   TH3D* m_HTruthPtVsFitFuncPt_VtxIInttEmcal;
   TH3D* m_HTruthPtVsFitFuncPt_VtxOInttEmcal;
   TH3D* m_HTruthPtVsFitFuncPt_VtxInttEmcal;

   TH3D* m_HTruthPtVsFitFuncPt_MvtxIInttEmcal;
   TH3D* m_HTruthPtVsFitFuncPt_MvtxOInttEmcal;
   TH3D* m_HTruthPtVsFitFuncPt_MvtxInttEmcal;

   TH2D* m_HTruthPVsRecoP_InttEmcal;
   TH2D* m_HTruthPVsRecoP_MvtxInttEmcal;
   TH2D* m_HTruthPVsRecoP_FitFunc;

   TH2D* m_HTruthPVsEOverP_InttEmcal;
   TH2D* m_HTruthPVsEOverP_MvtxInttEmcal;
   TH2D* m_HTruthPVsEOverP_FitFunc;

   TH2D* m_HTruthPVsTheta_InttEmcal;
   TH2D* m_HTruthPVsTheta_MvtxInttEmcal;

   TH2D* m_HTruthPVsPhi_InttEmcal;
   TH2D* m_HTruthPVsPhi_MvtxInttEmcal;

   TH2D* m_dVtxXY_InttEmcal;
   TH2D* m_dVtxXY_MvtxInttEmcal;

   TH1D* m_dVtxR_InttEmcal;
   TH1D* m_dVtxR_MvtxInttEmcal;

   TH1D* m_dVtxZ_InttEmcal;
   TH1D* m_dVtxZ_MvtxInttEmcal;

   TH3D* m_HDE;


   TH3D* m_HDPhiVsDStraightVsTruPt;

   TH2D* m_HDPhiDStraight_VtxIIntt;
   TH1D* m_HDPhiDStraight1D_VtxIIntt;
   TH2D* m_HDPhiDStraight_IInttOIntt;
   TH1D* m_HDPhiDStraight1D_IInttOIntt;
   TH2D* m_HDPhiDStraight_OInttEmcal;
   TH1D* m_HDPhiDStraight1D_OInttEmcal;

   TH2D* m_HTruthPtVsDdPhiddStraight_IInttOInttEmcal;

   TH3D* m_HDLVsTruthPtVsEta_VtxIIntt;
   TH1D* m_HDL1D_VtxIIntt;
   TH3D* m_HDLVsTruthPtVsEta_IInttOIntt;
   TH1D* m_HDL1D_IInttOIntt;
   TH3D* m_HDLVsTruthPtVsEta_OInttEmcal;
   TH1D* m_HDL1D_OInttEmcal;


   TH3D* m_HDPhiVsTruthPtVsEta_IInttOInttEmcal;

   TH3D* m_HDPhiVsTruthPtVsEta_VtxIIntt;
   TH3D* m_HDPhiVsTruthPtVsEta_IInttOIntt;
   TH3D* m_HDPhiVsTruthPtVsEta_OInttEmcal;

   TH3D* m_HDPhiVsTruthPtVsEta_VtxIInttEmcal;
   TH3D* m_HDPhiVsTruthPtVsEta_VtxOInttEmcal;
   TH3D* m_HDPhiVsTruthPtVsEta_VtxInttEmcal;

   TH3D* m_HDPhiVsTruthPtVsEta_MvtxIInttEmcal;
   TH3D* m_HDPhiVsTruthPtVsEta_MvtxOInttEmcal;
   TH3D* m_HDPhiVsTruthPtVsEta_MvtxInttEmcal;

   TProfile* m_HDPhiVsTruthPtProf_IInttOInttEmcal;

   TProfile* m_HDPhiVsTruthPtProf_VtxIIntt;
   TProfile* m_HDPhiVsTruthPtProf_IInttOIntt;
   TProfile* m_HDPhiVsTruthPtProf_OInttEmcal;

   TProfile* m_HDPhiVsTruthPtProf_VtxIInttEmcal;
   TProfile* m_HDPhiVsTruthPtProf_VtxOInttEmcal;
   TProfile* m_HDPhiVsTruthPtProf_VtxInttEmcal;

   TProfile* m_HDPhiVsTruthPtProf_MvtxIInttEmcal;
   TProfile* m_HDPhiVsTruthPtProf_MvtxOInttEmcal;
   TProfile* m_HDPhiVsTruthPtProf_MvtxInttEmcal;

   TH1D* m_HPtEfficiency;
   TH1D* m_HTruTrackNum;

   // == member functions == 
   void HistInit();

   void CheckPrimP(std::vector<hitStruct >& m_TruthParticle);

   void ReadInttHitting(std::vector<hitStruct >& vFMvtxHits,\
   std::vector<hitStruct >& vSMvtxHits, std::vector<hitStruct >& vTMvtxHits,\
   std::vector<hitStruct >& vIInttHits, std::vector<hitStruct >& vOInttHits,\
   std::vector<hitStruct >& vTpcHits);

   void ReadCalHitting(std::vector<hitStruct >& vEmcalHits,\
   std::vector<hitStruct >& vIHCalHits, std::vector<hitStruct >& vOHcalHits);

   void ReadCalCluHitting(std::vector<hitStruct >& vEmcalHits,\
   std::vector<hitStruct >& vIHCalHits, std::vector<hitStruct >& vOHcalHits);


   // Track Performance QA functions =======================================
   Int_t TruRecoMatching(hitStruct truthP, std::vector<tracKuma > vRecoTrk,\
   std::vector<Int_t > vMatchiedRecoTrkId);

   void TrackQA(std::vector<hitStruct > vTruthPs, std::vector<hitStruct > vEmcalHits);
   
   Int_t TruRecoMatching(hitStruct truthP);

   void DeltaPtPerform(hitStruct truthP, tracKuma trk);
   void TrackOtherPropertiesWTruth(hitStruct truthP, tracKuma trk,\
   Double_t sagittaR, Double_t centerX, Double_t centerY, Double_t recoPt,\
   TH2D* hTruthPVsTheta, TH2D* hTruthPVsPhi, TH2D* hTruthPVsRecoP, TH2D* hTruthPVsEOverP,\
   TH2D* hDVtxXY, TH1D* hDVtxR, TH1D* hDVtxZ);

   void CaloEnergyQA(Int_t CalId, std::vector<hitStruct > calHits);
   bool ParticleDecayCheck(tracKuma trk, std::vector<hitStruct > vEmcalHits);

   void EstimateMagneticShift(hitStruct truthP, tracKuma trk);
   // Track Performance QA functions =======================================


   void DrawEventDisplay(Int_t eventId, std::vector<tracKuma > trks);
   void EventJudge(Int_t eventNum, Double_t targetVal, Double_t minLim, Double_t maxLim, bool bIn);
   void ShowEventInfo();
   void ShowTrackInfo(tracKuma trk, Double_t dPt, Double_t centerX, Double_t centerY, Double_t sagittaR);

   void ResetValuesForEvent();
   void AllResetValuesForEvent();
   void PartResetValuesForEvent();

   void WrightHists();

   void ChecKuma(std::string checkNo);
   // Kuma Members ======



};



#endif


