#include "RandomConeValidation.h"

#include <iostream>

#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/TowerInfoContainer.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

#include <treefiller/TreeFiller.h>
#include <TTree.h>

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
  RandomCone cone_r02 = m_maker_r02.generate(cemc_retower, hcalin, hcalout, zvtx);
  m_data.r02.eta = cone_r02.eta;
  m_data.r02.phi = cone_r02.phi;
  m_data.r02.pt = cone_r02.pt;
  m_data.r02.energy = cone_r02.energy;

  RandomCone cone_r03 = m_maker_r03.generate(cemc_retower, hcalin, hcalout, zvtx);
  m_data.r03.eta = cone_r03.eta;
  m_data.r03.phi = cone_r03.phi;
  m_data.r03.pt = cone_r03.pt;
  m_data.r03.energy = cone_r03.energy;

  // Subtracted cones (sharing the same eta, phi as unsubtracted)
  RandomCone cone_r02_sub1 = m_maker_r02.generate(cemc_retower_sub1, hcalin_sub1, hcalout_sub1, zvtx, cone_r02.eta, cone_r02.phi);
  m_data.r02.pt_sub1 = cone_r02_sub1.pt;
  m_data.r02.energy_sub1 = cone_r02_sub1.energy;

  RandomCone cone_r03_sub1 = m_maker_r03.generate(cemc_retower_sub1, hcalin_sub1, hcalout_sub1, zvtx, cone_r03.eta, cone_r03.phi);
  m_data.r03.pt_sub1 = cone_r03_sub1.pt;
  m_data.r03.energy_sub1 = cone_r03_sub1.energy;

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
