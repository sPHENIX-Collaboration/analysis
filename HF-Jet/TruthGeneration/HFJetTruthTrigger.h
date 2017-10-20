#ifndef __HFJetTruthTrigger_H__
#define __HFJetTruthTrigger_H__

// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>
#include <cmath>
#include <string>
#include <limits.h>

class TTree;
class TFile;
class TH2D;

class PHCompositeNode;
class Jet;
namespace HepMC
{
  class GenEvent;
}

class HFJetTruthTrigger : public SubsysReco
{

public:

  HFJetTruthTrigger(std::string filename, int flavor = 5, std::string jet_node = "AntiKt_Truth_r04", int maxevent = INT_MAX);

  int
  Init(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

  float
  deltaR(float eta1, float eta2, float phi1, float phi2)
  {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if (dphi > +3.14159)
      dphi -= 2 * 3.14159;
    if (dphi < -3.14159)
      dphi += 2 * 3.14159;

    return sqrt(pow(deta, 2) + pow(dphi, 2));

  }

  double
  get_eta_max() const
  {
    return _eta_max;
  }

  void
  set_eta_max(double etaMax)
  {
    _eta_max = etaMax;
  }

  double
  get_eta_min() const
  {
    return _eta_min;
  }

  void
  set_eta_min(double etaMin)
  {
    _eta_min = etaMin;
  }

  double
  get_pt_max() const
  {
    return _pt_max;
  }

  void
  set_pt_max(double ptMax)
  {
    _pt_max = ptMax;
  }

  double
  get_pt_min() const
  {
    return _pt_min;
  }

  void
  set_pt_min(double ptMin)
  {
    _pt_min = ptMin;
  }

  //! action to take if no jet fitting _flavor requirement found. Action defined in <fun4all/Fun4AllReturnCodes.h>
  //! Default action is DISCARDEVENT for DST level filtering, one can further choose ABORTEVENT to use it as processing level filtering
  void
  set_rejection_action(int action)
  {
    _rejection_action = action;
  }

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int get_embedding_id() const { return _embedding_id; }
  //
  //! The embedding ID for the HepMC subevent to be analyzed.
  //! embedding ID for the event
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  void set_embedding_id(int id) { _embedding_id = id; }
private:

  //! tag jet flavor by parton matching, like PRL 113, 132301 (2014)
  int
  parton_tagging(Jet * jet, HepMC::GenEvent*, const double match_radius);

  //! tag jet flavor by hadron matching, like MIE proposal
  int
  hadron_tagging(Jet * jet, HepMC::GenEvent*, const double match_radius);

  bool _verbose;

  int _ievent;
  int _total_pass;

  TFile *_f;

  TH2D *_h2;
  TH2D *_h2all;
  TH2D *_h2_b;
  TH2D *_h2_c;

  std::string _foutname;

  int _flavor;
  int _maxevent;

  double _pt_min;
  double _pt_max;

  double _eta_min;
  double _eta_max;

  std::string _jet_name;

  //! action to take if no jet fitting _flavor requirement found. Action defined in <fun4all/Fun4AllReturnCodes.h>
  int _rejection_action;

  //! The embedding ID for the HepMC subevent to be analyzed.
  //! positive ID is the embedded event of interest, e.g. jetty event from pythia
  //! negative IDs are backgrounds, .e.g out of time pile up collisions
  //! Usually, ID = 0 means the primary Au+Au collision background
  int _embedding_id;

};

#endif // __HFJetTruthTrigger_H__
