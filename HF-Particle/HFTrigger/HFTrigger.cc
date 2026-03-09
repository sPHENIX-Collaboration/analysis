#include "HFTrigger.h"

/*
 * Basic heavy flavor software trigger
 * Used in study of hardware trigger
 * Cameron Dean
 * 01/02/2021
 */
std::map<std::string, bool> triggerDecisions =
{
  {"oneTrack", false}
, {"twoTrack", false}
, {"lowMultiplicity", false}
, {"highMultiplicity", false}
};

namespace HFTriggerRequirement
{
  float meanHighMult = 100; //Set min num. INTT hits
  float asymmHighMult = 0.1; //Set highMult asymm between INTT layers (fraction)
  float meanLowMult = 20;  //Set max num. INTT hits
  float asymmLowMult = 0.1; //Set lowMult asymm between INTT layers (fraction)
  float trackPT = 0.5; //Min track pT in GeV
  float trackVertexDCA = 0.01; //Min. DCA of a track with any vertex in cm
  float trackTrackDCA = 0.03; //Max. DCA of a track with other tracks in cm
}

HFTrigger::HFTrigger()
  : SubsysReco("HFTRIGGER")
  , m_useOneTrackTrigger(false)
  , m_useTwoTrackTrigger(false)
  , m_useLowMultiplicityTrigger(false)
  , m_useHighMultiplicityTrigger(false)
{
}

HFTrigger::HFTrigger(const std::string &name)
  : SubsysReco(name)
  , m_useOneTrackTrigger(false)
  , m_useTwoTrackTrigger(false)
  , m_useLowMultiplicityTrigger(false)
  , m_useHighMultiplicityTrigger(false)
{
}

int HFTrigger::Init(PHCompositeNode *topNode)
{
  return 0;
}

int HFTrigger::process_event(PHCompositeNode *topNode)
{
  bool successfulTrigger = runTrigger(topNode);
  
  if (Verbosity() >= VERBOSITY_MORE) 
  {
    if (successfulTrigger) std::cout << "One of the heavy flavor triggers fired" << std::endl;
    else std::cout << "No heavy flavor triggers fired" << std::endl;
  }

  int failedTriggerDecisions = 0;
  if (m_useOneTrackTrigger && !triggerDecisions.find("oneTrack")->second)
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "The oneTrackTrigger did not fire for this event, skipping." << std::endl;
    failedTriggerDecisions += 1;
  }
  if (m_useTwoTrackTrigger && !triggerDecisions.find("twoTrack")->second)
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "The twoTrackTrigger did not fire for this event, skipping." << std::endl;
    failedTriggerDecisions += 1;
  }
  if (m_useLowMultiplicityTrigger && !triggerDecisions.find("lowMultiplicity")->second)
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "The lowMultiplicityTrigger did not fire for this event, skipping." << std::endl;
    failedTriggerDecisions += 1;
  }
  if (m_useHighMultiplicityTrigger && !triggerDecisions.find("highMultiplicity")->second)
  {
    if (Verbosity() >= VERBOSITY_SOME) std::cout << "The highMultiplicityTrigger did not fire for this event, skipping." << std::endl;
    failedTriggerDecisions += 1;
  }
   
  if (failedTriggerDecisions > 0) return Fun4AllReturnCodes::ABORTEVENT;
  else return Fun4AllReturnCodes::EVENT_OK;
}

int HFTrigger::End(PHCompositeNode *topNode)
{
  return 0;
}

bool HFTrigger::runTrigger(PHCompositeNode *topNode)
{
  bool anyTriggerFired = false;

  std::vector<Track> allTracks = makeAllTracks(topNode);
  std::vector<Vertex> allVertices = makeAllPrimaryVertices(topNode);

  float meanMult = 0;
  float asymmMult = 0;
  calculateMultiplicity(topNode, meanMult, asymmMult);  

  if (m_useOneTrackTrigger) triggerDecisions.find("oneTrack")->second = runOneTrackTrigger(allTracks, allVertices);
  if (m_useTwoTrackTrigger) triggerDecisions.find("twoTrack")->second = runTwoTrackTrigger(allTracks, allVertices);
  if (m_useLowMultiplicityTrigger) triggerDecisions.find("lowMultiplicity")->second = runLowMultiplicityTrigger(meanMult, asymmMult);
  if (m_useHighMultiplicityTrigger) triggerDecisions.find("highMultiplicity")->second = runHighMultiplicityTrigger(meanMult, asymmMult);

  if (Verbosity() >= VERBOSITY_SOME) printTrigger();

  const int numberOfFiredTriggers = std::accumulate( begin(triggerDecisions), end(triggerDecisions), 0, 
                                                   [](const int previous, const std::pair<const std::string, bool>& element) 
                                                   { return previous + element.second; });

  if (numberOfFiredTriggers != 0) anyTriggerFired = true;

  return anyTriggerFired;
}

bool HFTrigger::runOneTrackTrigger(std::vector<Track> Tracks, std::vector<Vertex> Vertices)
{ //Can maybe use return when we get a positive trigger decision to break the loops
  bool oneTrackTriggerDecision = false;

  for (Track track : Tracks)
  {
    float pT = sqrt(pow(track(3,0), 2) + pow(track(4,0), 2));
    float minIP = FLT_MAX;
    for (Vertex vertex : Vertices)
    {
      float thisIP = calcualteTrackVertexDCA(track, vertex);
      minIP = std::min(minIP, thisIP);
    }
    if (pT > HFTriggerRequirement::trackPT && minIP > HFTriggerRequirement::trackVertexDCA) oneTrackTriggerDecision = true;
  }

  return oneTrackTriggerDecision;
}

