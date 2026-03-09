#include "singlePhotonClusterAna.h"

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

//for vetex information
#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>

//tracking
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

//truth information
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <HepMC/GenEvent.h>
#include <HepMC/GenParticle.h>
#include <HepMC/GenVertex.h>
#include <HepMC/IteratorRange.h>
#include <HepMC/SimpleVector.h> 
#include <HepMC/GenParticle.h>
#pragma GCC diagnostic pop
#include <CLHEP/Geometry/Point3D.h>


//____________________________________________________________________________..
singlePhotonClusterAna::singlePhotonClusterAna(const std::string &name, const std::string &outName = "singlePhotonClusterAnaOut"):
SubsysReco(name)
  , clusters_Towers(nullptr)
  , truth_photon(nullptr)
  , conversion(nullptr)
  //, caloevalstack(NULL)
  , m_eta_center()
  , m_phi_center()
  , m_tower_energy()
, m_cluster_eta()
  , m_cluster_phi()
, m_cluster_e()
, m_cluster_ecore()
  , m_cluster_chi2()
  , m_cluster_prob()
, m_cluster_nTowers()
  , m_photon_E()
, m_photon_eta()
, m_photon_phi()
  , m_electron_E()
, m_electron_eta()
, m_electron_phi()
  , m_positron_E()
, m_positron_eta()
, m_positron_phi()
  , m_vtx_x()
  , m_vtx_y()
  , m_vtx_z()
  , m_vtx_t()
  , m_vtx_r()
  , m_isConversionEvent()
  , Outfile(outName)

