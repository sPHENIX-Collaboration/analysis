
//____________________________________________________________________________..

//our base code
#include "pythiaEMCalAna.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>

//Fun4all stuff
#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllHistoManager.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <ffaobjects/EventHeader.h>

//ROOT stuff
#include <TH1F.h>
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <TLorentzVector.h>
#include <TTree.h>

//for emc clusters
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/TowerInfo.h>
#include <calobase/TowerInfoContainer.h>

// Minimum Bias
#include <calotrigger/MinimumBiasInfo.h>

//caloEvalStack for cluster to truth matching
#include <g4eval/CaloEvalStack.h>
/* #include <g4eval/CaloRawClusterEval.h> */

//for vertex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
/* #include <g4vertex/GlobalVertex.h> */
/* #include <g4vertex/GlobalVertexMap.h> */

//tracking
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
/* #include <trackbase_historic/SvtxVertex.h> */
/* #include <trackbase_historic/SvtxVertexMap.h> */

//truth information
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Shower.h>
#include <g4main/PHG4VtxPoint.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <phhepmc/PHHepMCGenEventMap.h>
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h> 
#include <HepMC/GenParticle.h>
#pragma GCC diagnostic pop

// PID stuff to print info on how many of each truth particle there were
// Mostly useful as a sanity check

bool IsBaryon(int pdg_id) {
    /* std::cout << Form("Entering IsBaryon(%d)\n", pdg_id); */
    bool ret = false;
    // baryon ids are 4-digit numbers
    if (abs(pdg_id) > 999 && abs(pdg_id) < 10000) ret = true;
    /* std::cout << Form("In IsBaryon(%d): returning %s\n", pdg_id, ret ? "true": "false"); */
    return ret;
}

bool IsNeutralBaryon(int pdg_id) {
    /* std::cout << Form("Entering IsNeutralBaryon(%d)\n", pdg_id); */
    if (!IsBaryon(pdg_id)) return false;
    // baryon ids are 4-digit numbers
    int base_id = abs(pdg_id) % 10000;
    // the last digit is the angular momentum state
    // the first three digits are the quark flavors
    int q1 = (int)(base_id/1000);
    int q2 = (int)(base_id/100 % 10);
    int q3 = (int)(base_id/10 % 10);
    // odd numbers 1,3,5 are d,s,b; even numbers 2,4,6 are u,c,t
    float charge = 0.;
    if (q1%2 == 0) charge += 2./3.;
    else charge -= 1./3.;
    if (q2%2 == 0) charge += 2./3.;
    else charge -= 1./3.;
    if (q3%2 == 0) charge += 2./3.;
    else charge -= 1./3.;
    /* std::cout << Form("In IsNeutralBaryon: base_id=%d, charge=%f\n", base_id, charge); */
    if (abs(charge) < 0.05) return true;
    else return false;
}

bool IsNeutralMeson(int pdg_id) {
    /* std::cout << Form("Entering IsNeutralMeson(%d)\n", pdg_id); */
    // excited states are >3 digit numbers; first reduce them to the base quark content
    int base_id = pdg_id % 1000;
    // meson ids are 3-digit numbers
    // the last digit is the angular momentum state
    // the first two digits are the quark flavors
    int q1 = (int)(base_id/100);
    int q2 = (int)(base_id/10 % 10);
    float charge = 0.;
    if (q1%2 == 0) charge += 2./3.;
    else charge -= 1./3.;
    if (q2%2 == 0) charge -= 2./3.;
    else charge += 1./3.;
    /* std::cout << Form("In IsNeutralMeson: base_id=%d, charge=%f\n", base_id, charge); */
    if (abs(charge) < 0.05) return true;
    else return false;
}
    
bool IsNeutralHadron(int pdg_id) {
    /* std::cout << Form("\nEntering IsNeutralHadron(%d)\n", pdg_id); */
    bool ret = (IsNeutralBaryon(pdg_id) || IsNeutralMeson(pdg_id));
    /* std::cout << Form("In IsNeutralHadron: returning %s\n", ret ? "true" : "false"); */
    return ret;
}

bool IsChargedHadron(int pdg_id) {
    if (pdg_id < 0) return true;
    if (pdg_id < 99) return false;
    return (!IsNeutralHadron(pdg_id));
}

