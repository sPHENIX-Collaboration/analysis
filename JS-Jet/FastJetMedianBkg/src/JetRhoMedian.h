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
  std::string m_recoJetName;
  std::string m_truthJetName;
  std::string m_outputFileName;

  //! eta range
  std::pair<double, double> m_etaRange;

  //! pT range
  std::pair<double, double> m_ptRange;

  //! flag to use initial vertex in track evaluator
  bool initial_vertex = false;

  //! Output Tree variables
  TTree *m_T;
  int   m_id;
  float m_pTlb {-1. }; // pT Lower Bound, 0. for MB, 10 for 10 GeV Jet Embedding, 30 for 30 GeV Jet Embedding
  float m_rho;
  float m_rho_sigma;
  float m_centrality;
  float m_impactparam;

  //Calo Jets
  std::vector<float> m_CaloJetEta;
  std::vector<float> m_CaloJetPhi;
  std::vector<float> m_CaloJetE;
  std::vector<float> m_CaloJetPtLessRhoA; // pT - rho x A
  std::vector<float> m_CaloJetArea;

  //Truth Jets
  std::vector<float> m_TruthJetEta;
  std::vector<float> m_TruthJetPhi;
  std::vector<float> m_TruthJetE;
  std::vector<float> m_TruthJetPt;
  std::vector<float> m_TruthJetArea;

  std::vector<JetInput *> m_inputs; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
  MemTimeProgression print_stats;
};

#endif  // JETRHOMEDIAN__H
