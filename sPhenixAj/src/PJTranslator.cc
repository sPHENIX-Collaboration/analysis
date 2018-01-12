//----------------------------------------------------------
// Module for flow jet reconstruction in sPHENIX
// J. Orjuela-Koop
// May 5 2015
//----------------------------------------------------------

#include"PJTranslator.h"
#include"PseudoJetPlus.h"
//#include <fastjet/PseudoJet.hh>

#include<calobase/RawCluster.h>
#include<calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>

#include<phool/PHCompositeNode.h>
#include<phool/PHNodeIterator.h>
#include<phool/PHNodeReset.h>
#include<fun4all/getClass.h>

// #include <fastjet/JetDefinition.hh>
// #include <fastjet/ClusterSequence.hh>
// #include <fastjet/SISConePlugin.hh>

#include <TF1.h>
// #include <TFile.h>
// #include <TH1.h>
// #include <TH2.h>
// #include <TLorentzVector.h>
// #include <TNtuple.h>

// #include <cmath>
using namespace std;

/*
 * Constructor
 */
PJTranslator::PJTranslator(const std::string &name):
  SubsysReco(name)
{
  //Define tolerance limits for track-cluster matching
  match_tolerance_low = new TF1("match_tolerance_low","pol4");
  match_tolerance_low->SetParameter(0,-0.470354);
  match_tolerance_low->SetParameter(1,0.928888);
  match_tolerance_low->SetParameter(2,-0.0958367);
  match_tolerance_low->SetParameter(3,0.00748122);
  match_tolerance_low->SetParameter(4,-0.000177858);
  
  match_tolerance_high = new TF1("match_tolerance_high","pol2");
  match_tolerance_high->SetParameter(0,0.457184);
  match_tolerance_high->SetParameter(1,1.24821);
  match_tolerance_high->SetParameter(2,-0.00848157);
}

/*
 * Empty destructor
 */
PJTranslator::~PJTranslator()
{
}

/*
 * Initialize module
 */
int PJTranslator::Init(PHCompositeNode* topNode)
{
  cout << "------PJTranslator::Init(PHCompositeNode*)------" << endl;
  PHNodeIterator iter(topNode);

  /// \TODO: Create your own composite node, then attach to it.

  // // Looking for the DST node
  // PHCompositeNode *dstNode 
  //   = static_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode","DST"));
  // if (!dstNode) {
  //   cout << PHWHERE << "DST Node missing, doing nothing." << endl;
  //   return Fun4AllReturnCodes::ABORTRUN;
  // }
    
  particles = new PJContainer;

  // Create the PJ node if required
  PHDataNode< PJContainer >* pjNode 
    = dynamic_cast< PHDataNode<PJContainer>* >(iter.findFirst("PHDataNode","PJ"));
  if (!pjNode) {
    pjNode = new PHDataNode< PJContainer >(particles, "PJ");
    topNode->addNode(pjNode);
  }
  
  return 0;
}

/*
 * Process event
 */
