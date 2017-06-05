#ifndef __RICHEvaluator_H__
#define __RICHEvaluator_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TTree;
class TFile;
class TH1D;

class PHCompositeNode;
class PHG4TruthInfoContainer;
class PHG4Hit;
class PHG4HitContainer;

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

  /* Truth info container */
  PHG4TruthInfoContainer* _truthinfo;

  /* Hit collection storing RICH photon hits */
  std::string _richhits_name;
  PHG4HitContainer* _richhits;

  /* ROOT file to store output ROOT tree */
  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TTree* _tree_rich;

  float _hit_x0;
  float _hit_y0;
  float _hit_z0;

  float _hit_lx0;
  float _hit_ly0;
  float _hit_lz0;

  float _track_px;
  float _track_py;
  float _track_pz;

  float _track_e;
  float _edep;

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
