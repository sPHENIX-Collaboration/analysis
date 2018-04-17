#ifndef __FastPid_RICH_H__
#define __FastPid_RICH_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxTrackMap;
class SvtxTrack;

class PidInfoContainer;
class TrackProjectorPid;
class SetupDualRICHAnalyzer;

/**
 * Class that allows to use a parametrized response of a RICH detector based on the
 * information of the reconstructed incoming charged track for particle ID.
 */
class FastPid_RICH : public SubsysReco
{

public:

  FastPid_RICH(std::string richname, std::string tracksname);

  int
  Init(PHCompositeNode*);
  int
  InitRun(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

private:

  void CreateNodes(PHCompositeNode *topNode);

  bool _verbose;

  int _ievent;

  /* RICH detector name */
  std::string _detector;

  /* Collection storing track information */
  std::string _trackmap_name;

  /* Node name for node to store PID infos */
  std::string _pidinfo_node_name;

  /* Collection to store PidInfos for each track on node tree */
  PidInfoContainer* _pidinfos;

  /* track projector object */
  TrackProjectorPid *_trackproj;

  /* Radius for track extrapolation */
  float _radius;
};

#endif // __FastPid_RICH_H__
