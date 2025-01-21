#include "JESMCTreeGen.h"

// Vertex.
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

// Tower.
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfov1.h>
#include <calobase/TowerInfov2.h>
#include <calobase/TowerInfov3.h>
#include <calobase/TowerInfoContainer.h>
#include <calobase/TowerInfoContainerv1.h>
#include <calobase/TowerInfoContainerv2.h>
#include <calobase/TowerInfoContainerv3.h>
#include <calobase/TowerInfoContainerv4.h>
#include <calobase/TowerInfoDefs.h>

// Cluster.
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerDefs.h>
#include <CLHEP/Vector/ThreeVector.h>

// Jet.
#include <jetbase/Jetv1.h>
#include <jetbase/Jetv2.h>
#include <jetbase/JetContainer.h>

// Truth particle.
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <TDatabasePDG.h>

// Fun4All.
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>

// Nodes.
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHObject.h>

// General.
#include <cstdint>
#include <iostream>
#include <utility>

class PHCompositeNode;
class TowerInfoContainer;
class JESMCTreeGen;

JESMCTreeGen::JESMCTreeGen(const std::string &name, const std::string &outfilename)
  :SubsysReco(name)
{
  verbosity = 0;
  foutname = outfilename;
  std::cout << "JESMCTreeGen::JESMCTreeGen(const std::string &name) Calling ctor" << std::endl;
}

int JESMCTreeGen::Init(PHCompositeNode * /*topNode*/) {
  if (verbosity > 0) std::cout << "Processing initialization: CaloEmulatorTreeMaker::Init(PHCompositeNode *topNode)" << std::endl;
  file = new TFile( foutname.c_str(), "RECREATE");
  tree = new TTree("ttree","TTree for JES calibration");

  // Vertex information.
  if (doZVertexReadout) Initialize_z_vertex();
  // Tower information.
  if (doTowerInfoReadout) {
    Initialize_calo_tower();
    if (doTowerInfoReadout_RetowerEMCal) Initialize_calo_tower_RetowerEMCal();
  }
  // Cluster information.
  if (doClusterReadout) {
    Initialize_cluster();
    if (doClusterIsoReadout) Initialize_clusterIso();
    if (doClusterCNNReadout) Initialize_clusterCNN();
  }
  // Jet information.
  if (doRecoJetReadout) {
    for (int ir = 0; ir < static_cast<int>(doRecoJet_radius.size()); ir++) {
      Initialize_jet("recojet", doRecoJet_radius[ir]);
    }
    if (doSeedJetRawReadout) Initialize_jet("seedjetraw", 0.2);
    if (doSeedJetSubReadout) Initialize_jet("seedjetsub", 0.2);
  }
  if (doTruthJetReadout) {
    for (int ir = 0; ir < static_cast<int>(doTruthJet_radius.size()); ir++) {
      Initialize_jet("truthjet", doTruthJet_radius[ir]); // Only 0.2, 0.3, 0.4, 0.5 are produced.
    }
  }
  // Truth particle information.
  if (doTruthParticleReadout) Initialize_truthparticle();

  ievent = 0;
  return Fun4AllReturnCodes::EVENT_OK;
}

int JESMCTreeGen::process_event(PHCompositeNode *topNode) {
  if (verbosity >= 0) {
    if (ievent%100 == 0) std::cout << "Processing event " << ievent << std::endl;
  }

  // Vertex information.
  if (doZVertexReadout) Fill_z_vertex(topNode);
  // Tower information.
  if (doTowerInfoReadout) {
    Fill_calo_tower(topNode, "CEMC");
    Fill_calo_tower(topNode, "HCALIN");
    Fill_calo_tower(topNode, "HCALOUT");
    if (doTowerInfoReadout_RetowerEMCal) Fill_emcal_retower(topNode);
  }
  // Cluster information.
  if (doClusterReadout) Fill_cluster(topNode);
  // Jet information.
  if (doRecoJetReadout) {
    for (int ir = 0; ir < static_cast<int>(doRecoJet_radius.size()); ir++) {
      Fill_recojet(topNode, "recojet", "subtracted", doRecoJet_radius[ir]);
    }
    if (doSeedJetRawReadout) Fill_recojet(topNode, "TowerInfo_HIRecoSeedsRaw", "seedjetraw", 0.2);
    if (doSeedJetSubReadout) Fill_recojet(topNode, "TowerInfo_HIRecoSeedsSub", "seedjetsub", 0.2);
  }
  if (doTruthJetReadout) {
    for (int ir = 0; ir < static_cast<int>(doTruthJet_radius.size()); ir++) {
      Fill_truthjet(topNode, doTruthJet_radius[ir]); // Only 0.2 - 0.8 are produced.
    }
  }
  // Truth particle information.
  if (doTruthParticleReadout) Fill_truthparticle(topNode);

  tree->Fill();
  ievent++;
  return Fun4AllReturnCodes::EVENT_OK;
}

