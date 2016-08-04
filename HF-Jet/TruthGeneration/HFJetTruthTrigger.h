#ifndef __HFJetTruthTrigger_H__
#define __HFJetTruthTrigger_H__


// --- need to check all these includes...
#include <fun4all/SubsysReco.h>
#include <vector>
#include <cmath>


class TTree;
class TFile;
class TH2D;

class PHCompositeNode;

class HFJetTruthTrigger: public SubsysReco
{

 public:

  HFJetTruthTrigger(std::string filename, int flavor, int maxevent);

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

};

#endif // __HFJetTruthTrigger_H__
