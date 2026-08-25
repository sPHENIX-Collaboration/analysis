#include "JetValidationv3.h"

// -- c++
#include <format>
#include <iostream>

// -- event
#include <ffaobjects/EventHeader.h>

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Nodes
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

// -- calo
#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

// -- jet
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>

// -- jetbackground
#include <jetbackground/TowerBackground.h>

#include <treefiller/TreeFiller.h>

#include "JetUtils.h"

// -- vertex
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// -- ROOT
#include <TH1.h>
#include <TH2.h>
#include <TProfile2D.h>
#include <TTree.h>

//____________________________________________________________________________..
JetValidationv3::JetValidationv3(const std::string &name)
  : SubsysReco(name)
{
}

void JetValidationv3::set_jet_radii(const std::vector<float>& radii)
{
  m_do_r02 = false;
  m_do_r03 = false;
  for (float r : radii)
  {
    if (std::abs(r - 0.2f) < 1e-3f) m_do_r02 = true;
    if (std::abs(r - 0.3f) < 1e-3f) m_do_r03 = true;
  }
}

void JetValidationv3::set_jet_radii(const std::vector<double>& radii)
{
  m_do_r02 = false;
  m_do_r03 = false;
  for (double r : radii)
  {
    if (std::abs(r - 0.2) < 1e-3) m_do_r02 = true;
    if (std::abs(r - 0.3) < 1e-3) m_do_r03 = true;
  }
}

void JetValidationv3::set_jet_radii(std::initializer_list<double> radii)
{
  set_jet_radii(std::vector<double>(radii));
}

std::vector<float> JetValidationv3::get_jet_radii() const
{
  std::vector<float> r;
  if (m_do_r02)
  {
    r.push_back(0.2F);
  }
  if (m_do_r03)
  {
    r.push_back(0.3F);
  }
  return r;
}

