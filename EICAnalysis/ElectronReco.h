#ifndef __ElectronReco_H__
#define __ElectronReco_H__

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
class CaloRawTowerEval;
class RawTowerContainer;
class RawTowerGeomContainer;
//class JetMap;
class SvtxTrackMap;
class PHHepMCGenEventMap;


class TauCandidate;

typedef std::map<float, TauCandidate*> type_map_tcan;
typedef std::map< RawTowerDefs::CalorimeterId , std::pair< RawTowerContainer*, RawTowerGeomContainer* > > type_map_cdata;

class ElectronReco : public SubsysReco
{

public:

  ElectronReco(std::string filename);

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
  TTree* _e_candidate;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;

  /** CaloRawTowerEvaluators to access tru particle info for
   * given towers */
  std::map< std::string, CaloRawTowerEval* > _map_towereval;

  /** Map of TauCandidate properties that will be written to
   * output ROOT Tree */
  std::map< TauCandidate::PROPERTY , float > _map_treebranches;

  int AddTrueElectronTag( type_map_tcan&, PHHepMCGenEventMap* );

  int AddCalorimeterInformation( type_map_tcan&, type_map_cdata* );

  int AddTrackInformation( type_map_tcan&, SvtxTrackMap* );

  int WriteCandidatesToTree( type_map_tcan& );

  /** Find tau candidate in map that is closest to given eta, phi angle */
  TauCandidate* FindMinDeltaRCandidate( type_map_tcan*, const float, const float );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );

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

#endif // __ElectronReco_H__
