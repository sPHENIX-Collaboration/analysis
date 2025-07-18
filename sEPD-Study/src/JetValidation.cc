#include "JetValidation.h"
#include "JetUtils.h"

// -- c++
#include <iostream>
#include <sstream>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- Calo
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoDefs.h>

// -- Vtx
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- MB
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- jet
#include <jetbase/JetContainer.h>

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>

//____________________________________________________________________________..
JetValidation::JetValidation(const std::string &name)
  : SubsysReco(name)
  , m_event(0)
  , m_bins_zvtx(200) /*cm*/
  , m_zvtx_low(-50)  /*cm*/
  , m_zvtx_high(50)  /*cm*/
  , m_bins_cent(100)
  , m_cent_low(-0.5)
  , m_cent_high(99.5)
  , m_bins_phi(64)
  , m_phi_low(-M_PI)
  , m_phi_high(M_PI)
  , m_bins_eta(24)
  , m_eta_low(-1.1)
  , m_eta_high(1.1)
  , m_bins_pt(400)
  , m_pt_low(0)
  , m_pt_high(200)
  , m_bins_nJets(50)
  , m_nJets_low(0)
  , m_nJets_high(50)
  , m_zvtx(9999)
  , m_cent(9999)
  , m_zvtx_max(10) /*cm*/
  , m_jet_pt_min_cut(10) /*GeV*/
  , m_R(0.2)
  , m_cent_min(9999)
  , m_cent_max(0)
  , m_jet_phi_min(9999)
  , m_jet_phi_max(0)
  , m_jet_eta_min(9999)
  , m_jet_eta_max(0)
  , m_jet_pt_min(9999)
  , m_jet_pt_max(0)
  , m_jet_n_min(9999)
  , m_jet_n_max(0)
{
}

