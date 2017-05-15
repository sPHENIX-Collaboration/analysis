#ifndef __RecoInfoExport_H__
#define __RecoInfoExport_H__

#include <fun4all/SubsysReco.h>
#include <string>
#include <vector>
#include <TH1D.h>
#include <TH2D.h>

class PHCompositeNode;

class RecoInfoExport : public SubsysReco
{

public:

  explicit
  RecoInfoExport(const std::string &name = "RecoInfoExport");

  int
  Init(PHCompositeNode *topNode);
  int
  process_event(PHCompositeNode *topNode);
  int
  End(PHCompositeNode *topNode);

  void
  set_file_prefix(const std::string &s)
  {
    _file_prefix = s;
  }

  double
  get_pT_threshold() const
  {
    return _pT_threshold;
  }

  void
  set_pT_threshold(double tThreshold)
  {
    _pT_threshold = tThreshold;
  }

  double
  get_tower_threshold() const
  {
    return _tower_threshold;
  }

  void
  set_tower_threshold(double towerThreshold)
  {
    _tower_threshold = towerThreshold;
  }

  double
  get_min_track_hit_dist() const
  {
    return _min_track_hit_dist;
  }

  void
  set_min_track_hit_dist(double minTrackHitDist)
  {
    _min_track_hit_dist = minTrackHitDist;
  }
private:

  int _event;
  std::string _file_prefix;

  std::vector<std::string> _calo_names;

  double _tower_threshold;
  double _pT_threshold;
  double _min_track_hit_dist;
};

#endif // __RecoInfoExport_H__
