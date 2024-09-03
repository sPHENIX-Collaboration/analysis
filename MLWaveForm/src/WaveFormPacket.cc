
#include "WaveFormPacket.h"

#include <ffarawobjects/CaloPacket.h>
#include <ffarawobjects/CaloPacketContainer.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

//____________________________________________________________________________..
WaveFormPacket::WaveFormPacket(const std::string &name):
 SubsysReco(name)
{
  outfile.open("data.txt",std::fstream::out);
}


//____________________________________________________________________________..
int WaveFormPacket::process_event(PHCompositeNode *topNode)
{
    CaloPacketContainer *calocont = findNode::getClass<CaloPacketContainer>(topNode, "CEMCPackets");
  if (calocont)
  {
    for (unsigned int i = 0; i < calocont->get_npackets(); i++)
    {
      CaloPacket *calopacket = calocont->getPacket(i);
      for (int k = 0; k < calopacket->iValue(0, "CHANNELS"); k++)
      {
	if (calopacket->iValue(k, "SUPPRESSED"))
	{
	  continue;
	}
        for (int j = 0; j < calopacket->iValue(0, "SAMPLES"); j++)
        {
	  std::cout << "channel " << k << ", sample " << j << "wf: " << calopacket->iValue(j, k) << std::endl;
outfile << calopacket->iValue(j, k) << std::endl;
	}
//	std::cout << "waveform_value[" << j << "]: " << rawtwr->get_waveform_value(j) << std::endl;
      }
    }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int WaveFormPacket::End(PHCompositeNode * /*topNode*/)
{
  outfile.close();
  return Fun4AllReturnCodes::EVENT_OK;
}
