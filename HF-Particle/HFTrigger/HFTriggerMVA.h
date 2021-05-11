#ifndef HFTRIGGERMVA_H
#define HFTRIGGERMVA_H

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

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>

//ROOT stuff
#include <TMVA/Reader.h>
#include <TMVA/Tools.h>

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

class HFTriggerMVA : public SubsysReco
{
 public:
  HFTriggerMVA();

  explicit HFTriggerMVA(const std::string &name);

  virtual ~HFTriggerMVA(){}

  int Init(PHCompositeNode *topNode);

  int process_event(PHCompositeNode *topNode);

  int End(PHCompositeNode *topNode);

  bool runTrigger(PHCompositeNode *topNode);

  bool runCutsTrigger(float mean_hits, float emcal_energy, float IP, float IP_xy, float DCA);

  bool runMVATrigger(TMVA::Reader* reader, std::string method, std::vector<float> inputValues, float cut);

  void calculateMultiplicity(PHCompositeNode *topNode, float& meanMultiplicity, float& asymmetryMultiplicity);

  float getMaxEMCalEnergy(PHCompositeNode *topNode);

  void getIPVariables(Track track1, Track track2, Vertex vertex, float& minIP, float& minIP_xy, float& maxIP, float& maxIP_xy, float& DCA);

  Vertex makeVertex(PHCompositeNode *topNode);

  std::vector<Vertex> makeAllPrimaryVertices(PHCompositeNode *topNode);

  Track makeTrack(PHCompositeNode *topNode);

  std::vector<Track> makeAllTracks(PHCompositeNode *topNode);

  int decomposeTrack(Track track, TrackX& trackPosition, TrackP& trackMomentum);

  float calcualteTrackVertex2DDCA(Track track, Vertex vertex);
  
  float calcualteTrackVertexDCA(Track track, Vertex vertex);

  float calcualteTrackTrackDCA(Track trackOne, Track trackTwo);

  std::tuple<TMVA::Reader*, std::vector<Float_t>> initMVA(std::vector<std::string> variableList, std::string method, std::string mvaFile);

  void printRunRecord();

  void printTrigger();

  //User configuration
  void requireCutsTrigger(bool useTrigger) { m_useCutsTrigger = useTrigger; }
  void requireMVAwCaloTrigger(bool useTrigger) { m_useMVAwCaloTrigger = useTrigger; }

  void setMeanMult(float value) { meanMult = value; }
  void setAsymmMult(float value) { asymmMult = value; }
  void setTrackVertex3DDCA(float value) { trackVertex3DDCA = value; }
  void setTrackVertex2DDCA(float value) { trackVertex2DDCA = value; }
  void setTrackTrackDCA(float value) { trackTrackDCA = value; }
  void setminEMCalEnergy(float value) { minEMCalEnergy = value; }
  void setMVA_wCaloResponse(float value) { MVA_wCaloResponse = value; }
  void setMVA_woutCaloResponse(float value) { MVA_woutCaloResponse = value; }
  void setMVA_woutCaloOrMinTrackResponse(float value) { MVA_woutCaloOrMinTrackResponse = value; }
 
 protected:
  SvtxEvalStack *m_svtx_evalstack = nullptr;
  SvtxTrackEval *trackeval = nullptr;

 private:

  float meanMult = 12;
  float asymmMult = 0.1;
  float trackVertex3DDCA = 0.05;
  float trackVertex2DDCA = 0.05;
  float trackTrackDCA = 0.05;
  float minEMCalEnergy = 0.5;
  float MVA_wCaloResponse = -0.05;
  float MVA_woutCaloResponse = -0.05;
  float MVA_woutCaloOrMinTrackResponse = -0.05;

  bool m_useCutsTrigger = false;
  bool m_useCutswoutTrigger = false;
  bool m_useMVAwCaloTrigger = false;
  bool m_useMVAwoutCaloTrigger = false;
  bool m_useMVAwoutCaloAndMinTrackTrigger = false;

  SvtxVertexMap *m_dst_vertexmap = nullptr;
  SvtxTrackMap *m_dst_trackmap = nullptr;
  SvtxVertex *m_dst_vertex = nullptr;
  SvtxTrack *m_dst_track = nullptr;

  //Counters
  int m_events = 0;
  int m_no_trigger = 0;
  int m_counter = 0;
  int m_cuts_and_mva_wcalo_counter = 0;
  int m_no_cuts_and_mva_wcalo_counter = 0;
  int m_cuts_and_no_mva_wcalo_counter = 0;
  int m_no_cuts_and_no_mva_wcalo_counter = 0;
  int m_cuts_and_mva_woutcalo_counter = 0;
  int m_no_cuts_and_mva_woutcalo_counter = 0;
  int m_cuts_and_no_mva_woutcalo_counter = 0;
  int m_no_cuts_and_no_mva_woutcalo_counter = 0;
  int m_cuts_and_mva_woutcalo_and_mintracks_counter = 0;
  int m_no_cuts_and_mva_woutcalo_and_mintracks_counter = 0;
  int m_cuts_and_no_mva_woutcalo_and_mintracks_counter = 0;
  int m_no_cuts_and_no_mva_woutcalo_and_mintracks_counter = 0;

  //MVA stuff
  std::string path = "/sphenix/u/cdean/software/analysis/HF-Particle/HFTrigger/weights/";

  std::string mvaType = "BDTG";

  std::vector<std::string> varListwCalo = {"min(track_1_IP,track_2_IP)",
                                           "min(abs(track_1_IP_xy),abs(track_2_IP_xy))",
                                           "max(track_1_IP,track_2_IP)",
                                           "max(abs(track_1_IP_xy),abs(track_2_IP_xy))",
                                           "max(track_1_EMCAL_energy_cluster,track_2_EMCAL_energy_cluster)",
                                           "track_1_track_2_DCA",
                                           "INTT_meanHits"};

  std::vector<std::string> varListwoutCalo = {"min(track_1_IP,track_2_IP)",
                                              "min(abs(track_1_IP_xy),abs(track_2_IP_xy))",
                                              "max(track_1_IP,track_2_IP)",
                                              "max(abs(track_1_IP_xy),abs(track_2_IP_xy))",
                                              "track_1_track_2_DCA",
                                              "INTT_meanHits"};

  std::vector<std::string> varListwoutCaloAndMinTrack = {"max(track_1_IP,track_2_IP)",
                                                         "max(abs(track_1_IP_xy),abs(track_2_IP_xy))",
                                                         "track_1_track_2_DCA",
                                                         "INTT_meanHits"};

  TMVA::Reader *wCaloReader, *woutCaloReader, *woutCaloAndMinTrackReader;
  std::vector<float> wCaloFloats, woutCaloFloats, woutCaloAndMinTrackFloats;
};

#endif  //HFTRIGGERMVA_H
