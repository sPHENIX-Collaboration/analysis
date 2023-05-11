//----------------------------------------------------------
// Module for flow jet reconstruction in sPHENIX
// J. Orjuela-Koop
// May 5 2015
//----------------------------------------------------------

#include <PHFlowJetMaker.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawClusterUtility.h>

#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackMap.h>

#include <phool/PHCompositeNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHNodeReset.h>
#include <phool/PHObject.h>
#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <fastjet/ClusterSequence.hh>
#include <fastjet/JetDefinition.hh>
#include <fastjet/PseudoJet.hh>
#include <fastjet/SISConePlugin.hh>

#include <g4jets/Jet.h>
#include <g4jets/JetMapV1.h>
#include <g4jets/JetV1.h>

#include <TF1.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TNtuple.h>

#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Fun4AllReturnCodes;

typedef std::map<int, TLorentzVector*> tlvmap;

// Jin - this thing is obsolete. Clusters now already have calibrated energies
const float PHFlowJetMaker::sfEMCAL = 0.03;
const float PHFlowJetMaker::sfHCALIN = 0.071;
const float PHFlowJetMaker::sfHCALOUT = 0.04;

/*
 * Constructor
 */
PHFlowJetMaker::PHFlowJetMaker(const std::string& name, const std::string algorithm, double r_param)
  : SubsysReco(name)
{
  flow_jet_map = NULL;
  this->algorithm = algorithm;
  this->r_param = r_param;
  //outfile = outputfile;

  //Define parameters for jet reconstruction
  min_jet_pT = 1.0;
  fastjet::Strategy strategy = fastjet::Best;

  if (algorithm == "AntiKt")
    fJetAlgorithm = new fastjet::JetDefinition(fastjet::antikt_algorithm, r_param, fastjet::E_scheme, strategy);

  if (algorithm == "Kt")
    fJetAlgorithm = new fastjet::JetDefinition(fastjet::kt_algorithm, r_param, fastjet::E_scheme, strategy);

  //Define tolerance limits for track-cluster matching
  match_tolerance_low = new TF1("match_tolerance_low", "pol4");
  match_tolerance_low->SetParameter(0, -0.470354);
  match_tolerance_low->SetParameter(1, 0.928888);
  match_tolerance_low->SetParameter(2, -0.0958367);
  match_tolerance_low->SetParameter(3, 0.00748122);
  match_tolerance_low->SetParameter(4, -0.000177858);

  match_tolerance_high = new TF1("match_tolerance_high", "pol2");
  match_tolerance_high->SetParameter(0, 0.457184);
  match_tolerance_high->SetParameter(1, 1.24821);
  match_tolerance_high->SetParameter(2, -0.00848157);
}

/*
 * Empty destructor
 */
PHFlowJetMaker::~PHFlowJetMaker()
{
}

/*
 * Initialize module
 */
int PHFlowJetMaker::Init(PHCompositeNode* topNode)
{
  cout << "------PHFlowJetMaker::Init(PHCompositeNode*)------" << endl;
  create_node_tree(topNode);

  return EVENT_OK;
}

/*
 * Create node tree for flow jets
 */
int PHFlowJetMaker::create_node_tree(PHCompositeNode* topNode)
{
  PHNodeIterator iter(topNode);

  //Get the DST node
  PHCompositeNode* dstNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
  {
    cout << "DST Node missing. Doing nothing." << endl;
    return ABORTEVENT;
  }

  //Get the JET node
  PHCompositeNode* jetNode = dynamic_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "JETS"));
  if (!jetNode)
  {
    if (algorithm == "AntiKt")
      jetNode = new PHCompositeNode("ANTIKT");

    else if (algorithm == "Kt")
      jetNode = new PHCompositeNode("KT");

    else
      jetNode = new PHCompositeNode(algorithm);

    dstNode->addNode(jetNode);
  }

  PHNodeIterator jiter(jetNode);

  PHCompositeNode* flowJetNode = dynamic_cast<PHCompositeNode*>(jiter.findFirst("PHCompositeNode", "FLOW_JETS"));
  if (!flowJetNode)
  {
    flowJetNode = new PHCompositeNode("FLOW_JETS");
    jetNode->addNode(flowJetNode);
  }

  //Add flow jet node to jet node
  flow_jet_map = new JetMapV1();
  string nodeName = "Flow";

  stringstream snodeName;
  snodeName << algorithm << "_" << nodeName << "_r" << setfill('0') << setw(2) << int(r_param * 10);
  nodeName = snodeName.str();

  PHIODataNode<PHObject>* PHFlowJetNode = new PHIODataNode<PHObject>(flow_jet_map, nodeName.c_str(), "PHObject");

  if (!flowJetNode->addNode(PHFlowJetNode))
  {
    cout << "PHFlowJetMaker::create_node_tree() - Can't add node to tree!" << endl;
  }

  return EVENT_OK;
}

