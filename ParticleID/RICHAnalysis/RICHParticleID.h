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
class SvtxTrack;

class PidInfoContainer;
class TrackProjectorPid;
class SetupDualRICHAnalyzer;
class PIDProbabilities;

class RICHParticleID : public SubsysReco
{

public:

  RICHParticleID(std::string richname, std::string tracksname);

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
  void CreateNodes(PHCompositeNode *topNode);

  bool _verbose;

  int _ievent;

  /* RICH detecto name */
  std::string _detector;

  /* Collection storing track information */
  std::string _trackmap_name;

  /* Hit collection storing RICH photon hits */
  std::string _richhits_name;

  /* Node name for node to store PID infos */
  std::string _pidinfo_node_name;

  /* Refractive index of RICH radiator */
  float _refractive_index;

  /* Collection to store PidInfos for each track on node tree */
  PidInfoContainer* _pidinfos;

  /* track projector object */
  TrackProjectorPid *_trackproj;

  /* acquire object */
  SetupDualRICHAnalyzer *_acquire;

  /* particleid object */
  PIDProbabilities *_particleid;

  /* Radius for track extrapolation */
  float _radius;

};

#endif // __RICHParticleID_H__
