#include <iostream>

// --- header for this clase
#include <TreeMaker.h>

// --- basic sPhenix environment stuff
#include <fun4all/Fun4AllServer.h>
#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

// --- trigger
//#include <calotrigger/CaloTriggerInfo.h>

// --- truth information
//#include <g4main/PHG4TruthInfoContainer.h>
//#include <g4main/PHG4Particle.h>

// --- calorimeter towers
//#include <g4cemc/RawTowerGeom.h>
//#include <g4cemc/RawTower.h>
//#include <g4cemc/RawTowerContainer.h>
//#include <g4cemc/RawTowerGeomContainer_Cylinderv1.h>
//#include <g4cemc/RawTowerGeomContainer.h>
// --- calorimeter clusters
//#include <g4cemc/RawClusterContainer.h>
//#include <g4cemc/RawCluster.h>

// --- jet specific stuff
#include <TLorentzVector.h>
#include <g4jets/JetMap.h>
#include <g4jets/Jet.h>
#include <jetbackground/TowerBackground.h>


using std::cout;
using std::endl;

TreeMaker::TreeMaker(const std::string &name) : SubsysReco("TREEMAKER")
{
  foutname = name;
}


int TreeMaker::Init(PHCompositeNode *topNode)
{

  outfile = new TFile( foutname.c_str(), "RECREATE");

  tree = new TTree("ttree","sequoia");

  tree->Branch("bkg_n",&bbkg_n, "bkg_n/I");
  tree->Branch("bkg_layer",&bbkg_layer, "bkg_layer[bkg_n]/I");
  tree->Branch("bkg_eta",&bbkg_eta, "bkg_eta[bkg_n]/I");
  tree->Branch("bkg_e",&bbkg_e, "bkg_e[bkg_n]/F");

  tree->Branch("bkg2_n",     &bbkg2_n,     "bkg2_n/I");
  tree->Branch("bkg2_layer", &bbkg2_layer, "bkg2_layer[bkg2_n]/I");
  tree->Branch("bkg2_eta",   &bbkg2_eta,   "bkg2_eta[bkg2_n]/I");
  tree->Branch("bkg2_e",     &bbkg2_e,     "bkg2_e[bkg2_n]/F");

  tree->Branch("particle_n", &b_particle_n,"particle_n/I");
  tree->Branch("particle_e", b_particle_e,"particle_e[particle_n]/F");
  tree->Branch("particle_pt", b_particle_pt,"particle_pt[particle_n]/F");
  tree->Branch("particle_eta", b_particle_eta,"particle_eta[particle_n]/F");
  tree->Branch("particle_phi", b_particle_phi,"particle_phi[particle_n]/F");
  tree->Branch("particle_pid", b_particle_pid,"particle_pid[particle_n]/I");

  tree->Branch("jet2_n", &b_jet2_n,"jet2_n/I");
  tree->Branch("jet2_e", b_jet2_e,"jet2_e[jet2_n]/F");
  tree->Branch("jet2_pt", b_jet2_pt,"jet2_pt[jet2_n]/F");
  tree->Branch("jet2_eta",b_jet2_eta,"jet2_eta[jet2_n]/F");
  tree->Branch("jet2_phi",b_jet2_phi,"jet2_phi[jet2_n]/F");

  tree->Branch("jet3_n", &b_jet3_n,"jet3_n/I");
  tree->Branch("jet3_e", b_jet3_e,"jet3_e[jet3_n]/F");
  tree->Branch("jet3_pt", b_jet3_pt,"jet3_pt[jet3_n]/F");
  tree->Branch("jet3_eta",b_jet3_eta,"jet3_eta[jet3_n]/F");
  tree->Branch("jet3_phi",b_jet3_phi,"jet3_phi[jet3_n]/F");

  tree->Branch("jet4_n", &b_jet4_n,"jet4_n/I");
  tree->Branch("jet4_e", b_jet4_e,"jet4_e[jet4_n]/F");
  tree->Branch("jet4_pt", b_jet4_pt,"jet4_pt[jet4_n]/F");
  tree->Branch("jet4_eta",b_jet4_eta,"jet4_eta[jet4_n]/F");
  tree->Branch("jet4_phi",b_jet4_phi,"jet4_phi[jet4_n]/F");

  tree->Branch("jet5_n", &b_jet5_n,"jet5_n/I");
  tree->Branch("jet5_e", b_jet5_e,"jet5_e[jet5_n]/F");
  tree->Branch("jet5_pt", b_jet5_pt,"jet5_pt[jet5_n]/F");
  tree->Branch("jet5_eta",b_jet5_eta,"jet5_eta[jet5_n]/F");
  tree->Branch("jet5_phi",b_jet5_phi,"jet5_phi[jet5_n]/F");

  // tree->Branch("jet2seed_n", &b_jet2seed_n,"jet2seed_n/I");
  // tree->Branch("jet2seed_e", b_jet2seed_e,"jet2seed_e[jet2seed_n]/F");
  // tree->Branch("jet2seed_pt", b_jet2seed_pt,"jet2seed_pt[jet2seed_n]/F");
  // tree->Branch("jet2seed_eta",b_jet2seed_eta,"jet2seed_eta[jet2seed_n]/F");
  // tree->Branch("jet2seed_phi",b_jet2seed_phi,"jet2seed_phi[jet2seed_n]/F");

  tree->Branch("jet2sub_n", &b_jet2sub_n,"jet2sub_n/I");
  tree->Branch("jet2sub_e", b_jet2sub_e,"jet2sub_e[jet2sub_n]/F");
  tree->Branch("jet2sub_pt", b_jet2sub_pt,"jet2sub_pt[jet2sub_n]/F");
  tree->Branch("jet2sub_eta",b_jet2sub_eta,"jet2sub_eta[jet2sub_n]/F");
  tree->Branch("jet2sub_phi",b_jet2sub_phi,"jet2sub_phi[jet2sub_n]/F");

  tree->Branch("jet3sub_n", &b_jet3sub_n,"jet3sub_n/I");
  tree->Branch("jet3sub_e", b_jet3sub_e,"jet3sub_e[jet3sub_n]/F");
  tree->Branch("jet3sub_pt", b_jet3sub_pt,"jet3sub_pt[jet3sub_n]/F");
  tree->Branch("jet3sub_eta",b_jet3sub_eta,"jet3sub_eta[jet3sub_n]/F");
  tree->Branch("jet3sub_phi",b_jet3sub_phi,"jet3sub_phi[jet3sub_n]/F");

  tree->Branch("jet4sub_n", &b_jet4sub_n,"jet4sub_n/I");
  tree->Branch("jet4sub_e", b_jet4sub_e,"jet4sub_e[jet4sub_n]/F");
  tree->Branch("jet4sub_pt", b_jet4sub_pt,"jet4sub_pt[jet4sub_n]/F");
  tree->Branch("jet4sub_eta",b_jet4sub_eta,"jet4sub_eta[jet4sub_n]/F");
  tree->Branch("jet4sub_phi",b_jet4sub_phi,"jet4sub_phi[jet4sub_n]/F");

  tree->Branch("jet5sub_n", &b_jet5sub_n,"jet5sub_n/I");
  tree->Branch("jet5sub_e", b_jet5sub_e,"jet5sub_e[jet5sub_n]/F");
  tree->Branch("jet5sub_pt", b_jet5sub_pt,"jet5sub_pt[jet5sub_n]/F");
  tree->Branch("jet5sub_eta",b_jet5sub_eta,"jet5sub_eta[jet5sub_n]/F");
  tree->Branch("jet5sub_phi",b_jet5sub_phi,"jet5sub_phi[jet5sub_n]/F");

  tree->Branch("truthjet2_n", &b_truthjet2_n,"truthjet2_n/I");
  tree->Branch("truthjet2_e", b_truthjet2_e,"truthjet2_e[truthjet2_n]/F");
  tree->Branch("truthjet2_pt", b_truthjet2_pt,"truthjet2_pt[truthjet2_n]/F");
  tree->Branch("truthjet2_eta",b_truthjet2_eta,"truthjet2_eta[truthjet2_n]/F");
  tree->Branch("truthjet2_phi",b_truthjet2_phi,"truthjet2_phi[truthjet2_n]/F");

  tree->Branch("truthjet3_n", &b_truthjet3_n,"truthjet3_n/I");
  tree->Branch("truthjet3_e", b_truthjet3_e,"truthjet3_e[truthjet3_n]/F");
  tree->Branch("truthjet3_pt", b_truthjet3_pt,"truthjet3_pt[truthjet3_n]/F");
  tree->Branch("truthjet3_eta",b_truthjet3_eta,"truthjet3_eta[truthjet3_n]/F");
  tree->Branch("truthjet3_phi",b_truthjet3_phi,"truthjet3_phi[truthjet3_n]/F");

  tree->Branch("truthjet4_n", &b_truthjet4_n,"truthjet4_n/I");
  tree->Branch("truthjet4_e", b_truthjet4_e,"truthjet4_e[truthjet4_n]/F");
  tree->Branch("truthjet4_pt", b_truthjet4_pt,"truthjet4_pt[truthjet4_n]/F");
  tree->Branch("truthjet4_eta",b_truthjet4_eta,"truthjet4_eta[truthjet4_n]/F");
  tree->Branch("truthjet4_phi",b_truthjet4_phi,"truthjet4_phi[truthjet4_n]/F");

  tree->Branch("truthjet5_n", &b_truthjet5_n,"truthjet5_n/I");
  tree->Branch("truthjet5_e", b_truthjet5_e,"truthjet5_e[truthjet5_n]/F");
  tree->Branch("truthjet5_pt", b_truthjet5_pt,"truthjet5_pt[truthjet5_n]/F");
  tree->Branch("truthjet5_eta",b_truthjet5_eta,"truthjet5_eta[truthjet5_n]/F");
  tree->Branch("truthjet5_phi",b_truthjet5_phi,"truthjet5_phi[truthjet5_n]/F");

  return 0;

}

