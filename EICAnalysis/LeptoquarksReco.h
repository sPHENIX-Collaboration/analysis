#ifndef __LeptoquarksReco_H__
#define __LeptoquarksReco_H__

#include "TauCandidate.h"

/* Fun4All includes */
#include <fun4all/SubsysReco.h>

/* STL includes */
#include <math.h>
#include <map>

/*HepMC include */
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

class TNtuple;
class TTree;
class TFile;

class PHCompositeNode;
class CaloRawTowerEval;
class RawTowerContainer;
class RawTowerGeomContainer;
class JetMap;
class SvtxTrackMap;
class PHHepMCGenEventMap;


class TauCandidate;

typedef std::map<float, TauCandidate*> map_tcan;

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
  set_save_towers( bool savetower=true )
  {
    _save_towers = savetower;
  }


  void
  set_save_tracks( bool savetracks=true )
  {
    _save_tracks = savetracks;
  }


  void
  set_beam_energies( float beam_electron , float beam_proton )
  {
    _ebeam_E = fabs( beam_electron );
    _pbeam_E = fabs( beam_proton );
  }

  void
  set_reco_jet_collection( std::string newname )
  {
    _jetcolname = newname;
  }

private:

  bool _verbose;
  bool _save_towers;
  bool _save_tracks;

  int _ievent;
  int _total_pass;

  std::string _filename;
  TFile *_tfile;

  /* output tree and variables */
  TTree* _t_candidate;
  TNtuple* _ntp_jet;
  TNtuple* _ntp_jet2;
  TNtuple* _ntp_tower;
  TNtuple* _ntp_track;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;

  /* set minimum energy for tau jets to consider */
  float _tau_jet_emin;

  /** name of jet collection */
  std::string _jetcolname;

  /** CaloRawTowerEvaluators to access tru particle info for
   * given towers */
  std::map< std::string, CaloRawTowerEval* > _map_towereval;

  /** Map of TauCandidate properties that will be written to
   * output ROOT Tree */
  std::map< TauCandidate::PROPERTY , float > _map_treebranches;

  int AddTrueTauTag( map_tcan&, PHHepMCGenEventMap* );

  int AddJetStructureInformation( map_tcan&, JetMap*, std::vector<RawTowerContainer*>, std::vector<RawTowerGeomContainer*> );

  int AddTrackInformation( map_tcan&, SvtxTrackMap* );

  int WriteTauCandidatesToTree( map_tcan& );

  /** Find tau candidate in map that is closest to given eta, phi angle */
  TauCandidate* FindMinDeltaRCandidate( map_tcan*, const float, const float );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );

};

#endif // __LeptoquarksReco_H__
