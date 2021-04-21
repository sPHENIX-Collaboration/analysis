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

#include <g4main/PHG4Particle.h>
#include <g4eval/SvtxClusterEval.h>
#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxHitEval.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4eval/SvtxVertexEval.h>

#include <Eigen/Core>
#include <Eigen/Dense>

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

class PHG4Particle;
class SvtxClusterEval;
class SvtxEvalStack;
class SvtxHitEval;
class SvtxTrack;
class SvtxTrackEval;
class SvtxTrackMap;
class SvtxTruthEval;
class SvtxVertexMap;
class SvtxVertex;
class SvtxVertexEval;
/*
class PHCompositeNode;

class SvtxVertexMap;
class SvtxTrackMap;
class SvtxVertex;
class SvtxTrack;
*/
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

  float calcualteTrackVertexDCA(Track track, Vertex vertex);

  float calcualteTrackTrackDCA(Track trackOne, Track trackTwo);

  void printTrigger();

  //User configuration
  void requireOneTrackTrigger(bool useTrigger) { m_useOneTrackTrigger = useTrigger; }
  void requireTwoTrackTrigger(bool useTrigger) { m_useTwoTrackTrigger = useTrigger; }
  void requireLowMultiplicityTrigger(bool useTrigger) { m_useLowMultiplicityTrigger = useTrigger; }
  void requireHighMultiplicityTrigger(bool useTrigger) { m_useHighMultiplicityTrigger = useTrigger; }
 
 protected:
  SvtxEvalStack *m_svtx_evalstack = nullptr;
  SvtxClusterEval *clustereval = nullptr;
  SvtxHitEval *hiteval = nullptr;
  SvtxTrackEval *trackeval = nullptr;
  SvtxTruthEval *trutheval = nullptr;
  SvtxVertexEval *vertexeval = nullptr;

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
