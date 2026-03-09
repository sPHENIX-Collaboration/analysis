#include "HFTriggerMVA.h"

/*
 * Basic heavy flavor software trigger
 * Used in study of hardware trigger
 * Cameron Dean
 * 01/02/2021
 */
std::map<std::string, bool> triggerDecisionsMVA =
{
  {"cuts", false}
, {"cutsWithoutCalo", false}
, {"MVAWithCalo", false}
, {"MVAWithoutCalo", false}
, {"MVAWithoutCaloAndMinTrack", false}
};
/*
namespace HFTriggerMVARequirement
{
  float meanMult = 12; //Set min num. INTT hits
  float asymmMult = 0.1; //Set asymm between INTT layers (fraction)
  float trackVertex3DDCA = 0.05; //Min. 3D DCA of a track with any vertex in cm
  float trackVertex2DDCA = 0.05; //Max. 2D DCA of a track with any vertex in cm
  float trackTrackDCA = 0.05; //Max. DCA of a track with other tracks in cm
  float minEMCalEnergy = 0.5;
  float MVA_wCaloResponse = -0.05;
  float MVA_woutCaloResponse = -0.05;
  float MVA_woutCaloOrMinTrackResponse = -0.05;
}
*/
HFTriggerMVA::HFTriggerMVA()
  : SubsysReco("HFTRIGGER")
  , m_useCutsTrigger(true)
  , m_useCutswoutTrigger(true)
  , m_useMVAwCaloTrigger(true)
  , m_useMVAwoutCaloTrigger(true)
  , m_useMVAwoutCaloAndMinTrackTrigger(true)
{
}

HFTriggerMVA::HFTriggerMVA(const std::string &name)
  : SubsysReco(name)
  , m_useCutsTrigger(true)
  , m_useCutswoutTrigger(true)
  , m_useMVAwCaloTrigger(true)
  , m_useMVAwoutCaloTrigger(true)
  , m_useMVAwoutCaloAndMinTrackTrigger(true)
{
}

int HFTriggerMVA::Init(PHCompositeNode *topNode)
{
  std::cout << "--Cut parameters--" << std::endl;
  std::cout << "meanMult: " << meanMult << std::endl;
  std::cout << "trackVertex3DDCA: " << trackVertex3DDCA << std::endl;
  std::cout << "trackVertex2DDCA: " << trackVertex2DDCA << std::endl;
  std::cout << "trackTrackDCA: " << trackTrackDCA << std::endl;
  std::cout << "minEMCalEnergy: " << minEMCalEnergy << std::endl;
  std::cout << "MVA_wCaloResponse: " << MVA_wCaloResponse << std::endl;
  std::cout << "MVA_woutCaloResponse: " << MVA_woutCaloResponse << std::endl;
  std::cout << "MVA_woutCaloOrMinTrackResponse: " << MVA_woutCaloOrMinTrackResponse << std::endl;
  std::cout << "-----------------" << std::endl;

  if (m_useMVAwCaloTrigger)
  {
    std::string algorithmFile = path + "wCalo/TMVAClassification_" + mvaType + ".weights.xml";
    std::tie(wCaloReader, wCaloFloats) = initMVA(varListwCalo, mvaType, algorithmFile);
  }
  if (m_useMVAwoutCaloTrigger)
  {
    std::string algorithmFile = path + "woutCalo/TMVAClassification_" + mvaType + ".weights.xml";
    std::tie(woutCaloReader, woutCaloFloats) = initMVA(varListwoutCalo, mvaType, algorithmFile);
  }
  if (m_useMVAwoutCaloAndMinTrackTrigger)
  {
    std::string algorithmFile = path + "woutCaloAndMinTrack/TMVAClassification_" + mvaType + ".weights.xml";
    std::tie(woutCaloAndMinTrackReader, woutCaloAndMinTrackFloats) = initMVA(varListwoutCaloAndMinTrack, mvaType, algorithmFile);
  }

  return 0;
}

