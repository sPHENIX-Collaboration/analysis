//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloCheck.h.
//
// CaloCheck(const std::string &name = "CaloCheck")
// everything is keyed to CaloCheck, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloCheck::~CaloCheck()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloCheck::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloCheck::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloCheck::process_event(PHCompositeNode *topNode)
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
// int CaloCheck::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloCheck::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloCheck::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloCheck::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloCheck::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloCheck.h"
#include "geometry_constants.h"

#include <fun4all/Fun4AllReturnCodes.h>

// -- Event
#include <ffaobjects/EventHeader.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- Calo
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>
#include <phool/recoConsts.h>

// ====================================================================
// ROOT Includes
// ====================================================================
#include <TFile.h>

// c++
#include <format>

//____________________________________________________________________________..
CaloCheck::CaloCheck(const std::string& name)
  : SubsysReco(name)
{
  std::cout << "CaloCheck::CaloCheck(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
int CaloCheck::Init([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloCheck::CaloCheck(const std::string &name) Calling init" << std::endl;

  recoConsts *rc = recoConsts::instance();
  m_run = rc->get_uint64Flag("TIMESTAMP");

  std::cout << std::format("Run: {}\n", m_run);

  std::string title = std::format("EMCal Energy: Run {}, Event: {}; Tower Index #phi; Tower Index #eta", m_run, m_event_id);
  m_hists2D["h2EMCalBase"] = std::make_unique<TH2F>("h2EMCalBase", title.c_str(), CaloGeometry::CEMC_PHI_BINS, 0, CaloGeometry::CEMC_PHI_BINS,
                                                                                  CaloGeometry::CEMC_ETA_BINS, 0, CaloGeometry::CEMC_ETA_BINS);

  m_hists2D["h2EMCal"] = std::make_unique<TH2F>("h2EMCal", title.c_str(), CaloGeometry::HCAL_PHI_BINS, 0, CaloGeometry::HCAL_PHI_BINS,
                                                                          CaloGeometry::HCAL_ETA_BINS, 0, CaloGeometry::HCAL_ETA_BINS);

  title = std::format("IHCal Energy: Run {}, Event: {}; Tower Index #phi; Tower Index #eta", m_run, m_event_id);
  m_hists2D["h2IHCal"] = std::make_unique<TH2F>("h2IHCal", title.c_str(), CaloGeometry::HCAL_PHI_BINS, 0, CaloGeometry::HCAL_PHI_BINS,
                                                                          CaloGeometry::HCAL_ETA_BINS, 0, CaloGeometry::HCAL_ETA_BINS);

  title = std::format("OHCal Energy: Run {}, Event: {}; Tower Index #phi; Tower Index #eta", m_run, m_event_id);
  m_hists2D["h2OHCal"] = std::make_unique<TH2F>("h2OHCal", title.c_str(), CaloGeometry::HCAL_PHI_BINS, 0, CaloGeometry::HCAL_PHI_BINS,
                                                                          CaloGeometry::HCAL_ETA_BINS, 0, CaloGeometry::HCAL_ETA_BINS);

  int bins_energy = 600;
  double energy_low = -300;
  double energy_high = 300; // MeV

  title = std::format("EMCal: Run {}, Event: {}; Tower Energy [MeV]; Counts", m_run, m_event_id);
  m_hists1D["hEMCal"] = std::make_unique<TH1F>("hEMCal", title.c_str(), bins_energy, energy_low, energy_high);

  title = std::format("IHCal: Run {}, Event: {}; Tower Energy [MeV]; Counts", m_run, m_event_id);
  m_hists1D["hIHCal"] = std::make_unique<TH1F>("hIHCal", title.c_str(), bins_energy, energy_low, energy_high);

  title = std::format("OHCal: Run {}, Event: {}; Tower Energy [MeV]; Counts", m_run, m_event_id);
  m_hists1D["hOHCal"] = std::make_unique<TH1F>("hOHCal", title.c_str(), bins_energy, energy_low, energy_high);

  m_hists.h2EMCalBase = m_hists2D["h2EMCalBase"].get();
  m_hists.h2EMCal = m_hists2D["h2EMCal"].get();
  m_hists.h2IHCal = m_hists2D["h2IHCal"].get();
  m_hists.h2OHCal = m_hists2D["h2OHCal"].get();

  m_hists.hEMCal = m_hists1D["hEMCal"].get();
  m_hists.hIHCal = m_hists1D["hIHCal"].get();
  m_hists.hOHCal = m_hists1D["hOHCal"].get();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::process_event([[maybe_unused]] PHCompositeNode* topNode)
{
  // Event
  auto* eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int globalEvent = eventInfo->get_EvtSequence();

  std::cout << std::format("Event: {}\n", globalEvent);

  // Z Vertex
  auto* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  double zvtx = -9999;

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    zvtx = vtx->get_z();
  }

  std::cout << std::format("Z Vertex: {:.2f} cm\n", zvtx);

  auto* towersCEMCbase = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  auto* towersCEMC     = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  auto* towersIHCal    = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal    = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  if (!towersCEMCbase)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("EMCal Base: Total Towers {}\n", towersCEMCbase->size());
  for(unsigned int towerIndex = 0; towerIndex < towersCEMCbase->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersCEMCbase->get_tower_at_channel(towerIndex);

    if (!tower->get_isGood())
    {
      continue;
    }

    double energy =  tower->get_energy();

    m_hists.h2EMCalBase->Fill(iphi, ieta, energy);

    if (Verbosity() > 0)
    {
      std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
    }
  }

  if (!towersCEMC)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("EMCal Retowered: Total Towers {}\n", towersCEMC->size());
  for(unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersCEMC->get_tower_at_channel(towerIndex);

    if (!tower->get_isGood())
    {
      continue;
    }

    double energy = tower->get_energy();

    m_hists.h2EMCal->Fill(iphi, ieta, energy);
    m_hists.hEMCal->Fill(energy*1e3);

    if (Verbosity() > 0)
    {
      std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
    }
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("IHCal: Total Towers {}\n", towersIHCal->size());
  for(unsigned int towerIndex = 0; towerIndex < towersIHCal->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersIHCal->get_tower_at_channel(towerIndex);

    if (!tower->get_isGood())
    {
      continue;
    }

    double energy =  tower->get_energy();

    m_hists.h2IHCal->Fill(iphi, ieta, energy);
    m_hists.hIHCal->Fill(energy*1e3);

    if (Verbosity() > 0)
    {
      std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
    }
  }

  std::cout << std::format("{:#<20}\n", "");
  std::cout << std::format("OHCal: Total Towers {}\n", towersOHCal->size());
  for(unsigned int towerIndex = 0; towerIndex < towersOHCal->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersOHCal->get_tower_at_channel(towerIndex);

    if (!tower->get_isGood())
    {
      continue;
    }

    double energy =  tower->get_energy();

    m_hists.h2OHCal->Fill(iphi, ieta, energy);
    m_hists.hOHCal->Fill(energy*1e3);

    if (Verbosity() > 0)
    {
      std::cout << std::format("Tower: {}: ({},{}), Energy: {:.6f} GeV\n", towerIndex, iphi, ieta, energy);
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::ResetEvent([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloCheck::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloCheck::End(PHCompositeNode *topNode) This is the End..." << std::endl;

  std::string output_filename = std::format("CaloCheck-{}-{}.root", m_run, m_event_id);

  auto output_file = std::make_unique<TFile>(output_filename.c_str(), "RECREATE");

  for (const auto& [name, hist] : m_hists1D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }

  for (const auto& [name, hist] : m_hists2D)
  {
    std::cout << std::format("Saving: {}, Estimated Size: {} Bytes\n", name, hist->Sizeof());
    hist->Write();
  }

  output_file->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
