#ifndef __LeptoquarksReco_H__
#define __LeptoquarksReco_H__

#include "PidCandidate.h"

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
class TNtuple;
class TTree;
class TFile;

class PHCompositeNode;
class CaloRawTowerEval;
class RawTowerContainer;
class RawTowerGeomContainer;
class JetMap;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxEvalStack;
class PHHepMCGenEventMap;

class PidCandidate;

typedef std::map<float, PidCandidate*> type_map_tcan;
typedef std::map< RawTowerDefs::CalorimeterId , std::pair< RawTowerContainer*, RawTowerGeomContainer* > > type_map_cdata;

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
  TTree* _t_event;
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

  /** Map of PidCandidate properties that will be written to
   * output ROOT Tree */
  std::map< PidCandidate::PROPERTY , std::vector< float > > _map_tau_candidate_branches;

  /** Map of Event properties that will be written to
   * output ROOT Tree */
  std::map< std::string , float > _map_event_branches;

  int AddTrueTauTag( type_map_tcan&, PHHepMCGenEventMap* );

  int AddJetInformation( type_map_tcan&, JetMap*, type_map_cdata* );

  int AddJetStructureInformation( type_map_tcan&, type_map_cdata* );

  int AddTrackInformation( type_map_tcan&, SvtxTrackMap*, SvtxVertexMap*, SvtxEvalStack*,double );

  int WritePidCandidatesToTree( type_map_tcan& );

  int AddGlobalEventInformation( type_map_tcan& , type_map_cdata* );

  /** Find tau candidate in map that is closest to given eta, phi angle */
  PidCandidate* FindMinDeltaRCandidate( type_map_tcan*, const float, const float );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );

  /** Reset branch maps for each event */
  void ResetBranchMap();

  /** Enum to identify calorimeter types */
  enum CALOTYPE
  {
    calo_cemc,
    calo_ihcal,
    calo_ohcal,
    calo_femc,
    calo_fhcal,
    calo_eemc
  };

};

#endif // __LeptoquarksReco_H__