int HFTriggerMVA::process_event(PHCompositeNode *topNode)
{
  bool successfulTrigger = runTrigger(topNode);
  
  if (Verbosity() >= VERBOSITY_MORE) 
  {
    if (successfulTrigger) std::cout << "One of the heavy flavor triggers fired" << std::endl;
    else std::cout << "No heavy flavor triggers fired" << std::endl;
  }

  if (triggerDecisionsMVA.find("cuts")->second && triggerDecisionsMVA.find("MVAWithCalo")->second) m_cuts_and_mva_wcalo_counter += 1;
  if (!triggerDecisionsMVA.find("cuts")->second && triggerDecisionsMVA.find("MVAWithCalo")->second) m_no_cuts_and_mva_wcalo_counter += 1;
  if (triggerDecisionsMVA.find("cuts")->second && !triggerDecisionsMVA.find("MVAWithCalo")->second) m_cuts_and_no_mva_wcalo_counter += 1;
  if (!triggerDecisionsMVA.find("cuts")->second && !triggerDecisionsMVA.find("MVAWithCalo")->second) m_no_cuts_and_no_mva_wcalo_counter += 1;

  if (triggerDecisionsMVA.find("cutsWithoutCalo")->second && triggerDecisionsMVA.find("MVAWithoutCalo")->second) m_cuts_and_mva_woutcalo_counter += 1;
  if (!triggerDecisionsMVA.find("cutsWithoutCalo")->second && triggerDecisionsMVA.find("MVAWithoutCalo")->second) m_no_cuts_and_mva_woutcalo_counter += 1;
  if (triggerDecisionsMVA.find("cutsWithoutCalo")->second && !triggerDecisionsMVA.find("MVAWithoutCalo")->second) m_cuts_and_no_mva_woutcalo_counter += 1;
  if (!triggerDecisionsMVA.find("cutsWithoutCalo")->second && !triggerDecisionsMVA.find("MVAWithoutCalo")->second) m_no_cuts_and_no_mva_woutcalo_counter += 1;

  if (triggerDecisionsMVA.find("cutsWithoutCalo")->second && triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second) m_cuts_and_mva_woutcalo_and_mintracks_counter += 1;
  if (!triggerDecisionsMVA.find("cutsWithoutCalo")->second && triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second) m_no_cuts_and_mva_woutcalo_and_mintracks_counter += 1;
  if (triggerDecisionsMVA.find("cutsWithoutCalo")->second && !triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second) m_cuts_and_no_mva_woutcalo_and_mintracks_counter += 1;
  if (!triggerDecisionsMVA.find("cutsWithoutCalo")->second && !triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second) m_no_cuts_and_no_mva_woutcalo_and_mintracks_counter += 1;

  m_events += 1;
  if (successfulTrigger) m_counter += 1;
  if (!successfulTrigger) m_no_trigger += 1;

  triggerDecisionsMVA.find("cuts")->second = false;
  triggerDecisionsMVA.find("cutsWithoutCalo")->second = false;
  triggerDecisionsMVA.find("MVAWithCalo")->second = false;
  triggerDecisionsMVA.find("MVAWithoutCalo")->second = false;
  triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second = false;

  return Fun4AllReturnCodes::EVENT_OK;
}

int HFTriggerMVA::End(PHCompositeNode *topNode)
{
  if (Verbosity() >= VERBOSITY_SOME) printRunRecord();
  return 0;
}

