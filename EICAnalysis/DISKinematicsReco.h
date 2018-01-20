#ifndef __DISKinematicsReco_H__
#define __DISKinematicsReco_H__

#include "TauCandidate.h"

/* Fun4All includes */
#include <fun4all/SubsysReco.h>
#include <calobase/RawTowerDefs.h>

/* STL includes */
#include <math.h>
#include <map>

/*HepMC include */
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

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


class TauCandidate;

typedef std::map<float, TauCandidate*> type_map_tcan;
typedef std::map< RawTowerDefs::CalorimeterId , std::pair< RawTowerContainer*, RawTowerGeomContainer* > > type_map_cdata;

class DISKinematicsReco : public SubsysReco
{

public:

  DISKinematicsReco(std::string filename);

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
  bool _save_towers;
  bool _save_tracks;

  int _ievent;
  int _total_pass;

  std::string _filename;
  TFile *_tfile;

  /* output tree and variables */
  TTree* _tree_event;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;

  /** CaloRawTowerEvaluators to access tru particle info for
   * given towers */
  std::map< std::string, CaloRawTowerEval* > _map_towereval;

  //  /** Map of TauCandidate properties that will be written to
  //   * output ROOT Tree */
  //  std::map< TauCandidate::PROPERTY , float > _map_treebranches;

  /** Map of EM Candidates that will be written to
   * output ROOT Tree */
  std::map< TauCandidate::PROPERTY , std::vector< float > > _map_em_candidate_branches;

  /** Map of Event properties that will be written to
   * output ROOT Tree */
  std::map< std::string , float > _map_event_branches;

  /** helper pointer to topNode */
  PHCompositeNode *_topNode;

  int CollectEmCandidates( type_map_tcan& );

  int InsertCandidateFromCluster( type_map_tcan& , RawCluster* , CaloEvalStack* );

  int AddGlobalCalorimeterInformation();

  int AddReconstructedKinematics( type_map_tcan& );

  int WriteCandidatesToTree( type_map_tcan& );

  int AddTruthEventInformation();

  /** Find tau candidate in map that is closest to given eta, phi angle */
  TauCandidate* FindMinDeltaRCandidate( type_map_tcan*, const float, const float );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );


  /** Find track with minimum delta R from given cluster */
  SvtxTrack* FindClosestTrack( RawCluster* );

  /** get energy in 3x3 calorimeter towers around track projection to calorimeter surface.
   * Copied from FastTrackingEval.C */
  float getE33Barrel( std::string, float, float );

  /** get energy in 3x3 calorimeter towers around track projection to calorimeter surface.
   * Copied from FastTrackingEval.C */
  float getE33Forward( std::string, float, float );

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

#endif // __DISKinematicsReco_H__