int JESMCTreeGen::ResetEvent(PHCompositeNode * /*topNode*/) {
  if (Verbosity() > 1) std::cout << "Resetting the tree branches" << std::endl;

  if (doClusterReadout) Reset_cluster();
  if (doRecoJetReadout || doTruthJetReadout) Reset_jet();
  if (doTruthParticleReadout) Reset_truthparticle();

  return Fun4AllReturnCodes::EVENT_OK;
}

int JESMCTreeGen::End(PHCompositeNode * /*topNode*/) {
  if (verbosity > 0) std::cout << "Processing end: CaloEmulatorTreeMaker::End(PHCompositeNode *topNode)" << std::endl;
  file->cd();
  tree->Write();
  file->Close();
  delete file;
  if (verbosity > 0) std::cout << "CaloEmulatorTreeMaker complete." << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

////////// ********** Initialize functions ********** //////////
void JESMCTreeGen::Initialize_z_vertex() {
  tree->Branch("z_vertex", &z_vertex, "z_vertex/F");
}

void JESMCTreeGen::Initialize_calo_tower() {
  tree->Branch("emcal_tower_e", emcal_tower_e, "emcal_tower_e[96][256]/F"); // EMCal tower is original tower here.
  tree->Branch("ihcal_tower_e", ihcal_tower_e, "ihcal_tower_e[24][64]/F");
  tree->Branch("ohcal_tower_e", ohcal_tower_e, "ohcal_tower_e[24][64]/F");
  tree->Branch("emcal_tower_time", emcal_tower_time, "emcal_tower_time[96][256]/F"); // unit is sample.
  tree->Branch("ihcal_tower_time", ihcal_tower_time, "ihcal_tower_time[24][64]/F");
  tree->Branch("ohcal_tower_time", ohcal_tower_time, "ohcal_tower_time[24][64]/F");
  tree->Branch("emcal_tower_status", emcal_tower_status, "emcal_tower_status[96][256]/I"); // 0 is good, 1 is bad.
  tree->Branch("ihcal_tower_status", ihcal_tower_status, "ihcal_tower_status[24][64]/I");
  tree->Branch("ohcal_tower_status", ohcal_tower_status, "ohcal_tower_status[24][64]/I");
}

void JESMCTreeGen::Initialize_calo_tower_RetowerEMCal() {
  tree->Branch("emcal_retower_e", emcal_retower_e, "emcal_retower_e[24][64]/F");
}

void JESMCTreeGen::Initialize_cluster() {
  tree->Branch("cluster_e", &cluster_e);
  tree->Branch("cluster_pt", &cluster_pt);
  tree->Branch("cluster_eta", &cluster_eta);
  tree->Branch("cluster_phi", &cluster_phi);
  tree->Branch("cluster_e_core", &cluster_e_core);
  tree->Branch("cluster_pt_core", &cluster_pt_core);
  tree->Branch("cluster_eta_core", &cluster_eta_core);
  tree->Branch("cluster_phi_core", &cluster_phi_core);
  tree->Branch("cluster_probability", &cluster_probability);
}

void JESMCTreeGen::Initialize_clusterIso() {
  tree->Branch("cluster_iso", &cluster_iso);
}

void JESMCTreeGen::Initialize_clusterCNN() {
  tree->Branch("cluster_e_cnn", &cluster_e_cnn);
  tree->Branch("cluster_probability_cnn", &cluster_probability_cnn);
}

void JESMCTreeGen::Initialize_jet(std::string jet_prefix , float radius) {
  // Prefix for the jet.
  std::string vector_prefix = jet_prefix + "0" + std::to_string((int)(radius * 10)) + "_";
  // Jet general information.
  std::string jet_e_vectorname = vector_prefix + "e"; jet_e_map[jet_e_vectorname] = std::vector<float>(); tree->Branch(jet_e_vectorname.c_str(), &jet_e_map[jet_e_vectorname]);
  std::string jet_et_vectorname = vector_prefix + "et"; jet_et_map[jet_et_vectorname] = std::vector<float>(); tree->Branch(jet_et_vectorname.c_str(), &jet_et_map[jet_et_vectorname]);
  std::string jet_pt_vectorname = vector_prefix + "pt"; jet_pt_map[jet_pt_vectorname] = std::vector<float>(); tree->Branch(jet_pt_vectorname.c_str(), &jet_pt_map[jet_pt_vectorname]);
  std::string jet_eta_vectorname = vector_prefix + "eta"; jet_eta_map[jet_eta_vectorname] = std::vector<float>(); tree->Branch(jet_eta_vectorname.c_str(), &jet_eta_map[jet_eta_vectorname]);
  std::string jet_phi_vectorname = vector_prefix + "phi"; jet_phi_map[jet_phi_vectorname] = std::vector<float>(); tree->Branch(jet_phi_vectorname.c_str(), &jet_phi_map[jet_phi_vectorname]);
  // Jet tower information or truth jet particle information.
  if (jet_prefix == "truthjet") {
    std::string jet_truthparticle_e_vectorname = vector_prefix + "truthparticle_e"; jet_truthparticle_e_map[jet_truthparticle_e_vectorname] = std::vector<float>(); tree->Branch(jet_truthparticle_e_vectorname.c_str(), &jet_truthparticle_e_map[jet_truthparticle_e_vectorname]);
    std::string jet_truthparticle_pt_vectorname = vector_prefix + "truthparticle_pt"; jet_truthparticle_pt_map[jet_truthparticle_pt_vectorname] = std::vector<float>(); tree->Branch(jet_truthparticle_pt_vectorname.c_str(), &jet_truthparticle_pt_map[jet_truthparticle_pt_vectorname]);
    std::string jet_truthparticle_eta_vectorname = vector_prefix + "truthparticle_eta"; jet_truthparticle_eta_map[jet_truthparticle_eta_vectorname] = std::vector<float>(); tree->Branch(jet_truthparticle_eta_vectorname.c_str(), &jet_truthparticle_eta_map[jet_truthparticle_eta_vectorname]);
    std::string jet_truthparticle_phi_vectorname = vector_prefix + "truthparticle_phi"; jet_truthparticle_phi_map[jet_truthparticle_phi_vectorname] = std::vector<float>(); tree->Branch(jet_truthparticle_phi_vectorname.c_str(), &jet_truthparticle_phi_map[jet_truthparticle_phi_vectorname]);
    std::string jet_truthparticle_pid_vectorname = vector_prefix + "truthparticle_pid"; jet_truthparticle_pid_map[jet_truthparticle_pid_vectorname] = std::vector<int>(); tree->Branch(jet_truthparticle_pid_vectorname.c_str(), &jet_truthparticle_pid_map[jet_truthparticle_pid_vectorname]);
  } else {
    // EMCal tower information. (Retower)
    std::string jet_emcal_ncomponent_vectorname = vector_prefix + "emcal_ncomponent"; jet_tower_ncomponent_map[jet_emcal_ncomponent_vectorname] = std::vector<int>(); tree->Branch(jet_emcal_ncomponent_vectorname.c_str(), &jet_tower_ncomponent_map[jet_emcal_ncomponent_vectorname]);
    std::string jet_emcal_tower_e_vectorname = vector_prefix + "emcal_tower_e"; jet_tower_e_map[jet_emcal_tower_e_vectorname] = std::vector<float>(); tree->Branch(jet_emcal_tower_e_vectorname.c_str(), &jet_tower_e_map[jet_emcal_tower_e_vectorname]);
    std::string jet_emcal_tower_ieta_vectorname = vector_prefix + "emcal_tower_ieta"; jet_tower_ieta_map[jet_emcal_tower_ieta_vectorname] = std::vector<int>(); tree->Branch(jet_emcal_tower_ieta_vectorname.c_str(), &jet_tower_ieta_map[jet_emcal_tower_ieta_vectorname]);
    std::string jet_emcal_tower_iphi_vectorname = vector_prefix + "emcal_tower_iphi"; jet_tower_iphi_map[jet_emcal_tower_iphi_vectorname] = std::vector<int>(); tree->Branch(jet_emcal_tower_iphi_vectorname.c_str(), &jet_tower_iphi_map[jet_emcal_tower_iphi_vectorname]);
    // iHCaL tower information.
    std::string jet_ihcal_ncomponent_vectorname = vector_prefix + "ihcal_ncomponent"; jet_tower_ncomponent_map[jet_ihcal_ncomponent_vectorname] = std::vector<int>(); tree->Branch(jet_ihcal_ncomponent_vectorname.c_str(), &jet_tower_ncomponent_map[jet_ihcal_ncomponent_vectorname]);
    std::string jet_ihcal_tower_e_vectorname = vector_prefix + "ihcal_tower_e"; jet_tower_e_map[jet_ihcal_tower_e_vectorname] = std::vector<float>(); tree->Branch(jet_ihcal_tower_e_vectorname.c_str(), &jet_tower_e_map[jet_ihcal_tower_e_vectorname]);
    std::string jet_ihcal_tower_ieta_vectorname = vector_prefix + "ihcal_tower_ieta"; jet_tower_ieta_map[jet_ihcal_tower_ieta_vectorname] = std::vector<int>(); tree->Branch(jet_ihcal_tower_ieta_vectorname.c_str(), &jet_tower_ieta_map[jet_ihcal_tower_ieta_vectorname]);
    std::string jet_ihcal_tower_iphi_vectorname = vector_prefix + "ihcal_tower_iphi"; jet_tower_iphi_map[jet_ihcal_tower_iphi_vectorname] = std::vector<int>(); tree->Branch(jet_ihcal_tower_iphi_vectorname.c_str(), &jet_tower_iphi_map[jet_ihcal_tower_iphi_vectorname]);
    // oHCaL tower information.
    std::string jet_ohcal_ncomponent_vectorname = vector_prefix + "ohcal_ncomponent"; jet_tower_ncomponent_map[jet_ohcal_ncomponent_vectorname] = std::vector<int>(); tree->Branch(jet_ohcal_ncomponent_vectorname.c_str(), &jet_tower_ncomponent_map[jet_ohcal_ncomponent_vectorname]);
    std::string jet_ohcal_tower_e_vectorname = vector_prefix + "ohcal_tower_e"; jet_tower_e_map[jet_ohcal_tower_e_vectorname] = std::vector<float>(); tree->Branch(jet_ohcal_tower_e_vectorname.c_str(), &jet_tower_e_map[jet_ohcal_tower_e_vectorname]);
    std::string jet_ohcal_tower_ieta_vectorname = vector_prefix + "ohcal_tower_ieta"; jet_tower_ieta_map[jet_ohcal_tower_ieta_vectorname] = std::vector<int>(); tree->Branch(jet_ohcal_tower_ieta_vectorname.c_str(), &jet_tower_ieta_map[jet_ohcal_tower_ieta_vectorname]);
    std::string jet_ohcal_tower_iphi_vectorname = vector_prefix + "ohcal_tower_iphi"; jet_tower_iphi_map[jet_ohcal_tower_iphi_vectorname] = std::vector<int>(); tree->Branch(jet_ohcal_tower_iphi_vectorname.c_str(), &jet_tower_iphi_map[jet_ohcal_tower_iphi_vectorname]);
  }
}

void JESMCTreeGen::Initialize_truthparticle() {
  tree->Branch("truthparticle_e", &truthparticle_e);
  tree->Branch("truthparticle_pt", &truthparticle_pt);
  tree->Branch("truthparticle_eta", &truthparticle_eta);
  tree->Branch("truthparticle_phi", &truthparticle_phi);
  tree->Branch("truthparticle_pid", &truthparticle_pid);
}

////////// ********** Fill functions ********** //////////
void JESMCTreeGen::Fill_z_vertex(PHCompositeNode *topNode) {
  z_vertex = -999;
  GlobalVertexMap *vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (vertexmap) {
    if (!vertexmap->empty()) {
      GlobalVertex *vtx = vertexmap->begin()->second;
      if (vtx) {
        z_vertex = vtx->get_z();
      }
    } else {
      std::cout << "GlobalVertexMap is empty" << std::endl;
    }
  } else {
    std::cout << "GlobalVertexMap is missing" << std::endl;
  }
}

void JESMCTreeGen::Fill_calo_tower(PHCompositeNode *topNode, std::string calorimeter) {
  std::string tower_info_container_name = "TOWERINFO_CALIB_" + calorimeter;
  TowerInfoContainer *_towers_calo = findNode::getClass<TowerInfoContainer>(topNode, tower_info_container_name);
  if (_towers_calo) {
    for (int channel = 0; channel < (int)_towers_calo->size(); ++channel) {
	    TowerInfo *_tower = _towers_calo->get_tower_at_channel(channel);
	    unsigned int towerkey = _towers_calo->encode_key(channel);
	    int etabin = _towers_calo->getTowerEtaBin(towerkey);
	    int phibin = _towers_calo->getTowerPhiBin(towerkey);
      float towE = _tower->get_energy();
      float towT = _tower->get_time_float();
      int towS = 1;
      if (!_tower->get_isHot() && !_tower->get_isNoCalib() && !_tower->get_isNotInstr() && !_tower->get_isBadChi2()) {
        towS = 0;
      }
	    if (calorimeter == "CEMC") {
        emcal_tower_e[etabin][phibin] = towE;
        emcal_tower_time[etabin][phibin] = towT;
        emcal_tower_status[etabin][phibin] = towS;
      } else if (calorimeter == "HCALIN") {
        ihcal_tower_e[etabin][phibin] = towE;
        ihcal_tower_time[etabin][phibin] = towT;
        ihcal_tower_status[etabin][phibin] = towS;
      } else if (calorimeter == "HCALOUT") {
        ohcal_tower_e[etabin][phibin] = towE;
        ohcal_tower_time[etabin][phibin] = towT;
        ohcal_tower_status[etabin][phibin] = towS;
      }
    }
  } else {
    std::cout << "TowerInfoContainer for " << calorimeter << " is missing" << std::endl;
  }
}

void JESMCTreeGen::Fill_emcal_retower(PHCompositeNode *topNode) {
  TowerInfoContainer *_towers_calo = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
  if (_towers_calo) {
    for (int channel = 0; channel < (int)_towers_calo->size(); ++channel) {
	    TowerInfo *_tower = _towers_calo->get_tower_at_channel(channel);
	    unsigned int towerkey = _towers_calo->encode_key(channel);
	    int etabin = _towers_calo->getTowerEtaBin(towerkey);
	    int phibin = _towers_calo->getTowerPhiBin(towerkey);
      float towE = _tower->get_energy();
      emcal_retower_e[etabin][phibin] = towE;
    }
  } else {
    std::cout << "TowerInfoContainer EMCal retower node is missing" << std::endl;
  }
}

void JESMCTreeGen::Fill_cluster(PHCompositeNode *topNode) {
  std::string _clustername;
  _clustername = "CLUSTERINFO_CEMC";
  RawClusterContainer *_clusters = findNode::getClass<RawClusterContainer>(topNode, _clustername.c_str());
  if (_clusters) {
    RawClusterContainer::ConstRange clusterEnd = _clusters->getClusters();
	  for (RawClusterContainer::ConstIterator clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++) {
	    RawCluster *_cluster = clusterIter->second;
      CLHEP::Hep3Vector vertex(0, 0, 0); // Vertex is assumed to be at (0, 0, 0). This is a placeholder for vertex correction.
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*_cluster, vertex);
      CLHEP::Hep3Vector E_vec_cluster_Full = RawClusterUtility::GetEVec(*_cluster, vertex);
      float clusE = E_vec_cluster_Full.mag();
      if (clusE < 1) continue; // Skip clusters with energy less than 1 GeV.
      float clusEcore = E_vec_cluster.mag();
      float clus_eta = E_vec_cluster_Full.pseudoRapidity();
      float clus_phi = E_vec_cluster_Full.phi();
      float clus_pt = E_vec_cluster_Full.perp();
      float clus_coreeta = E_vec_cluster.pseudoRapidity();
      float clus_corephi = E_vec_cluster.phi();
      float clus_corept = E_vec_cluster.perp();
      float clus_prob = _cluster->get_prob();
      cluster_e.push_back(clusE);
	    cluster_pt.push_back(clus_pt);
	    cluster_eta.push_back(clus_eta);
      cluster_phi.push_back(clus_phi);
      cluster_e_core.push_back(clusEcore);
      cluster_pt_core.push_back(clus_corept);
      cluster_eta_core.push_back(clus_coreeta);
      cluster_phi_core.push_back(clus_corephi);
      cluster_probability.push_back(clus_prob);
      if (doClusterIsoReadout) {
        float clus_iso = _cluster->get_et_iso(4, false, true);
        cluster_iso.push_back(clus_iso);
      }
	  }
  } else {
    std::cout << "RawClusterContainer is missing" << std::endl;
  }
  if (doClusterCNNReadout) {
    std::string _clustername_cnn;
    _clustername_cnn = "CLUSTERINFO_CEMC_CNN";
    RawClusterContainer *_clusters_cnn = findNode::getClass<RawClusterContainer>(topNode, _clustername_cnn.c_str());
    if (_clusters_cnn) {
      RawClusterContainer::ConstRange clusterEnd_cnn = _clusters_cnn->getClusters();
      for (RawClusterContainer::ConstIterator clusterIter = clusterEnd_cnn.first; clusterIter != clusterEnd_cnn.second; clusterIter++) {
        RawCluster *_cluster = clusterIter->second;
        float clus_prob_cnn = _cluster->get_prob();
        float clusE = _cluster->get_energy();
        cluster_probability_cnn.push_back(clus_prob_cnn);
        cluster_e_cnn.push_back(clusE);
      }
    } else {
      std::cout << "RawClusterContainer for CNN cluster is missing" << std::endl;
    }
  }
}

