//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in HCalJetPhiShift.h.
//
// HCalJetPhiShift(const std::string &name = "HCalJetPhiShift")
// everything is keyed to HCalJetPhiShift, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// HCalJetPhiShift::~HCalJetPhiShift()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int HCalJetPhiShift::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int HCalJetPhiShift::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int HCalJetPhiShift::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT; 
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT; 
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int HCalJetPhiShift::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int HCalJetPhiShift::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int HCalJetPhiShift::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void HCalJetPhiShift::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "HCalJetPhiShift.h"

#include "g4eval/CaloEvalStack.h"
#include "g4eval/CaloRawClusterEval.h"
#include "g4eval/CaloRawTowerEval.h"
#include "g4eval/CaloTruthEval.h"

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <TFile.h>
#include <TTree.h>

//____________________________________________________________________________..
HCalJetPhiShift::HCalJetPhiShift(const std::string &name, const std::string &outputFile):
SubsysReco(name),
m_outputFileName(outputFile),
m_T(nullptr),
m_event(-1),
m_nTow_in(0),
m_nTow_out(0),
m_eta(),
m_phi(),
m_e(),
m_pt(),
m_vx(),
m_vy(),
m_vz(),
m_id(),
m_eta_in(),
m_phi_in(),
m_e_in(),
m_ieta_in(),
m_iphi_in(),
m_eta_out(),
m_phi_out(),
m_e_out(),
m_ieta_out(),
m_iphi_out()
{
  std::cout << "HCalJetPhiShift::HCalJetPhiShift(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
HCalJetPhiShift::~HCalJetPhiShift()
{
  std::cout << "HCalJetPhiShift::~HCalJetPhiShift() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int HCalJetPhiShift::Init(PHCompositeNode* /*topNode*/)
{
  std::cout << "HCalJetPhiShift::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  PHTFileServer::get().open(m_outputFileName, "RECREATE");
  std::cout << "HCalJetPhiShift::Init - Output to " << m_outputFileName << std::endl;
  
  // configure Tree
  m_T = new TTree("T", "HCalJetPhiShift Tree");
  m_T->Branch("event", &m_event);
  m_T->Branch("eta", &m_eta);
  m_T->Branch("phi", &m_phi);
  m_T->Branch("e", &m_e);
  m_T->Branch("pt", &m_pt);
  m_T->Branch("vx", &m_vx);
  m_T->Branch("vy", &m_vy);
  m_T->Branch("vz", &m_vz);
  m_T->Branch("id", &m_id);
  m_T->Branch("nTow_in", &m_nTow_in);
  m_T->Branch("eta_in", &m_eta_in);
  m_T->Branch("phi_in", &m_phi_in);
  m_T->Branch("e_in", &m_e_in);
  m_T->Branch("ieta_in", &m_ieta_in);
  m_T->Branch("iphi_in", &m_iphi_in);
  m_T->Branch("nTow_out", &m_nTow_out);
  m_T->Branch("eta_out", &m_eta_out);
  m_T->Branch("phi_out", &m_phi_out);
  m_T->Branch("e_out", &m_e_out);
  m_T->Branch("ieta_out", &m_ieta_out);
  m_T->Branch("iphi_out", &m_iphi_out);
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::InitRun(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::process_event(PHCompositeNode *topNode)
{
  //  std::cout << "HCalJetPhiShift::process_event(PHCompositeNode *topNode) Processing Event" << std::endl;
  ++m_event;
  
  //fill the tree
  FillTTree(topNode);
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode)
{
  //  std::cout << "HCalJetPhiShift::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
  m_nTow_in = 0;
  m_nTow_out = 0;
  m_id.clear();
  m_eta_in.clear();
  m_phi_in.clear();
  m_e_in.clear();
  m_ieta_in.clear();
  m_iphi_in.clear();
  
  m_eta_out.clear();
  m_phi_out.clear();
  m_e_out.clear();
  m_ieta_out.clear();
  m_iphi_out.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::EndRun(const int runnumber)
{
  std::cout << "HCalJetPhiShift::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::End(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::End - Output to " << m_outputFileName << std::endl;
  PHTFileServer::get().cd(m_outputFileName);
  
  m_T->Write();
  std::cout << "HCalJetPhiShift::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int HCalJetPhiShift::Reset(PHCompositeNode *topNode)
{
  std::cout << "HCalJetPhiShift::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void HCalJetPhiShift::Print(const std::string &what) const
{
  std::cout << "HCalJetPhiShift::Print(const std::string &what) const Printing info for " << what << std::endl;
}

//____________________________________________________________________________..
int HCalJetPhiShift::FillTTree(PHCompositeNode *topNode)
{  
//  std::cout << "HCalJetPhiShift::FillTTree(PHCompositeNode *topNode)" << std::endl;
  
  // fill truth info from nodetree
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo)
  {
    std::cout << PHWHERE << " ERROR: Can't find G4TruthInfo" << std::endl;
    exit(-1);
  }
  
  PHG4TruthInfoContainer::ConstRange range = truthinfo->GetPrimaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second;
       ++iter)
  {
    PHG4Particle* primary = iter->second;
    
    if (primary->get_e() < 0.)
    {
      continue;
    }
    
    float gpx = primary->get_px();
    float gpy = primary->get_py();
    float gpz = primary->get_pz();
    m_e = primary->get_e();
    
    m_pt = std::sqrt(gpx * gpx + gpy * gpy);
    m_eta = NAN;
    if (m_pt != 0.0)
    {
      m_eta = std::asinh(gpz / m_pt);
    }
    m_phi = std::atan2(gpy, gpx);
    
    PHG4VtxPoint* gvertex = truthinfo->GetPrimaryVtx(truthinfo->GetPrimaryVertexIndex());
    m_vx = gvertex->get_x();
    m_vy = gvertex->get_y();
    m_vz = gvertex->get_z();
  }
  
  //calorimeter towers
  TowerInfoContainer *towersIH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer *towersOH3 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!towersIH3 || !towersOH3){
    std::cout
    <<"MyJetAnalysis::process_event - Error cannot find raw tower node "
    << std::endl;
    exit(-1);
  }
  
  if(!tower_geomIH || !tower_geomOH){
    std::cout
    <<"MyJetAnalysis::process_event - Error cannot find raw tower geometry "
    << std::endl;
    exit(-1);
  }
  
  TowerInfo *tower_in, *tower_out;
  const int n_channels_IH = (int) towersIH3->size();
  
  // Inner HCal
  for (int i_chan=0; i_chan<n_channels_IH; ++i_chan) {
    tower_in = towersIH3->get_tower_at_channel(i_chan);
    if (tower_in->get_energy()>0.) {
      ++m_nTow_in;
      
      unsigned int calokey = towersIH3->encode_key(i_chan);
      int ieta = towersIH3->getTowerEtaBin(calokey);
      int iphi = towersIH3->getTowerPhiBin(calokey);
      
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
      
      m_eta_in.push_back(tower_eta);
      m_phi_in.push_back(tower_phi);
      m_e_in.push_back(tower_in->get_energy());
      m_ieta_in.push_back(ieta);
      m_iphi_in.push_back(iphi);
    }

    // Outer HCal
    tower_out = towersOH3->get_tower_at_channel(i_chan);
    if (tower_out->get_energy()>0.) {
      ++m_nTow_out;
      
      unsigned int calokey = towersOH3->encode_key(i_chan);
      int ieta = towersOH3->getTowerEtaBin(calokey);
      int iphi = towersOH3->getTowerPhiBin(calokey);
      
      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
      float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
      float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
      
      m_eta_out.push_back(tower_eta);
      m_phi_out.push_back(tower_phi);
      m_e_out.push_back(tower_out->get_energy());
      m_ieta_out.push_back(ieta);
      m_iphi_out.push_back(iphi);
    }

  }

  m_T->Fill();

  return Fun4AllReturnCodes::EVENT_OK;
}
