#ifndef __LeptoquarksReco_H__
#define __LeptoquarksReco_H__

/* Fun4All includes */
#include <fun4all/SubsysReco.h>

/* STL includes */
#include <math.h>
#include <map>

/*HepMC include */
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>

class TNtuple;
class TFile;

class PHCompositeNode;
class CaloRawTowerEval;
class RawTowerContainer;
class RawTowerGeomContainer;
class JetMap;
class SvtxTrackMap;
class PHHepMCGenEventMap;


class TauCandidate;

typedef std::map<float, TauCandidate> map_tcan;

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
  TNtuple* _ntp_jet;
  TNtuple* _ntp_jet2;
  TNtuple* _ntp_tower;
  TNtuple* _ntp_track;

  /* beam energies electron and proton */
  float _ebeam_E;
  float _pbeam_E;

  /** name of jet collection */
  std::string _jetcolname;

  /* CaloRawTowerEvaluators to access tru particle info for
   * given towers */
  std::map< std::string, CaloRawTowerEval* > _map_towereval;

  int AddTrueTauTag( map_tcan&, PHHepMCGenEventMap* );

  int AddJetStructureInformation( map_tcan&, JetMap*, std::vector<RawTowerContainer*>, std::vector<RawTowerGeomContainer*> );

  int AddTrackInformation( map_tcan&, SvtxTrackMap* );

  int WriteTauCandidatesToTree( map_tcan& );

  /** Check if this is the final state particle or not- update pointer if needed */
  void UpdateFinalStateParticle( HepMC::GenParticle *& );

  /** Calculate Delta R ("distance in eta-phi space") between two sets of eta, phi angles */
  float CalculateDeltaR( float, float, float, float );

  /* Function to find final decay of tau */
  //  void FindDecayParticles(HepMC::GenParticle *, int&, int&, int&);
};

#endif // __LeptoquarksReco_H__