/*
 * Process event
 */
int PHFlowJetMaker::process_event(PHCompositeNode* topNode)
{
  //-------------------------------------------------
  // Get Information from Node Tree
  //-------------------------------------------------

  //Get calorimeter clusters from node tree
  RawClusterContainer* emc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  RawClusterContainer* hci_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALIN");
  RawClusterContainer* hco_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_HCALOUT");

  //Get reconstructed tracks from nodetree
  SvtxTrackMap* reco_tracks = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");

  //Vertex for converting cluster position to momentum direction
  GlobalVertexMap* vertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if (!vertexmap)
  {
    cout << "PHFlowJetMaker::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << endl;
    assert(vertexmap);  // force quit

    return Fun4AllReturnCodes::ABORTRUN;
  }

  if (vertexmap->empty())
  {
    cout << "PHFlowJetMaker::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_bbc or tracking reco flags in the main macro in order to reconstruct the global vertex." << endl;

    return Fun4AllReturnCodes::ABORTRUN;
  }
  GlobalVertex* vtx = vertexmap->begin()->second;
  assert(vtx);

  //-------------------------------------------------
  // Create Jets
  //-------------------------------------------------

  //  //Create jets from raw clusters
  //  vector<fastjet::PseudoJet> raw_cluster_particles;
  //  create_calo_pseudojets(raw_cluster_particles, emc_clusters, hci_clusters, hco_clusters, vtx);
  //  fastjet::ClusterSequence jet_finder_raw(raw_cluster_particles, *fJetAlgorithm);
  //  vector<fastjet::PseudoJet> raw_cluster_jets = jet_finder_raw.inclusive_jets(min_jet_pT);

  //Apply particle flow jets algorithm and create jets from flow particles
  vector<fastjet::PseudoJet> flow_particles;
  run_particle_flow(flow_particles, emc_clusters, hci_clusters, hco_clusters, reco_tracks, vtx);
  fastjet::ClusterSequence jet_finder_flow(flow_particles, *fJetAlgorithm);
  vector<fastjet::PseudoJet> flow_jets = jet_finder_flow.inclusive_jets(min_jet_pT);

  //Create JetMap from flow jets
  if (algorithm == "AntiKt")
    flow_jet_map->set_algo(Jet::ANTIKT);

  if (algorithm == "Kt")
    flow_jet_map->set_algo(Jet::KT);

  flow_jet_map->set_par(r_param);
  flow_jet_map->insert_src(Jet::TRACK);
  flow_jet_map->insert_src(Jet::CEMC_CLUSTER);
  flow_jet_map->insert_src(Jet::HCALIN_CLUSTER);
  flow_jet_map->insert_src(Jet::HCALOUT_CLUSTER);

  for (unsigned int i = 0; i < flow_jets.size(); i++)
  {
    JetV1* j = new JetV1();
    float px = flow_jets[i].px();
    float py = flow_jets[i].py();
    float pz = flow_jets[i].pz();
    float energy = flow_jets[i].E();

    //      cout << "--px = " << px << endl;

    j->set_px(px);
    j->set_py(py);
    j->set_pz(pz);
    j->set_e(energy);

    flow_jet_map->insert(j);
  }

  //  cout << "IDENTIFYING JET MAP" << endl;
  //  flow_jet_map->identify();
  //
  //  cout << "IDENTIFYING INDIVIDUAL JETS" << endl;
  //  for(JetMap::Iter it = flow_jet_map->begin(); it != flow_jet_map->end(); it++)
  //    {
  //      (it->second)->identify();
  //      cout << endl;
  //    }
  //
  //  cout << "FOUND " << flow_jet_map->size() << " FLOW JETS" << endl;
  return EVENT_OK;
}

