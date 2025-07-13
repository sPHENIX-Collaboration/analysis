#include "sEPDValidation.h"

// -- c++
#include <iostream>
#include <sstream>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// -- Calo
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoDefs.h>

// -- Vtx
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/GlobalVertex.h>

// -- MB
#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

// -- Trigger
#include <ffarawobjects/Gl1Packet.h>
#include <calotrigger/TriggerRunInfo.h>

// -- Event Plane
#include <eventplaneinfo/EventplaneinfoMap.h>

// -- MBD
#include <mbd/MbdGeom.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <TFile.h>
#include <TH2.h>
#include <TH3.h>

//____________________________________________________________________________..
sEPDValidation::sEPDValidation(const std::string &name)
 : SubsysReco(name)
 , m_event(0)
 , m_bins_zvtx(200) /*cm*/
 , m_zvtx_low(-50) /*cm*/
 , m_zvtx_high(50) /*cm*/
 , m_bins_centbin(100)
 , m_centbin_low(-0.5)
 , m_centbin_high(99.5)
 , m_bins_sepd_q(150)
 , m_sepd_q_low(0)
 , m_sepd_q_high(1.5e4)
 , m_bins_psi(64)
 , m_psi_low(-1.6)
 , m_psi_high(1.6)
 , m_bins_Q(100)
 , m_Q_low(0)
 , m_Q_high(200)
 , m_zvtx(9999)
 , m_centbin(9999)
 , m_zvtx_max(10) /*cm*/
 , m_centbin_min(9999)
 , m_centbin_max(0)
 , m_cent_min(9999)
 , m_cent_max(0)
 , m_sepd_q_south_min(9999)
 , m_sepd_q_south_max(0)
 , m_sepd_q_north_min(9999)
 , m_sepd_q_north_max(0)
 , m_psi_min(9999)
 , m_psi_max(0)
 , m_Q_min(9999)
 , m_Q_max(0)
{}

