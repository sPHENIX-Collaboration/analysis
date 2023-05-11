#ifndef HFTRIGGER_H
#define HFTRIGGER_H

//sPHENIX stuff
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>
#include <phool/getClass.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <Eigen/Core>
#include <Eigen/Dense>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <string>

typedef Eigen::Matrix<float, 6, 1> Track;
typedef Eigen::Matrix<float, 3, 1> Vertex;
typedef Eigen::Matrix<float, 3, 1> TrackX;
typedef Eigen::Matrix<float, 3, 1> TrackP;
typedef Eigen::Matrix<float, 3, 1> DCA;

class SvtxTrack;
class SvtxTrackMap;
class SvtxVertexMap;
class SvtxVertex;

class HFTrigger : public SubsysReco
{
 public:
  HFTrigger();

  explicit HFTrigger(const std::string &name);

  virtual ~HFTrigger(){}

  int Init(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  int End(PHCompositeNode *topNode);

  bool runTrigger(PHCompositeNode *topNode);

  bool runOneTrackTrigger(std::vector<Track> Tracks, std::vector<Vertex> Vertices);

  bool runTwoTrackTrigger(std::vector<Track> Tracks, std::vector<Vertex> Vertices);

  void calculateMultiplicity(PHCompositeNode *topNode, float& meanMultiplicity, float& asymmetryMultiplicity);

  bool runHighMultiplicityTrigger(float meanMultiplicity, float asymmetryMultiplicity);

  bool runLowMultiplicityTrigger(float meanMultiplicity, float asymmetryMultiplicity);

  Vertex makeVertex(PHCompositeNode *topNode);

  std::vector<Vertex> makeAllPrimaryVertices(PHCompositeNode *topNode);

  Track makeTrack(PHCompositeNode *topNode);

  std::vector<Track> makeAllTracks(PHCompositeNode *topNode);

  int decomposeTrack(Track track, TrackX& trackPosition, TrackP& trackMomentum);

  float calcualteTrackVertex2DDCA(Track track, Vertex vertex);

  float calcualteTrackVertexDCA(Track track, Vertex vertex);

  float calcualteTrackTrackDCA(Track trackOne, Track trackTwo);

  void printTrigger();

  //User configuration
  void requireOneTrackTrigger(bool useTrigger) { m_useOneTrackTrigger = useTrigger; }
  void requireTwoTrackTrigger(bool useTrigger) { m_useTwoTrackTrigger = useTrigger; }
  void requireLowMultiplicityTrigger(bool useTrigger) { m_useLowMultiplicityTrigger = useTrigger; }
  void requireHighMultiplicityTrigger(bool useTrigger) { m_useHighMultiplicityTrigger = useTrigger; }

 private:

  bool m_useOneTrackTrigger = false;
  bool m_useTwoTrackTrigger = false;
  bool m_useLowMultiplicityTrigger = false;
  bool m_useHighMultiplicityTrigger = false;

  SvtxVertexMap *m_dst_vertexmap = nullptr;
  SvtxTrackMap *m_dst_trackmap = nullptr;
  SvtxVertex *m_dst_vertex = nullptr;
  SvtxTrack *m_dst_track = nullptr;
};

#endif  //HFTRIGGER_H