/*
 * Run particle flow algorithm
 */
void PHFlowJetMaker::run_particle_flow(std::vector<fastjet::PseudoJet>& flow_particles, RawClusterContainer* emc_clusters, RawClusterContainer* hci_clusters, RawClusterContainer* hco_clusters, SvtxTrackMap* reco_tracks, GlobalVertex* vtx)
{
  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());

  double emc_energy = 0;
  double hci_energy = 0;
  double hco_energy = 0;
  double cluster_energy = 0;

  double px = 0;
  double py = 0;
  double pz = 0;
  //  double pt = 0;
  double et = 0;
  double p = 0;
  double track_energy = 0;
  double phi = 0;
  double eta = 0;

  //Make track maps
  tlvmap emc_map;
  tlvmap hci_map;
  tlvmap hco_map;

  for (unsigned int i = 0; i < emc_clusters->size(); i++)
  {
    RawCluster* part = emc_clusters->getCluster(i);
    //    double pT = (part->get_energy()) / cosh(part->get_eta());

    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);

    emc_map[i] = new TLorentzVector();
    //    emc_map[i]->SetPtEtaPhiE(pT, part->get_eta(), part->get_phi(), part->get_energy());
    emc_map[i]->SetPxPyPzE(
        E_vec_cluster.x(),
        E_vec_cluster.y(),
        E_vec_cluster.z(),
        E_vec_cluster.mag());
  }

  for (unsigned int i = 0; i < hci_clusters->size(); i++)
  {
    RawCluster* part = hci_clusters->getCluster(i);
    //    double pT = (part->get_energy()) / cosh(part->get_eta());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);
    hci_map[i] = new TLorentzVector();
    hci_map[i]->
        //    SetPtEtaPhiE(pT, part->get_eta(), part->get_phi(), part->get_energy());
        SetPxPyPzE(
            E_vec_cluster.x(),
            E_vec_cluster.y(),
            E_vec_cluster.z(),
            E_vec_cluster.mag());
  }

  for (unsigned int i = 0; i < hco_clusters->size(); i++)
  {
    RawCluster* part = hco_clusters->getCluster(i);
    //    double pT = (part->get_energy()) / cosh(part->get_eta());
    CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetEVec(*part, vertex);
    hco_map[i] = new TLorentzVector();
    hco_map[i]->
        //    SetPtEtaPhiE(pT, part->get_eta(), part->get_phi(), part->get_energy());
        SetPxPyPzE(
            E_vec_cluster.x(),
            E_vec_cluster.y(),
            E_vec_cluster.z(),
            E_vec_cluster.mag());
  }

  //Loop over all tracks
  for (SvtxTrackMap::Iter iter = reco_tracks->begin(); iter != reco_tracks->end(); ++iter)
  {
    SvtxTrack* trk = iter->second;
    px = trk->get_px();
    py = trk->get_py();
    pz = trk->get_pz();
    //      pt = sqrt(px*px + py*py);
    p = sqrt(px * px + py * py + pz * pz);
    track_energy = TMath::Sqrt(p * p + 0.139 * 0.139);  //Assume pion mass
    phi = atan2(py, px);
    eta = -log(tan(acos(pz / p) / 2.0));
    et = track_energy / cosh(eta);

    //Account for angle wrap-around
    if (phi < 0)
    {
      phi = phi + 2 * TMath::Pi();
    }
    else if (phi > 2 * TMath::Pi())
    {
      phi = phi + 2 * TMath::Pi();
    }

    //Quality cut on tracks
    if (trk->get_quality() > 3.0) continue;

    //Find ID of clusters that match to track in each layer
    int emcID = -1;
    int hciID = -1;
    int hcoID = -1;
    emcID = (int) trk->get_cal_cluster_id(SvtxTrack::CEMC);
    hciID = (int) trk->get_cal_cluster_id(SvtxTrack::HCALIN);
    hcoID = (int) trk->get_cal_cluster_id(SvtxTrack::HCALOUT);

    //Find energy deposited by track in each layer
    tlvmap::iterator it = emc_map.find(emcID);
    if (it != emc_map.end())
    {
      emc_energy = emc_map[emcID]->Energy();
    }

    it = hci_map.find(hciID);
    if (it != hci_map.end())
    {
      hci_energy = hci_map[hciID]->Energy();
    }

    it = hco_map.find(hcoID);
    if (it != hco_map.end())
    {
      hco_energy = hco_map[hcoID]->Energy();
    }

    cluster_energy = emc_energy + hci_energy + hco_energy;

    //Does the track match the cluster to within tolerance?
    //  *matched = 0 --> clus_energy < track_energy
    //  *matched = 1 --> clus_energy > track_energy
    //  *matched = 2 --> clus_energy = track_energy
    int matched = -1;
    matched = get_matched(cluster_energy, track_energy);

    //If matched = 1, remove track energy from clusters
    if (matched == 1)
    {
      float fracEnergyEMC = emc_energy / cluster_energy;
      float fracEnergyHCI = hci_energy / cluster_energy;
      float fracEnergyHCO = hco_energy / cluster_energy;

      it = emc_map.find(emcID);
      if (it != emc_map.end())
      {
        (emc_map.find(emcID)->second)->SetE(emc_energy - fracEnergyEMC * track_energy);
      }

      it = hci_map.find(hciID);
      if (it != hci_map.end())
      {
        (hci_map.find(hciID)->second)->SetE(hci_energy - fracEnergyHCI * track_energy);
      }

      it = hco_map.find(hcoID);
      if (it != hco_map.end())
      {
        (hco_map.find(hcoID)->second)->SetE(hco_energy - fracEnergyHCO * track_energy);
      }
    }
    else if (matched == 2)
    {
      it = emc_map.find(emcID);
      if (it != emc_map.end())
      {
        delete emc_map[emcID];
        emc_map.erase(emcID);
      }

      it = hci_map.find(hciID);
      if (it != emc_map.end())
      {
        delete hci_map[hciID];
        hci_map.erase(hciID);
      }

      it = hco_map.find(hcoID);
      if (it != hco_map.end())
      {
        delete hco_map[hcoID];
        hco_map.erase(hcoID);
      }
    }
    else if (matched == 0)
    {
      continue;
    }

    //Add perfectly matched and partially matched tracks to flow particle container
    if (et < 0.000001)
    {
      et = 0.001;
      pz = et * sinh(eta);
      track_energy = sqrt(et * et + pz * pz);
    }
    fastjet::PseudoJet pseudoJet_track(et * cos(phi), et * sin(phi), pz, track_energy);
    flow_particles.push_back(pseudoJet_track);
  }

  //Add remaining clusters to flow particle container
  for (tlvmap::iterator it = emc_map.begin(); it != emc_map.end(); it++)
  {
    double energy_clus = (it->second)->Energy();
    double eta_clus = (it->second)->Eta();
    double phi_clus = (it->second)->Phi();
    double et_clus = energy_clus / cosh(eta_clus);
    double pz_clus = et * sinh(eta_clus);

    if (et_clus < 0.000001)
    {
      et_clus = 0.001;
      pz_clus = et_clus * sinh(eta_clus);
      energy_clus = sqrt(et_clus * et_clus + pz_clus * pz_clus);
    }
    fastjet::PseudoJet pseudoJet_clus(et_clus * cos(phi_clus), et_clus * sin(phi_clus), pz_clus, energy_clus);
    flow_particles.push_back(pseudoJet_clus);
  }

  for (tlvmap::iterator it = hci_map.begin(); it != hci_map.end(); it++)
  {
    double energy_clus = (it->second)->Energy();
    double eta_clus = (it->second)->Eta();
    double phi_clus = (it->second)->Phi();
    double et_clus = energy_clus / cosh(eta_clus);
    double pz_clus = et * sinh(eta_clus);

    if (et_clus < 0.000001)
    {
      et_clus = 0.001;
      pz_clus = et_clus * sinh(eta_clus);
      energy_clus = sqrt(et_clus * et_clus + pz_clus * pz_clus);
    }
    fastjet::PseudoJet pseudoJet_clus(et_clus * cos(phi_clus), et_clus * sin(phi_clus), pz_clus, energy_clus);
    flow_particles.push_back(pseudoJet_clus);
  }

  for (tlvmap::iterator it = hco_map.begin(); it != hco_map.end(); it++)
  {
    double energy_clus = (it->second)->Energy();
    double eta_clus = (it->second)->Eta();
    double phi_clus = (it->second)->Phi();
    double et_clus = energy_clus / cosh(eta_clus);
    double pz_clus = et * sinh(eta_clus);

    if (et_clus < 0.000001)
    {
      et_clus = 0.001;
      pz_clus = et_clus * sinh(eta_clus);
      energy_clus = sqrt(et_clus * et_clus + pz_clus * pz_clus);
    }
    fastjet::PseudoJet pseudoJet_clus(et_clus * cos(phi_clus), et_clus * sin(phi_clus), pz_clus, energy_clus);
    flow_particles.push_back(pseudoJet_clus);
  }
}