void JESMCTreeGen::Fill_recojet(PHCompositeNode *topNode, std::string jet_prefix, std::string node_prefix, float radius) {
  // Set vector name prefix for the jet map.
  std::string vector_prefix = jet_prefix + "0" + std::to_string((int)(radius * 10)) + "_";
  // Get the jet container.
  std::string nodename = "AntiKt_" + node_prefix + "_r0" + std::to_string((int)(radius * 10));
  JetContainer* _jets = findNode::getClass<JetContainer>(topNode, nodename);
  if (!_jets) std::cout << "JetContainer for " << nodename << " is missing" << std::endl;
  // Get the towerinfo container.
  TowerInfoContainer *_emcal_towers = nullptr;
  TowerInfoContainer *_ihcal_towers = nullptr;
  TowerInfoContainer *_ohcal_towers = nullptr;
  if (node_prefix == "TowerInfo_HIRecoSeedsRaw") {
    _emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER");
    _ihcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN");
    _ohcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT");
  } else {
    _emcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_CEMC_RETOWER_SUB1");
    _ihcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALIN_SUB1");
    _ohcal_towers = findNode::getClass<TowerInfoContainer>(topNode, "TOWERINFO_CALIB_HCALOUT_SUB1");
  }
  if (!_emcal_towers || !_ihcal_towers || !_ohcal_towers) std::cout << "TowerInfoContainer is missing" << std::endl;
  // Fill the jet information.
  for (auto _jet : *_jets) {
    if (_jet->get_pt() < 1) continue; // Skip jets with pt less than 1 GeV. (same cut in jet reconstruction)
    jet_e_map[vector_prefix+"e"].push_back(_jet->get_e());
    jet_et_map[vector_prefix+"et"].push_back(_jet->get_et());
    jet_pt_map[vector_prefix+"pt"].push_back(_jet->get_pt());
    jet_eta_map[vector_prefix+"eta"].push_back(_jet->get_eta());
    jet_phi_map[vector_prefix+"phi"].push_back(_jet->get_phi());
    int ncomponent_emcal = 0;
    int ncomponent_ihcal = 0;
    int ncomponent_ohcal = 0;
    for (auto comp: _jet->get_comp_vec()) {
	    unsigned int channel = comp.second;
      if (comp.first == 14 || comp.first == 29 || comp.first == 25 || comp.first == 28) {
        TowerInfo *_tower = _emcal_towers->get_tower_at_channel(channel);
	      unsigned int calokey = _emcal_towers->encode_key(channel);
	      int jet_tower_ieta = _emcal_towers->getTowerEtaBin(calokey);
	      int jet_tower_iphi = _emcal_towers->getTowerPhiBin(calokey);
	      float jet_tower_e = _tower->get_energy();
        jet_tower_e_map[vector_prefix+"emcal_tower_e"].push_back(jet_tower_e);
        jet_tower_ieta_map[vector_prefix+"emcal_tower_ieta"].push_back(jet_tower_ieta);
        jet_tower_iphi_map[vector_prefix+"emcal_tower_iphi"].push_back(jet_tower_iphi);
        ncomponent_emcal++;
	    } else if (comp.first == 15 ||  comp.first == 30 || comp.first == 26) {
        TowerInfo *_tower = _ihcal_towers->get_tower_at_channel(channel);
	      unsigned int calokey = _ihcal_towers->encode_key(channel);
	      int jet_tower_ieta = _ihcal_towers->getTowerEtaBin(calokey);
	      int jet_tower_iphi = _ihcal_towers->getTowerPhiBin(calokey);
	      float jet_tower_e = _tower->get_energy();
        jet_tower_e_map[vector_prefix+"ihcal_tower_e"].push_back(jet_tower_e);
        jet_tower_ieta_map[vector_prefix+"ihcal_tower_ieta"].push_back(jet_tower_ieta);
        jet_tower_iphi_map[vector_prefix+"ihcal_tower_iphi"].push_back(jet_tower_iphi);
        ncomponent_ihcal++;
	    } else if (comp.first == 16 || comp.first == 31 || comp.first == 27) {
        TowerInfo *_tower = _ohcal_towers->get_tower_at_channel(channel);
	      unsigned int calokey = _ohcal_towers->encode_key(channel);
	      int jet_tower_ieta = _ohcal_towers->getTowerEtaBin(calokey);
	      int jet_tower_iphi = _ohcal_towers->getTowerPhiBin(calokey);
	      float jet_tower_e = _tower->get_energy();
        jet_tower_e_map[vector_prefix+"ohcal_tower_e"].push_back(jet_tower_e);
        jet_tower_ieta_map[vector_prefix+"ohcal_tower_ieta"].push_back(jet_tower_ieta);
        jet_tower_iphi_map[vector_prefix+"ohcal_tower_iphi"].push_back(jet_tower_iphi);
        ncomponent_ohcal++;
      }
	  }
    jet_tower_ncomponent_map[vector_prefix+"emcal_ncomponent"].push_back(ncomponent_emcal);
    jet_tower_ncomponent_map[vector_prefix+"ihcal_ncomponent"].push_back(ncomponent_ihcal);
    jet_tower_ncomponent_map[vector_prefix+"ohcal_ncomponent"].push_back(ncomponent_ohcal);
  }
}

