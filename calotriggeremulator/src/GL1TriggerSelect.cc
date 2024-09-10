#include "GL1TriggerSelect.h"

// Calo 
#include <phool/getClass.h>
#include <phool/phool.h>  // for PHWHERE
#include <fun4all/Fun4AllReturnCodes.h>
#include <ffarawobjects/Gl1Packet.h>

#include <cassert>
#include <cmath>  // for log10, pow, sqrt, abs, M_PI
#include <cstdint>
#include <iostream>  // for operator<<, endl, basic_...
#include <limits>
#include <map>  // for operator!=, _Rb_tree_con...
#include <string>
#include <utility>  // for pair

GL1TriggerSelect::GL1TriggerSelect(const std::string& name)
: SubsysReco(name)
{						

}

GL1TriggerSelect::~GL1TriggerSelect()
{
  
}

int GL1TriggerSelect::Init(PHCompositeNode* /*unused*/)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

int GL1TriggerSelect::process_event(PHCompositeNode* topNode)
{
  _eventcounter++;  
  //  std::cout << "In process_event" << std::endl;

  Gl1Packet *gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, "GL1Packet");
  if (!gl1PacketInfo)
  {
    std::cout << PHWHERE << "GL1TriggerSelect::process_event: GL1Packet node is missing" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  

  uint64_t triggervec = gl1PacketInfo->getScaledVector();
  bool mbd_event = (((triggervec >> m_triggerbit) & 0x1U) == 0x1U);
  
  if (!mbd_event)
    {
      std::cout << " GL1TriggerSelect ABORTING EVENT" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
      std::cout << " GL1TriggerSelect SELECT EVENT" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
  
}

int GL1TriggerSelect::End(PHCompositeNode* /*unused*/) 
{
  return Fun4AllReturnCodes::EVENT_OK;
}
