#include "CopyCutVertex.h"

// Calo 
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>  // for PHNode
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>  // for PHObject
#include <phool/getClass.h>
#include <phool/phool.h>  // for PHWHERE
#include <fun4all/Fun4AllReturnCodes.h>
#include <ffarawobjects/Gl1Packet.h>
#include <globalvertex/MbdVertexv2.h>
#include <globalvertex/MbdVertexMapv1.h>
#include <globalvertex/Vertex.h>
#include <globalvertex/GlobalVertexv2.h>
#include <globalvertex/GlobalVertexMapv1.h>

#include <cassert>
#include <cmath>  // for log10, pow, sqrt, abs, M_PI
#include <cstdint>
#include <iostream>  // for operator<<, endl, basic_...
#include <limits>
#include <map>  // for operator!=, _Rb_tree_con...
#include <memory>
#include <string>
#include <utility>  // for pair

CopyCutVertex::CopyCutVertex(const std::string& name)
: SubsysReco(name)
{						

}

CopyCutVertex::~CopyCutVertex()
{
  
}

int CopyCutVertex::Init(PHCompositeNode* topNode)
{
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cout << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  // store the GLOBAL stuff under a sub-node directory
  PHCompositeNode *globalNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "GLOBAL"));
  if (!globalNode)
    {
      globalNode = new PHCompositeNode("GLOBAL");
      dstNode->addNode(globalNode);
    }

  // create the GlobalVertexMap
  GlobalVertexMap *vertexes = findNode::getClass<GlobalVertexMap>(topNode, "RealVertexMap");
  if (!vertexes)
    {
      vertexes = new GlobalVertexMapv1();
      PHIODataNode<PHObject> *VertexMapNode = new PHIODataNode<PHObject>(vertexes, "RealVertexMap", "PHObject");
      globalNode->addNode(VertexMapNode);
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

int CopyCutVertex::process_event(PHCompositeNode* topNode)
{

  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
    {
      return Fun4AllReturnCodes::EVENT_OK;
    }

  GlobalVertexMap *my_vertexmap = findNode::getClass<GlobalVertexMap>(topNode, m_nodename.c_str());


  if (!vertexmap->empty())
    {
      GlobalVertex *myvtx = vertexmap->begin()->second;
      auto mbd2 = new MbdVertexv2();
      mbd2->set_id(0);
      mbd2->set_t(0);
      mbd2->set_z(myvtx->get_z());
      mbd2->set_z_err(0.6);
      mbd2->set_t_err(0.1);
      mbd2->set_beam_crossing(0);
      GlobalVertex *vtx2 = new GlobalVertexv2();
      vtx2->set_id(my_vertexmap->size());
      vtx2->insert_vtx(GlobalVertex::MBD, mbd2);
      my_vertexmap->insert(vtx2);
    }

  vertexmap->Reset();

  auto mbd = new MbdVertexv2();
  mbd->set_id(0);
  mbd->set_t(0);
  mbd->set_z(0);
  mbd->set_z_err(0.6);
  mbd->set_t_err(0.1);
  mbd->set_beam_crossing(0);

  GlobalVertex *vertex = new GlobalVertexv2();
  vertex->set_id(vertexmap->size());
  vertex->insert_vtx(GlobalVertex::MBD, mbd);
  vertexmap->insert( vertex);
  return Fun4AllReturnCodes::EVENT_OK;;
}

int CopyCutVertex::End(PHCompositeNode* /*unused*/) 
{
  return Fun4AllReturnCodes::EVENT_OK;
}
