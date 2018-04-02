#ifndef __RICHParticleID_H__
#define __RICHParticleID_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class TTree;
class TFile;
class TH1D;

class PHCompositeNode;
class PHG4TruthInfoContainer;
class PHG4Hit;
class PHG4HitContainer;
class SvtxTrackMap;
class SvtxTrack_FastSim;

class TrackProjectorRICH;
class SetupDualRICHAnalyzer;
class PIDProbabilities;

class RICHParticleID : public SubsysReco
{

public:

  RICHParticleID(std::string tracksname, std::string richname);

  int
  Init(PHCompositeNode*);
  int
  InitRun(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

  /* set refractive index of RICH radiator */
  void set_refractive_index( float newidx )
  {
    _refractive_index = newidx;
    return;
  }

private:

  bool _verbose;

  int _ievent;

  /* Collection storing track information */
  std::string _trackmap_name;

  /* Hit collection storing RICH photon hits */
  std::string _richhits_name;

  /* Refractive index of RICH radiator */
  float _refractive_index;

  /* track projector object */
  TrackProjectorRICH *_trackproj;

  /* acquire object */
  SetupDualRICHAnalyzer *_acquire;

  /* particleid object */
  PIDProbabilities *_particleid;

};

#endif // __RICHParticleID_H__
