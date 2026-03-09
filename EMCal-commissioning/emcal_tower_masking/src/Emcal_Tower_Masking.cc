
#include "Emcal_Tower_Masking.h"

#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>

#include <caloreco/CaloWaveformProcessing.h>  // for CaloWaveformProcessing
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexv1.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/SubsysReco.h>  // for SubsysReco

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>    // for PHIODataNode
#include <phool/PHNode.h>          // for PHNode
#include <phool/PHNodeIterator.h>  // for PHNodeIterator
#include <phool/PHObject.h>        // for PHObject
#include <phool/getClass.h>

#include <Event/Event.h>
#include <Event/packet.h>

#include <iostream>  // for operator<<, endl, basic...
#include <memory>    // for allocator_traits<>::val...
#include <vector>    // for vector
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//____________________________________________________________________________..
Emcal_Tower_Masking::Emcal_Tower_Masking(const std::string &name):
 SubsysReco(name)
{
}

//____________________________________________________________________________..
Emcal_Tower_Masking::~Emcal_Tower_Masking()
{
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::Init(PHCompositeNode *topNode)
{
 
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::InitRun(PHCompositeNode *topNode)
{
 CreateNodeTree(topNode);
  topNode->print();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::process_event(PHCompositeNode *topNode)
{
  float emcalescale = 1.0;

  TowerInfoContainer* emcaltowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERS_CEMC");
  if (!emcaltowers)
  {
    std::cout
        << "MyJetAnalysis::process_event - Error can not find DST TOWERS_CEMC node "
        << std::endl;
    exit(-1);
  }
  TowerInfoContainer* calib_emcaltowers = findNode::getClass<TowerInfoContainerv1>(topNode, "TOWERINFO_CALIB_CEMC");
  if (!calib_emcaltowers)
  {
    std::cout
        << "MyJetAnalysis::process_event - Error can not find DST TOWERINFO_CALIB_CEMC node "
        << std::endl;
    exit(-1);
  }


  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  GlobalVertex *vertex = new GlobalVertexv1(GlobalVertex::VTXTYPE::SVTX);
  vertex->set_id(vertexmap->size());
  for (unsigned int i = 0; i < 3; ++i)
    {
      vertex->set_position(i, 0);
      for (unsigned int j = i; j < 3; ++j)
        {
          vertex->set_error(i, j, 0);
        }
      }
  vertexmap->insert(vertex);


  {
    unsigned int nchannels = emcaltowers->size();
    for (unsigned int channel = 0; channel < nchannels;channel++)
      {
	TowerInfo* tower = emcaltowers->get_tower_at_channel(channel);
	TowerInfo* calib_tower = calib_emcaltowers->get_tower_at_channel(channel);
	unsigned int channelkey = emcaltowers->encode_key(channel);
	unsigned int ieta = emcaltowers->getTowerEtaBin(channelkey);
	unsigned int iphi = emcaltowers->getTowerPhiBin(channelkey);
	int n_masked_packets = packetmasklist.size();
	bool maskpacket = false;
	if (n_masked_packets > 0 )
	  {
	    for (int i = 0; i < n_masked_packets;i++)
	      {
		if (channel >= 192*(packetmasklist.at(i)-1) && channel < 192*packetmasklist.at(i))
		  {
		    maskpacket = true;
		  }
	      }
	  }
	if (maskpacket == true)
	  {
	    calib_tower->set_energy(0);
	    calib_tower->set_time(0);	
	  }
	else
	  {
	    calib_tower->set_energy(tower->get_energy() * emcalescale);
	    calib_tower->set_time(tower->get_time());
	  }
	int nbads = etamasklist.size();
	if(nbads > 0 )
	  {
	    for(int q =0 ; q < nbads;q++)
	      {
		if (ieta == etamasklist.at(q) && iphi == phimasklist.at(q))
		  {
		    calib_tower->set_energy(0);
		    calib_tower->set_time(0);
		  }
	      }
	  }
	
      }
  }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int Emcal_Tower_Masking::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void Emcal_Tower_Masking::Print(const std::string &what) const
{
  std::cout << "Emcal_Tower_Masking::Print(const std::string &what) const Printing info for " << what << std::endl;
}

void Emcal_Tower_Masking::CreateNodeTree(PHCompositeNode *topNode)
{
  PHNodeIterator nodeItr(topNode);
  // DST node
  PHCompositeNode *dst_node = dynamic_cast<PHCompositeNode *>(
							      nodeItr.findFirst("PHCompositeNode", "DST"));
  if (!dst_node)
    {
      std::cout << "PHComposite node created: DST" << std::endl;
      dst_node = new PHCompositeNode("DST");
      topNode->addNode(dst_node);
    }
  PHNodeIterator dstItr(dst_node);

  PHCompositeNode *CEMCNode = dynamic_cast<PHCompositeNode *>(dstItr.findFirst("PHCompositeNode", "CEMC"));
  if (!CEMCNode)
    {
      std::cout << " no CEMC Node? " << std::endl;
      CEMCNode = new PHCompositeNode("CEMC");
    }
  TowerInfoContainer* m_CEMC_calib_container = new TowerInfoContainerv1(TowerInfoContainer::DETECTOR::EMCAL);
  PHIODataNode<PHObject> *emcal_towerNode = new PHIODataNode<PHObject>(m_CEMC_calib_container, "TOWERINFO_CALIB_CEMC", "PHObject");
  CEMCNode->addNode(emcal_towerNode);


  // store the GLOBAL stuff under a sub-node directory
  PHCompositeNode *globalNode = dynamic_cast<PHCompositeNode *>(dstItr.findFirst("PHCompositeNode", "GLOBAL"));
  if (!globalNode)
  {
    globalNode = new PHCompositeNode("GLOBAL");
    dst_node->addNode(globalNode);
  }

  // create the GlobalVertexMap
  GlobalVertexMap *vertexes = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexes)
  {
    vertexes = new GlobalVertexMapv1();
    PHIODataNode<PHObject> *VertexMapNode = new PHIODataNode<PHObject>(vertexes, "GlobalVertexMap", "PHObject");
    globalNode->addNode(VertexMapNode);
  }

}
