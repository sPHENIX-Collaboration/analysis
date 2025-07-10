#include "Detinfo.h"

// -- c++
#include <iostream>
#include <sstream>

// -- event
#include <ffaobjects/EventHeader.h>

#include <ffamodules/CDBInterface.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

#include <ffarawobjects/Gl1Packet.h>

#include <globalvertex/MbdVertex.h>

#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

#include <eventplaneinfo/Eventplaneinfo.h>
#include <eventplaneinfo/Eventplaneinfov1.h>
#include <eventplaneinfo/EventplaneinfoMap.h>

#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <TFile.h>

//____________________________________________________________________________..
Detinfo::Detinfo(const std::string &name)
 : SubsysReco(name)
 , m_event(0)
 , m_overrideSEPDMapName(false)
 , m_overrideSEPDFieldName(false)
 , cdbttree(nullptr)
{}

//____________________________________________________________________________..
int Detinfo::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  //get sEPD tower mapping from CBD
  if (!m_overrideSEPDMapName) {
    m_sEPDMapName = "SEPD_CHANNELMAP";
  }
  if (!m_overrideSEPDFieldName) {
    m_sEPDfieldname = "epd_channel_map";
  }
  std::string calibdir = CDBInterface::instance()->getUrl(m_sEPDMapName);
  if (!calibdir.empty()) {
        cdbttree = std::make_unique<CDBTTree>(calibdir);
 } else
 {
    std::cout << "Detinfo::::InitRun No SEPD mapping file for domain " << m_sEPDMapName << " found" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
 }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Detinfo::process_event(PHCompositeNode *topNode)
{

    EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
    if(!eventInfo)
    {
        std::cout << PHWHERE << "Detinfo::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    Int_t m_globalEvent = eventInfo->get_EvtSequence();
    // Int_t m_run         = eventInfo->get_RunNumber();

    if (m_event % 20 == 0) {
        std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
    }
    ++m_event;

    return Fun4AllReturnCodes::EVENT_OK;
}


//____________________________________________________________________________..
int Detinfo::End([[maybe_unused]] PHCompositeNode *topNode)
{
    std::cout << "Detinfo::End" << std::endl;

    return Fun4AllReturnCodes::EVENT_OK;

}