bool HFTrigger::runTwoTrackTrigger(std::vector<Track> Tracks, std::vector<Vertex> Vertices)
{
  bool twoTrackTriggerDecision = false;
  std::vector<Track> goodTracks;

  for (Track track : Tracks)
  {
    std::vector<Track> singleTrack = {track};
    bool oneTrackTriggerDecision = runOneTrackTrigger(singleTrack, Vertices);
    if (oneTrackTriggerDecision) goodTracks.push_back(track);
  }

  if (goodTracks.size() > 1) //We need at least two good track to start a two track trigger
  {
    for (unsigned int i = 0; i < goodTracks.size(); i++)
    {
      for (unsigned int j = i + 1; j < goodTracks.size(); j++)
      {
        float trackDCA = calcualteTrackTrackDCA(goodTracks[i], goodTracks[j]);
        if (trackDCA < HFTriggerRequirement::trackTrackDCA) twoTrackTriggerDecision = true;
      } 
    }
  }

  return twoTrackTriggerDecision;
}

void HFTrigger::calculateMultiplicity(PHCompositeNode *topNode, float& meanMultiplicity, float& asymmetryMultiplicity)
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

bool HFTrigger::runHighMultiplicityTrigger(float meanMultiplicity, float asymmetryMultiplicity)
{
  bool multiplicityTriggerDecision = false;
  float meanRequirement = HFTriggerRequirement::meanHighMult;
  float asymmRequirement = HFTriggerRequirement::asymmHighMult;

  if ( meanMultiplicity > meanRequirement && asymmetryMultiplicity < asymmRequirement)
    multiplicityTriggerDecision = true;

  return multiplicityTriggerDecision;
}

bool HFTrigger::runLowMultiplicityTrigger(float meanMultiplicity, float asymmetryMultiplicity)
{
  bool multiplicityTriggerDecision = false;
  float meanRequirement = HFTriggerRequirement::meanLowMult;
  float asymmRequirement = HFTriggerRequirement::asymmLowMult;

  if ( meanMultiplicity < meanRequirement && asymmetryMultiplicity < asymmRequirement)
    multiplicityTriggerDecision = true;

  return multiplicityTriggerDecision;
}

Vertex HFTrigger::makeVertex(PHCompositeNode *topNode)
{
  Vertex vertex;

  vertex(0,0) = m_dst_vertex->get_x();
  vertex(1,0) = m_dst_vertex->get_y();
  vertex(2,0) = m_dst_vertex->get_z();

  return vertex;
}

std::vector<Vertex> HFTrigger::makeAllPrimaryVertices(PHCompositeNode *topNode)
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

Track HFTrigger::makeTrack(PHCompositeNode *topNode)  
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

std::vector<Track> HFTrigger::makeAllTracks(PHCompositeNode *topNode)
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

int HFTrigger::decomposeTrack(Track track, TrackX& trackPosition, TrackP& trackMomentum)
{
  for (unsigned int i = 0; i < 3; i++)
  {
    trackPosition(i,0) = track(i,0);
    trackMomentum(i,0) = track(i+3,0);
  }

  return 0;
}

float HFTrigger::calcualteTrackVertex2DDCA(Track track, Vertex vertex)
{
  TrackX pos;
  TrackP mom;
  DCA dcaVertex;

  decomposeTrack(track, pos, mom);

  dcaVertex = pos - vertex - mom.dot(pos - vertex)*mom/(mom.dot(mom));

  float twoD_DCA = std::sqrt(std::pow(dcaVertex(0,0), 2) + std::pow(dcaVertex(1,0), 2));

  return twoD_DCA;
}

float HFTrigger::calcualteTrackVertexDCA(Track track, Vertex vertex)
{
  TrackX pos;
  TrackP mom;
  DCA dcaVertex;

  decomposeTrack(track, pos, mom);

  dcaVertex = pos - vertex - mom.dot(pos - vertex)*mom/(mom.dot(mom));
  
  return std::abs(dcaVertex.norm());
}

float HFTrigger::calcualteTrackTrackDCA(Track trackOne, Track trackTwo)
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

void HFTrigger::printTrigger()
{
  std::cout << "\n---------------HFTrigger information---------------" << std::endl;
  if (m_useOneTrackTrigger) std::cout << "The oneTrack trigger decision is " << triggerDecisions.find("oneTrack")->second << std::endl;
  if (m_useTwoTrackTrigger) std::cout << "The twoTrack trigger decision is " << triggerDecisions.find("twoTrack")->second << std::endl;
  if (m_useLowMultiplicityTrigger) std::cout << "The lowMultiplicity trigger decision is " << triggerDecisions.find("lowMultiplicity")->second << std::endl;
  if (m_useHighMultiplicityTrigger) std::cout << "The highMultiplicity trigger decision is " << triggerDecisions.find("highMultiplicity")->second << std::endl;
  std::cout << "---------------------------------------------------\n" << std::endl;
}