int PJTranslator::process_event(PHCompositeNode* topNode)
{
  cout << "------PJTranslator::process_event(PHCompositeNode*)------" << endl;
  
  if ( particles ) {
    particles->data.clear();
  } else {
    cerr << " This should never happen." << endl;
    return Fun4AllReturnCodes::ABORTRUN;

  }

  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vertexmap) {

    cout <<"PJTranslator::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex."<<endl;
    assert(vertexmap); // force quit

    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (vertexmap->empty()) {
    cout <<"PJTranslator::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_bbc or tracking reco flags in the main macro in order to reconstruct the global vertex."<<endl;
    return Fun4AllReturnCodes::ABORTRUN;
  }

  GlobalVertex* vtx = vertexmap->begin()->second;
  CLHEP::Hep3Vector vertex ;
  if (vtx) vertex . set(vtx->get_x(),vtx->get_y(),vtx->get_z());


  //  vector<fastjet::PseudoJet> particles;
  //-------------------------------------------------
  // Gather constituents
  //-------------------------------------------------    

  //Loop over EMCAL clusters 
  //------------------------
 RawClusterContainer *emc_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  for(unsigned int i=0; i<emc_clusters->size(); i++) {
    RawCluster* part = emc_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy()/sfEMCAL;
//    double eT = energy/cosh(eta);
//    double pz = eT*sinh(eta);

    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);
    E_vec_cluster = E_vec_cluster/sfEMCAL;
    double eT = E_vec_cluster.perp();
    double pz = E_vec_cluster.z();

     
    if(eT<0.000001) {
	eT = 0.001;
	pz = eT*sinh(eta);
	energy = sqrt(eT*eT + pz*pz);
      }
    /// \TODO For EMCAL jets
    /// \TODO - Add kinematic cuts, add quality cuts, - add corrections 
    
    particles->data.push_back( fastjet::PseudoJet(eT*cos(phi),eT*sin(phi),pz,energy) ); 
  }
  

  //Loop over HCALIN clusters 
  //-------------------------
  RawClusterContainer *hci_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALIN");
  for(unsigned int i=0; i<hci_clusters->size(); i++) {
    RawCluster* part = hci_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy()/sfHCALIN;
//    double eT = energy/cosh(eta);
//    double pz = eT*sinh(eta);

    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);
    E_vec_cluster = E_vec_cluster/sfHCALIN;
    double eT = E_vec_cluster.perp();
    double pz = E_vec_cluster.z();
    
    if(eT<0.000001)  {
      eT = 0.001;
      pz = eT*sinh(eta);
      energy = sqrt(eT*eT + pz*pz);
    }
      
    /// \TODO For HCALIN jets
    /// \TODO - Add kinematic cuts, add quality cuts, - add corrections 
    particles->data.push_back( fastjet::PseudoJet(eT*cos(phi),eT*sin(phi),pz,energy) );
  }

  //Loop over HCALOUT clusters 
  //-------------------------
  RawClusterContainer *hco_clusters = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_HCALOUT");
  for(unsigned int i=0; i<hco_clusters->size(); i++) {
    RawCluster* part = hco_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy()/sfHCALOUT;
//    double eT = energy/cosh(eta);
//    double pz = eT*sinh(eta);

    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);
    E_vec_cluster = E_vec_cluster/sfHCALOUT;
    double eT = E_vec_cluster.perp();
    double pz = E_vec_cluster.z();
    
    if(eT<0.000001) {
      eT = 0.001;
      pz = eT*sinh(eta);
      energy = sqrt(eT*eT + pz*pz);
    }
    
    /// \TODO For HCALOUT jets
    /// \TODO - Add kinematic cuts, add quality cuts, - add corrections 
    particles->data.push_back( fastjet::PseudoJet(eT*cos(phi),eT*sin(phi),pz,energy) );
  }

  //Get reconstructed tracks from nodetree
  // -------------------------------------
  //  const float M = 0.139; //Assume pion mass
  const float M = 0;

  SvtxTrackMap* reco_tracks = findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  for(SvtxTrackMap::Iter iter = reco_tracks->begin(); iter != reco_tracks->end(); ++iter) {
    SvtxTrack *trk = &iter->second;
    double px = trk->get3Momentum(0);
    double py = trk->get3Momentum(1);
    double pz = trk->get3Momentum(2);
    // double pt = sqrt(px*px + py*py);
    double p = sqrt(px*px + py*py + pz*pz);
    double track_energy = sqrt(p*p + M*M); //Assume pion mass
    double phi = atan2(py,px);
    double eta = -log(tan(acos(pz/p)/2.0));
    double et = track_energy/cosh(eta);

    //Account for angle wrap-around
    while( phi < 0 )       {  phi += 2.0*M_PI; }
    while( phi >= 2*M_PI ) {  phi -= 2.0*M_PI; }

    //Quality cut on tracks
    if(trk->getQuality() > 3.0) continue;

    // //Find ID of clusters that match to track in each layer
    // int emcID = -1;
    // int hciID = -1;
    // int hcoID = -1;
    // emcID = (int)trk->get_cal_cluster_id(SvtxTrack::CEMC);
    // hciID = (int)trk->get_cal_cluster_id(SvtxTrack::HCALIN);
    // hcoID = (int)trk->get_cal_cluster_id(SvtxTrack::HCALOUT);

    // //Find energy deposited by track in each layer
    // tlvmap::iterator it = emc_map.find(emcID);
    // if(it != emc_map.end()) {
    //   emc_energy = emc_map[emcID]->Energy();
    // }
      
    // it = hci_map.find(hciID);
    // if(it != hci_map.end()) {
    //   hci_energy = hci_map[hciID]->Energy();
    // }
      
    // it = hco_map.find(hcoID);
    // if(it != hco_map.end()) {
    //   hco_energy = hco_map[hcoID]->Energy();
    // }

    // cluster_energy = emc_energy + hci_energy + hco_energy;

    // //Does the track match the cluster to within tolerance?
    // //  *matched = 0 --> clus_energy < track_energy
    // //  *matched = 1 --> clus_energy > track_energy
    // //  *matched = 2 --> clus_energy = track_energy 
    // int matched = -1;
    // matched = get_matched(cluster_energy,track_energy);
    
    // //If matched = 1, remove track energy from clusters
    // if(matched == 1) {
    //   float fracEnergyEMC = emc_energy/cluster_energy;
    //   float fracEnergyHCI = hci_energy/cluster_energy;
    //   float fracEnergyHCO = hco_energy/cluster_energy;
      
    //   it = emc_map.find(emcID);
    //   if(it!=emc_map.end()) {
    // 	(emc_map.find(emcID)->second)->SetE(emc_energy - fracEnergyEMC*track_energy);
    //   }
	
    //   it = hci_map.find(hciID);
    //   if(it!=hci_map.end()) {
    // 	(hci_map.find(hciID)->second)->SetE(hci_energy - fracEnergyHCI*track_energy);
    //   }
	  
    //   it = hco_map.find(hcoID);
    //   if(it!=hco_map.end()) {
    // 	(hco_map.find(hcoID)->second)->SetE(hco_energy - fracEnergyHCO*track_energy);
    //   }
    // } else if(matched == 2) {
    // 	it = emc_map.find(emcID);
    // 	if(it!=emc_map.end()) {
    // 	  delete emc_map[emcID];
    // 	  emc_map.erase(emcID);
    // 	}
	
    // 	it = hci_map.find(hciID);
    // 	if(it!=emc_map.end()) {
    // 	  delete hci_map[hciID];
    // 	  hci_map.erase(hciID);
    // 	}
	
    // 	it = hco_map.find(hcoID);
    // 	if(it!=hco_map.end()) {
    // 	  delete hco_map[hcoID];
    // 	  hco_map.erase(hcoID);
    // 	}
    // } else if(matched == 0) {
    //   continue;
    // }

    // //Add perfectly matched and partially matched tracks to flow particle container
    // if(et<0.000001) {
    //   et = 0.001;
    //   pz = et*sinh(eta);
    //   track_energy = sqrt(et*et + pz*pz);
    // }
    
    particles->data.push_back( fastjet::PseudoJet (et*cos(phi),et*sin(phi),pz,track_energy) );
  }
  

  cout << "EMC:     " << emc_clusters->size() << endl;
  cout << "HCO:     " << hco_clusters->size() << endl;
  cout << "HCI:     " << hci_clusters->size() << endl;
  cout << "Tracker: " << reco_tracks->size() << endl;
  cout << particles->data.size() << endl;  


  return 0;
}

/*
 * Given a track and cluster energies, determine if they match within tolerance
 */
int PJTranslator::get_matched(double clus_energy, double track_energy)
{
  double limLo = match_tolerance_low->Eval(track_energy);
  double limHi = match_tolerance_high->Eval(track_energy);

  int matched = -1;

  if(clus_energy < limLo)
    {
      //Track energy greater than cluster energy. Most likely a fake
      matched = 0;
    }
  else if(clus_energy > limHi)
    {
      //Contaminated cluster
      matched = 1;
    }
  else
    {
      //Track and cluster match to within reason
      matched = 2;
    }

  return matched;
}

/*
 * Write jets to node tree
 */
/*
todo - we are working on a jet object
void PHAJMaker::save_jets_to_nodetree()
{
  flow_jet_container = new PHPyJetContainerV2();
}
*/
