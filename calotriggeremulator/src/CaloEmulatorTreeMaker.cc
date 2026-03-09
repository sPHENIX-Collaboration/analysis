#include "CaloEmulatorTreeMaker.h"
//for emc clusters
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <TMath.h>
#include <calotrigger/TriggerRunInfov1.h>

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
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
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
CaloEmulatorTreeMaker::CaloEmulatorTreeMaker(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  isSim = false;
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
}

//____________________________________________________________________________..
CaloEmulatorTreeMaker::~CaloEmulatorTreeMaker()
{

}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::Init(PHCompositeNode *topNode)
{


  if (isSim)
    {
      pt_cut = 1.;
    }
  triggeranalyzer = new TriggerAnalyzer();
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  _f = new TFile( _foutname.c_str(), "RECREATE");

  std::cout << " making a file = " <<  _foutname.c_str() << " , _f = " << _f << std::endl;
  
  _tree = new TTree("ttree","a persevering date tree");

  if (isSim)
    {
      // _tree->Branch("_truth_particle_n", &b_truth_particle_n);
      // _tree->Branch("_truth_particle_pid",&b_truth_particle_pid);
      // _tree->Branch("_truth_particle_pt",&b_truth_particle_pt);
      // _tree->Branch("_truth_particle_eta",&b_truth_particle_eta);
      // _tree->Branch("_truth_particle_phi",&b_truth_particle_phi);

      _tree->Branch("truth_jet_pt_2",&b_truth_jet_pt_2);
      _tree->Branch("truth_jet_eta_2",&b_truth_jet_eta_2);
      _tree->Branch("truth_jet_phi_2",&b_truth_jet_phi_2);
      _tree->Branch("truth_jet_pt_4",&b_truth_jet_pt_4);
      _tree->Branch("truth_jet_eta_4",&b_truth_jet_eta_4);
      _tree->Branch("truth_jet_phi_4",&b_truth_jet_phi_4);
      _tree->Branch("truth_jet_pt_6",&b_truth_jet_pt_6);
      _tree->Branch("truth_jet_eta_6",&b_truth_jet_eta_6);
      _tree->Branch("truth_jet_phi_6",&b_truth_jet_phi_6);

    }

  if (useLL1)
    {

    }
  _i_event = 0;
  _tree->Branch("emcal_good",&b_emcal_good);
  _tree->Branch("emcal_energy",&b_emcal_energy);
  _tree->Branch("emcal_time",&b_emcal_time);
  _tree->Branch("emcal_phibin",&b_emcal_phibin);
  _tree->Branch("emcal_etabin",&b_emcal_etabin);
  _tree->Branch("hcalin_good",&b_hcalin_good);
  _tree->Branch("hcalin_energy",&b_hcalin_energy);
  _tree->Branch("hcalin_time",&b_hcalin_time);
  _tree->Branch("hcalin_phibin",&b_hcalin_phibin);
  _tree->Branch("hcalin_etabin",&b_hcalin_etabin);
  _tree->Branch("hcalout_good",&b_hcalout_good);
  _tree->Branch("hcalout_energy",&b_hcalout_energy);
  _tree->Branch("hcalout_time",&b_hcalout_time);
  _tree->Branch("hcalout_phibin",&b_hcalout_phibin);
  _tree->Branch("hcalout_etabin",&b_hcalout_etabin);

  _tree->Branch("mbd_vertex_z", &b_vertex_z, "mbd_vertex_z/F");

  _tree->Branch("njet_2", &b_njet_2, "njet_2/I");
  _tree->Branch("jet_pt_2", &b_jet_pt_2);
  _tree->Branch("jet_et_2", &b_jet_et_2);
  _tree->Branch("jet_eta_2", &b_jet_eta_2);
  _tree->Branch("jet_phi_2", &b_jet_phi_2);
  _tree->Branch("jet_emcal_2", &b_jet_emcal_2);
  _tree->Branch("jet_hcalout_2", &b_jet_hcalout_2);
  _tree->Branch("jet_hcalin_2", &b_jet_hcalin_2);

  _tree->Branch("njet_4", &b_njet_4, "njet_4/I");
  _tree->Branch("jet_pt_4", &b_jet_pt_4);
  _tree->Branch("jet_et_4", &b_jet_et_4);
  _tree->Branch("jet_eta_4", &b_jet_eta_4);
  _tree->Branch("jet_phi_4", &b_jet_phi_4);
  _tree->Branch("jet_emcal_4", &b_jet_emcal_4);
  _tree->Branch("jet_hcalout_4", &b_jet_hcalout_4);
  _tree->Branch("jet_hcalin_4", &b_jet_hcalin_4);

  _tree->Branch("njet_6", &b_njet_6, "njet_6/I");
  _tree->Branch("jet_pt_6", &b_jet_pt_6);
  _tree->Branch("jet_et_6", &b_jet_et_6);
  _tree->Branch("jet_eta_6", &b_jet_eta_6);
  _tree->Branch("jet_phi_6", &b_jet_phi_6);
  _tree->Branch("jet_emcal_6", &b_jet_emcal_6);
  _tree->Branch("jet_hcalout_6", &b_jet_hcalout_6);
  _tree->Branch("jet_hcalin_6", &b_jet_hcalin_6);

  std::cout << "Done initing the treemaker"<<std::endl;  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void CaloEmulatorTreeMaker::SetVerbosity(int verbo){
  _verbosity = verbo;
  return;
}

void CaloEmulatorTreeMaker::reset_tree_vars()
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;

  b_njet_2 = 0;
  b_jet_pt_2.clear();
  b_jet_et_2.clear();
  b_jet_eta_2.clear();
  b_jet_phi_2.clear();
  b_jet_emcal_2.clear();
  b_jet_hcalin_2.clear();
  b_jet_hcalout_2.clear();
  b_njet_4 = 0;
  b_jet_pt_4.clear();
  b_jet_et_4.clear();
  b_jet_eta_4.clear();
  b_jet_phi_4.clear();
  b_jet_emcal_4.clear();
  b_jet_hcalin_4.clear();
  b_jet_hcalout_4.clear();

  b_njet_6 = 0;
  b_jet_pt_6.clear();
  b_jet_et_6.clear();
  b_jet_eta_6.clear();
  b_jet_phi_6.clear();
  b_jet_emcal_6.clear();
  b_jet_hcalin_6.clear();
  b_jet_hcalout_6.clear();

  b_emcal_good.clear();
  b_emcal_energy.clear();
  b_emcal_time.clear();
  b_emcal_phibin.clear();
  b_emcal_etabin.clear();

  b_hcalin_good.clear();
  b_hcalin_energy.clear();
  b_hcalin_time.clear();
  b_hcalin_phibin.clear();
  b_hcalin_etabin.clear();

  b_hcalout_good.clear();  
  b_hcalout_energy.clear();
  b_hcalout_time.clear();
  b_hcalout_phibin.clear();
  b_hcalout_etabin.clear();

  if (isSim)
    {
      b_ntruth_jet_2 = 0;
      b_truth_jet_pt_2.clear();
      b_truth_jet_eta_2.clear();
      b_truth_jet_phi_2.clear();
      b_ntruth_jet_4 = 0;
      b_truth_jet_pt_4.clear();
      b_truth_jet_eta_4.clear();
      b_truth_jet_phi_4.clear();
      b_ntruth_jet_6 = 0;
      b_truth_jet_pt_6.clear();
      b_truth_jet_eta_6.clear();
      b_truth_jet_phi_6.clear();
    }

  return;
}

