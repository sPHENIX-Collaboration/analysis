#include "RandomConeAna.h"
//for emc clusters
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <TMath.h>
#include <calotrigger/TriggerRunInfov1.h>
#include <calotrigger/TriggerAnalyzer.h>
#include "TRandom.h"
#include "TH1D.h"
#include "TProfile.h"

#include <calobase/RawTowerGeom.h>
#include <jetbackground/TowerBackground.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <HepMC/SimpleVector.h> 
//for vetex information

#include <vector>
#include <fun4all/Fun4AllHistoManager.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>
// G4Cells includes

#include <iostream>

#include <map>

//____________________________________________________________________________..
RandomConeAna::RandomConeAna(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  isSim = false;
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
  m_mb_triggerlist[0] = "MBD N&S >= 1";

  m_mb_triggerlist[1] = "MBD N&S >= 1, vtx < 10 cm";
}

//____________________________________________________________________________..
RandomConeAna::~RandomConeAna()
{

}

//____________________________________________________________________________..
int RandomConeAna::Init(PHCompositeNode *topNode)
{

  rdm = new TRandom();
  hm = new Fun4AllHistoManager("RANDOMCONEHISTOS");

  triggeranalyzer = new TriggerAnalyzer();

  h_random_et = new TH1D("h_random_et", ";eT;", 200, -10, 10);
  h_random_emcal_et = new TH1D("h_random_emcal_et", ";eT;", 200, -10, 10);
  h_random_emcalre_et = new TH1D("h_random_emcalre_et", ";eT;", 200, -10, 10);
  h_random_hcalin_et = new TH1D("h_random_hcalin_et", ";eT;", 200, -10, 10);
  h_random_hcalout_et = new TH1D("h_random_hcalout_et", ";eT;", 200, -10, 10);


  hp_random_emcal_fraction = new TProfile("hp_random_emcal_fraction", ";et;", 200, -10, 10);
  hp_random_emcalre_fraction = new TProfile("hp_random_emcalre_fraction", ";et;", 200, -10, 10);
  hp_random_hcalin_fraction = new TProfile("hp_random_hcalin_fraction", ";eT;", 200, -10, 10);
  hp_random_hcalout_fraction = new TProfile("hp_random_hcalout_fraction", ";eT;", 200, -10, 10);
  hm->registerHisto(h_random_et);
  hm->registerHisto(h_random_emcal_et);
  hm->registerHisto(h_random_emcalre_et);
  hm->registerHisto(h_random_hcalin_et);
  hm->registerHisto(h_random_hcalout_et);
  hm->registerHisto(hp_random_emcal_fraction);
  hm->registerHisto(hp_random_emcalre_fraction);
  hm->registerHisto(hp_random_hcalin_fraction);
  hm->registerHisto(hp_random_hcalout_fraction);


 return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int RandomConeAna::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void RandomConeAna::SetVerbosity(int verbo){
  _verbosity = verbo;
  return;
}

void RandomConeAna::reset_tree_vars()
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;


  return;
}

