#ifndef __HFJetTruthTrigger_H__
#define __HFJetTruthTrigger_H__


// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>
#include <cmath>
#include <limits.h>

class TTree;
class TFile;
class TH2D;

class PHCompositeNode;

class HFJetTruthTrigger: public SubsysReco
{

 public:

  HFJetTruthTrigger(std::string filename, int flavor, int maxevent = INT_MAX);

  int Init(PHCompositeNode*);
  int process_event(PHCompositeNode*);
  int End(PHCompositeNode*);

  float deltaR( float eta1, float eta2, float phi1, float phi2) {

    float deta = eta1 - eta2;
    float dphi = phi1 - phi2;
    if (dphi > +3.14159) dphi -= 2*3.14159;
    if (dphi < -3.14159) dphi += 2*3.14159;

    return sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );

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

 private:

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


};

#endif // __HFJetTruthTrigger_H__
