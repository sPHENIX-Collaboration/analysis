#include <EventCheck.h>
// -- c++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <algorithm>
// -- event
#include <ffaobjects/EventHeader.h>
#include <fun4all/Fun4AllServer.h>
// -- fun4all
#include <fun4all/Fun4AllReturnCodes.h>
// -- vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
// -- DST node
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
// -- Trigger
#include <calotrigger/TriggerRunInfo.h>

using std::cout;
using std::endl;
using std::to_string;
using std::stringstream;
using std::make_pair;

//____________________________________________________________________________..
EventCheck::EventCheck()
  : SubsysReco("EventCheck")
  , m_triggerBit(17) /*Jet 8 GeV + MBD NS >= 1*/
  , m_zvtx_max(30) /*cm*/
  , m_doSpecificEvents(false)
{
  cout << "EventCheck::EventCheck(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
EventCheck::~EventCheck()
{
  cout << "EventCheck::~EventCheck() Calling dtor" << endl;
}

//____________________________________________________________________________..
Int_t EventCheck::Init(PHCompositeNode *topNode)
{
  cout << "EventCheck::Init(PHCompositeNode *topNode) Initializing" << endl;

  m_triggeranalyzer = new TriggerAnalyzer();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventCheck::process_event(PHCompositeNode *topNode)
{
  EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
  if(!eventInfo)
  {
    cout << PHWHERE << "EventValidation::process_event - Fatal Error - EventHeader node is missing. " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();
  int m_run         = eventInfo->get_RunNumber();

  if (m_doSpecificEvents && std::find(m_eventList_vec.begin(), m_eventList_vec.end(), make_pair(m_run,m_globalEvent)) == m_eventList_vec.end()) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  else if(m_doSpecificEvents) {
    cout << "Event Found! Run: " << m_run << ", Event: " << m_globalEvent << endl;
  }

  TriggerRunInfo* triggerruninfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");
  if (!triggerruninfo) {
    cout << "EventCheck::process_event - Error can not find TriggerRunInfo node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // zvertex
  float m_zvtx = -9999;
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap) {
    cout << "EventCheck::process_event - Error can not find global vertex node " << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if(!vertexmap->empty()) {
    GlobalVertex* vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  // skip event if zvtx is too large
  if(!m_doSpecificEvents && fabs(m_zvtx) >= m_zvtx_max) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_triggeranalyzer->decodeTriggers(topNode);

  // skip event if it did not fire the desired trigger
  if(!m_doSpecificEvents && !m_triggeranalyzer->didTriggerFire(m_triggerBit)) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventCheck::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
Int_t EventCheck::End(PHCompositeNode *topNode)
{
  cout << "EventCheck::End(PHCompositeNode *topNode) This is the End..." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

