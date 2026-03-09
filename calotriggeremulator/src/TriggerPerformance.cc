#include "TriggerPerformance.h"
#include "FindDijets.h"
//for emc clusters
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TFile.h>
#include <TF1.h>
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
#include "DijetEventDisplay.h"
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
TriggerPerformance::TriggerPerformance(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
  m_photon_triggernames = {"Photon 2 GeV+ MBD NS >= 1",
			   "Photon 3 GeV + MBD NS >= 1",
			   "Photon 4 GeV + MBD NS >= 1",
			   "Photon 5 GeV + MBD NS >= 1"};
  m_photon_prescales = {-1,
			-1,
			-1,
			-1};
  
  m_jet_triggernames = {"Jet 6 GeV + MBD NS >= 1",
			"Jet 8 GeV + MBD NS >= 1",
			"Jet 10 GeV + MBD NS >= 1",
			"Jet 12 GeV + MBD NS >= 1"};
  m_jet_prescales = {-1,
		     -1,
		     -1,
		     -1};

  ///////// FOR CALO CALIB ///////////
  m_jet_emu_triggernames = {"Jet 6 GeV",
			    "Jet 8 GeV",
			    "Jet 10 GeV",
			    "Jet 12 GeV"};
  m_photon_emu_triggernames = {"Photon 2 GeV",
			       "Photon 3 GeV",
			       "Photon 4 GeV",
			       "Photon 5 GeV"};

  m_mbd_prescale = -1;
}

//____________________________________________________________________________..
TriggerPerformance::~TriggerPerformance()
{

}