void JESMCTreeGen::Fill_truthjet(PHCompositeNode *topNode, float radius) {
  // Set vector name prefix for the jet map.
  std::string vector_prefix = "truthjet0" + std::to_string((int)(radius * 10)) + "_";
  // Get the jet container.
  std::string nodename = "AntiKt_Truth_r0" + std::to_string((int)(radius * 10));
  JetContainer* _truth_jets = findNode::getClass<JetContainer>(topNode, nodename);
  if (!_truth_jets) std::cout << "JetContainer for " << nodename << " is missing" << std::endl;
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (_truth_jets && truthinfo) {
    for (auto _truthjet : *_truth_jets) {
      if (_truthjet->get_eta() < -2 || _truthjet->get_eta() > 2 || _truthjet->get_pt() < 5 || _truthjet->get_pt() > 100) continue; // Cut for truth jets.
      jet_e_map[vector_prefix+"e"].push_back(_truthjet->get_e());
      jet_et_map[vector_prefix+"et"].push_back(_truthjet->get_et());
      jet_pt_map[vector_prefix+"pt"].push_back(_truthjet->get_pt());
      jet_eta_map[vector_prefix+"eta"].push_back(_truthjet->get_eta());
      jet_phi_map[vector_prefix+"phi"].push_back(_truthjet->get_phi());
      for (auto comp: _truthjet->get_comp_vec()) {
	      PHG4Particle* _truthparticle = truthinfo->GetParticle(comp.second);
        float particle_e = _truthparticle->get_e();
        float particle_pt = sqrt(_truthparticle->get_px() * _truthparticle->get_px() + _truthparticle->get_py() * _truthparticle->get_py());
        float particle_eta = atanh(_truthparticle->get_pz() / sqrt(_truthparticle->get_px()*_truthparticle->get_px()+_truthparticle->get_py()*_truthparticle->get_py()+_truthparticle->get_pz()*_truthparticle->get_pz()));
        float particle_phi = atan2(_truthparticle->get_py(), _truthparticle->get_px());
        int particle_pid = _truthparticle->get_pid();
        jet_truthparticle_e_map[vector_prefix+"truthparticle_e"].push_back(particle_e);
        jet_truthparticle_pt_map[vector_prefix+"truthparticle_pt"].push_back(particle_pt);
        jet_truthparticle_eta_map[vector_prefix+"truthparticle_eta"].push_back(particle_eta);
        jet_truthparticle_phi_map[vector_prefix+"truthparticle_phi"].push_back(particle_phi);
        jet_truthparticle_pid_map[vector_prefix+"truthparticle_pid"].push_back(particle_pid);
      }
	  }
  } else {
    std::cout << "JetContainer for " << nodename << " is missing, or PHG4TruthInfoContainer is missing" << std::endl;
  }
}

