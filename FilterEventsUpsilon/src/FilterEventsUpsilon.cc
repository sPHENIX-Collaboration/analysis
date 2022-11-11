
#include "FilterEventsUpsilon.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase/TrkrDefs.h>
#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

typedef PHIODataNode<PHObject> PHObjectNode_t;

using namespace std;

//==============================================================

FilterEventsUpsilon::FilterEventsUpsilon(const std::string &name) : SubsysReco(name)
{
  outnodename = "FilterEventsUpsilonNode";
  EventNumber=0;
}

//-------------------------------------------------------------------------------

int FilterEventsUpsilon::Init(PHCompositeNode *topNode)
{

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cerr << PHWHERE << " ERROR: Can not find DST node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  std::cout << "FilterEventsUpsilon::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEventsUpsilon::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEventsUpsilon::process_event(PHCompositeNode *topNode) {
  EventNumber++;

  SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!trackmap) {
    cerr << PHWHERE << " ERROR: Can not find SvtxTrackMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  SvtxVertexMap *vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!vtxmap) {
      cout << "SvtxVertexMap node not found!" << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertexMap *global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!global_vtxmap) {
    cerr << PHWHERE << " ERROR: Can not find GlobalVertexMap node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  RawClusterContainer* cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!cemc_clusters) {
    cerr << PHWHERE << " ERROR: Can not find CLUSTER_CEMC node." << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int FilterEventsUpsilon::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