{


  std::cout << "singlePhotonClusterAna::singlePhotonClusterAna(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
singlePhotonClusterAna::~singlePhotonClusterAna()
{
  std::cout << "singlePhotonClusterAna::~singlePhotonClusterAna() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::Init(PHCompositeNode *topNode)
{
  
  clusters_Towers = new TTree("TreeClusterTower","Tree for cluster and tower information");
  clusters_Towers -> Branch("towerEtaCEnter",&m_eta_center);
  clusters_Towers -> Branch("towerPhiCenter",& m_phi_center);
  clusters_Towers -> Branch("towerEnergy",&m_tower_energy);
  clusters_Towers -> Branch("clusterEta",&m_cluster_eta);
  clusters_Towers -> Branch("clusterPhi", &m_cluster_phi);
  clusters_Towers -> Branch("clusterE",&m_cluster_e);
  clusters_Towers -> Branch("clusterEcore",&m_cluster_ecore);
  clusters_Towers -> Branch("clusterChi2",&m_cluster_chi2);
  clusters_Towers -> Branch("clusterProb",&m_cluster_prob);
  clusters_Towers -> Branch("clusterNTowers",&m_cluster_nTowers);

  truth_photon = new TTree("TreeTruthPhoton", "Tree for Truth Photon");
  truth_photon -> Branch("photonE",&m_photon_E);
  truth_photon -> Branch("photon_phi",&m_photon_phi);
  truth_photon -> Branch("photon_eta",&m_photon_eta);
    
  conversion = new TTree("TreeConversion", "Tree for Photon Conversion Info");
  conversion -> Branch("electronE",&m_electron_E);
  conversion -> Branch("electron_phi",&m_electron_phi);
  conversion -> Branch("electron_eta",&m_electron_eta);
  conversion -> Branch("positronE",&m_positron_E);
  conversion -> Branch("positron_phi",&m_positron_phi);
  conversion -> Branch("positron_eta",&m_positron_eta);
  conversion -> Branch("vtx_x",&m_vtx_x);
  conversion -> Branch("vtx_y",&m_vtx_y);
  conversion -> Branch("vtx_z",&m_vtx_z);
  conversion -> Branch("vtx_t",&m_vtx_t);
  conversion -> Branch("vtx_r",&m_vtx_r);
  conversion -> Branch("isConversionEvent",&m_isConversionEvent);
    
  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  hm = new Fun4AllHistoManager("MYHISTOS");

  se -> registerHistoManager(hm);

  se -> registerHisto(truth_photon -> GetName(), truth_photon);
  se -> registerHisto(clusters_Towers -> GetName(), clusters_Towers);
  
  out = new TFile(Outfile.c_str(),"RECREATE");
  
  std::cout << "singlePhotonClusterAna::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::InitRun(PHCompositeNode *topNode)
{
  std::cout << "singlePhotonClusterAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::process_event(PHCompositeNode *topNode)
{

  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;

      return 0;
    }


  /* //Vertex information */
  GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxContainer)
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vtxContainer);  // force quit

      return 0;
    }

  if (vtxContainer->empty())
    {
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find vtx from vtxContainer"  << std::endl;
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
      std::cout << PHWHERE << "singlePhotonClusterAna::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  
  float photonEta = -99999;
  float photonEtaMax = 1.1;
  TLorentzVector photon;
  //Now we go and find our truth photon and just take its eta coordinate for now
  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;

  PHG4Particle *truthPar = NULL;

  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      truthPar = truthIter->second;
      
      // photon pid is 22
      if(truthPar -> get_pid() == 22 && truthPar -> get_parent_id() == 0) 
	{
	  photonEta = getEta(truthPar);
	  if(abs(photonEta) >= photonEtaMax) 
	    {
	      return 0;
	    }
	  /* std::cout << "\n\nGreg info:\n\nFound truth photon.\n"; */
	  /* std::cout << "Truth photon E = " << truthPar->get_e() << "; eta = " << getEta(truthPar) << "\n"; */
	  /* std::cout << "truthPar = " << truthPar << "\n"; */
	  /* std::cout << "truthPar->get_primary_id() = " << truthPar->get_primary_id() << "\n"; */
	  /* std::cout << "truthPar->get_pid() = " << truthPar->get_pid() << "\n"; */
	  /* std::cout << "truthPar->get_vtx_id() = " << truthPar->get_vtx_id() << "\n"; */
	  /* std::cout << "truthPar->get_track_id() = " << truthPar->get_track_id() << "\n"; */
	  photon.SetPxPyPzE(truthPar -> get_px(), truthPar -> get_py(), truthPar -> get_pz(), truthPar -> get_e());
	}
    }

  // Check the daughter particles of truth photon
  int decay_vtx_idx = 0;
  int decay_vtx_idx1 = 0; // first decay particle vertex
  int decay_vtx_idx2 = 0; // second decay particle vertex; SHOULD be the same!
  bool first_decay_particle = false;
  bool second_decay_particle = false;
  bool decay_electron = false;
  bool decay_positron = false;
  int n_children = 0;
  TLorentzVector electron;
  TLorentzVector positron;
  bool isGoodEvent = true;

  PHG4TruthInfoContainer::Range truthRangeDecay1 = truthinfo->GetSecondaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIterDecay1;

  // First just check that the truth photon has exactly 2 daughters
  // and they share the same vertex
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      PHG4Particle *decay = truthIterDecay1 -> second;
      
      int dumparentid = decay -> get_parent_id();
      if(dumparentid == 1) {
	n_children++;
	if (! first_decay_particle) {
	    first_decay_particle = true;
	    decay_vtx_idx1 = decay->get_vtx_id();
	}
	else {
	    second_decay_particle = true;
	    decay_vtx_idx2 = decay->get_vtx_id();
	}
	/* std::cout << "\nDecay particle E = " << decay->get_e() << "; eta = " << getEta(decay) << "\n"; */
	/* std::cout << "decay = " << decay << "\n"; */
	/* std::cout << "decay->get_primary_id() = " << decay->get_primary_id() << "\n"; */
	/* std::cout << "decay->get_pid() = " << decay->get_pid() << "\n"; */
	/* std::cout << "decay->get_vtx_id() = " << decay->get_vtx_id() << "\n"; */
	/* std::cout << "decay->get_track_id() = " << decay->get_track_id() << "\n"; */
      }
    }

  if (n_children != 2) isGoodEvent = false;
  if (!(first_decay_particle && second_decay_particle)) isGoodEvent = false;
  if (decay_vtx_idx1 != decay_vtx_idx2) isGoodEvent = false;

  // Now get decay electron and positron kinematics
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      PHG4Particle *decay = truthIterDecay1 -> second;
      
      int dumtruthpid = decay -> get_pid();
      int dumparentid = decay -> get_parent_id();
      if(dumparentid == 1) {
	if (dumtruthpid == 11) {
	    // electron
	    decay_electron = true;
	    electron.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e());
	}
	if (dumtruthpid == -11) {
	    // positron
	    decay_positron = true;
	    positron.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e());
	}
      }
    }

  if (!(decay_electron && decay_positron)) isGoodEvent = false;

  // Conversion vertex information
  /* std::cout << "\nVertex information:\n"; */
  /* PHG4VtxPoint* photon_vtx = truthinfo->GetVtx(photon_vxt_idx); */
  /* std::cout << "\ntruthinfo->is_primary_vtx(photon_vtx) = " << truthinfo->is_primary_vtx(photon_vtx) << "\n"; */
  /* std::cout << "photon_vtx->get_id() = " << photon_vtx->get_id() << "\n"; */
  /* std::cout << "photon_vtx (x,y,z,t) = (" << photon_vtx->get_x() << ", " << photon_vtx->get_y() << ", " << photon_vtx->get_z() << ", " << photon_vtx->get_t() << ")\n"; */
  if (isGoodEvent) {
      m_photon_E.push_back(photon.Energy());
      m_photon_eta.push_back(photon.PseudoRapidity());
      m_photon_phi.push_back(photon.Phi());
      m_electron_E.push_back(electron.Energy());
      m_electron_eta.push_back(electron.PseudoRapidity());
      m_electron_phi.push_back(electron.Phi());
      m_positron_E.push_back(positron.Energy());
      m_positron_eta.push_back(positron.PseudoRapidity());
      m_positron_phi.push_back(positron.Phi());

      decay_vtx_idx = decay_vtx_idx1;
      PHG4VtxPoint* decay_vtx = truthinfo->GetVtx(decay_vtx_idx);
      m_vtx_x.push_back(decay_vtx->get_x());
      m_vtx_y.push_back(decay_vtx->get_y());
      m_vtx_z.push_back(decay_vtx->get_z());
      m_vtx_t.push_back(decay_vtx->get_t());
      float vtx_x = decay_vtx->get_x();
      float vtx_y = decay_vtx->get_y();
      float vtx_r = sqrt(vtx_x*vtx_x + vtx_y*vtx_y);
      m_vtx_r.push_back(vtx_r);
      bool isConversionEvent = false;
      float EMCal_inner_radius = 95; // nominal radius is 97.5cm
      if (vtx_r < EMCal_inner_radius) isConversionEvent = true;
      m_isConversionEvent.push_back(isConversionEvent);
      /* std::cout << "\ntruthinfo->is_primary_vtx(decay_vtx) = " << truthinfo->is_primary_vtx(decay_vtx) << "\n"; */
      /* std::cout << "decay_vtx->get_id() = " << decay_vtx->get_id() << "\n"; */
      /* std::cout << "decay_vtx (x,y,z,t) = (" << decay_vtx->get_x() << ", " << decay_vtx->get_y() << ", " << decay_vtx->get_z() << ", " << decay_vtx->get_t() << ")\n"; */
  }
  else {
      std::cout << "\nGreg info:\nBad conversion event\n";
      std::cout << "n_children = " << n_children << "\n";
      std::cout << "first_decay_particle = " << first_decay_particle << "; second_decay_particle = " << second_decay_particle << "\n";
      std::cout << "decay_vtx_idx1 = " << decay_vtx_idx1 << "; decay_vtx_idx2 = " << decay_vtx_idx2 << "\n";
      std::cout << "decay_electron = " << decay_electron << "; decay_positron = " << decay_positron << "\n";
      return 0;
  }

  // Anthony's code for pi0 decays
  /*

  int firstphotonflag = 0;
  int firstfirstphotonflag = 0;
  int secondphotonflag = 0;
 
  //int secondsecondphotonflag = 0;
  
  PHG4TruthInfoContainer::Range truthRangeDecay1 = truthinfo->GetSecondaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIterDecay1;

  
  TLorentzVector photon1;
  TLorentzVector photon2;
  int nParticles = 0;
  
  //loop over all our decay photons. 
  //Make sure they fall within the desired acceptance
  //Toss Dalitz decays
  //Retain photon kinematics to determine lead photon for eta binning
  for(truthIterDecay1 = truthRangeDecay1.first; truthIterDecay1 != truthRangeDecay1.second; truthIterDecay1++)
    {
      PHG4Particle *decay = truthIterDecay1 -> second;
      
      int dumtruthpid = decay -> get_pid();
      int dumparentid = decay -> get_parent_id();
      
      //if the parent is the pi0 and it's a photon and we haven't marked one yet
      if(dumparentid == 1 && dumtruthpid == 22 && !firstphotonflag)
	{
	  if(abs(getEta(decay)) > photonEtaMax) 
	    {
	      return 0;
	    }
	  photon1.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e());
	  
	  firstphotonflag = 1;
	}
      
      if(dumparentid == 1 && dumtruthpid == 22 && firstphotonflag && firstfirstphotonflag)
	{
	  if(abs(getEta(decay)) > photonEtaMax) 
	    {
	      return 0;
	    }
	  photon2.SetPxPyPzE(decay -> get_px(), decay -> get_py(), decay -> get_pz(), decay -> get_e()) ;
	  
	  secondphotonflag = 1;
	}

      //Need this flag to make it skip the first photon slot
      if(firstphotonflag) firstfirstphotonflag = 1;
      if(dumparentid == 1)nParticles ++; 
    }

  if((!firstphotonflag || !secondphotonflag) && nParticles > 1) //Dalitz
    {
      return 0;
    }
  else if((!firstphotonflag || !secondphotonflag) && nParticles == 1) //One photon falls outside simulation acceptance
    {
      return 0;
    }
  else if((!firstphotonflag || !secondphotonflag) && nParticles == 0) //Both photons fall outside simulation acceptance
    {
      return 0;
    }
  */ // End check on decay photons
  
  //grab all the towers and fill their energies. 
  RawTowerContainer::ConstRange tower_range = towerContainer -> getTowers();
  for(RawTowerContainer::ConstIterator tower_iter = tower_range.first; tower_iter!= tower_range.second; tower_iter++)
    {
      int phibin = tower_iter -> second -> get_binphi();
      int etabin = tower_iter -> second -> get_bineta();
      double phi = towergeom -> get_phicenter(phibin);
      double eta = towergeom -> get_etacenter(etabin);
      double energy = tower_iter -> second -> get_energy();

      m_phi_center.push_back(phi);
      m_eta_center.push_back(eta);
      m_tower_energy.push_back(energy);
    }
  
  //This is how we iterate over items in the container.
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
      
      m_cluster_eta.push_back(clus_eta);
      m_cluster_phi.push_back(clus_phi);
      m_cluster_e.push_back(clusE);
      m_cluster_ecore.push_back(clusEcore);
      
      m_cluster_chi2.push_back(recoCluster -> get_chi2());
      m_cluster_prob.push_back(recoCluster -> get_prob());
      m_cluster_nTowers.push_back(recoCluster -> getNTowers());
    }


  clusters_Towers -> Fill();
  truth_photon -> Fill();
  conversion -> Fill();
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "singlePhotonClusterAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  m_eta_center.clear();
  m_phi_center.clear();
  m_tower_energy.clear();
  m_cluster_eta.clear();
  m_cluster_phi.clear();
  m_cluster_e.clear();
  m_cluster_chi2.clear();
  m_cluster_prob.clear();
  m_cluster_nTowers.clear();
  m_photon_E.clear();
  m_photon_eta.clear();
  m_photon_phi.clear();
  m_electron_E.clear();
  m_electron_eta.clear();
  m_electron_phi.clear();
  m_positron_E.clear();
  m_positron_eta.clear();
  m_positron_phi.clear();
  m_vtx_x.clear();
  m_vtx_y.clear();
  m_vtx_z.clear();
  m_vtx_t.clear();
  m_vtx_r.clear();
  m_isConversionEvent.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::EndRun(const int runnumber)
{
  std::cout << "singlePhotonClusterAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::End(PHCompositeNode *topNode)
{
  std::cout << "singlePhotonClusterAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  out -> cd();
  
  truth_photon -> Write();
  clusters_Towers -> Write();
  conversion -> Write();
  
  out -> Close();
  delete out; 

  hm -> dumpHistos(Outfile.c_str(),"UPDATE");


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int singlePhotonClusterAna::Reset(PHCompositeNode *topNode)
{
  std::cout << "singlePhotonClusterAna::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void singlePhotonClusterAna::Print(const std::string &what) const
{
  std::cout << "singlePhotonClusterAna::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________.. 
float singlePhotonClusterAna::getEta(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return 0.5*log((p+pz)/(p-pz));
}