//____________________________________________________________________________..
int JetValidationv3::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Print("NODETREE");

  TTree* tree = TreeFiller::getTree();
  if (!tree)
  {
    std::cout << "JetValidationv3: Failed to get/create TTree" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (m_do_iter)
  {
    tree->Branch("seeds_iter", &m_data.seeds_iter);
    tree->Branch("calo_v2_iter", &m_data.calo_v2_iter);
    tree->Branch("is_flow_failure_iter", &m_data.is_flow_failure_iter);

    if (m_do_r02)
    {
      tree->Branch("max_pt_iter_r02", &m_data.iter_r02.max_pt);
      tree->Branch("pt_iter_r02", &m_data.iter_r02.pt);
      tree->Branch("pt_calib_iter_r02", &m_data.iter_r02.pt_calib);
      tree->Branch("e_iter_r02", &m_data.iter_r02.e);
      tree->Branch("phi_iter_r02", &m_data.iter_r02.phi);
      tree->Branch("eta_iter_r02", &m_data.iter_r02.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_iter_r02", &m_data.iter_r02.emcal_tower_index);
        tree->Branch("emcal_tower_energy_iter_r02", &m_data.iter_r02.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_iter_r02", &m_data.iter_r02.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_iter_r02", &m_data.iter_r02.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_iter_r02", &m_data.iter_r02.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_iter_r02", &m_data.iter_r02.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_iter_r02", &m_data.iter_r02.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_iter_r02", &m_data.iter_r02.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_iter_r02", &m_data.iter_r02.ohcal_tower_pt);
      }
    }

    if (m_do_r03)
    {
      tree->Branch("max_pt_iter_r03", &m_data.iter_r03.max_pt);
      tree->Branch("pt_iter_r03", &m_data.iter_r03.pt);
      tree->Branch("pt_calib_iter_r03", &m_data.iter_r03.pt_calib);
      tree->Branch("e_iter_r03", &m_data.iter_r03.e);
      tree->Branch("phi_iter_r03", &m_data.iter_r03.phi);
      tree->Branch("eta_iter_r03", &m_data.iter_r03.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_iter_r03", &m_data.iter_r03.emcal_tower_index);
        tree->Branch("emcal_tower_energy_iter_r03", &m_data.iter_r03.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_iter_r03", &m_data.iter_r03.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_iter_r03", &m_data.iter_r03.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_iter_r03", &m_data.iter_r03.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_iter_r03", &m_data.iter_r03.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_iter_r03", &m_data.iter_r03.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_iter_r03", &m_data.iter_r03.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_iter_r03", &m_data.iter_r03.ohcal_tower_pt);
      }
    }
  }

  if (m_do_mult)
  {
    tree->Branch("seeds_mult", &m_data.seeds_mult);
    tree->Branch("calo_v2_mult", &m_data.calo_v2_mult);
    tree->Branch("is_flow_failure_mult", &m_data.is_flow_failure_mult);

    if (m_do_r02)
    {
      tree->Branch("max_pt_mult_r02", &m_data.mult_r02.max_pt);
      tree->Branch("pt_mult_r02", &m_data.mult_r02.pt);
      tree->Branch("pt_calib_mult_r02", &m_data.mult_r02.pt_calib);
      tree->Branch("e_mult_r02", &m_data.mult_r02.e);
      tree->Branch("phi_mult_r02", &m_data.mult_r02.phi);
      tree->Branch("eta_mult_r02", &m_data.mult_r02.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_mult_r02", &m_data.mult_r02.emcal_tower_index);
        tree->Branch("emcal_tower_energy_mult_r02", &m_data.mult_r02.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_mult_r02", &m_data.mult_r02.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_mult_r02", &m_data.mult_r02.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_mult_r02", &m_data.mult_r02.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_mult_r02", &m_data.mult_r02.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_mult_r02", &m_data.mult_r02.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_mult_r02", &m_data.mult_r02.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_mult_r02", &m_data.mult_r02.ohcal_tower_pt);
      }
    }

    if (m_do_r03)
    {
      tree->Branch("max_pt_mult_r03", &m_data.mult_r03.max_pt);
      tree->Branch("pt_mult_r03", &m_data.mult_r03.pt);
      tree->Branch("pt_calib_mult_r03", &m_data.mult_r03.pt_calib);
      tree->Branch("e_mult_r03", &m_data.mult_r03.e);
      tree->Branch("phi_mult_r03", &m_data.mult_r03.phi);
      tree->Branch("eta_mult_r03", &m_data.mult_r03.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_mult_r03", &m_data.mult_r03.emcal_tower_index);
        tree->Branch("emcal_tower_energy_mult_r03", &m_data.mult_r03.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_mult_r03", &m_data.mult_r03.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_mult_r03", &m_data.mult_r03.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_mult_r03", &m_data.mult_r03.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_mult_r03", &m_data.mult_r03.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_mult_r03", &m_data.mult_r03.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_mult_r03", &m_data.mult_r03.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_mult_r03", &m_data.mult_r03.ohcal_tower_pt);
      }
    }
  }

  if (m_do_unsub)
  {
    if (m_do_r02)
    {
      tree->Branch("max_pt_unsub_r02", &m_data.unsub_r02.max_pt);
      tree->Branch("pt_unsub_r02", &m_data.unsub_r02.pt);
      tree->Branch("pt_calib_unsub_r02", &m_data.unsub_r02.pt_calib);
      tree->Branch("e_unsub_r02", &m_data.unsub_r02.e);
      tree->Branch("phi_unsub_r02", &m_data.unsub_r02.phi);
      tree->Branch("eta_unsub_r02", &m_data.unsub_r02.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_unsub_r02", &m_data.unsub_r02.emcal_tower_index);
        tree->Branch("emcal_tower_energy_unsub_r02", &m_data.unsub_r02.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_unsub_r02", &m_data.unsub_r02.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_unsub_r02", &m_data.unsub_r02.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_unsub_r02", &m_data.unsub_r02.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_unsub_r02", &m_data.unsub_r02.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_unsub_r02", &m_data.unsub_r02.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_unsub_r02", &m_data.unsub_r02.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_unsub_r02", &m_data.unsub_r02.ohcal_tower_pt);
      }
    }

    if (m_do_r03)
    {
      tree->Branch("max_pt_unsub_r03", &m_data.unsub_r03.max_pt);
      tree->Branch("pt_unsub_r03", &m_data.unsub_r03.pt);
      tree->Branch("pt_calib_unsub_r03", &m_data.unsub_r03.pt_calib);
      tree->Branch("e_unsub_r03", &m_data.unsub_r03.e);
      tree->Branch("phi_unsub_r03", &m_data.unsub_r03.phi);
      tree->Branch("eta_unsub_r03", &m_data.unsub_r03.eta);

      if (m_do_detailed)
      {
        tree->Branch("emcal_tower_index_unsub_r03", &m_data.unsub_r03.emcal_tower_index);
        tree->Branch("emcal_tower_energy_unsub_r03", &m_data.unsub_r03.emcal_tower_energy);
        tree->Branch("emcal_tower_pt_unsub_r03", &m_data.unsub_r03.emcal_tower_pt);

        tree->Branch("ihcal_tower_index_unsub_r03", &m_data.unsub_r03.ihcal_tower_index);
        tree->Branch("ihcal_tower_energy_unsub_r03", &m_data.unsub_r03.ihcal_tower_energy);
        tree->Branch("ihcal_tower_pt_unsub_r03", &m_data.unsub_r03.ihcal_tower_pt);

        tree->Branch("ohcal_tower_index_unsub_r03", &m_data.unsub_r03.ohcal_tower_index);
        tree->Branch("ohcal_tower_energy_unsub_r03", &m_data.unsub_r03.ohcal_tower_energy);
        tree->Branch("ohcal_tower_pt_unsub_r03", &m_data.unsub_r03.ohcal_tower_pt);
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::InitRun(PHCompositeNode *topNode)
{
  m_geom_cemc.fill(TowerGeomInfo{});
  m_geom_hcalin.fill(TowerGeomInfo{});
  m_geom_hcalout.fill(TowerGeomInfo{});

  RawTowerGeomContainer* geom_hcalin = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer* geom_hcalout = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if (geom_hcalin)
  {
    for (unsigned int ieta = 0; ieta < CaloGeometry::HCAL_ETA_BINS; ++ieta)
    {
      for (unsigned int iphi = 0; iphi < CaloGeometry::HCAL_PHI_BINS; ++iphi)
      {
        RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
        RawTowerGeom *geom = geom_hcalin->get_tower_geometry(key);
        if (geom)
        {
          double eta = geom->get_eta();
          double phi = geom->get_phi();
          double sinh_eta = std::sinh(eta);
          size_t idx = static_cast<size_t>(ieta) * CaloGeometry::HCAL_PHI_BINS + iphi;

          m_geom_hcalin[idx] = {eta, phi, sinh_eta * m_r_hcalin, true};
          m_geom_cemc[idx] = {eta, phi, sinh_eta * m_r_cemc, true};
        }
      }
    }
  }

  if (geom_hcalout)
  {
    for (unsigned int ieta = 0; ieta < CaloGeometry::HCAL_ETA_BINS; ++ieta)
    {
      for (unsigned int iphi = 0; iphi < CaloGeometry::HCAL_PHI_BINS; ++iphi)
      {
        RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
        RawTowerGeom *geom = geom_hcalout->get_tower_geometry(key);
        if (geom)
        {
          double eta = geom->get_eta();
          double phi = geom->get_phi();
          double sinh_eta = std::sinh(eta);
          size_t idx = static_cast<size_t>(ieta) * CaloGeometry::HCAL_PHI_BINS + iphi;

          m_geom_hcalout[idx] = {eta, phi, sinh_eta * m_r_hcalout, true};
        }
      }
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_UE(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : -1;

  if (m_do_iter)
  {
    TowerBackground* towerBkg_iter = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
    if (!towerBkg_iter)
    {
      std::cout << "Aborting Run: Iter Background Info null" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

    m_data.is_flow_failure_iter = towerBkg_iter->get_flow_failure_flag();
    m_data.calo_v2_iter = towerBkg_iter->get_v2();
    m_data.seeds_iter = towerBkg_iter->get_nHIRecoSeedsSub();

    if (Verbosity() > 0)
    {
      std::cout << "JetValidationv3::process_UE - [Event " << event_id << "] [Iter] seeds: " << m_data.seeds_iter
                << " | v2: " << m_data.calo_v2_iter
                << " | flow_fail: " << m_data.is_flow_failure_iter << std::endl;
    }
  }

  if (m_do_mult)
  {
    TowerBackground* towerBkg_mult = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_MultSub2");
    if (!towerBkg_mult)
    {
      std::cout << "Aborting Run: Mult Background Info null" << std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

    m_data.is_flow_failure_mult = towerBkg_mult->get_flow_failure_flag();
    m_data.calo_v2_mult = towerBkg_mult->get_v2();
    m_data.seeds_mult = towerBkg_mult->get_nHIRecoSeedsSub();

    if (Verbosity() > 0)
    {
      std::cout << "JetValidationv3::process_UE - [Event " << event_id << "] [Mult] seeds: " << m_data.seeds_mult
                << " | v2: " << m_data.calo_v2_mult
                << " | flow_fail: " << m_data.is_flow_failure_mult << std::endl;
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_jets(PHCompositeNode *topNode)
{
  EventHeader *eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : -1;

  double zvtx = 0.0;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    std::cout << "JetValidationv3: GlobalVertexMap null" << std::endl;
  }
  else if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    if (vtx)
    {
      zvtx = vtx->get_z();
    }
  }

  auto fill_jets = [&](JetContainer* jets, JetContainer* jets_calib, JetData& jd, double jet_radius,
                       TowerInfoContainer* cemc_towers, TowerInfoContainer* ihcal_towers, TowerInfoContainer* ohcal_towers,
                       const std::string& jet_label = "")
  {
    if (Verbosity() > 1)
    {
      std::cout << "JetValidationv3::process_jets - [Event " << event_id << "] [" << jet_label << "] (R=" << jet_radius
                << ") Container sizes: raw=" << jets->size() << ", calib=" << jets_calib->size() << std::endl;
    }

    for (unsigned int i = 0; i < std::min(jets->size(), jets_calib->size()); ++i)
    {
      auto *jet = jets->get_jet(i);
      auto *jet_calib = jets_calib->get_jet(i);

      if (!jet || !jet_calib)
      {
        continue;
      }

      double pt = jet->get_pt();
      double pt_calib = jet_calib->get_pt();
      double energy = jet->get_e();
      double phi = jet->get_phi();
      double eta = jet->get_eta();

      // map [-pi,pi] -> [0,2pi]
      if (phi < 0)
      {
        phi += 2.0 * std::numbers::pi;
      }

      if (pt_calib >= m_jet_pt_min_cut && !JetUtils::check_bad_jet_eta(eta, zvtx, jet_radius))
      {
        jd.pt.push_back(pt);
        jd.pt_calib.push_back(pt_calib);
        jd.e.push_back(energy);
        jd.phi.push_back(phi);
        jd.eta.push_back(eta);

        if (energy > 0)
        {
          jd.max_pt = std::max(jd.max_pt, pt_calib);
        }

        if (Verbosity() > 0)
        {
          std::cout << "JetValidationv3::process_jets - [Event " << event_id << "] [" << jet_label << "] Jet #" << i
                    << " | pT: " << pt << " -> pT_calib: " << pt_calib << " GeV"
                    << " | E: " << energy << " GeV"
                    << " | eta: " << eta
                    << " | phi: " << phi
                    << " | zvtx: " << zvtx << " cm"
                    << std::endl;
        }

        if (m_do_detailed)
        {
          std::vector<int> emcal_idx;
          std::vector<double> emcal_e;
          std::vector<double> emcal_pt;

          std::vector<int> ihcal_idx;
          std::vector<double> ihcal_e;
          std::vector<double> ihcal_pt;

          std::vector<int> ohcal_idx;
          std::vector<double> ohcal_e;
          std::vector<double> ohcal_pt;

          for (const auto& comp : jet->get_comp_vec())
          {
            Jet::SRC src = comp.first;
            unsigned int channel = comp.second;

            TowerInfoContainer* towers = nullptr;
            const TowerGeomArray* geom_map = nullptr;
            double detector_radius = 0.0;

            if (src == Jet::SRC::CEMC_TOWERINFO_RETOWER || src == Jet::SRC::CEMC_TOWERINFO_SUB1)
            {
              towers = cemc_towers;
              geom_map = &m_geom_cemc;
              detector_radius = m_r_cemc;
            }
            else if (src == Jet::SRC::HCALIN_TOWERINFO || src == Jet::SRC::HCALIN_TOWERINFO_SUB1)
            {
              towers = ihcal_towers;
              geom_map = &m_geom_hcalin;
              detector_radius = m_r_hcalin;
            }
            else if (src == Jet::SRC::HCALOUT_TOWERINFO || src == Jet::SRC::HCALOUT_TOWERINFO_SUB1)
            {
              towers = ohcal_towers;
              geom_map = &m_geom_hcalout;
              detector_radius = m_r_hcalout;
            }

            if (!towers || !geom_map)
            {
              continue;
            }

            if (channel >= towers->size())
            {
              continue;
            }

            TowerInfo *tower = towers->get_tower_at_channel(channel);
            if (!tower)
            {
              continue;
            }

            unsigned int channelkey = towers->encode_key(channel);
            unsigned int ieta = towers->getTowerEtaBin(channelkey);
            unsigned int iphi = towers->getTowerPhiBin(channelkey);

            if (ieta >= CaloGeometry::HCAL_ETA_BINS || iphi >= CaloGeometry::HCAL_PHI_BINS)
            {
              continue;
            }

            size_t idx = static_cast<size_t>(ieta) * CaloGeometry::HCAL_PHI_BINS + iphi;
            const auto &geom = (*geom_map)[idx];
            if (!geom.is_valid)
            {
              continue;
            }

            double eta_corrected = geom.eta;
            if (zvtx != 0.0 && detector_radius > 0.0)
            {
              double z = geom.z0 - zvtx;
              eta_corrected = std::asinh(z / detector_radius);
            }

            double tower_energy = tower->get_energy();
            double tower_pt = tower_energy / std::cosh(eta_corrected);

            if (src == Jet::SRC::CEMC_TOWERINFO_RETOWER || src == Jet::SRC::CEMC_TOWERINFO_SUB1)
            {
              emcal_idx.push_back(static_cast<int>(channel));
              emcal_e.push_back(tower_energy);
              emcal_pt.push_back(tower_pt);
            }
            else if (src == Jet::SRC::HCALIN_TOWERINFO || src == Jet::SRC::HCALIN_TOWERINFO_SUB1)
            {
              ihcal_idx.push_back(static_cast<int>(channel));
              ihcal_e.push_back(tower_energy);
              ihcal_pt.push_back(tower_pt);
            }
            else if (src == Jet::SRC::HCALOUT_TOWERINFO || src == Jet::SRC::HCALOUT_TOWERINFO_SUB1)
            {
              ohcal_idx.push_back(static_cast<int>(channel));
              ohcal_e.push_back(tower_energy);
              ohcal_pt.push_back(tower_pt);
            }
          }

          if (Verbosity() > 1)
          {
            std::cout << "    Constituents -> EMCAL: " << emcal_idx.size()
                      << " towers | IHCAL: " << ihcal_idx.size()
                      << " towers | OHCAL: " << ohcal_idx.size()
                      << " towers" << std::endl;
          }

          jd.emcal_tower_index.push_back(std::move(emcal_idx));
          jd.emcal_tower_energy.push_back(std::move(emcal_e));
          jd.emcal_tower_pt.push_back(std::move(emcal_pt));

          jd.ihcal_tower_index.push_back(std::move(ihcal_idx));
          jd.ihcal_tower_energy.push_back(std::move(ihcal_e));
          jd.ihcal_tower_pt.push_back(std::move(ihcal_pt));

          jd.ohcal_tower_index.push_back(std::move(ohcal_idx));
          jd.ohcal_tower_energy.push_back(std::move(ohcal_e));
          jd.ohcal_tower_pt.push_back(std::move(ohcal_pt));
        }
      }
    }

    if (Verbosity() > 0 && !jd.pt.empty())
    {
      std::cout << "JetValidationv3::process_jets - [Event " << event_id << "] [" << jet_label << "] Total accepted jets: "
                << jd.pt.size() << " / " << std::min(jets->size(), jets_calib->size())
                << " (max pT_calib = " << jd.max_pt << " GeV/c)" << std::endl;
    }
  };

  if (m_do_iter)
  {
    TowerInfoContainer* cemc_sub1 = nullptr;
    TowerInfoContainer* ihcal_sub1 = nullptr;
    TowerInfoContainer* ohcal_sub1 = nullptr;
    if (m_do_detailed)
    {
      cemc_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
      ihcal_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
      ohcal_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
    }

    if (m_do_r02)
    {
      JetContainer *jets_iter_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_r02);
      JetContainer *jets_iter_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_calib_r02);
      if (!jets_iter_r02 || !jets_iter_calib_r02)
      {
        std::cout << "Aborting Run: Iter R=0.2 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_iter_r02, jets_iter_calib_r02, m_data.iter_r02, 0.2, cemc_sub1, ihcal_sub1, ohcal_sub1, "Iter R=0.2");
    }

    if (m_do_r03)
    {
      JetContainer *jets_iter_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_r03);
      JetContainer *jets_iter_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_iter_calib_r03);
      if (!jets_iter_r03 || !jets_iter_calib_r03)
      {
        std::cout << "Aborting Run: Iter R=0.3 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_iter_r03, jets_iter_calib_r03, m_data.iter_r03, 0.3, cemc_sub1, ihcal_sub1, ohcal_sub1, "Iter R=0.3");
    }
  }

  if (m_do_mult)
  {
    TowerInfoContainer* cemc_mult = nullptr;
    TowerInfoContainer* ihcal_mult = nullptr;
    TowerInfoContainer* ohcal_mult = nullptr;
    if (m_do_detailed)
    {
      cemc_mult = findNode::getClass<TowerInfoContainer>(topNode, m_towerNode_mult_cemc);
      ihcal_mult = findNode::getClass<TowerInfoContainer>(topNode, m_towerNode_mult_ihcal);
      ohcal_mult = findNode::getClass<TowerInfoContainer>(topNode, m_towerNode_mult_ohcal);
    }

    if (m_do_r02)
    {
      JetContainer *jets_mult_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_r02);
      JetContainer *jets_mult_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_calib_r02);
      if (!jets_mult_r02 || !jets_mult_calib_r02)
      {
        std::cout << "Aborting Run: Mult R=0.2 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_mult_r02, jets_mult_calib_r02, m_data.mult_r02, 0.2, cemc_mult, ihcal_mult, ohcal_mult, "Mult R=0.2");
    }

    if (m_do_r03)
    {
      JetContainer *jets_mult_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_r03);
      JetContainer *jets_mult_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_mult_calib_r03);
      if (!jets_mult_r03 || !jets_mult_calib_r03)
      {
        std::cout << "Aborting Run: Mult R=0.3 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_mult_r03, jets_mult_calib_r03, m_data.mult_r03, 0.3, cemc_mult, ihcal_mult, ohcal_mult, "Mult R=0.3");
    }
  }

  if (m_do_unsub)
  {
    TowerInfoContainer* cemc_unsub = nullptr;
    TowerInfoContainer* ihcal_unsub = nullptr;
    TowerInfoContainer* ohcal_unsub = nullptr;
    if (m_do_detailed)
    {
      cemc_unsub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
      ihcal_unsub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
      ohcal_unsub = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
    }

    if (m_do_r02)
    {
      JetContainer *jets_unsub_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_r02);
      JetContainer *jets_unsub_calib_r02 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_calib_r02);
      if (!jets_unsub_r02 || !jets_unsub_calib_r02)
      {
        std::cout << "Aborting Run: Unsub R=0.2 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_unsub_r02, jets_unsub_calib_r02, m_data.unsub_r02, 0.2, cemc_unsub, ihcal_unsub, ohcal_unsub, "Unsub R=0.2");
    }

    if (m_do_r03)
    {
      JetContainer *jets_unsub_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_r03);
      JetContainer *jets_unsub_calib_r03 = findNode::getClass<JetContainer>(topNode, m_recoJetName_unsub_calib_r03);
      if (!jets_unsub_r03 || !jets_unsub_calib_r03)
      {
        std::cout << "Aborting Run: Unsub R=0.3 Jets Info null" << std::endl;
        return Fun4AllReturnCodes::ABORTRUN;
      }
      fill_jets(jets_unsub_r03, jets_unsub_calib_r03, m_data.unsub_r03, 0.3, cemc_unsub, ihcal_unsub, ohcal_unsub, "Unsub R=0.3");
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::process_event(PHCompositeNode *topNode)
{
  if (m_do_iter || m_do_mult)
  {
    int ret = process_UE(topNode);
    if (ret)
    {
      return ret;
    }
  }

  if (m_do_iter || m_do_mult || m_do_unsub)
  {
    int ret = process_jets(topNode);
    if (ret)
    {
      return ret;
    }
  }

  // TTree filling is managed by TreeFiller

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  // UE
  if (m_do_iter)
  {
    m_data.seeds_iter = 0;
    m_data.calo_v2_iter = 9999;
    m_data.is_flow_failure_iter = false;
    if (m_do_r02)
    {
      m_data.iter_r02.clear();
    }
    if (m_do_r03)
    {
      m_data.iter_r03.clear();
    }
  }

  if (m_do_mult)
  {
    m_data.seeds_mult = 0;
    m_data.calo_v2_mult = 9999;
    m_data.is_flow_failure_mult = false;
    if (m_do_r02)
    {
      m_data.mult_r02.clear();
    }
    if (m_do_r03)
    {
      m_data.mult_r03.clear();
    }
  }

  if (m_do_unsub)
  {
    if (m_do_r02)
    {
      m_data.unsub_r02.clear();
    }
    if (m_do_r03)
    {
      m_data.unsub_r03.clear();
    }
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetValidationv3::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "JetValidationv3::End" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