int TreeMaker::process_event(PHCompositeNode *topNode)
{

  //cout << "DVP : at process_event, tree size is: " << tree->GetEntries() << endl;


  b_tower_n = 0;

  b_tower_total_0 = 0;
  b_tower_total_1 = 0;
  b_tower_total_2 = 0;

  b_truthjet2_n = 0;
  b_truthjet3_n = 0;
  b_truthjet4_n = 0;
  b_truthjet5_n = 0;

  b_jet2_n = 0;
  b_jet3_n = 0;
  b_jet4_n = 0;
  b_jet5_n = 0;

  b_jet2seed_n = 0;

  b_jet2sub_n = 0;
  b_jet3sub_n = 0;
  b_jet4sub_n = 0;
  b_jet5sub_n = 0;

  bbkg_n = 0;



  // ------------------------------------------------------------------------------
  // --- get truth jet information
  // ------------------------------------------------------------------------------

  JetMap* truth2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r02");
  //cout << "R = 0.2 truth jets has size " << truth2_jets->size() << endl;
  for (JetMap::Iter iter = truth2_jets->begin(); iter != truth2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet2_e[ b_truthjet2_n ] = this_e;
      b_truthjet2_pt[ b_truthjet2_n ] = this_pt;
      b_truthjet2_eta[ b_truthjet2_n ] = this_eta;
      b_truthjet2_phi[ b_truthjet2_n ] = this_phi;
      //cout << " truth R=0.2 jet # " << b_truthjet2_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet2_n++;
    } // loop over R = 0.2 truth jets

  JetMap* truth3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r03");
  //cout << "R = 0.3 truth jets has size " << truth3_jets->size() << endl;
  for (JetMap::Iter iter = truth3_jets->begin(); iter != truth3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet3_e[ b_truthjet3_n ] = this_e;
      b_truthjet3_pt[ b_truthjet3_n ] = this_pt;
      b_truthjet3_eta[ b_truthjet3_n ] = this_eta;
      b_truthjet3_phi[ b_truthjet3_n ] = this_phi;
      //cout << " truth R=0.3 jet # " << b_truthjet3_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet3_n++;
    } // loop over R = 0.3 truth jets

  JetMap* truth4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r04");
  //cout << "R = 0.4 truth jets has size " << truth4_jets->size() << endl;
  for (JetMap::Iter iter = truth4_jets->begin(); iter != truth4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet4_e[ b_truthjet4_n ] = this_e;
      b_truthjet4_pt[ b_truthjet4_n ] = this_pt;
      b_truthjet4_eta[ b_truthjet4_n ] = this_eta;
      b_truthjet4_phi[ b_truthjet4_n ] = this_phi;
      //cout << " truth R=0.4 jet # " << b_truthjet4_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet4_n++;
    } // loop over R = 0.4 truth jets

  JetMap* truth5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Truth_r05");
  //cout << "R = 0.5 truth jets has size " << truth5_jets->size() << endl;
  for (JetMap::Iter iter = truth5_jets->begin(); iter != truth5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_truthjet5_e[ b_truthjet5_n ] = this_e;
      b_truthjet5_pt[ b_truthjet5_n ] = this_pt;
      b_truthjet5_eta[ b_truthjet5_n ] = this_eta;
      b_truthjet5_phi[ b_truthjet5_n ] = this_phi;
      //cout << " truth R=0.5 jet # " << b_truthjet5_n << " pt / eta / phi / m = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_truthjet5_n++;
    } // loop over R = 0.5 truth jets



  // ------------------------------------------------------------------------------
  // --- get jet seed information
  // ------------------------------------------------------------------------------

  // JetMap* seedraw_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsRaw_r02");
  // //cout << "reco jets R=0.2 has size " << seedraw_jets->size() << endl;
  // for (JetMap::Iter iter = seedraw_jets->begin(); iter != seedraw_jets->end(); ++iter)
  //   {
  //     Jet* this_jet = iter->second;
  //     float this_pt = this_jet->get_pt();
  //     float this_phi = this_jet->get_phi();
  //     float this_eta = this_jet->get_eta();
  //     if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
  //     //cout << " SeedRaw R=0.2 jet #" << b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
  //   }

  // JetMap* seedsub_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_HIRecoSeedsSub_r02");
  // //cout << "reco jets R=0.2 has size " << seedsub_jets->size() << endl;
  // for (JetMap::Iter iter = seedsub_jets->begin(); iter != seedsub_jets->end(); ++iter)
  //   {
  //     Jet* this_jet = iter->second;
  //     float this_pt = this_jet->get_pt();
  //     float this_phi = this_jet->get_phi();
  //     float this_eta = this_jet->get_eta();
  //     b_jet2seed_pt[ b_jet2seed_n ] = this_pt;
  //     b_jet2seed_eta[ b_jet2seed_n ] = this_eta;
  //     b_jet2seed_phi[ b_jet2seed_n ] = this_phi;
  //     if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
  //     //cout << " SeedSub R=0.2 jet #" << b_jet2seed_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
  //     b_jet2seed_n++;
  //   }



  // ------------------------------------------------------------------------------
  // --- get reconstructed jet information (no subtraction)
  // ------------------------------------------------------------------------------

  JetMap* nsreco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02");
  //cout << "reco jets R=0.2 has size " << nsreco2_jets->size() << endl;
  for (JetMap::Iter iter = nsreco2_jets->begin(); iter != nsreco2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet2_e[ b_jet2_n ] = this_e;
      b_jet2_pt[ b_jet2_n ] = this_pt;
      b_jet2_eta[ b_jet2_n ] = this_eta;
      b_jet2_phi[ b_jet2_n ] = this_phi;
      //cout << " pp reco R=0.2 jet #" << b_jet2_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2_n++;
    } // loop over R=0.2 jets

  JetMap* nsreco3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03");
  //cout << "reco jets R=0.3 has size " << nsreco3_jets->size() << endl;
  for (JetMap::Iter iter = nsreco3_jets->begin(); iter != nsreco3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet3_e[ b_jet3_n ] = this_e;
      b_jet3_pt[ b_jet3_n ] = this_pt;
      b_jet3_eta[ b_jet3_n ] = this_eta;
      b_jet3_phi[ b_jet3_n ] = this_phi;
      //cout << " pp reco R=0.3 jet #" << b_jet3_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet3_n++;
    } // loop over R=0.3 jets

  JetMap* nsreco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04");
  //cout << "reco jets R=0.4 has size " << nsreco4_jets->size() << endl;
  for (JetMap::Iter iter = nsreco4_jets->begin(); iter != nsreco4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet4_e[ b_jet4_n ] = this_e;
      b_jet4_pt[ b_jet4_n ] = this_pt;
      b_jet4_eta[ b_jet4_n ] = this_eta;
      b_jet4_phi[ b_jet4_n ] = this_phi;
      //cout << " pp reco R=0.4 jet #" << b_jet4_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet4_n++;
    } // loop over R=0.4 jets

  JetMap* nsreco5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05");
  //cout << "reco jets R=0.5 has size " << nsreco5_jets->size() << endl;
  for (JetMap::Iter iter = nsreco5_jets->begin(); iter != nsreco5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 10 || fabs(this_eta) > 5) continue; // stricter pt cut for unsubtracted
      b_jet5_e[ b_jet5_n ] = this_e;
      b_jet5_pt[ b_jet5_n ] = this_pt;
      b_jet5_eta[ b_jet5_n ] = this_eta;
      b_jet5_phi[ b_jet5_n ] = this_phi;
      //cout << " pp reco R=0.2 jet #" << b_jet5_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet5_n++;
    } // loop over R=0.5 jets



  // ------------------------------------------------------------------------------
  // --- get reconstructed jet information (with subtraction)
  // ------------------------------------------------------------------------------

  JetMap* reco2_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r02_Sub1");
  //cout << "reco jets R=0.4 has size " << reco2_jets->size() << endl;
  for (JetMap::Iter iter = reco2_jets->begin(); iter != reco2_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet2sub_e[ b_jet2sub_n ] = this_e;
      b_jet2sub_pt[ b_jet2sub_n ] = this_pt;
      b_jet2sub_eta[ b_jet2sub_n ] = this_eta;
      b_jet2sub_phi[ b_jet2sub_n ] = this_phi;
      //cout << " reco R=0.2 jet (Sub1) #" << b_jet2sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet2sub_n++;
    } // loop over R=0.2 reco jets

  JetMap* reco3_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r03_Sub1");
  //cout << "reco jets R=0.4 has size " << reco3_jets->size() << endl;
  for (JetMap::Iter iter = reco3_jets->begin(); iter != reco3_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet3sub_e[ b_jet3sub_n ] = this_e;
      b_jet3sub_pt[ b_jet3sub_n ] = this_pt;
      b_jet3sub_eta[ b_jet3sub_n ] = this_eta;
      b_jet3sub_phi[ b_jet3sub_n ] = this_phi;
      //cout << " reco R=0.3 jet (Sub1) #" << b_jet3sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet3sub_n++;
    } // loop over R=0.3 reco jets

  JetMap* reco4_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r04_Sub1");
  //cout << "reco jets R=0.4 has size " << reco4_jets->size() << endl;
  for (JetMap::Iter iter = reco4_jets->begin(); iter != reco4_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet4sub_e[ b_jet4sub_n ] = this_e;
      b_jet4sub_pt[ b_jet4sub_n ] = this_pt;
      b_jet4sub_eta[ b_jet4sub_n ] = this_eta;
      b_jet4sub_phi[ b_jet4sub_n ] = this_phi;
      //cout << " reco R=0.4 jet (Sub1) #" << b_jet4sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet4sub_n++;
    } // loop over R=0.4 reco jets

  JetMap* reco5_jets = findNode::getClass<JetMap>(topNode,"AntiKt_Tower_r05_Sub1");
  //cout << "reco jets R=0.4 has size " << reco4_jets->size() << endl;
  for (JetMap::Iter iter = reco5_jets->begin(); iter != reco5_jets->end(); ++iter)
    {
      Jet* this_jet = iter->second;
      float this_e = this_jet->get_e();
      float this_pt = this_jet->get_pt();
      float this_phi = this_jet->get_phi();
      float this_eta = this_jet->get_eta();
      if (this_jet->get_pt() < 5 || fabs(this_eta) > 5) continue;
      b_jet5sub_e[ b_jet5sub_n ] = this_e;
      b_jet5sub_pt[ b_jet5sub_n ] = this_pt;
      b_jet5sub_eta[ b_jet5sub_n ] = this_eta;
      b_jet5sub_phi[ b_jet5sub_n ] = this_phi;
      //cout << " reco R=0.5 jet (Sub1) #" << b_jet5sub_n << ", pt / eta / phi = " << this_pt << " / " << this_eta << " / " << this_phi << endl;
      b_jet5sub_n++;
    } // loop over R=0.5 jets



  b_cluster_n = 0; // used only here

  b_particle_n = 0; // used only here





  // -----------------------------------------------------------------------------------------------------
  // --- tower background part
  // -----------------------------------------------------------------------------------------------------



  TowerBackground* towerbackground1 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub1");
  //cout << "TowerBackground_Sub1" << endl;
  bbkg_n = 0;
  for (int layer = 0; layer < 3; layer++)
  {
    for (unsigned int n = 0; n < towerbackground1->get_UE( layer ).size(); n++)
    {
      bbkg_layer[ bbkg_n ] = layer;
      bbkg_eta[ bbkg_n ] = n;
      bbkg_e[ bbkg_n ] =  towerbackground1->get_UE( layer ).at( n );
      bbkg_n++;
    } // loop over size of tower background
  } // loop over calorimeter layers



  TowerBackground* towerbackground2 = findNode::getClass<TowerBackground>(topNode,"TowerBackground_Sub2");
  //cout << "TowerBackground_Sub2" << endl;
  bbkg2_n = 0;
  for (int layer = 0; layer < 3; layer++)
  {
    for (unsigned int n = 0; n < towerbackground2->get_UE( layer ).size(); n++)
    {
      bbkg2_layer[ bbkg2_n ] = layer;
      bbkg2_eta[ bbkg2_n ] = n;
      bbkg2_e[ bbkg2_n ] =  towerbackground2->get_UE( layer ).at( n );
      bbkg2_n++;
    } // loop over size of tower background
  } // loop over calorimeter layers



  // -----------------------------------------------------------------------------------------------------
  // --- all done!  fill the tree and move on to the next event
  // -----------------------------------------------------------------------------------------------------

  tree->Fill();

  return 0;
}



int TreeMaker::End(PHCompositeNode *topNode)
{
  outfile->Write();
  outfile->Close();
  cout << "All done!  Have a nice day!" << endl;
  return 0;
}