//____________________________________________________________________________..
int TriggerPerformance::Init(PHCompositeNode *topNode)
{
  _i_event = 0;

  ///////// FOR CALO CALIB ///////////
  triggeranalyzer = new TriggerAnalyzer();

  triggeranalyzer->UseEmulator(m_useEmulator);

  hm = new Fun4AllHistoManager("TRIGGER_HISTOGRAMS");

  TH1D *h;
  TH2D *h2;


  h = new TH1D("h_jet_et_mb", ";E_{T}; count", 200, 0, 100);
  hm->registerHisto(h);
  h = new TH1D("h_jet_pt_mb", ";p_{T}; count", 200, 0, 100);
  hm->registerHisto(h);

  h = new TH1D("h_jet_et_scaled_mb", ";E_{T}; count", 200, 0, 100);
  hm->registerHisto(h);
  h = new TH1D("h_jet_pt_scaled_mb", ";p_{T}; count", 200, 0, 100);
  hm->registerHisto(h);

  h2 = new TH2D("h_2d_jet_dist_mb",";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
  hm->registerHisto(h2);

  h = new TH1D("h_jet_max_et_mb", ";E_{T}; count", 200, 0, 100);
  hm->registerHisto(h);
  h = new TH1D("h_jet_max_pt_mb", ";p_{T}; count", 200, 0, 100);
  hm->registerHisto(h);

  h = new TH1D("h_jet_max_et_scaled_mb", ";E_{T}; count", 200, 0, 100);
  hm->registerHisto(h);
  h = new TH1D("h_jet_max_pt_scaled_mb", ";p_{T}; count", 200, 0, 100);
  hm->registerHisto(h);

  h2 = new TH2D("h_2d_jet_max_dist_mb",";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
  hm->registerHisto(h2);

  for (int j = 0; j < 4; j++)
    {
      h2 = new TH2D(Form("h_2d_jet_dist_trig_%d", j),";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
      hm->registerHisto(h2);


      h = new TH1D(Form("h_jet_et_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_pt_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_et_scaled_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_pt_scaled_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_et_raw_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_pt_raw_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      //Maximum
      h2 = new TH2D(Form("h_2d_jet_max_dist_trig_%d", j),";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
      hm->registerHisto(h2);


      h = new TH1D(Form("h_jet_max_et_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_max_pt_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_max_et_scaled_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_max_pt_scaled_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);

      h = new TH1D(Form("h_jet_max_et_raw_trig_%d", j), ";E_{T}; count", 200, 0, 100);
      hm->registerHisto(h);
      h = new TH1D(Form("h_jet_max_pt_raw_trig_%d", j), ";p_{T}; count", 200, 0, 100);
      hm->registerHisto(h);

    }
  h = new TH1D("h_cluster_et_mb", ";E_{T}; count", 200, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_cluster_pt_mb", ";p_{T}; count", 200, 0, 50);
  hm->registerHisto(h);

  h = new TH1D("h_cluster_et_scaled_mb", ";E_{T}; count", 200, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_cluster_pt_scaled_mb", ";p_{T}; count", 200, 0, 50);
  hm->registerHisto(h);

  h2 = new TH2D("h_2d_cluster_dist_mb",";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
  hm->registerHisto(h2);

  h = new TH1D("h_cluster_max_et_mb", ";E_{T}; count", 200, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_cluster_max_pt_mb", ";p_{T}; count", 200, 0, 50);
  hm->registerHisto(h);

  h = new TH1D("h_cluster_max_et_scaled_mb", ";E_{T}; count", 200, 0, 50);
  hm->registerHisto(h);
  h = new TH1D("h_cluster_max_pt_scaled_mb", ";p_{T}; count", 200, 0, 50);
  hm->registerHisto(h);

  h2 = new TH2D("h_2d_cluster_max_dist_mb",";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
  hm->registerHisto(h2);

  for (int j = 0; j < 4; j++)
    {
      h2 = new TH2D(Form("h_2d_cluster_dist_trig_%d", j),";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
      hm->registerHisto(h2);


      h = new TH1D(Form("h_cluster_et_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_pt_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_cluster_et_scaled_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_pt_scaled_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_cluster_et_raw_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_pt_raw_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      //Maximum
      h2 = new TH2D(Form("h_2d_cluster_max_dist_trig_%d", j),";#eta;#phi", 24, -1.1, 1.1, 64, -1*TMath::Pi(), TMath::Pi());
      hm->registerHisto(h2);


      h = new TH1D(Form("h_cluster_max_et_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_max_pt_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_cluster_max_et_scaled_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_max_pt_scaled_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);

      h = new TH1D(Form("h_cluster_max_et_raw_trig_%d", j), ";E_{T}; count", 200, 0, 50);
      hm->registerHisto(h);
      h = new TH1D(Form("h_cluster_max_pt_raw_trig_%d", j), ";p_{T}; count", 200, 0, 50);
      hm->registerHisto(h);

    }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TriggerPerformance::InitRun(PHCompositeNode *topNode)
{
  if (m_useEmulator)
    {
      std::cout << "using emulator " <<std::endl;
      m_mbd_prescale = 1;
    }
  else
    {
      triggeranalyzer->decodeTriggers(topNode);
      m_mbd_prescale = triggeranalyzer->getTriggerPrescale("MBD N&S >= 1") + 1;  
    }
  return Fun4AllReturnCodes::EVENT_OK;

}

int TriggerPerformance::process_event(PHCompositeNode *topNode)
{


  _i_event++;
  ///////// FOR CALO CALIB ///////////
  triggeranalyzer->decodeTriggers(topNode);
  if (Verbosity())
    {
      triggeranalyzer->Print();
    }
  std::string recoJetName = "AntiKt_Tower_r04_Sub1";

  JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
  
  if (!jets_4)
    {
      std::cout << " Nothing in the jets"<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  GlobalVertexMap* vertexmap =  findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

  float vtx_z = 999;
  if (vertexmap && !vertexmap->empty())
    {
      GlobalVertex* vtx = vertexmap->begin()->second;
      if (vtx)
  	{
  	  vtx_z = vtx->get_z();
  	}
    }
  if (vtx_z == 999)
    {
      return Fun4AllReturnCodes::EVENT_OK;
    }

  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");

  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALIN");
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALOUT");
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC_RETOWER");

  float background_v2 = 0;
  float background_Psi2 = 0;
  bool has_tower_background = false;
  TowerBackground *towBack = findNode::getClass<TowerBackground>(topNode, "TowerInfoBackground_Sub2");
  if (towBack)
    {
      has_tower_background = true;
      background_v2 = towBack->get_v2();
      background_Psi2 = towBack->get_Psi2();
    }

  std::vector<jetty> jets{};
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
      struct jetty myjet;
      myjet.pt = jet->get_pt();
      myjet.eta = jet->get_eta();
      myjet.phi = jet->get_phi();
      myjet.et = jet_total_eT;
      myjet.emcal = eFrac_emcal;
      myjet.ihcal = eFrac_ihcal;
      myjet.ohcal = eFrac_ohcal;
      jets.push_back(myjet);
    }
  CLHEP::Hep3Vector vertex(0, 0, vtx_z);
  CLHEP::Hep3Vector max_cluster(0,0,0);
  std::vector<CLHEP::Hep3Vector> rawclusters{};
  if (clusters)
    {
     

      auto clusterrange = clusters->getClusters();
      for (auto iclus = clusterrange.first; iclus != clusterrange.second; ++iclus)
	{

	  RawCluster *cluster = iclus->second;
	  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

	  float energy = E_vec_cluster.mag();

	  if (energy < 1) continue;
	  rawclusters.push_back(E_vec_cluster);
	  if (energy > max_cluster.mag()) max_cluster = E_vec_cluster;
	}
    }
  std::string histoname = "_";
  bool isMB = false;
  if (m_useEmulator) isMB = true;
  else
    {
      isMB = triggeranalyzer->didTriggerFire("MBD N&S >= 1");
    }
  if (rawclusters.size() > 0)
    {

      if (isMB)
	{
	  auto h_cluster_max_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_et_mb", histoname.c_str())));
	  h_cluster_max_et_mb->Fill(max_cluster.mag());
	  auto h_cluster_max_et_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_et_scaled_mb", histoname.c_str())));
	  h_cluster_max_et_scaled_mb->Fill(max_cluster.mag(), m_mbd_prescale);
	  auto h_cluster_max_dist_mb = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%scluster_max_dist_mb", histoname.c_str())));
	  h_cluster_max_dist_mb->Fill(max_cluster.pseudoRapidity(), max_cluster.phi());
	  auto h_cluster_max_pt_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_pt_mb", histoname.c_str())));
	  h_cluster_max_pt_mb->Fill(max_cluster.perp());
	  auto h_cluster_max_pt_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_pt_scaled_mb", histoname.c_str())));
	  h_cluster_max_pt_scaled_mb->Fill(max_cluster.perp(), m_mbd_prescale);

	  auto h_cluster_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_et_mb", histoname.c_str())));
	  auto h_cluster_pt_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_pt_mb", histoname.c_str())));
	  auto h_cluster_et_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_et_scaled_mb", histoname.c_str())));
	  auto h_cluster_pt_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_pt_scaled_mb", histoname.c_str())));
	  auto h_cluster_dist_mb = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%scluster_dist_mb", histoname.c_str())));

	  for (auto cluster : rawclusters)
	    {
	      h_cluster_et_mb->Fill(cluster.mag());
	      h_cluster_et_scaled_mb->Fill(cluster.mag(), m_mbd_prescale);
	      h_cluster_dist_mb->Fill(cluster.pseudoRapidity(), cluster.phi());
	      h_cluster_pt_mb->Fill(cluster.perp());
	      h_cluster_pt_scaled_mb->Fill(cluster.perp(), m_mbd_prescale);	
	    }
	  for (int j = 0; j < 4; j++)
	    {
	      bool firerare = false;

	      ///////// FOR CALO CALIB ///////////
	      if (m_useEmulator)
		firerare = triggeranalyzer->didTriggerFire(m_photon_emu_triggernames[j]);
	      else
		firerare = triggeranalyzer->checkRawTrigger(m_photon_triggernames[j]);
	      if (firerare)
		{
		  auto h_cluster_max_et_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_et_raw_trig_%d", histoname.c_str(), j)));
		  h_cluster_max_et_raw_trig->Fill(max_cluster.mag());
		  auto h_cluster_max_pt_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_pt_raw_trig_%d", histoname.c_str(), j)));
		  h_cluster_max_pt_raw_trig->Fill(max_cluster.perp());
		  
		  auto h_cluster_et_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_et_raw_trig_%d", histoname.c_str(), j)));
		  auto h_cluster_pt_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_pt_raw_trig_%d", histoname.c_str(), j)));
		  for (auto &cluster : rawclusters)
		    {
		      h_cluster_et_raw_trig->Fill(cluster.mag());
		      h_cluster_pt_raw_trig->Fill(cluster.perp());
		    }
		  
		}
	      
	    }
	}
      
      for (int j = 0; j < 4; j++)
	{
	  if (m_useEmulator) continue;
	  if (triggeranalyzer->didTriggerFire(m_photon_triggernames[j]))
	    {
	      if ( m_photon_prescales[j] == -1 ) m_photon_prescales[j] = triggeranalyzer->getTriggerPrescale(m_photon_triggernames[j]) + 1;

	      auto h_cluster_max_et_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_et_trig_%d", histoname.c_str(), j)));
	      h_cluster_max_et_trig->Fill(max_cluster.mag());
	      auto h_cluster_max_et_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_et_scaled_trig_%d", histoname.c_str(), j)));
	      h_cluster_max_et_scaled_trig->Fill(max_cluster.mag(), m_photon_prescales[j]);	
	      auto h_cluster_max_dist_trig = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%scluster_max_dist_trig_%d", histoname.c_str(), j)));
	      h_cluster_max_dist_trig->Fill(max_cluster.pseudoRapidity(), max_cluster.phi());
	      
	      auto h_cluster_max_pt_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_pt_trig_%d", histoname.c_str(), j)));
	      h_cluster_max_pt_trig->Fill(max_cluster.perp());
	      auto h_cluster_max_pt_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_max_pt_scaled_trig_%d", histoname.c_str(), j)));
	      h_cluster_max_pt_scaled_trig->Fill(max_cluster.perp(), m_photon_prescales[j]);
	  
	  
	      auto h_cluster_et_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_et_trig_%d", histoname.c_str(), j)));
	      auto h_cluster_pt_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_pt_trig_%d", histoname.c_str(), j)));

	      auto h_cluster_et_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_et_scaled_trig_%d", histoname.c_str(), j)));
	      auto h_cluster_pt_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%scluster_pt_scaled_trig_%d", histoname.c_str(), j)));

	      auto h_cluster_dist_trig = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%scluster_dist_trig_%d", histoname.c_str(), j)));

	      for (auto &cluster : rawclusters)
		{
		  h_cluster_et_trig->Fill(cluster.mag());
		  h_cluster_et_scaled_trig->Fill(cluster.mag(), m_photon_prescales[j]);
		  h_cluster_dist_trig->Fill(cluster.pseudoRapidity(), cluster.phi());
		  
		  h_cluster_pt_trig->Fill(cluster.perp());
		  h_cluster_pt_scaled_trig->Fill(cluster.perp(), m_photon_prescales[j]);
		}
	    }
	}  
    }      
  
  if (jets.size() == 0) return Fun4AllReturnCodes::EVENT_OK;
  
  auto pt_ordered = jets;
  std::sort(jets.begin(), jets.end(), [](auto a, auto b) { return a.et > b.et; });
  std::sort(pt_ordered.begin(), pt_ordered.end(), [](auto a, auto b) { return a.pt > b.pt; });


  float max_et = jets.at(0).et;
  float max_pt = pt_ordered.at(0).pt;
  float max_eta = jets.at(0).eta;
  float max_phi = jets.at(0).phi;
  

  if (isMB)
    {

      if (max_et > 4)
	{
	  auto h_jet_max_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_et_mb", histoname.c_str())));
	  h_jet_max_et_mb->Fill(max_et);
	  auto h_jet_max_et_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_et_scaled_mb", histoname.c_str())));
	  h_jet_max_et_scaled_mb->Fill(max_et, m_mbd_prescale);

	  auto h_jet_max_dist_mb = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%sjet_max_dist_mb", histoname.c_str())));
	  h_jet_max_dist_mb->Fill(max_eta, max_phi);

	}
      if (max_pt > 4)
	{
	  auto h_jet_max_pt_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_pt_mb", histoname.c_str())));
	  h_jet_max_pt_mb->Fill(max_pt);
	  auto h_jet_max_pt_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_pt_scaled_mb", histoname.c_str())));
	  h_jet_max_pt_scaled_mb->Fill(max_pt, m_mbd_prescale);

	}

      auto h_jet_et_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_et_mb", histoname.c_str())));
      auto h_jet_pt_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_pt_mb", histoname.c_str())));
      auto h_jet_et_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_et_scaled_mb", histoname.c_str())));
      auto h_jet_pt_scaled_mb = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_pt_scaled_mb", histoname.c_str())));
      auto h_jet_dist_mb = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%sjet_dist_mb", histoname.c_str())));

      for (auto &jet : jets)
 	{
	  if (max_et > 4)
	    {
	      h_jet_et_mb->Fill(jet.et);
	      h_jet_et_scaled_mb->Fill(jet.et, m_mbd_prescale);
	      h_jet_dist_mb->Fill(jet.eta, jet.phi);
	    }
	  if (max_et > 4)
	    {
	      h_jet_pt_mb->Fill(jet.pt);
	      h_jet_pt_scaled_mb->Fill(jet.pt, m_mbd_prescale);
	    }
	}
      for (int j = 0; j < 4; j++)
	{
	  bool firerare = false;

	  ///////// FOR CALO CALIB ///////////
	  if (m_useEmulator)
	    firerare = triggeranalyzer->didTriggerFire(m_jet_emu_triggernames[j]);
	  else
	    firerare = triggeranalyzer->checkRawTrigger(m_jet_triggernames[j]);
	  if (firerare)
	    {
	      if (max_et > 4)
		{
		  auto h_jet_max_et_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_et_raw_trig_%d", histoname.c_str(), j)));
		  h_jet_max_et_raw_trig->Fill(max_et);
		}
	      if (max_pt > 4)
		{

		  auto h_jet_max_pt_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_pt_raw_trig_%d", histoname.c_str(), j)));
		  h_jet_max_pt_raw_trig->Fill(max_pt);
		}

	      auto h_jet_et_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_et_raw_trig_%d", histoname.c_str(), j)));
	      auto h_jet_pt_raw_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_pt_raw_trig_%d", histoname.c_str(), j)));
	      for (auto &jet : jets)
		{
		  if (max_et > 4)
		    h_jet_et_raw_trig->Fill(jet.et);
		  if (max_pt > 4)
		    h_jet_pt_raw_trig->Fill(jet.pt);
		}

	    }

	}
    }

  for (int j = 0; j < 4; j++)
    {
      if (m_useEmulator) continue;
      if (triggeranalyzer->didTriggerFire(m_jet_triggernames[j]))
	{
	  if ( m_jet_prescales[j] == -1 ) m_jet_prescales[j] = triggeranalyzer->getTriggerPrescale(m_jet_triggernames[j]) + 1;

	  if (max_et > 4)
	    {
	      auto h_jet_max_et_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_et_trig_%d", histoname.c_str(), j)));
	      h_jet_max_et_trig->Fill(max_et);
	      auto h_jet_max_et_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_et_scaled_trig_%d", histoname.c_str(), j)));
	      h_jet_max_et_scaled_trig->Fill(max_et, m_jet_prescales[j]);	
	      auto h_jet_max_dist_trig = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%sjet_max_dist_trig_%d", histoname.c_str(), j)));
	      h_jet_max_dist_trig->Fill(max_eta, max_phi);
	    }
	  if (max_pt > 4)
	    {
	      auto h_jet_max_pt_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_pt_trig_%d", histoname.c_str(), j)));
	      h_jet_max_pt_trig->Fill(max_pt);
	      auto h_jet_max_pt_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_max_pt_scaled_trig_%d", histoname.c_str(), j)));
	      h_jet_max_pt_scaled_trig->Fill(max_pt, m_jet_prescales[j]);
	    }

	  auto h_jet_et_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_et_trig_%d", histoname.c_str(), j)));
	  auto h_jet_pt_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_pt_trig_%d", histoname.c_str(), j)));

	  auto h_jet_et_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_et_scaled_trig_%d", histoname.c_str(), j)));
	  auto h_jet_pt_scaled_trig = dynamic_cast<TH1*>(hm->getHisto(Form("h%sjet_pt_scaled_trig_%d", histoname.c_str(), j)));

	  auto h_jet_dist_trig = dynamic_cast<TH2*>(hm->getHisto(Form("h_2d%sjet_dist_trig_%d", histoname.c_str(), j)));

	  for (auto &jet : jets)
	    {
	      if (max_et > 4)
		{
		  h_jet_et_trig->Fill(jet.et);
		  h_jet_et_scaled_trig->Fill(jet.et, m_jet_prescales[j]);
		  h_jet_dist_trig->Fill(jet.eta, jet.phi);
		}
	      if (max_pt > 4)
		{
		  h_jet_pt_trig->Fill(jet.pt);
		  h_jet_pt_scaled_trig->Fill(jet.pt, m_jet_prescales[j]);
		}
	    }
 	}
    }  
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}



void TriggerPerformance::GetNodes(PHCompositeNode* topNode)
{


}

int TriggerPerformance::ResetEvent(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TriggerPerformance::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TriggerPerformance::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "TriggerPerformance::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  hm->dumpHistos(_foutname.c_str());
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TriggerPerformance::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "TriggerPerformance::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}
