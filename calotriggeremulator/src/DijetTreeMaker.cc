#include "DijetTreeMaker.h"
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
DijetTreeMaker::DijetTreeMaker(const std::string &name, const std::string &outfilename):
  SubsysReco(name)
  
{
  isSim = false;
  _foutname = outfilename;  
  m_calo_nodename = "TOWERINFO_CALIB";
}

//____________________________________________________________________________..
DijetTreeMaker::~DijetTreeMaker()
{

}

//____________________________________________________________________________..
int DijetTreeMaker::Init(PHCompositeNode *topNode)
{


  if (isSim)
    {
      pt_cut = 1.;
    }


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

  _i_event = 0;
  if (save_calo)
    {
      _tree->Branch("emcal_good",&b_emcal_good);
      _tree->Branch("emcal_energy",&b_emcal_energy);
      _tree->Branch("emcal_time",&b_emcal_time);
      _tree->Branch("emcal_phibin",&b_emcal_phibin);
      _tree->Branch("emcal_etabin",&b_emcal_etabin);
      _tree->Branch("emcal_phi",&b_emcal_phi);
      _tree->Branch("emcal_eta",&b_emcal_eta);

      _tree->Branch("hcalin_good",&b_hcalin_good);
      _tree->Branch("hcalin_energy",&b_hcalin_energy);
      _tree->Branch("hcalin_time",&b_hcalin_time);
      _tree->Branch("hcalin_phibin",&b_hcalin_phibin);
      _tree->Branch("hcalin_etabin",&b_hcalin_etabin);
      _tree->Branch("hcalin_phi",&b_hcalin_phi);
      _tree->Branch("hcalin_eta",&b_hcalin_eta);

      _tree->Branch("hcalout_good",&b_hcalout_good);
      _tree->Branch("hcalout_energy",&b_hcalout_energy);
      _tree->Branch("hcalout_time",&b_hcalout_time);
      _tree->Branch("hcalout_phibin",&b_hcalout_phibin);
      _tree->Branch("hcalout_etabin",&b_hcalout_etabin);
      _tree->Branch("hcalout_phi",&b_hcalout_phi);
      _tree->Branch("hcalout_eta",&b_hcalout_eta);
    }

  _tree->Branch("mbd_vertex_z", &b_vertex_z, "mbd_vertex_z/F");

  _tree->Branch("njet_2", &b_njet_2, "njet_2/I");
  _tree->Branch("jet_pt_2", &b_jet_pt_2);
  _tree->Branch("jet_et_2", &b_jet_et_2);
  _tree->Branch("jet_eta_2", &b_jet_eta_2);
  _tree->Branch("jet_phi_2", &b_jet_phi_2);
  _tree->Branch("jet_emcal_2", &b_jet_emcal_2);
  _tree->Branch("jet_hcalout_2", &b_jet_hcalout_2);
  _tree->Branch("jet_hcalin_2", &b_jet_hcalin_2);
  _tree->Branch("jet_eccen_2", &b_jet_eccen_2);

  _tree->Branch("njet_4", &b_njet_4, "njet_4/I");
  _tree->Branch("jet_pt_4", &b_jet_pt_4);
  _tree->Branch("jet_et_4", &b_jet_et_4);
  _tree->Branch("jet_eta_4", &b_jet_eta_4);
  _tree->Branch("jet_phi_4", &b_jet_phi_4);
  _tree->Branch("jet_emcal_4", &b_jet_emcal_4);
  _tree->Branch("jet_hcalout_4", &b_jet_hcalout_4);
  _tree->Branch("jet_hcalin_4", &b_jet_hcalin_4);
  _tree->Branch("jet_eccen_4", &b_jet_eccen_4);

  _tree->Branch("njet_6", &b_njet_6, "njet_6/I");
  _tree->Branch("jet_pt_6", &b_jet_pt_6);
  _tree->Branch("jet_et_6", &b_jet_et_6);
  _tree->Branch("jet_eta_6", &b_jet_eta_6);
  _tree->Branch("jet_phi_6", &b_jet_phi_6);
  _tree->Branch("jet_emcal_6", &b_jet_emcal_6);
  _tree->Branch("jet_hcalout_6", &b_jet_hcalout_6);
  _tree->Branch("jet_hcalin_6", &b_jet_hcalin_6);
  _tree->Branch("jet_eccen_6", &b_jet_eccen_6);

  _tree->Branch("ncluster", &b_ncluster);
  _tree->Branch("cluster_ecore", &b_cluster_ecore);
  _tree->Branch("cluster_e", &b_cluster_e);
  _tree->Branch("cluster_pt", &b_cluster_pt);
  _tree->Branch("cluster_chi2", &b_cluster_chi2);
  _tree->Branch("cluster_prob", &b_cluster_prob);
  _tree->Branch("cluster_eta", &b_cluster_eta);
  _tree->Branch("cluster_phi", &b_cluster_phi);

  std::cout << "Done initing the treemaker"<<std::endl;  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetTreeMaker::InitRun(PHCompositeNode *topNode)
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

void DijetTreeMaker::SetVerbosity(int verbo){
  _verbosity = verbo;
  return;
}

void DijetTreeMaker::reset_tree_vars()
{
  if (Verbosity()) std::cout << __FUNCTION__ << __LINE__<<std::endl;

  b_ncluster = 0;
  b_cluster_e.clear();
  b_cluster_ecore.clear();
  b_cluster_eta.clear();
  b_cluster_phi.clear();
  b_cluster_pt.clear();
  b_cluster_chi2.clear();
  b_cluster_prob.clear();

  b_njet_2 = 0;
  b_jet_pt_2.clear();
  b_jet_et_2.clear();
  b_jet_eta_2.clear();
  b_jet_phi_2.clear();
  b_jet_emcal_2.clear();
  b_jet_hcalin_2.clear();
  b_jet_hcalout_2.clear();
  b_jet_eccen_2.clear();

  b_njet_4 = 0;
  b_jet_pt_4.clear();
  b_jet_et_4.clear();
  b_jet_eta_4.clear();
  b_jet_phi_4.clear();
  b_jet_emcal_4.clear();
  b_jet_hcalin_4.clear();
  b_jet_hcalout_4.clear();
  b_jet_eccen_4.clear();

  b_njet_6 = 0;
  b_jet_pt_6.clear();
  b_jet_et_6.clear();
  b_jet_eta_6.clear();
  b_jet_phi_6.clear();
  b_jet_emcal_6.clear();
  b_jet_hcalin_6.clear();
  b_jet_hcalout_6.clear();
  b_jet_eccen_4.clear();

  b_emcal_good.clear();
  b_emcal_energy.clear();
  b_emcal_time.clear();
  b_emcal_phibin.clear();
  b_emcal_etabin.clear();
  b_emcal_phi.clear();
  b_emcal_eta.clear();

  b_hcalin_good.clear();
  b_hcalin_energy.clear();
  b_hcalin_time.clear();
  b_hcalin_phibin.clear();
  b_hcalin_etabin.clear();
  b_hcalin_phi.clear();
  b_hcalin_eta.clear();

  b_hcalout_good.clear();  
  b_hcalout_energy.clear();
  b_hcalout_time.clear();
  b_hcalout_phibin.clear();
  b_hcalout_etabin.clear();
  b_hcalout_phi.clear();
  b_hcalout_eta.clear();

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

int DijetTreeMaker::process_event(PHCompositeNode *topNode)
{

  if (Verbosity()) std::cout << __FILE__ << " "<< __LINE__<<" "<<std::endl;

  int dijet_candidate = false;
  _i_event++;

  reset_tree_vars();

  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTERINFO_CEMC");

  RawTowerGeomContainer *tower_geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  RawTowerGeomContainer *tower_geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  RawTowerGeomContainer *tower_geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  float max_secondmax_et[2]={0};
  float max_secondmax_pt[2]={0};
  
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
	      for (int im = 0; im < 2; im++)
		{
		  if (jet->get_pt() > max_secondmax_pt[im])
		    {
		      if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		      max_secondmax_pt[im] = jet->get_pt();
		    }
		}
	      
	    }
     
	  if (max_secondmax_pt[0] > 10 && max_secondmax_pt[1] > 5) dijet_candidate = true;
	  for (int im = 0; im < 2; im++)
	    {
	      max_secondmax_pt[im]=0;
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
	      for (int im = 0; im < 2; im++)
		{
		  if (jet->get_pt() > max_secondmax_pt[im])
		    {
		      if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		      max_secondmax_pt[im] = jet->get_pt();
		    }
		}
	      
	    }
     
	  if (max_secondmax_pt[0] > 10 && max_secondmax_pt[1] > 5) dijet_candidate = true;
	  for (int im = 0; im < 2; im++)
	    {
	      max_secondmax_pt[im]=0;
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
	      for (int im = 0; im < 2; im++)
		{
		  if (jet->get_pt() > max_secondmax_pt[im])
		    {
		      if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		      max_secondmax_pt[im] = jet->get_pt();
		    }
		}
	      
	    }
     
	  if (max_secondmax_pt[0] > 10 && max_secondmax_pt[1] > 5) dijet_candidate = true;
	  for (int im = 0; im < 2; im++)
	    {
	      max_secondmax_pt[im]=0;
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
  if (Verbosity() > 5) std::cout << "Mbd Vertex = " << b_vertex_z << std::endl;

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

	   b_njet_2++;
	   b_jet_pt_2.push_back(jet->get_pt());
	   b_jet_eta_2.push_back(jet->get_eta());
	   b_jet_phi_2.push_back(jet->get_phi());

	   std::vector<TVector3> hcal_constituents;
	    
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
		   TVector3 v;
		   v.SetPtEtaPhi(tower_eT, tower_eta, tower_phi);
		   hcal_constituents.push_back(v);
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

		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   //	   std::cout << n_comp_total << " = " << n_comp_emcal << " " << n_comp_hcalout << " from " << 
	   eFrac_emcal /= jet_total_eT;
	   eFrac_ihcal /= jet_total_eT;

	   float eccen = get_eccentricity(hcal_constituents, eFrac_ohcal);
	   eFrac_ohcal /= jet_total_eT;

	   b_jet_et_2.push_back(jet_total_eT);
	   b_jet_emcal_2.push_back(eFrac_emcal);
	   b_jet_hcalin_2.push_back(eFrac_ihcal);
	   b_jet_hcalout_2.push_back(eFrac_ohcal);
	   b_jet_eccen_2.push_back(eccen);
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet_total_eT > max_secondmax_et[im])
		 {
		   if (im == 0) max_secondmax_et[1] = max_secondmax_et[0];
		   max_secondmax_et[im] = jet_total_eT;
		 }
	     }
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet->get_pt() > max_secondmax_pt[im])
		 {
		   if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		   max_secondmax_pt[im] = jet->get_pt();
		 }
	     }

	 }
     }
   dijet_candidate = false;
   if (max_secondmax_et[0] > dijetcut) dijet_candidate = true;
   if (max_secondmax_pt[0] > dijetcut) dijet_candidate = true;
   for (int im = 0; im < 2; im++)
     {
       max_secondmax_et[im]=0;
       max_secondmax_pt[im]=0;
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

	   b_njet_4++;
	   b_jet_pt_4.push_back(jet->get_pt());
	   b_jet_eta_4.push_back(jet->get_eta());
	   b_jet_phi_4.push_back(jet->get_phi());

	   std::vector<TVector3> hcal_constituents;
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
		   TVector3 v;
		   v.SetPtEtaPhi(tower_eT, tower_eta, tower_phi);
		   hcal_constituents.push_back(v);

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

		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   float eccen = get_eccentricity(hcal_constituents, eFrac_ohcal);
	   eFrac_ohcal /= jet_total_eT;
	   eFrac_emcal /= jet_total_eT;
	   eFrac_ihcal /= jet_total_eT;

	   b_jet_et_4.push_back(jet_total_eT);
	   b_jet_emcal_4.push_back(eFrac_emcal);
	   b_jet_hcalin_4.push_back(eFrac_ihcal);
	   b_jet_hcalout_4.push_back(eFrac_ohcal);

	   b_jet_eccen_4.push_back(eccen);
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet_total_eT > max_secondmax_et[im])
		 {
		   if (im == 0) max_secondmax_et[1] = max_secondmax_et[0];
		   max_secondmax_et[im] = jet_total_eT;
		 }
	     }
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet->get_pt() > max_secondmax_pt[im])
		 {
		   if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		   max_secondmax_pt[im] = jet->get_pt();
		 }
	     }

	 }
     }

   if (max_secondmax_et[0] > dijetcut) dijet_candidate = true;
   if (max_secondmax_pt[0] > dijetcut) dijet_candidate = true;
   for (int im = 0; im < 2; im++)
     {
       max_secondmax_et[im]=0;
       max_secondmax_pt[im]=0;
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

	   b_njet_6++;
	   b_jet_pt_6.push_back(jet->get_pt());
	   b_jet_eta_6.push_back(jet->get_eta());
	   b_jet_phi_6.push_back(jet->get_phi());

	   std::vector<TVector3> hcal_constituents;
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
		   TVector3 v;
		   v.SetPtEtaPhi(tower_eT, tower_eta, tower_phi);
		   hcal_constituents.push_back(v);

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

		   eFrac_emcal += tower_eT;
		   jet_total_eT += tower_eT;
		   n_comp_emcal++;
		   n_comp_total++;
		 }	       
	     }
	   float eccen = get_eccentricity(hcal_constituents, eFrac_ohcal);
	   eFrac_ohcal /= jet_total_eT;
	   eFrac_emcal /= jet_total_eT;
	   eFrac_ihcal /= jet_total_eT;

	   b_jet_eccen_6.push_back(eccen);

	   b_jet_et_6.push_back(jet_total_eT);
	   b_jet_emcal_6.push_back(eFrac_emcal);
	   b_jet_hcalin_6.push_back(eFrac_ihcal);
	   b_jet_hcalout_6.push_back(eFrac_ohcal);
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet_total_eT > max_secondmax_et[im])
		 {
		   if (im == 0) max_secondmax_et[1] = max_secondmax_et[0];
		   max_secondmax_et[im] = jet_total_eT;
		 }
	     }
	   for (int im = 0; im < 2; im++)
	     {
	       if (jet->get_pt() > max_secondmax_pt[im])
		 {
		   if (im == 0) max_secondmax_pt[1] = max_secondmax_pt[0];
		   max_secondmax_pt[im] = jet->get_pt();
		 }
	     }

	 }
     }

   if (max_secondmax_et[0] > dijetcut) dijet_candidate = true;
   if (max_secondmax_pt[0] > dijetcut) dijet_candidate = true;

   for (int im = 0; im < 2; im++)
     {
       max_secondmax_et[im]=0;
       max_secondmax_pt[im]=0;
     }

   if (!dijet_candidate)
     {
       return Fun4AllReturnCodes::EVENT_OK;
     }

  int size;
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
	  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::CEMC, ieta, iphi);
	  float tower_phi = tower_geomEM->get_tower_geometry(key)->get_phi();
	  float tower_eta = tower_geomEM->get_tower_geometry(key)->get_eta();

	  b_emcal_good.push_back(good);
	  b_emcal_energy.push_back(energy);
	  b_emcal_time.push_back(time);
	  b_emcal_etabin.push_back(ieta);
	  b_emcal_phibin.push_back(iphi);
	  b_emcal_eta.push_back(tower_eta);
	  b_emcal_phi.push_back(tower_phi);
	}
    }

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
	  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALIN, ieta, iphi);
	  float tower_phi = tower_geomIH->get_tower_geometry(key)->get_phi();
	  float tower_eta = tower_geomIH->get_tower_geometry(key)->get_eta();

	  b_hcalin_good.push_back(good);
	  b_hcalin_energy.push_back(energy);
	  b_hcalin_time.push_back(time);
	  b_hcalin_etabin.push_back(ieta);
	  b_hcalin_phibin.push_back(iphi);
	  b_hcalin_eta.push_back(tower_eta);
	  b_hcalin_phi.push_back(tower_phi);
	}
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
	  const RawTowerDefs::keytype key = RawTowerDefs::encode_towerid(RawTowerDefs::CalorimeterId::HCALOUT, ieta, iphi);
	  float tower_phi = tower_geomOH->get_tower_geometry(key)->get_phi();
	  float tower_eta = tower_geomOH->get_tower_geometry(key)->get_eta();

	  b_hcalout_good.push_back(good);
	  b_hcalout_energy.push_back(energy);
	  b_hcalout_time.push_back(time);
	  b_hcalout_etabin.push_back(ieta);
	  b_hcalout_phibin.push_back(iphi);
	  b_hcalout_eta.push_back(tower_eta);
	  b_hcalout_phi.push_back(tower_phi);
	}
    }


  if (clusters)
    {
      CLHEP::Hep3Vector vertex(0, 0, b_vertex_z);

      auto clusterrange = clusters->getClusters();
      for (auto iclus = clusterrange.first; iclus != clusterrange.second; ++iclus)
	{

	  RawCluster *cluster = iclus->second;
	  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*cluster, vertex);

	  float energy = E_vec_cluster.mag();
	  float pt = E_vec_cluster.perp();
	  float clus_eta = E_vec_cluster.pseudoRapidity();
	  float clus_phi = E_vec_cluster.phi();
	  float chi2 = cluster->get_chi2();
	  float prob = cluster->get_prob();
	  float ecore = cluster->get_ecore();
	  if (energy < 0.5) continue;
	  b_ncluster++;
	  b_cluster_e.push_back(energy);
	  b_cluster_ecore.push_back(ecore);
	  b_cluster_eta.push_back(clus_eta);
	  b_cluster_phi.push_back(clus_phi);
	  b_cluster_pt.push_back(pt);
	  b_cluster_chi2.push_back(chi2);
	  b_cluster_prob.push_back(prob);
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



void DijetTreeMaker::GetNodes(PHCompositeNode* topNode)
{


}

int DijetTreeMaker::ResetEvent(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "DijetTreeMaker::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    }


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetTreeMaker::EndRun(const int runnumber)
{
  if (Verbosity() > 0)
    {
      std::cout << "DijetTreeMaker::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetTreeMaker::End(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "DijetTreeMaker::End(PHCompositeNode *topNode) This is the End..." << std::endl;
    }
  std::cout<<"Total events: "<<_i_event<<std::endl;
  _f->Write();
  _f->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int DijetTreeMaker::Reset(PHCompositeNode *topNode)
{
  if (Verbosity() > 0)
    {
      std::cout << "DijetTreeMaker::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    }
  return Fun4AllReturnCodes::EVENT_OK;
}

Double_t DijetTreeMaker::getDPHI(Double_t phi1, Double_t phi2) {
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

float DijetTreeMaker::get_eccentricity(std::vector<TVector3> hcaltowers, float oh_sum)
{
  float avg[2] = { 0 };
  for (auto &constituent : hcaltowers)
    {
      avg[0]+=constituent.Eta();
      avg[1]+=constituent.Phi();
    }

  avg[0]/=oh_sum;
  avg[1]/=oh_sum;

  float cov[2][2] = {0};
  float cov1[2][2] = {0};
  float cov2[2][2] = {0};

  for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
	{
	  for (auto &tower : hcaltowers)
	    {
	      float diff1 = 0;
	      float diff2 = 0;

	      if (i == 0)
		diff1 = tower.Eta();
	      else
		diff1 = tower.Phi();
	      if (j == 0)
		diff2 = tower.Eta();
	      else
		diff2 = tower.Phi();

	      cov[i][j] += tower.Pt()*diff1*diff2/oh_sum;
	      cov1[i][j] += tower.Pt()*diff1/oh_sum;
	      cov2[i][j] += tower.Pt()*diff2/oh_sum;

	    }

	  cov[i][j] -= cov1[i][j]*cov2[i][j];
	}
    }

  if (fabs(cov[0][0]) < fabs(cov[1][1])) return 0.0;

  float maxi = std::max(        cov[0][0]*cov[0][0] , cov[1][1]*cov[1][1] );
  float mini = std::min(        cov[0][0]*cov[0][0] , cov[1][1]*cov[1][1] );
  float eccen = sqrt(maxi - mini)/maxi;
  return eccen;
}
