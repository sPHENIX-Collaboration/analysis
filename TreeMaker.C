#include "TreeMaker.h"

#include <phool/getClass.h>
#include <fun4all/Fun4AllServer.h>

#include <phool/PHCompositeNode.h>

#include "TLorentzVector.h"
#include <iostream>

#include <calotrigger/CaloTriggerInfo.h>

#include <g4cemc/RawClusterContainer.h>
#include <g4cemc/RawCluster.h>

#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTower.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeomContainer_Cylinderv1.h>
#include <g4cemc/RawTowerGeomContainer.h>

#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>

#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>

#include <jetbackground/TowerBackground.h>

TreeMaker::TreeMaker(const std::string &name) : SubsysReco("TRIGGERTEST")
{

  _foutname = name;

}

int TreeMaker::Init(PHCompositeNode *topNode)
{

  _f = new TFile( _foutname.c_str(), "RECREATE");

  _tree = new TTree("ttree","sequoia");

  /*
  _tree->Branch("tower_total_0",&_b_tower_total_0, "tower_total_0/F");
  _tree->Branch("tower_total_1",&_b_tower_total_1, "tower_total_1/F");
  _tree->Branch("tower_total_2",&_b_tower_total_2, "tower_total_2/F");

  _tree->Branch("tower_n",&_b_tower_n, "tower_n/I");
  _tree->Branch("tower_layer",&_b_tower_layer, "tower_layer[tower_n]/I");
  _tree->Branch("tower_E",&_b_tower_E, "tower_E[tower_n]/F");
  _tree->Branch("tower_eta",&_b_tower_eta, "tower_eta[tower_n]/F");
  _tree->Branch("tower_phi",&_b_tower_phi, "tower_phi[tower_n]/F");
  */

  _tree->Branch("bkg_n",&_b_bkg_n, "bkg_n/I");
  _tree->Branch("bkg_layer",&_b_bkg_layer, "bkg_layer[bkg_n]/I");
  _tree->Branch("bkg_eta",&_b_bkg_eta, "bkg_eta[bkg_n]/I");
  _tree->Branch("bkg_E",&_b_bkg_E, "bkg_E[bkg_n]/F");

  _tree->Branch("bkg2_n",     &_b_bkg2_n,     "bkg2_n/I");
  _tree->Branch("bkg2_layer", &_b_bkg2_layer, "bkg2_layer[bkg2_n]/I");
  _tree->Branch("bkg2_eta",   &_b_bkg2_eta,   "bkg2_eta[bkg2_n]/I");
  _tree->Branch("bkg2_E",     &_b_bkg2_E,     "bkg2_E[bkg2_n]/F");

  /*
  _tree->Branch("trigEM2_E", &_b_trigEM2_E, "trigEM2_E/F");
  _tree->Branch("trigEM2_phi", &_b_trigEM2_phi, "trigEM2_phi/F");
  _tree->Branch("trigEM2_eta", &_b_trigEM2_eta, "trigEM2_eta/F");

  _tree->Branch("trigEM4_E", &_b_trigEM4_E, "trigEM4_E/F");
  _tree->Branch("trigEM4_phi", &_b_trigEM4_phi, "trigEM4_phi/F");
  _tree->Branch("trigEM4_eta", &_b_trigEM4_eta, "trigEM4_eta/F");

  _tree->Branch("trigFull2_E", &_b_trigFull2_E, "trigFull2_E/F");
  _tree->Branch("trigFull2_eta", &_b_trigFull2_eta, "trigFull2_eta/F");
  _tree->Branch("trigFull2_phi", &_b_trigFull2_phi, "trigFull2_phi/F");

  _tree->Branch("trigFull4_E", &_b_trigFull4_E, "trigFull4_E/F");
  _tree->Branch("trigFull4_eta", &_b_trigFull4_eta, "trigFull4_eta/F");
  _tree->Branch("trigFull4_phi", &_b_trigFull4_phi, "trigFull4_phi/F");

  _tree->Branch("trigFull6_E", &_b_trigFull6_E, "trigFull6_E/F");
  _tree->Branch("trigFull6_eta", &_b_trigFull6_eta, "trigFull6_eta/F");
  _tree->Branch("trigFull6_phi", &_b_trigFull6_phi, "trigFull6_phi/F");

  _tree->Branch("trigFull8_E", &_b_trigFull8_E, "trigFull8_E/F");
  _tree->Branch("trigFull8_eta", &_b_trigFull8_eta, "trigFull8_eta/F");
  _tree->Branch("trigFull8_phi", &_b_trigFull8_phi, "trigFull8_phi/F");

  _tree->Branch("trigFull10_E", &_b_trigFull10_E, "trigFull10_E/F");
  _tree->Branch("trigFull10_eta", &_b_trigFull10_eta, "trigFull10_eta/F");
  _tree->Branch("trigFull10_phi", &_b_trigFull10_phi, "trigFull10_phi/F");

  _tree->Branch("cluster_n", &_b_cluster_n,"cluster_n/I");
  _tree->Branch("cluster_pt", _b_cluster_pt,"cluster_pt[cluster_n]/F");
  _tree->Branch("cluster_eta",_b_cluster_eta,"cluster_eta[cluster_n]/F");
  _tree->Branch("cluster_phi",_b_cluster_phi,"cluster_phi[cluster_n]/F");
  _tree->Branch("cluster_layer",_b_cluster_layer,"cluster_layer[cluster_n]/I");
  */


  _tree->Branch("particle_n", &_b_particle_n,"particle_n/I");
  _tree->Branch("particle_pt", _b_particle_pt,"particle_pt[particle_n]/F");
  _tree->Branch("particle_eta", _b_particle_eta,"particle_eta[particle_n]/F");
  _tree->Branch("particle_phi", _b_particle_phi,"particle_phi[particle_n]/F");
  _tree->Branch("particle_pid", _b_particle_pid,"particle_pid[particle_n]/I");

  _tree->Branch("jet2_n", &_b_jet2_n,"jet2_n/I");
  _tree->Branch("jet2_pt", _b_jet2_pt,"jet2_pt[jet2_n]/F");
  _tree->Branch("jet2_eta",_b_jet2_eta,"jet2_eta[jet2_n]/F");
  _tree->Branch("jet2_phi",_b_jet2_phi,"jet2_phi[jet2_n]/F");

  _tree->Branch("jet3_n", &_b_jet3_n,"jet3_n/I");
  _tree->Branch("jet3_pt", _b_jet3_pt,"jet3_pt[jet3_n]/F");
  _tree->Branch("jet3_eta",_b_jet3_eta,"jet3_eta[jet3_n]/F");
  _tree->Branch("jet3_phi",_b_jet3_phi,"jet3_phi[jet3_n]/F");

  _tree->Branch("jet4_n", &_b_jet4_n,"jet4_n/I");
  _tree->Branch("jet4_pt", _b_jet4_pt,"jet4_pt[jet4_n]/F");
  _tree->Branch("jet4_eta",_b_jet4_eta,"jet4_eta[jet4_n]/F");
  _tree->Branch("jet4_phi",_b_jet4_phi,"jet4_phi[jet4_n]/F");

  _tree->Branch("jet5_n", &_b_jet5_n,"jet5_n/I");
  _tree->Branch("jet5_pt", _b_jet5_pt,"jet5_pt[jet5_n]/F");
  _tree->Branch("jet5_eta",_b_jet5_eta,"jet5_eta[jet5_n]/F");
  _tree->Branch("jet5_phi",_b_jet5_phi,"jet5_phi[jet5_n]/F");

  _tree->Branch("jet2sub_n", &_b_jet2sub_n,"jet2sub_n/I");
  _tree->Branch("jet2sub_pt", _b_jet2sub_pt,"jet2sub_pt[jet2sub_n]/F");
  _tree->Branch("jet2sub_eta",_b_jet2sub_eta,"jet2sub_eta[jet2sub_n]/F");
  _tree->Branch("jet2sub_phi",_b_jet2sub_phi,"jet2sub_phi[jet2sub_n]/F");

  _tree->Branch("jet2seed_n", &_b_jet2seed_n,"jet2seed_n/I");
  _tree->Branch("jet2seed_pt", _b_jet2seed_pt,"jet2seed_pt[jet2seed_n]/F");
  _tree->Branch("jet2seed_eta",_b_jet2seed_eta,"jet2seed_eta[jet2seed_n]/F");
  _tree->Branch("jet2seed_phi",_b_jet2seed_phi,"jet2seed_phi[jet2seed_n]/F");

  _tree->Branch("jet3sub_n", &_b_jet3sub_n,"jet3sub_n/I");
  _tree->Branch("jet3sub_pt", _b_jet3sub_pt,"jet3sub_pt[jet3sub_n]/F");
  _tree->Branch("jet3sub_eta",_b_jet3sub_eta,"jet3sub_eta[jet3sub_n]/F");
  _tree->Branch("jet3sub_phi",_b_jet3sub_phi,"jet3sub_phi[jet3sub_n]/F");

  _tree->Branch("jet4sub_n", &_b_jet4sub_n,"jet4sub_n/I");
  _tree->Branch("jet4sub_pt", _b_jet4sub_pt,"jet4sub_pt[jet4sub_n]/F");
  _tree->Branch("jet4sub_eta",_b_jet4sub_eta,"jet4sub_eta[jet4sub_n]/F");
  _tree->Branch("jet4sub_phi",_b_jet4sub_phi,"jet4sub_phi[jet4sub_n]/F");

  _tree->Branch("jet5sub_n", &_b_jet5sub_n,"jet5sub_n/I");
  _tree->Branch("jet5sub_pt", _b_jet5sub_pt,"jet5sub_pt[jet5sub_n]/F");
  _tree->Branch("jet5sub_eta",_b_jet5sub_eta,"jet5sub_eta[jet5sub_n]/F");
  _tree->Branch("jet5sub_phi",_b_jet5sub_phi,"jet5sub_phi[jet5sub_n]/F");

  _tree->Branch("truthjet2_n", &_b_truthjet2_n,"truthjet2_n/I");
  _tree->Branch("truthjet2_pt", _b_truthjet2_pt,"truthjet2_pt[truthjet2_n]/F");
  _tree->Branch("truthjet2_eta",_b_truthjet2_eta,"truthjet2_eta[truthjet2_n]/F");
  _tree->Branch("truthjet2_phi",_b_truthjet2_phi,"truthjet2_phi[truthjet2_n]/F");

  _tree->Branch("truthjet3_n", &_b_truthjet3_n,"truthjet3_n/I");
  _tree->Branch("truthjet3_pt", _b_truthjet3_pt,"truthjet3_pt[truthjet3_n]/F");
  _tree->Branch("truthjet3_eta",_b_truthjet3_eta,"truthjet3_eta[truthjet3_n]/F");
  _tree->Branch("truthjet3_phi",_b_truthjet3_phi,"truthjet3_phi[truthjet3_n]/F");

  _tree->Branch("truthjet4_n", &_b_truthjet4_n,"truthjet4_n/I");
  _tree->Branch("truthjet4_pt", _b_truthjet4_pt,"truthjet4_pt[truthjet4_n]/F");
  _tree->Branch("truthjet4_eta",_b_truthjet4_eta,"truthjet4_eta[truthjet4_n]/F");
  _tree->Branch("truthjet4_phi",_b_truthjet4_phi,"truthjet4_phi[truthjet4_n]/F");

  _tree->Branch("truthjet5_n", &_b_truthjet5_n,"truthjet5_n/I");
  _tree->Branch("truthjet5_pt", _b_truthjet5_pt,"truthjet5_pt[truthjet5_n]/F");
  _tree->Branch("truthjet5_eta",_b_truthjet5_eta,"truthjet5_eta[truthjet5_n]/F");
  _tree->Branch("truthjet5_phi",_b_truthjet5_phi,"truthjet5_phi[truthjet5_n]/F");

  return 0;

}

