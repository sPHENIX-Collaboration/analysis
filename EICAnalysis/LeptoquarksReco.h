#ifndef __LeptoquarksReco_H__
#define __LeptoquarksReco_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TNtuple;
class TFile;
class TH1D;

class PHCompositeNode;
namespace HepMC
{
  class GenEvent;
}

class LeptoquarksReco : public SubsysReco
{

public:

  LeptoquarksReco(std::string filename);

  int
  Init(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

  void
  set_beam_energies( float beam_electron , float beam_proton )
  {
    _ebeam_E = fabs( beam_electron );
    _pbeam_E = fabs( beam_proton );
  }

private:

  bool _verbose;

  int _ievent;
  int _total_pass;

  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TNtuple* _tree_dis;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;
};

#endif // __LeptoquarksReco_H__