void JESMCTreeGen::Fill_truthparticle(PHCompositeNode *topNode) {
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (truthinfo) {
    PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
    for (PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter) {
      PHG4Particle *truth = iter->second;
      if (!truthinfo->is_primary(truth)) continue;
      float particle_eta = atanh(truth->get_pz() / sqrt(truth->get_px()*truth->get_px()+truth->get_py()*truth->get_py()+truth->get_pz()*truth->get_pz()));
      if (fabs(particle_eta) > 1.6) continue;
      float particle_e = truth->get_e();
      float particle_pt = sqrt(truth->get_px() * truth->get_px() + truth->get_py() * truth->get_py());
      float particle_phi = atan2(truth->get_py(), truth->get_px());
      int particle_pid = truth->get_pid();
      truthparticle_e.push_back(particle_e);
      truthparticle_pt.push_back(particle_pt);
      truthparticle_eta.push_back(particle_eta);
      truthparticle_phi.push_back(particle_phi);
      truthparticle_pid.push_back(particle_pid);
    }
  } else {
    std::cout << "PHG4TruthInfoContainer is missing" << std::endl;
  }
}

////////// ********** Reset functions ********** //////////
void JESMCTreeGen::Reset_cluster() {
  cluster_e.clear();
  cluster_pt.clear();
  cluster_eta.clear();
  cluster_phi.clear();
  cluster_e_core.clear();
  cluster_pt_core.clear();
  cluster_eta_core.clear();
  cluster_phi_core.clear();
  cluster_probability.clear();
  if (doClusterIsoReadout) {
    cluster_iso.clear();  
  }
  if (doClusterCNNReadout) {
    cluster_e_cnn.clear();
    cluster_probability_cnn.clear();
  }
}