//____________________________________________________________________________..
int sEPDValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  // init hists
  m_hists["hEvent"]      = std::make_unique<TH1F>("hEvent","Event Type; Type; Events", m_eventType.size(), 0, m_eventType.size());
  m_hists["hVtxZ"]       = std::make_unique<TH1F>("hVtxZ","Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hVtxZ_MB"]    = std::make_unique<TH1F>("hVtxZ_MB","Z Vertex; z [cm]; Events", m_bins_zvtx, m_zvtx_low, m_zvtx_high);
  m_hists["hCentrality"] = std::make_unique<TH1F>("hCentrality","Centrality; Centrality [%]; Events", m_bins_centbin, m_centbin_low, m_centbin_high);
  m_hists["h3SEPD_Charge"] = std::make_unique<TH3F>("h3SEPD_Charge","sEPD Total Charge: |z| < 10 cm and MB; South; North; Centrality [%]", m_bins_sepd_q, m_sepd_q_low, m_sepd_q_high, m_bins_sepd_q, m_sepd_q_low, m_sepd_q_high, m_bins_centbin, m_centbin_low, m_centbin_high);

  // Order 2
  m_hists["h3SEPD_North_Psi2_Q_cent"] = std::make_unique<TH3F>("h2SEPD_North_Psi2_Q_cent","sEPD North: |z| < 10 cm and MB; #Psi^{N}_{2}; |Q^{N}_{2}|; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_Q, m_Q_low, m_Q_high, m_bins_centbin, m_centbin_low, m_centbin_high);
  m_hists["h3SEPD_South_Psi2_Q_cent"] = std::make_unique<TH3F>("h2SEPD_South_Psi2_Q_cent","sEPD South: |z| < 10 cm and MB; #Psi^{N}_{2}; |Q^{N}_{2}|; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_Q, m_Q_low, m_Q_high, m_bins_centbin, m_centbin_low, m_centbin_high);

  // Order 3
  m_hists["h3SEPD_North_Psi3_Q_cent"] = std::make_unique<TH3F>("h2SEPD_North_Psi3_Q_cent","sEPD North: |z| < 10 cm and MB; #Psi^{N}_{3}; |Q^{N}_{3}|; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_Q, m_Q_low, m_Q_high, m_bins_centbin, m_centbin_low, m_centbin_high);
  m_hists["h3SEPD_South_Psi3_Q_cent"] = std::make_unique<TH3F>("h2SEPD_South_Psi3_Q_cent","sEPD South: |z| < 10 cm and MB; #Psi^{N}_{3}; |Q^{N}_{3}|; Centrality [%]", m_bins_psi, m_psi_low, m_psi_high, m_bins_Q, m_Q_low, m_Q_high, m_bins_centbin, m_centbin_low, m_centbin_high);

  for(UInt_t i = 0; i < m_eventType.size(); ++i) {
    m_hists["hEvent"]->GetXaxis()->SetBinLabel(i+1, m_eventType[i].c_str());
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_event_check(PHCompositeNode *topNode) {
    m_hists["hEvent"]->Fill(m_event_type::ALL);

    TriggerRunInfo* triggerruninfo = findNode::getClass<TriggerRunInfo>(topNode, "TriggerRunInfo");

    if (!triggerruninfo) {
        std::cout << "sEPDValidation::process_event - Error can not find TriggerRunInfo node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    if(m_event == 1) {
        triggerruninfo->identify();
    }

    // zvertex
    m_zvtx = -9999;
    GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

    if (!vertexmap) {
        std::cout << "sEPDValidation::process_event - Error can not find global vertex node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    if(!vertexmap->empty()) {
        GlobalVertex* vtx = vertexmap->begin()->second;
        m_zvtx = vtx->get_z();
    }

    m_hists["hVtxZ"]->Fill(m_zvtx);

    if(fabs(m_zvtx) < m_zvtx_max) {
        m_hists["hEvent"]->Fill(m_event_type::ZVTX10);
    }

    MinimumBiasInfo* m_mb_info = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
    if (!m_mb_info)
    {
        std::cout << "sEPDValidation::process_event - Error can not find MinimumBiasInfo node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    // skip event if not minimum bias
    if (!m_mb_info->isAuAuMinimumBias())
    {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    m_hists["hVtxZ_MB"]->Fill(m_zvtx);

    // skip event if zvtx is too large
    if(fabs(m_zvtx) >= m_zvtx_max) {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    m_hists["hEvent"]->Fill(m_event_type::ZVTX10_MB);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_centrality(PHCompositeNode *topNode) {

    m_centbin = -9999;
    Double_t m_cent = -9999;
    CentralityInfo* centInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
    if (!centInfo) {
        std::cout << "sEPDValidation::process_event - Error can not find Centrality Info node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    m_centbin = centInfo->get_centrality_bin(CentralityInfo::PROP::mbd_NS);
    m_cent    = centInfo->get_centile(CentralityInfo::PROP::mbd_NS);

    m_hists["hCentrality"]->Fill(m_centbin);

    m_cent_min = std::min(m_cent_min, m_cent);
    m_cent_max = std::max(m_cent_max, m_cent);

    m_centbin_min = std::min(m_centbin_min, m_centbin);
    m_centbin_max = std::max(m_centbin_max, m_centbin);

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_sEPD(PHCompositeNode *topNode) {
    TowerInfoContainer* towerinfosEPD = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_SEPD");
    if(!towerinfosEPD)
    {
        std::cout << "sEPDValidation::process_event - Error can not find TOWERINFO_CALIB_SEPD node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    //sepd
    UInt_t nchannels_epd = towerinfosEPD->size();

    Double_t sepd_q_south = 0;
    Double_t sepd_q_north = 0;

    for (UInt_t channel = 0; channel < nchannels_epd; ++channel)
    {
        Double_t charge = towerinfosEPD->get_tower_at_channel(channel)->get_energy();
        UInt_t key = TowerInfoDefs::encode_epd(channel);

        //sepd charge sums
        UInt_t arm = TowerInfoDefs::get_epd_arm(key);
        if (arm == 0)
        {
            sepd_q_south += charge;
        }
        else if (arm == 1)
        {
            sepd_q_north += charge;
        }
    }

    dynamic_cast<TH3*>(m_hists["h3SEPD_Charge"].get())->Fill(sepd_q_south, sepd_q_north, m_centbin);

    m_sepd_q_south_min = std::min(m_sepd_q_south_min, sepd_q_south);
    m_sepd_q_south_max = std::max(m_sepd_q_south_max, sepd_q_south);

    m_sepd_q_north_min = std::min(m_sepd_q_north_min, sepd_q_north);
    m_sepd_q_north_max = std::max(m_sepd_q_north_max, sepd_q_north);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_EventPlane(Eventplaneinfo *_EPDS, Eventplaneinfo *_EPDN, Int_t order) {

    std::stringstream suffix;
    suffix << "Psi" << order << "_Q_cent";

    std::pair<Double_t, Double_t> epdsouthQn = _EPDS->get_qvector(order);
    std::pair<Double_t, Double_t> epdnorthQn = _EPDN->get_qvector(order);

    Double_t Qx_south = epdsouthQn.first;
    Double_t Qy_south = epdsouthQn.second;

    Double_t Qx_north = epdnorthQn.first;
    Double_t Qy_north = epdnorthQn.second;

    // Compute Q vector magnitude
    Double_t Q_south = std::sqrt(Qx_south*Qx_south + Qy_south*Qy_south);
    Double_t Q_north = std::sqrt(Qx_north*Qx_north + Qy_north*Qy_north);

    // ensure the magnitude of both Q vectors are non-zero
    if(Q_south == 0 || Q_north == 0) {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    m_Q_min = std::min(m_Q_min, std::min(Q_south, Q_north));
    m_Q_max = std::max(m_Q_max, std::max(Q_south, Q_north));

    Double_t psi_n_south = _EPDS->GetPsi(Qx_south, Qy_south, order);
    Double_t psi_n_north = _EPDN->GetPsi(Qx_north, Qy_north, order);

    m_psi_min = std::min(m_psi_min, std::min(psi_n_south, psi_n_north));
    m_psi_max = std::max(m_psi_max, std::max(psi_n_south, psi_n_north));

    dynamic_cast<TH3*>(m_hists["h3SEPD_North_"+suffix.str()].get())->Fill(psi_n_north, Q_north, m_centbin);
    dynamic_cast<TH3*>(m_hists["h3SEPD_South_"+suffix.str()].get())->Fill(psi_n_south, Q_south, m_centbin);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_EventPlane(PHCompositeNode *topNode) {
    //get event plane map
    EventplaneinfoMap* epmap = findNode::getClass<EventplaneinfoMap>(topNode, "EventplaneinfoMap");
    if (!epmap)
    {
        std::cout << "sEPDValidation::process_event - Error can not find EventplaneinfoMap node " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }
    if (epmap->empty())
    {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    Eventplaneinfo* _EPDS = epmap->get(EventplaneinfoMap::sEPDS);
    Eventplaneinfo* _EPDN = epmap->get(EventplaneinfoMap::sEPDN);

    // ensure the ptrs are valid
    if(!_EPDS || !_EPDN) {
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    // process order 2
    Int_t ret = process_EventPlane(_EPDS, _EPDN, 2);
    if(ret) return ret;

    // process order 3
    ret = process_EventPlane(_EPDS, _EPDN, 3);
    if(ret) return ret;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::process_event(PHCompositeNode *topNode)
{

    EventHeader* eventInfo = findNode::getClass<EventHeader>(topNode,"EventHeader");
    if(!eventInfo)
    {
        std::cout << PHWHERE << "sEPDValidation::process_event - Fatal Error - EventHeader node is missing. " << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
    }

    Int_t m_globalEvent = eventInfo->get_EvtSequence();
    // Int_t m_run         = eventInfo->get_RunNumber();

    if (m_event % 20 == 0) {
        std::cout << "Progress: " << m_event << ", Global: " << m_globalEvent << std::endl;
    }
    ++m_event;

    Int_t ret = process_event_check(topNode);
    if(ret) return ret;

    ret = process_centrality(topNode);
    if(ret) return ret;

    ret = process_sEPD(topNode);
    if(ret) return ret;

    ret = process_EventPlane(topNode);
    if(ret) return ret;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int sEPDValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
    std::cout << "sEPDValidation::End" << std::endl;

    std::cout << "=====================" << std::endl;
    std::cout << "stats" << std::endl;
    std::cout << "Cent: Min: " << m_cent_min << ", Max: " << m_cent_max << std::endl;
    std::cout << "Centbin: Min: " << m_centbin_min << ", Max: " << m_centbin_max << std::endl;
    std::cout << "sEPD Total Charge South: Min: " << m_sepd_q_south_min << ", Max: " << m_sepd_q_south_max << std::endl;
    std::cout << "sEPD Total Charge North: Min " << m_sepd_q_north_min << ", Max: " << m_sepd_q_north_max << std::endl;
    std::cout << "Psi: Min " << m_psi_min << ", Max: " << m_psi_max << std::endl;
    std::cout << "Q: Min " << m_Q_min << ", Max: " << m_Q_max << std::endl;
    std::cout << "Events" << std::endl;
    for(UInt_t i = 0; i < m_eventType.size(); ++i) {
        std::cout << m_eventType[i] << ": " << m_hists["hEvent"]->GetBinContent(i+1) << std::endl;
    }
    std::cout << "=====================" << std::endl;

    TFile output(m_outfile_name.c_str(),"recreate");
    output.cd();

    for (const auto& [name, hist] : m_hists) {
        hist->Write();
    }

    dynamic_cast<TH3*>(m_hists["h3SEPD_Charge"].get())->Project3D("xy")->Write();

    // Order 2
    dynamic_cast<TH3*>(m_hists["h3SEPD_North_Psi2_Q_cent"].get())->Project3D("x")->Write();
    dynamic_cast<TH3*>(m_hists["h3SEPD_South_Psi2_Q_cent"].get())->Project3D("x")->Write();

    // Order 3
    dynamic_cast<TH3*>(m_hists["h3SEPD_North_Psi3_Q_cent"].get())->Project3D("x")->Write();
    dynamic_cast<TH3*>(m_hists["h3SEPD_South_Psi3_Q_cent"].get())->Project3D("x")->Write();

    output.Close();

    return Fun4AllReturnCodes::EVENT_OK;

}