int CaloEmulatorTreeMaker::process_event(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;

  _i_event++;

  reset_tree_vars();



  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  if (isSim)
    {

      std::string truthJetName = "AntiKt_Truth_r02";
      JetContainer *jetstruth_2 = findNode::getClass<JetContainer>(topNode, truthJetName);
      if (jetstruth_2)
	{
	  // zero out counters
	  
	  for (auto jet : *jetstruth_2)
	    {
	      if (jet->get_pt() < pt_cut_truth) continue;

	      b_ntruth_jet_2++;
	      b_truth_jet_pt_2.push_back(jet->get_pt());
	      b_truth_jet_eta_2.push_back(jet->get_eta());
	      b_truth_jet_phi_2.push_back(jet->get_phi());
	    }
	}
      truthJetName = "AntiKt_Truth_r04";
      JetContainer *jetstruth_4 = findNode::getClass<JetContainer>(topNode, truthJetName);
      if (jetstruth_4)
	{
	  // zero out counters

	  for (auto jet : *jetstruth_4)
	    {
	      if (jet->get_pt() < pt_cut_truth) continue;

	      b_ntruth_jet_4++;
	      b_truth_jet_pt_4.push_back(jet->get_pt());
	      b_truth_jet_eta_4.push_back(jet->get_eta());
	      b_truth_jet_phi_4.push_back(jet->get_phi());
	    }
	}
      truthJetName = "AntiKt_Truth_r06";
      JetContainer *jetstruth_6 = findNode::getClass<JetContainer>(topNode, truthJetName);
      if (jetstruth_6)
	{
	  // zero out counters

	  for (auto jet : *jetstruth_6)
	    {
	      if (jet->get_pt() < pt_cut_truth) continue;

	      b_ntruth_jet_6++;
	      b_truth_jet_pt_6.push_back(jet->get_pt());
	      b_truth_jet_eta_6.push_back(jet->get_eta());
	      b_truth_jet_phi_6.push_back(jet->get_phi());
	    }
	}
    }

  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "RealVertexMap");
  if (!vertexmap)
    {
      vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
    }

  float vtx_z = 0;
  b_vertex_z = -999;
  if (vertexmap && !vertexmap->empty())
    {
      GlobalVertex* vtx = vertexmap->begin()->second;
      if (vtx)
  	{
  	  vtx_z = vtx->get_z();
  	  b_vertex_z = vtx_z;
  	}
    }


  TowerInfoContainer *hcalin_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALIN");
  if (!hcalin_towers)
    {
      std::cout << "no hcalin towers "<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  int size;
  if (hcalin_towers)
    {

      size = hcalin_towers->size(); //online towers should be the same!
      for (int channel = 0; channel < size;channel++)
	{
	  _tower = hcalin_towers->get_tower_at_channel(channel);
	  float energy = _tower->get_energy();
	  float time = _tower->get_time();	  
	  short good = (_tower->get_isGood() ? 1:0);
	  unsigned int towerkey = hcalin_towers->encode_key(channel);
	  int ieta = hcalin_towers->getTowerEtaBin(towerkey);
	  int iphi = hcalin_towers->getTowerPhiBin(towerkey);

	  b_hcalin_good.push_back(good);
	  b_hcalin_energy.push_back(energy);
	  b_hcalin_time.push_back(time);
	  b_hcalin_etabin.push_back(ieta);
	  b_hcalin_phibin.push_back(iphi);
	}
    }
  TowerInfoContainer *hcalout_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_HCALOUT");
  if (!hcalout_towers)
    {
      std::cout << "no hcalout towers "<<std::endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }
  if (hcalout_towers)
    {

      size = hcalout_towers->size(); //online towers should be the same!
      for (int channel = 0; channel < size;channel++)
	{
	  _tower = hcalout_towers->get_tower_at_channel(channel);
	  float energy = _tower->get_energy();
	  float time = _tower->get_time();	  
	  unsigned int towerkey = hcalout_towers->encode_key(channel);
	  int ieta = hcalout_towers->getTowerEtaBin(towerkey);
	  int iphi = hcalout_towers->getTowerPhiBin(towerkey);
	  short good = (_tower->get_isGood() ? 1:0);
	  b_hcalout_good.push_back(good);
	  b_hcalout_energy.push_back(energy);
	  b_hcalout_time.push_back(time);
	  b_hcalout_etabin.push_back(ieta);
	  b_hcalout_phibin.push_back(iphi);
	}
    }
  TowerInfoContainer *emcalre_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC_RETOWER");
  TowerInfoContainer *emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, m_calo_nodename + "_CEMC");
  if (emcal_towers)
    {
      size = emcal_towers->size(); //online towers should be the same!
      for (int channel = 0; channel < size;channel++)
	{
	  _tower = emcal_towers->get_tower_at_channel(channel);
	  float energy = _tower->get_energy();
	  float time = _tower->get_time();
	  unsigned int towerkey = emcal_towers->encode_key(channel);
	  int ieta = emcal_towers->getTowerEtaBin(towerkey);
	  int iphi = emcal_towers->getTowerPhiBin(towerkey);
	  short good = (_tower->get_isGood() ? 1:0);
	  b_emcal_good.push_back(good);
	  b_emcal_energy.push_back(energy);
	  b_emcal_time.push_back(time);
	  b_emcal_etabin.push_back(ieta);
	  b_emcal_phibin.push_back(iphi);
	}
    }

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

   std::string recoJetName = "AntiKt_Tower_r02_Sub1";
   JetContainer *jets_2 = findNode::getClass<JetContainer>(topNode, recoJetName);
   if (jets_2)
     {
       // zero out counters

       for (auto jet : *jets_2)
	 {
	   if (jet->get_pt() < pt_cut) continue;

	   int n_comp_total = 0;
	   int n_comp_ihcal = 0;
	   int n_comp_ohcal = 0;
	   int n_comp_emcal = 0;

	   float jet_total_eT = 0;
	   float eFrac_ihcal = 0;
	   float eFrac_ohcal = 0;
	   float eFrac_emcal = 0;
	   float eFrac_emcal_ind = 0;

	   float emcal_tower_max = 0;
	   b_njet_2++;
	   b_jet_pt_2.push_back(jet->get_pt());
	   b_jet_eta_2.push_back(jet->get_eta());
	   b_jet_phi_2.push_back(jet->get_phi());
	    
	   for (int iem = 0; iem < (int) emcal_towers->size(); iem++)
	     {

	       unsigned int calokey = emcal_towers->encode_key(iem);
	       TowerInfo *tower = emcal_towers->get_tower_at_channel(iem);
	       if (!tower->get_isGood())
		 {
		   continue;
		 }
	       int ieta = emcal_towers->getTowerEtaBin(calokey);
	       int iphi = emcal_towers->getTowerPhiBin(calokey);
	       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
	       double tower_phi = tower_geomEM->get_tower_geometry(key)->get_phi();
	       double tower_eta = tower_geomEM->get_tower_geometry(key)->get_eta();

	       double dR = sqrt(TMath::Power(getDPHI(tower_phi, jet->get_phi()), 2) + TMath::Power(tower_eta - jet->get_eta(), 2));
	       if (dR > 0.2) continue;

	       float tower_eT = tower->get_energy() / std::cosh(tower_eta);

	       eFrac_emcal_ind += tower_eT;
	     }	   
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
		   n_comp_ihcal++;
		   n_comp_total++;
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
		   n_comp_ohcal++;
		   n_comp_total++;
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

		   if (tower_eT > emcal_tower_max)
		     {
		       emcal_tower_max = tower_eT;
		     }
		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   //	   std::cout << n_comp_total << " = " << n_comp_emcal << " " << n_comp_hcalout << " from " << 
	   eFrac_emcal /= jet_total_eT;
	   eFrac_ihcal /= jet_total_eT;
	   eFrac_ohcal /= jet_total_eT;

	   b_jet_et_2.push_back(jet_total_eT);
	   b_jet_emcal_2.push_back(eFrac_emcal);
	   b_jet_hcalin_2.push_back(eFrac_ihcal);
	   b_jet_hcalout_2.push_back(eFrac_ohcal);
	 }
     }

   recoJetName = "AntiKt_Tower_r04_Sub1";
   JetContainer *jets_4 = findNode::getClass<JetContainer>(topNode, recoJetName);
   if (jets_4)
     {
       // zero out counters

       for (auto jet : *jets_4)
	 {
	   if (jet->get_pt() < pt_cut) continue;

	   int n_comp_total = 0;
	   int n_comp_ihcal = 0;
	   int n_comp_ohcal = 0;
	   int n_comp_emcal = 0;

	   float jet_total_eT = 0;
	   float eFrac_ihcal = 0;
	   float eFrac_ohcal = 0;
	   float eFrac_emcal = 0;
	   float eFrac_emcal_ind = 0;

	   float emcal_tower_max = 0;
	   b_njet_4++;
	   b_jet_pt_4.push_back(jet->get_pt());
	   b_jet_eta_4.push_back(jet->get_eta());
	   b_jet_phi_4.push_back(jet->get_phi());

	   for (int iem = 0; iem < (int) emcal_towers->size(); iem++)
	     {

	       unsigned int calokey = emcal_towers->encode_key(iem);
	       TowerInfo *tower = emcal_towers->get_tower_at_channel(iem);
	       if (!tower->get_isGood())
		 {
		   continue;
		 }
	       int ieta = emcal_towers->getTowerEtaBin(calokey);
	       int iphi = emcal_towers->getTowerPhiBin(calokey);
	       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
	       double tower_phi = tower_geomEM->get_tower_geometry(key)->get_phi();
	       double tower_eta = tower_geomEM->get_tower_geometry(key)->get_eta();

	       double dR = sqrt(TMath::Power(getDPHI(tower_phi, jet->get_phi()), 2) + TMath::Power(tower_eta - jet->get_eta(), 2));
	       if (dR > 0.4) continue;

	       float tower_eT = tower->get_energy() / std::cosh(tower_eta);

	       eFrac_emcal_ind += tower_eT;
	     }	   
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
		   n_comp_ihcal++;
		   n_comp_total++;
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
		   n_comp_ohcal++;
		   n_comp_total++;
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

		   if (tower_eT > emcal_tower_max)
		     {
		       emcal_tower_max = tower_eT;
		     }
		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   //	   std::cout << n_comp_total << " = " << n_comp_emcal << " " << n_comp_hcalout << " from " << 
	   emcal_tower_max /= eFrac_emcal;

	   b_jet_et_4.push_back(jet_total_eT);
	   b_jet_emcal_4.push_back(eFrac_emcal);
	   b_jet_hcalin_4.push_back(eFrac_ihcal);
	   b_jet_hcalout_4.push_back(eFrac_ohcal);

	 }
     }


   recoJetName = "AntiKt_Tower_r06_Sub1";
   JetContainer *jets_6 = findNode::getClass<JetContainer>(topNode, recoJetName);
   if (jets_6)
     {
       // zero out counters

       for (auto jet : *jets_6)
	 {
	   if (jet->get_pt() < pt_cut) continue;

	   int n_comp_total = 0;
	   int n_comp_ihcal = 0;
	   int n_comp_ohcal = 0;
	   int n_comp_emcal = 0;

	   float jet_total_eT = 0;
	   float eFrac_ihcal = 0;
	   float eFrac_ohcal = 0;
	   float eFrac_emcal = 0;
	   float eFrac_emcal_ind = 0;

	   float emcal_tower_max = 0;
	   b_njet_6++;
	   b_jet_pt_6.push_back(jet->get_pt());
	   b_jet_eta_6.push_back(jet->get_eta());
	   b_jet_phi_6.push_back(jet->get_phi());

	   for (int iem = 0; iem < (int) emcal_towers->size(); iem++)
	     {

	       unsigned int calokey = emcal_towers->encode_key(iem);
	       TowerInfo *tower = emcal_towers->get_tower_at_channel(iem);
	       if (!tower->get_isGood())
		 {
		   continue;
		 }
	       int ieta = emcal_towers->getTowerEtaBin(calokey);
	       int iphi = emcal_towers->getTowerPhiBin(calokey);
	       const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
	       double tower_phi = tower_geomEM->get_tower_geometry(key)->get_phi();
	       double tower_eta = tower_geomEM->get_tower_geometry(key)->get_eta();

	       double dR = sqrt(TMath::Power(getDPHI(tower_phi, jet->get_phi()), 2) + TMath::Power(tower_eta - jet->get_eta(), 2));
	       if (dR > 0.6) continue;

	       float tower_eT = tower->get_energy() / std::cosh(tower_eta);

	       eFrac_emcal_ind += tower_eT;
	     }	   
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
		   n_comp_ihcal++;
		   n_comp_total++;
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
		   n_comp_ohcal++;
		   n_comp_total++;
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

		   if (tower_eT > emcal_tower_max)
		     {
		       emcal_tower_max = tower_eT;
		     }
		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   //	   std::cout << n_comp_total << " = " << n_comp_emcal << " " << n_comp_hcalout << " from " << 
	   b_jet_et_6.push_back(jet_total_eT);
	   b_jet_emcal_6.push_back(eFrac_emcal);
	   b_jet_hcalin_6.push_back(eFrac_ihcal);
	   b_jet_hcalout_6.push_back(eFrac_ohcal);

	 }
     }


   // recoJetName = "AntiKt_Tower_r06_Sub1";
   // JetContainer *jets_6 = findNode::getClass<JetContainer>(topNode, recoJetName);
   // if (jets_6)
   //   {
   //     // zero out counters

       
   //     for (auto jet : *jets_6)
   // 	 {
   // 	   //	   if (jet->get_pt() < pt_cut) continue;
   // 	   int n_comp_total = 0;
   // 	   int n_comp_ihcal = 0;
   // 	   int n_comp_ohcal = 0;
   // 	   int n_comp_emcal = 0;

   // 	   float jet_total_eT = 0;
   // 	   float eFrac_ihcal = 0;
   // 	   float eFrac_ohcal = 0;
   // 	   float eFrac_emcal = 0;

   // 	   float emcal_tower_max = 0;
   // 	   b_njet_6++;
   // 	   b_jet_pt_6.push_back(jet->get_pt());
   // 	   b_jet_eta_6.push_back(jet->get_eta());
   // 	   b_jet_phi_6.push_back(jet->get_phi());
   // 	   b_jet_mass_6.push_back(jet->get_mass());
   // 	   for (auto comp : jet->get_comp_vec())
   // 	     {
   // 	       unsigned int channel = comp.second;
   // 	       TowerInfo *tower;

   // 	       float tower_eT = 0;
   // 	       if (comp.first == 26 || comp.first == 30)
   // 		 {  // IHcal
   // 		   tower = hcalin_towers->get_tower_at_channel(channel);
   // 		   if (!tower || !tower_geomIH)
   // 		     {
   // 		       continue;
   // 		     }
   // 		   if(!tower->get_isGood()) continue;

   // 		   unsigned int calokey = hcalin_towers->encode_key(channel);
   // 		   int ieta = hcalin_towers->getTowerEtaBin(calokey);
   // 		   int iphi = hcalin_towers->getTowerPhiBin(calokey);
   // 		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
   // 		   float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
   // 		   float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
   // 		   tower_eT = tower->get_energy() / std::cosh(tower_eta);

   // 		   if (comp.first == 30)
   // 		     {  // is sub1
   // 		       if (has_tower_background)
   // 			 {
   // 			   float UE = towBack->get_UE(1).at(ieta);
   // 			   float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
   // 			   tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
   // 			 }
   // 		     }

   // 		   eFrac_ihcal += tower_eT;
   // 		   jet_total_eT += tower_eT;
   // 		   n_comp_ihcal++;
   // 		   n_comp_total++;
   // 		 }
   // 	       else if (comp.first == 27 || comp.first == 31)
   // 		 {  // IHcal
   // 		   tower = hcalout_towers->get_tower_at_channel(channel);

   // 		   if (!tower || !tower_geomOH)
   // 		     {
   // 		       continue;
   // 		     }

   // 		   unsigned int calokey = hcalout_towers->encode_key(channel);
   // 		   int ieta = hcalout_towers->getTowerEtaBin(calokey);
   // 		   int iphi = hcalout_towers->getTowerPhiBin(calokey);
   // 		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
   // 		   float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
   // 		   float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();
   // 		   tower_eT = tower->get_energy() / std::cosh(tower_eta);

   // 		   if (comp.first == 31)
   // 		     {  // is sub1
   // 		       if (has_tower_background)
   // 			 {
   // 			   float UE = towBack->get_UE(2).at(ieta);
   // 			   float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
   // 			   tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
   // 			 }
   // 		     }

   // 		   eFrac_ohcal += tower_eT;
   // 		   jet_total_eT += tower_eT;
   // 		   n_comp_ohcal++;
   // 		   n_comp_total++;
   // 		 }
   // 	       else if (comp.first == 28 || comp.first == 29)
   // 		 {  // IHcal
   // 		   tower = emcalre_towers->get_tower_at_channel(channel);

   // 		   if (!tower || !tower_geomIH)
   // 		     {
   // 		       continue;
   // 		     }

   // 		   unsigned int calokey = emcalre_towers->encode_key(channel);
   // 		   int ieta = emcalre_towers->getTowerEtaBin(calokey);
   // 		   int iphi = emcalre_towers->getTowerPhiBin(calokey);
   // 		   const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
   // 		   float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
   // 		   float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();
   // 		   tower_eT = tower->get_energy() / std::cosh(tower_eta);

   // 		   if (comp.first == 29)
   // 		     {  // is sub1
   // 		       if (has_tower_background)
   // 			 {
   // 			   float UE = towBack->get_UE(0).at(ieta);
   // 			   float tower_UE = UE * (1 + 2 * background_v2 * std::cos(2 * (tower_phi - background_Psi2)));
   // 			   tower_eT = (tower->get_energy() - tower_UE) / std::cosh(tower_eta);
   // 			 }
   // 		     }

   // 		   if (tower_eT > emcal_tower_max)
   // 		     {
   // 		       emcal_tower_max = tower_eT;
   // 		     }
   // 		   eFrac_emcal += tower_eT;
   // 		   jet_total_eT += tower_eT;
   // 		   n_comp_emcal++;
   // 		   n_comp_total++;
   // 		 }	       
   // 	     }
   // 	   emcal_tower_max /= eFrac_emcal;
   // 	   eFrac_ihcal /= jet_total_eT;
   // 	   eFrac_ohcal /= jet_total_eT;
   // 	   eFrac_emcal /= jet_total_eT;

   // 	   b_jet_et_6.push_back(jet_total_eT);
   // 	   b_jet_emcal_tower_frac_6.push_back(emcal_tower_max);
   // 	   b_jet_emcal_frac_6.push_back(eFrac_emcal);
   // 	   b_jet_hcalin_frac_6.push_back(eFrac_ihcal);
   // 	   b_jet_hcalout_frac_6.push_back(eFrac_ohcal);

   // 	 }
   //   }
   if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;
   
   _tree->Fill();
   
   return Fun4AllReturnCodes::EVENT_OK;
}



void CaloEmulatorTreeMaker::GetNodes(PHCompositeNode* topNode)
{


}

int CaloEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int CaloEmulatorTreeMaker::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "CaloEmulatorTreeMaker::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

Double_t CaloEmulatorTreeMaker::getDPHI(Double_t phi1, Double_t phi2) {
  Double_t dphi = phi1 - phi2;

  //3.141592653589
  if ( dphi > TMath::Pi() )
    dphi = dphi - 2. * TMath::Pi();
  if ( dphi <= -TMath::Pi() )
    dphi = dphi + 2. * TMath::Pi();

  if ( TMath::Abs(dphi) > TMath::Pi() ) {
    std::cout << " commonUtility::getDPHI error!!! dphi is bigger than TMath::Pi() " << std::endl;
    std::cout << " " << phi1 << ", " << phi2 << ", " << dphi << std::endl;
  }

  return dphi;
}