bool HFTriggerMVA::runTrigger(PHCompositeNode *topNode)
{
  bool anyTriggerFired = false;

  std::vector<Track> allTracks = makeAllTracks(topNode);
  std::vector<Vertex> allVertices = makeAllPrimaryVertices(topNode);

  float min_TrackVertex_3D_DCA = 0;
  float min_TrackVertex_2D_DCA = 0;
  float max_TrackVertex_3D_DCA = 0;
  float max_TrackVertex_2D_DCA = 0;
  float min_TrackTrack_3D_DCA = 0;

  float meanMult = 0;
  float asymmMult = 0;
  calculateMultiplicity(topNode, meanMult, asymmMult);  

  float maxEMCalEnergy = getMaxEMCalEnergy(topNode);

  for (unsigned int k = 0; k < allVertices.size(); k++)
  {
    for (unsigned int i = 0; i < allTracks.size(); i++)
    {
      for (unsigned int j = i + 1; j < allTracks.size(); j++)
      {

        getIPVariables(allTracks[i], allTracks[j], allVertices[k], 
                       min_TrackVertex_3D_DCA, min_TrackVertex_2D_DCA,
                       max_TrackVertex_3D_DCA, max_TrackVertex_2D_DCA, 
                       min_TrackTrack_3D_DCA);

        wCaloFloats[0] = woutCaloFloats[0] = min_TrackVertex_3D_DCA;
        wCaloFloats[1] = woutCaloFloats[1] = min_TrackVertex_2D_DCA;
        wCaloFloats[2] = woutCaloFloats[2] = woutCaloAndMinTrackFloats[0] = max_TrackVertex_3D_DCA;
        wCaloFloats[3] = woutCaloFloats[3] = woutCaloAndMinTrackFloats[1] = max_TrackVertex_2D_DCA;
        wCaloFloats[4] = maxEMCalEnergy;
        wCaloFloats[5] = woutCaloFloats[4] = woutCaloAndMinTrackFloats[2] = min_TrackTrack_3D_DCA;
        wCaloFloats[6] = woutCaloFloats[5] = woutCaloAndMinTrackFloats[3] = meanMult;

        if (m_useCutsTrigger)
        {  
          bool trigger = runCutsTrigger(meanMult, maxEMCalEnergy,
                                        max_TrackVertex_3D_DCA,
                                        max_TrackVertex_2D_DCA,
                                        min_TrackTrack_3D_DCA);

          if (trigger) triggerDecisionsMVA.find("cuts")->second = true;
        }

        if (m_useCutswoutTrigger)
        {  
          bool trigger = runCutsTrigger(meanMult, FLT_MAX,
                                        max_TrackVertex_3D_DCA,
                                        max_TrackVertex_2D_DCA,
                                        min_TrackTrack_3D_DCA);

          if (trigger) triggerDecisionsMVA.find("cutsWithoutCalo")->second = true;
        }

        if (m_useMVAwCaloTrigger)
        {
          bool trigger = runMVATrigger(wCaloReader, mvaType, wCaloFloats, MVA_wCaloResponse);
          if (trigger) triggerDecisionsMVA.find("MVAWithCalo")->second = true;
        }

        if (m_useMVAwoutCaloTrigger)
        {
          bool trigger = runMVATrigger(woutCaloReader, mvaType, woutCaloFloats, MVA_woutCaloResponse);
          if (trigger) triggerDecisionsMVA.find("MVAWithoutCalo")->second = true;
        }

        if (m_useMVAwoutCaloAndMinTrackTrigger)
        {
          bool trigger = runMVATrigger(woutCaloAndMinTrackReader, mvaType, woutCaloAndMinTrackFloats, MVA_woutCaloOrMinTrackResponse);
          if (trigger) triggerDecisionsMVA.find("MVAWithoutCaloAndMinTrack")->second = true;
        }

      }
    }
  }

  if (Verbosity() >= VERBOSITY_MORE) printTrigger();

  const int numberOfFiredTriggers = std::accumulate(begin(triggerDecisionsMVA), end(triggerDecisionsMVA), 0, 
                                                    [](const int previous, const std::pair<const std::string, bool>& element) 
                                                    { return previous + element.second; });

  if (numberOfFiredTriggers != 0) anyTriggerFired = true;

  return anyTriggerFired;
}