int TreeMaker::process_event(PHCompositeNode *topNode)
{

  std::cout << "DVP : at process_event, tree size is: " << _tree->GetEntries() << std::endl;

  //RawTowerContainer *towersEM1 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_CEMC");
  //RawTowerContainer *towersEM2 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_CEMC");
  //RawTowerContainer *towersEM3old = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
  //RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER");
  //RawTowerContainer *towersEM4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC_RETOWER_SUB1");
  //std::cout << "TreeMaker::process_event: " << towersEM1->size() << " TOWER_SIM_CEMC towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersEM2->size() << " TOWER_RAW_CEMC towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersEM3old->size() << " TOWER_CALIB_CEMC towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersEM3->size() << " TOWER_CALIB_CEMC_RETOWER towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersEM4->size() << " TOWER_CALIB_CEMC_RETOWER_SUB1 towers" << std::endl;

  //RawTowerContainer *towersIH1 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_HCALIN");
  //RawTowerContainer *towersIH2 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_HCALIN");
  //RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
  //RawTowerContainer *towersIH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN_SUB1");
  //std::cout << "TreeMaker::process_event: " << towersIH1->size() << " TOWER_SIM_HCALIN towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersIH2->size() << " TOWER_RAW_HCALIN towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersIH3->size() << " TOWER_CALIB_HCALIN towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersIH4->size() << " TOWER_CALIB_HCALIN_SUB1 towers" << std::endl;

  //RawTowerContainer *towersOH1 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_SIM_HCALOUT");
  //RawTowerContainer *towersOH2 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_RAW_HCALOUT");
  //RawTowerContainer *towersOH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT");
  //RawTowerContainer *towersOH4 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALOUT_SUB1");
  //std::cout << "TreeMaker::process_event: " << towersOH1->size() << " TOWER_SIM_HCALOUT towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersOH2->size() << " TOWER_RAW_HCALOUT towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersOH3->size() << " TOWER_CALIB_HCALOUT towers" << std::endl;
  //std::cout << "TreeMaker::process_event: " << towersOH4->size() << " TOWER_CALIB_HCALOUT_SUB1 towers" << std::endl;

  //RawTowerGeomContainer *geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  //RawTowerGeomContainer *geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  //RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");

  /*
  std::cout << " EM has nphi / neta = " << geomEM->get_phibins() << " / " << geomEM->get_etabins() << std::endl;

  double EMCAL_ETA_LOW[96];
  double EMCAL_ETA_HIGH[96];

  double EMCAL_PHI_LOW[256];
  double EMCAL_PHI_HIGH[256];

  for (int n = 0; n < geomEM->get_phibins(); n++) {
    std::pair<double, double> bounds = geomEM->get_phibounds( n );
    std::cout << " phi bin #" << n << ", bounds = " << bounds.first << " - " << bounds.second << std::endl;
    EMCAL_PHI_LOW[ n ] = bounds.first;
    EMCAL_PHI_HIGH[ n ] = bounds.second;
  }
  for (int n = 0; n < geomEM->get_etabins(); n++) {
    std::pair<double, double> bounds = geomEM->get_etabounds( n );
    std::cout << " eta bin #" << n << ", bounds = " << bounds.first << " - " << bounds.second << std::endl;
    EMCAL_ETA_LOW[ n ] = bounds.first;
    EMCAL_ETA_HIGH[ n ] = bounds.second;
  }

  std::cout << " IH has nphi / neta = " << geomIH->get_phibins() << " / " << geomIH->get_etabins() << std::endl;
  std::cout << " OH has nphi / neta = " << geomOH->get_phibins() << " / " << geomOH->get_etabins() << std::endl;

  double HCAL_ETA_LOW[24];
  double HCAL_ETA_HIGH[24];

  double HCAL_PHI_LOW[64];
  double HCAL_PHI_HIGH[64];

  for (int n = 0; n < geomIH->get_phibins(); n++) {
    std::pair<double, double> bounds = geomIH->get_phibounds( n );
    std::cout << " phi bin #" << n << ", bounds = " << bounds.first << " - " << bounds.second << std::endl;
    HCAL_PHI_LOW[ n ] = bounds.first;
    HCAL_PHI_HIGH[ n ] = bounds.second;
  }
  for (int n = 0; n < geomIH->get_etabins(); n++) {
    std::pair<double, double> bounds = geomIH->get_etabounds( n );
    std::cout << " eta bin #" << n << ", bounds = " << bounds.first << " - " << bounds.second << std::endl;
    HCAL_ETA_LOW[ n ] = bounds.first;
    HCAL_ETA_HIGH[ n ] = bounds.second;
  }

  // emcal
  std::cout << " EMCAL_PHI_LOW[256] = { ";
  for (int n = 0; n < 256; n++) {
    std::cout << EMCAL_PHI_LOW[ n ];
    if (n < 255) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " EMCAL_PHI_HIGH[256] = { ";
  for (int n = 0; n < 256; n++) {
    std::cout << EMCAL_PHI_HIGH[ n ];
    if (n < 255) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " EMCAL_ETA_LOW[96] = { ";
  for (int n = 0; n < 96; n++) {
    std::cout << EMCAL_ETA_LOW[ n ];
    if (n < 95) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " EMCAL_ETA_HIGH[96] = { ";
  for (int n = 0; n < 96; n++) {
    std::cout << EMCAL_ETA_HIGH[ n ];
    if (n < 95) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  // hcal
  std::cout << " HCAL_PHI_LOW[64] = { ";
  for (int n = 0; n < 64; n++) {
    std::cout << HCAL_PHI_LOW[ n ];
    if (n < 63) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " HCAL_PHI_HIGH[64] = { ";
  for (int n = 0; n < 64; n++) {
    std::cout << HCAL_PHI_HIGH[ n ];
    if (n < 63) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " HCAL_ETA_LOW[24] = { ";
  for (int n = 0; n < 24; n++) {
    std::cout << HCAL_ETA_LOW[ n ];
    if (n < 23) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  std::cout << " HCAL_ETA_HIGH[24] = { ";
  for (int n = 0; n < 24; n++) {
    std::cout << HCAL_ETA_HIGH[ n ];
    if (n < 23) std::cout << ", ";
    else std::cout << " }; ";
  }
  std::cout << std::endl;

  return 0;
*/

  _b_tower_n = 0;

  _b_tower_total_0 = 0;
  _b_tower_total_1 = 0;
  _b_tower_total_2 = 0;

  _b_truthjet2_n = 0;
  _b_truthjet3_n = 0;
  _b_truthjet4_n = 0;
  _b_truthjet5_n = 0;

  _b_jet2_n = 0;
  _b_jet3_n = 0;
  _b_jet4_n = 0;
  _b_jet5_n = 0;

  _b_jet2seed_n = 0;

  _b_jet2sub_n = 0;
  _b_jet3sub_n = 0;
  _b_jet4sub_n = 0;
  _b_jet5sub_n = 0;

  _b_bkg_n = 0;

  /*
  // subtracted
  {
    RawTowerContainer::ConstRange begin_end = towersEM4->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 3;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();
      _b_tower_n++;
    }
  }
  {
    RawTowerContainer::ConstRange begin_end = towersIH4->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 4;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();
      _b_tower_n++;
    }
  }
  {
    RawTowerContainer::ConstRange begin_end = towersOH4->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {
      RawTower *tower = rtiter->second;
      RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 5;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();
      _b_tower_n++;
    }
  }
  */

  {
    JetMap* truth_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r02");
    //std::cout << "R = 0.2 truth jets has size " << truth_jets->size() << std::endl;

    for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_truthjet2_pt[ _b_truthjet2_n ] = this_pt;
      _b_truthjet2_eta[ _b_truthjet2_n ] = this_eta;
      _b_truthjet2_phi[ _b_truthjet2_n ] = this_phi;

      std::cout << " truth R=0.2 jet # " << _b_truthjet2_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_truthjet2_n++;
    }

  }

  {
    JetMap* truth_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r03");
    //std::cout << "R = 0.3 truth jets has size " << truth_jets->size() << std::endl;

    for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end(); ++iter) {

      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_truthjet3_pt[ _b_truthjet3_n ] = this_pt;
      _b_truthjet3_eta[ _b_truthjet3_n ] = this_eta;
      _b_truthjet3_phi[ _b_truthjet3_n ] = this_phi;

      std::cout << " truth R=0.3 jet # " << _b_truthjet3_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_truthjet3_n++;
    }

  }

  {
    JetMap* truth_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r04");
    //std::cout << "R = 0.4 truth jets has size " << truth_jets->size() << std::endl;

    for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_truthjet4_pt[ _b_truthjet4_n ] = this_pt;
      _b_truthjet4_eta[ _b_truthjet4_n ] = this_eta;
      _b_truthjet4_phi[ _b_truthjet4_n ] = this_phi;

      std::cout << " truth R=0.4 jet # " << _b_truthjet4_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_truthjet4_n++;
    }
  }

  {
    JetMap* reco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsRaw_r02");
    //std::cout << "reco jets R=0.2 has size " << reco2_jets->size() << std::endl;

    for (JetMap::Iter iter = reco2_jets->begin(); iter != reco2_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      std::cout << " SeedRaw R=0.2 jet #" << _b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;
    }
  }
  {
    JetMap* reco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsSub_r02");
    //std::cout << "reco jets R=0.2 has size " << reco2_jets->size() << std::endl;

    for (JetMap::Iter iter = reco2_jets->begin(); iter != reco2_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      _b_jet2seed_pt[ _b_jet2seed_n ] = this_pt;
      _b_jet2seed_eta[ _b_jet2seed_n ] = this_eta;
      _b_jet2seed_phi[ _b_jet2seed_n ] = this_phi;

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      std::cout << " SeedSub R=0.2 jet #" << _b_jet2seed_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet2seed_n++;
    }
  }

  {
    JetMap* truth_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r05");
    //std::cout << "R = 0.4 truth jets has size " << truth_jets->size() << std::endl;

    for (JetMap::Iter iter = truth_jets->begin(); iter != truth_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_truthjet5_pt[ _b_truthjet5_n ] = this_pt;
      _b_truthjet5_eta[ _b_truthjet5_n ] = this_eta;
      _b_truthjet5_phi[ _b_truthjet5_n ] = this_phi;

      std::cout << " truth R=0.5 jet # " << _b_truthjet5_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_truthjet5_n++;
    }
  }

  /*
  {
    JetMap* reco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02");
    //std::cout << "reco jets R=0.2 has size " << reco2_jets->size() << std::endl;

    for (JetMap::Iter iter = reco2_jets->begin(); iter != reco2_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue;

      _b_jet2_pt[ _b_jet2_n ] = this_pt;
      _b_jet2_eta[ _b_jet2_n ] = this_eta;
      _b_jet2_phi[ _b_jet2_n ] = this_phi;

      std::cout << " pp reco R=0.2 jet #" << _b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet2_n++;
    }
  }

  {
    JetMap* reco3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03");
    //std::cout << "reco jets R=0.3 has size " << reco3_jets->size() << std::endl;

    for (JetMap::Iter iter = reco3_jets->begin(); iter != reco3_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue;

      _b_jet3_pt[ _b_jet3_n ] = this_pt;
      _b_jet3_eta[ _b_jet3_n ] = this_eta;
      _b_jet3_phi[ _b_jet3_n ] = this_phi;

      std::cout << " reco R=0.3 jet #" << _b_jet3_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet3_n++;
    }
  }

  {
    JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue;

      _b_jet4_pt[ _b_jet4_n ] = this_pt;
      _b_jet4_eta[ _b_jet4_n ] = this_eta;
      _b_jet4_phi[ _b_jet4_n ] = this_phi;

      std::cout << " pp reco R=0.4 jet #" << _b_jet4_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet4_n++;
    }
  }
  */

  /*
  {
    JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue;

      _b_jet5_pt[ _b_jet5_n ] = this_pt;
      _b_jet5_eta[ _b_jet5_n ] = this_eta;
      _b_jet5_phi[ _b_jet5_n ] = this_phi;

      std::cout << " reco R=0.5 jet #" << _b_jet5_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet5_n++;
    }
  }
  */


  // now do subtracted collections
  {
    JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02_Sub1");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_jet2sub_pt[ _b_jet2sub_n ] = this_pt;
      _b_jet2sub_eta[ _b_jet2sub_n ] = this_eta;
      _b_jet2sub_phi[ _b_jet2sub_n ] = this_phi;

      std::cout << " reco R=0.2 jet (Sub1) #" << _b_jet2sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet2sub_n++;
    }
  }

  // now do subtracted collections
  {
    JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03_Sub1");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_jet3sub_pt[ _b_jet3sub_n ] = this_pt;
      _b_jet3sub_eta[ _b_jet3sub_n ] = this_eta;
      _b_jet3sub_phi[ _b_jet3sub_n ] = this_phi;

      std::cout << " reco R=0.3 jet (Sub1) #" << _b_jet3sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet3sub_n++;
    }
  }

  // now do subtracted collections
  {
    JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04_Sub1");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_jet4sub_pt[ _b_jet4sub_n ] = this_pt;
      _b_jet4sub_eta[ _b_jet4sub_n ] = this_eta;
      _b_jet4sub_phi[ _b_jet4sub_n ] = this_phi;

      std::cout << " reco R=0.4 jet (Sub1) #" << _b_jet4sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet4sub_n++;
    }
  }


  {
    JetMap* reco5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05_Sub1");
    //std::cout << "reco jets R=0.4 has size " << reco4_jets->size() << std::endl;

    for (JetMap::Iter iter = reco5_jets->begin(); iter != reco5_jets->end(); ++iter) {
      Jet* this_jet = iter->second;

      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();

      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;

      _b_jet5sub_pt[ _b_jet5sub_n ] = this_pt;
      _b_jet5sub_eta[ _b_jet5sub_n ] = this_eta;
      _b_jet5sub_phi[ _b_jet5sub_n ] = this_phi;

      std::cout << " reco R=0.5 jet (Sub1) #" << _b_jet5sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << std::endl;

      _b_jet5sub_n++;
    }
  }


  _b_cluster_n = 0;

  /*
  {
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;

  //std::cout << " I see " << clusters->size() << " clusters " << std::endl;

  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
    RawCluster *cluster = rtiter->second;

    float pt = cluster->get_energy() / cosh(  cluster->get_eta() );

    if (pt < 0.1) continue;

    _b_cluster_pt[ _b_cluster_n ] = pt;
    _b_cluster_eta[ _b_cluster_n ] =  cluster->get_eta();
    _b_cluster_phi[ _b_cluster_n ] =  cluster->get_phi();
    _b_cluster_layer[ _b_cluster_n ] = 0;

    std::cout << " cluster (CEMC) # " << _b_cluster_n << " pt/eta/phi = " << pt << " / " << cluster->get_eta() << " / " << cluster->get_phi() << std::endl;

    _b_cluster_n++;
  }
  }

  {
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;

  //std::cout << " I see " << clusters->size() << " clusters " << std::endl;

  //_b_cluster_n = 0;
  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
    RawCluster *cluster = rtiter->second;

    float pt = cluster->get_energy() / cosh(  cluster->get_eta() );

    if (pt < 0.1) continue;

    _b_cluster_pt[ _b_cluster_n ] = pt;
    _b_cluster_eta[ _b_cluster_n ] =  cluster->get_eta();
    _b_cluster_phi[ _b_cluster_n ] =  cluster->get_phi();
    _b_cluster_layer[ _b_cluster_n ] = 1;

    std::cout << " cluster (IHCal) # " << _b_cluster_n << " pt/eta/phi = " << pt << " / " << cluster->get_eta() << " / " << cluster->get_phi() << std::endl;

    _b_cluster_n++;
  }
  }

  {
  RawClusterContainer *clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");

  RawClusterContainer::ConstRange begin_end = clusters->getClusters();
  RawClusterContainer::ConstIterator rtiter;

  //std::cout << " I see " << clusters->size() << " clusters " << std::endl;

  //_b_cluster_n = 0;
  for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter) {
    RawCluster *cluster = rtiter->second;

    float pt = cluster->get_energy() / cosh(  cluster->get_eta() );

    if (pt < 0.1) continue;

    _b_cluster_pt[ _b_cluster_n ] = pt;
    _b_cluster_eta[ _b_cluster_n ] =  cluster->get_eta();
    _b_cluster_phi[ _b_cluster_n ] =  cluster->get_phi();
    _b_cluster_layer[ _b_cluster_n ] = 2;

    std::cout << " cluster (OHCal) # " << _b_cluster_n << " pt/eta/phi = " << pt << " / " << cluster->get_eta() << " / " << cluster->get_phi() << std::endl;

    _b_cluster_n++;
  }
  }
  */

  _b_particle_n = 0;

  /*
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode,"G4TruthInfo");
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();

  for ( PHG4TruthInfoContainer::ConstIterator iter = range.first; iter != range.second; ++iter ) {
    PHG4Particle* g4particle = iter->second; // You may ask yourself, why second?

    //if ( truthinfo->isEmbeded( g4particle->get_track_id() ) != 17 ) continue;

    TLorentzVector t; t.SetPxPyPzE( g4particle->get_px(), g4particle->get_py(), g4particle->get_pz(), g4particle->get_e() );

    float truth_pt = t.Pt();
    //if (truth_pt < 1) continue;
    float truth_eta = t.Eta();
    if (fabs(truth_eta) > 1.1) continue;
    float truth_phi = t.Phi();
    int truth_pid = g4particle->get_pid();

    //if (truth_pid == 22 || truth_pid == 2112 || truth_pid == -2112 || truth_pid == 130) continue;
    //if (truth_pid == 2112 || truth_pid == -2112 || truth_pid == 130) continue;
    // save high-pT photons
    //if (truth_pid == 22 && truth_pt < 20) continue;
    //if (truth_pid == 12 || truth_pid == -12 || truth_pid == 13 || truth_pid == -13 || truth_pid == 14 || truth_pid == -14) continue;

    _b_particle_pt[ _b_particle_n ] = truth_pt;
    _b_particle_eta[ _b_particle_n ] = truth_eta;
    _b_particle_phi[ _b_particle_n ] = truth_phi;
    _b_particle_pid[ _b_particle_n ] = truth_pid;
    //_b_particle_embed[ _b_particle_n ] = embed_id;

    //std::cout << " --> truth #" << _b_particle_n << ", pt / eta / phi = " << truth_pt << " / " << truth_eta << " / " << truth_phi << ", PID " << truth_pid << ", embed = " <<  truthinfo->isEmbeded( g4particle->get_track_id() ) << std::endl;

    _b_particle_n++;

  }
  */

  /*
  {
    RawTowerContainer::ConstRange begin_end = towersEM3old->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {


      RawTower *tower = rtiter->second;
      _b_tower_total_0 += tower->get_energy();
      RawTowerGeom *tower_geom = geomEM->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 0;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();

      float deta = _b_tower_eta[ _b_tower_n ] - _b_particle_eta[ 0 ] ;
      float dphi = _b_tower_phi[ _b_tower_n ] - _b_particle_phi[ 0 ] ;
      if (dphi > +3.14159) dphi -= 2 * 3.14159;
      if (dphi < -3.14159) dphi += 2 * 3.14159;
      float dR = sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
      if (dR > 0.3) continue;

      _b_tower_n++;
    }
  }
  {
    RawTowerContainer::ConstRange begin_end = towersIH3->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {


      RawTower *tower = rtiter->second;
      _b_tower_total_1 += tower->get_energy();
      RawTowerGeom *tower_geom = geomIH->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 1;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();

      float deta = _b_tower_eta[ _b_tower_n ] - _b_particle_eta[ 0 ] ;
      float dphi = _b_tower_phi[ _b_tower_n ] - _b_particle_phi[ 0 ] ;
      if (dphi > +3.14159) dphi -= 2 * 3.14159;
      if (dphi < -3.14159) dphi += 2 * 3.14159;
      float dR = sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
      if (dR > 0.3) continue;

      _b_tower_n++;
    }
  }
  {
    RawTowerContainer::ConstRange begin_end = towersOH3->getTowers();
    for (RawTowerContainer::ConstIterator rtiter = begin_end.first; rtiter != begin_end.second; ++rtiter) {


      RawTower *tower = rtiter->second;
      _b_tower_total_2 += tower->get_energy();
      RawTowerGeom *tower_geom = geomOH->get_tower_geometry(tower->get_key());
      _b_tower_layer[ _b_tower_n ] = 2;
      _b_tower_E[ _b_tower_n ] = tower->get_energy();
      _b_tower_eta[ _b_tower_n ] = tower_geom->get_eta();
      _b_tower_phi[ _b_tower_n ] = tower_geom->get_phi();

      float deta = _b_tower_eta[ _b_tower_n ] - _b_particle_eta[ 0 ] ;
      float dphi = _b_tower_phi[ _b_tower_n ] - _b_particle_phi[ 0 ] ;
      if (dphi > +3.14159) dphi -= 2 * 3.14159;
      if (dphi < -3.14159) dphi += 2 * 3.14159;
      float dR = sqrt( pow( deta, 2 ) + pow( dphi, 2 ) );
      if (dR > 0.3) continue;

      _b_tower_n++;
    }
  }
  */

  /*
  //

  {
    //RawTowerContainer *towersEM3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_CEMC");
    RawTowerGeomContainer_Cylinderv1 *geomEM = findNode::getClass<RawTowerGeomContainer_Cylinderv1>(topNode, "TOWERGEOM_CEMC");
    int geom_etabins = geomEM->get_etabins();
    int geom_phibins = geomEM->get_phibins();
    std::cout << " neta x nphi = " << geom_etabins << " x " << geom_phibins << std::endl;
    for (int phi = 0; phi < geom_phibins; phi++) {
      std::pair<double, double> b = geomEM->get_phibounds( phi );
      std::cout << " phi bin " << phi << ", bounds : " << b.first << " - " << b.second << ", center = " << geomEM->get_phicenter( phi ) << std::endl;
    }
    for (int eta = 0; eta < geom_etabins; eta++) {
      std::pair<double, double> b = geomEM->get_etabounds( eta );
      std::cout << " eta bin " << eta << ", bounds : " << b.first << " - " << b.second << ", width = " << b.second - b.first << ", center = " << geomEM->get_etacenter( eta ) << std::endl;
    }
  }

  //
  {
    //RawTowerContainer *towersIH3 = findNode::getClass<RawTowerContainer>(topNode, "TOWER_CALIB_HCALIN");
    int geom_etabins = geomIH->get_etabins();
    int geom_phibins = geomIH->get_phibins();
    std::cout << " neta x nphi = " << geom_etabins << " x " << geom_phibins << std::endl;
    for (int phi = 0; phi < geom_phibins; phi++) {
      std::pair<double, double> b = geomIH->get_phibounds( phi );
      std::cout << " phi bin " << phi << ", bounds : " << b.first << " - " << b.second << ", center = " << geomIH->get_phicenter( phi ) << std::endl;
    }
    for (int eta = 0; eta < geom_etabins; eta++) {
      std::pair<double, double> b = geomIH->get_etabounds( eta );
      std::cout << " eta bin " << eta << ", bounds : " << b.first << " - " << b.second << ", width = " << b.second - b.first << ", center = " << geomIH->get_etacenter( eta ) << std::endl;
    }
  }

  //
  {
    RawTowerGeomContainer *geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
    int geom_etabins = geomOH->get_etabins();
    int geom_phibins = geomOH->get_phibins();
    std::cout << " neta x nphi = " << geom_etabins << " x " << geom_phibins << std::endl;
    for (int phi = 0; phi < geom_phibins; phi++) {
      std::pair<double, double> b = geomOH->get_phibounds( phi );
      std::cout << " phi bin " << phi << ", bounds : " << b.first << " - " << b.second << ", center = " << geomOH->get_phicenter( phi ) << std::endl;
    }
    for (int eta = 0; eta < geom_etabins; eta++) {
      std::pair<double, double> b = geomOH->get_etabounds( eta );
      std::cout << " eta bin " << eta << ", bounds : " << b.first << " - " << b.second << ", width = " << b.second - b.first << ", center = " << geomOH->get_etacenter( eta ) << std::endl;
    }
  }


  //

  CaloTriggerInfo* triggerinfo = findNode::getClass<CaloTriggerInfo>(topNode,"CaloTriggerInfo");

  if (triggerinfo) {
    std::cout << " triggerinfo = " << triggerinfo << std::endl;
    std::cout << " --> get_best_EMCal_4x4_E = " << triggerinfo->get_best_EMCal_4x4_E() << std::endl;
    //std::cout << " --> get_best_EMCal_4x4_eta = " << triggerinfo->get_best_EMCal_4x4_eta() << std::endl;
    //std::cout << " --> get_best_EMCal_4x4_phi = " << triggerinfo->get_best_EMCal_4x4_phi() << std::endl;

    _b_trigEM2_E =  triggerinfo->get_best_EMCal_2x2_E();
    _b_trigEM2_eta = triggerinfo->get_best_EMCal_2x2_eta();
    _b_trigEM2_phi = triggerinfo->get_best_EMCal_2x2_phi();

    _b_trigEM4_E =  triggerinfo->get_best_EMCal_4x4_E();
    _b_trigEM4_eta = triggerinfo->get_best_EMCal_4x4_eta();
    _b_trigEM4_phi = triggerinfo->get_best_EMCal_4x4_phi();

    //

    _b_trigFull2_E = triggerinfo->get_best_FullCalo_0p2x0p2_E();
    std::cout << " --> get_best_FullCalo_0p2x0p2_E = " << _b_trigFull2_E << std::endl;

    _b_trigFull4_E = triggerinfo->get_best_FullCalo_0p4x0p4_E();
    std::cout << " --> get_best_FullCalo_0p4x0p4_E = " << _b_trigFull4_E << std::endl;

    _b_trigFull6_E = triggerinfo->get_best_FullCalo_0p6x0p6_E();
    std::cout << " --> get_best_FullCalo_0p6x0p6_E = " << _b_trigFull6_E << std::endl;

    _b_trigFull8_E = triggerinfo->get_best_FullCalo_0p8x0p8_E();
    std::cout << " --> get_best_FullCalo_0p8x0p8_E = " << _b_trigFull8_E << std::endl;

    _b_trigFull10_E = triggerinfo->get_best_FullCalo_1p0x1p0_E();
    std::cout << " --> get_best_FullCalo_1p0x1p0_E = " << _b_trigFull10_E << std::endl;

    _b_trigFull2_eta = triggerinfo->get_best_FullCalo_0p2x0p2_eta();
    _b_trigFull4_eta = triggerinfo->get_best_FullCalo_0p4x0p4_eta();
    _b_trigFull6_eta = triggerinfo->get_best_FullCalo_0p6x0p6_eta();
    _b_trigFull8_eta = triggerinfo->get_best_FullCalo_0p8x0p8_eta();
    _b_trigFull10_eta = triggerinfo->get_best_FullCalo_1p0x1p0_eta();

    _b_trigFull2_phi = triggerinfo->get_best_FullCalo_0p2x0p2_phi();
    _b_trigFull4_phi = triggerinfo->get_best_FullCalo_0p4x0p4_phi();
    _b_trigFull6_phi = triggerinfo->get_best_FullCalo_0p6x0p6_phi();
    _b_trigFull8_phi = triggerinfo->get_best_FullCalo_0p8x0p8_phi();
    _b_trigFull10_phi = triggerinfo->get_best_FullCalo_1p0x1p0_phi();
  }

  */

  TowerBackground* towerbackground1 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub1");
  std::cout << "TowerBackground_Sub1" << std::endl;
  //towerbackground->identify();

  TowerBackground* towerbackground2 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub2");
  std::cout << "TowerBackground_Sub2" << std::endl;
  //towerbackground2->identify();

  _b_bkg_n = 0;
  _b_bkg2_n = 0;

  for (int layer = 0; layer < 3; layer++) {
    for (unsigned int n = 0; n < towerbackground1->get_UE( layer ).size(); n++) {
      _b_bkg_layer[ _b_bkg_n ] = layer;
      _b_bkg_eta[ _b_bkg_n ] = n;
      _b_bkg_E[ _b_bkg_n ] =  towerbackground1->get_UE( layer ).at( n );

      _b_bkg_n++;
    }
  }

  for (int layer = 0; layer < 3; layer++) {
    for (unsigned int n = 0; n < towerbackground2->get_UE( layer ).size(); n++) {
      _b_bkg2_layer[ _b_bkg2_n ] = layer;
      _b_bkg2_eta[ _b_bkg2_n ] = n;
      _b_bkg2_E[ _b_bkg2_n ] =  towerbackground2->get_UE( layer ).at( n );

      _b_bkg2_n++;
    }
  }

  _tree->Fill();

  return 0;
}



int TreeMaker::End(PHCompositeNode *topNode)
{

  _f->Write();
  _f->Close();

  return 0;
}

