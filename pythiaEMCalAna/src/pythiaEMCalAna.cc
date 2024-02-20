
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

//____________________________________________________________________________..
pythiaEMCalAna::pythiaEMCalAna(const std::string &name, const std::string &oname, bool isMC, bool hasPythia):
SubsysReco(name),
  clusters_Towers(nullptr),
  truth_particles(nullptr),
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
  m_cluster_maxE_trackID(0),
  m_cluster_maxE_PID(0),
  m_cluster_all_trackIDs(0),
  /* m_truthBarcode(0), */
  /* m_truthParentBarcode(0), */
  m_truthIsPrimary(0),
  m_truthTrackID(0),
  m_truthPid(0),
  m_truthE(0),
  m_truthEta(0),
  m_truthPhi(0),
  m_truthPt(0),
  m_truthMass(0),
  m_truthEndVtx_x(0),
  m_truthEndVtx_y(0),
  m_truthEndVtx_z(0),
  m_truthEndVtx_t(0),
  m_truthEndVtx_r(0),
  m_truth_all_clusterIDs(0),
  fout(NULL),
  outname(oname),
  getEvent(-9999),
  /* hasHIJING(isAuAu), */
  isMonteCarlo(isMC),
  hasPYTHIA(hasPythia),
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
  pythiaBarcodes()
  /* n_direct_photons_in_acceptance(0), */
  /* n_pythia_direct_photons(0), */
  /* n_decay_photons(0), */
  /* n_decay_photons_in_acceptance(0), */
  /* n_pythia_decays(0), */
  /* n_pythia_decays_in_acceptance(0), */
  /* n_geant_decays(0), */
  /* n_geant_decays_in_acceptance(0), */
  /* n_primary_in_acceptance(0), */
  /* n_pythia_decay_photons(0), */
  /* n_pythia_decay_photons_in_acceptance(0), */
  /* n_pythia_decayed_pi0s(0), */
  /* n_pythia_decayed_pi0s_in_acceptance(0), */
  /* n_pythia_nondecayed_hadrons(0), */
  /* n_pythia_nondecayed_hadrons_in_acceptance(0), */
  /* n_pythia_nondecayed_pi0s(0), */
  /* n_pythia_nondecayed_pi0s_in_acceptance(0), */
  /* n_geant_decay_photons(0), */
  /* n_geant_decay_photons_in_acceptance(0), */
  /* n_geant_primary_hadrons(0), */
  /* n_geant_primary_hadrons_in_acceptance(0), */
  /* n_geant_primary_pi0s(0), */
  /* n_geant_primary_pi0s_in_acceptance(0), */
  /* pythia_primary_barcodes(0), */
  /* primaryBarcodes(0), */
  /* secondaryBarcodes(0) */
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
  if (isMonteCarlo) {
      clusters_Towers -> Branch("clusterMaxE_trackID",&m_cluster_maxE_trackID);
      clusters_Towers -> Branch("clusterMaxE_PID",&m_cluster_maxE_PID);
      clusters_Towers -> Branch("clusterAll_trackIDs",&m_cluster_all_trackIDs, 32000, 0);
  }

  if (isMonteCarlo) {
      truth_particles = new TTree("TreeTruthParticles", "Tree for Truth Particles");
      /* truth_particles->Branch("truthBarcode",&m_truthBarcode, 32000, 0); */
      /* truth_particles->Branch("truthParentBarcode",&m_truthParentBarcode, 32000, 0); */
      truth_particles->Branch("truthIsPrimary",&m_truthIsPrimary);
      truth_particles->Branch("truthTrackID",&m_truthTrackID);
      truth_particles->Branch("truthPid",&m_truthPid);
      truth_particles->Branch("truthE",&m_truthE);
      truth_particles->Branch("truthEta",&m_truthEta);
      truth_particles->Branch("truthPhi",&m_truthPhi);
      truth_particles->Branch("truthPt",&m_truthPt);
      truth_particles->Branch("truthMass",&m_truthMass);
      truth_particles->Branch("truthEndVtx_x",&m_truthEndVtx_x);
      truth_particles->Branch("truthEndVtx_y",&m_truthEndVtx_y);
      truth_particles->Branch("truthEndVtx_z",&m_truthEndVtx_z);
      truth_particles->Branch("truthEndVtx_t",&m_truthEndVtx_t);
      truth_particles->Branch("truthEndVtx_r",&m_truthEndVtx_r);
      truth_particles->Branch("truthAllClusterIDs",&m_truth_all_clusterIDs, 32000, 0);
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

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = nullptr;
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
	  if (isMonteCarlo) std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
	  else std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node TOWERINFO_CALIB_CEMC"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }

  CaloEvalStack *caloevalstack = nullptr;
  CaloRawClusterEval *clustereval = nullptr;
  if (isMonteCarlo) {
      caloevalstack = new CaloEvalStack(topNode, "CEMC");
      caloevalstack->next_event(topNode);
      clustereval = caloevalstack->get_rawcluster_eval();
      if(!clustereval)
      {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event cluster eval not available"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      } 
  }
  
  //Information on clusters
  RawClusterContainer *clusterContainer = nullptr;
  // Name of node is different in MC and RD
  if (isMonteCarlo) {
      /* clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC"); */
      clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  }
  else {
      /* clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_CEMC"); */
      clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTERINFO_POS_COR_CEMC");
  }
  if(!clusterContainer)
  {
      if (isMonteCarlo) std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;
      else std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - CLUSTERINFO_CEMC node is missing. " << std::endl;
      return 0;
  }

  // Truth information
  PHG4TruthInfoContainer *truthinfo = nullptr;
  if (isMonteCarlo) {
      truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
      if(!truthinfo) {
	  std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node G4TruthInfo"  << std::endl;
	  return Fun4AllReturnCodes::ABORTEVENT;
      }
  }
  //Vertex information
  GlobalVertex* gVtx = nullptr;
  PHG4VtxPoint* mcVtx = nullptr;
  // Problem is MC has a PrimaryVtx but no GlobalVertex, while RD has the opposite
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::VtxRange vtx_range = truthinfo->GetPrimaryVtxRange();
      PHG4TruthInfoContainer::ConstVtxIterator vtxIter = vtx_range.first;
      mcVtx = vtxIter->second;
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
	  /* std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl; */
	  /* return 0; */
	  std::cout << "Info: global vertex map is empty. Using (0,0,0) for vertex\n";
      }

      //More vertex information
      else {
	  gVtx = vtxContainer->begin()->second;
	  if(!gVtx)
	  {
	      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find vtx from vtxContainer"  << std::endl;
	      return Fun4AllReturnCodes::ABORTEVENT;
	  }
      }
  }

  /* PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange(); */
  /* PHG4TruthInfoContainer::ConstIterator truthIter; */
  /* //from the HepMC event log */
  /* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
  /*   { */
  /*     //PHG4Particle* part = truthinfo->GetParticle(truthIter->second->get_trkid()) */

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
   
  HepMC::GenEvent *theEvent = nullptr; // = genEvent -> getEvent();

  //grab all the towers and fill their energies. 
  bool write_towers = true;
  if (write_towers && isMonteCarlo) {
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
  }
  
  //grab all the clusters in the event
  bool write_clusters = true;
  bool apply_energy_cut = false;
  float clusterMinEnergyCut = 0.3; // in GeV
  if (write_clusters) {
      RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
      RawClusterContainer::ConstIterator clusterIter;
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

	  if (isMonteCarlo) {
	      std::set<PHG4Particle*> all_parts = clustereval->all_truth_primary_particles(recoCluster);
	      if (all_parts.empty()) {
		  // noise cluster without any real particles creating the shower
		  m_cluster_maxE_trackID.push_back(-1);
		  m_cluster_maxE_PID.push_back(-1);
		  std::vector<float> all_cluster_trackIDs;
		  m_cluster_all_trackIDs.push_back(all_cluster_trackIDs);
	      }
	      else {
		  PHG4Particle* maxE_part = clustereval->max_truth_primary_particle_by_energy(recoCluster);
		  m_cluster_maxE_trackID.push_back(maxE_part->get_track_id());
		  m_cluster_maxE_PID.push_back(maxE_part->get_pid());
		  // next we want a vector with all the tracks contributing to this cluster
		  std::vector<float> all_cluster_trackIDs;
		  for (auto& part : all_parts) {
		      all_cluster_trackIDs.push_back(part->get_track_id());
		      allTrackIDs.insert(part->get_track_id());
		  }
		  m_cluster_all_trackIDs.push_back(all_cluster_trackIDs);
	      }
	  }
      } // end loop over clusters
  }
  
  // Next grab the truth particles
  if (isMonteCarlo) {
      PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
      PHG4TruthInfoContainer::ConstIterator truthIter;
      /* std::cout << "\n\nGreg info: starting loop over Geant primary particles\n"; */
      for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
      {
	  PHG4Particle *truthPar = truthIter->second;
	  int track_id = truthPar->get_track_id();
	  // only interested in particles which produced a cluster
	  if (allTrackIDs.count(track_id) == 0) continue;
	  int embedID = truthinfo->isEmbeded(track_id);
	  genEvent = genEventMap -> get(embedID);
	  theEvent = genEvent -> getEvent();
	  if (truthPar->get_pid() != 22) {
	      // if it's not a photon, add it as a primary
	      addPrimaryFromGeant(truthPar, truthinfo, clustereval, theEvent);
	  }
	  else {
	      n_primary_photons++;
	      // if it is a photon, we have to check whether it's truly primary,
	      // or a decay photon handled in pythia
	      if (isDirectPhoton(truthPar, theEvent)) {
		  // if it's really a direct photon, add it normally
		  addPrimaryFromGeant(truthPar, truthinfo, clustereval, theEvent);
	      }
	      else {
		  // primary photon is actually a decay photon from a decay handled by pythia
		  // find the parent in pythia and add that as the true primary
		  HepMC::GenParticle* p = getGenParticle(truthPar->get_barcode(), theEvent);
		  HepMC::GenVertex* prod_vtx = p->production_vertex();
		  // should only have 1 parent; otherwise print an error
		  if (prod_vtx->particles_in_size() == 1) {
		      HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin();
		      assert((*parent));
		      // parent's status and pdg_id tell us whether it's a direct photon
		      // or a decay photon
		      if ((*parent)->status() == 2 && abs((*parent)->pdg_id()) >= 100) {
			  // p is a decay photon and parent is the primary hadron
			  // check if we've already added the parent
			  if (pythiaBarcodes.count((*parent)->barcode()) == 0) {
			      // if not, add it
			      addPrimaryFromPythia((*parent));
			  }
		      }
		      else {
			  // weird case??
			  std::cout << "\nGreg info: weird combination of photon or parent status or PID... photon:\n";
			  p->print();
			  std::cout << "Parent:\n";
			  (*parent)->print();
			  std::cout << "\n";
		      }
		  }
		  else {
		      // weird photon -- they should only have 1 parent
		      std::cout << "\nGreg info: in PYTHIA check, found a photon with " << prod_vtx->particles_in_size() << " parent(s). Photon:\n";
		      p->print();
		      for (HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin(); parent != prod_vtx->particles_in_const_end(); parent++) {
			  std::cout << "Parent:\n";
			  (*parent)->print();
		      }
		      std::cout << "\n";
		  }
	      } // end decay photon case
	  } // end photon check
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
  m_cluster_maxE_trackID.clear();
  m_cluster_maxE_PID.clear();
  m_cluster_all_trackIDs.clear();
  m_truthIsPrimary.clear();
  m_truthTrackID.clear();
  m_truthPid.clear();
  m_truthE.clear();
  m_truthEta.clear();
  m_truthPhi.clear();
  m_truthPt.clear();
  m_truthMass.clear();
  m_truthEndVtx_x.clear();
  m_truthEndVtx_y.clear();
  m_truthEndVtx_z.clear();
  m_truthEndVtx_t.clear();
  m_truthEndVtx_r.clear();
  m_truth_all_clusterIDs.clear();
  allTrackIDs.clear();
  pythiaBarcodes.clear();
  
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

void pythiaEMCalAna::addPrimaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo, CaloRawClusterEval* caloEval, HepMC::GenEvent* theEvent) {
    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
    if (! withinAcceptance(part)) return;
    int this_id = part->get_track_id();
    
    PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo);
    if (!end_vtx) {
	std::cout << "\nGreg info: no end vertex found for Geant-decayed primary:\n";
	part->identify();
	std::cout << Form("Mass=%f, pT=%f\n\n", truth_momentum.M(), truth_momentum.Pt());
	return;
    }

    n_primaries++;

    /* int embedID = truthInfo->isEmbeded(part->get_track_id()); */
    /* m_truthBarcode.push_back(std::pair<int,int>(embedID, part->get_barcode())); */
    /* m_truthParentBarcode.push_back(std::pair<int,int>(embedID, 0)); */
    if (part->get_pid() == 22) {
	/* n_primary_photons++; */
	// check for direct photons is done in process_event
	n_direct_photons++;
    }
    if (abs(part->get_pid()) < 20) n_leptons++;
    if (abs(part->get_pid()) > 100) {
	if (IsNeutralHadron(abs(part->get_pid()))) {
	    n_neutral_hadrons++;
	    n_neutral_hadrons_geant++;
	}
	else {
	    n_charged_hadrons++;
	    n_charged_hadrons_geant++;
	}
    }

    m_truthIsPrimary.push_back(1);
    m_truthTrackID.push_back(this_id);
    m_truthPid.push_back(part->get_pid());

    m_truthE.push_back(truth_momentum.E());
    m_truthEta.push_back(truth_momentum.PseudoRapidity());
    m_truthPhi.push_back(truth_momentum.Phi());
    m_truthPt.push_back(truth_momentum.Pt());
    m_truthMass.push_back(truth_momentum.M());

    m_truthEndVtx_x.push_back(end_vtx->get_x());
    m_truthEndVtx_y.push_back(end_vtx->get_y());
    m_truthEndVtx_z.push_back(end_vtx->get_z());
    m_truthEndVtx_t.push_back(end_vtx->get_t());
    float x = end_vtx->get_x();
    float y = end_vtx->get_y();
    float r = sqrt(x*x + y*y);
    m_truthEndVtx_r.push_back(r);

    std::vector<float> allClusterIDs;

    std::set<RawCluster*> clusters = caloEval->all_clusters_from(part);
    for (auto& cl : clusters) {
	allClusterIDs.push_back(cl->get_id());
    }
    m_truth_all_clusterIDs.push_back(allClusterIDs);
}


