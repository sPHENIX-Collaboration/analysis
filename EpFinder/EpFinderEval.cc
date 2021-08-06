
#include "EpFinderReco.h"

#include <phool/phool.h>
#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <fun4all/Fun4AllServer.h>

#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNode.h>                // for PHNode

#include <calobase/RawTowerDefs.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTower.h>
#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <phhepmc/PHHepMCGenEventMap.h>
#include <phhepmc/PHHepMCGenEvent.h>
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/HeavyIon.h>               

#include <TTree.h>
#include <TH2D.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TMath.h>

#include <iostream>
#include <utility>

#define TOWER_E_CUT 0.0

#include "EpFinder.h"

using namespace std;

double XYtoPhi(double x, double y)
{
  // -Pi to +Pi
  Double_t phi = atan2(y,x);
  if(phi<-TMath::Pi()) phi += TMath::TwoPi();
  if(phi>=TMath::Pi()) phi -= TMath::TwoPi();
  return phi;
}

double XYtoPhi_02PI(double x, double y)
{
  // 0 to 2pi
  Double_t phi = atan2(y,x);
  if(phi<0.0) phi += TMath::TwoPi();
  if(phi>=TMath::TwoPi()) phi -= TMath::TwoPi();
  return phi;
}

  double getEta(double pt, double pz){
  float theta = XYtoPhi(pz,pt);
  float eta = -log(tan(theta/2.0));
  return eta; 
} 

double DeltaPhi(double phi1, double phi2){
  Double_t dphi;
  dphi = phi1 - phi2;
  if(dphi<-TMath::Pi()) dphi += TMath::TwoPi();
  if(dphi>=TMath::Pi()) dphi -= TMath::TwoPi();
  return dphi;
}

//----------------------------------------------------------------------------//
//-- Constructor:
//--  simple initialization
//----------------------------------------------------------------------------//

EpFinderReco::EpFinderReco(const string &name) :
  SubsysReco(name)
  , _algonode("EVENT_PLANE")
{
  //initialize
  prim_EpFinder = NULL;
  fprim_EpFinder = NULL;
}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int EpFinderReco::Init(PHCompositeNode *topNode) {


  // binning (phi,eta): cemc (96,256)
  CEMC_EpFinder = new EpFinder(1,"CEMC_EpFinderCorrectionHistograms_OUTPUT.root", "CEMC_EpFinderCorrectionHistograms_INPUT.root", 96, 256); 
  CEMC_EpFinder->SetnMipThreshold(0.0); 
  CEMC_EpFinder->SetMaxTileWeight(100.0); 
  cout << CEMC_EpFinder->Report() << endl; 

  // binning (phi,eta): cemc (96,256) +  hcalout (24,64) + hcalin (24,64)
  CEMC_HCAL_EpFinder = new EpFinder(1,"CEMC_HCAL_EpFinderCorrectionHistograms_OUTPUT.root", "CEMC_HCAL_EpFinderCorrectionHistograms_INPUT.root", 96+24+24, 256+64+64); 
  CEMC_HCAL_EpFinder->SetnMipThreshold(0.0); 
  CEMC_HCAL_EpFinder->SetMaxTileWeight(100.0); 
  cout << CEMC_HCAL_EpFinder->Report() << endl; 

  prim_EpFinder = new EpFinder(1, "prim_EpFinderCorrectionHistograms_OUTPUT.root", "prim_EpFinderCorrectionHistograms_INPUT.root"); 
  cout << prim_EpFinder->Report() << endl; 

  fprim_EpFinder = new EpFinder(1, "fprim_EpFinderCorrectionHistograms_OUTPUT.root", "fprim_EpFinderCorrectionHistograms_INPUT.root",
				FPRIM_PHI_BINS, FPRIM_ETA_BINS); 
  cout << fprim_EpFinder->Report() << endl; 


  return CreateNodes(topNode);
}

