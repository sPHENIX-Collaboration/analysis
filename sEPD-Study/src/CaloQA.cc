//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in CaloQA.h.
//
// CaloQA(const std::string &name = "CaloQA")
// everything is keyed to CaloQA, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// CaloQA::~CaloQA()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int CaloQA::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int CaloQA::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int CaloQA::process_event(PHCompositeNode *topNode)
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
// int CaloQA::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int CaloQA::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int CaloQA::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int CaloQA::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void CaloQA::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "CaloQA.h"
#include "geometry_constants.h"

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Event
#include <ffaobjects/EventHeader.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- MB
#include <calotrigger/MinimumBiasClassifier.h>
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- Calo
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

// c++
#include <format>

//____________________________________________________________________________..
CaloQA::CaloQA(const std::string& name)
  : SubsysReco(name)
{
  std::cout << "CaloQA::CaloQA(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
int CaloQA::Init([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloQA::CaloQA(const std::string &name) Calling init" << std::endl;

  int bins_emcal_phi = CaloGeometry::CEMC_PHI_BINS;
  int bins_emcal_eta = CaloGeometry::CEMC_ETA_BINS;

  int bins_hcal_phi = CaloGeometry::HCAL_PHI_BINS;
  int bins_hcal_eta = CaloGeometry::HCAL_ETA_BINS;

  int bins_cent = 8;
  int cent_low = 0;
  int cent_high = 80;

  int bins_energy = 200;
  int energy_low = -50;
  int energy_high = 50;

  m_hists.h2EMCal = new TProfile2D("h2EMCal", "EMCal; Tower Index #phi; Tower Index #eta",
                                    bins_emcal_phi, 0, bins_emcal_phi,
                                    bins_emcal_eta, 0, bins_emcal_eta);

  m_hists.h2IHCal = new TProfile2D("h2IHCal", "IHCal; Tower Index #phi; Tower Index #eta",
                                    bins_hcal_phi, 0, bins_hcal_phi,
                                    bins_hcal_eta, 0, bins_hcal_eta);

  m_hists.h2OHCal = new TProfile2D("h2OHCal", "OHCal; Tower Index #phi; Tower Index #eta",
                                    bins_hcal_phi, 0, bins_hcal_phi,
                                    bins_hcal_eta, 0, bins_hcal_eta);

  m_hists.h2EMCalCent = new TH2F("h2EMCalCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  m_hists.h2IHCalCent = new TH2F("h2IHCalCent", "IHCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  m_hists.h2OHCalCent = new TH2F("h2OHCalCent", "OHCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  m_hists.h2EMCalZSCent = new TH2F("h2EMCalZSCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  m_hists.h2IHCalZSCent = new TH2F("h2IHCalZSCent", "IHCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  m_hists.h2OHCalZSCent = new TH2F("h2OHCalZSCent", "OHCal; Tower Energy [GeV]; Centrality [%]",
                                  bins_energy, energy_low, energy_high,
                                  bins_cent, cent_low, cent_high);

  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  se->registerHisto(m_hists.h2EMCal);
  se->registerHisto(m_hists.h2IHCal);
  se->registerHisto(m_hists.h2OHCal);

  se->registerHisto(m_hists.h2EMCalCent);
  se->registerHisto(m_hists.h2IHCalCent);
  se->registerHisto(m_hists.h2OHCalCent);

  se->registerHisto(m_hists.h2EMCalZSCent);
  se->registerHisto(m_hists.h2IHCalZSCent);
  se->registerHisto(m_hists.h2OHCalZSCent);

  return Fun4AllReturnCodes::EVENT_OK;
}

int CaloQA::process_event_check(PHCompositeNode *topNode)
{
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    std::cout << PHWHERE << "GlobalVertexMap Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (vertexmap->empty())
  {
    if (Verbosity() > 1)
    {
      std::cout << PHWHERE << "GlobalVertexMap Empty, Skipping Event: " << m_data.event_id << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  GlobalVertex *vtx = vertexmap->begin()->second;
  double zvtx = vtx->get_z();

  MinimumBiasInfo *m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    std::cout << PHWHERE << "MinimumBiasInfo Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    if (Verbosity() > 1)
    {
      std::cout << "Event: " << m_data.event_id << ", Not Min Bias, Skipping" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // skip event if zvtx is too large
  if (std::abs(zvtx) >= m_cuts.m_zvtx_max)
  {
    if (Verbosity() > 1)
    {
      std::cout << "Event: " << m_data.event_id << ", Z: " << zvtx << " cm, Skipping" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    std::cout << PHWHERE << "CentralityInfo Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  double cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  // skip event if centrality is too peripheral
  if (!std::isfinite(cent) || cent < 0 || cent >= m_cuts.m_cent_max)
  {
    if (Verbosity() > 1)
    {
      std::cout << "Event: " << m_data.event_id << ", Centrality: " << cent << ", Skipping" << std::endl;
    }
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_data.centrality = cent;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::process_calo(PHCompositeNode *topNode)
{
  auto* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  double cent = m_data.centrality;

  if (!towersCEMC || !towersIHCal || !towersOHCal)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (towersIHCal->size() != towersOHCal->size())
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // EMCal
  for(unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex) {
    unsigned int key = TowerInfoDefs::encode_emcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* tower = towersCEMC->get_tower_at_channel(towerIndex);

    if (!tower->get_isGood())
    {
      continue;
    }

    double energy = tower->get_energy();

    m_hists.h2EMCal->Fill(iphi, ieta, energy);
    m_hists.h2EMCalCent->Fill(energy, cent);

    if (tower->get_isZS())
    {
      m_hists.h2EMCalZSCent->Fill(energy, cent);
    }
  }

  // HCal
  for (unsigned int towerIndex = 0; towerIndex < towersIHCal->size(); ++towerIndex)
  {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* IHCal_tower = towersIHCal->get_tower_at_channel(towerIndex);
    auto* OHCal_tower = towersOHCal->get_tower_at_channel(towerIndex);

    if (IHCal_tower->get_isGood())
    {
      double energy = IHCal_tower->get_energy();

      m_hists.h2IHCal->Fill(iphi, ieta, energy);
      m_hists.h2IHCalCent->Fill(energy, cent);

      if (IHCal_tower->get_isZS())
      {
        m_hists.h2IHCalZSCent->Fill(energy, cent);
      }
    }

    if (OHCal_tower->get_isGood())
    {
      double energy = OHCal_tower->get_energy();

      m_hists.h2OHCal->Fill(iphi, ieta, energy);
      m_hists.h2OHCalCent->Fill(energy, cent);

      if (OHCal_tower->get_isZS())
      {
        m_hists.h2OHCalZSCent->Fill(energy, cent);
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::process_event([[maybe_unused]] PHCompositeNode* topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << PHWHERE << "EventHeader Node missing, doing nothing." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_data.event_id = eventInfo->get_EvtSequence();

  if (Verbosity() && m_event % PROGRESS_PRINT_INTERVAL == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_data.event_id << std::endl;
  }
  ++m_event;

  int ret = process_event_check(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_centrality(topNode);
  if (ret)
  {
    return ret;
  }

  ret = process_calo(topNode);
  if (ret)
  {
    return ret;
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::ResetEvent([[maybe_unused]] PHCompositeNode* topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloQA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
