#include "pi0ClusterAna.h"

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
#include <g4vertex/GlobalVertex.h>
#include <g4vertex/GlobalVertexMap.h>

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
pi0ClusterAna::pi0ClusterAna(const std::string &name, const std::string &outName = "pi0ClusterAnaOut"):
SubsysReco(name)
  , clusters_Towers(nullptr)
  , truth_photon(nullptr)
  , truth_pi0(nullptr)
  //, caloevalstack(NULL)
  , m_eta_center()
  , m_phi_center()
  , m_tower_energy()
, m_cluster_eta()
  , m_cluster_phi()
, m_cluster_e()
  , m_cluster_chi2()
  , m_cluster_prob()
, m_cluster_nTowers()
  , m_asym()
  , m_deltaR()
  , m_lead_E()
, m_sublead_E()
, m_lead_phi()
, m_lead_eta()
  , m_sublead_phi()
  , m_sublead_eta()
  , m_pi0_E()
, m_pi0_eta()
, m_pi0_phi()
  , Outfile(outName)

{


  std::cout << "pi0ClusterAna::pi0ClusterAna(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
pi0ClusterAna::~pi0ClusterAna()
{
  std::cout << "pi0ClusterAna::~pi0ClusterAna() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int pi0ClusterAna::Init(PHCompositeNode *topNode)
{
  
  clusters_Towers = new TTree("TreeClusterTower","Tree for cluster and tower information");
  clusters_Towers -> Branch("towerEtaCEnter",&m_eta_center);
  clusters_Towers -> Branch("towerPhiCenter",& m_phi_center);
  clusters_Towers -> Branch("towerEnergy",&m_tower_energy);
  clusters_Towers -> Branch("clusterEta",&m_cluster_eta);
  clusters_Towers -> Branch("clusterPhi", &m_cluster_phi);
  clusters_Towers -> Branch("clusterE",&m_cluster_e);
  clusters_Towers -> Branch("clusterChi2",&m_cluster_chi2);
  clusters_Towers -> Branch("clusterProb",&m_cluster_prob);
  clusters_Towers -> Branch("clusterNTowers",&m_cluster_nTowers);

  truth_photon = new TTree("TreeTruthPhoton", "Tree for truth photons");
  truth_photon -> Branch("pairAsym",&m_asym);
  truth_photon -> Branch("pairDeltaR",&m_deltaR);
  truth_photon -> Branch("leadPhotonPhi",&m_lead_phi);
  truth_photon -> Branch("leadPhotonEta",&m_lead_eta);
  truth_photon -> Branch("subleadPhotonPhi",&m_sublead_phi);
  truth_photon -> Branch("subleadPhotonEta",&m_sublead_eta);
  truth_photon -> Branch("leadPhotonE", &m_lead_E);
  truth_photon -> Branch("subLeadPhotonE", &m_sublead_E);
  
  truth_pi0 = new TTree("TreeTruthPi0", "Tree for Truth pi0");
  truth_pi0 -> Branch("pi0E",&m_pi0_E);
  truth_pi0 -> Branch("pi0_phi",&m_pi0_phi);
  truth_pi0 -> Branch("pi0_eta",&m_pi0_eta);
    
  //so that the histos actually get written out
  Fun4AllServer *se = Fun4AllServer::instance();
  se -> Print("NODETREE"); 
  hm = new Fun4AllHistoManager("MYHISTOS");

  se -> registerHistoManager(hm);

  se -> registerHisto(truth_pi0 -> GetName(),truth_pi0);
  se -> registerHisto(truth_photon -> GetName(), truth_photon);
  se -> registerHisto(clusters_Towers -> GetName(), clusters_Towers);
  
  out = new TFile(Outfile.c_str(),"RECREATE");
  
  std::cout << "pi0ClusterAna::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::InitRun(PHCompositeNode *topNode)
{
  std::cout << "pi0ClusterAna::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::process_event(PHCompositeNode *topNode)
{

  //Information on clusters
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  //RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - CLUSTER_CEMC node is missing. " << std::endl;

      return 0;
    }


  //Vertex information
  GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vtxContainer);  // force quit

      return 0;
    }

  if (vtxContainer->empty())
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find vtx from vtxContainer"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Tower geometry node for location information
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, "TOWERGEOM_CEMC");
  if (!towergeom)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node TOWERGEOM_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //Raw tower information
  RawTowerContainer *towerContainer = findNode::getClass<RawTowerContainer>(topNode,"TOWER_CALIB_CEMC");
  if(!towerContainer)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node TOWER_CALIB_CEMC"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "pi0Efficiency::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
  
  
  float pi0Eta = -99999;
  float photonEtaMax = 1.1;
  float mesonEtaMax = 0.3;
  //Now we go and find our truth pi0s and just take its eta coordinate for now
  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;

  PHG4Particle *truthPar = NULL;

  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      truthPar = truthIter->second;
      
      if(truthPar -> get_pid() == 111 && truthPar -> get_parent_id() == 0) 
	{
	  pi0Eta = getEta(truthPar);
	  if(abs(pi0Eta) >= mesonEtaMax) 
	    {
	      return 0;
	    }
	}
    }

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
  
  
  TLorentzVector pi0;
  pi0.SetPxPyPzE(truthPar -> get_px(), truthPar -> get_py(), truthPar -> get_pz(), truthPar -> get_e());
  TLorentzVector leadPho, subLeadPho;
  if(photon1.Energy() >= photon2.Energy())
    {
      leadPho = photon1;
      subLeadPho = photon2;
    }
  else
    {
      leadPho = photon2;
      subLeadPho = photon1;
    }
  
  float asym = abs(photon1.Energy() - photon2.Energy())/(photon1.Energy() + photon2.Energy());

  m_asym.push_back(asym);
  
  float deltaR = photon1.DeltaR(photon2);

  m_deltaR.push_back(deltaR);

  m_lead_phi.push_back(leadPho.Phi());
  m_sublead_phi.push_back(subLeadPho.Phi());
  
  m_lead_eta.push_back(leadPho.PseudoRapidity());
  m_sublead_eta.push_back(subLeadPho.PseudoRapidity());
  
  m_lead_E.push_back(leadPho.Energy());
  m_sublead_E.push_back(subLeadPho.Energy());

  m_pi0_E.push_back(pi0.Energy());
  m_pi0_phi.push_back(pi0.Phi());
  m_pi0_eta.push_back(pi0.PseudoRapidity());
 
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
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      float clusE = E_vec_cluster.mag();
      float clus_eta = E_vec_cluster.pseudoRapidity();
      float clus_phi = E_vec_cluster.phi();
      
      m_cluster_eta.push_back(clus_eta);
      m_cluster_phi.push_back(clus_phi);
      m_cluster_e.push_back(clusE);
      
      m_cluster_chi2.push_back(recoCluster -> get_chi2());
      m_cluster_prob.push_back(recoCluster -> get_prob());
      m_cluster_nTowers.push_back(recoCluster -> getNTowers());
    }


  clusters_Towers -> Fill();
  truth_photon -> Fill();
  truth_pi0 -> Fill();
  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::ResetEvent(PHCompositeNode *topNode)
{
  //std::cout << "pi0ClusterAna::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;

  m_eta_center.clear();
  m_phi_center.clear();
  m_tower_energy.clear();
  m_cluster_eta.clear();
  m_cluster_phi.clear();
  m_cluster_e.clear();
  m_cluster_chi2.clear();
  m_cluster_prob.clear();
  m_cluster_nTowers.clear();
  m_asym.clear();
  m_deltaR.clear();
  m_lead_E.clear();
  m_sublead_E.clear();
  m_lead_phi.clear();
  m_lead_eta.clear();
  m_sublead_phi.clear();
  m_sublead_eta.clear();
  m_pi0_E.clear();
  m_pi0_eta.clear();
  m_pi0_phi.clear();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::EndRun(const int runnumber)
{
  std::cout << "pi0ClusterAna::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::End(PHCompositeNode *topNode)
{
  std::cout << "pi0ClusterAna::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  out -> cd();
  
  truth_pi0 -> Write();
  truth_photon -> Write();
  clusters_Towers -> Write();
  
  out -> Close();
  delete out; 

  hm -> dumpHistos(Outfile.c_str(),"UPDATE");


  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int pi0ClusterAna::Reset(PHCompositeNode *topNode)
{
  std::cout << "pi0ClusterAna::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void pi0ClusterAna::Print(const std::string &what) const
{
  std::cout << "pi0ClusterAna::Print(const std::string &what) const Printing info for " << what << std::endl;
}
//____________________________________________________________________________.. 
float pi0ClusterAna::getEta(PHG4Particle *particle)
{
  float px = particle -> get_px();
  float py = particle -> get_py();
  float pz = particle -> get_pz();
  float p = sqrt(pow(px,2) + pow(py,2) + pow(pz,2));

  return 0.5*log((p+pz)/(p-pz));
}
