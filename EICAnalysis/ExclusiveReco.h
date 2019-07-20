#ifndef __ExclusiveReco_H__
#define __ExclusiveReco_H__

#include "TrackProjectorPlaneECAL.h"
/* Fun4All includes */
#include <fun4all/SubsysReco.h>
#include <calobase/RawTowerDefs.h>

/* STL includes */
#include <math.h>
#include <map>

/*HepMC include */
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#include <HepMC/GenEvent.h>
class TTree;
class TFile;

class PHCompositeNode;
class CaloEvalStack;
class CaloRawTowerEval;
class RawCluster;
class RawTowerContainer;
class RawTowerGeomContainer;
//class JetMap;
class SvtxTrack;
class SvtxTrackMap;
class PHHepMCGenEventMap;


class ExclusiveReco : public SubsysReco
{

public:

  ExclusiveReco(std::string filename);

  int
  Init(PHCompositeNode*);
  int
  InitRun(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

  void
  set_beam_energies( float beam_electron , float beam_proton )
  {
    _beam_electron_ptotal = fabs( beam_electron );
    _beam_hadron_ptotal = fabs( beam_proton );
  }
  
  void
  set_do_process_dvmp( bool select)
  {
    _do_process_dvmp = select;
  }
  
  

private:

  /* proton rest mass */
  const float _mproton;

  bool _verbose;
  bool _save_towers;
  bool _save_tracks;
  bool _do_process_dvmp;

  int _ievent;
  int _total_pass;

  std::string _filename;
  TFile *_tfile;

  /* output tree for invariant mass calculation */
  TTree* _tree_invariant_mass;

  /* output tree for reco particles */
  TTree* _tree_event_reco;

  /* output tree for truth particles */
  TTree* _tree_event_truth;

  /* beam energies electron and proton */
  float _beam_electron_ptotal;
  float _beam_hadron_ptotal;

  /* Track Projector object */
  TrackProjectorPlaneECAL* _trackproj;

  /** helper pointer to topNode */
  PHCompositeNode *_topNode;

  int AddInvariantMassInformation();
  
  std::vector<float> true_eta, true_phi, true_ptotal;
  std::vector<int> true_pid;
  std::vector<bool> is_scattered_lepton;

  std::vector<float> reco_eta, reco_phi, reco_ptotal, reco_cluster_e;
  std::vector<int> reco_charge;
  std::vector<bool> reco_is_scattered_lepton;
  
  std::vector<float> _vect1;
  std::vector<float> _vect2;
  std::vector<float> _vect3;
  std::vector<float> _vect4;
  std::vector<float> _vect5;
  std::vector<float> _vect6;

};

#endif // __ExclusiveReco_H__
