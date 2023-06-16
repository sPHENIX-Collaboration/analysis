#ifndef JETRHOMEDIAN__H
#define JETRHOMEDIAN__H

#include <fun4all/SubsysReco.h>
#include "MemTimeProgression.h"

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair

#include <array>
#include <vector>

class PHCompositeNode;
class JetEvalStack;
class TTree;
class TH2D;
class TH1;
class JetInput;

class JetRhoMedian : public SubsysReco
{
 public:
  JetRhoMedian
    ( const std::string& outputfilename = "JetRhoMedian.root"
    , const float        pTlb           = -1. 
    , const int          n_print_freq   = 500    // useful for print freq.
    , const int          total_jobs     = 1000000  // useful for printing frequency
    , const double       min_calo_pt    = 0.05
    , const std::string& truthjetname   = "AntiKt_Truth_r04"
    );

  const double m_min_calo_pt;
  virtual ~JetRhoMedian();

  //! set eta range
  void setEtaRange(double low, double high)
  {
    m_etaRange.first  = low;
    m_etaRange.second = high;
  }
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
  void add_input(JetInput *input) { m_inputs.push_back(input); }

 private:
  long long int nevent { 0 };
  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;

  //! eta range
  std::pair<double, double> m_etaRange;

  //! pT range
  std::pair<double, double> m_ptRange;

  //! flag to use initial vertex in track evaluator
  /* bool initial_vertex = false; */

  //! Output Tree variables
  TTree *m_T;
  TH2D* h2d_nHcalOut_vs_cen;
  TH2D* h2d_nHcalIn_vs_cen;
  TH2D* h2d_nCEMC_vs_cen;
  array<TH2D*,3> arr_n_v_cen;
     /* { &h2d_nCEMC_vs_cen, &h2d_nHcalIn_vs_cen, &h2d_nHcalOut_vs_cen }; */

  /* int   m_id; */
  float m_pTlb; // pT Lower Bound, 0. for MB, 10 for 10 GeV Jet Embedding, 30 for 30 GeV Jet Embedding
  float m_rho; // all calorimeters
  float m_rho_HCALIn;
  float m_rho_HCALOut;
  float m_rho_HCAL;
  float m_rho_EMCAL;
  float m_rho_sigma;
  float m_cent_mdb { -1. };
  float m_cent_epd { -1. };
  float m_impactparam { -1. };

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
  //
  //HCAL-Out jets
  std::vector<float> m_HCALOutJetEta        {};
  std::vector<float> m_HCALOutJetPhi        {};
  std::vector<float> m_HCALOutJetE          {};
  std::vector<float> m_HCALOutJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_HCALOutJetArea       {};
  //
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

  //Truth Jets
  std::vector<float> m_TruthJetEta  {};
  std::vector<float> m_TruthJetPhi  {};
  std::vector<float> m_TruthJetE    {};
  std::vector<float> m_TruthJetPt   {};
  std::vector<float> m_TruthJetArea {};

  // FIXME -- functions straight from background median estimator
  float        mBGE_mean_area    { 0 };
  float        mBGE_empty_area   { 0 };
  float        mBGE_n_empty_jets { 0 };
  unsigned int mBGE_n_jets_used  { 0 };

  std::vector<float> m_BGE_JetEta   {};
  std::vector<float> m_BGE_JetPhi   {};
  std::vector<float> m_BGE_JetPt    {};
  std::vector<float> m_BGE_JetArea  {};

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

  // kt jets
  /* std::vector<float> m_kTjetEta {}; */
  /* std::vector<float> m_kTjetPhi {}; */
  /* std::vector<float> m_kTjetPt {}; */ 
  /* std::vector<float> m_kTjetArea {}; */

  std::vector<JetInput *> m_inputs; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
  MemTimeProgression print_stats;
};

#endif  // JETRHOMEDIAN__H
