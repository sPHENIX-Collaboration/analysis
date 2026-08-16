#include "CaloQA.h"
#include "geometry_constants.h"

// -- Fun4All
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// -- Centrality
#include <centrality/CentralityInfo.h>

// -- Calo
#include <calobase/TowerInfoDefs.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfo.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/getClass.h>

#include <treefiller/TreeFiller.h>

#include <TTree.h>

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

  if (m_do_hists)
  {
    int bins_emcal_phi = CaloGeometry::CEMC_PHI_BINS;
    int bins_emcal_eta = CaloGeometry::CEMC_ETA_BINS;

    int bins_hcal_phi = CaloGeometry::HCAL_PHI_BINS;
    int bins_hcal_eta = CaloGeometry::HCAL_ETA_BINS;

    int bins_cent_full = 80;
    int bins_cent = 8;
    double cent_low = -0.5;
    double cent_high = 79.5;

    int bins_energy = 80;
    int energy_low = -10;
    int energy_high = 30;

    int bins_energy_zs = 1100;
    int energy_zs_low = -10;
    int energy_zs_high = 1;

    m_hists.h2EMCal = new TProfile2D("h2EMCal", "EMCal; Tower Index #phi; Tower Index #eta",
                                     bins_emcal_phi, 0, bins_emcal_phi,
                                     bins_emcal_eta, 0, bins_emcal_eta);

    m_hists.h2EMCalRetowered = new TProfile2D("h2EMCalRetowered", "EMCal; Tower Index #phi; Tower Index #eta",
                                              bins_hcal_phi, 0, bins_hcal_phi,
                                              bins_hcal_eta, 0, bins_hcal_eta);

    m_hists.h2IHCal = new TProfile2D("h2IHCal", "IHCal; Tower Index #phi; Tower Index #eta",
                                     bins_hcal_phi, 0, bins_hcal_phi,
                                     bins_hcal_eta, 0, bins_hcal_eta);

    m_hists.h2OHCal = new TProfile2D("h2OHCal", "OHCal; Tower Index #phi; Tower Index #eta",
                                     bins_hcal_phi, 0, bins_hcal_phi,
                                     bins_hcal_eta, 0, bins_hcal_eta);

    m_hists.h2EMCalCent = new TH2F("h2EMCalCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                   bins_energy, energy_low, energy_high,
                                   bins_cent, cent_low, cent_high);

    m_hists.h2EMCalRetoweredCent = new TH2F("h2EMCalRetoweredCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                            bins_energy, energy_low, energy_high,
                                            bins_cent, cent_low, cent_high);

    m_hists.h2IHCalCent = new TH2F("h2IHCalCent", "IHCal; Tower Energy [GeV]; Centrality [%]",
                                   bins_energy, energy_low, energy_high,
                                   bins_cent, cent_low, cent_high);

    m_hists.h2OHCalCent = new TH2F("h2OHCalCent", "OHCal; Tower Energy [GeV]; Centrality [%]",
                                   bins_energy, energy_low, energy_high,
                                   bins_cent, cent_low, cent_high);

    m_hists.h2EMCalZSCent = new TH2F("h2EMCalZSCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                     bins_energy_zs, energy_zs_low, energy_zs_high,
                                     bins_cent, cent_low, cent_high);

    m_hists.h2IHCalZSCent = new TH2F("h2IHCalZSCent", "IHCal; Tower Energy [GeV]; Centrality [%]",
                                     bins_energy_zs, energy_zs_low, energy_zs_high,
                                     bins_cent, cent_low, cent_high);

    m_hists.h2OHCalZSCent = new TH2F("h2OHCalZSCent", "OHCal; Tower Energy [GeV]; Centrality [%]",
                                     bins_energy_zs, energy_zs_low, energy_zs_high,
                                     bins_cent, cent_low, cent_high);

    m_hists.h2EMCalNoZSCent = new TH2F("h2EMCalNoZSCent", "EMCal; Tower Energy [GeV]; Centrality [%]",
                                       bins_energy, energy_low, energy_high,
                                       bins_cent, cent_low, cent_high);

    m_hists.h2IHCalNoZSCent = new TH2F("h2IHCalNoZSCent", "IHCal; Tower Energy [GeV]; Centrality [%]",
                                       bins_energy, energy_low, energy_high,
                                       bins_cent, cent_low, cent_high);

    m_hists.h2OHCalNoZSCent = new TH2F("h2OHCalNoZSCent", "OHCal; Tower Energy [GeV]; Centrality [%]",
                                       bins_energy, energy_low, energy_high,
                                       bins_cent, cent_low, cent_high);

    int bins_totalCaloE = 2500;
    double totalCaloE_low = 0;
    double totalCaloE_high = 2500;

    m_hists.h2CentralityTotalCaloE = new TH2F("h2CentralityTotalCaloE", "|z| < 10 cm and MB; Total Calorimeter Energy [GeV]; Centrality [%]",
                                              bins_totalCaloE, totalCaloE_low, totalCaloE_high,
                                              bins_cent_full, cent_low, cent_high);

    Fun4AllServer* se = Fun4AllServer::instance();

    se->registerHisto(m_hists.h2EMCal);
    se->registerHisto(m_hists.h2EMCalRetowered);
    se->registerHisto(m_hists.h2IHCal);
    se->registerHisto(m_hists.h2OHCal);

    se->registerHisto(m_hists.h2EMCalCent);
    se->registerHisto(m_hists.h2EMCalRetoweredCent);
    se->registerHisto(m_hists.h2IHCalCent);
    se->registerHisto(m_hists.h2OHCalCent);

    se->registerHisto(m_hists.h2EMCalZSCent);
    se->registerHisto(m_hists.h2IHCalZSCent);
    se->registerHisto(m_hists.h2OHCalZSCent);

    se->registerHisto(m_hists.h2EMCalNoZSCent);
    se->registerHisto(m_hists.h2IHCalNoZSCent);
    se->registerHisto(m_hists.h2OHCalNoZSCent);

    se->registerHisto(m_hists.h2CentralityTotalCaloE);
  }

  TTree* tree = TreeFiller::getTree();
  if (tree)
  {
    tree->Branch("emcal_energy", &m_data.emcal_energy);
    tree->Branch("ihcal_energy", &m_data.ihcal_energy);
    tree->Branch("ohcal_energy", &m_data.ohcal_energy);
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

  m_data.centrality = centInfo->get_centile(CentralityInfo::PROP::mbd_NS) * 100;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::process_calo(PHCompositeNode *topNode)
{
  auto* towersCEMC  = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC");
  auto* towersCEMCRetowered = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  auto* towersIHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
  auto* towersOHCal = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");

  double cent = m_data.centrality;

  if (!towersCEMC || !towersCEMCRetowered || !towersIHCal || !towersOHCal)
  {
    return Fun4AllReturnCodes::ABORTRUN;
  }

  size_t nTowersCEMC = towersCEMCRetowered->size();
  size_t nTowersIHCal = towersIHCal->size();
  size_t nTowersOHCal = towersOHCal->size();

  if(nTowersCEMC != nTowersIHCal || nTowersCEMC != nTowersOHCal)
  {
    std::cout << "Calo Contains Missing Towers!" << std::endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (m_do_hists)
  {
    // EMCal
    for (unsigned int towerIndex = 0; towerIndex < towersCEMC->size(); ++towerIndex)
    {
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
      else
      {
        m_hists.h2EMCalNoZSCent->Fill(energy, cent);
      }
    }
  }

  double totalCaloE = 0;
  for (unsigned int towerIndex = 0; towerIndex < towersCEMCRetowered->size(); ++towerIndex)
  {
    unsigned int key = TowerInfoDefs::encode_hcal(towerIndex);
    unsigned int iphi = TowerInfoDefs::getCaloTowerPhiBin(key);
    unsigned int ieta = TowerInfoDefs::getCaloTowerEtaBin(key);

    auto* towerCEMC = towersCEMCRetowered->get_tower_at_channel(towerIndex);
    if(towerCEMC && towerCEMC->get_isGood())
    {
      float energy = towerCEMC->get_energy();
      m_data.emcal_energy += energy;
      totalCaloE += energy;

      if (m_do_hists)
      {
        m_hists.h2EMCalRetowered->Fill(iphi, ieta, energy);
        m_hists.h2EMCalRetoweredCent->Fill(energy, cent);
      }
    }

    auto* towerIHCal = towersIHCal->get_tower_at_channel(towerIndex);
    if(towerIHCal && towerIHCal->get_isGood())
    {
      float energy = towerIHCal->get_energy();
      m_data.ihcal_energy += energy;
      totalCaloE += energy;

      if (m_do_hists)
      {
        m_hists.h2IHCal->Fill(iphi, ieta, energy);
        m_hists.h2IHCalCent->Fill(energy, cent);

        if (towerIHCal->get_isZS())
        {
          m_hists.h2IHCalZSCent->Fill(energy, cent);
        }
        else
        {
          m_hists.h2IHCalNoZSCent->Fill(energy, cent);
        }
      }
    }

    auto* towerOHCal = towersOHCal->get_tower_at_channel(towerIndex);
    if(towerOHCal && towerOHCal->get_isGood())
    {
      float energy = towerOHCal->get_energy();
      m_data.ohcal_energy += energy;
      totalCaloE += energy;

      if (m_do_hists)
      {
        m_hists.h2OHCal->Fill(iphi, ieta, energy);
        m_hists.h2OHCalCent->Fill(energy, cent);

        if (towerOHCal->get_isZS())
        {
          m_hists.h2OHCalZSCent->Fill(energy, cent);
        }
        else
        {
          m_hists.h2OHCalNoZSCent->Fill(energy, cent);
        }
      }
    }
  }

  if (m_do_hists)
  {
    m_hists.h2CentralityTotalCaloE->Fill(totalCaloE, cent);
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::process_event([[maybe_unused]] PHCompositeNode* topNode)
{
  int ret = process_centrality(topNode);
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
  // Calo
  m_data.emcal_energy = 0;
  m_data.ihcal_energy = 0;
  m_data.ohcal_energy = 0;

  // Centrality
  m_data.centrality = 0;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloQA::End([[maybe_unused]] PHCompositeNode* topNode)
{
  std::cout << "CaloQA::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}
