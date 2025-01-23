#include <EventCheck.h>
// -- c++
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
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
using std::string;
using std::to_string;
using std::stringstream;

//____________________________________________________________________________..
EventCheck::EventCheck()
  : SubsysReco("EventCheck")
  , m_triggerBit(17) /*Jet 8 GeV + MBD NS >= 1*/
  , m_zvtx_max(30) /*cm*/
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
  if(fabs(m_zvtx) >= m_zvtx_max) {
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_triggeranalyzer->decodeTriggers(topNode);

  // skip event if it did not fire the desired trigger
  if(!m_triggeranalyzer->didTriggerFire(m_triggerBit)) {
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