bool HFTriggerMVA::runCutsTrigger(float mean_hits, float emcal_energy,
                                  float IP, float IP_xy, float DCA)
{ 
  bool triggerDecision = false;

  if (mean_hits >= meanMult && 
      emcal_energy >= minEMCalEnergy && 
      IP >= trackVertex3DDCA && 
      IP_xy >= trackVertex2DDCA && 
      DCA <= trackTrackDCA) triggerDecision = true;

  return triggerDecision;
}

bool HFTriggerMVA::runMVATrigger(TMVA::Reader* reader, std::string method, std::vector<float> inputValues, float cut)
{
  float mvaResponse = (Float_t) reader->EvaluateMVA(inputValues, method.c_str());
  return mvaResponse >= cut;
}

void HFTriggerMVA::calculateMultiplicity(PHCompositeNode *topNode, float& meanMultiplicity, float& asymmetryMultiplicity)
{
  TrkrHitSetContainer* hitContainer = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");

  TrkrHitSetContainer::ConstRange inttHitSetRange[2];
  for (int i = 0; i < 2; i++) inttHitSetRange[i] = hitContainer->getHitSets(TrkrDefs::TrkrId::inttId, i + 4);

  int inttHits[2] = {0};

  for (int i = 0; i < 2; i++) 
  {
    for (TrkrHitSetContainer::ConstIterator hitsetitr = inttHitSetRange[i].first;
         hitsetitr != inttHitSetRange[i].second;
         ++hitsetitr)
    {
      TrkrHitSet *hitset = hitsetitr->second;
      inttHits[i] += hitset->size();
    }
  }

  meanMultiplicity = (inttHits[0] + inttHits[1])/2;
  asymmetryMultiplicity = (inttHits[0] - inttHits[1])/(inttHits[0] + inttHits[1]);
}

float HFTriggerMVA::getMaxEMCalEnergy(PHCompositeNode *topNode)
{
  float maxEnergy = 0;
  PHNodeIterator nodeIter(topNode);
  m_dst_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  for (SvtxTrackMap::Iter iter = m_dst_trackmap->begin(); iter != m_dst_trackmap->end(); ++iter)
  {
    m_dst_track = iter->second;
    float thisEnergy = m_dst_track->get_cal_cluster_e(SvtxTrack::CAL_LAYER(1));
    maxEnergy = std::max(maxEnergy, thisEnergy);
  }

  return maxEnergy;

}

void HFTriggerMVA::getIPVariables(Track track1, Track track2, Vertex vertex,
                               float& minIP, float& minIP_xy,
                               float& maxIP, float& maxIP_xy, float& DCA)
{
  float track1_IP = calcualteTrackVertexDCA(track1, vertex);
  float track1_IP_xy = calcualteTrackVertex2DDCA(track1, vertex);
  float track2_IP = calcualteTrackVertexDCA(track2, vertex);
  float track2_IP_xy = calcualteTrackVertex2DDCA(track2, vertex);

  minIP = std::min(track1_IP, track2_IP);
  minIP_xy = std::min(track1_IP_xy, track2_IP_xy);
  maxIP = std::max(track1_IP, track2_IP);
  maxIP_xy = std::max(track1_IP_xy, track2_IP_xy);
  DCA = calcualteTrackTrackDCA(track1, track2);
}

Vertex HFTriggerMVA::makeVertex(PHCompositeNode *topNode)
{
  Vertex vertex;

  vertex(0,0) = m_dst_vertex->get_x();
  vertex(1,0) = m_dst_vertex->get_y();
  vertex(2,0) = m_dst_vertex->get_z();

  return vertex;
}

std::vector<Vertex> HFTriggerMVA::makeAllPrimaryVertices(PHCompositeNode *topNode)
{
  std::vector<Vertex> primaryVertices;
  m_dst_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");

  for (SvtxVertexMap::ConstIter iter = m_dst_vertexmap->begin(); iter != m_dst_vertexmap->end(); ++iter)
  {
    m_dst_vertex = iter->second;
    primaryVertices.push_back(makeVertex(topNode));
  }

  return primaryVertices;
}

