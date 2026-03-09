// Truth Calorimeter Calibration TreeMaker
#include <fun4all/Fun4AllBase.h>
#include <iostream>
#include "TruthCaloTree.h"

// General F4A includes 
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4VtxPoint.h>

// Calorimeter includes
#include <calobase/RawTowerv2.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>

// ROOT includes 
#include "TLorentzVector.h"
#include "TMath.h"
#include "TROOT.h"
#include "TH2.h"

TruthCaloTree::TruthCaloTree(const std::string &name) : SubsysReco("TRUTH_CALO_TREE")
{
  _foutname = name;
  _hcal_sim_name = "TOWER_SIM_HCALOUT";
  _hcalIN_sim_name = "TOWER_SIM_HCALIN";
  _EMcal_sim_name = "TOWER_SIM_CEMC";

}

int TruthCaloTree::reset_tree_vars() {

  _b_event = -99; 
  _b_tower_sim_n = -99;
  _b_EMcal_sim_n = -99;
  _b_hcalIN_sim_n = -99;
  _nBH = -99;

  for (int i = 0; i <nTowers; i++){
    
    _b_tower_sim_E[i] = -99;
    _b_tower_sim_eta[i] = -99;
    _b_tower_sim_phi[i] = -99;
    _b_tower_sim_ieta[i] = -99;
    _b_tower_sim_iphi[i] = -99;
    
    _b_EMcal_sim_E[i] = -99;
    _b_EMcal_sim_eta[i] = -99;
    _b_EMcal_sim_phi[i] = -99;
    _b_EMcal_sim_iphi[i] = -99;
    _b_EMcal_sim_ieta[i] = -99;

    _b_hcalIN_sim_E[i] = -99;
    _b_hcalIN_sim_eta[i] = -99;
    _b_hcalIN_sim_phi[i] = -99;
    _b_hcalIN_sim_iphi[i] = -99;
    _b_hcalIN_sim_ieta[i] = -99;
  }
  
  _b_n_truth = -99;
  n_child = -99;
  n_vertex = -99;

  for (int i = 0; i < nTruth; i++) {
    _b_truthenergy[i] = -99;
    _b_trutheta[i] = -99;
    _b_truthphi[i] = -99;
    _b_truthpt[i] = -99;
    _b_truthp[i] = -99;
    _b_truthpid[i] = -99;
    _b_truth_trackid[i] = -99;
    vertex_id[i] = -99;
    vertex_x[i] = -99;
    vertex_y[i] = -99;
    vertex_z[i] = -99;
    _BH_e[i] = -99;
    _BH_px[i] = -99;
    _BH_py[i] = -99;
    _BH_pz[i] = -99;
    _BH_track_id[i] = -99;
  }

  for (int i = 0; i < nTruth; i++) {
    child_vertex_id[i] = -99;
    child_parent_id[i] = -99;
    child_pid[i] = -99;
    child_px[i] = -99;
    child_py[i] = -99;
    child_pz[i] = -99;
    child_energy[i] = -99;
  }

  return 1;
}

int TruthCaloTree::GetNodes(PHCompositeNode *topNode) {

  blackhole = findNode::getClass<PHG4HitContainer>(topNode, "G4HIT_BH_1");
  if (!blackhole) std::cout << "No blackhole" << std::endl;

  if (_debug) std::cout<<"GettingNodes..."<<std::endl;
  _geomOH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALOUT");
  if(!_geomOH) std::cout<<"No TOWERGeOM_HCALOUT"<<std::endl;

  _geomIH = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_HCALIN");
  if(!_geomIH) std::cout<<"No TOWERGeOM_HCALIN"<<std::endl;

  _geomEM = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if(!_geomEM) std::cout<<"No TOWERGeOM_CEMC"<<std::endl;
  
  _towersSimOH = findNode::getClass<RawTowerContainer>(topNode, _hcal_sim_name);
  if (!_towersSimOH) std::cout<<"No TOWER_SIM_HCALOUT Node"<<std::endl;
  
  _towersSimIH = findNode::getClass<RawTowerContainer>(topNode, _hcalIN_sim_name);
  if (!_towersSimIH) std::cout<<"No TOWER_SIM_HCALIN Node"<<std::endl;

  _towersSimEM = findNode::getClass<RawTowerContainer>(topNode, _EMcal_sim_name);
  if (!_towersSimEM) std::cout<<"No TOWER_SIM_CEMC Node"<<std::endl;

  truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!truthinfo) std::cout << "PHG4TruthInfoContainer node is missing, can't collect G4 truth particles"<< std::endl;

  return 1;
}