void JESMCTreeGen::Reset_jet() {
  for (auto& entry : jet_e_map) entry.second.clear();
  for (auto& entry : jet_et_map) entry.second.clear();
  for (auto& entry : jet_pt_map) entry.second.clear();
  for (auto& entry : jet_eta_map) entry.second.clear();
  for (auto& entry : jet_phi_map) entry.second.clear();
  if (doRecoJetReadout) {
    for (auto& entry : jet_tower_ncomponent_map) entry.second.clear();
    for (auto& entry : jet_tower_e_map) entry.second.clear();
    for (auto& entry : jet_tower_ieta_map) entry.second.clear();
    for (auto& entry : jet_tower_iphi_map) entry.second.clear();
  }
  if (doTruthJetReadout) {
    for (auto& entry : jet_truthparticle_e_map) entry.second.clear();
    for (auto& entry : jet_truthparticle_pt_map) entry.second.clear();
    for (auto& entry : jet_truthparticle_eta_map) entry.second.clear();
    for (auto& entry : jet_truthparticle_phi_map) entry.second.clear();
    for (auto& entry : jet_truthparticle_pid_map) entry.second.clear();
  }
}

void JESMCTreeGen::Reset_truthparticle() {
  truthparticle_e.clear();
  truthparticle_pt.clear();
  truthparticle_eta.clear();
  truthparticle_phi.clear();
  truthparticle_pid.clear();
}