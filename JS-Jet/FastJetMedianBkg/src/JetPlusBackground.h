#ifndef JETPLUSBACKGROUND_H
#define JETPLUSBACKGROUND_H

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
class JetInput;

class JetPlusBackground : public SubsysReco
{
 public:
  JetPlusBackground(
      const double min_calo_pt          = 0.02,
      const int total_jobs              = 0,
      const int n_print_freq            = 10,
      const std::string &recojetname    = "AntiKt_Tower_r04",
      /* const std::string &truthjetname   = "AntiKt_Truth_r04", */
      const std::string &outputfilename = "JetPlusBackground.root");

  const double min_calo_pt;
  virtual ~JetPlusBackground();

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
  void add_input(JetInput *input) { _inputs.push_back(input); }

 private:
  std::string m_recoJetName;
  /* std::string m_truthJetName; */
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
  float m_rho;
  float m_rho_sigma;
  float m_centrality;
  float m_impactparam;

  float m_RhoBias;

  //Calo Jets -- Note: the event will be rejected if the leading jet does not
  //             contain the embedded particle
  std::vector<float> m_CaloJetEta;
  std::vector<float> m_CaloJetPhi;
  std::vector<float> m_CaloJetE;
  std::vector<float> m_CaloJetPt;
  std::vector<float> m_CaloJetArea;

  // embedded particle
  float m_embEta;
  float m_embPhi;
  float m_embPt;

  //Truth Jets -> only fill for the single leading leading jet
  /* std::vector<float> m_TruthJetEta; */
  /* std::vector<float> m_TruthJetPhi; */
  /* std::vector<float> m_TruthJetE; */
  /* std::vector<float> m_TruthJetPt; */
  /* std::vector<float> m_TruthJetArea; */

  std::vector<JetInput *> _inputs; // copied from /direct/sphenix+u/dstewart/vv/coresoftware/simulation/g4simulation/g4jets/JetReco.h .cc
  MemTimeProgression print_stats;
  TRandom3 rng;

  /* bool jet_has_index(vector<fastjet::PseudoJet>& jets, int which_jet, int index); */
  bool jetGT2_has_index(vector<fastjet::PseudoJet>& jets, int index);
};

#endif  // JETPLUSBACKGROUND_H_H
