
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
#include <g4eval/CaloRawClusterEval.h>

//for vertex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
/* #include <g4vertex/GlobalVertex.h> */
/* #include <g4vertex/GlobalVertexMap.h> */

//tracking
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

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
pythiaEMCalAna::pythiaEMCalAna(const std::string &name):
SubsysReco(name),
  clusters_Towers(nullptr),
  truth_particles(nullptr),
  m_tower_energy(0),
  m_eta_center(0),
  m_phi_center(0),
  m_cluster_e(0),
  m_cluster_eta(0),
  m_cluster_phi(0),
  m_cluster_ecore(0),
  m_cluster_chi2(0),
  m_cluster_prob(0),
  m_cluster_nTowers(0),
  m_truthBarcode(0),
  m_truthParentBarcode(0),
  m_truthIsPrimary(0),
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
fout(NULL),
  outname(name),
  getEvent(-9999),
  primaryBarcodes(0),
  secondaryBarcodes(0)
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
  clusters_Towers -> Branch("towerEtaCEnter",&m_eta_center);
  clusters_Towers -> Branch("towerPhiCenter",& m_phi_center);
  clusters_Towers -> Branch("clusterE",&m_cluster_e);
  clusters_Towers -> Branch("clusterEta",&m_cluster_eta);
  clusters_Towers -> Branch("clusterPhi", &m_cluster_phi);
  clusters_Towers -> Branch("clusterEcore",&m_cluster_ecore);
  clusters_Towers -> Branch("clusterChi2",&m_cluster_chi2);
  clusters_Towers -> Branch("clusterProb",&m_cluster_prob);
  clusters_Towers -> Branch("clusterNTowers",&m_cluster_nTowers);

  truth_particles = new TTree("TreeTruthParticles", "Tree for Truth Particles");
  truth_particles->Branch("truthBarcode",&m_truthBarcode);
  truth_particles->Branch("truthParentBarcode",&m_truthParentBarcode);
  truth_particles->Branch("truthIsPrimary",&m_truthIsPrimary);
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
  std::cout << "pythiaEMCalAna::process_event(PHCompositeNode *topNode) Processing event..." << std::endl;
  //Information on clusters
  // RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "pythiaEMCalAna::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;

      return 0;
    }

  //Vertex information
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
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find vtx from vtxContainer"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  if(!towerContainer)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //For eventgen ancestory information
  PHHepMCGenEventMap *genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if(!genEventMap)
    {
      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find PHHepMCGenEventMap"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //event level information of the above
  PHHepMCGenEvent *genEvent = genEventMap -> get(getEvent);
  if(!genEvent)
    {
      std::cout << PHWHERE << "pythiaEMCalAna::process_event Could not find PHHepMCGenEvent"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
   
  HepMC::GenEvent *theEvent = genEvent -> getEvent();

  CaloEvalStack caloevalstack(topNode, "CEMC");
  CaloRawClusterEval *clustereval = caloevalstack.get_rawcluster_eval();
  if(!clustereval)
    {
      std::cout << PHWHERE << "pythiaEMCalAna::process_event cluster eval not available"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    } 
  
  //grab all the towers and fill their energies. 
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
  
  //grab all the clusters in the event
  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;
      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      /* CLHEP::Hep3Vector vertex(0., 0., 0.); */
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      float clusE = E_vec_cluster.mag();
      float clusEcore = recoCluster->get_ecore();
      float clus_eta = E_vec_cluster.pseudoRapidity();
      float clus_phi = E_vec_cluster.phi();
      
      m_cluster_e.push_back(clusE);
      m_cluster_eta.push_back(clus_eta);
      m_cluster_phi.push_back(clus_phi);
      m_cluster_ecore.push_back(clusEcore);
      
      m_cluster_chi2.push_back(recoCluster -> get_chi2());
      m_cluster_prob.push_back(recoCluster -> get_prob());
      m_cluster_nTowers.push_back(recoCluster -> getNTowers());
    }
  
  // next grab the truth particles

  /* //grab all the direct photons in the event */
  /* PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange(); */
  /* PHG4TruthInfoContainer::ConstIterator truthIter; */
  /* //from the HepMC event log */
  /* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
  /*   { */
  /*     //PHG4Particle* part = truthinfo->GetParticle(truthIter->second->get_trkid()) */
      
  /*     PHG4Particle *truthPar = truthIter->second; */
  /*     int previousparentid = -999; */
  /*     if(truthPar -> get_pid() != 22) continue;//end with a photon */

      
  /*     int parentid = truthPar->get_parent_id(); */
  /*     previousparentid = truthPar->get_track_id(); */
  /*     while (parentid != 0) */
	/* { */
	  /* previousparentid = parentid; */
	  /* PHG4Particle* mommypart = truthinfo->GetParticle(parentid); */
	  /* parentid = mommypart->get_parent_id(); */
	/* } */
      
  /*     if(previousparentid > 0)truthPar = truthinfo -> GetParticle(previousparentid); */
  /*     if(truthPar -> get_pid() != 22) continue;//start with a photon */

  /*     for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p) */
	/* { */
	  /* assert(*p); */
	   
	  /* if((*p) -> barcode() == truthPar -> get_barcode()) */
	    /* { */
	      /* for(HepMC::GenVertex::particle_iterator mother = (*p)->production_vertex()->particles_begin(HepMC::parents); */
		  /* mother != (*p)->production_vertex()->particles_end(HepMC::parents); ++mother) */
		/* { */
		  /* //std::cout << "Mother pid: " << (*mother) -> pdg_id() << std::endl; */
		  /* //if((*mother) -> pdg_id() != 21 || (*mother) -> pdg_id() != 1 || (*mother) -> pdg_id() != 2) continue; */
		  /* if((*mother) -> pdg_id() != 22) continue; */

		  /* HepMC::FourVector moMomentum = (*mother) -> momentum(); */

		  /* m_photonE.push_back(moMomentum.e()); */
		  /* m_photonEta.push_back(moMomentum.pseudoRapidity()); */
		  /* m_photonPhi.push_back(moMomentum.phi()); */
		  /* m_photonPt.push_back(sqrt(moMomentum.px()*moMomentum.px()+moMomentum.py()*moMomentum.py())); */
		  
		  
		/* } */
		       
	    /* } */
	/* } */
  /*   } */

  // PYTHIA info
  /* for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p) */
  /* { */
  /*     assert(*p); */
  /*     /1* (*p)->print(); *1/ */
  /*     m_truthPid.push_back((*p)->pdg_id()); */
  /*     n_pythia++; */
  /*     if ( (*p)->pdg_id() == 22 ) { */
	  /* /1* std::cout << "Found a photon in PYTHIA\n"; *1/ */
	  /* /1* (*p)->print(); *1/ */
	  /* /1* std::cout << "status = " << (*p)->status() << "\n"; *1/ */
	  /* /1* HepMC::GenVertex* prod_vtx = (*p)->production_vertex(); *1/ */
	  /* /1* std::cout << "printing production vertex info:\n"; *1/ */
	  /* /1* prod_vtx->print(); *1/ */
	  /* n_pho_pythia++; */
	  /* pho_barcodes_pythia.push_back((*p)->barcode()); */
  /*     } */
  /*     if ( (*p)->pdg_id() == 111 ) { */
	  /* n_pi0_pythia++; */
	  /* pi0_barcodes_pythia.push_back((*p)->barcode()); */
  /*     } */
  /* } */
  /* std::cout << "PYTHIA: " << n_pythia << " total particles, " << n_pho_pythia << " photons, " << n_pi0_pythia << " pi0s\n"; */

  // Start with primary particles from GEANT
  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
  {
      PHG4Particle *truthPar = truthIter->second;
      std::cout << "Beginning geant primary particle truthPar=" << truthPar << "\n";
      int geant_barcode = truthPar->get_barcode();
      int pid = truthPar->get_pid();
      if (pid != 22) {
	  // if it's not a photon, consider it a valid primary particle
	  std::cout << "Found a non-photon primary truthPar=" << truthPar << ", adding it\n";
	  addPrimaryFromGeant(truthPar, truthinfo);
	  std::cout << "\tadded\n";
      }
      else {
	  // if it's a photon, check if it came from a pi0
	  // get the corresponding truth particle from PYTHIA
	  HepMC::GenParticle* genPar = getGenParticle(geant_barcode, theEvent);
	  HepMC::GenVertex* prod_vtx = genPar->production_vertex();
	  std::cout << "Found production vertex " << prod_vtx << " with " << prod_vtx->particles_in_size() << " particle(s) going in\n";
	  if (prod_vtx->particles_in_size() == 1) {
	      HepMC::GenVertex::particles_in_const_iterator parent = prod_vtx->particles_in_const_begin();
	      std::cout << "Found *parent=" << *parent << "\n";
	      if ( (*parent)->pdg_id() == 111 ) {
		  // make sure the parent pi0 hasn't already been added
		  std::cout << "Parent is a pi0; checking if we already added it\n";
		  if (!vector_contains((*parent)->barcode(), primaryBarcodes)) {
		      std::cout << "Adding primary *parent=" << *parent << "\n";
		      addPrimaryFromPythia((*parent));
		      std::cout << "\tadded\n";
		  }
		  // add the photon as a secondary particle
		  std::cout << "Adding secondary truthPar=" << truthPar << "\n";
		  addSecondary(truthPar, truthinfo);
		  std::cout << "\tadded\n";
	      } // end case where parent is a pi0
	  }  // end case where photon has exactly 1 parent
	  else {
	      // photon does not come from a decay
	      addPrimaryFromGeant(truthPar, truthinfo);
	  }
	  std::cout << "Ending geant primary particle truthPar=" << truthPar << "\n";
      }
  } // Done looping over GEANT primary particles

  // Now loop over GEANT secondary particles, taking only the daughters
  // of primary particles we already found
  truthRange = truthinfo->GetSecondaryParticleRange();
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
  {
      PHG4Particle *truthPar = truthIter->second;
      // check if the parent is in the primary list
      PHG4Particle* parent = truthinfo->GetParticle(truthPar->get_parent_id());
      if (!vector_contains(parent->get_barcode(), primaryBarcodes)) continue;
      else addSecondary(truthPar, truthinfo);
  }

    /* if (n_pi0_pythia > n_pi0_geant) { */
	/* std::cout << "Found more pi0s in pythia than in geant. More details below...\n"; */
	/* std::cout << "pythia photon barcodes: "; */
	/* for (unsigned int i=0; i<pho_barcodes_pythia.size(); i++) std::cout << pho_barcodes_pythia.at(i) << ", "; */
	/* std::cout << std::endl; */
	/* std::cout << "geant photon barcodes: "; */
	/* for (unsigned int i=0; i<pho_barcodes_geant.size(); i++) std::cout << pho_barcodes_geant.at(i) << ", "; */
	/* std::cout << std::endl; */
	/* std::cout << "pythia pi0 barcodes: "; */
	/* for (unsigned int i=0; i<pi0_barcodes_pythia.size(); i++) std::cout << pi0_barcodes_pythia.at(i) << ", "; */
	/* std::cout << std::endl; */
	/* std::cout << "geant pi0 barcodes: "; */
	/* for (unsigned int i=0; i<pi0_barcodes_geant.size(); i++) std::cout << pi0_barcodes_geant.at(i) << ", "; */
	/* std::cout << std::endl; */
	/* for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p) */
	/* { */
	/*     assert(*p); */
	/*     if ( (*p)->pdg_id() == 111 && (*p)->barcode() == 71) { */
	/* 	std::cout << "Found a pi0 in PYTHIA\n"; */
	/* 	(*p)->print(); */
	/* 	std::cout << "status = " << (*p)->status() << "\n"; */
	/* 	HepMC::GenVertex* prod_vtx = (*p)->production_vertex(); */
	/* 	std::cout << "printing production vertex info:\n"; */
	/* 	prod_vtx->print(); */
	/* 	HepMC::GenVertex* decay_vtx = (*p)->end_vertex(); */
	/* 	std::cout << "printing decay vertex info:\n"; */
	/* 	if (decay_vtx) decay_vtx->print(); */
	/*     } */
	/* } */
	/* for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++) */
	/* { */
	/*     PHG4Particle *truthPar = truthIter->second; */
	/*     /1* std::cout << "Barcode " << truthPar->get_barcode() << " ID: " << truthPar->get_pid() << "\n"; *1/ */
	/*     if (truthPar->get_pid() == 22) { */
	/* 	std::cout << "Found a photon in GEANT\n"; */
	/* 	std::cout << Form("name=%s, pid=%d, barcode=%d, parent_id=%d, primary_id=%d\n", truthPar->get_name().c_str(), truthPar->get_pid(), truthPar->get_barcode(), truthPar->get_parent_id(), truthPar->get_primary_id()); */
	/* 	int barcode = truthPar->get_barcode(); */
	/* 	for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p) */
	/* 	{ */
	/* 	    assert(*p); */
	/* 	    if ( (*p)->barcode() == barcode ) { */
	/* 		std::cout << "Found the corresponding photon in pythia\n"; */
	/* 		(*p)->print(); */
	/* 		HepMC::GenVertex* prod_vtx = (*p)->production_vertex(); */
	/* 		std::cout << "printing production vertex info:\n"; */
	/* 		prod_vtx->print(); */
	/* 		HepMC::GenVertex* decay_vtx = (*p)->end_vertex(); */
	/* 		std::cout << "printing decay vertex info:\n"; */
	/* 		if (decay_vtx) decay_vtx->print(); */

	/* 		if (prod_vtx->particles_in_size() == 1) { */
	/* 		    HepMC::GenVertex::particles_in_const_iterator p_in = prod_vtx->particles_in_const_begin(); */
	/* 		    std::cout << "Printing parent particle:\n"; */
	/* 		    (*p_in)->print(); */
	/* 		} */
	/* 	    } */
	/* 	} */
	/*     } */
	/* } */



  
  clusters_Towers -> Fill();
  truth_particles -> Fill();
  
return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pythiaEMCalAna::ResetEvent(PHCompositeNode *topNode)
{

  m_eta_center.clear();
  m_phi_center.clear();
  m_tower_energy.clear();
  m_cluster_eta.clear();
  m_cluster_phi.clear();
  m_cluster_e.clear();
  m_cluster_chi2.clear();
  m_cluster_prob.clear();
  m_cluster_nTowers.clear();
  m_truthBarcode.clear();
  m_truthParentBarcode.clear();
  m_truthIsPrimary.clear();
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
  primaryBarcodes.clear();
  secondaryBarcodes.clear();
  
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
  fout -> cd();
  clusters_Towers -> Write();
  truth_particles -> Write();
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

void pythiaEMCalAna::addPrimaryFromGeant(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) {
    m_truthBarcode.push_back(part->get_barcode());
    m_truthParentBarcode.push_back(0);
    m_truthIsPrimary.push_back(1);
    m_truthPid.push_back(part->get_pid());

    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
    m_truthE.push_back(truth_momentum.E());
    m_truthEta.push_back(truth_momentum.PseudoRapidity());
    m_truthPhi.push_back(truth_momentum.Phi());
    m_truthPt.push_back(truth_momentum.Pt());
    m_truthMass.push_back(truth_momentum.M());

    // part->get_vtx_id() gives the *production* vertex, not the end vertex
    /* PHG4VtxPoint* end_vtx = truthInfo->GetVtx(part->get_vtx_id()); // DOES NOT WORK */
    // get the end vertex from one of the daughter particles
    int this_id = part->get_track_id();
    PHG4VtxPoint* end_vtx = getG4EndVtx(this_id, truthInfo);
    assert(end_vtx);
    m_truthEndVtx_x.push_back(end_vtx->get_x());
    m_truthEndVtx_y.push_back(end_vtx->get_y());
    m_truthEndVtx_z.push_back(end_vtx->get_z());
    m_truthEndVtx_t.push_back(end_vtx->get_t());
    float x = end_vtx->get_x();
    float y = end_vtx->get_y();
    float r = sqrt(x*x + y*y);
    m_truthEndVtx_r.push_back(r);

    primaryBarcodes.push_back(part->get_barcode());
}

void pythiaEMCalAna::addPrimaryFromPythia(HepMC::GenParticle* part) {
    std::cout << "Entering pythiaEMCalAna::addPrimaryFromPythia\npart=" << part << "\n";
    m_truthBarcode.push_back(part->barcode());
    m_truthParentBarcode.push_back(0);
    m_truthIsPrimary.push_back(1);
    m_truthPid.push_back(part->pdg_id());

    std::cout << "Getting momentum\n";
    HepMC::FourVector mom = part->momentum();
    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(mom.px(), mom.py(), mom.pz(), mom.e());
    m_truthE.push_back(truth_momentum.E());
    m_truthEta.push_back(truth_momentum.PseudoRapidity());
    m_truthPhi.push_back(truth_momentum.Phi());
    m_truthPt.push_back(truth_momentum.Pt());
    m_truthMass.push_back(truth_momentum.M());

    std::cout << "Getting end vertex\n";
    HepMC::GenVertex* end_vtx = part->end_vertex();
    assert(end_vtx);
    std::cout << "Found end vertex in pythia\n";
    HepMC::FourVector position = end_vtx->position();

    m_truthEndVtx_x.push_back(position.x());
    m_truthEndVtx_y.push_back(position.y());
    m_truthEndVtx_z.push_back(position.z());
    m_truthEndVtx_t.push_back(position.t());
    float x = position.x();
    float y = position.y();
    float r = sqrt(x*x + y*y);
    std::cout << Form("Vertex r=%f, perp=%f\n", r, position.perp());
    m_truthEndVtx_r.push_back(r);

    primaryBarcodes.push_back(part->barcode());
}

void pythiaEMCalAna::addSecondary(PHG4Particle* part, PHG4TruthInfoContainer* truthInfo) {
    m_truthBarcode.push_back(part->get_barcode());
    PHG4Particle* parent = truthInfo->GetParticle(part->get_parent_id());
    m_truthParentBarcode.push_back(parent->get_barcode());
    m_truthIsPrimary.push_back(0);
    m_truthPid.push_back(part->get_pid());

    TLorentzVector truth_momentum;
    truth_momentum.SetPxPyPzE(part->get_px(), part->get_py(), part->get_pz(), part->get_e());
    m_truthE.push_back(truth_momentum.E());
    m_truthEta.push_back(truth_momentum.PseudoRapidity());
    m_truthPhi.push_back(truth_momentum.Phi());
    m_truthPt.push_back(truth_momentum.Pt());
    m_truthMass.push_back(truth_momentum.M());

    // part->get_vtx_id() gives the *production* vertex, not the end vertex
    /* PHG4VtxPoint* end_vtx = truthInfo->GetVtx(part->get_vtx_id()); // DOES NOT WORK */
    // get the end vertex from one of the daughter particles
    PHG4VtxPoint* end_vtx = nullptr;
    int this_id = part->get_track_id();
    PHG4TruthInfoContainer::Range truthRange = truthInfo->GetSecondaryParticleRange();
    PHG4TruthInfoContainer::ConstIterator truthIter;
    for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
	PHG4Particle *decayPar = truthIter->second;
	int parent_id = decayPar->get_parent_id();
	if (parent_id == this_id) {
	    end_vtx = truthInfo->GetVtx(decayPar->get_vtx_id());
	    break;
	}
    }
    assert(end_vtx);
    m_truthEndVtx_x.push_back(end_vtx->get_x());
    m_truthEndVtx_y.push_back(end_vtx->get_y());
    m_truthEndVtx_z.push_back(end_vtx->get_z());
    m_truthEndVtx_t.push_back(end_vtx->get_t());
    float x = end_vtx->get_x();
    float y = end_vtx->get_y();
    float r = sqrt(x*x + y*y);
    m_truthEndVtx_r.push_back(r);

    secondaryBarcodes.push_back(part->get_barcode());
}

HepMC::GenParticle* getGenParticle(int barcode, HepMC::GenEvent* theEvent) {
    for(HepMC::GenEvent::particle_const_iterator p=theEvent->particles_begin(); p!=theEvent->particles_end(); ++p)
    {
	assert(*p);
	if ( (*p)->barcode() == barcode ) {
	    // found the right particle
	    return (*p);
	}
    }
    // reached end of loop... if we still haven't found the right particle,
    // we have a problem
    std::cout << "In getGenParticle(): could not find correct generated particle!\n";
    return nullptr;
}


PHG4VtxPoint* getG4EndVtx(int id, PHG4TruthInfoContainer* truthInfo) {
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
    return end_vtx;
}

bool pythiaEMCalAna::vector_contains(int val, std::vector<int> vec) {
    unsigned int s = vec.size();
    if ( s == 0 ) return false;
    unsigned int i;
    for (i=0; i<s; i++) {
	if (vec.at(i) == val) break;
    }
    if ( i == s ) return false;
    else return true;
}
