#ifndef __FastPid_RICH_H__
#define __FastPid_RICH_H__

#include <fun4all/SubsysReco.h>
#include <math.h>

class PHCompositeNode;
class PHG4TruthInfoContainer;
class SvtxTrackMap;
class SvtxTrack;

class TrackProjectorPid;
class SetupDualRICHAnalyzer;

class FastPid_RICH : public SubsysReco
{

public:

  FastPid_RICH(std::string tracksname, std::string richname);

  int
  Init(PHCompositeNode*);
  int
  InitRun(PHCompositeNode*);
  int
  process_event(PHCompositeNode*);
  int
  End(PHCompositeNode*);

private:

  bool _verbose;

  int _ievent;

  /* Collection storing track information */
  std::string _trackmap_name;

  /* track projector object */
  TrackProjectorPid *_trackproj;

};

#endif // __FastPid_RICH_H__