int RandomConeAna::process_event(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;

  _i_event++;

  if (!isSim)
    {
      triggeranalyzer->decodeTriggers(topNode);
      
      if (!triggeranalyzer->didTriggerFire(m_mb_triggerlist[0]) && !triggeranalyzer->didTriggerFire(m_mb_triggerlist[1]))
	{
	  return Fun4AllReturnCodes::EVENT_OK;
	}
    }
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");


  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALIN");
  if (!hcalin_towers)
    {
      if (Verbosity()) std::cout << "no hcalin towers "<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALOUT");
  if (!hcalout_towers)
    {
      std::cout << "no hcalout towers "<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC_RETOWER");
  TowerInfoContainer *emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC");

   float jet_phi = rdm->Uniform(-1*TMath::Pi(), TMath::Pi());
   float jet_eta = rdm->Uniform(-0.7, 0.7);

   int size;

   double emcalre_eT = 0;
   size = emcalre_towers->size();
   for (int i = 0; i < size; i++)
     {
       TowerInfo* tower = emcalre_towers->get_tower_at_channel(i);

       if (!tower || !tower_geomIH)
	 {
	   continue;
	 }
       if(!tower->get_isGood()) continue;

       unsigned int calokey = emcalre_towers->encode_key(i);

       int ieta = emcalre_towers->getTowerEtaBin(calokey);
       int iphi = emcalre_towers->getTowerPhiBin(calokey);
       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
       float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
       float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
       float dR = sqrt(TMath::Power(jet_eta - tower_eta, 2) + TMath::Power(jet_phi - tower_phi, 2));
       if (dR >= 0.4)
	 {
	   continue;
	 }
       double tower_eT = tower->get_energy() / std::cosh(tower_eta);
       emcalre_eT += tower_eT;       
     }
   double emcal_eT = 0;
   size = emcal_towers->size();
   for (int i = 0; i < size; i++)
     {
       TowerInfo* tower = emcal_towers->get_tower_at_channel(i);

       if (!tower || !tower_geomEM)
	 {
	   continue;
	 }
       if(!tower->get_isGood()) continue;

       unsigned int calokey = emcal_towers->encode_key(i);

       int ieta = emcal_towers->getTowerEtaBin(calokey);
       int iphi = emcal_towers->getTowerPhiBin(calokey);
       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
       float tower_phi = tower_geomEM->get_tower_geometry(key)->get_phi();
       float tower_eta = tower_geomEM->get_tower_geometry(key)->get_eta();
       float dR = sqrt(TMath::Power(jet_eta - tower_eta, 2) + TMath::Power(jet_phi - tower_phi, 2));
       if (dR >= 0.4)
	 {
	   continue;
	 }
       double tower_eT = tower->get_energy() / std::cosh(tower_eta);
       emcal_eT += tower_eT;       
     }
   double hcalin_eT = 0;
   size = hcalin_towers->size();
   for (int i = 0; i < size; i++)
     {
       TowerInfo* tower = hcalin_towers->get_tower_at_channel(i);

       if (!tower || !tower_geomIH)
	 {
	   continue;
	 }
       if(!tower->get_isGood()) continue;

       unsigned int calokey = hcalin_towers->encode_key(i);

       int ieta = hcalin_towers->getTowerEtaBin(calokey);
       int iphi = hcalin_towers->getTowerPhiBin(calokey);
       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
       float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
       float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
       float dR = sqrt(TMath::Power(jet_eta - tower_eta, 2) + TMath::Power(jet_phi - tower_phi, 2));
       if (dR >= 0.4)
	 {
	   continue;
	 }
       double tower_eT = tower->get_energy() / std::cosh(tower_eta);
       hcalin_eT += tower_eT;       
     }
   double hcalout_eT = 0;
   size = hcalout_towers->size();
   for (int i = 0; i < size; i++)
     {
       TowerInfo* tower = hcalout_towers->get_tower_at_channel(i);

       if (!tower || !tower_geomIH)
	 {
	   continue;
	 }
       if(!tower->get_isGood()) continue;

       unsigned int calokey = hcalout_towers->encode_key(i);

       int ieta = hcalout_towers->getTowerEtaBin(calokey);
       int iphi = hcalout_towers->getTowerPhiBin(calokey);
       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
       float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
       float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
       float dR = sqrt(TMath::Power(jet_eta - tower_eta, 2) + TMath::Power(jet_phi - tower_phi, 2));
       if (dR >= 0.4)
	 {
	   continue;
	 }
       double tower_eT = tower->get_energy() / std::cosh(tower_eta);
       hcalout_eT += tower_eT;       
     }


   double totalre_eT = emcalre_eT + hcalin_eT + hcalout_eT;
   double total_eT = emcal_eT + hcalin_eT + hcalout_eT;

   h_random_et->Fill(totalre_eT);
   h_random_emcalre_et->Fill(emcalre_eT);
   h_random_emcal_et->Fill(emcal_eT);
   h_random_hcalin_et->Fill(hcalin_eT);
   h_random_hcalout_et->Fill(hcalout_eT);

   hp_random_emcalre_fraction->Fill(totalre_eT, emcalre_eT/totalre_eT);
   hp_random_emcal_fraction->Fill(total_eT,emcal_eT/total_eT);
   hp_random_hcalin_fraction->Fill(totalre_eT,hcalin_eT/totalre_eT);
   hp_random_hcalout_fraction->Fill(totalre_eT,hcalout_eT/totalre_eT);
   return Fun4AllReturnCodes::EVENT_OK;
}



void RandomConeAna::GetNodes(PHCompositeNode* topNode)
{


}

int RandomConeAna::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "RandomConeAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int RandomConeAna::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "RandomConeAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int RandomConeAna::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "RandomConeAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  hm->dumpHistos(_foutname.c_str());

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int RandomConeAna::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "RandomConeAna::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

