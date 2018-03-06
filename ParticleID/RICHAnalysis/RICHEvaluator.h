#ifndef __RICHEvaluator_H__
#define __RICHEvaluator_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TTree;
class TFile;
class TH1D;

class PHCompositeNode;
class SvtxTrack_FastSim;
class SvtxTrackMap;

class RICHEvaluator : public SubsysReco
{

public:

  RICHEvaluator(std::string tracksname, std::string richname, std::string filename);

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

  /** get position from track state */
  bool get_position_from_track_state(  SvtxTrack_FastSim * track, std::string statename, double arr_pos[3] );

  /** get track momentum from track state */
  bool get_momentum_from_track_state( SvtxTrack_FastSim * track, std::string statename, double arr_mom[3] );
  
  bool _verbose;

  int _ievent;

  /* Track collection */
  std::string _trackmap_name;

  /* Hit collection storing RICH photon hits */
  std::string _richhits_name;

  /* Name of the track state inside the RICH radiator volume */
  std::string _trackstate_name;

  /* ROOT file to store output ROOT tree */
  std::string _foutname;
  TFile *_fout_root;

  /* output tree and variables */
  TTree* _tree_rich;
  TTree* _tree_irt;

  double _hit_x0;
  double _hit_y0;
  double _hit_z0;

  double _hit_lx0;
  double _hit_ly0;
  double _hit_lz0;

  double _emi_x;
  double _emi_y;
  double _emi_z;

  double _track_px;
  double _track_py;
  double _track_pz;

  double _mtrack_px;
  double _mtrack_py;
  double _mtrack_pz;

  double _track_e;
  double _mtrack_e;
  double _edep;

  int _bankid;
  int _volumeid;
  int _hitid;
  int _pid;
  int _mpid;
  int _trackid;
  int _mtrackid;
  int _otrackid;

  double _Ex;
  double _Ey;
  double _Ez;
  double _Dx;
  double _Dy;
  double _Dz;
  double _Vx;
  double _Vy;
  double _Vz;
  double _Cx;
  double _Cy;
  double _Cz;


};

#endif // __RICHEvaluator_H__
