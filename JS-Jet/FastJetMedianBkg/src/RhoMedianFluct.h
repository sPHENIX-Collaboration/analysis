#ifndef RHOMEDIANFLUCT__H
#define RHOMEDIANFLUCT__H

#include <fun4all/SubsysReco.h>
#include "MemTimeProgression.h"

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair

#include <array>
#include <vector>
#include <TRandom3.h>
#include <fastjet/PseudoJet.hh>

class PHCompositeNode;
class JetEvalStack;
class TTree;
class TH1;
class TH2D;
class JetInput;

class RhoMedianFluct : public SubsysReco
{
 public:
  RhoMedianFluct
    ( const std::string &outputfilename = "RhoMedianFluct.root"
    , const int n_print_freq            = 20
    , const int total_jobs              = 1000
    , const double min_calo_pt          = 0.05
    , const std::string &recojetname    = "AntiKt_Tower_r04"
  );

  const double min_calo_pt;
  virtual ~RhoMedianFluct();

  //! set eta range
  void setPtRange(double low, double high)
  {
    m_ptRange.first  = low;
    m_ptRange.second = high;
  }
  /* void use_initial_vertex(const bool b = true) {initial_vertex = b;} */
  int Init          (PHCompositeNode *topNode);
  int InitRun       (PHCompositeNode *topNode);
  int process_event (PHCompositeNode *topNode);
  int End           (PHCompositeNode *topNode);
  void clear_vectors();
  void add_input(JetInput *input) { _inputs.push_back(input); }

 private:
  int nevent {0};
  std::string m_recoJetName;
  /* std::string m_truthJetName; */
  std::string m_outputFileName;

  //! pT range
  std::pair<double, double> m_ptRange;

  //! flag to use initial vertex in track evaluator
  bool initial_vertex = false;

  //! Output Tree variables
  TTree *m_T;
  TH2D* h2d_nHcalOut_vs_cen;
  TH2D* h2d_nHcalIn_vs_cen;
  TH2D* h2d_nCEMC_vs_cen;
  array<TH2D*,3> arr_n_v_cen;

  float m_cent_mdb { -1. };
  float m_cent_epd { -1. };
  float m_impactparam {0.};

  float m_embEta {0.};
  float m_embPhi {0.};

  float m_embPt; // all calorimeters
  float m_embPt_HCALIn  {0.};
  float m_embPt_HCALOut {0.};
  float m_embPt_HCAL    {0.};
  float m_embPt_EMCAL   {0.};
  vector<float*> v_embPt {&m_embPt, &m_embPt_HCALIn, &m_embPt_HCALOut, &m_embPt_HCAL, &m_embPt_EMCAL};

  float m_fluct; // all calorimeters
  float m_fluct_HCALIn  ;
  float m_fluct_HCALOut ;
  float m_fluct_HCAL    ;
  float m_fluct_EMCAL   ;
  vector<float*> v_fluct {&m_fluct, &m_fluct_HCALIn, &m_fluct_HCALOut, &m_fluct_HCAL, &m_fluct_EMCAL};

  float m_rho; // all calorimeters
  float m_rho_HCALIn;
  float m_rho_HCALOut;
  float m_rho_HCAL;
  float m_rho_EMCAL;
  vector<float*> v_rho {&m_rho, &m_rho_HCALIn, &m_rho_HCALOut, &m_rho_HCAL, &m_rho_EMCAL};

  float m_rhosigma; // all calorimeters
  float m_rhosigma_HCALIn;
  float m_rhosigma_HCALOut;
  float m_rhosigma_HCAL;
  float m_rhosigma_EMCAL;
  vector<float*> v_rhosigma {&m_rhosigma, &m_rhosigma_HCALIn, &m_rhosigma_HCALOut, &m_rhosigma_HCAL, &m_rhosigma_EMCAL};

  //Calo Jets -- all
  std::vector<float> m_CaloJetEta        {};
  std::vector<float> m_CaloJetPhi        {};
  std::vector<float> m_CaloJetE          {};
  std::vector<float> m_CaloJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_CaloJetArea       {};