void pythiaEMCalAna::addPrimaryFromPythia(HepMC::GenParticle* part) {
    /* std::cout << "Entering pythiaEMCalAna::addPrimaryFromPythia\npart=" << part << "\n"; */
    HepMC::FourVector mom = part->momentum();
    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e());
    if (! withinAcceptance(part)) return;

    pythiaBarcodes.insert(part->barcode());
    n_primaries++;
    n_pythia_decayed_hadrons++;
    if (IsNeutralHadron(abs(part->pdg_id()))) {
	n_neutral_hadrons++;
	n_neutral_hadrons_pythia++;
    }
    else {
	n_charged_hadrons++;
	n_charged_hadrons_pythia++;
    }

    m_truthIsPrimary.push_back(1);
    // no track ID available from pythia
    m_truthTrackID.push_back(-999);
    m_truthPid.push_back(part->pdg_id());

    m_truthE.push_back(truth_momentum.E());
    m_truthEta.push_back(truth_momentum.PseudoRapidity());
    m_truthPhi.push_back(truth_momentum.Phi());
    m_truthPt.push_back(truth_momentum.Pt());
    m_truthMass.push_back(truth_momentum.M());

    /* std::cout << "Getting end vertex\n"; */
    HepMC::GenVertex* end_vtx = part->end_vertex();
    assert(end_vtx);
    /* std::cout << "Found end vertex in pythia\n"; */
    HepMC::FourVector position = end_vtx->position();

    m_truthEndVtx_x.push_back(position.x());
    m_truthEndVtx_y.push_back(position.y());
    m_truthEndVtx_z.push_back(position.z());
    m_truthEndVtx_t.push_back(position.t());
    float x = position.x();
    float y = position.y();
    float r = sqrt(x*x + y*y);
    /* std::cout << Form("pythiaEMCalAna::addPrimaryFromPythia(): Vertex r=%f, perp=%f\n", r, position.perp()); */
    m_truthEndVtx_r.push_back(r);

    // no cluster matching available from pythia
    std::vector<float> allClusterIDs;
    m_truth_all_clusterIDs.push_back(allClusterIDs);
}