int EpFinderReco::InitRun(PHCompositeNode *topNode) {


	return Fun4AllReturnCodes::EVENT_OK;

}

int EpFinderReco::CreateNodes(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  // Looking for the DST node
  PHCompositeNode *dstNode = static_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cout << PHWHERE << "DST Node missing, doing nothing." << endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  // Create the algorithm node if required
  PHCompositeNode *AlgoNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode",  _algonode.c_str()));
  if (!AlgoNode)
  {
    AlgoNode = new PHCompositeNode( _algonode.c_str());
    dstNode->addNode(AlgoNode);
  }
    
    
  
  // Create the output objects
  EpInfo *prim_EpInfo = new EpInfo(); 
  PHIODataNode<PHObject> *prim_EpInfo_node = new PHIODataNode<PHObject>(prim_EpInfo, "primary_EpInfo", "PHObject");
  AlgoNode->addNode(prim_EpInfo_node);

  EpInfo *fprim_EpInfo = new EpInfo(); 
  PHIODataNode<PHObject> *fprim_EpInfo_node = new PHIODataNode<PHObject>(fprim_EpInfo, "forward_primary_EpInfo", "PHObject");
  AlgoNode->addNode(fprim_EpInfo_node);

  EpInfo *HIJetReco_EpInfo = new EpInfo(); 
  PHIODataNode<PHObject> *HIJetReco_EpInfo_node = new PHIODataNode<PHObject>(HIJetReco_EpInfo, "HIJetReco_EpInfo", "PHObject");
  AlgoNode->addNode(HIJetReco_EpInfo_node);

  EpInfo *CEMC_EpInfo = new EpInfo();
  PHIODataNode<PHObject> *CEMC_EpInfo_node = new PHIODataNode<PHObject>(CEMC_EpInfo,"CEMC_EpInfo", "PHObject");
  AlgoNode->addNode(CEMC_EpInfo_node);
  
  EpInfo *CEMCHCAL_EpInfo = new EpInfo();
  PHIODataNode<PHObject> *CEMCHCAL_EpInfo_node = new PHIODataNode<PHObject>(CEMCHCAL_EpInfo,"CEMCHCAL_EpInfo", "PHObject");
  AlgoNode->addNode(CEMCHCAL_EpInfo_node);

  return Fun4AllReturnCodes::EVENT_OK;
    
}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//

