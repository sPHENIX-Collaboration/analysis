#ifndef PRINTTOWERS___H
#define PRINTTOWERS___H

#include <fun4all/SubsysReco.h>
#include <jetbase/Jet.h>

#include <memory>
#include <string>
#include <utility>  // std::pair, std::make_pair

#include <array>
#include <vector>

class Jet;
class PHCompositeNode;
class JetEvalStack;
class TTree;
class TH2D;
class TH1;
class JetInput;

class PrintTowers : public SubsysReco
{
 public:
  PrintTowers
    (
        std::vector<std::string> jet_names
      , std::vector<std::pair<std::string,std::string>> matchlist = {}
      , std::vector<std::pair<Jet::SRC,std::string>> tower_sources = {}
      , const float min_jet_pt = 5.
      , const float m_tow_tow_pt = 0.01
    /* , const float        _jet_R          = 0.4 */
    /* , const std::string& truthjetname   = "AntiKt_Truth_r04" */
    /* , const std::string& sub1jetname   = "AntiKt_Truth_r04" */
    /* , const float min_lead_truth_pt = 10. */
    );

  virtual ~PrintTowers();
  float m_max_cent = 100.;
  int nmax_jetprint = 10.;
  float m_pt_min_match = 30.;

  /* void use_initial_vertex(const bool b = true) {initial_vertex = b;} */
  int Init          (PHCompositeNode *topNode);
  int InitRun       (PHCompositeNode *topNode);
  int process_event (PHCompositeNode *topNode);
  int End           (PHCompositeNode *topNode);
  void clear_vectors();
  /* void add_input(JetInput *input) { m_inputs.push_back(input); } */

 private:

  const std::vector<std::string> m_jet_names;
  const std::vector<std::pair<std::string,std::string>> m_matchlist;
  const std::vector<std::pair<Jet::SRC,std::string>> m_SRC;
  const float m_min_jet_pt;
  const float m_min_tow_pt;
  int nevnt {0};

};

#endif  // JETRHOMEDIAN__H