int TruthCaloTree::Init(PHCompositeNode *topNode) {
  _ievent = 0;
  _b_event = -1;

  if (_debug) std::cout<<"Initiating..."<<std::endl;

  reset_tree_vars();

  _file = new TFile(_foutname.c_str(), "RECREATE");
  
  _tree = new TTree("T", "keep on giving tree");
    
  _tree->Branch("n_truth",&_b_n_truth,"n_truth/I");
  _tree->Branch("truthenergy",_b_truthenergy,"truthenergy[n_truth]/F");
  _tree->Branch("trutheta",_b_trutheta,"trutheta[n_truth]/F");
  _tree->Branch("truthphi",_b_truthphi,"truthphi[n_truth]/F");
  _tree->Branch("truthpt",_b_truthpt,"truthpt[n_truth]/F");
  _tree->Branch("truthp",_b_truthp,"truthp[n_truth]/F");
  _tree->Branch("truthpid",_b_truthpid,"truthpid[n_truth]/I");
  _tree->Branch("truth_track_id",_b_truth_trackid,"truth_track_id[n_truth]/I");

  _tree->Branch("EMcal_sim_n",   &_b_EMcal_sim_n,     "EMcal_sim_n/I");
  _tree->Branch("EMcal_sim_E",    _b_EMcal_sim_E,     "EMcal_sim_E[EMcal_sim_n]/F");
  _tree->Branch("EMcal_sim_eta",  _b_EMcal_sim_eta,   "EMcal_sim_eta[EMcal_sim_n]/F");
  _tree->Branch("EMcal_sim_phi",  _b_EMcal_sim_phi,   "EMcal_sim_phi[EMcal_sim_n]/F");
  _tree->Branch("EMcal_sim_iphi",  _b_EMcal_sim_iphi,   "EMcal_sim_iphi[EMcal_sim_n]/I");
  _tree->Branch("EMcal_sim_ieta",  _b_EMcal_sim_ieta,   "EMcal_sim_ieta[EMcal_sim_n]/I");

  _tree->Branch("hcalIN_sim_n",   &_b_hcalIN_sim_n,     "hcalIN_sim_n/I");
  _tree->Branch("hcalIN_sim_E",    _b_hcalIN_sim_E,     "hcalIN_sim_E[hcalIN_sim_n]/F");
  _tree->Branch("hcalIN_sim_eta",  _b_hcalIN_sim_eta,   "hcalIN_sim_eta[hcalIN_sim_n]/F");
  _tree->Branch("hcalIN_sim_phi",  _b_hcalIN_sim_phi,   "hcalIN_sim_phi[hcalIN_sim_n]/F");
  _tree->Branch("hcalIN_sim_iphi",  _b_hcalIN_sim_iphi,   "hcalIN_sim_iphi[hcalIN_sim_n]/I");
  _tree->Branch("hcalIN_sim_ieta",  _b_hcalIN_sim_ieta,   "hcalIN_sim_ieta[hcalIN_sim_n]/I");

  _tree->Branch("tower_sim_n",&_b_tower_sim_n, "tower_sim_n/I");
  _tree->Branch("tower_sim_E",_b_tower_sim_E, "tower_sim_E[tower_sim_n]/F");
  _tree->Branch("tower_sim_eta",_b_tower_sim_eta, "tower_sim_eta[tower_sim_n]/F");
  _tree->Branch("tower_sim_phi",_b_tower_sim_phi, "tower_sim_phi[tower_sim_n]/F");
  _tree->Branch("tower_sim_ieta",_b_tower_sim_ieta, "tower_sim_ieta[tower_sim_n]/I");
  _tree->Branch("tower_sim_iphi",_b_tower_sim_iphi, "tower_sim_iphi[tower_sim_n]/I");

  _tree->Branch("n_vertex",&n_vertex,"n_vertex/I");
  _tree->Branch("vertex_id",vertex_id,"vertex_id[n_vertex]/I");
  _tree->Branch("vertex_x",vertex_x,"vertex_x[n_vertex]/F");
  _tree->Branch("vertex_y",vertex_y,"vertex_y[n_vertex]/F");
  _tree->Branch("vertex_z",vertex_z,"vertex_z[n_vertex]/F");

  _tree->Branch("n_child",&n_child,"n_child/I");
  _tree->Branch("child_vertex_id",child_vertex_id,"child_vertex_id[n_child]/I");
  _tree->Branch("child_parent_id",child_parent_id,"child_parent_id[n_child]/I");
  _tree->Branch("child_pid",child_pid,"child_pid[n_child]/I");
  _tree->Branch("child_px",child_px,"child_px[n_child]/F");
  _tree->Branch("child_py",child_py,"child_py[n_child]/F");
  _tree->Branch("child_pz",child_pz,"child_pz[n_child]/F");
  _tree->Branch("child_energy",child_energy,"child_energy[n_child]/F");

  _tree->Branch("BH_n",&_nBH,"BH_n/I");
  _tree->Branch("BH_px",_BH_px,"BH_px[BH_n]/F");
  _tree->Branch("BH_py",_BH_py,"BH_py[BH_n]/F");
  _tree->Branch("BH_pz",_BH_pz,"BH_pz[BH_n]/F");
  _tree->Branch("BH_track_id",_BH_track_id,"BH_track_id[BH_n]/I");
  _tree->Branch("BH_e",_BH_e,"BH_e[BH_n]/F");


  return 0;
}

