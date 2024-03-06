#include "EventSelection.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <jetbase/Jet.h>
#include <jetbase/JetMap.h>
#include <jetbase/Jetv1.h>
#include <jetbase/JetAlgo.h>
#include <jetbase/FastJetAlgo.h>
#include <jetbase/JetInput.h>
#include <jetbase/TowerJetInput.h>
#include <jetbase/JetMapv1.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>

#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertexMapv1.h>

#include <cmath>
#include <map>
#include <utility>
#include <cstdlib>  // for exit
#include <iostream>
#include <memory>  // for allocator_traits<>::value_type
#include <vector>

#include <TTree.h>
#include <TFile.h>

using namespace std;

EventSelection::EventSelection(const std::string& truthjetname, const std::string& outputfilename)
 : SubsysReco("EventSelection")
 , m_outputfilename(outputfilename)
 , outFile(nullptr)
 , m_tree(nullptr) // Initialize m_tree to nullptr
 , m_vtxZ_cut(10.0)
 , m_event(-1)
 , m_vertex_z()
   // , m_vertex_z()
{std::cout << "Output file path: " << m_outputfilename << std::endl;
}

EventSelection::~EventSelection()
{}

/////////////////////
int EventSelection::Init(PHCompositeNode *topNode)
{  // create output tree

  std::cout << "Output file path (Init): " << m_outputfilename << std::endl;
  outFile = new TFile(m_outputfilename.c_str(), "RECREATE");
  if (!outFile || outFile->IsZombie()) {
    std::cerr << "Error: Could not open output file " << m_outputfilename << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }
  m_tree = new TTree("Tree", "EventSelection");
  m_tree->Branch("m_vertex_z", &m_vertex_z);
  m_tree->Branch("m_event", &m_event, "event/I");

  std::cout << "EventSelection::Init(PHCompositeNode *topNode) Initialization successful" << std::endl;
  std::cout << "EventSelection::Init(PHCompositeNode *topNode) Output tree: " << m_tree->GetName() << std::endl;

  return Fun4AllReturnCodes::EVENT_OK; // Ensure that this return statement is within the function body
}

//////////////////
int EventSelection::InitRun(PHCompositeNode *topNode)
{
  std::cout << "EventSelection::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//////////////////
int EventSelection::process_event(PHCompositeNode *topNode)
{
  //std::cout << "EventSelection::process_event(PHCompositeNode *topNode) Processing event " << m_event << std::endl;
  ++m_event;

  GlobalVertexMap *vtxMap = findNode::getClass<GlobalVertexMapv1>(topNode,"GlobalVertexMap");
  if (!vtxMap)
    {
      if(Verbosity()) std::cout << "EventSelection::processEvent(PHCompositeNode *topNode) Could not find global vertex map node" << std::endl;
      exit(-1);
    }
  if (!vtxMap->get(0))
    {
      if(Verbosity()) std::cout << "no vertex found" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  double mvtxz =vtxMap->get(0)->get_z();
  
  if (fabs(mvtxz) > m_vtxZ_cut)
    {
      if(Verbosity()) std::cout << "vertex not in range" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;

    }
  m_vertex_z = (float) mvtxz;

  // If the event passes the z vertex selection, fill the output tree
  
  //==================================
  // Fill tree
  //==================================
  m_tree->Fill();
  //  std::cout << "m_vertex_z after fill: " << m_vertex_z << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

int EventSelection::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "EventSelection::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  //clear vectors or arrays here 
  //m_vertex_z.clear();

 return Fun4AllReturnCodes::EVENT_OK;

}
//____________________________________________________________________________..
int EventSelection::EndRun(const int runnumber)
{
  std::cout << "EventSelection::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
/////////////////_____________________________________________________________..
int EventSelection::End(PHCompositeNode *topNode)
{

  outFile->cd();
  //outFile->Write();
  m_tree->Write();
  outFile->Close();
  
  std::cout << "EventSelection::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//////////////////
int EventSelection::Reset(PHCompositeNode *topNode)
{
  // std::cout << "EventSelection::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void EventSelection::Print(const std::string &what) const
{
  std::cout << "EventSelection::Print(const std::string &what) const Printing info for " << what << std::endl;
}
