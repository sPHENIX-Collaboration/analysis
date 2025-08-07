#include "JetPerformance.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TH1D.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <TMath.h>
#include <calotrigger/TriggerRunInfov1.h>

#include <calobase/RawTowerGeom.h>
#include <jetbackground/TowerBackground.h>
#include <jetbase/Jet.h>
#include <jetbase/JetContainer.h>
#include <jetbase/JetContainerv1.h>
#include <jetbase/Jetv2.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>

#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <HepMC/SimpleVector.h> 
//for vetex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include "TrashInfov1.h"
#include <vector>

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
JetPerformance::JetPerformance(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  isSim = false;
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
  m_jet_triggernames = {"Jet 6 GeV + MBD NS >= 1",
			"Jet 8 GeV + MBD NS >= 1",
			"Jet 10 GeV + MBD NS >= 1",
			"Jet 12 GeV + MBD NS >= 1",
			"Jet 6 GeV",
			"Jet 8 GeV",
			"Jet 10 GeV",
			"Jet 12 GeV"};
}

//____________________________________________________________________________..
JetPerformance::~JetPerformance()
{

}

//____________________________________________________________________________..
int JetPerformance::Init(PHCompositeNode *topNode)
{

  triggeranalyzer = new TriggerAnalyzer();

  hm = new Fun4AllHistoManager("DIJET_HISTOGRAMS");

  TH1D *h;

  h = new TH1D("h_jet_et_mbd", ";et;N", 50, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_jet_pt_mbd", ";pt;N", 50, 0, 50);
  hm->registerHisto(h);

  h = new TH1D("h_jet_et_scaled_mbd", ";et;N", 50, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_jet_pt_scaled_mbd", ";pt;N", 50, 0, 50);
  hm->registerHisto(h);

  for (int i = 0 ; i < 8; i++)
    {
      h = new TH1D(Form("h_jet_et_scaled_gl1_%d", i), ";et;N", 50, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_pt_scaled_gl1_%d", i), ";pt;N", 50, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_et_raw_gl1_%d", i), ";et;N", 50, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_pt_raw_gl1_%d", i), ";pt;N", 50, 0, 50);
      hm->registerHisto(h);
    }

 
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetPerformance::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

int JetPerformance::process_event(PHCompositeNode *topNode)
{


  _i_event++;

  triggeranalyzer->decodeTriggers(topNode);


  if (mbd_prescale == -2)
    {
      mbd_prescale = triggeranalyzer->getPrescaleByName("MBD N&S >= 1");
    }
  for (int i = 0; i < 8 ; i++)
    {
      if (jet_prescale[i] == -2)
	{
	  jet_prescale[i] = triggeranalyzer->getPrescaleByName(m_jet_triggernames[i]);
	}
    }

  bool mbd_fired = triggeranalyzer->didTriggerFire("MBD N&S >= 1");
  bool jet_fired[8]; 
  bool jet_raw[8];
  for (int i = 0; i < 8; i++)
    {
      jet_fired[i] = triggeranalyzer->didTriggerFire(m_jet_triggernames);
      jet_raw[i] = triggeranalyzer->checkRawTrigger(m_jet_triggernames);
    }
  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");


  std::string recoJetName = "AntiKt_Tower_r04_Sub1";

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);

  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALIN");
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALOUT");
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC_RETOWER");

  float background_v2 = 0;
  float background_Psi2 = 0;
  bool has_tower_background = false;
  TowerBackground *towBack = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if (!towBack)
    {

    }
  else
    {
      has_tower_background = true;
      background_v2 = towBack->get_v2();
      background_Psi2 = towBack->get_Psi2();
    }

  if (!jets_4)
    {
      return Fun4AllReturnCodes::ABORTRUN;
    }

  auto hem = dynamic_cast<TH1*>(hm->getHisto("h_jet_et_mbd"));// ";et;N", 50, 0, 50);
  auto hpm = dynamic_cast<TH1*>(hm->getHisto("h_jet_pt_mbd"));// ";pt;N", 50, 0, 50);
  auto hesm = dynamic_cast<TH1*>(hm->getHisto("h_jet_et_scaled_mbd"));// ";et;N", 50, 0, 50);
  auto hpsm = dynamic_cast<TH1*>(hm->getHisto("h_jet_pt_scaled_mbd"));// ";pt;N", 50, 0, 50);
  TH1D *hesg[8];
  TH1D *hpsg[8];
  TH1D *herg[8];
  TH1D *hprg[8];
  for (int i = 0 ; i < 8; i++)
    {
      hesg[i] = dynamic_cast<TH1*>(hm->getHisto(Form("h_jet_et_scaled_gl1_%d", i)));// ";et;N", 50, 0, 50);
      hpsg[i] = dynamic_cast<TH1*>(hm->getHisto(Form("h_jet_pt_scaled_gl1_%d", i)));// ";pt;N", 50, 0, 50);
      herg[i] = dynamic_cast<TH1*>(hm->getHisto(Form("h_jet_et_raw_gl1_%d", i)));// ";et;N", 50, 0, 50);
      hprg[i] = dynamic_cast<TH1*>(hm->getHisto(Form("h_jet_pt_raw_gl1_%d", i)));// ";pt;N", 50, 0, 50);
    }




  for (auto jet : *jets_4)
    {
      float jet_total_eT = 0;
      float eFrac_ihcal = 0;
      float eFrac_ohcal = 0;
      float eFrac_emcal = 0;
      
      for (auto comp : jet->get_comp_vec())
	{
	  
	  unsigned int channel = comp.second;
	  TowerInfo *tower;
	  float tower_eT = 0;
	  if (comp.first == 26 || comp.first == 30)
	    {  // IHcal
	      tower = hcalin_towers->get_tower_at_channel(channel);
	      if (!tower || !tower_geomIH)
		{
		  continue;
		}
	      if(!tower->get_isGood()) continue;

	      unsigned int calokey = hcalin_towers->encode_key(channel);

	      int ieta = hcalin_towers->getTowerEtaBin(calokey);
	      int iphi = hcalin_towers->getTowerPhiBin(calokey);
	      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      if (comp.first == 30)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(1).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}

	      eFrac_ihcal += tower_eT;
	      jet_total_eT += tower_eT;
	    }
	  else if (comp.first == 27 || comp.first == 31)
	    {  // IHcal
	      tower = hcalout_towers->get_tower_at_channel(channel);

	      if (!tower || !tower_geomOH)
		{
		  continue;
		}

	      unsigned int calokey = hcalout_towers->encode_key(channel);
	      int ieta = hcalout_towers->getTowerEtaBin(calokey);
	      int iphi = hcalout_towers->getTowerPhiBin(calokey);
	      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
	      float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      if (comp.first == 31)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(2).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}

	      eFrac_ohcal += tower_eT;
	      jet_total_eT += tower_eT;
	    }
	  else if (comp.first == 28 || comp.first == 29)
	    {  // IHcal
	      tower = emcalre_towers->get_tower_at_channel(channel);

	      if (!tower || !tower_geomIH)
		{
		  continue;
		}

	      unsigned int calokey = emcalre_towers->encode_key(channel);
	      int ieta = emcalre_towers->getTowerEtaBin(calokey);
	      int iphi = emcalre_towers->getTowerPhiBin(calokey);
	      const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
	      float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	      float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
	      tower_eT = tower->get_energy() / std::cosh(tower_eta);

	      if (comp.first == 29)
		{  // is sub1
		  if (has_tower_background)
		    {
		      float UE = towBack->get_UE(0).at(ieta);
		      float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
		      tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
		    }
		}


	      eFrac_emcal += tower_eT;
	      jet_total_eT += tower_eT;
	    }	       
	}

      if (jet->get_pt() > pt_cut)
    }
  return Fun4AllReturnCodes::EVENT_OK;
}



void JetPerformance::GetNodes(PHCompositeNode* topNode)
{


}

int JetPerformance::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetPerformance::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetPerformance::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "JetPerformance::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  hm->dumpHistos(_foutname.c_str());

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int JetPerformance::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "JetPerformance::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
