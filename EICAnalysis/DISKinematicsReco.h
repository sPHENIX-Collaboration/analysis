#ifndef __DISKinematicsReco_H__
#define __DISKinematicsReco_H__

#include "PidCandidate.h"
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


class PidCandidate;

typedef std::map<float, PidCandidate*> type_map_tcan;
typedef std::map< RawTowerDefs::CalorimeterId , std::pair< RawTowerContainer*, RawTowerGeomContainer* > > type_map_cdata;

class DISKinematicsReco : public SubsysReco
{

public:

  DISKinematicsReco(std::string filename);

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
  set_do_process_geant4_cluster( bool select )
  {
    _do_process_geant4_cluster = select;
  }

  void
  set_do_process_truth( bool select )
  {
    _do_process_truth = select;
  }
  

private:

  /* proton rest mass */
  const float _mproton;

  bool _verbose;
  bool _save_towers;
  bool _save_tracks;
  bool _do_process_geant4_cluster;
  bool _do_process_truth;

  int _ievent;
  int _total_pass;

  std::string _filename;
  TFile *_tfile;

  /* output tree and variables */
  TTree* _tree_event_cluster;

  /* output tree and variables from TRUTH particles */
  TTree* _tree_event_truth;

  /* beam energies electron and proton */
  float _beam_electron_ptotal;
  float _beam_hadron_ptotal;

  /** CaloRawTowerEvaluators to access tru particle info for
   * given towers */
  std::map< std::string, CaloRawTowerEval* > _map_towereval;
  
  /* Track Projector object */
  TrackProjectorPlaneECAL* _trackproj;
  //  /** Map of PidCandidate properties that will be written to
  //   * output ROOT Tree */
  //  std::map< PidCandidate::PROPERTY , float > _map_treebranches;

  /** Map of EM Candidates that will be written to
   * output ROOT Tree */
  std::map< PidCandidate::PROPERTY , std::vector< float > > _map_em_candidate_branches;

  /** Map of Event properties that will be written to
   * output ROOT Tree */
  std::map< std::string , float > _map_event_branches;

  /** helper pointer to topNode */
  PHCompositeNode *_topNode;

  int CollectEmCandidatesFromCluster( type_map_tcan& );

  int CollectEmCandidatesFromTruth( type_map_tcan& );

  int InsertCandidateFromCluster( type_map_tcan& , RawCluster* , CaloEvalStack* , SvtxTrackMap*);

  int AddGlobalCalorimeterInformation();

  int AddReconstructedKinematics( type_map_tcan& , std::string );

  int WriteCandidatesToTree( type_map_tcan& );

  int AddTruthEventInformation();

  /** Find tau candidate in map that is closest to given eta, phi angle */
  PidCandidate* FindMinDeltaRCandidate( type_map_tcan*, const float, const float );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );

  /** Reset branch maps for each event */
  void ResetBranchMap();

  
};

#endif // __DISKinematicsReco_H__