Track HFTriggerMVA::makeTrack(PHCompositeNode *topNode)  
{
  Track track;

  track(0,0) = m_dst_track->get_x();
  track(1,0) = m_dst_track->get_y();
  track(2,0) = m_dst_track->get_z();
  track(3,0) = m_dst_track->get_px();
  track(4,0) = m_dst_track->get_py();
  track(5,0) = m_dst_track->get_pz();

  return track;
}

std::vector<Track> HFTriggerMVA::makeAllTracks(PHCompositeNode *topNode)
{
  std::vector<Track> tracks;
  m_dst_trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  for (SvtxTrackMap::Iter iter = m_dst_trackmap->begin(); iter != m_dst_trackmap->end(); ++iter)
  {
    m_dst_track = iter->second;
    tracks.push_back(makeTrack(topNode));  
  }

  return tracks;
}

int HFTriggerMVA::decomposeTrack(Track track, TrackX& trackPosition, TrackP& trackMomentum)
{
  for (unsigned int i = 0; i < 3; i++)
  {
    trackPosition(i,0) = track(i,0);
    trackMomentum(i,0) = track(i+3,0);
  }

  return 0;
}


float HFTriggerMVA::calcualteTrackVertex2DDCA(Track track, Vertex vertex)
{
  TrackX pos;
  TrackP mom;
  DCA dcaVertex;

  decomposeTrack(track, pos, mom);

  dcaVertex = pos - vertex - mom.dot(pos - vertex)*mom/(mom.dot(mom));

  float twoD_DCA = std::sqrt(std::pow(dcaVertex[0], 2) + std::pow(dcaVertex[1], 2));

  return twoD_DCA;
}

float HFTriggerMVA::calcualteTrackVertexDCA(Track track, Vertex vertex)
{
  TrackX pos;
  TrackP mom;
  DCA dcaVertex;

  decomposeTrack(track, pos, mom);

  dcaVertex = pos - vertex - mom.dot(pos - vertex)*mom/(mom.dot(mom));
  
  return std::abs(dcaVertex.norm());
}

float HFTriggerMVA::calcualteTrackTrackDCA(Track trackOne, Track trackTwo)
{
  TrackX posOne;
  TrackP momOne;
  TrackX posTwo;
  TrackP momTwo;
  float dcaTrackSize;

  decomposeTrack(trackOne, posOne, momOne);
  decomposeTrack(trackTwo, posTwo, momTwo);

  Eigen::Vector3f momOneCrossmomTwo = momOne.cross(momTwo);
  Eigen::Vector3f posOneMinusposTwo = posOne - posTwo;
  dcaTrackSize = std::abs(momOneCrossmomTwo.dot(posOneMinusposTwo)/(momOneCrossmomTwo.norm()));
  
  return dcaTrackSize;
}

std::tuple<TMVA::Reader*, std::vector<Float_t>> HFTriggerMVA::initMVA(std::vector<std::string> variableList, std::string method, std::string mvaFile)
{
  TMVA::Tools::Instance();  //Start TMVA
  TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

  std::vector<Float_t> reader_floats;

  for (unsigned int i = 0; i < variableList.size(); ++i)
  {
    reader_floats.push_back(0);
    reader->AddVariable(variableList[i].c_str(), &reader_floats[i]);
  }
  reader->BookMVA(method.c_str(), mvaFile.c_str());

  return make_tuple(reader, reader_floats);
}

