#ifndef __RICHEvaluator_H__
#define __RICHEvaluator_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TTree;
class TFile;
class TH1D;

class PHCompositeNode;

class RICHEvaluator : public SubsysReco
{

public:

  RICHEvaluator(std::string filename, std::string richname);

  int
  Init(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);


private:

  /* reset all output tree variables */
  void reset_tree_vars();

  /* initialize output tree */
  int init_tree();

  bool _verbose;

  int _ievent;

  /* Hit collection storing RICH photon hits */
  std::string _richhits_name;

  /* ROOT file to store output ROOT tree */
  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TTree* _tree_rich;

  double _hit_x0;
  double _hit_y0;
  double _hit_z0;

  double _hit_lx0;
  double _hit_ly0;
  double _hit_lz0;

  double _track_px;
  double _track_py;
  double _track_pz;

  double _track_e;
  double _edep;

  int _bankid;
  int _volumeid;
  int _hitid;
  int _pid;
  int _mpid;
  int _trackid;
  int _mtrackid;
  int _otrackid;

};

#endif // __RICHEvaluator_H__