//____________________________________________________________________________..
pythiaEMCalAna::pythiaEMCalAna(const std::string &name, const std::string &oname, bool isMC, bool hasPythia):
SubsysReco(name),
  clusters_Towers(nullptr),
  truth_particles(nullptr),
  m_truthInfo(nullptr),
  m_theEvent(nullptr),
  m_clusterContainer(nullptr),
  m_clusterEval(nullptr),
  m_tower_energy(0),
  m_eta_center(0),
  m_phi_center(0),
  m_cluster_ID(0),
  m_cluster_e(0),
  m_cluster_eta(0),
  m_cluster_phi(0),
  m_cluster_ecore(0),
  m_cluster_chi2(0),
  m_cluster_prob(0),
  m_cluster_nTowers(0),
  m_cluster_allTowersE(0),
  m_cluster_allTowersEta(0),
  m_cluster_allTowersPhi(0),
  m_cluster_nParticles(0),
  m_cluster_primaryParticlePid(0),
  m_cluster_allSecondaryPids(0),
  /* m_cluster_maxE_trackID(0), */
  /* m_cluster_maxE_PID(0), */
  /* m_cluster_all_trackIDs(0), */
  m_truth_Parent_Barcode(0),
  m_truth_Parent_Pid(0),
  m_truth_Parent_M(0),
  m_truth_Parent_E(0),
  m_truth_Parent_Eta(0),
  m_truth_Parent_Phi(0),
  m_truth_Parent_Pt(0),
  m_truth_Parent_xF(0),
  m_truth_Parent_EndVtx_x(0),
  m_truth_Parent_EndVtx_y(0),
  m_truth_Parent_EndVtx_z(0),
  /* m_truth_Parent_EndVtx_t(0), */
  m_truth_Parent_EndVtx_r(0),
  /* m_truth_Parent_TotalNDaughters(0), */
  /* m_truth_Parent_AllDaughterPids(0), */
  /* m_truth_Parent_AllDaughterEnergyFractions(0), */
  m_truth_Parent_TotalNClusters(0),
  m_truth_Parent_AllClusterIDs(0),
  m_truth_Parent_AllClusterEnergyFractions(0),
  m_truth_Decay1_Barcode(0),
  m_truth_Decay1_Pid(0),
  m_truth_Decay1_M(0),
  m_truth_Decay1_E(0),
  m_truth_Decay1_Eta(0),
  m_truth_Decay1_Phi(0),
  m_truth_Decay1_Pt(0),
  m_truth_Decay1_xF(0),
  m_truth_Decay1_EndVtx_x(0),
  m_truth_Decay1_EndVtx_y(0),
  m_truth_Decay1_EndVtx_z(0),
  /* m_truth_Decay1_EndVtx_t(0), */
  m_truth_Decay1_EndVtx_r(0),
  m_truth_Decay1_TotalNClusters(0),
  m_truth_Decay1_BestClusterID(0),
  m_truth_Decay1_BestClusterEfraction(0),
  /* m_truth_Decay1_AllClusterIDs(0), */
  /* m_truth_Decay1_AllClusterEnergyFractions(0), */
  m_truth_Decay2_Barcode(0),
  m_truth_Decay2_Pid(0),
  m_truth_Decay2_M(0),
  m_truth_Decay2_E(0),
  m_truth_Decay2_Eta(0),
  m_truth_Decay2_Phi(0),
  m_truth_Decay2_Pt(0),
  m_truth_Decay2_xF(0),
  m_truth_Decay2_EndVtx_x(0),
  m_truth_Decay2_EndVtx_y(0),
  m_truth_Decay2_EndVtx_z(0),
  /* m_truth_Decay2_EndVtx_t(0), */
  m_truth_Decay2_EndVtx_r(0),
  m_truth_Decay2_TotalNClusters(0),
  m_truth_Decay2_BestClusterID(0),
  m_truth_Decay2_BestClusterEfraction(0),
  /* m_truth_Decay2_AllClusterIDs(0), */
  /* m_truth_Decay2_AllClusterEnergyFractions(0), */
  /* m_truth_Cluster1_Id(0), */
  /* m_truth_Cluster1_E(0), */
  /* m_truth_Cluster1_Ecore(0), */
  /* m_truth_Cluster1_Eta(0), */
  /* m_truth_Cluster1_Phi(0), */
  /* m_truth_Cluster1_Pt(0), */
  /* m_truth_Cluster1_xF(0), */
  /* m_truth_Cluster1_Chi2(0), */
  /* m_truth_Cluster1_Decay1EnergyFraction(0), */
  /* m_truth_Cluster1_Decay2EnergyFraction(0), */
  /* m_truth_Cluster2_Id(0), */
  /* m_truth_Cluster2_E(0), */
  /* m_truth_Cluster2_Ecore(0), */
  /* m_truth_Cluster2_Eta(0), */
  /* m_truth_Cluster2_Phi(0), */
  /* m_truth_Cluster2_Pt(0), */
  /* m_truth_Cluster2_xF(0), */
  /* m_truth_Cluster2_Chi2(0), */
  /* m_truth_Cluster2_Decay1EnergyFraction(0), */
  /* m_truth_Cluster2_Decay2EnergyFraction(0), */
  fout(NULL),
  outname(oname),
  getEvent(-9999),
  /* hasHIJING(isAuAu), */
  isMonteCarlo(isMC),
  hasPYTHIA(hasPythia),
  n_events_total(0),
  n_events_minbias(0),
  n_events_with_vertex(0),
  n_events_with_good_vertex(0),
  n_events_positiveCaloE(0),
  n_primaries(0),
  n_primary_photons(0),
  n_direct_photons(0),
  n_leptons(0),
  n_neutral_hadrons(0),
  n_neutral_hadrons_geant(0),
  n_neutral_hadrons_pythia(0),
  n_charged_hadrons(0),
  n_charged_hadrons_geant(0),
  n_charged_hadrons_pythia(0),
  n_pythia_decayed_hadrons(0),
  allTrackIDs(),
  primaryBarcodes()
{
std::cout << "pythiaEMCalAna::pythiaEMCalAna(const std::string &name) Calling ctor" << std::endl;
}
//____________________________________________________________________________..
pythiaEMCalAna::~pythiaEMCalAna()
{
  std::cout << "pythiaEMCalAna::~pythiaEMCalAna() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int pythiaEMCalAna::Init(PHCompositeNode *topNode)
{
  std::cout << "pythiaEMCalAna::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  fout = new TFile(outname.c_str(),"RECREATE");

  clusters_Towers = new TTree("TreeClusterTower","Tree for cluster and tower information");
  clusters_Towers -> Branch("towerEnergy",&m_tower_energy);
  clusters_Towers -> Branch("towerEtaCenter",&m_eta_center);
  clusters_Towers -> Branch("towerPhiCenter",& m_phi_center);
  clusters_Towers -> Branch("clusterID",&m_cluster_ID);
  clusters_Towers -> Branch("clusterE",&m_cluster_e);
  clusters_Towers -> Branch("clusterEta",&m_cluster_eta);
  clusters_Towers -> Branch("clusterPhi", &m_cluster_phi);
  clusters_Towers -> Branch("clusterEcore",&m_cluster_ecore);
  clusters_Towers -> Branch("clusterChi2",&m_cluster_chi2);
  clusters_Towers -> Branch("clusterProb",&m_cluster_prob);
  clusters_Towers -> Branch("clusterNTowers",&m_cluster_nTowers);
  clusters_Towers -> Branch("clusterAllTowersE",&m_cluster_allTowersE);
  clusters_Towers -> Branch("clusterAllTowersEta",&m_cluster_allTowersEta);
  clusters_Towers -> Branch("clusterAllTowersPhi",&m_cluster_allTowersPhi);
  if (isMonteCarlo) {
      clusters_Towers -> Branch("clusterNParticles",&m_cluster_nParticles);
      clusters_Towers -> Branch("clusterPrimaryParticlePid",&m_cluster_primaryParticlePid);
      clusters_Towers -> Branch("clusterAllParticlePids",&m_cluster_allSecondaryPids, 32000, 0);
      /* clusters_Towers -> Branch("clusterMaxE_trackID",&m_cluster_maxE_trackID); */
      /* clusters_Towers -> Branch("clusterMaxE_PID",&m_cluster_maxE_PID); */
      /* clusters_Towers -> Branch("clusterAll_trackIDs",&m_cluster_all_trackIDs, 32000, 0); */
  }

  if (isMonteCarlo) {
      truth_particles = new TTree("TreeTruthParticles", "Tree for Truth Particles");
      truth_particles->Branch("truth_Parent_Barcode", &m_truth_Parent_Barcode);
      truth_particles->Branch("truth_Parent_Pid", &m_truth_Parent_Pid);
      truth_particles->Branch("truth_Parent_M", &m_truth_Parent_M);
      truth_particles->Branch("truth_Parent_E", &m_truth_Parent_E);
      truth_particles->Branch("truth_Parent_Eta", &m_truth_Parent_Eta);
      truth_particles->Branch("truth_Parent_Phi", &m_truth_Parent_Phi);
      truth_particles->Branch("truth_Parent_Pt", &m_truth_Parent_Pt);
      truth_particles->Branch("truth_Parent_xF", &m_truth_Parent_xF);
      truth_particles->Branch("truth_Parent_EndVtx_x", &m_truth_Parent_EndVtx_x);
      truth_particles->Branch("truth_Parent_EndVtx_y", &m_truth_Parent_EndVtx_y);
      truth_particles->Branch("truth_Parent_EndVtx_z", &m_truth_Parent_EndVtx_z);
      /* truth_particles->Branch("truth_Parent_EndVtx_t", &m_truth_Parent_EndVtx_t); */
      truth_particles->Branch("truth_Parent_EndVtx_r", &m_truth_Parent_EndVtx_r);
      truth_particles->Branch("truth_Parent_TotalNClusters", &m_truth_Parent_TotalNClusters);
      truth_particles->Branch("truth_Parent_AllClusterIDs", &m_truth_Parent_AllClusterIDs);
      truth_particles->Branch("truth_Parent_AllClusterEnergyFractions", &m_truth_Parent_AllClusterEnergyFractions);
      truth_particles->Branch("truth_Decay1_Barcode", &m_truth_Decay1_Barcode);
      truth_particles->Branch("truth_Decay1_Pid", &m_truth_Decay1_Pid);
      truth_particles->Branch("truth_Decay1_M", &m_truth_Decay1_M);
      truth_particles->Branch("truth_Decay1_E", &m_truth_Decay1_E);
      truth_particles->Branch("truth_Decay1_Eta", &m_truth_Decay1_Eta);
      truth_particles->Branch("truth_Decay1_Phi", &m_truth_Decay1_Phi);
      truth_particles->Branch("truth_Decay1_Pt", &m_truth_Decay1_Pt);
      truth_particles->Branch("truth_Decay1_xF", &m_truth_Decay1_xF);
      truth_particles->Branch("truth_Decay1_EndVtx_x", &m_truth_Decay1_EndVtx_x);
      truth_particles->Branch("truth_Decay1_EndVtx_y", &m_truth_Decay1_EndVtx_y);
      truth_particles->Branch("truth_Decay1_EndVtx_z", &m_truth_Decay1_EndVtx_z);
      /* truth_particles->Branch("truth_Decay1_EndVtx_t", &m_truth_Decay1_EndVtx_t); */
      truth_particles->Branch("truth_Decay1_EndVtx_r", &m_truth_Decay1_EndVtx_r);
      truth_particles->Branch("truth_Decay1_TotalNClusters", &m_truth_Decay1_TotalNClusters);
      truth_particles->Branch("truth_Decay1_BestClusterID", &m_truth_Decay1_BestClusterID);
      truth_particles->Branch("truth_Decay1_BestClusterEfraction", &m_truth_Decay1_BestClusterEfraction);
      /* truth_particles->Branch("truth_Decay1_AllClusterIDs", &m_truth_Decay1_AllClusterIDs); */
      /* truth_particles->Branch("truth_Decay1_AllClusterEnergyFractions", &m_truth_Decay1_AllClusterEnergyFractions); */
      truth_particles->Branch("truth_Decay2_Barcode", &m_truth_Decay2_Barcode);
      truth_particles->Branch("truth_Decay2_Pid", &m_truth_Decay2_Pid);
      truth_particles->Branch("truth_Decay2_M", &m_truth_Decay2_M);
      truth_particles->Branch("truth_Decay2_E", &m_truth_Decay2_E);
      truth_particles->Branch("truth_Decay2_Eta", &m_truth_Decay2_Eta);
      truth_particles->Branch("truth_Decay2_Phi", &m_truth_Decay2_Phi);
      truth_particles->Branch("truth_Decay2_Pt", &m_truth_Decay2_Pt);
      truth_particles->Branch("truth_Decay2_xF", &m_truth_Decay2_xF);
      truth_particles->Branch("truth_Decay2_EndVtx_x", &m_truth_Decay2_EndVtx_x);
      truth_particles->Branch("truth_Decay2_EndVtx_y", &m_truth_Decay2_EndVtx_y);
      truth_particles->Branch("truth_Decay2_EndVtx_z", &m_truth_Decay2_EndVtx_z);
      /* truth_particles->Branch("truth_Decay2_EndVtx_t", &m_truth_Decay2_EndVtx_t); */
      truth_particles->Branch("truth_Decay2_EndVtx_r", &m_truth_Decay2_EndVtx_r);
      truth_particles->Branch("truth_Decay2_TotalNClusters", &m_truth_Decay2_TotalNClusters);
      truth_particles->Branch("truth_Decay2_BestClusterID", &m_truth_Decay2_BestClusterID);
      truth_particles->Branch("truth_Decay2_BestClusterEfraction", &m_truth_Decay2_BestClusterEfraction);
      /* truth_particles->Branch("truth_Decay2_AllClusterIDs", &m_truth_Decay2_AllClusterIDs); */
      /* truth_particles->Branch("truth_Decay2_AllClusterEnergyFractions", &m_truth_Decay2_AllClusterEnergyFractions); */
      /* truth_particles->Branch("truth_Cluster1_Id", &m_truth_Cluster1_Id); */
      /* truth_particles->Branch("truth_Cluster1_E", &m_truth_Cluster1_E); */
      /* truth_particles->Branch("truth_Cluster1_Ecore", &m_truth_Cluster1_Ecore); */
      /* truth_particles->Branch("truth_Cluster1_Eta", &m_truth_Cluster1_Eta); */
      /* truth_particles->Branch("truth_Cluster1_Phi", &m_truth_Cluster1_Phi); */
      /* truth_particles->Branch("truth_Cluster1_Pt", &m_truth_Cluster1_Pt); */
      /* truth_particles->Branch("truth_Cluster1_xF", &m_truth_Cluster1_xF); */
      /* truth_particles->Branch("truth_Cluster1_Chi2", &m_truth_Cluster1_Chi2); */
      /* truth_particles->Branch("truth_Cluster1_Decay1EnergyFraction", &m_truth_Cluster1_Decay1EnergyFraction); */
      /* truth_particles->Branch("truth_Cluster1_Decay2EnergyFraction", &m_truth_Cluster1_Decay2EnergyFraction); */
      /* truth_particles->Branch("truth_Cluster2_Id", &m_truth_Cluster2_Id); */
      /* truth_particles->Branch("truth_Cluster2_E", &m_truth_Cluster2_E); */
      /* truth_particles->Branch("truth_Cluster2_Ecore", &m_truth_Cluster2_Ecore); */
      /* truth_particles->Branch("truth_Cluster2_Eta", &m_truth_Cluster2_Eta); */
      /* truth_particles->Branch("truth_Cluster2_Phi", &m_truth_Cluster2_Phi); */
      /* truth_particles->Branch("truth_Cluster2_Pt", &m_truth_Cluster2_Pt); */
      /* truth_particles->Branch("truth_Cluster2_xF", &m_truth_Cluster2_xF); */
      /* truth_particles->Branch("truth_Cluster2_Chi2", &m_truth_Cluster2_Chi2); */
      /* truth_particles->Branch("truth_Cluster2_Decay1EnergyFraction", &m_truth_Cluster2_Decay1EnergyFraction); */
      /* truth_particles->Branch("truth_Cluster2_Decay2EnergyFraction", &m_truth_Cluster2_Decay2EnergyFraction); */
  }

  /* clusters_Towers->MakeClass("cluster"); */
  /* truth_particles->MakeClass("truthParticles"); */

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::InitRun(PHCompositeNode *topNode)
{
  std::cout << "pythiaEMCalAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::process_event(PHCompositeNode *topNode)
{
  /* std::cout << "pythiaEMCalAna::process_event(PHCompositeNode *topNode) Processing event..." << std::endl; */
  n_events_total++;

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = nullptr;
  TowerInfoContainer *towerInfoContainer = nullptr;
  // again, node has different names in MC and RD
  if (isMonteCarlo) {
      towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
      /* std::cout << "isMonteCarlo=" << isMonteCarlo << ", towerContainer=" << towerContainer << "\n"; */
  /* else { */
  /*     std::cout << "Greg info: isMonteCarlo is false\n"; */
  /*     towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWERINFO_CALIB_CEMC"); */
  /*     std::cout << "Greg info: towerContainer is " << towerContainer << std::endl; */
  /* } */
      if(!towerContainer) {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }
  else {
      towerInfoContainer = findNode::getClass<TowerInfoContainer>(topNode,"TOWERINFO_CALIB_CEMC");
      if (!towerInfoContainer) {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node TOWERINFO_CALIB_CEMC"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }


  CaloEvalStack *caloevalstack = nullptr;
  /* CaloRawClusterEval *clustereval = nullptr; */
  if (isMonteCarlo) {
      caloevalstack = new CaloEvalStack(topNode, "CEMC");
      caloevalstack->next_event(topNode);
      m_clusterEval = caloevalstack->get_rawcluster_eval();
      if(!m_clusterEval)
      {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event cluster eval not available"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      } 
  }
  
  //Information on clusters
  /* RawClusterContainer *clusterContainer = nullptr; */
  // Name of node is different in MC and RD
  if (isMonteCarlo) {
      /* m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC"); */
      m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  }
  else {
      m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_CEMC");
      /* m_clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC"); */
  }
  if(!m_clusterContainer)
  {
      if (isMonteCarlo) std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
      else std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - CLUSTERINFO_CEMC node is missing. " << std::endl;
      return 0;
  }

  // Truth information
  /* PHG4TruthInfoContainer *truthinfo = nullptr; */
  if (isMonteCarlo) {
      m_truthInfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
      if(!m_truthInfo) {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node G4TruthInfo"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }
  
  // Minbias info
  if (!isMonteCarlo) {
      MinimumBiasInfo *minBiasInfo = findNode::getClass<MinimumBiasInfo>(topNode,"MinimumBiasInfo");
      bool isMinBias = (minBiasInfo) ? minBiasInfo->isAuAuMinimumBias() : false;
      if (!isMinBias) {
	  /* std::cout << PHWHERE << "pythiaEMCalAna::process_event Event is not minbias"  << std::endl; */
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }
  n_events_minbias++;

  //Vertex information
  GlobalVertex* gVtx = nullptr;
  PHG4VtxPoint* mcVtx = nullptr;
  // Problem is MC has a PrimaryVtx but no GlobalVertex, while RD has the opposite
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::VtxRange vtx_range = m_truthInfo->GetPrimaryVtxRange();
      PHG4TruthInfoContainer::ConstVtxIterator vtxIter = vtx_range.first;
      mcVtx = vtxIter->second;
      n_events_with_vertex++;
      if (abs(mcVtx->get_z()) > 10.0) {
	  /* std::cout << "Greg info: vertex z is " << mcVtx->get_z() << "\n"; */
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
      n_events_with_good_vertex++;
  }
  else {
      GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
      if (!vtxContainer)
      {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
	  assert(vtxContainer);  // force quit
	  return 0;
      }
      if (vtxContainer->empty())
      {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }

      //More vertex information
      else {
	  gVtx = vtxContainer->begin()->second;
	  if(!gVtx)
	  {
	      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find vtx from vtxContainer"  << std::endl;
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  n_events_with_vertex++;
	  // Require vertex to be within 10cm of 0
	  if (abs(gVtx->get_z()) > 10.0) {
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  n_events_with_good_vertex++;
      }
  }

  /* PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetPrimaryParticleRange(); */
  /* PHG4TruthInfoContainer::ConstIterator truthIter; */
  /* //from the HepMC event log */
  /* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
  /*   { */
  /*     //PHG4Particle* part = m_truthInfo->GetParticle(truthIter->second->get_trkid()) */

  //For eventgen ancestory information
  PHHepMCGenEventMap *genEventMap = nullptr;
  if (isMonteCarlo) {
    genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
    if(!genEventMap)
    {
	std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find PHHepMCGenEventMap"  << std::endl;
	return Fun4AllReturnCodes::ABORTEVENT;
    }
    /* std::cout << "genEventMap is " << genEventMap << " with size " << genEventMap->size() << "; more details:\n"; */
    /* genEventMap->identify(); */
 }

  //event level information of the above
  PHHepMCGenEvent *genEvent = nullptr; // = genEventMap -> get(getEvent);
  /* if(!genEvent) */
  /*   { */
  /*     std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find PHHepMCGenEvent"  << std::endl; */
  /*     return Fun4AllReturnCodes::ABORTEVENT; */
  /*   } */
  /* std::cout << "genEvent is " << genEvent << " with size " << genEvent->size() << "; more details:\n"; */
  /* genEvent->identify(); */
   
  /* HepMC::GenEvent *theEvent = nullptr; // = genEvent -> getEvent(); */

  //grab all the towers and fill their energies. 
  bool write_towers = true;
  if (write_towers) {
      if (isMonteCarlo) {
	  RawTowerContainer::ConstRange tower_range = towerContainer -> getTowers();
	  for(RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter!= tower_range.second; tower_iter++)
	  {
	      int phibin = tower_iter -> second -> get_binphi();
	      int etabin = tower_iter -> second -> get_bineta();
	      double phi = towergeom -> get_phicenter(phibin);
	      double eta = towergeom -> get_etacenter(etabin);
	      double energy = tower_iter -> second -> get_energy();

	      m_tower_energy.push_back(energy);
	      m_eta_center.push_back(eta);
	      m_phi_center.push_back(phi);
	  }
	  n_events_positiveCaloE++;
      }
      else {
	  unsigned int tower_range = towerInfoContainer->size();
	  double totalCaloE = 0;
	  int goodTowerCtr = 0;
	  for(unsigned int iter = 0; iter < tower_range; iter++)
	  {
	      unsigned int towerkey = towerInfoContainer->encode_key(iter);
	      unsigned int etabin = towerInfoContainer->getTowerEtaBin(towerkey);
	      unsigned int phibin = towerInfoContainer->getTowerPhiBin(towerkey);
	      double eta = towergeom -> get_etacenter(etabin);
	      double phi = towergeom -> get_phicenter(phibin);

	      TowerInfo* tower = towerInfoContainer->get_tower_at_channel(iter);
	      // check if tower is good
	      if(!tower->get_isGood()) continue;
	      double energy = tower->get_energy();
	      goodTowerCtr++;
	      totalCaloE += energy;
	      m_tower_energy.push_back(energy);
	      m_eta_center.push_back(eta);
	      m_phi_center.push_back(phi);
	  }
	  if (totalCaloE < 0) {
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
	  n_events_positiveCaloE++;
      }
  } // done writing tower info
  
  //grab all the clusters in the event
  bool write_clusters = true;
  bool apply_energy_cut = true;
  float clusterMinEnergyCut = 0.3; // in GeV
  if (write_clusters) {
      RawClusterContainer::ConstRange clusterEnd = m_clusterContainer -> getClusters();
      RawClusterContainer::ConstIterator clusterIter;
      /* std::cout << "\n\nBeginning cluster loop\n"; */
      for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
      {
	  RawCluster *recoCluster = clusterIter -> second;
	  CLHEP::Hep3Vector vertex;
	  if (isMonteCarlo) vertex = CLHEP::Hep3Vector(mcVtx->get_x(), mcVtx->get_y(), mcVtx->get_z());
	  else {
	      if (gVtx) vertex = CLHEP::Hep3Vector(gVtx->get_x(), gVtx->get_y(), gVtx->get_z());
	      else vertex = CLHEP::Hep3Vector(0,0,0);
	  }
	  /* vertex = CLHEP::Hep3Vector(0., 0., 0.); */
	  CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
	  float clusE = recoCluster->get_energy();
	  float clusEcore = recoCluster->get_ecore();
	  float clus_eta = E_vec_cluster.pseudoRapidity();
	  float clus_phi = E_vec_cluster.phi();
	  
	  if (apply_energy_cut) {
	      if (clusE < clusterMinEnergyCut) continue;
	  }

	  m_cluster_ID.push_back(recoCluster->get_id());
	  m_cluster_e.push_back(clusE);
	  m_cluster_eta.push_back(clus_eta);
	  m_cluster_phi.push_back(clus_phi);
	  m_cluster_ecore.push_back(clusEcore);
	  m_cluster_chi2.push_back(recoCluster -> get_chi2());
	  m_cluster_prob.push_back(recoCluster -> get_prob());
	  m_cluster_nTowers.push_back(recoCluster -> getNTowers());
	  // get all the towers in this cluster
	  std::vector<float> allE;
	  std::vector<float> allEta;
	  std::vector<float> allPhi;
	  RawCluster::TowerConstRange tower_range = recoCluster->get_towers();
	  RawCluster::TowerConstIterator tower_iter;
	  for (tower_iter = tower_range.first; tower_iter != tower_range.second; tower_iter++) {
	      double E = tower_iter->second;
	      allE.push_back(E);
	      unsigned int towerkey = tower_iter->first;
	      /* unsigned int etabin = towerInfoContainer->getTowerEtaBin(towerkey); */
	      unsigned int etabin = RawTowerDefs::decode_index1(towerkey);
	      /* unsigned int phibin = towerInfoContainer->getTowerPhiBin(towerkey); */
	      unsigned int phibin = RawTowerDefs::decode_index2(towerkey);
	      double eta = towergeom -> get_etacenter(etabin);
	      double phi = towergeom -> get_phicenter(phibin);
	      allEta.push_back(eta);
	      allPhi.push_back(phi);
	  }
	  m_cluster_allTowersE.push_back(allE);
	  m_cluster_allTowersEta.push_back(allEta);
	  m_cluster_allTowersPhi.push_back(allPhi);
	  

	  if (isMonteCarlo) {
	      /* std::cout << "\nEntering cluster isMonteCarlo for cluster "; */
	      /* recoCluster->identify(); */
	      std::set<PHG4Particle*> all_parts = m_clusterEval->all_truth_primary_particles(recoCluster);
	      // first add all relevant particles to allTrackIDs for later
	      for (auto& part : all_parts) {
		  allTrackIDs.insert(part->get_track_id());
	      }
	      if (all_parts.empty()) {
		  /* std::cout << "In cluster association, all_parts is empty. recoCluster is "; */
		  /* recoCluster->identify(); */
		  // noise cluster without any real particles creating the shower
		  /* m_cluster_maxE_trackID.push_back(-9999); */
		  /* m_cluster_maxE_PID.push_back(-9999); */
		  /* std::vector<float> all_cluster_trackIDs; */
		  /* m_cluster_all_trackIDs.push_back(all_cluster_trackIDs); */
		  m_cluster_nParticles.push_back(-9999);
		  m_cluster_primaryParticlePid.push_back(-9999);
		  std::vector<float> allPids;
		  m_cluster_allSecondaryPids.push_back(allPids);
	      }
	      else {
		  /* std::cout << "In cluster association, all_parts has " << all_parts.size() << " entries\n"; */
		  PHG4Particle* maxE_part = m_clusterEval->max_truth_primary_particle_by_energy(recoCluster);
		  assert(maxE_part);
		  std::vector<float> allPids;
		  int track_id = maxE_part->get_track_id();
		  int embedID = m_truthInfo->isEmbeded(track_id);
		  genEvent = genEventMap -> get(embedID);
		  m_theEvent = genEvent -> getEvent();
		  /* std::cout << "maxE_part is "; */
		  /* maxE_part->identify(); */
		  if (maxE_part->get_pid() == 22) {
		      // "primary" is a photon
		      if (isDirectPhoton(maxE_part)) {
			  /* std::cout << "maxE_part is a direct photon, adding primary ID\n"; */
			  m_cluster_primaryParticlePid.push_back(22);
		      }
		      else {
			  // it's a decay photon, get the parent form pythia
			  /* std::cout << "maxE_part is a decay photon\n"; */
			  HepMC::GenParticle* parent = GetHepMCParent(maxE_part);
			  /* std::cout << "Parent is "; */
			  /* parent->print(); */
			  m_cluster_primaryParticlePid.push_back(parent->pdg_id());
		      }
		  } // end photon check
		  else {
		      // if it's not a photon, add the PID directly
		      /* std::cout << "maxE_part is not a photon, adding it as primary\n"; */
		      m_cluster_primaryParticlePid.push_back(maxE_part->get_pid());
		  }
		  // now get the secondaries that produced the shower(s)
		  std::set<PHG4Shower*> showers = m_clusterEval->all_truth_primary_showers(recoCluster);
		  /* std::cout << "Cluster " << recoCluster << " has " << showers.size() << " showers\n"; */
		  m_cluster_nParticles.push_back(showers.size());
		  for (auto& shower : showers) {
		      int parent_id = shower->get_parent_particle_id();
		      PHG4Particle* par = m_truthInfo->GetParticle(parent_id);
		      /* std::cout << "Shower-producing particle is "; */
		      /* par->identify(); */
		      allPids.push_back(par->get_pid());
		  }
		  m_cluster_allSecondaryPids.push_back(allPids);

	      } // end case where all_parts isn't empty
	  } // end MC-specific stuff
      } // end loop over clusters
  }
  
  // Next grab the truth particles
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetPrimaryParticleRange();
      PHG4TruthInfoContainer::ConstIterator truthIter;
      /* std::cout << "\n\nGreg info: event " << n_events_total << ", starting loop over Geant primary particles\n"; */
      for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
      {
	  /* std::cout << "Starting new primary. n_primaries=" << n_primaries << "\n"; */
	  PHG4Particle *truthPar = truthIter->second;
	  int track_id = truthPar->get_track_id();
	  // only interested in particles which produced a cluster
	  if (allTrackIDs.count(track_id) == 0) continue;
	  /* std::cout << "\ttruthPar PID=" << truthPar->get_pid() << "; adding 1 primary\n"; */
	  n_primaries++;
	  /* std::cout << "\nIn main loop, n_primaries=" << n_primaries << ", truthPar is "; */
	  /* truthPar->identify(); */
	  int embedID = m_truthInfo->isEmbeded(track_id);
	  genEvent = genEventMap -> get(embedID);
	  m_theEvent = genEvent -> getEvent();
	  int pid = truthPar->get_pid();
	  if (abs(pid) > 99) {
	      /* std::cout << "Geant-decayed hadron. PID=" << pid << "\n"; */
	      /* truthPar->identify(); */
	      /* std::cout << "IsNeutralHadron=" << IsNeutralHadron(abs(pid)) << ", IsChargedHadron=" << IsChargedHadron(abs(pid)) << "\n"; */
	      // if it's not a photon, add it as a primary
	      /* addPrimaryFromGeant(truthPar); */
	      if (IsNeutralHadron(abs(pid))) {
		  n_neutral_hadrons++;
		  n_neutral_hadrons_geant++;
	      }
	      if (IsChargedHadron(abs(pid))) {
		  n_charged_hadrons++;
		  n_charged_hadrons_geant++;
	      }
	      /* PrintParent(truthPar); */
	      if (pid == 111 || pid == 221) {
	      /* if (false) { */
		  std::vector<PHG4Particle*> daughters = GetAllDaughters(truthPar);
		  if (daughters.size() == 2) {
		      if (!withinAcceptance(truthPar)) continue;
		      /* std::cout << "Greg info: found a geant primary pi0/eta with 2 daughters:\n"; */
		      /* PrintParent(truthPar); */
		      FillParent(truthPar);
		      PHG4Particle* decay1 = nullptr;
		      PHG4Particle* decay2 = nullptr;
		      GetBestDaughters(truthPar, decay1, decay2);
		      /* std::cout << "After GetBestDaughters: decay1=" << decay1 << "; decay2=" << decay2 << "\n"; */
		      /* std::cout << "Filling decay1 (geant decay)\n"; */
		      FillDecay("decay1", decay1, truthPar);
		      FillDecay("decay2", decay2, truthPar);
		  }
	      }
	  }
	  if (abs(pid) < 20) n_leptons++;
	  if (pid == 22) {
	      n_primary_photons++;
	      // if it is a photon, we have to check whether it's truly primary,
	      // or a decay photon handled in pythia
	      if (isDirectPhoton(truthPar)) {
		  // if it's really a direct photon, add it normally
		  /* addPrimaryFromGeant(truthPar); */
		  if (!withinAcceptance(truthPar)) continue;
		  FillParent(truthPar);
		  /* std::cout << "Filling decay1 (direct photon)\n"; */
		  FillDecay("decay1", nullptr, truthPar);
		  FillDecay("decay2", nullptr, truthPar);
		  n_direct_photons++;
	      }
	      else {
		  // "primary" photon is actually a decay photon from a decay handled by pythia
		  /* std::cout << "\tIt's a decay photon, subtracting 1 primary\n"; */
		  n_primaries--;
		  // find the parent in pythia and add that as the true primary
		  HepMC::GenParticle* parent = GetHepMCParent(truthPar);
		  /* std::cout << "\tnot a direct photon -- parent is\n"; */
		  /* parent->print(); */
		  // Check if we've already added this primary via another "primary" decay photon
		  if (primaryBarcodes.count(parent->barcode()) != 0) {
		      /* std::cout << "Already added HepMC primary " << parent << ":\n"; */
		      /* parent->print(); */
		      continue;
		  }
		  primaryBarcodes.insert(parent->barcode());
		  /* std::cout << "\tAdding parent as 1 primary\n"; */
		  n_primaries++;
		  pid = parent->pdg_id();
		  if (IsNeutralHadron(abs(pid))) {
		      n_neutral_hadrons++;
		      n_neutral_hadrons_pythia++;
		      n_pythia_decayed_hadrons++;
		  }
		  if (IsChargedHadron(abs(pid))) {
		      n_charged_hadrons++;
		      n_charged_hadrons_pythia++;
		      n_pythia_decayed_hadrons++;
		  }
		  if (parent->status() == 2 && abs(parent->pdg_id()) >= 100) {
		      // truthPar is a decay photon and parent is the primary hadron
		      /* addPrimaryFromPythia((*parent)); */
		      if (pid == 111 || pid == 221) {
			  /* if (!withinAcceptance(parent)) continue; */
			  /* std::cout << "Adding particles via FillTruth (pythia decay)\n"; */
			  FillTruth(parent);
		      }
		  }
		  else {
		      // weird case??
		      std::cout << "\nGreg info: weird combination of photon or parent status or PID... photon:\n";
		      truthPar->identify();
		      std::cout << "Parent:\n";
		      parent->print();
		      std::cout << "\n";
		  }
	      } // end decay photon case
	  } // end photon check
	  /* std::cout << "\tEnding primary. n_primaries=" << n_primaries << "\n"; */
      } // end loop over primary particles
  } // end grabbing truth info


  clusters_Towers -> Fill();
  if (isMonteCarlo) {
      truth_particles -> Fill();
      delete caloevalstack;
  }
  
      // Below is some example code showing cluster->particle matching 

      /* // Start with clusters, and find the corresponding truth particle(s) that */
      /* // produced the shower */
      /* RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters(); */
      /* RawClusterContainer::ConstIterator clusterIter; */
      /* for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++) */
      /* { */
	  /* RawCluster *recoCluster = clusterIter -> second; */
	  /* CLHEP::Hep3Vector vertex = CLHEP::Hep3Vector(mcVtx->get_x(), mcVtx->get_y(), mcVtx->get_z()); */
	  /* CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex); */
	  /* float clusE = E_vec_cluster.mag(); */
	  /* // Ignore clusters below 300MeV */
	  /* if (clusE < 0.3) continue; */

	  /* // all particles that contributed energy to this cluster */
	  /* std::set<PHG4Particle*> all_parts = clustereval->all_truth_primary_particles(recoCluster); */
	  /* // need at least one particle; if not, cluster is just noise */
	  /* if (all_parts.size() < 1) continue; */
	  /* // particle which contributed the most energy to this cluster */
	  /* PHG4Particle* max_part = clustereval->max_truth_primary_particle_by_energy(recoCluster); */
	  /* /1* if (max_part->get_pid() > 0) continue; *1/ */
	  /* std::cout << "Greg info: printing cluster info:\n"; */
	  /* std::cout << Form("id=%d, (E, phi, eta) = (%f, %f, %f)\n", recoCluster->get_id(), clusE, E_vec_cluster.phi(), E_vec_cluster.pseudoRapidity()); */
	  /* /1* recoCluster->identify(); *1/ */
	  /* std::cout << "Corresponding particle info: (all_parts size=" << all_parts.size() << ", max_part=" << max_part << ")\n"; */
	  /* max_part->identify(); */
	  /* std::set<RawCluster*> max_part_clusters = clustereval->all_clusters_from(max_part); */
	  /* std::cout << "max_part contributed to " << max_part_clusters.size() << " clusters; printing those clusters\n"; */
	  /* for (auto& cl : max_part_clusters) { */
	      /* cl->identify(); */
	  /* } */
	  /* std::cout << "\n"; */
      /* } */


      // Below shows how to loop over all the different HepMC events in this Fun4All event

      /* PHHepMCGenEventMap::Iter genEventIter; */
      /* for (genEventIter = genEventMap->begin(); genEventIter != genEventMap->end(); genEventIter++) { */
	  /* int embedID = genEventIter->first; */
	  /* // For HIJING embedded samples, the PYTHIA event has a positive embedID */
	  /* if (hasHIJING && embedID < 1) continue; */
	  /* if (embedID < 1) continue; */
	  /* genEvent = genEventIter->second; */
	  /* /1* std::cout << "Greg info: embedID=" << embedID << "; printing genEvent\n"; *1/ */
	  /* /1* genEvent->identify(); *1/ */
	  /* theEvent = genEvent->getEvent(); */
	  /* /1* std::cout << "Greg info: printing theEvent\n"; *1/ */
	  /* /1* theEvent->print(); *1/ */
	  /* /1* std::cout << Form("Greg info: embedID=%d, genEvent->is_simulated()=%d\n", embedID, genEvent->is_simulated()); *1/ */
	  /* // now loop over PYTHIA particles */
	  /* for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p) */
	  /* { */
	      /* assert(*p); */
	      /* // look for photons only */
	      /* if ((*p)->pdg_id() == 22) { */
		  /* // only want final-state particles, i.e. status = 1 */
		  /* if ((*p)->status() != 1) continue; */
		  /* // get the photon's parent */
		  /* HepMC::GenVertex* prod_vtx = (*p)->production_vertex(); */
		  /* if (prod_vtx->particles_in_size() == 1) { */
		      /* HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin(); */
		      /* assert((*parent)); */
		      /* // parent's status and pdg_id tell us whether it's a direct photon */
		      /* // or a decay photon */
		      /* if ((*parent)->status() > 2 && abs((*parent)->pdg_id()) < 100) { */
			  /* // direct photon */
			  /* /1* addPrimaryFromPythia((*p)); *1/ */
			  /* n_pythia_direct_photons++; */
		      /* } */
		      /* else if ((*parent)->status() == 2 && abs((*parent)->pdg_id()) >= 100) { */
			  /* // decay photon */
			  /* n_pythia_decay_photons++; */
			  /* if (withinAcceptance((*p))) n_pythia_decay_photons_in_acceptance++; */
			  /* std::pair<int,int> embedBarcode = std::pair<int,int>(embedID, (*parent)->barcode()); */
			  /* if (! vector_contains(embedBarcode, pythia_primary_barcodes) ) { */
			      /* pythia_primary_barcodes.push_back(embedBarcode); */
			      /* n_pythia_decays++; */
			      /* if (withinAcceptance((*parent))) n_pythia_decays_in_acceptance++; */
			      /* if ( (*parent)->pdg_id() == 111 ) { */
				  /* n_pythia_decayed_pi0s++; */
				  /* if (withinAcceptance((*parent))) n_pythia_decayed_pi0s_in_acceptance++; */
			      /* } */
			  /* } */

		      /* } */
		      /* else { */
			  /* // weird case?? */
			  /* std::cout << "\nGreg info: weird combination of photon or parent status or PID... photon:\n"; */
			  /* (*p)->print(); */
			  /* std::cout << "Parent:\n"; */
			  /* (*parent)->print(); */
			  /* std::cout << "\n"; */
		      /* } */
		  /* } */
		  /* else { */
		      /* // weird photon -- they should only have 1 parent */
		      /* std::cout << "\nGreg info: in PYTHIA check, found a photon with " << prod_vtx->particles_in_size() << " parent(s). Photon:\n"; */
		      /* (*p)->print(); */
		      /* for (HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin(); parent != prod_vtx->particles_in_const_end(); parent++) { */
			  /* std::cout << "Parent:\n"; */
			  /* (*parent)->print(); */
		      /* } */
		      /* std::cout << "\n"; */
		  /* } */
	      /* } // end photon check */
	      /* else { */
		  /* // check for (final state) non-decayed pi0s */
		  /* if ( (*p)->status() == 1 && (*p)->pdg_id() > 100) { */
		      /* n_pythia_nondecayed_hadrons++; */
		      /* if (withinAcceptance((*p))) n_pythia_nondecayed_hadrons_in_acceptance++; */
		      /* if ( (*p)->pdg_id() == 111 ) { */
			  /* n_pythia_nondecayed_pi0s++; */
			  /* if (withinAcceptance((*p))) n_pythia_nondecayed_pi0s_in_acceptance++; */
		      /* } */
		  /* } */
	      /* } */
	  /* } // end PYTHIA loop */
      /* } // end loop over embedID */
      /* std::cout << "PYTHIA: " << n_pythia << " total particles, " << n_pho_pythia << " photons, " << n_pi0_pythia << " pi0s\n"; */


      // Below is the old code I used to find truth photons
      // Warning! Likely will not work after uncommenting. Should only be used as an example

      // Next check for decays handled by Geant
      // Start with primary particles from GEANT
      // Look for photons and decide if they are direct or decay photons
      /* PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange(); */
      /* PHG4TruthInfoContainer::ConstIterator truthIter; */
      /* /1* std::cout << "\n\nGreg info: starting loop over Geant primary particles\n"; *1/ */
      /* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
      /* { */
	  /* PHG4Particle *truthPar = truthIter->second; */
	  /* int embedID = truthinfo->isEmbeded(truthPar->get_track_id()); */
	  /* if (hasHIJING && embedID < 1) continue; */
	  /* if (embedID < 1) continue; */
	  /* int geant_barcode = truthPar->get_barcode(); */
	  /* int pid = truthPar->get_pid(); */
	  /* // look for photons only */
	  /* if (pid == 22) { */
	      /* /1* std::cout << "\nFound a primary photon (embedID=" << embedID << ", barcode=" << geant_barcode << "); details:\t"; *1/ */
	      /* /1* truthPar->identify(); *1/ */
	      /* genEvent = genEventMap -> get(embedID); */
	      /* theEvent = genEvent -> getEvent(); */
	      /* // is it a direct photon? */
	      /* if (isDirectPhoton(truthPar, theEvent)) { */
		  /* /1* std::cout << "\tbarcode " << geant_barcode << " is a direct photon... adding to list\n"; *1/ */
		  /* n_direct_photons++; */
		  /* addDirectPhoton(truthPar, truthinfo); */
		  /* TLorentzVector truth_momentum; */
		  /* truth_momentum.SetPxPyPzE(truthPar->get_px(), truthPar->get_py(), truthPar->get_pz(), truthPar->get_e()); */
		  /* float eta = truth_momentum.PseudoRapidity(); */
		  /* if (abs(eta) <= 1.1) n_direct_photons_in_acceptance++; */
	      /* } */
	      /* else { */
		  /* // decay photon */
		  /* /1* std::cout << "\tbarcode " << geant_barcode << " is a decay photon... check parent info\n"; *1/ */
		  /* n_decay_photons++; */
		  /* if (withinAcceptance(truthPar)) n_decay_photons_in_acceptance++; */
		  /* addDecayPhoton(truthPar, truthinfo, theEvent); */
		  /* // find the parent and add it as a primary */
		  /* // primary photon means geant doesn't know about the parent */
		  /* // so match this photon to the pythia photon */
		  /* HepMC::GenParticle* pho = getGenParticle(geant_barcode, theEvent); */
		  /* /1* assert(pho); *1/ */
		  /* if (!pho) { */
		      /* // problem -- we couldn't find the corresponding pythia particle */
		      /* std::cout << "\t\tGreg info: skipping Geant primary with barcode " << geant_barcode << " because corresponding pythia particle could not be found; more details:\t"; */
		      /* truthPar->identify(); */
		      /* continue; */
		  /* } */
		  /* // now find the parent in pythia */
		  /* HepMC::GenVertex* prod_vtx = pho->production_vertex(); */
		  /* if (prod_vtx->particles_in_size() == 1) { */
		      /* HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin(); */
		      /* assert((*parent)); */
		      /* /1* std::cout << "\t\tParent barcode=" << (*parent)->barcode() << ", id=" << (*parent)->pdg_id() << "... "; *1/ */
		      /* std::pair<int,int> embedBarcode(embedID, (*parent)->barcode()); */
		      /* if (! vector_contains(embedBarcode, primaryBarcodes) ) { */
			  /* /1* std::cout << "adding to primaries.\n"; *1/ */
			  /* primaryBarcodes.push_back(embedBarcode); */
			  /* n_primary++; */
			  /* if (withinAcceptance((*parent))) n_primary_in_acceptance++; */
			  /* /1* n_pythia_decays++; *1/ */
			  /* /1* std::cout << "adding primary from pythia. PID=" << (*parent)->pdg_id() << "\n"; *1/ */
			  /* addPrimaryHadronFromPythia((*parent), embedID); */
		      /* } */
		      /* else { */
			  /* /1* std::cout << "already added this primary.\n"; *1/ */
		      /* } */
		  /* } */
		  /* else { */
		      /* std::cout << "\t\tGreg info: pythia-decayed photon with " << prod_vtx->particles_in_size() << " parents. Skipping...\n"; */
		  /* } */
	      /* } */
	  /* } // end photon check */
	  /* else { */
	      /* if (pid > 100) { */
		  /* n_geant_primary_hadrons++; */
		  /* if (withinAcceptance(truthPar)) n_geant_primary_hadrons_in_acceptance++; */
	      /* } */
	      /* if (pid == 111) { */
		  /* n_geant_primary_pi0s++; */
		  /* if (withinAcceptance(truthPar)) n_geant_primary_pi0s_in_acceptance++; */
	      /* } */
	  /* } */
      /* } // end loop over geant primary particles */


      // Now loop over GEANT secondary particles, taking only the daughters
      // of primary particles we already found
      /* truthRange = truthinfo->GetSecondaryParticleRange(); */
      /* /1* std::cout << "Greg info: starting loop over Geant secondary particles\n"; *1/ */
      /* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
      /* { */
	  /* PHG4Particle *truthPar = truthIter->second; */
	  /* int embedID = truthinfo->isEmbeded(truthPar->get_track_id()); */
	  /* if (hasHIJING && embedID < 1) continue; */
	  /* if (embedID < 1) continue; */
	  /* // only looking for decay photons and their corresponding primaries */
	  /* if (truthPar->get_pid() == 22) { */
	      /* // get the parent */
	      /* PHG4Particle* parent = truthinfo->GetParticle(truthPar->get_parent_id()); */
	      /* // make sure parent is a primary */
	      /* if ( truthinfo->is_primary(parent) ) { */
		  /* // make sure the "parent" isn't also a photon, i.e. the */ 
		  /* // same photon before and after an interaction */
		  /* if (parent->get_pid() == 22) continue; */

		  /* /1* std::cout << "\tFound a secondary photon (barcode=" << truthPar->get_barcode() << ")\n"; *1/ */
		  /* /1* std::cout << "\t\tParent is primary (barcode=" << parent->get_barcode() << ", id=" << parent->get_pid() << "), "; *1/ */
		  /* n_decay_photons++; */
		  /* if (withinAcceptance(truthPar)) n_decay_photons_in_acceptance++; */
		  /* n_geant_decay_photons++; */
		  /* if (withinAcceptance(truthPar)) n_geant_decay_photons_in_acceptance++; */
		  /* addDecayPhoton(truthPar, truthinfo, theEvent); */
		  /* int embedID = truthinfo->isEmbeded(parent->get_track_id()); */
		  /* std::pair<int,int> embedBarcode(embedID, parent->get_barcode()); */
		  /* /1* if (parent->get_pid() == 111) { *1/ */
		  /* /1* std::cout << "Found a primary pi0 with embedID " << embedID << ", barcode " << parent->get_barcode() << "\n"; *1/ */
		  /* /1* } *1/ */
		  /* if (! vector_contains(embedBarcode, primaryBarcodes) ) { */
		      /* /1* std::cout << "adding to primaries.\n"; *1/ */
		      /* primaryBarcodes.push_back(embedBarcode); */
		      /* n_primary++; */
		      /* if (withinAcceptance(parent)) n_primary_in_acceptance++; */
		      /* n_geant_decays++; */
		      /* if (withinAcceptance(parent)) n_geant_decays_in_acceptance++; */

		      /* /1* std::cout << "\tadded primary with embedID " << embedID << ", barcode " << parent->get_barcode() << "\n"; *1/ */
		      /* /1* std::cout << "Adding primary from geant. PID=" << parent->get_pid() << "\n"; *1/ */
		      /* addPrimaryHadronFromGeant(parent, truthinfo); */
		      /* /1* std::cout << "\nFound a geant-decayed primary:\n"; *1/ */
		      /* /1* parent->identify(); *1/ */
		      /* /1* std::cout << "Daughter photon is\n"; *1/ */
		      /* /1* truthPar->identify(); *1/ */
		      /* /1* std::cout << "\n"; *1/ */
		  /* } */
		  /* else { */
		      /* /1* std::cout << "already added this primary.\n"; *1/ */
		  /* } */
	      /* } */
	      /* /1* else { *1/ */
	      /* /1*     std::cout << "\t\tParent is *not* primary. Skipping!\n"; *1/ */
	      /* /1* } *1/ */
	  /* } //end photon check */
      /* } // end loop over geant secondaries */


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::ResetEvent(PHCompositeNode *topNode)
{

  m_tower_energy.clear();
  m_eta_center.clear();
  m_phi_center.clear();
  m_cluster_ID.clear();
  m_cluster_e.clear();
  m_cluster_eta.clear();
  m_cluster_phi.clear();
  m_cluster_ecore.clear();
  m_cluster_chi2.clear();
  m_cluster_prob.clear();
  m_cluster_nTowers.clear();
  m_cluster_allTowersE.clear();
  m_cluster_allTowersEta.clear();
  m_cluster_allTowersPhi.clear();
  m_cluster_nParticles.clear();
  m_cluster_primaryParticlePid.clear();
  m_cluster_allSecondaryPids.clear();
  /* m_cluster_maxE_trackID.clear(); */
  /* m_cluster_maxE_PID.clear(); */
  /* m_cluster_all_trackIDs.clear(); */
  m_truth_Parent_Barcode.clear();
  m_truth_Parent_Pid.clear();
  m_truth_Parent_M.clear();
  m_truth_Parent_E.clear();
  m_truth_Parent_Eta.clear();
  m_truth_Parent_Phi.clear();
  m_truth_Parent_Pt.clear();
  m_truth_Parent_xF.clear();
  m_truth_Parent_EndVtx_x.clear();
  m_truth_Parent_EndVtx_y.clear();
  m_truth_Parent_EndVtx_z.clear();
  /* m_truth_Parent_EndVtx_t.clear(); */
  m_truth_Parent_EndVtx_r.clear();
  /* m_truth_Parent_TotalNDaughters.clear(); */
  /* m_truth_Parent_AllDaughterPids.clear(); */
  /* m_truth_Parent_AllDaughterEnergyFractions.clear(); */
  m_truth_Parent_TotalNClusters.clear();
  m_truth_Parent_AllClusterIDs.clear();
  m_truth_Parent_AllClusterEnergyFractions.clear();
  m_truth_Decay1_Barcode.clear();
  m_truth_Decay1_Pid.clear();
  m_truth_Decay1_M.clear();
  m_truth_Decay1_E.clear();
  m_truth_Decay1_Eta.clear();
  m_truth_Decay1_Phi.clear();
  m_truth_Decay1_Pt.clear();
  m_truth_Decay1_xF.clear();
  m_truth_Decay1_EndVtx_x.clear();
  m_truth_Decay1_EndVtx_y.clear();
  m_truth_Decay1_EndVtx_z.clear();
  /* m_truth_Decay1_EndVtx_t.clear(); */
  m_truth_Decay1_EndVtx_r.clear();
  m_truth_Decay1_TotalNClusters.clear();
  m_truth_Decay1_BestClusterID.clear();
  m_truth_Decay1_BestClusterEfraction.clear();
  /* m_truth_Decay1_AllClusterIDs.clear(); */
  /* m_truth_Decay1_AllClusterEnergyFractions.clear(); */
  m_truth_Decay2_Barcode.clear();
  m_truth_Decay2_Pid.clear();
  m_truth_Decay2_M.clear();
  m_truth_Decay2_E.clear();
  m_truth_Decay2_Eta.clear();
  m_truth_Decay2_Phi.clear();
  m_truth_Decay2_Pt.clear();
  m_truth_Decay2_xF.clear();
  m_truth_Decay2_EndVtx_x.clear();
  m_truth_Decay2_EndVtx_y.clear();
  m_truth_Decay2_EndVtx_z.clear();
  /* m_truth_Decay2_EndVtx_t.clear(); */
  m_truth_Decay2_EndVtx_r.clear();
  m_truth_Decay2_TotalNClusters.clear();
  m_truth_Decay2_BestClusterID.clear();
  m_truth_Decay2_BestClusterEfraction.clear();
  /* m_truth_Decay2_AllClusterIDs.clear(); */
  /* m_truth_Decay2_AllClusterEnergyFractions.clear(); */
  /* m_truth_Cluster1_Id.clear(); */
  /* m_truth_Cluster1_E.clear(); */
  /* m_truth_Cluster1_Ecore.clear(); */
  /* m_truth_Cluster1_Eta.clear(); */
  /* m_truth_Cluster1_Phi.clear(); */
  /* m_truth_Cluster1_Pt.clear(); */
  /* m_truth_Cluster1_xF.clear(); */
  /* m_truth_Cluster1_Chi2.clear(); */
  /* m_truth_Cluster1_Decay1EnergyFraction.clear(); */
  /* m_truth_Cluster1_Decay2EnergyFraction.clear(); */
  /* m_truth_Cluster2_Id.clear(); */
  /* m_truth_Cluster2_E.clear(); */
  /* m_truth_Cluster2_Ecore.clear(); */
  /* m_truth_Cluster2_Eta.clear(); */
  /* m_truth_Cluster2_Phi.clear(); */
  /* m_truth_Cluster2_Pt.clear(); */
  /* m_truth_Cluster2_xF.clear(); */
  /* m_truth_Cluster2_Chi2.clear(); */
  /* m_truth_Cluster2_Decay1EnergyFraction.clear(); */
  /* m_truth_Cluster2_Decay2EnergyFraction.clear(); */
  allTrackIDs.clear();
  primaryBarcodes.clear();
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::EndRun(const int runnumber)
{
  std::cout << "pythiaEMCalAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::End(PHCompositeNode *topNode)
{
  std::cout << "pythiaEMCalAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  std::cout << Form("Total # events: %ld\n", n_events_total);
  std::cout << Form("\t# minbias: %ld (%.2f%% total)\n", n_events_minbias, (float)(100.0*n_events_minbias/n_events_total));
  std::cout << Form("\t# w/ vertex: %ld (%.2f%% total, %.2f%% minbias)\n", n_events_with_vertex, (float)(100.0*n_events_with_vertex/n_events_total), (float)(100.0*n_events_with_vertex/n_events_minbias));
  std::cout << Form("\t# w/ good vertex: %ld (%.2f%% total, %.2f%% minbias, %.2f%% w/ vertex)\n", n_events_with_good_vertex, (float)(100.0*n_events_with_good_vertex/n_events_total), (float)(100.0*n_events_with_good_vertex/n_events_minbias), (float)(100.0*n_events_with_good_vertex/n_events_with_vertex));
  std::cout << Form("\t# w/ positive calo E: %ld (%.2f%% total, %.2f%% minbias, %.2f%% w/ vertex, %.2f%% w/ good vertex)\n", n_events_positiveCaloE, (float)(100.0*n_events_positiveCaloE/n_events_total), (float)(100.0*n_events_positiveCaloE/n_events_minbias), (float)(100.0*n_events_positiveCaloE/n_events_with_vertex), (float)(100.0*n_events_positiveCaloE/n_events_with_good_vertex));
  if (isMonteCarlo) {
      std::cout << Form("Total primary particles: %ld\n", n_primaries);
      std::cout << Form("Total primary hadrons decayed by pythia: %ld\n", n_pythia_decayed_hadrons);
      std::cout << Form("Total \"primary\" photons: %ld\n", n_primary_photons);
      std::cout << Form("Total direct photons: %ld\n", n_direct_photons);
      std::cout << Form("Total neutral hadrons: %ld (%ld geant, %ld pythia)\n", n_neutral_hadrons, n_neutral_hadrons_geant, n_neutral_hadrons_pythia);
      std::cout << Form("Total charged hadrons: %ld (%ld geant, %ld pythia)\n", n_charged_hadrons, n_charged_hadrons_geant, n_charged_hadrons_pythia);
      std::cout << Form("Total primary leptons: %ld\n", n_leptons);
  }
  fout -> cd();
  std::cout << "Writing clusters_Towers\n";
  clusters_Towers -> Write();
  if (isMonteCarlo) {
      std::cout << "Writing truth_particles\n";
      truth_particles -> Write();
  }
  fout -> Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::Reset(PHCompositeNode *topNode)
{
  std::cout << "pythiaEMCalAna::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void pythiaEMCalAna::Print(const std::string &what) const
{
  std::cout << "pythiaEMCalAna::Print(const std::string &what) const Printing info for " << what << std::endl;
}

/* void pythiaEMCalAna::addPrimaryFromGeant(PHG4Particle* part) { */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     if (! withinAcceptance(part)) return; */
/*     int this_id = part->get_track_id(); */
/*     n_primaries++; */
    
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id); */
/*     if (!end_vtx) { */
/* 	std::cout << "\nGreg info: no end vertex found for Geant-decayed primary:\n"; */
/* 	part->identify(); */
/* 	std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); */
/* 	return; */
/*     } */

/*     /1* int embedID = truthInfo->isEmbeded(part->get_track_id()); *1/ */
/*     /1* m_truthBarcode.push_back(std::pair<int,int>(embedID, part->get_barcode())); *1/ */
/*     /1* m_truthParentBarcode.push_back(std::pair<int,int>(embedID, 0)); *1/ */
/*     if (part->get_pid() == 22) { */
/* 	/1* n_primary_photons++; *1/ */
/* 	// check for direct photons is done in process_event */
/* 	n_direct_photons++; */
/*     } */
/*     if (abs(part->get_pid()) < 20) n_leptons++; */
/*     if (abs(part->get_pid()) > 100) { */
/* 	if (IsNeutralHadron(abs(part->get_pid()))) { */
/* 	    n_neutral_hadrons++; */
/* 	    n_neutral_hadrons_geant++; */
/* 	} */
/* 	else { */
/* 	    n_charged_hadrons++; */
/* 	    n_charged_hadrons_geant++; */
/* 	} */
/*     } */

/*     m_truthIsPrimary.push_back(1); */
/*     m_truthTrackID.push_back(this_id); */
/*     m_truthPid.push_back(part->get_pid()); */

/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */

/*     std::vector<float> allClusterIDs; */

/*     std::set<RawCluster*> clusters = m_clusterEval->all_clusters_from(part); */
/*     for (auto& cl : clusters) { */
/* 	allClusterIDs.push_back(cl->get_id()); */
/*     } */
/*     m_truth_all_clusterIDs.push_back(allClusterIDs); */
/* } */


/* void pythiaEMCalAna::addPrimaryFromPythia(HepMC::GenParticle* part) { */
/*     /1* std::cout << "Entering pythiaEMCalAna::addPrimaryFromPythia\npart=" << part << "\n"; *1/ */
/*     HepMC::FourVector mom = part->momentum(); */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e()); */
/*     if (! withinAcceptance(part)) return; */

/*     primaryBarcodes.insert(part->barcode()); */
/*     n_primaries++; */
/*     n_pythia_decayed_hadrons++; */
/*     if (IsNeutralHadron(abs(part->pdg_id()))) { */
/* 	n_neutral_hadrons++; */
/* 	n_neutral_hadrons_pythia++; */
/*     } */
/*     else { */
/* 	n_charged_hadrons++; */
/* 	n_charged_hadrons_pythia++; */
/*     } */

/*     m_truthIsPrimary.push_back(1); */
/*     // no track ID available from pythia */
/*     m_truthTrackID.push_back(-999); */
/*     m_truthPid.push_back(part->pdg_id()); */

/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     /1* std::cout << "Getting end vertex\n"; *1/ */
/*     HepMC::GenVertex* end_vtx = part->end_vertex(); */
/*     assert(end_vtx); */
/*     /1* std::cout << "Found end vertex in pythia\n"; *1/ */
/*     HepMC::FourVector position = end_vtx->position(); */

/*     m_truthEndVtx_x.push_back(position.x()); */
/*     m_truthEndVtx_y.push_back(position.y()); */
/*     m_truthEndVtx_z.push_back(position.z()); */
/*     m_truthEndVtx_t.push_back(position.t()); */
/*     float x = position.x(); */
/*     float y = position.y(); */
/*     float r = sqrt(x*x + y*y); */
/*     /1* std::cout << Form("pythiaEMCalAna::addPrimaryFromPythia(): Vertex r=%f, perp=%f\n", r, position.perp()); *1/ */
/*     m_truthEndVtx_r.push_back(r); */

/*     // no cluster matching available from pythia */
/*     std::vector<float> allClusterIDs; */
/*     m_truth_all_clusterIDs.push_back(allClusterIDs); */
/* } */

bool pythiaEMCalAna::isDirectPhoton(PHG4Particle* part) {
    /* std::cout << "\tGreg info: entering isDirectPhoton(). "; */
    /* std::cout << "G4 particle is " << part << ", barcode " << part->get_barcode() << "\n";//; printing info\n"; */
    /* part->identify(); */
    
    // Only bother with this check if we have PYTHIA information
    // Otherwise assume any photon is a "direct" photon, since we can't tell from HIJING alone
    if (!hasPYTHIA) return (part->get_pid() == 22);
    HepMC::GenParticle* genpart = getGenParticle(part->get_barcode());
    if (!genpart) {
	std::cout << "\t\tGreg info: in isDirectPhoton(), could not find pythia particle with barcode " << part->get_barcode() << "; returning true. (This may be an error!)\n";
	return true;
    }
    assert(genpart);
    /* std::cout << "\tFound corresponding pythia particle; printing info\t"; */
    /* genpart->print(); */
    /* else std::cout << "Greg info: found corresponding pythia photon (" << genpart << ")\n"; */
    HepMC::GenVertex* prod_vtx = genpart->production_vertex();
    if (prod_vtx->particles_in_size() == 1) {
	HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin();
	assert((*parent));
	// parent's status and pdg_id tell us whether it's a direct photon
	// or a decay photon
	if ((*parent)->status() > 2 && abs((*parent)->pdg_id()) < 100) {
	    // direct photon
	    /* std::cout << "\nGreg info: found a direct photon. Photon:\n"; */
	    /* genpart->print(); */
	    bool printHistory = false;
	    if (printHistory) {
		// print history of all ancestors
		// generation 0 is genpart, gen -1 is its parent, etc.
		int generation = -1;
		std::cout << "\tGeneration " << generation << " -- ";
		(*parent)->print();
		while (true) {
		    generation--;
		    HepMC::GenVertex::particles_in_const_iterator parentparent;
		    HepMC::GenVertex* parent_prod_vtx = (*parent)->production_vertex();
		    if (parent_prod_vtx) {
			parentparent = parent_prod_vtx->particles_in_const_begin();
			std::cout << "\tGeneration " << generation << ": ";
			(*parentparent)->print();
			parent = parentparent;
		    }
		    else break;
		}
		parent = prod_vtx->particles_in_const_begin(); // reset parent to genpart's actual parent
	    }
	    return true;
	}
	else if ((*parent)->status() == 2 && abs((*parent)->pdg_id()) >= 100) {
	    // decay photon
	    /* std::cout << "\nGreg info: found a decay photon. Photon:\n"; */
	    /* genpart->print(); */
	    /* std::cout << "Parent:\n"; */
	    /* (*parent)->print(); */
	    /* std::cout << "\n"; */
	    return false;
	}
	else {
	    // weird case??
	    std::cout << "\nGreg info: weird combination of photon or parent status or PID... photon:\n";
	    genpart->print();
	    std::cout << "Parent:\n";
	    (*parent)->print();
	    std::cout << "\n";
	    return false;
	}
    } // single parent check
    else {
	// weird photon -- they should only have 1 parent
	std::cout << "Greg info: found a photon with " << prod_vtx->particles_in_size() << " parent(s). Photon:\n";
	genpart->print();
	for (HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin(); parent != prod_vtx->particles_in_const_end(); parent++) {
	    std::cout << "Parent:\n";
	    (*parent)->print();
	}
	/* std::cout << "\n"; */
	return false;
    }
}

void pythiaEMCalAna::FillTruthParticle(std::string which, PHG4Particle* par) {
    std::vector<float>* Barcode = nullptr;
    std::vector<float>* Pid = nullptr;
    std::vector<float>* M = nullptr;
    std::vector<float>* E = nullptr;
    std::vector<float>* Eta = nullptr;
    std::vector<float>* Phi = nullptr;
    std::vector<float>* Pt = nullptr;
    std::vector<float>* xF = nullptr;
    std::vector<float>* EndVtx_x = nullptr;
    std::vector<float>* EndVtx_y = nullptr;
    std::vector<float>* EndVtx_z = nullptr;
    /* std::vector<float>* EndVtx_t = nullptr; */
    std::vector<float>* EndVtx_r = nullptr;
    
    if (which == "parent") {
	Barcode = &m_truth_Parent_Barcode;
	Pid = &m_truth_Parent_Pid;
	M = &m_truth_Parent_M;
	E = &m_truth_Parent_E;
	Eta = &m_truth_Parent_Eta;
	Phi = &m_truth_Parent_Phi;
	Pt = &m_truth_Parent_Pt;
	xF = &m_truth_Parent_xF;
	EndVtx_x = &m_truth_Parent_EndVtx_x;
	EndVtx_y = &m_truth_Parent_EndVtx_y;
	EndVtx_z = &m_truth_Parent_EndVtx_z;
	/* EndVtx_t = &m_truth_Parent_EndVtx_t; */
	EndVtx_r = &m_truth_Parent_EndVtx_r;
    }
    if (which == "decay1") {
	/* std::cout << "In FillTruthParticle: setting decay1\n"; */
	Barcode = &m_truth_Decay1_Barcode;
	Pid = &m_truth_Decay1_Pid;
	M = &m_truth_Decay1_M;
	E = &m_truth_Decay1_E;
	Eta = &m_truth_Decay1_Eta;
	Phi = &m_truth_Decay1_Phi;
	Pt = &m_truth_Decay1_Pt;
	xF = &m_truth_Decay1_xF;
	EndVtx_x = &m_truth_Decay1_EndVtx_x;
	EndVtx_y = &m_truth_Decay1_EndVtx_y;
	EndVtx_z = &m_truth_Decay1_EndVtx_z;
	/* EndVtx_t = &m_truth_Decay1_EndVtx_t; */
	EndVtx_r = &m_truth_Decay1_EndVtx_r;
    }
    if (which == "decay2") {
	Barcode = &m_truth_Decay2_Barcode;
	Pid = &m_truth_Decay2_Pid;
	M = &m_truth_Decay2_M;
	E = &m_truth_Decay2_E;
	Eta = &m_truth_Decay2_Eta;
	Phi = &m_truth_Decay2_Phi;
	Pt = &m_truth_Decay2_Pt;
	xF = &m_truth_Decay2_xF;
	EndVtx_x = &m_truth_Decay2_EndVtx_x;
	EndVtx_y = &m_truth_Decay2_EndVtx_y;
	EndVtx_z = &m_truth_Decay2_EndVtx_z;
	/* EndVtx_t = &m_truth_Decay2_EndVtx_t; */
	EndVtx_r = &m_truth_Decay2_EndVtx_r;
    }

    if (!par) {
	/* std::cout << "In FillTruthParticle: pushing back for nullptr\n"; */
	Barcode->push_back(-9999);
	Pid->push_back(-1);
	M->push_back(-9999);
	E->push_back(-9999);
	Eta->push_back(-9999);
	Phi->push_back(-9999);
	Pt->push_back(-9999);
	xF->push_back(-9999);
	EndVtx_x->push_back(-9999);
	EndVtx_y->push_back(-9999);
	EndVtx_z->push_back(-9999);
	/* EndVtx_t->push_back(-9999); */
	EndVtx_r->push_back(-9999);
	return;
    }
    else {
	if (which == "parent" && !withinAcceptance(par)) return;
	TLorentzVector truth_momentum;
	truth_momentum.SetPxPyPzE(par->get_px(), par->get_py(), par->get_pz(), par->get_e());
	int this_id = par->get_track_id();

	/* std::cout << "In FillTruthParticle: pushing back barcode " << par->get_barcode() << ", E=" << par->get_e() << " for " << which << "\n"; */
	Barcode->push_back(par->get_barcode());
	Pid->push_back(par->get_pid());
	M->push_back(truth_momentum.M());
	E->push_back(truth_momentum.E());
	Eta->push_back(truth_momentum.PseudoRapidity());
	Phi->push_back(truth_momentum.Phi());
	Pt->push_back(truth_momentum.Pt());
	float xf = 2.0*truth_momentum.Pz()/200.0;
	xF->push_back(xf);

	PHG4VtxPoint* end_vtx = getG4EndVtx(this_id);
	if (!end_vtx) {
	    /* std::cout << "\nGreg info: no end vertex found for Geant-decayed particle:\n"; */
	    /* par->identify(); */
	    /* std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); */
	    EndVtx_x->push_back(-9999);
	    EndVtx_y->push_back(-9999);
	    EndVtx_z->push_back(-9999);
	    /* EndVtx_t->push_back(-9999); */
	    EndVtx_r->push_back(-9999);
	    /* return; */
	}
	else {
	    EndVtx_x->push_back(end_vtx->get_x());
	    EndVtx_y->push_back(end_vtx->get_y());
	    EndVtx_z->push_back(end_vtx->get_z());
	    /* EndVtx_t->push_back(end_vtx->get_t()); */
	    float x = end_vtx->get_x();
	    float y = end_vtx->get_y();
	    float r = sqrt(x*x + y*y);
	    EndVtx_r->push_back(r);
	}
    }
}

void pythiaEMCalAna::FillAllClustersFromParent(PHG4Particle* par) {
    std::set<RawCluster*> cl_set = m_clusterEval->all_clusters_from(par);
    std::vector<float> allIDs;
    std::vector<float> allEfractions;
    if (cl_set.empty()) {
	std::cout << "Greg info: in FillAllClustersFromParent, cl_set is empty. Parent is\n";
	par->identify();
	m_truth_Parent_TotalNClusters.push_back(0);
	m_truth_Parent_AllClusterIDs.push_back(allIDs);
	m_truth_Parent_AllClusterEnergyFractions.push_back(allEfractions);
	return;
    }
    m_truth_Parent_TotalNClusters.push_back(cl_set.size());
    for (auto& cl : cl_set) {
	allIDs.push_back(cl->get_id());
	allEfractions.push_back(cl->get_ecore()/par->get_e());
    }
    m_truth_Parent_AllClusterIDs.push_back(allIDs);
    m_truth_Parent_AllClusterEnergyFractions.push_back(allEfractions);
    return;
}

void pythiaEMCalAna::FillParent(PHG4Particle* par) {
    FillTruthParticle("parent", par);
    FillAllClustersFromParent(par);
    /* std::vector<PHG4Particle*> daughters = GetAllDaughters(par); */
    /* m_truth_Parent_TotalNDaughters.push_back(daughters.size()); */
    /* std::vector<float> allPids; */
    /* std::vector<float> allEfractions; */
    /* for (auto daughter : daughters) { */
	/* allPids.push_back(daughter->get_pid()); */
	/* allEfractions.push_back(daughter->get_e()/par->get_e()); */
    /* } */
    /* m_truth_Parent_AllDaughterPids.push_back(allPids); */
    /* m_truth_Parent_AllDaughterEnergyFractions.push_back(allEfractions); */
}

HepMC::GenParticle* pythiaEMCalAna::GetHepMCParent(PHG4Particle* par) {
    HepMC::GenParticle* p = getGenParticle(par->get_barcode());
    HepMC::GenVertex* prod_vtx = p->production_vertex();
    HepMC::GenVertex::particles_in_const_iterator parent;
    // should only have 1 parent; otherwise print an error
    if (prod_vtx->particles_in_size() == 1) {
	parent = prod_vtx->particles_in_const_begin();
	assert((*parent));
	return (*parent);
    }
    else {
	// weird photon -- they should only have 1 parent
	std::cout << "\nGreg info: in GetHepMCParent, found a photon with " << prod_vtx->particles_in_size() << " parent(s). Photon:\n";
	p->print();
	for (parent = prod_vtx->particles_in_const_begin(); parent != prod_vtx->particles_in_const_end(); parent++) {
	    std::cout << "Parent:\n";
	    (*parent)->print();
	}
	std::cout << "\n";
	return nullptr;
    }
}

PHG4Particle* pythiaEMCalAna::getG4Particle(int barcode) {
    PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetPrimaryParticleRange();
    PHG4TruthInfoContainer::ConstIterator truthIter;
    /* std::cout << "\n\nGreg info: starting loop over Geant primary particles\n"; */
    for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
	PHG4Particle *p = truthIter->second;
	if ( p->get_barcode() == barcode ) {
	    // found the right particle
	    return p;
	}
    }
    // reached end of loop... if we still haven't found the right particle,
    // we have a problem
    std::cout << "\t\tGreg info: in getG4Particle(), could not find correct generated particle!\n";
    return nullptr;
}

void pythiaEMCalAna::FillTruth(HepMC::GenParticle* par) {
    if (!par) {
	std::cout << "In FillTruth: par is empty!\n";
	m_truth_Parent_Barcode.push_back(-9999);
	m_truth_Parent_Pid.push_back(-1);
	m_truth_Parent_M.push_back(-9999);
	m_truth_Parent_E.push_back(-9999);
	m_truth_Parent_Eta.push_back(-9999);
	m_truth_Parent_Phi.push_back(-9999);
	m_truth_Parent_Pt.push_back(-9999);
	m_truth_Parent_xF.push_back(-9999);
	m_truth_Parent_EndVtx_x.push_back(-9999);
	m_truth_Parent_EndVtx_y.push_back(-9999);
	m_truth_Parent_EndVtx_z.push_back(-9999);
	/* m_truth_Parent_EndVtx_t.push_back(-9999); */
	m_truth_Parent_EndVtx_r.push_back(-9999);
	m_truth_Parent_TotalNClusters.push_back(-9999);
	std::vector<float> allIDs;
	std::vector<float> allEfractions;
	m_truth_Parent_AllClusterIDs.push_back(allIDs);
	m_truth_Parent_AllClusterEnergyFractions.push_back(allEfractions);
	return;
    }
    else {
	if (! withinAcceptance(par)) {
	    /* std::cout << "In FillTruth: HepMC particle is outside acceptance. Details:\n"; */
	    /* par->print(); */
	    /* HepMC::FourVector mom = par->momentum(); */
	    /* std::cout << "(eta, E) = (" << mom.pseudoRapidity() << ", " << mom.e() << ")\n"; */
	    return;
	}
	primaryBarcodes.insert(par->barcode());
	HepMC::FourVector mom = par->momentum();
	TLorentzVector truth_momentum;
	truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e());

	HepMC::GenVertex* end_vtx = par->end_vertex();
	if (!end_vtx) {
	    std::cout << "\nGreg info: no end vertex found for Pythia-decayed particle:\n";
	    par->print();
	    std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt());
	    return;
	}
	HepMC::FourVector position = end_vtx->position();

	m_truth_Parent_Barcode.push_back(par->barcode());
	m_truth_Parent_Pid.push_back(par->pdg_id());
	m_truth_Parent_M.push_back(truth_momentum.M());
	m_truth_Parent_E.push_back(truth_momentum.E());
	m_truth_Parent_Eta.push_back(truth_momentum.PseudoRapidity());
	m_truth_Parent_Phi.push_back(truth_momentum.Phi());
	m_truth_Parent_Pt.push_back(truth_momentum.Pt());
	float xf = 2.0*truth_momentum.Pz()/200.0;
	m_truth_Parent_xF.push_back(xf);

	m_truth_Parent_EndVtx_x.push_back(position.x());
	m_truth_Parent_EndVtx_y.push_back(position.y());
	m_truth_Parent_EndVtx_z.push_back(position.z());
	/* EndVtx_t.push_back(end_vtx->get_t()); */
	float x = position.x();
	float y = position.y();
	float r = sqrt(x*x + y*y);
	m_truth_Parent_EndVtx_r.push_back(r);

	int NClusters = 0;
	std::vector<float> allIDs;
	std::vector<float> allEfractions;
	bool first = false;
	bool second = false;

	HepMC::GenVertex::particles_out_const_iterator out;
	/* std::cout << "Greg info: In FillParent(HepMC), end_vtx has " << end_vtx->particles_out_size() << " outgoing particles. Adding their info (parent is)\n"; */
	/* par->print(); */
	for (out = end_vtx->particles_out_const_begin(); out != end_vtx->particles_out_const_end(); out++) {
	    PHG4Particle* out_g4 = getG4Particle((*out)->barcode());
	    std::set<RawCluster*> cl_set = m_clusterEval->all_clusters_from(out_g4);
	    if (cl_set.empty()) {
		/* std::cout << "Greg info: in FillTruth, cl_set is empty. Daughter is\n"; */
		/* out_g4->identify(); */
	    }
	    else {
		for (auto& cl : cl_set) {
		    NClusters++;
		    allIDs.push_back(cl->get_id());
		    allEfractions.push_back(cl->get_ecore()/truth_momentum.E());
		}
	    }
	    if (!first) {
		first = true;
		/* std::cout << "Filling decay1\n"; */
		FillDecay("decay1", out_g4, nullptr);
	    }
	    else {
		if (!second) {
		    second = true;
		    FillDecay("decay2", out_g4, nullptr);
		}
	    }
	}  // end loop over outgoing particles
	m_truth_Parent_TotalNClusters.push_back(NClusters);
	m_truth_Parent_AllClusterIDs.push_back(allIDs);
	m_truth_Parent_AllClusterEnergyFractions.push_back(allEfractions);
    }
}

std::vector<PHG4Particle*> pythiaEMCalAna::GetAllDaughters(PHG4Particle* parent) {
    std::vector<PHG4Particle*> vec;
    int parent_id = parent->get_track_id();
    /* std::cout << "In GetAllDaughters: parent is\n"; */
    /* parent->identify(); */
    PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetSecondaryParticleRange();
    PHG4TruthInfoContainer::ConstIterator truthIter;
    for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
	PHG4Particle *decayPar = truthIter->second;
	int this_parent_id = decayPar->get_parent_id();
	if (this_parent_id == parent_id) {
	    vec.push_back(decayPar);
	}
    }
    return vec;
}

void pythiaEMCalAna::PrintParent(PHG4Particle* par) {
    std::vector<PHG4Particle*> daughters = GetAllDaughters(par);
    std::set<RawCluster*> cl_set = m_clusterEval->all_clusters_from(par);
    std::cout << "In PrintParent: parent is \n";
    par->identify();
    std::cout << Form("Parent has %ld daughter particles and %ld associated clusters.\n", daughters.size(), cl_set.size());
    return;
}

void pythiaEMCalAna::GetBestDaughters(PHG4Particle* parent, PHG4Particle* &decay1, PHG4Particle* &decay2) {
    /* std::cout << "\nIn GetBestDaughters: parent is\n"; */
    /* parent->identify(); */
    std::vector<PHG4Particle*> daughters = GetAllDaughters(parent);
    /* std::cout << "Parent has " << daughters.size() << " daughters.\n"; */
    if (daughters.size() == 0) {
	decay1 = nullptr;
	decay2 = nullptr;
	return;
    }
    if (daughters.size() == 1) {
	decay1 = daughters.at(0);
	/* std::cout << "In GetBestDaughters: only daughter is" << decay1 << ":\n"; */
	/* decay1->identify(); */
	decay2 = nullptr;
	return;
    }
    std::vector<float> Efractions;
    /* for (std::vector<PHG4Particle*>::iterator it = daughters.begin(); it != daughters.end(); it++) { */
    for (auto daughter : daughters) {
	Efractions.push_back(daughter->get_e()/parent->get_e());
    }
    float maxE = 0.0;
    float submaxE = 0.0;
    std::vector<float>::iterator max;
    max = std::max_element(Efractions.begin(), Efractions.end());
    maxE = *max;
    unsigned int max_index = std::distance(Efractions.begin(), max);
    decay1 = daughters.at(max_index);
    if (false) std::cout << "In GetBestDaughters: highest energy daughter is " << decay1 << " with energy fraction " << maxE << ":\n";
    /* decay1->identify(); */
    // set the max to 0 so we can use max_element to find the submax
    Efractions.at(max_index) = 0.0;
    max = std::max_element(Efractions.begin(), Efractions.end());
    submaxE = *max;
    unsigned int submax_index = std::distance(Efractions.begin(), max);
    decay2 = daughters.at(submax_index);
    if (false) std::cout << "In GetBestDaughters: second highest energy daughter is " << decay2 << " with energy fraction " << submaxE << ":\n";
    /* decay2->identify(); */
}

/* void pythiaEMCalAna::GetBestDaughters(PHG4Particle* parent, PHG4Particle* &decay1, PHG4Particle* &decay2) { */
/*     std::cout << "In GetBestDaughters: parent is\n"; */
/*     parent->identify(); */
/*     bool first = false; */
/*     bool second = false; */
/*     PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetSecondaryParticleRange(); */
/*     PHG4TruthInfoContainer::ConstIterator truthIter; */
/*     for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
/*     { */
/* 	PHG4Particle *decayPar = truthIter->second; */
/* 	int this_parent_id = decayPar->get_parent_id(); */
/* 	if (this_parent_id == parent_id) { */
/* 	    if (!first) { */
/* 		std::cout << "In GetDaughters: found first. decayPar=" << decayPar << "\n"; */
/* 		decay1 = decayPar; */
/* 		first = true; */
/* 		std::cout << "decay1=" << decay1 << "\n"; */
/* 		std::cout << "Found decay1 with primary id " << decay1->get_primary_id() << ": "; */
/* 		decay1->identify(); */
/* 	    } */
/* 	    else { */
/* 		if (!second) { */
/* 		    std::cout << "In GetDaughters: found second. decayPar=" << decayPar << "\n"; */
/* 		    decay2 = decayPar; */
/* 		    second = true; */
/* 		    std::cout << "decay2=" << decay2 << "\n"; */
/* 		    std::cout << "Found decay2 with primary id " << decay2->get_primary_id() << ": "; */
/* 		    decay2->identify(); */
/* 		    /1* break; *1/ */
/* 		} */
/* 		else { */
/* 		    std::cout << "Greg info: found a parent with more than 2 decay particles. Parent is \n"; */
/* 		    parent->identify(); */
/* 		    std::cout << "Decay1 is \n"; */
/* 		    decay1->identify(); */
/* 		    std::cout << "Decay2 is \n"; */
/* 		    decay2->identify(); */
/* 		    std::cout << "decayPar is \n"; */
/* 		    decayPar->identify(); */
/* 		    break; */
/* 		} */
/* 	    } */
/* 	} */
/*     } */
/*     if (!first || !second) { */
/* 	std::cout << "Greg info: got to end of secondary particles range without finding two decays!\n"; */
/* 	if (!first) std::cout << "\tFound 0 decays. Parent is\n"; */
/* 	else std::cout << "\tFound 1 decay. Parent is\n"; */
/* 	parent->identify(); */
/*     } */
/* } */

void pythiaEMCalAna::FillDecay(std::string which, PHG4Particle* decay, PHG4Particle* parent) {
    /* std::cout << "Entering FillDecay(" << which << ")\n"; */
    FillTruthParticle(which, decay);
    std::vector<float>* TotalNClusters = nullptr;
    std::vector<float>* BestClusterID = nullptr;
    std::vector<float>* BestClusterEfraction = nullptr;
    /* std::vector<std::vector<float>> AllClusterIDs; */
    /* std::vector<std::vector<float>> AllClusterEnergyFractions; */
    if (which == "decay1") {
	TotalNClusters = &m_truth_Decay1_TotalNClusters;
	BestClusterID = &m_truth_Decay1_BestClusterID;
	BestClusterEfraction = &m_truth_Decay1_BestClusterEfraction;
	/* AllClusterIDs = m_truth_Decay1_AllClusterIDs; */
	/* AllClusterEnergyFractions = m_truth_Decay1_AllClusterEnergyFractions; */
    }
    if (which == "decay2") {
	TotalNClusters = &m_truth_Decay2_TotalNClusters;
	BestClusterID = &m_truth_Decay2_BestClusterID;
	BestClusterEfraction = &m_truth_Decay2_BestClusterEfraction;
	/* AllClusterIDs = m_truth_Decay2_AllClusterIDs; */
	/* AllClusterEnergyFractions = m_truth_Decay2_AllClusterEnergyFractions; */
    }
    if (!decay) {
	TotalNClusters->push_back(0);
	BestClusterID->push_back(-9999);
	BestClusterEfraction->push_back(-9999);
	/* std::vector<float> allIDs; */
	/* std::vector<float> allEfractions; */
	/* AllClusterIDs->push_back(allIDs); */
	/* AllClusterEnergyFractions->push_back(allEfractions); */
	return;
    }

    std::set<RawCluster*> cl_set = m_clusterEval->all_clusters_from(decay);
    if (cl_set.empty()) {
	/* std::cout << "Greg info: in FillDecay, cl_set is empty. Decay is " << decay << ":\n"; */
	/* decay->identify(); */
	TotalNClusters->push_back(-9999);
    }
    else {
	TotalNClusters->push_back(cl_set.size());
    }
    RawCluster* best_cl = m_clusterEval->best_cluster_from(decay);
    if (!best_cl) {
	/* std::cout << "Greg info: in FillDecay, best_cluster_from failed\n"; */
	/* std::cout << "decay is "; */
	/* decay->identify(); */
	if (parent) best_cl = FindBestCluster(decay, parent);
	else {
	    /* std::cout << "No parent given, and best_cluster_from failed!\n"; */
	    BestClusterID->push_back(-9999);
	    BestClusterEfraction->push_back(-9999);
	    return;
	}
	/* std::cout << "Result of FindBestCluster is "; */
	/* CLHEP::Hep3Vector cl3vec = best_cl->get_position(); */
	/* std::cout << Form("id=%d, E=%f, eta=%f, phi=%f\n", best_cl->get_id(), best_cl->get_energy(), cl3vec.pseudoRapidity(), cl3vec.phi()); */

    }
    else {
	/* std::cout << "Greg info: in FillDecay, best_cluster_from succeeded\n"; */
    }
    BestClusterID->push_back(best_cl->get_id());
    BestClusterEfraction->push_back(best_cl->get_ecore()/decay->get_e());

    /* std::vector<float> allIDs; */
    /* std::vector<float> allEfractions; */
    /* if (cl_set.empty()) { */
	/* allIDs->push_back(best_cl->get_id()); */
	/* allEfractions->push_back(best_cl->get_ecore()/decay->get_e()); */
    /* } */
    /* else { */
	/* for (auto& cl : cl_set) { */
	    /* allIDs->push_back(cl->get_id()); */
	    /* allEfractions->push_back(cl->get_ecore()/decay->get_e()); */
	/* } */
    /* } */
    /* AllClusterIDs->push_back(allIDs); */
    /* AllClusterEnergyFractions->push_back(allEfractions); */
}

RawCluster* pythiaEMCalAna::FindBestCluster(PHG4Particle* decay, PHG4Particle* parent) {
    RawCluster* best_cl = nullptr;
    std::set<RawCluster*> cl_set = m_clusterEval->all_clusters_from(parent);
    if (cl_set.empty()) {
	std::cout << "Greg info: in FindBestCluster, cl_set is empty. Parent is\n";
	parent->identify();
	return best_cl;
    }
    /* std::cout << "In FindBestCluster: parent has " << cl_set.size() << " associated clusters.\n"; */
    TLorentzVector decay_vec;
    decay_vec.SetPxPyPzE(decay->get_px(), decay->get_py(), decay->get_pz(), decay->get_e());
    TLorentzVector cl_vec;
    float min_dR = 999.9;
    for (auto& cl : cl_set) {
	CLHEP::Hep3Vector cl3vec = cl->get_position();
	cl_vec.SetPtEtaPhiM(cl3vec.perp(), cl3vec.pseudoRapidity(), cl3vec.phi(), 0);
	float dR = decay_vec.DeltaR(cl_vec);
	min_dR = std::min(min_dR, dR);
	if (dR == min_dR) best_cl = cl;
	/* std::cout << "In FindBestCluster: min_dR=" << min_dR << " from cluster id " << best_cl->get_id() << "\n"; */
    }
    /* std::cout << "Done in FindBestCluster. min_dR=" << min_dR << "\n"; */
    return best_cl;
}

HepMC::GenParticle* pythiaEMCalAna::getGenParticle(int barcode) {
    /* std::cout << "Greg info: in getGenParticle, looking for barcode " << barcode << "\n"; */
    /* std::cout << "m_theEvent = " << m_theEvent << "\n"; */ 
    for(HepMC::GenEvent::particle_const_iterator p=m_theEvent->particles_begin(); p!=m_theEvent->particles_end(); ++p)
    {
	/* std::cout << "In loop, (*p)=" << (*p) << "\n"; */
	assert(*p);
	/* std::cout << "\tpythia barcode " << (*p)->barcode() << "\n"; */
	/* if ((*p)->barcode() == 33) { */
	/*     std::cout << "barcode 33 details: \t"; */
	/*     (*p)->print(); */
	/* } */
	if ( (*p)->barcode() == barcode ) {
	    // found the right particle
	    /* std::cout << "\tpythia barcode " << (*p)->barcode() << ", returning (*p)\n"; */
	    return (*p);
	}
    }
    // reached end of loop... if we still haven't found the right particle,
    // we have a problem
    std::cout << "\t\tGreg info: in getGenParticle(), could not find correct generated particle!\n";
    return nullptr;
}

bool pythiaEMCalAna::withinAcceptance(PHG4Particle* part) {
    float max_eta = 1.1;
    float min_E = 1.0;
    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
    float eta = truth_momentum.PseudoRapidity();
    if (abs(eta) > max_eta) return false;
    if (truth_momentum.E() < min_E) return false;
    else return true;
}

bool pythiaEMCalAna::withinAcceptance(HepMC::GenParticle* part) {
    float max_eta = 1.1;
    float min_E = 1.0;
    HepMC::FourVector mom = part->momentum();
    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e());
    float eta = truth_momentum.PseudoRapidity();
    if (abs(eta) > max_eta) return false;
    if (truth_momentum.E() < min_E) return false;
    else return true;
}

PHG4VtxPoint* pythiaEMCalAna::getG4EndVtx(int id) {
    /* std::cout << "Entering getG4EndVtx\n"; */
    PHG4VtxPoint* end_vtx = nullptr;
    PHG4TruthInfoContainer::Range truthRange = m_truthInfo->GetSecondaryParticleRange();
    PHG4TruthInfoContainer::ConstIterator truthIter;
    for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
	PHG4Particle *decayPar = truthIter->second;
	/* std::cout << "In getG4EndVtx: decayPar = " << decayPar << "\n"; */
	int parent_id = decayPar->get_parent_id();
	/* std::cout << "Parent id is " << parent_id << "\n"; */
	if (parent_id == id) {
	    end_vtx = m_truthInfo->GetVtx(decayPar->get_vtx_id());
	    /* std::cout << "In getG4EndVtx: assigning end_vtx\n"; */
	    break;
	}
    }
    if (!end_vtx) {
	// couldn't find any daughter particles in secondary list
	// check primary list instead??
	/* std::cout << "Could not find any daughters in secondary particle list\n"; */
	truthRange = m_truthInfo->GetPrimaryParticleRange();
	for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
	{
	    PHG4Particle *decayPar = truthIter->second;
	    int parent_id = decayPar->get_parent_id();
	    if (parent_id == id) {
		std::cout << "Greg info: in getG4EndVtx, found daughter in *primary* particle range!\n";
		end_vtx = m_truthInfo->GetVtx(decayPar->get_vtx_id());
		break;
	    }
	}
    }
    /* std::cout << "In getG4EndVtx: returning end_vtx = " << end_vtx << "\n"; */
    return end_vtx;
}

// Old stuff

/* void pythiaEMCalAna::addDirectPhoton(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) { */
/*     /1* std::cout << "Found a direct photon!\n"; *1/ */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     if (! withinAcceptance(part)) return; */
/*     /1* std::cout << "\tIs within acceptance!\n"; *1/ */
/*     /1* part->identify(); *1/ */
/*     int this_id = part->get_track_id(); */
    
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
/*     if (!end_vtx) { */
/* 	std::cout << "\nGreg info: no end vertex found for direct photon:\n"; */
/* 	part->identify(); */
/* 	std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); */
/* 	return; */
/*     } */

/*     int embedID = truthInfo->isEmbeded(part->get_track_id()); */
/*     m_truthBarcode.push_back(std::pair<int,int>(embedID, part->get_barcode())); */
/*     m_truthParentBarcode.push_back(std::pair<int,int>(embedID, 0)); */
/*     m_truthIsPrimary.push_back(1); */
/*     m_truthPid.push_back(part->get_pid()); */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     // part->get_vtx_id() gives the *production* vertex, not the end vertex */
/*     /1* PHG4VtxPoint* end_vtx = truthInfo->GetVtx(part->get_vtx_id()); // DOES NOT WORK *1/ */
/*     // get the end vertex from one of the daughter particles */
/*     /1* int this_id = part->get_track_id(); *1/ */
/*     /1* std::cout << "this_id = " << this_id << "; getting end vertex\n"; *1/ */
/*     /1* PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); *1/ */
/*     /1* std::cout << "end_vtx = " << end_vtx << "\n"; *1/ */
/*     assert(end_vtx); */
/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */

/*     /1* primaryBarcodes.push_back(part->get_barcode()); *1/ */
/* } */

/* void pythiaEMCalAna::addDecayPhoton(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo, HepMC::GenEvent* theEvent) { */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     if (! withinAcceptance(part)) return; */
/*     int this_id = part->get_track_id(); */
    
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
/*     if (!end_vtx) { */
/* 	/1* std::cout << "\nGreg info: no end vertex found for decay photon:\n"; *1/ */
/* 	/1* part->identify(); *1/ */
/* 	/1* std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); *1/ */
/* 	return; */
/*     } */

/*     int parent_barcode; */
/*     // If Geant handled the decay, we can get the parent directly */
/*     PHG4Particle* geant_parent = truthInfo->GetParticle(part->get_parent_id()); */
/*     if (geant_parent) { */
/* 	parent_barcode = geant_parent->get_barcode(); */
/*     } */
/*     else { */
/* 	// pythia handled the decay, so get the parent from there */
/* 	HepMC::GenParticle* pho = getGenParticle(part->get_barcode(), theEvent); */
/* 	assert(pho); */
/* 	HepMC::GenVertex* prod_vtx = pho->production_vertex(); */
/* 	if (prod_vtx->particles_in_size() == 1) { */
/* 	    HepMC::GenVertex::particles_in_const_iterator pythia_parent = prod_vtx->particles_in_const_begin(); */
/* 	    assert((*pythia_parent)); */
/* 	    parent_barcode = (*pythia_parent)->barcode(); */
/* 	} */
/* 	else { */
/* 	    std::cout << "Greg info: pythia-decayed photon with " << prod_vtx->particles_in_size() << " parents. Skipping...\n"; */
/* 	    return; */
/* 	} */
/*     } // done getting parent barcode */

/*     /1* std::cout << "Adding decay photon with barcode " << part->get_barcode() << "; geant_parent is " << geant_parent << "\n"; *1/ */
/*     int embedID = truthInfo->isEmbeded(part->get_track_id()); */
/*     m_truthBarcode.push_back(std::pair<int,int>(embedID, part->get_barcode())); */
/*     m_truthParentBarcode.push_back(std::pair<int,int>(embedID, parent_barcode)); */
/*     m_truthIsPrimary.push_back(0); */
/*     m_truthPid.push_back(part->get_pid()); */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     // part->get_vtx_id() gives the *production* vertex, not the end vertex */
/*     /1* PHG4VtxPoint* end_vtx = truthInfo->GetVtx(part->get_vtx_id()); // DOES NOT WORK *1/ */
/*     // get the end vertex from one of the daughter particles */
/*     /1* int this_id = part->get_track_id(); *1/ */
/*     /1* std::cout << "this_id = " << this_id << "; getting end vertex\n"; *1/ */
/*     /1* PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); *1/ */
/*     /1* std::cout << "end_vtx = " << end_vtx << "\n"; *1/ */
/*     assert(end_vtx); */
/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */

/*     /1* secondaryBarcodes.push_back(part->get_barcode()); *1/ */
/* } */

/* void pythiaEMCalAna::addPrimaryHadronFromPythia(HepMC::GenParticle* part, int embedID) { */
/*     // case where PYTHIA handles the decay and Geant never knows about the primary */
/*     HepMC::FourVector mom = part->momentum(); */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e()); */
/*     if (! withinAcceptance(part)) return; */
/*     HepMC::GenVertex* end_vtx = part->end_vertex(); */
/*     if (!end_vtx) { */
/* 	std::cout << "\nGreg info: no end vertex found for Pythia-decayed primary:\n"; */
/* 	part->print(); */
/* 	std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); */
/* 	return; */
/*     } */
    
/*     m_truthBarcode.push_back(std::pair<int,int>(embedID, part->barcode())); */
/*     m_truthParentBarcode.push_back(std::pair<int,int>(embedID, 0)); */
/*     m_truthIsPrimary.push_back(1); */
/*     m_truthPid.push_back(part->pdg_id()); */
/*     if (part->pdg_id() == 22) std::cout << "Greg info: primary in addPrimaryHadronFromPythia is a photon!\n"; */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     assert(end_vtx); */
/*     HepMC::FourVector position = end_vtx->position(); */

/*     m_truthEndVtx_x.push_back(position.x()); */
/*     m_truthEndVtx_y.push_back(position.y()); */
/*     m_truthEndVtx_z.push_back(position.z()); */
/*     m_truthEndVtx_t.push_back(position.t()); */
/*     float x = position.x(); */
/*     float y = position.y(); */
/*     float r = sqrt(x*x + y*y); */
/*     /1* std::cout << Form("pythiaEMCalAna::addPrimaryFromPythia(): Vertex r=%f, perp=%f\n", r, position.perp()); *1/ */
/*     m_truthEndVtx_r.push_back(r); */

/*     /1* primaryBarcodes.push_back(part->barcode()); *1/ */
/* } */   

/* void pythiaEMCalAna::addPrimaryHadronFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) { */
/*     // case where Geant handles the decay */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     if (! withinAcceptance(part)) return; */
/*     int this_id = part->get_track_id(); */
    
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
/*     if (!end_vtx) { */
/* 	std::cout << "\nGreg info: no end vertex found for Geant-decayed primary:\n"; */
/* 	part->identify(); */
/* 	std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); */
/* 	return; */
/*     } */

/*     int embedID = truthInfo->isEmbeded(part->get_track_id()); */
/*     m_truthBarcode.push_back(std::pair<int,int>(embedID, part->get_barcode())); */
/*     m_truthParentBarcode.push_back(std::pair<int,int>(embedID, 0)); */
/*     m_truthIsPrimary.push_back(1); */
/*     m_truthPid.push_back(part->get_pid()); */
/*     if (part->get_pid() == 22) std::cout << "Greg info: primary in addPrimaryHadronFromGeant is a photon!\n"; */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     assert(end_vtx); */
/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */

/*     /1* primaryBarcodes.push_back(part->get_barcode()); *1/ */
/* } */

/* void pythiaEMCalAna::addSecondaryFromPythia(HepMC::GenParticle* part) { */
/*     /1* std::cout << "Entering pythiaEMCalAna::addSecondaryFromPythia\npart=" << part << "\n"; *1/ */
/*     HepMC::FourVector mom = part->momentum(); */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e()); */
/*     float eta = truth_momentum.PseudoRapidity(); */
/*     if (abs(eta) > 1.1) return; */

/*     HepMC::GenVertex* prod_vtx = (*p)->production_vertex(); */
/*     HepMC::GenVertex::particles_in_const_iterator parent; */
/*     if (prod_vtx->particles_in_size() == 1) { */
/* 	parent = prod_vtx->particles_in_const_begin(); */
/*     } */
/*     else { */
/* 	std::cout << "Greg info: found a weird decay photon in pythia. Has " << prod_vtx->particles_in_size() << " parents. Skipping this photon!\n"; */
/* 	return; */
/*     } */

/*     m_truthBarcode.push_back(part->barcode()); */
/*     m_truthParentBarcode.push_back((*parent)->barcode()); */
/*     m_truthIsPrimary.push_back(0); */
/*     m_truthPid.push_back(part->pdg_id()); */

/*     /1* std::cout << "Getting momentum\n"; *1/ */
/*     /1* HepMC::FourVector mom = part->momentum(); *1/ */
/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */

/*     /1* std::cout << "Getting end vertex\n"; *1/ */
/*     HepMC::GenVertex* end_vtx = part->end_vertex(); */
/*     assert(end_vtx); */
/*     /1* std::cout << "Found end vertex in pythia\n"; *1/ */
/*     HepMC::FourVector position = end_vtx->position(); */

/*     m_truthEndVtx_x.push_back(position.x()); */
/*     m_truthEndVtx_y.push_back(position.y()); */
/*     m_truthEndVtx_z.push_back(position.z()); */
/*     m_truthEndVtx_t.push_back(position.t()); */
/*     float x = position.x(); */
/*     float y = position.y(); */
/*     float r = sqrt(x*x + y*y); */
/*     /1* std::cout << Form("pythiaEMCalAna::addPrimaryFromPythia(): Vertex r=%f, perp=%f\n", r, position.perp()); *1/ */
/*     m_truthEndVtx_r.push_back(r); */

/*     primaryBarcodes.push_back(part->barcode()); */
/* } */

/* void pythiaEMCalAna::addPrimaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) { */
    /* /1* std::cout << "Entering pythiaEMCalAna::addPrimaryFromGeant\npart=" << part << "\n"; *1/ */
    /* /1* part->identify(); *1/ */
    /* /1* std::cout << "Barcode is " << part->get_barcode() << "\n"; *1/ */
    /* /1* int vtx_id = part->get_vtx_id(); *1/ */
    /* /1* std::cout << "vertex id is " << vtx_id << "\n"; *1/ */
    /* /1* PHG4VtxPoint* vtxpt = truthInfo->GetVtx(vtx_id); *1/ */
    /* /1* vtxpt->identify(); *1/ */
    /* TLorentzVector truth_momentum; */
    /* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
    /* float eta = truth_momentum.PseudoRapidity(); */
    /* if (abs(eta) > 1.1) return; */
    /* int this_id = part->get_track_id(); */
    /* PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
    /* if (!end_vtx) { */
	/* /1* std::cout << "\nGreg info: no end vertex found for primary particle:\n"; *1/ */
	/* /1* part->identify(); *1/ */
	/* /1* std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt()); *1/ */
	/* return; */
    /* } */

    /* m_truthBarcode.push_back(part->get_barcode()); */
    /* m_truthParentBarcode.push_back(0); */
    /* m_truthIsPrimary.push_back(1); */
    /* m_truthPid.push_back(part->get_pid()); */

    /* /1* TLorentzVector truth_momentum; *1/ */
    /* /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
    /* m_truthE.push_back(truth_momentum.E()); */
    /* m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
    /* m_truthPhi.push_back(truth_momentum.Phi()); */
    /* m_truthPt.push_back(truth_momentum.Pt()); */
    /* m_truthMass.push_back(truth_momentum.M()); */

    /* // part->get_vtx_id() gives the *production* vertex, not the end vertex */
    /* /1* PHG4VtxPoint* end_vtx = truthInfo->GetVtx(part->get_vtx_id()); // DOES NOT WORK *1/ */
    /* // get the end vertex from one of the daughter particles */
    /* /1* int this_id = part->get_track_id(); *1/ */
    /* /1* std::cout << "this_id = " << this_id << "; getting end vertex\n"; *1/ */
    /* /1* PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); *1/ */
    /* /1* std::cout << "end_vtx = " << end_vtx << "\n"; *1/ */
    /* assert(end_vtx); */
    /* m_truthEndVtx_x.push_back(end_vtx->get_x()); */
    /* m_truthEndVtx_y.push_back(end_vtx->get_y()); */
    /* m_truthEndVtx_z.push_back(end_vtx->get_z()); */
    /* m_truthEndVtx_t.push_back(end_vtx->get_t()); */
    /* float x = end_vtx->get_x(); */
    /* float y = end_vtx->get_y(); */
    /* float r = sqrt(x*x + y*y); */
    /* m_truthEndVtx_r.push_back(r); */

    /* primaryBarcodes.push_back(part->get_barcode()); */
/* } */

/* void pythiaEMCalAna::addSecondaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) { */
/*     /1* std::cout << "Entering pythiaEMCalAna::addSecondary(), geant particle is " << part << "\n"; *1/ */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     float eta = truth_momentum.PseudoRapidity(); */
/*     if (abs(eta) > 1.1) return; */
/*     /1* std::cout << "Done checking eta\n"; *1/ */
/*     int this_id = part->get_track_id(); */
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
/*     if (!end_vtx) { */
/* 	/1* std::cout << "\n\nGreg info: no end vertex found for secondary particle:\n"; *1/ */
/* 	/1* part->identify(); *1/ */
/* 	/1* std::cout << "Particle eta=" << eta << "\n"; *1/ */
/* 	return; */
/*     } */
/*     /1* std::cout << "Done checking end_vtx\n"; *1/ */

/*     m_truthBarcode.push_back(part->get_barcode()); */
/*     PHG4Particle* parent = truthInfo->GetParticle(part->get_parent_id()); */
/*     m_truthParentBarcode.push_back(parent->get_barcode()); */
/*     m_truthIsPrimary.push_back(0); */
/*     m_truthPid.push_back(part->get_pid()); */
/*     /1* std::cout << "Done adding pid to vector\n"; *1/ */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */
/*     /1* std::cout << "Done adding kinematics to vector\n"; *1/ */

/*     /1* std::cout << "Asserting end_vtx= " << end_vtx << "\n"; *1/ */
/*     assert(end_vtx); */
/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */
/*     /1* std::cout << "Done adding vertex to vector\n"; *1/ */

/*     secondaryBarcodes.push_back(part->get_barcode()); */
/* } */

/* void pythiaEMCalAna::addSecondaryWithoutParent(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo, HepMC::GenParticle* genParent) { */
/*     /1* std::cout << "Entering pythiaEMCalAna::addSecondaryWithoutParent(), geant particle is " << part << ", pythia particle is " << genParent << "\n"; *1/ */
/*     TLorentzVector truth_momentum; */
/*     truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); */
/*     float eta = truth_momentum.PseudoRapidity(); */
/*     if (abs(eta) > 1.1) return; */
/*     /1* std::cout << "Done checking eta\n"; *1/ */
/*     int this_id = part->get_track_id(); */
/*     PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo); */
/*     if (!end_vtx) { */
/* 	/1* std::cout << "\n\nGreg info: no end vertex found for secondary particle:\n"; *1/ */
/* 	/1* part->identify(); *1/ */
/* 	/1* std::cout << "Particle eta=" << eta << "\n"; *1/ */
/* 	return; */
/*     } */
/*     /1* std::cout << "Done checking end_vtx\n"; *1/ */

/*     m_truthBarcode.push_back(part->get_barcode()); */
/*     m_truthParentBarcode.push_back(genParent->barcode()); */
/*     m_truthIsPrimary.push_back(0); */
/*     m_truthPid.push_back(part->get_pid()); */
/*     /1* std::cout << "Done adding pid to vector\n"; *1/ */

/*     /1* TLorentzVector truth_momentum; *1/ */
/*     /1* truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e()); *1/ */
/*     m_truthE.push_back(truth_momentum.E()); */
/*     m_truthEta.push_back(truth_momentum.PseudoRapidity()); */
/*     m_truthPhi.push_back(truth_momentum.Phi()); */
/*     m_truthPt.push_back(truth_momentum.Pt()); */
/*     m_truthMass.push_back(truth_momentum.M()); */
/*     /1* std::cout << "Done adding kinematics to vector\n"; *1/ */

/*     /1* std::cout << "Asserting end_vtx= " << end_vtx << "\n"; *1/ */
/*     assert(end_vtx); */
/*     m_truthEndVtx_x.push_back(end_vtx->get_x()); */
/*     m_truthEndVtx_y.push_back(end_vtx->get_y()); */
/*     m_truthEndVtx_z.push_back(end_vtx->get_z()); */
/*     m_truthEndVtx_t.push_back(end_vtx->get_t()); */
/*     float x = end_vtx->get_x(); */
/*     float y = end_vtx->get_y(); */
/*     float r = sqrt(x*x + y*y); */
/*     m_truthEndVtx_r.push_back(r); */
/*     /1* std::cout << "Done adding vertex to vector\n"; *1/ */

/*     secondaryBarcodes.push_back(part->get_barcode()); */
/* } */


/* bool pythiaEMCalAna::vector_contains(std::pair<int,int> val, std::vector<std::pair<int,int>> vec) { */
/*     unsigned int s = vec.size(); */
/*     if ( s == 0 ) return false; */
/*     unsigned int i; */
/*     for (i=0; i<s; i++) { */
/* 	if (vec.at(i) == val) break; */
/*     } */
/*     if ( i == s ) return false; */
/*     else return true; */
/* } */