//____________________________________________________________________________..
int JetValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  // init hists
  m_hists["hEvent"] = std::make_unique<TH1F>("hEvent", "Event Type; Type; Events", m_eventType.size(), 0, m_eventType.size());
  m_hists["hVtxZ"] = std::make_unique<TH1F>("hVtxZ", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hVtxZ_MB"] = std::make_unique<TH1F>("hVtxZ_MB", "Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hCentrality"] = std::make_unique<TH1F>("hCentrality", "Centrality; Centrality [%]; Events", m_bins_cent, m_cent_low, m_cent_high);

  m_hists["h3JetPhiEtaPt"] = std::make_unique<TH3F>("h3JetPhiEtaPt", "Jet: |z| < 10 cm and MB; #phi; #eta; p_{T} [GeV]", m_bins_phi, m_phi_low, m_phi_high, m_bins_eta, m_eta_low, m_eta_high, m_bins_pt, m_pt_low, m_pt_high);

  std::stringstream title;
  title << "Jet: |z| < 10 cm and MB; N_{jets} with p_{T} #geq " << m_jet_pt_min_cut << " GeV; Centrality [%]";
  m_hists["h2JetN"] = std::make_unique<TH2F>("h2JetN", title.str().c_str(), m_bins_nJets, m_nJets_low, m_nJets_high, m_bins_cent, m_cent_low, m_cent_high);

  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i + 1, m_eventType[i].c_str());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_event_check(PHCompositeNode *topNode)
{
  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ALL));

  // zvertex
  m_zvtx = -9999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  if (!vertexmap)
  {
    std::cout << "JetValidation::process_event - Error can not find global vertex node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    m_zvtx = vtx->get_z();
  }

  m_hists["hVtxZ"]->Fill(m_zvtx);

  if (fabs(m_zvtx) < m_zvtx_max)
  {
    m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10));
  }

  MinimumBiasInfo *m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
  if (!m_mb_info)
  {
    std::cout << "JetValidation::process_event - Error can not find MinimumBiasInfo node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // skip event if not minimum bias
  if (!m_mb_info->isAuAuMinimumBias())
  {
    ++m_ctr["abort_not_minbias"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hVtxZ_MB"]->Fill(m_zvtx);

  // skip event if zvtx is too large
  if (fabs(m_zvtx) >= m_zvtx_max)
  {
    ++m_ctr["abort_zvtx_high"];
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_hists["hEvent"]->Fill(static_cast<std::uint8_t>(EventType::ZVTX10_MB));
  ++m_ctr["event_zvtx10_mb"];

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_centrality(PHCompositeNode *topNode)
{
  CentralityInfo *centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
  if (!centInfo)
  {
    std::cout << "JetValidation::process_event - Error can not find Centrality Info node " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  m_cent = centInfo->get_centile(CentralityInfo::PROP::mbd_NS)*100;

  if(Verbosity()) {
    std::cout << "Cent: " << m_cent << std::endl;
  }

  m_hists["hCentrality"]->Fill(m_cent);

  JetUtils::update_min_max(m_cent, m_cent_min, m_cent_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_jets(PHCompositeNode *topNode)
{
  // interface to reco jets
  JetContainer* jets = findNode::getClass<JetContainer>(topNode, "AntiKt_Tower_r02_Sub1");
  if (!jets)
  {
    std::cout << "JetValidation::process_event - Error can not find DST Reco JetContainer node." << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int nJets = 0;

  for (auto jet : *jets)
  {
    double phi = jet->get_phi();
    double eta = jet->get_eta();
    double pt = jet->get_pt();

    // exclude jets near the edge of the detector
    if (JetUtils::check_bad_jet_eta(eta, m_zvtx, m_R))
    {
      ++m_ctr["bad_jet_eta"];
      continue;
    }

    // exclude noise jets
    if (pt < m_jet_pt_min_cut) {
      ++m_ctr["bad_jet_pt"];
      continue;
    }

    dynamic_cast<TH3 *>(m_hists["h3JetPhiEtaPt"].get())->Fill(phi, eta, pt);
    JetUtils::update_min_max(phi, m_jet_phi_min, m_jet_phi_max);
    JetUtils::update_min_max(eta, m_jet_eta_min, m_jet_eta_max);
    JetUtils::update_min_max(pt, m_jet_pt_min, m_jet_pt_max);
    ++m_ctr["ctr_jets"];
    ++nJets;
  }

  dynamic_cast<TH2 *>(m_hists["h2JetN"].get())->Fill(nJets, m_cent);

  JetUtils::update_min_max(nJets, m_jet_n_min, m_jet_n_max);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::process_event(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  if (!eventInfo)
  {
    std::cout << PHWHERE << "JetValidation::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  int m_globalEvent = eventInfo->get_EvtSequence();
  // int m_run         = eventInfo->get_RunNumber();

  if (m_event % 20 == 0)
  {
    std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
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

  ret = process_jets(topNode);
  if (ret)
  {
    return ret;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "JetValidation::End" << std::endl;

  std::cout << "=====================" << std::endl;
  std::cout << "stats" << std::endl;
  std::cout << "Cent: Min: " << m_cent_min << ", Max: " << m_cent_max << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "Abort Events Types" << std::endl;
  std::cout << "No Min Bias: " << m_ctr["abort_not_minbias"] << std::endl;
  std::cout << "|z| > 10 cm: " << m_ctr["abort_zvtx_high"] << std::endl;
  std::cout << "=====================" << std::endl;
  std::cout << "Events" << std::endl;
  for (unsigned int i = 0; i < m_eventType.size(); ++i)
  {
    std::cout << m_eventType[i] << ": " << m_hists["hEvent"]->GetBinContent(i + 1) << std::endl;
  }
  std::cout << "=====================" << std::endl;
  std::cout << "Jet" << std::endl;
  std::cout << "Avg Bad Jets per Event: " << m_ctr["bad_jet_eta"] * 1./ m_ctr["event_zvtx10_mb"] << std::endl;
  std::cout << "Avg Low Jet Pt per Event: " << m_ctr["bad_jet_pt"] * 1./ m_ctr["event_zvtx10_mb"] << std::endl;
  std::cout << "Avg Jet per Event: " << m_ctr["ctr_jets"] * 1./ m_ctr["event_zvtx10_mb"] << std::endl;
  std::cout << "Number of jets per event: Min: " << m_jet_n_min << ", Max: " << m_jet_n_max << std::endl;
  std::cout << "Phi: Min: " << m_jet_phi_min << ", Max: " << m_jet_phi_max << std::endl;
  std::cout << "Eta: Min: " << m_jet_eta_min << ", Max: " << m_jet_eta_max << std::endl;
  std::cout << "Pt: Min: " << m_jet_pt_min << ", Max: " << m_jet_pt_max << std::endl;
  std::cout << "=====================" << std::endl;

  TFile output(m_outfile_name.c_str(), "recreate");
  output.cd();

  for (const auto &[name, hist] : m_hists)
  {
    hist->Write();
  }

  // local
  dynamic_cast<TH3 *>(m_hists["h3JetPhiEtaPt"].get())->Project3D("yx")->Write();
  dynamic_cast<TH3 *>(m_hists["h3JetPhiEtaPt"].get())->Project3D("z")->Write();

  output.Close();

  return Fun4AllReturnCodes::EVENT_OK;
}