bool pythiaEMCalAna::isDirectPhoton(PHG4Particle* part, HepMC::GenEvent* theEvent) {
    /* std::cout << "\tGreg info: entering isDirectPhoton(). "; */
    /* std::cout << "G4 particle is " << part << ", barcode " << part->get_barcode() << "\n";//; printing info\n"; */
    /* part->identify(); */
    
    // Only bother with this check if we have PYTHIA information
    // Otherwise assume any photon is a "direct" photon, since we can't tell from HIJING alone
    if (!hasPYTHIA) return (part->get_pid() == 22);
    HepMC::GenParticle* genpart = getGenParticle(part->get_barcode(), theEvent);
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

HepMC::GenParticle* pythiaEMCalAna::getGenParticle(int barcode, HepMC::GenEvent* theEvent) {
    /* std::cout << "Greg info: in getGenParticle, looking for barcode " << barcode << "\n"; */
    for(HepMC::GenEvent::particle_const_iterator p=theEvent->particles_begin(); p!=theEvent->particles_end(); ++p)
    {
	assert(*p);
	/* std::cout << "\tpythia barcode " << (*p)->barcode() << "\n"; */
	/* if ((*p)->barcode() == 33) { */
	/*     std::cout << "barcode 33 details: \t"; */
	/*     (*p)->print(); */
	/* } */
	if ( (*p)->barcode() == barcode ) {
	    // found the right particle
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

PHG4VtxPoint* pythiaEMCalAna::getG4EndVtx(int id, PHG4TruthInfoContainer* truthInfo) {
    PHG4VtxPoint* end_vtx = nullptr;
    PHG4TruthInfoContainer::Range truthRange = truthInfo->GetSecondaryParticleRange();
    PHG4TruthInfoContainer::ConstIterator truthIter;
    for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
	PHG4Particle *decayPar = truthIter->second;
	int parent_id = decayPar->get_parent_id();
	if (parent_id == id) {
	    end_vtx = truthInfo->GetVtx(decayPar->get_vtx_id());
	    break;
	}
    }
    if (!end_vtx) {
	// couldn't find any daughter particles in secondary list
	// check primary list instead??
	truthRange = truthInfo->GetPrimaryParticleRange();
	for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
	{
	    PHG4Particle *decayPar = truthIter->second;
	    int parent_id = decayPar->get_parent_id();
	    if (parent_id == id) {
		std::cout << "\tGreg info: found daughter in *primary* particle range!\n";
		end_vtx = truthInfo->GetVtx(decayPar->get_vtx_id());
		break;
	    }
	}
    }
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

