
#include "EpFinderEval.h"

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

EpFinderEval::EpFinderEval(const string &name) :
  SubsysReco(name), _eval_tree_event(NULL) {
	//initialize
	_event = 0;
	_outfile_name = "EpFinder_Eval.root";
	RpFinder = NULL; 
	RpFinderL = NULL; 
	RpFinderR = NULL; 
	primRpFinder = NULL; 
	fprimRpFinder = NULL; 

}

//----------------------------------------------------------------------------//
//-- Init():
//--   Intialize all histograms, trees, and ntuples
//----------------------------------------------------------------------------//
int EpFinderEval::Init(PHCompositeNode *topNode) {

	cout << PHWHERE << " Opening file " << _outfile_name << endl;
	PHTFileServer::get().open(_outfile_name, "RECREATE");

	_eval_tree_event = new TTree("event", "FastSim Eval => event parameters");
	_eval_tree_event->Branch("event", &_event, "_event/I");
	_eval_tree_event->Branch("rplane_angle", &rplane_angle, "rplane_angle/F");
	_eval_tree_event->Branch("bimpact", &bimpact, "bimpact/F");
	_eval_tree_event->Branch("prim_rplane_angle", &prim_rplane_angle, "prim_rplane_angle/F");

	_eval_tree_event->Branch("fprim_rplane_angle", &fprim_rplane_angle, "fprim_rplane_angle/F");
	_eval_tree_event->Branch("fprim_phiweighted_rplane_angle", &fprim_phiweighted_rplane_angle, "fprim_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("fprim_phiweightedandshifted_rplane_angle", &fprim_phiweightedandshifted_rplane_angle, "fprim_phiweightedandshifted_rplane_angle/F");

	_eval_tree_event->Branch("rfprim_rplane_angle", &rfprim_rplane_angle, "rfprim_rplane_angle/F");
	_eval_tree_event->Branch("rfprim_phiweighted_rplane_angle", &rfprim_phiweighted_rplane_angle, "rfprim_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("rfprim_phiweightedandshifted_rplane_angle", &rfprim_phiweightedandshifted_rplane_angle, "rfprim_phiweightedandshifted_rplane_angle/F");

	_eval_tree_event->Branch("femc_raw_rplane_angle", &femc_raw_rplane_angle, "femc_raw_rplane_angle/F");
	_eval_tree_event->Branch("femc_phiweighted_rplane_angle", &femc_phiweighted_rplane_angle, "femc_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("femc_phiweightedandshifted_rplane_angle", &femc_phiweightedandshifted_rplane_angle, "femc_phiweightedandshifted_rplane_angle/F");

	_eval_tree_event->Branch("rfemc_raw_rplane_angle", &rfemc_raw_rplane_angle, "rfemc_raw_rplane_angle/F");
	_eval_tree_event->Branch("rfemc_phiweighted_rplane_angle", &rfemc_phiweighted_rplane_angle, "rfemc_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("rfemc_phiweightedandshifted_rplane_angle", &rfemc_phiweightedandshifted_rplane_angle, "rfemc_phiweightedandshifted_rplane_angle/F");

	_eval_tree_event->Branch("femcL_raw_rplane_angle", &femcL_raw_rplane_angle, "femcL_raw_rplane_angle/F");
	_eval_tree_event->Branch("femcL_phiweighted_rplane_angle", &femcL_phiweighted_rplane_angle, "femcL_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("femcL_phiweightedandshifted_rplane_angle", &femcL_phiweightedandshifted_rplane_angle, "femcL_phiweightedandshifted_rplane_angle/F");

	_eval_tree_event->Branch("femcR_raw_rplane_angle", &femcR_raw_rplane_angle, "femcR_raw_rplane_angle/F");
	_eval_tree_event->Branch("femcR_phiweighted_rplane_angle", &femcR_phiweighted_rplane_angle, "femcR_phiweighted_rplane_angle/F");
	_eval_tree_event->Branch("femcR_phiweightedandshifted_rplane_angle", &femcR_phiweightedandshifted_rplane_angle, "femcR_phiweightedandshifted_rplane_angle/F");

	RpFinder = new EpFinder(4,"EpFinderCorrectionHistograms_OUTPUT.root", "EpFinderCorrectionHistograms_INPUT.root", 181, 181); 
	RpFinder->SetnMipThreshold(0.0); 
	RpFinder->SetMaxTileWeight(100.0); 
	cout << RpFinder->Report() << endl; 

	rRpFinder = new EpFinder(4,"rEpFinderCorrectionHistograms_OUTPUT.root", "rEpFinderCorrectionHistograms_INPUT.root", 181, 181); 
	rRpFinder->SetnMipThreshold(0.0); 
	rRpFinder->SetMaxTileWeight(100.0); 
	cout << rRpFinder->Report() << endl; 

	RpFinderL = new EpFinder(4, "L_EpFinderCorrectionHistograms_OUTPUT.root", "L_EpFinderCorrectionHistograms_INPUT.root", 181, 181); 
	RpFinderL->SetnMipThreshold(0.0); 
	RpFinderL->SetMaxTileWeight(100.0); 
	cout << RpFinderL->Report() << endl; 

	RpFinderR = new EpFinder(4, "R_EpFinderCorrectionHistograms_OUTPUT.root", "R_EpFinderCorrectionHistograms_INPUT.root", 181, 181); 
	RpFinderR->SetnMipThreshold(0.0); 
	RpFinderR->SetMaxTileWeight(100.0); 
	cout << RpFinderR->Report() << endl; 

	primRpFinder = new EpFinder(1, "primEpFinderCorrectionHistograms_OUTPUT.root", "primEpFinderCorrectionHistograms_INPUT.root"); 
	cout << primRpFinder->Report() << endl; 

	fprimRpFinder = new EpFinder(1, "fprimEpFinderCorrectionHistograms_OUTPUT.root", "fprimEpFinderCorrectionHistograms_INPUT.root",
				     FPRIM_PHI_BINS, FPRIM_ETA_BINS); 
	cout << fprimRpFinder->Report() << endl; 

	rfprimRpFinder = new EpFinder(1, "rfprimEpFinderCorrectionHistograms_OUTPUT.root", "rfprimEpFinderCorrectionHistograms_INPUT.root",
				     FPRIM_PHI_BINS, RFPRIM_ETA_BINS); 
	cout << rfprimRpFinder->Report() << endl; 

	return Fun4AllReturnCodes::EVENT_OK;
}

int EpFinderEval::InitRun(PHCompositeNode *topNode) {


	return Fun4AllReturnCodes::EVENT_OK;

}

//----------------------------------------------------------------------------//
//-- process_event():
//--   Call user instructions for every event.
//--   This function contains the analysis structure.
//----------------------------------------------------------------------------//

int EpFinderEval::process_event(PHCompositeNode *topNode) {
	_event++;

	GetNodes(topNode);

	fill_tree(topNode);

	return Fun4AllReturnCodes::EVENT_OK;
}

//----------------------------------------------------------------------------//
//-- End():
//--   End method, wrap everything up
//----------------------------------------------------------------------------//
int EpFinderEval::End(PHCompositeNode *topNode) {

	PHTFileServer::get().cd(_outfile_name);

	_eval_tree_event->Write();

	RpFinder->Finish(); 
	rRpFinder->Finish(); 
	RpFinderL->Finish(); 
	RpFinderR->Finish(); 
	primRpFinder->Finish(); 
	fprimRpFinder->Finish(); 
	
	delete RpFinder;
	delete rRpFinder;
	delete RpFinderL;
	delete RpFinderR;
	delete primRpFinder;	
	delete fprimRpFinder;

	return Fun4AllReturnCodes::EVENT_OK;
}

int EpFinderEval::GetPhiBin(float tphi, float numPhiDivisions)
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

int EpFinderEval::GetEtaBin(float teta, float eta_low, float eta_high, float numEtaDivisions)
{

  // determine the eta bin

  float seta = fabs((eta_high-eta_low)/numEtaDivisions);
       
  return (int)((teta-eta_low)/seta); 

}


//----------------------------------------------------------------------------//
//-- fill_tree():
//--   Fill the various trees...
//----------------------------------------------------------------------------//

void EpFinderEval::fill_tree(PHCompositeNode *topNode) {
	  
  // Read the FEMC geometry and set up the arrays for phi weighting.

  static bool first = true;

  if(first){

    for(int i=0; i<PHI_BINS; i++){
      phi_list[i].clear(); 
      rphi_list[i].clear(); 
    }

    for(int i=0; i<FPRIM_PHI_BINS; i++){
      fprim_phi_list[i].clear(); 
      rfprim_phi_list[i].clear(); 
    }

    // generate a list of all towers in the same phi range
    // the phi grouping is determined by dividing phi into 
    // PHI_BINS even slices in phi

    RawTowerDefs::CalorimeterId calo_id_ = RawTowerDefs::convert_name_to_caloid( "FEMC" );   

    for (int twr_j = 0; twr_j< 181; twr_j++){
      for (int twr_k = 0; twr_k< 181; twr_k++){
	RawTowerDefs::keytype towerid = RawTowerDefs::encode_towerid( calo_id_, twr_j+500, twr_k+500 ); 
	RawTowerGeom *tgeo = towergeom->get_tower_geometry(towerid); 
	if(tgeo){
	  int idx = GetPhiBin(tgeo->get_phi(), PHI_BINS); 
	  std::pair<int,int> newPair(tgeo->get_column()-500,tgeo->get_row()-500); 
	  phi_list[idx].push_back(newPair); 
	  if(tgeo->get_eta()<2.4) rphi_list[idx].push_back(newPair); 
	}
      }
    }

    // For the forward primary weighting, all the eta bins are at the same phi

    for(int i=0; i<FPRIM_PHI_BINS; i++){
      for(int j=0; j<FPRIM_ETA_BINS; j++){
        std::pair<int,int> newPair(i,j); 
	fprim_phi_list[i].push_back(newPair); 	
      }
      for(int j=0; j<RFPRIM_ETA_BINS; j++){
        std::pair<int,int> newPair(i,j); 
	rfprim_phi_list[i].push_back(newPair); 	
      }
    }

    first = false; 
  }

  // get the event properties
  
  rplane_angle = -9999.0; 

  PHNodeIterator iter(topNode);
  PHHepMCGenEventMap *genevent_map = findNode::getClass<PHHepMCGenEventMap>(topNode,"PHHepMCGenEventMap");
  if(genevent_map){
    // For now just take the first HEPMC event 
    PHHepMCGenEvent *genevent = (genevent_map->begin())->second; 
    if(genevent){
      HepMC::GenEvent *event = genevent->getEvent();
      HepMC::HeavyIon *hi = event->heavy_ion();
      rplane_angle = hi->event_plane_angle();
      bimpact =  hi->impact_parameter(); 
    }
  }

  // -------------------------------------
  // Primary Particle Event Plane Finder
  // -------------------------------------

  std::vector<EpHit> phits; 
  phits.clear(); 

  std::vector<EpHit> fphits; 
  fphits.clear(); 
 
  std::vector<EpHit> rfphits; 
  rfphits.clear(); 
 
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

    if ( (partMom.Eta() >= 1.4) &&
	 (partMom.Eta() < 2.4)) {

      EpHit newHit; 

      newHit.nMip = 1; 
      newHit.phi = partMom.Phi();
      newHit.ix = GetPhiBin(partMom.Phi(), FPRIM_PHI_BINS); 
      newHit.iy = GetEtaBin(partMom.Eta(), 1.4, 2.4, RFPRIM_ETA_BINS);      
      newHit.samePhi = &rfprim_phi_list[newHit.ix]; 

      rfphits.push_back(newHit); 
    }


  }

  EpInfo primRpResult = primRpFinder->Results(&phits,0); 

  prim_rplane_angle = primRpResult.RawPsi(2); 

  EpInfo fprimRpResult = fprimRpFinder->Results(&fphits,0); 

  fprim_rplane_angle = fprimRpResult.RawPsi(2); 
  fprim_phiweighted_rplane_angle = fprimRpResult.PhiWeightedPsi(2); 
  fprim_phiweightedandshifted_rplane_angle = fprimRpResult.PhiWeightedAndShiftedPsi(2); 

  EpInfo rfprimRpResult = rfprimRpFinder->Results(&rfphits,0); 

  rfprim_rplane_angle = rfprimRpResult.RawPsi(2); 
  rfprim_phiweighted_rplane_angle = rfprimRpResult.PhiWeightedPsi(2); 
  rfprim_phiweightedandshifted_rplane_angle = rfprimRpResult.PhiWeightedAndShiftedPsi(2); 

  phits.clear(); 
  fphits.clear(); 
  rfphits.clear(); 

  // --------------------------------
  // Run the FEMC event plane finder
  // --------------------------------

  std::vector<EpHit> hits; 
  hits.clear(); 

  std::vector<EpHit> rhits; 
  rhits.clear(); 

  std::vector<EpHit> hitsL; 
  hitsL.clear(); 

  std::vector<EpHit> hitsR; 
  hitsR.clear(); 

  RawTowerContainer::ConstRange begin_end  = towers->getTowers();
  RawTowerContainer::ConstIterator itr = begin_end.first;
  for (; itr != begin_end.second; ++itr) {
    RawTowerDefs::keytype towerid = itr->first;
    RawTower *rawtower = towers->getTower(towerid);
    if(rawtower) {
      if(rawtower->get_energy()>TOWER_E_CUT) {

	RawTowerGeom *tgeo = towergeom->get_tower_geometry(towerid); 

	EpHit newHit; 

	//newHit.nMip = 1; 
	newHit.nMip = rawtower->get_energy(); 
	newHit.phi = tgeo->get_phi(); 
	newHit.ix = tgeo->get_column() - 500; 
	newHit.iy = tgeo->get_row() - 500;

	int idx = GetPhiBin(tgeo->get_phi(), PHI_BINS); 
	newHit.samePhi = &phi_list[idx]; 

	hits.push_back(newHit); 
	if((idx>=(PHI_BINS/4))&&(idx<(3*PHI_BINS/4))) 
	  hitsL.push_back(newHit); 
	else 
	  hitsR.push_back(newHit);

	if( tgeo->get_eta()<2.4 ) {
	  newHit.samePhi = &rphi_list[idx]; 
	  rhits.push_back(newHit); 
	}
	  
      }
    }

  }

  // Select the event class based on the impact parameter
  
  int ev_class = 0; 
  if((bimpact>=0.0) && (bimpact<4.0))
    ev_class = 0; 
  else if((bimpact>=4.0)&&(bimpact<8.0))
    ev_class = 1; 
  else if((bimpact>=8.0)&&(bimpact<14.0))
    ev_class = 2; 
  else
    ev_class = 3;

  // Run the event plane finder

  EpInfo RpResult = RpFinder->Results(&hits,ev_class); 
  
  femc_raw_rplane_angle = RpResult.RawPsi(2); 
  femc_phiweighted_rplane_angle = RpResult.PhiWeightedPsi(2); 
  femc_phiweightedandshifted_rplane_angle = RpResult.PhiWeightedAndShiftedPsi(2); 

  EpInfo rRpResult = rRpFinder->Results(&rhits,ev_class); 
  
  rfemc_raw_rplane_angle = rRpResult.RawPsi(2); 
  rfemc_phiweighted_rplane_angle = rRpResult.PhiWeightedPsi(2); 
  rfemc_phiweightedandshifted_rplane_angle = rRpResult.PhiWeightedAndShiftedPsi(2); 

  EpInfo RpResultL = RpFinderL->Results(&hitsL,ev_class); 

  femcL_raw_rplane_angle = RpResultL.RawPsi(2); 
  femcL_phiweighted_rplane_angle = RpResultL.PhiWeightedPsi(2); 
  femcL_phiweightedandshifted_rplane_angle = RpResultL.PhiWeightedAndShiftedPsi(2); 

  EpInfo RpResultR = RpFinderR->Results(&hitsR,ev_class); 

  femcR_raw_rplane_angle = RpResultR.RawPsi(2); 
  femcR_phiweighted_rplane_angle = RpResultR.PhiWeightedPsi(2); 
  femcR_phiweightedandshifted_rplane_angle = RpResultR.PhiWeightedAndShiftedPsi(2); 

  _eval_tree_event->Fill();

  hits.clear(); 
  hitsL.clear(); 
  hitsR.clear(); 

  return;

}

//----------------------------------------------------------------------------//
//-- GetNodes():
//--   Get all the all the required nodes off the node tree
//----------------------------------------------------------------------------//
int EpFinderEval::GetNodes(PHCompositeNode * topNode) {

  //Truth container
  _truth_container = findNode::getClass<PHG4TruthInfoContainer>(topNode,
								"G4TruthInfo");
  if (!_truth_container) {
    cout << PHWHERE << " PHG4TruthInfoContainer node not found on node tree"
	 << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  string towernodename = "TOWER_CALIB_FEMC";
  // Grab the towers
  towers = findNode::getClass<RawTowerContainer>(topNode, towernodename.c_str());
  if (!towers)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  // Grab the geometry
  string towergeomnodename = "TOWERGEOM_FEMC";
  towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, towergeomnodename.c_str());
  if (!towergeom)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  return Fun4AllReturnCodes::EVENT_OK;
}