void HFTriggerMVA::printRunRecord()
{
  std::cout << "\n---------------HFTriggerMVA run information---------------" << std::endl;
  std::cout << "The number of events processed is: " << m_events << std::endl;
  std::cout << "The number of events that did not trigger is: " << m_no_trigger << std::endl;
  std::cout << "The number of events that fired the cuts-based trigger is: " << m_cuts_and_no_mva_wcalo_counter + m_cuts_and_mva_wcalo_counter << std::endl;
  std::cout << "The number of events that fired the cuts-based trigger, with no calorimetry is: " << m_cuts_and_no_mva_woutcalo_counter + m_cuts_and_mva_woutcalo_counter << std::endl;
  std::cout << "The number of events that fired the MVA trigger is: " << m_no_cuts_and_mva_wcalo_counter + m_cuts_and_mva_wcalo_counter << std::endl;
  std::cout << "The number of events that fired the MVA trigger, with no calorimetry is: " << m_no_cuts_and_mva_woutcalo_counter + m_cuts_and_mva_woutcalo_counter << std::endl;
  std::cout << "The number of events that fired the MVA trigger, with no calorimetry or min track is: " << m_no_cuts_and_mva_woutcalo_and_mintracks_counter + m_cuts_and_mva_woutcalo_and_mintracks_counter << std::endl;
  std::cout << "The number of events triggered is: " << m_counter << std::endl;
  std::cout << "--Comparing cuts-based and MVA, both with Calorimetry:" << std::endl;
  std::cout << "----The number of events that fired the cuts-based trigger but not the MVA trigger is: " << m_cuts_and_no_mva_wcalo_counter << std::endl;
  std::cout << "----The number of events that fired the MVA trigger but not the cuts-based trigger is: " << m_no_cuts_and_mva_wcalo_counter << std::endl;
  std::cout << "----The number of events that fired both the cuts-based trigger and the MVA triggers is: " << m_cuts_and_mva_wcalo_counter  << std::endl;
  std::cout << "----The number of events that fired neither the cuts-based trigger or the MVA triggers is: " <<  m_no_cuts_and_no_mva_wcalo_counter << std::endl;
  std::cout << "--Comparing cuts-based and MVA, both without Calorimetry:" << std::endl;
  std::cout << "----The number of events that fired the cuts-based trigger but not the MVA trigger is: " << m_cuts_and_no_mva_woutcalo_counter << std::endl;
  std::cout << "----The number of events that fired the MVA trigger but not the cuts-based trigger is: " << m_no_cuts_and_mva_woutcalo_counter << std::endl;
  std::cout << "----The number of events that fired both the cuts-based trigger and the MVA triggers is: " << m_cuts_and_mva_woutcalo_counter  << std::endl;
  std::cout << "----The number of events that fired neither the cuts-based trigger or the MVA triggers is: " <<  m_no_cuts_and_no_mva_woutcalo_counter << std::endl;
  std::cout << "--Comparing cuts-based and MVA without Calorimetry or IP of second track:" << std::endl;
  std::cout << "----The number of events that fired the cuts-based trigger but not the MVA trigger is: " << m_cuts_and_no_mva_woutcalo_and_mintracks_counter << std::endl;
  std::cout << "----The number of events that fired the MVA trigger but not the cuts-based trigger is: " << m_no_cuts_and_mva_woutcalo_and_mintracks_counter << std::endl;
  std::cout << "----The number of events that fired both the cuts-based trigger and the MVA triggers is: " << m_cuts_and_mva_woutcalo_and_mintracks_counter  << std::endl;
  std::cout << "----The number of events that fired neither the cuts-based trigger or the MVA triggers is: " <<  m_no_cuts_and_no_mva_woutcalo_and_mintracks_counter << std::endl;
  std::cout << "----------------------------------------------------------\n" << std::endl;
}

void HFTriggerMVA::printTrigger()
{
  std::cout << "\n---------------HFTriggerMVA event information---------------" << std::endl;
  if (m_useCutsTrigger) std::cout << "The cuts based trigger decision is " << triggerDecisionsMVA.find("cuts")->second << std::endl;
  if (m_useMVAwCaloTrigger) std::cout << "The MVA wCalo trigger decision is " << triggerDecisionsMVA.find("MVAWithCalo")->second << std::endl;
  std::cout << "---------------------------------------------------------\n" << std::endl;
}