int EpFinderReco::process_event(PHCompositeNode *topNode) {

	GetNodes(topNode);

	GetEventPlanes(topNode);

	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//
int EpFinderReco::EndRun(PHCompositeNode *topNode) {

	return Fun4AllReturnCodes::EVENT_OK;
}

int EpFinderReco::End(PHCompositeNode *topNode) {

	prim_EpFinder->Finish();
	fprim_EpFinder->Finish(); 

	CEMC_EpFinder->Finish(); 
	CEMC_HCAL_EpFinder->Finish(); 
	
	delete prim_EpFinder;
	delete fprim_EpFinder; 
	delete CEMC_EpFinder; 
	delete CEMC_HCAL_EpFinder; 

	return Fun4AllReturnCodes::EVENT_OK;
}

int EpFinderReco::GetPhiBin(float tphi, float numPhiDivisions)
{

  // determine the phi bin

  float sphi = (TMath::TwoPi()/numPhiDivisions);

  if(tphi>=TMath::TwoPi()){
    tphi -= TMath::TwoPi(); 
  }
  else if(tphi<0.0){
    tphi += TMath::TwoPi(); 
  }
       
  return (int)(tphi/sphi); 

}

int EpFinderReco::GetEtaBin(float teta, float eta_low, float eta_high, float numEtaDivisions)
{

  // determine the eta bin

  float seta = fabs((eta_high-eta_low)/numEtaDivisions);
       
  return (int)((teta-eta_low)/seta); 

}


//----------------------------------------------------------------------------//
//-- fill_tree():
//--   Fill the various trees...
//----------------------------------------------------------------------------//

void EpFinderReco::GetEventPlanes(PHCompositeNode *topNode) {
	  
  // Read the detector geometry and set up the arrays for phi weighting.

  static bool first = true;

  if(first){

    for(int i=0; i<PHI_BINS; i++){
      phi_list[i].clear(); 
    }

    for(int i=0; i<FPRIM_PHI_BINS; i++){
      fprim_phi_list[i].clear(); 
    }

    for(int i=0; i<256; i++){
      cemc_phi_list[i].clear(); 
    }

    for(int i=0; i<64; i++){
      hcalout_phi_list[i].clear(); 
      hcalin_phi_list[i].clear(); 
    }


    // For the forward primary weighting, all the eta bins are at the same phi

    for(int i=0; i<FPRIM_PHI_BINS; i++){
      for(int j=0; j<FPRIM_ETA_BINS; j++){
        std::pair<int,int> newPair(i,j); 
	fprim_phi_list[i].push_back(newPair); 	
      }
    }

    // For central barrel the phi bin segmentation is by detector element

    // CEMC

    for (int twr_k = 0; twr_k< 256; twr_k++){
      for (int twr_j = 0; twr_j< 96; twr_j++){
	std::pair<int,int> newPair(twr_j,twr_k); 
	cemc_phi_list[twr_k].push_back(newPair); 
      }
    }



    // oHCAL 

    for (int twr_k = 0; twr_k< 64; twr_k++){
      for (int twr_j = 0; twr_j< 24; twr_j++){
	std::pair<int,int> newPair(twr_j+96,twr_k+256); 
	hcalout_phi_list[twr_k].push_back(newPair); 
      }
    }
      

    // iHCAL 

    for (int twr_k = 0; twr_k< 64; twr_k++){
      for (int twr_j = 0; twr_j< 24; twr_j++){
	std::pair<int,int> newPair(twr_j+96+24,twr_k+256+64); 
	hcalin_phi_list[twr_k].push_back(newPair); 
      }
    }


    first = false; 
  }

  // -------------------------------------
  // Primary Particle Event Plane Finder
  // -------------------------------------

  std::vector<EpHit> phits; 
  phits.clear(); 

  std::vector<EpHit> fphits; 
  fphits.clear(); 
  
  PHG4TruthInfoContainer::ConstRange range = _truth_container->GetPrimaryParticleRange();

  for (PHG4TruthInfoContainer::ConstIterator truth_itr = range.first;
       truth_itr != range.second; ++truth_itr) {

    PHG4Particle* g4particle = truth_itr->second;
    if(!g4particle) {
      continue;
    }

    // remove some particles (muons, taus, neutrinos)...
    // 12 == nu_e
    // 13 == muons
    // 14 == nu_mu
    // 15 == taus
    // 16 == nu_tau
    if ((abs(g4particle->get_pid()) >= 12) && (abs( g4particle->get_pid()) <= 16)) continue;
    
    // acceptance... _etamin,_etamax
    if ((g4particle->get_px() == 0.0) && (g4particle->get_px() == 0.0)) continue; // avoid pt=0
    
    TVector3 partMom(g4particle->get_px(),g4particle->get_py(),g4particle->get_pz()); 
    if ( (partMom.Eta() >= -1.0) &&
	 (partMom.Eta() <=  1.0)) {

      EpHit newHit; 

      newHit.nMip = 1; 
      newHit.phi = partMom.Phi(); 
      newHit.samePhi = NULL;  

      phits.push_back(newHit); 
    }

    if ( (partMom.Eta() >= 1.4) &&
	 (partMom.Eta() < 4.0)) {

      EpHit newHit; 

      newHit.nMip = 1; 
      newHit.phi = partMom.Phi();
      newHit.ix = GetPhiBin(partMom.Phi(), FPRIM_PHI_BINS); 
      newHit.iy = GetEtaBin(partMom.Eta(), 1.4, 4.0, FPRIM_ETA_BINS);      
      newHit.samePhi = &fprim_phi_list[newHit.ix]; 

      fphits.push_back(newHit); 
    }

  }

  EpInfo prim_EpResult = prim_EpFinder->Results(&phits,0);
  *_prim_EpInfo = prim_EpResult; 

  EpInfo fprim_EpResult = fprim_EpFinder->Results(&fphits,0); 
  *_fprim_EpInfo = fprim_EpResult; 

  phits.clear(); 
  fphits.clear(); 

  
  // ----------------------------------------------
  // Run the CEMC and CEMC+HCAL event plane finders
  // ----------------------------------------------

  //---CEMC INFO---//

  std::vector <EpHit> cemchits;
  cemchits.clear();
  std::vector <EpHit> cemchcalhits;
  cemchcalhits.clear();


  
  RawTowerContainer::ConstRange cemc_begin_end = cemctowers->getTowers();
  RawTowerContainer::ConstIterator cemc_itr = cemc_begin_end.first;
  for(; cemc_itr != cemc_begin_end.second; ++cemc_itr) {
    RawTowerDefs::keytype cemctowerid = cemc_itr->first;
    RawTower *cemcrawtower = cemctowers->getTower(cemctowerid);
    if(cemcrawtower) {

       if(cemcrawtower->get_energy()>TOWER_E_CUT) {
       
         EpHit newHit;

	 RawTowerGeom *cemctgeo = cemctowergeom->get_tower_geometry(cemctowerid);

	 newHit.nMip = cemcrawtower->get_energy();
	 newHit.phi  = cemctgeo->get_phi();
	 newHit.ix = cemctgeo->get_column();  // eta index
	 newHit.iy = cemctgeo->get_row();     // phi index
	 newHit.samePhi = &cemc_phi_list[cemctgeo->get_row()]; 

     cemchits.push_back(newHit);
     cemchcalhits.push_back(newHit);
	
       }
    }
  }

  //---HCALOUT INFO---//
  
  RawTowerContainer::ConstRange hcalo_begin_end = hcalotowers->getTowers();
  RawTowerContainer::ConstIterator hcalo_itr = hcalo_begin_end.first;
  for(; hcalo_itr != hcalo_begin_end.second; ++hcalo_itr) {
    RawTowerDefs::keytype hcalotowerid = hcalo_itr->first;
    RawTower *hcalorawtower = hcalotowers->getTower(hcalotowerid);
    if(hcalorawtower) {
      if(hcalorawtower->get_energy()>TOWER_E_CUT) {
 	
	RawTowerGeom *hcalotgeo = hcalotowergeom->get_tower_geometry(hcalotowerid);

	EpHit newHit;
   
	newHit.nMip = hcalorawtower->get_energy();
	newHit.phi  = hcalotgeo->get_phi();
	newHit.ix = hcalotgeo->get_column() + 96;
	newHit.iy = hcalotgeo->get_row() + 256;
	newHit.samePhi = &hcalin_phi_list[hcalotgeo->get_row()]; 

	cemchcalhits.push_back(newHit);

      }
    }   
  }

  //---HCALIN INFO---//
  
  RawTowerContainer::ConstRange hcali_begin_end = hcalitowers->getTowers();
  RawTowerContainer::ConstIterator hcali_itr = hcali_begin_end.first;
  for(; hcali_itr != hcali_begin_end.second; ++hcali_itr) {
    RawTowerDefs::keytype hcalitowerid = hcali_itr->first;
    RawTower *hcalirawtower = hcalitowers->getTower(hcalitowerid);
    if(hcalirawtower) {
      if(hcalirawtower->get_energy()>TOWER_E_CUT) {
 	
	RawTowerGeom *hcalitgeo = hcalitowergeom->get_tower_geometry(hcalitowerid);

	EpHit newHit;
   
	newHit.nMip = hcalirawtower->get_energy();
	newHit.phi  = hcalitgeo->get_phi();
	newHit.ix = hcalitgeo->get_column() + 96 + 24;
	newHit.iy = hcalitgeo->get_row() + 256 + 64;
	newHit.samePhi = &hcalin_phi_list[hcalitgeo->get_row()]; 

	cemchcalhits.push_back(newHit);

      }
    }   
  }

  EpInfo CEMC_EpResult = CEMC_EpFinder->Results(&cemchits,0);
  *_CEMC_EpInfo = CEMC_EpResult;
  
  EpInfo CEMCHCAL_EpResult = CEMC_HCAL_EpFinder->Results(&cemchcalhits,0);
  *_CEMCHCAL_EpInfo = CEMCHCAL_EpResult;

  cemchits.clear();
  cemchcalhits.clear();

  return;

}

//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
int EpFinderReco::GetNodes(PHCompositeNode * topNode) {

  // EpInfo nodes


  _prim_EpInfo = findNode::getClass<EpInfo>(topNode,"primary_EpInfo");
  if (!_prim_EpInfo) {
    cout << PHWHERE << " _prim_EpInfo node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _fprim_EpInfo = findNode::getClass<EpInfo>(topNode,"forward_primary_EpInfo");
  if (!_fprim_EpInfo) {
    cout << PHWHERE << " _fprim_EpInfo node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  _CEMC_EpInfo = findNode::getClass<EpInfo>(topNode,"CEMC_EpInfo");
  if (!_CEMC_EpInfo) {
    cout << PHWHERE << " _CEMC_EpInfo node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  _CEMCHCAL_EpInfo = findNode::getClass<EpInfo>(topNode,"CEMCHCAL_EpInfo");
  if (!_CEMCHCAL_EpInfo) {
    cout << PHWHERE << " _CEMCHCAL_EpInfo node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  


  //Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
								"G4TruthInfo");
  if (!_truth_container) {
    cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  // Grab the towers

  string cemctowernodename = "TOWER_CALIB_CEMC";
  cemctowers = findNode::getClass<RawTowerContainer>(topNode, cemctowernodename.c_str());
  if (!cemctowers)
   {
      std::cout << PHWHERE << ": Could not find node " << cemctowernodename.c_str() << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT; 
   }

  string hcalotowernodename = "TOWER_CALIB_HCALOUT";
  hcalotowers = findNode::getClass<RawTowerContainer>(topNode, hcalotowernodename.c_str());
  if (!hcalotowers)
   {
      std::cout << PHWHERE << ": Could not find node " << hcalotowernodename.c_str() << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT; 
   }
  
  string hcalitowernodename = "TOWER_CALIB_HCALIN";
  hcalitowers = findNode::getClass<RawTowerContainer>(topNode, hcalitowernodename.c_str());
  if (!hcalitowers)
   {
      std::cout << PHWHERE << ": Could not find node " << hcalitowernodename.c_str() << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT; 
   }

    
  // Grab the geometry
  string cemctowergeomnodename = "TOWERGEOM_CEMC";
  cemctowergeom = findNode::getClass<RawTowerGeomContainer>(topNode, cemctowergeomnodename.c_str());
  if (! cemctowergeom)
  {
    cout << PHWHERE << ": Could not find node " << cemctowergeomnodename.c_str() << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  string hcalotowergeomnodename = "TOWERGEOM_HCALOUT";
  hcalotowergeom = findNode::getClass<RawTowerGeomContainer>(topNode, hcalotowergeomnodename.c_str());
  if (! hcalotowergeom)
  {
    cout << PHWHERE << ": Could not find node " << hcalotowergeomnodename.c_str() << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  
  string hcalitowergeomnodename = "TOWERGEOM_HCALIN";
  hcalitowergeom = findNode::getClass<RawTowerGeomContainer>(topNode, hcalitowergeomnodename.c_str());
  if (! hcalitowergeom)
  {
    cout << PHWHERE << ": Could not find node " << hcalitowergeomnodename.c_str() << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

