#include "RandomConeValidation.h"

#include <iostream>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <ffaobjects/EventHeader.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <treefiller/TreeFiller.h>
#include <TTree.h>

namespace
{
  // 64-bit golden ratio and SplitMix64 diffusion constants for uniform hash distribution
  constexpr uint64_t GOLDEN_RATIO_HASH_64 = 0x9e3779b97f4a7c15ULL;
  constexpr uint64_t SPLITMIX64_MIX_CONST = 0xbf58476d1ce4e5b9ULL;

  uint32_t compute_event_seed(int runnumber, int event_id, uint32_t seed_offset)
  {
    uint64_t h = static_cast<uint64_t>(runnumber) * GOLDEN_RATIO_HASH_64;
    h ^= static_cast<uint64_t>(event_id) * SPLITMIX64_MIX_CONST;
    h ^= (h >> 30U);
    h *= SPLITMIX64_MIX_CONST;
    h ^= (h >> 27U);
    return static_cast<uint32_t>(h) + seed_offset;
  }
}  // namespace

RandomConeValidation::RandomConeValidation(const std::string &name)
  : SubsysReco(name)
  , m_maker_r02(0.2)
  , m_maker_r03(0.3)
{
}

int RandomConeValidation::Init([[maybe_unused]] PHCompositeNode *topNode)
{
  TTree* tree = TreeFiller::getTree();
  if (!tree)
  {
    std::cout << "RandomConeValidation: Failed to get/create TTree" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  tree->Branch("rcone_r02_eta", &m_data.r02.eta);
  tree->Branch("rcone_r02_phi", &m_data.r02.phi);
  tree->Branch("rcone_r02_pt", &m_data.r02.pt);
  tree->Branch("rcone_r02_energy", &m_data.r02.energy);
  tree->Branch("rcone_r02_sub1_pt", &m_data.r02.pt_sub1);
  tree->Branch("rcone_r02_sub1_energy", &m_data.r02.energy_sub1);

  tree->Branch("rcone_r03_eta", &m_data.r03.eta);
  tree->Branch("rcone_r03_phi", &m_data.r03.phi);
  tree->Branch("rcone_r03_pt", &m_data.r03.pt);
  tree->Branch("rcone_r03_energy", &m_data.r03.energy);
  tree->Branch("rcone_r03_sub1_pt", &m_data.r03.pt_sub1);
  tree->Branch("rcone_r03_sub1_energy", &m_data.r03.energy_sub1);

  if (m_do_detailed)
  {
    // R = 0.2 unsubtracted
    tree->Branch("rcone_r02_emcal_tower_index", &m_data.r02.emcal_tower_index);
    tree->Branch("rcone_r02_emcal_tower_energy", &m_data.r02.emcal_tower_energy);
    tree->Branch("rcone_r02_emcal_tower_pt", &m_data.r02.emcal_tower_pt);

    tree->Branch("rcone_r02_ihcal_tower_index", &m_data.r02.ihcal_tower_index);
    tree->Branch("rcone_r02_ihcal_tower_energy", &m_data.r02.ihcal_tower_energy);
    tree->Branch("rcone_r02_ihcal_tower_pt", &m_data.r02.ihcal_tower_pt);

    tree->Branch("rcone_r02_ohcal_tower_index", &m_data.r02.ohcal_tower_index);
    tree->Branch("rcone_r02_ohcal_tower_energy", &m_data.r02.ohcal_tower_energy);
    tree->Branch("rcone_r02_ohcal_tower_pt", &m_data.r02.ohcal_tower_pt);

    // R = 0.2 subtracted (sub1)
    tree->Branch("rcone_r02_sub1_emcal_tower_energy", &m_data.r02.emcal_sub1_tower_energy);
    tree->Branch("rcone_r02_sub1_emcal_tower_pt", &m_data.r02.emcal_sub1_tower_pt);

    tree->Branch("rcone_r02_sub1_ihcal_tower_energy", &m_data.r02.ihcal_sub1_tower_energy);
    tree->Branch("rcone_r02_sub1_ihcal_tower_pt", &m_data.r02.ihcal_sub1_tower_pt);

    tree->Branch("rcone_r02_sub1_ohcal_tower_energy", &m_data.r02.ohcal_sub1_tower_energy);
    tree->Branch("rcone_r02_sub1_ohcal_tower_pt", &m_data.r02.ohcal_sub1_tower_pt);

    // R = 0.3 unsubtracted
    tree->Branch("rcone_r03_emcal_tower_index", &m_data.r03.emcal_tower_index);
    tree->Branch("rcone_r03_emcal_tower_energy", &m_data.r03.emcal_tower_energy);
    tree->Branch("rcone_r03_emcal_tower_pt", &m_data.r03.emcal_tower_pt);

    tree->Branch("rcone_r03_ihcal_tower_index", &m_data.r03.ihcal_tower_index);
    tree->Branch("rcone_r03_ihcal_tower_energy", &m_data.r03.ihcal_tower_energy);
    tree->Branch("rcone_r03_ihcal_tower_pt", &m_data.r03.ihcal_tower_pt);

    tree->Branch("rcone_r03_ohcal_tower_index", &m_data.r03.ohcal_tower_index);
    tree->Branch("rcone_r03_ohcal_tower_energy", &m_data.r03.ohcal_tower_energy);
    tree->Branch("rcone_r03_ohcal_tower_pt", &m_data.r03.ohcal_tower_pt);

    // R = 0.3 subtracted (sub1)
    tree->Branch("rcone_r03_sub1_emcal_tower_energy", &m_data.r03.emcal_sub1_tower_energy);
    tree->Branch("rcone_r03_sub1_emcal_tower_pt", &m_data.r03.emcal_sub1_tower_pt);

    tree->Branch("rcone_r03_sub1_ihcal_tower_energy", &m_data.r03.ihcal_sub1_tower_energy);
    tree->Branch("rcone_r03_sub1_ihcal_tower_pt", &m_data.r03.ihcal_sub1_tower_pt);

    tree->Branch("rcone_r03_sub1_ohcal_tower_energy", &m_data.r03.ohcal_sub1_tower_energy);
    tree->Branch("rcone_r03_sub1_ohcal_tower_pt", &m_data.r03.ohcal_sub1_tower_pt);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeValidation::InitRun(PHCompositeNode *topNode)
{
  RawTowerGeomContainer* geom_hcalin = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer* geom_hcalout = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  m_maker_r02.init(geom_hcalin, geom_hcalout);
  m_maker_r03.init(geom_hcalin, geom_hcalout);

  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeValidation::process_event(PHCompositeNode *topNode)
{
  auto* eventInfo = findNode::getClass<EventHeader>(topNode, "EventHeader");
  int runnumber = eventInfo ? eventInfo->get_RunNumber() : 0;
  int event_id = eventInfo ? eventInfo->get_EvtSequence() : 0;

  uint32_t event_seed = compute_event_seed(runnumber, event_id, m_seed_offset);

  m_maker_r02.setSeed(event_seed);
  m_maker_r03.setSeed(event_seed + 1);

  TowerInfoContainer* cemc_retower = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  TowerInfoContainer* hcalin = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  TowerInfoContainer* hcalout = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  TowerInfoContainer* cemc_retower_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
  TowerInfoContainer* hcalin_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
  TowerInfoContainer* hcalout_sub1 = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");

  double zvtx = 0.0;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    std::cout << "RandomConeValidation: GlobalVertexMap null" << std::endl;
  }
  else if (!vertexmap->empty())
  {
    GlobalVertex *vtx = vertexmap->begin()->second;
    if (vtx)
    {
      zvtx = vtx->get_z();
    }
  }

  // Unsubtracted cones (randomly chosen eta, phi)
  RandomCone cone_r02 = m_maker_r02.generate(cemc_retower, hcalin, hcalout, zvtx, m_do_detailed);
  m_data.r02.eta = cone_r02.eta;
  m_data.r02.phi = cone_r02.phi;
  m_data.r02.pt = cone_r02.pt;
  m_data.r02.energy = cone_r02.energy;
  if (m_do_detailed)
  {
    m_data.r02.emcal_tower_index = std::move(cone_r02.emcal_tower_index);
    m_data.r02.emcal_tower_energy = std::move(cone_r02.emcal_tower_energy);
    m_data.r02.emcal_tower_pt = std::move(cone_r02.emcal_tower_pt);

    m_data.r02.ihcal_tower_index = std::move(cone_r02.ihcal_tower_index);
    m_data.r02.ihcal_tower_energy = std::move(cone_r02.ihcal_tower_energy);
    m_data.r02.ihcal_tower_pt = std::move(cone_r02.ihcal_tower_pt);

    m_data.r02.ohcal_tower_index = std::move(cone_r02.ohcal_tower_index);
    m_data.r02.ohcal_tower_energy = std::move(cone_r02.ohcal_tower_energy);
    m_data.r02.ohcal_tower_pt = std::move(cone_r02.ohcal_tower_pt);
  }

  RandomCone cone_r03 = m_maker_r03.generate(cemc_retower, hcalin, hcalout, zvtx, m_do_detailed);
  m_data.r03.eta = cone_r03.eta;
  m_data.r03.phi = cone_r03.phi;
  m_data.r03.pt = cone_r03.pt;
  m_data.r03.energy = cone_r03.energy;
  if (m_do_detailed)
  {
    m_data.r03.emcal_tower_index = std::move(cone_r03.emcal_tower_index);
    m_data.r03.emcal_tower_energy = std::move(cone_r03.emcal_tower_energy);
    m_data.r03.emcal_tower_pt = std::move(cone_r03.emcal_tower_pt);

    m_data.r03.ihcal_tower_index = std::move(cone_r03.ihcal_tower_index);
    m_data.r03.ihcal_tower_energy = std::move(cone_r03.ihcal_tower_energy);
    m_data.r03.ihcal_tower_pt = std::move(cone_r03.ihcal_tower_pt);

    m_data.r03.ohcal_tower_index = std::move(cone_r03.ohcal_tower_index);
    m_data.r03.ohcal_tower_energy = std::move(cone_r03.ohcal_tower_energy);
    m_data.r03.ohcal_tower_pt = std::move(cone_r03.ohcal_tower_pt);
  }

  // Subtracted cones (sharing the same eta, phi as unsubtracted)
  RandomCone cone_r02_sub1 = m_maker_r02.generate(cemc_retower_sub1, hcalin_sub1, hcalout_sub1, zvtx, m_do_detailed, cone_r02.eta, cone_r02.phi);
  m_data.r02.pt_sub1 = cone_r02_sub1.pt;
  m_data.r02.energy_sub1 = cone_r02_sub1.energy;
  if (m_do_detailed)
  {
    m_data.r02.emcal_sub1_tower_energy = std::move(cone_r02_sub1.emcal_tower_energy);
    m_data.r02.emcal_sub1_tower_pt = std::move(cone_r02_sub1.emcal_tower_pt);

    m_data.r02.ihcal_sub1_tower_energy = std::move(cone_r02_sub1.ihcal_tower_energy);
    m_data.r02.ihcal_sub1_tower_pt = std::move(cone_r02_sub1.ihcal_tower_pt);

    m_data.r02.ohcal_sub1_tower_energy = std::move(cone_r02_sub1.ohcal_tower_energy);
    m_data.r02.ohcal_sub1_tower_pt = std::move(cone_r02_sub1.ohcal_tower_pt);
  }

  RandomCone cone_r03_sub1 = m_maker_r03.generate(cemc_retower_sub1, hcalin_sub1, hcalout_sub1, zvtx, m_do_detailed, cone_r03.eta, cone_r03.phi);
  m_data.r03.pt_sub1 = cone_r03_sub1.pt;
  m_data.r03.energy_sub1 = cone_r03_sub1.energy;
  if (m_do_detailed)
  {
    m_data.r03.emcal_sub1_tower_energy = std::move(cone_r03_sub1.emcal_tower_energy);
    m_data.r03.emcal_sub1_tower_pt = std::move(cone_r03_sub1.emcal_tower_pt);

    m_data.r03.ihcal_sub1_tower_energy = std::move(cone_r03_sub1.ihcal_tower_energy);
    m_data.r03.ihcal_sub1_tower_pt = std::move(cone_r03_sub1.ihcal_tower_pt);

    m_data.r03.ohcal_sub1_tower_energy = std::move(cone_r03_sub1.ohcal_tower_energy);
    m_data.r03.ohcal_sub1_tower_pt = std::move(cone_r03_sub1.ohcal_tower_pt);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeValidation::ResetEvent([[maybe_unused]] PHCompositeNode *topNode)
{
  m_data.r02.clear();
  m_data.r03.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

int RandomConeValidation::End([[maybe_unused]] PHCompositeNode *topNode)
{
  std::cout << "RandomConeValidation::End" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