/*
 * Create pseudojets from calorimeter clusters before applying particle flow algorithm
 * Jin - disable. Please use the standard cluster jet on the node tree
 */
//void PHFlowJetMaker::create_calo_pseudojets(std::vector<fastjet::PseudoJet>& particles, RawClusterContainer* emc_clusters, RawClusterContainer* hci_clusters, RawClusterContainer* hco_clusters, GlobalVertex* vtx)
//{
//  CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
//
//  //Loop over EMCAL clusters
//  for (unsigned int i = 0; i < emc_clusters->size(); i++)
//  {
//    RawCluster* part = emc_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy() / sfEMCAL;
//    double eT = energy / cosh(eta);
//    double pz = eT * sinh(eta);
//
//    if (eT < 0.000001)
//    {
//      eT = 0.001;
//      pz = eT * sinh(eta);
//      energy = sqrt(eT * eT + pz * pz);
//    }
//
//    fastjet::PseudoJet pseudoJet(eT * cos(phi), eT * sin(phi), pz, energy);
//    particles.push_back(pseudoJet);
//  }
//
//  //Loop over HCALIN clusters
//  for (unsigned int i = 0; i < hci_clusters->size(); i++)
//  {
//    RawCluster* part = hci_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy() / sfHCALIN;
//    double eT = energy / cosh(eta);
//    double pz = eT * sinh(eta);
//
//    if (eT < 0.000001)
//    {
//      eT = 0.001;
//      pz = eT * sinh(eta);
//      energy = sqrt(eT * eT + pz * pz);
//    }
//
//    fastjet::PseudoJet pseudoJet(eT * cos(phi), eT * sin(phi), pz, energy);
//    particles.push_back(pseudoJet);
//  }
//
//  //Loop over HCALOUT clusters
//  for (unsigned int i = 0; i < hco_clusters->size(); i++)
//  {
//    RawCluster* part = hco_clusters->getCluster(i);
//    double eta = part->get_eta();
//    double phi = part->get_phi();
//    double energy = part->get_energy() / sfHCALOUT;
//    double eT = energy / cosh(eta);
//    double pz = eT * sinh(eta);
//
//    if (eT < 0.000001)
//    {
//      eT = 0.001;
//      pz = eT * sinh(eta);
//      energy = sqrt(eT * eT + pz * pz);
//    }
//
//    fastjet::PseudoJet pseudoJet(eT * cos(phi), eT * sin(phi), pz, energy);
//    particles.push_back(pseudoJet);
//  }
//}

/*
 * Given a track and cluster energies, determine if they match within tolerance
 */
int PHFlowJetMaker::get_matched(double clus_energy, double track_energy)
{
  double limLo = match_tolerance_low->Eval(track_energy);
  double limHi = match_tolerance_high->Eval(track_energy);

  int matched = -1;

  if (clus_energy < limLo)
  {
    //Track energy greater than cluster energy. Most likely a fake
    matched = 0;
  }
  else if (clus_energy > limHi)
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
 * End
 */
int PHFlowJetMaker::End(PHCompositeNode* topNode)
{
  return EVENT_OK;
}