  //HCAL-In jets
  std::vector<float> m_HCALInJetEta        {};
  std::vector<float> m_HCALInJetPhi        {};
  std::vector<float> m_HCALInJetE          {};
  std::vector<float> m_HCALInJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_HCALInJetArea       {};

  //HCAL-Out jets
  std::vector<float> m_HCALOutJetEta        {};
  std::vector<float> m_HCALOutJetPhi        {};
  std::vector<float> m_HCALOutJetE          {};
  std::vector<float> m_HCALOutJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_HCALOutJetArea       {};

  //HCAL jets in+out
  std::vector<float> m_HCALJetEta        {};
  std::vector<float> m_HCALJetPhi        {};
  std::vector<float> m_HCALJetE          {};
  std::vector<float> m_HCALJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_HCALJetArea       {};
  //
  //EMCal jets 
  std::vector<float> m_EMCALJetEta        {};
  std::vector<float> m_EMCALJetPhi        {};
  std::vector<float> m_EMCALJetE          {};
  std::vector<float> m_EMCALJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_EMCALJetArea       {};

  std::vector<std::vector<float>*> v_JetEta        {&m_CaloJetEta, &m_HCALInJetEta, &m_HCALOutJetEta, &m_HCALJetEta, &m_EMCALJetEta};
  std::vector<std::vector<float>*> v_JetPhi        {&m_CaloJetPhi, &m_HCALInJetPhi, &m_HCALOutJetPhi, &m_HCALJetPhi, &m_EMCALJetPhi};
  std::vector<std::vector<float>*> v_JetE          {&m_CaloJetE, &m_HCALInJetE, &m_HCALOutJetE, &m_HCALJetE, &m_EMCALJetE};
  std::vector<std::vector<float>*> v_JetPtLessRhoA {&m_CaloJetPtLessRhoA, &m_HCALInJetPtLessRhoA, &m_HCALOutJetPtLessRhoA, &m_HCALJetPtLessRhoA, &m_EMCALJetPtLessRhoA};
  std::vector<std::vector<float>*> v_JetArea       {&m_CaloJetArea, &m_HCALInJetArea, &m_HCALOutJetArea, &m_HCALJetArea, &m_EMCALJetArea};

  float mBGE_mean_area          { 0 };
  float mBGE_empty_area         { 0 };
  float mBGE_n_empty_jets       { 0 };
  unsigned int mBGE_n_jets_used { 0 };

  std::vector<float> m_BGE_JetEta      {};
  std::vector<float> m_BGE_JetPhi      {};
  std::vector<float> m_BGE_JetPt       {};
  std::vector<float> m_BGE_JetArea     {};
  
  // in interesting (lo, mi, or hi) events: Towers
  std::vector<float> m_CEMCtowEta {};
  std::vector<float> m_CEMCtowPhi {};
  std::vector<float> m_CEMCtowPt {};

  std::vector<float> m_HCALINtowEta {};
  std::vector<float> m_HCALINtowPhi {};
  std::vector<float> m_HCALINtowPt {};

  std::vector<float> m_HCALOUTtowEta {};
  std::vector<float> m_HCALOUTtowPhi {};
  std::vector<float> m_HCALOUTtowPt {};

  vector<vector<float>*> v_towEta { &m_CEMCtowEta, &m_HCALINtowEta, &m_HCALOUTtowEta };
  vector<vector<float>*> v_towPhi { &m_CEMCtowPhi, &m_HCALINtowPhi, &m_HCALOUTtowPhi };
  vector<vector<float>*> v_towPt  { &m_CEMCtowPt,  &m_HCALINtowPt,  &m_HCALOUTtowPt  };

  std::vector<JetInput *> _inputs; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
  MemTimeProgression print_stats;
  TRandom3 rng;
};

#endif  // JETPLUSBACKGROUND_H_H