int TruthCaloTree::process_event(PHCompositeNode *topNode) {

  GetNodes(topNode);
  
  _b_event = _ievent;
  if (_ievent %5000==0) std::cout<<"Event: "<<_ievent<<std::endl;


  if (_debug) std::cout<<"Processing Event: "<< _b_event<<std::endl;
  if (_debug) std::cout << "hello";

  ////////////////////
  // BLACKHOLE INFO
  ////////////////////

  _nBH = 0;
  PHG4HitContainer::ConstRange bh_hit_range = blackhole->getHits();
  for (PHG4HitContainer::ConstIterator hit_iter = bh_hit_range.first;
         hit_iter != bh_hit_range.second; hit_iter++)
    {
      PHG4Hit *this_hit = hit_iter->second;
      assert(this_hit);
      _BH_e[_nBH] = this_hit->get_edep();
      _BH_px[_nBH] = this_hit->get_px(0);
      _BH_py[_nBH] = this_hit->get_py(0);
      _BH_pz[_nBH] = this_hit->get_pz(0);
      PHG4Particle *p = truthinfo->GetParticle(this_hit->get_trkid());
      _BH_track_id[_nBH] = p->get_track_id();
      _nBH++;
    }
  
  //////////////////////////////
  // OUTER HCAL
  //////////////////////////////
  
  _b_tower_sim_n=0;
  RawTowerContainer::ConstRange begin_end_sim = _towersSimOH->getTowers();
  if (_debug) std::cout<<"Got the iterator"<<std::endl;

  for (RawTowerContainer::ConstIterator rtiter = begin_end_sim.first; rtiter != begin_end_sim.second; ++rtiter) {
    if ((_b_tower_sim_n%10==0)&&(_debug)) std::cout<<"At sim tower: "<< _b_tower_sim_n<<std::endl;

    RawTower *tower = rtiter->second;
    RawTowerGeom *tower_geom = _geomOH->get_tower_geometry(tower->get_key());
      _b_tower_sim_E   [ _b_tower_sim_n ] = tower->get_energy();
      _b_tower_sim_eta [ _b_tower_sim_n ] = tower_geom->get_eta();
      _b_tower_sim_phi [ _b_tower_sim_n ] = tower_geom->get_phi();
      _b_tower_sim_ieta[ _b_tower_sim_n ] = tower_geom->get_bineta();
      _b_tower_sim_iphi[ _b_tower_sim_n ] = tower_geom->get_binphi();
      _b_tower_sim_n++;
    if(_b_tower_sim_n >= nTowers){
      std::cout << __FILE__ << " ERROR: _b_tower_sim_n has hit cap of " << nTowers << "!!!" << std::endl;
    } 
  }

  //////////////////////
  // INNER HCAL
  /////////////////////
  
  _b_hcalIN_sim_n = 0;
  RawTowerContainer::ConstRange begin_end_simIN = _towersSimIH->getTowers();
  for (RawTowerContainer::ConstIterator rtiter = begin_end_simIN.first; rtiter != begin_end_simIN.second; ++rtiter) {
    RawTower *tower = rtiter->second;
    RawTowerGeom *tower_geom = _geomIH->get_tower_geometry(tower->get_key());
      _b_hcalIN_sim_E    [ _b_hcalIN_sim_n ] = tower->get_energy();
      _b_hcalIN_sim_eta  [ _b_hcalIN_sim_n ] = tower_geom->get_eta();
      _b_hcalIN_sim_phi  [ _b_hcalIN_sim_n ] = tower_geom->get_phi();
      _b_hcalIN_sim_ieta [ _b_hcalIN_sim_n ] = tower_geom->get_bineta();
      _b_hcalIN_sim_iphi [ _b_hcalIN_sim_n ] = tower_geom->get_binphi();
      _b_hcalIN_sim_n++;

  }

  //////////////////////
  // EM CAL
  //////////////////////
  
  _b_EMcal_sim_n = 0;
  RawTowerContainer::ConstRange begin_end_simEM = _towersSimEM->getTowers();
  for (RawTowerContainer::ConstIterator rtiter = begin_end_simEM.first; rtiter != begin_end_simEM.second; ++rtiter) {
    RawTower *tower = rtiter->second;
    RawTowerGeom *tower_geom = _geomEM->get_tower_geometry(tower->get_key());
      _b_EMcal_sim_E    [ _b_EMcal_sim_n ] = tower->get_energy();
      _b_EMcal_sim_eta  [ _b_EMcal_sim_n ] = tower_geom->get_eta();
      _b_EMcal_sim_phi  [ _b_EMcal_sim_n ] = tower_geom->get_phi();
      _b_EMcal_sim_ieta [ _b_EMcal_sim_n ] = tower_geom->get_bineta();
      _b_EMcal_sim_iphi [ _b_EMcal_sim_n ] = tower_geom->get_binphi();
      _b_EMcal_sim_n++;

  }
  
  ////////////////////
  // PRIMARY TRUTH INFO
  ////////////////////
  
  
  n_child = 0;
  //std::set <int> primary;
  std::set<int> vertex;
  //if (!primary.empty()) primary.clear();
  if (!vertex.empty()) vertex.clear();
  PHG4TruthInfoContainer::Range range = truthinfo->GetPrimaryParticleRange();
  _b_n_truth = 0;
  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second; ++iter) {
    const PHG4Particle *truth = iter->second;
    if (!truthinfo->is_primary(truth)) continue;
    _b_truthp[_b_n_truth] = sqrt(truth->get_px() * truth->get_px()
                           + truth->get_py() * truth->get_py()
                           + truth->get_pz() * truth->get_pz());
    _b_truthpt[_b_n_truth] = sqrt(truth->get_px() * truth->get_px()
                            + truth->get_py() * truth->get_py());
    _b_truthenergy[_b_n_truth] = truth->get_e();
    _b_truthphi[_b_n_truth] = atan2(truth->get_py(),truth->get_px());
    _b_trutheta[_b_n_truth] = atanh(truth->get_pz() / _b_truthenergy[_b_n_truth]);

    /// Check for nansx
    if (_b_trutheta[_b_n_truth] != _b_trutheta[_b_n_truth])
      _b_trutheta[_b_n_truth] = -99;
    _b_truthpid[_b_n_truth] = truth->get_pid();
    _b_truth_trackid[_b_n_truth] = truth->get_track_id();

    //if (truth->get_e() > 2) primary.insert(truth->get_track_id());
    _b_n_truth++;
    if (_b_n_truth >= 50000) break;
  }

  // for finding all particles of a hadron shower recursively 
  /*
  PHG4TruthInfoContainer::Range second_range = truthinfo->GetSecondaryParticleRange();
  for (PHG4TruthInfoContainer::ConstIterator iter = second_range.second; iter != second_range.first; --iter) {
    PHG4Particle *child = iter->second;
    if (primary.find(child->get_parent_id()) != primary.end()) {
      primary.insert(child->get_track_id());
      vertex.insert(child->get_vtx_id());
      child_pid[n_child] = child->get_pid();
      child_parent_id[n_child] = child->get_parent_id();
      child_vertex_id[n_child] = child->get_vtx_id();
      child_px[n_child] = child->get_px();
      child_py[n_child] = child->get_py();
      child_pz[n_child] = child->get_pz();
      child_energy[n_child] = child->get_e();
      n_child++;
      }
    }
    */

  ///////////////////////////////////
  // SECONDARY AND VERTEX TRUTH INFO
  ///////////////////////////////////

  
  PHG4TruthInfoContainer::Range second_range = truthinfo->GetSecondaryParticleRange();

  for (PHG4TruthInfoContainer::ConstIterator iter = second_range.first; iter != second_range.second; ++iter) {
    const PHG4Particle *child = iter->second;
    if (child->get_parent_id() > 0) {
      PHG4Particle *parent = truthinfo->GetParticle(child->get_parent_id());
      if (parent->get_e() > 2.0) {
        vertex.insert(child->get_vtx_id());
        child_pid[n_child] = child->get_pid();
        child_parent_id[n_child] = child->get_parent_id();
        child_vertex_id[n_child] = child->get_vtx_id();
        child_px[n_child] = child->get_px();
        child_py[n_child] = child->get_py();
        child_pz[n_child] = child->get_pz();
        child_energy[n_child] = child->get_e();
        n_child++;
      }
    }
  }
  

  PHG4TruthInfoContainer::VtxRange vtxrange = truthinfo->GetVtxRange();
  n_vertex = 0;
  for (PHG4TruthInfoContainer::ConstVtxIterator iter = vtxrange.first; iter != vtxrange.second; ++iter) {
    PHG4VtxPoint *vtx = iter->second;
    if (vertex.find(vtx->get_id()) != vertex.end()) {
      vertex_x[n_vertex] = vtx->get_x();
      vertex_y[n_vertex] = vtx->get_y();
      vertex_z[n_vertex] = vtx->get_z();
      vertex_id[n_vertex] = vtx->get_id();
      n_vertex++;
    }
  }

  _file->cd();
  _tree->Fill();
  _ievent++;
  
  return 0;
}

int TruthCaloTree::End(PHCompositeNode *topNode)
{
  if (_debug) std::cout<<"Writing File"<<std::endl;
  _file->cd();
  _file->Write();
  _file->Close();

  return 0;
}
