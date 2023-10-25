#ifndef RHOFLUCT__H
#define RHOFLUCT__H

#include <fun4all/SubsysReco.h>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair

#include <array>
#include <vector>

#include <TRandom3.h>

class PHCompositeNode;
class JetEvalStack;
class TTree;
class TH2D;
class TH1;
class JetInput;

class RhoFluct : public SubsysReco
{
 public:
  RhoFluct
    ( const std::string& outputfilename = "RhoFluct.root"
    , const float _jet_R = 0.4
    );

  virtual ~RhoFluct();

  /* void use_initial_vertex(const bool b = true) {initial_vertex = b;} */
  int Init          (PHCompositeNode *topNode);
  int InitRun       (PHCompositeNode *topNode);
  int process_event (PHCompositeNode *topNode);
  int End           (PHCompositeNode *topNode);
  void add_input_Sub1(JetInput *input) { m_input_Sub1.push_back(input); }
  void add_input_rhoA(JetInput *input) { m_input_rhoA.push_back(input); }

 private:
  TRandom3 rand3 {};

  long long int nevent { 0 };
  long long int outevent { 0 };
  const std::string m_outputfilename;
  const float jet_R;
  const std::string m_truthJetName;
  const std::string m_Sub1JetName;

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
  float m_1TeV_phi ; // always embed a 100 GeV particle
  float m_1TeV_eta ;


  bool   m_rhoA_ismatched;
  float  m_rhoAJetPhi ;
  float  m_rhoAJetEta ;
  float  m_rhoAJetPt  ;
  float  m_rhoAJetArea  ;
  float  m_rhoAJetPtLessRhoA ; // pT - rho x A
  float  m_rhoAJet_delpt; // (pT-rhoxA)-1TeV

  bool   m_Sub1_ismatched;
  float  m_Sub1JetPhi ;
  float  m_Sub1JetEta ;
  float  m_Sub1JetPt  ;
  float   m_Sub1Jet_delpt ; // pT - 1TeV

  // FIXME -- functions straight from background median estimator
  float        mBGE_mean_area    { 0 };
  float        mBGE_empty_area   { 0 };
  float        mBGE_n_empty_jets { 0 };
  unsigned int mBGE_n_jets_used  { 0 };

  std::vector<JetInput *> m_input_Sub1, m_input_rhoA; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
};

#endif  // RhoFluct
