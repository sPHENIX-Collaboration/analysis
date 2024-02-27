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

EventSelection::EventSelection(const double jet_R, const std::string& outputfilename)
 : SubsysReco("EventSelection")
 , m_jet_R(jet_R)
 , m_outputfilename(outputfilename)
 , m_vtxZ_cut(10.0)
 , m_event(-1)
 , m_tree(nullptr) // Initialize m_tree to nullptr

{std::cout << "Output file path: " << m_outputfilename << std::endl;
}

EventSelection::~EventSelection()
{}
/////////////////////
int EventSelection::Init(PHCompositeNode *topNode)
{  // create output tree
  PHTFileServer::get().open(m_outputfilename, "RECREATE");
  outFile = new TFile(m_outputfilename.c_str(), "RECREATE");
  m_tree = new TTree("T", "EventSelection");

  std::cout << "EventSelection::Init(PHCompositeNode *topNode) Initialization successful" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK; // Ensure that this return statement is within the function body
}

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
  if (fabs(vtxMap->get(0)->get_z()) > m_vtxZ_cut)
    {
      if(Verbosity()) std::cout << "vertex not in range" << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  // If the event passes the z vertex selection, fill the output tree
  
  //==================================
  // Fill tree
  //==================================
  m_tree->Fill();


  return Fun4AllReturnCodes::EVENT_OK;
}

int EventSelection::End(PHCompositeNode *topNode)
{
  PHTFileServer::get().cd(m_outputfilename);
  std::cout << "EventSelection::End - Output to " << m_outputfilename << std::endl;

  // Write tree to file
  if (m_tree) {
    PHTFileServer::get().cd(m_outputfilename);
    m_tree->Write();
  } else {
    std::cerr << "EventSelection::End - Error: TTree not initialized" << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  outFile->cd();
  outFile->Write();
  outFile->Close();
  
  std::cout << "EventSelection::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}



