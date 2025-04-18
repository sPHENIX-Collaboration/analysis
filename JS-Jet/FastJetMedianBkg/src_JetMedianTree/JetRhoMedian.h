#ifndef JETRHOMEDIAN__H
#define JETRHOMEDIAN__H

#include <fun4all/SubsysReco.h>

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
    , const float        _jet_R          = 0.4
    , const std::string& truthjetname   = "AntiKt_Truth_r04"
    , const std::string& sub1jetname   = "AntiKt_Truth_r04"
    , const float min_lead_truth_pt = 10.
    );

  virtual ~JetRhoMedian();

  /* void use_initial_vertex(const bool b = true) {initial_vertex = b;} */
  int Init          (PHCompositeNode *topNode);
  int InitRun       (PHCompositeNode *topNode);
  int process_event (PHCompositeNode *topNode);
  int End           (PHCompositeNode *topNode);
  void clear_vectors();
  void add_input(JetInput *input) { m_inputs.push_back(input); }

 private:
  long long int nevent { 0 };
  long long int outevent { 0 };
  const std::string m_outputfilename;
  const float jet_R;
  const std::string m_truthJetName;
  const std::string m_sub1JetName;
  const float m_min_lead_truth_pt;

  int i_truthset = 0;

  std::string m_recoJetName;
  //! Output Tree variables
  TTree *m_T;

  /* int   m_id; */
  float m_rho; // all calorimeters
  float m_rho_sigma;
  float m_cent_mdb    { -1. };
  float m_cent { -1. };
  float m_cent_epd    { -1. };
  float m_impactparam { -1. };

  //Calo Jets -- all
  std::vector<float> m_CaloJetEta        {};
  std::vector<float> m_CaloJetPhi        {};
  std::vector<float> m_CaloJetPt          {};
  std::vector<float> m_CaloJetPtLessRhoA {}; // pT - rho x A
  std::vector<float> m_CaloJetArea       {};

  //Truth Jets
  std::vector<float> m_TruthJetEta  {};
  std::vector<float> m_TruthJetPhi  {};
  std::vector<float> m_TruthJetPt   {};

  //SUB1 Jets
  std::vector<float> m_Sub1JetEta  {};
  std::vector<float> m_Sub1JetPhi  {};
  std::vector<float> m_Sub1JetPt   {};

  // FIXME -- functions straight from background median estimator
  float        mBGE_mean_area    { 0 };
  float        mBGE_empty_area   { 0 };
  float        mBGE_n_empty_jets { 0 };
  unsigned int mBGE_n_jets_used  { 0 };

  std::vector<JetInput *> m_inputs; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
};

#endif  // JETRHOMEDIAN__H
