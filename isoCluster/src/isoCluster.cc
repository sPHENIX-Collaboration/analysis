
//____________________________________________________________________________..

//our base code
#include "isoCluster.h"

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
isoCluster::isoCluster(const std::string &name):
SubsysReco(name),
  m_clusterPhi(0),
m_clusterEta(0),
  m_clusterE(0), 
m_clusterPt(0),
  m_clusterEtIso(0),
  m_clusterChisq(0),
  m_photonPhi(0),
  m_photonEta(0),
  m_photonE(0),
m_photonPt(0),
fout(NULL),
  outname(name),
  getEvent(-9999)
{
std::cout << "isoCluster::isoCluster(const std::string &name) Calling ctor" << std::endl;
}
//____________________________________________________________________________..
isoCluster::~isoCluster()
{
  std::cout << "isoCluster::~isoCluster() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int isoCluster::Init(PHCompositeNode *topNode)
{
  std::cout << "isoCluster::Init(PHCompositeNode *topNode) Initializing" << std::endl;
  
  fout = new TFile(outname.c_str(),"RECREATE");

  T = new TTree("T","T");

  T -> Branch("clusterPhi",&m_clusterPhi);
  T -> Branch("clusterEta",&m_clusterEta);
  T -> Branch("clusterE",&m_clusterE);
  T -> Branch("clusterPt",&m_clusterPt);
  T -> Branch("clusterEtIso",&m_clusterEtIso);
  T -> Branch("clusterchisq",&m_clusterChisq);
  
  T -> Branch("photonPhi",&m_photonPhi);
  T -> Branch("photonEta",&m_photonEta);
  T -> Branch("photonE",&m_photonE);
  T -> Branch("photonPt",&m_photonPt);
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::InitRun(PHCompositeNode *topNode)
{
  std::cout << "isoCluster::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::process_event(PHCompositeNode *topNode)
{
  //Information on clusters
  // RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_POS_COR_CEMC");
  RawClusterContainer *clusterContainer = findNode::getClass<RawClusterContainer>(topNode,"CLUSTER_CEMC");
  if(!clusterContainer)
    {
      std::cout << PHWHERE << "isoCluster::process_event - Fatal Error - CLUSTER_POS_COR_CEMC node is missing. " << std::endl;

      return 0;
    }

  //Vertex information
  GlobalVertexMap *vtxContainer = findNode::getClass<GlobalVertexMap>(topNode,"GlobalVertexMap");
  if (!vtxContainer)
    {
      std::cout << PHWHERE << "isoCluster::process_event - Fatal Error - GlobalVertexMap node is missing. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      assert(vtxContainer);  // force quit

      return 0;
    }

  if (vtxContainer->empty())
    {
      std::cout << PHWHERE << "isoCluster::process_event - Fatal Error - GlobalVertexMap node is empty. Please turn on the do_global flag in the main macro in order to reconstruct the global vertex." << std::endl;
      return 0;
    }

  //More vertex information
  GlobalVertex *vtx = vtxContainer->begin()->second;
  if(!vtx)
    {

      std::cout << PHWHERE << "isoCluster::process_event Could not find vtx from vtxContainer"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //truth particle information
  PHG4TruthInfoContainer *truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if(!truthinfo)
    {
      std::cout << PHWHERE << "isoCluster::process_event Could not find node G4TruthInfo"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //For eventgen ancestory information
  PHHepMCGenEventMap *genEventMap = findNode::getClass<PHHepMCGenEventMap>(topNode, "PHHepMCGenEventMap");
  if(!genEventMap)
    {
      std::cout << PHWHERE << "isoCluster::process_event Could not find PHHepMCGenEventMap"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  //event level information of the above
  PHHepMCGenEvent *genEvent = genEventMap -> get(getEvent);
  if(!genEvent)
    {
      std::cout << PHWHERE << "isoCluster::process_event Could not find PHHepMCGenEvent"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
   
  HepMC::GenEvent *theEvent = genEvent -> getEvent();

  CaloEvalStack caloevalstack(topNode, "CEMC");
  CaloRawClusterEval *clustereval = caloevalstack.get_rawcluster_eval();
  if(!clustereval)
    {
      std::cout << PHWHERE << "isoCluster::process_event cluster eval not available"  << std::endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    } 
  
  //grab all the clusters in the event
  RawClusterContainer::ConstRange clusterEnd = clusterContainer -> getClusters();
  RawClusterContainer::ConstIterator clusterIter;
  for(clusterIter = clusterEnd.first; clusterIter != clusterEnd.second; clusterIter++)
    {
      RawCluster *recoCluster = clusterIter -> second;
      CLHEP::Hep3Vector vertex(vtx->get_x(), vtx->get_y(), vtx->get_z());
      CLHEP::Hep3Vector E_vec_cluster = RawClusterUtility::GetECoreVec(*recoCluster, vertex);
      if(E_vec_cluster.mag() < 3) continue;
      m_clusterE.push_back(E_vec_cluster.mag());
      m_clusterEta.push_back(E_vec_cluster.pseudoRapidity());
      m_clusterPhi.push_back(E_vec_cluster.phi());
      m_clusterPt.push_back(E_vec_cluster.perp());
      m_clusterChisq.push_back(recoCluster -> get_chi2());
      m_clusterEtIso.push_back(recoCluster -> get_et_iso(3,1,1));
    }
  

  //grab all the direct photons in the event
  PHG4TruthInfoContainer::Range truthRange = truthinfo->GetPrimaryParticleRange();
  PHG4TruthInfoContainer::ConstIterator truthIter;
  //from the HepMC event log
  for(truthIter = truthRange.first; truthIter != truthRange.second; truthIter++)
    {
      //PHG4Particle* part = truthinfo->GetParticle(truthIter->second->get_trkid())
      
      PHG4Particle *truthPar = truthIter->second;
      int previousparentid = -999;
      if(truthPar -> get_pid() != 22) continue;//end with a photon

      
      int parentid = truthPar->get_parent_id();
      previousparentid = truthPar->get_track_id();
      while (parentid != 0)
	{
	  previousparentid = parentid;
	  PHG4Particle* mommypart = truthinfo->GetParticle(parentid);
	  parentid = mommypart->get_parent_id();
	}
      
      if(previousparentid > 0)truthPar = truthinfo -> GetParticle(previousparentid);
      if(truthPar -> get_pid() != 22) continue;//start with a photon

      for(HepMC::GenEvent::particle_const_iterator p = theEvent -> particles_begin(); p != theEvent -> particles_end(); ++p)
	{
	  assert(*p);
	   
	  if((*p) -> barcode() == truthPar -> get_barcode())
	    {
	      for(HepMC::GenVertex::particle_iterator mother = (*p)->production_vertex()->particles_begin(HepMC::parents);
		  mother != (*p)->production_vertex()->particles_end(HepMC::parents); ++mother)
		{
		  //std::cout << "Mother pid: " << (*mother) -> pdg_id() << std::endl;
		  //if((*mother) -> pdg_id() != 21 || (*mother) -> pdg_id() != 1 || (*mother) -> pdg_id() != 2) continue;
		  if((*mother) -> pdg_id() != 22) continue;

		  HepMC::FourVector moMomentum = (*mother) -> momentum();

		  m_photonE.push_back(moMomentum.e());
		  m_photonEta.push_back(moMomentum.pseudoRapidity());
		  m_photonPhi.push_back(moMomentum.phi());
		  m_photonPt.push_back(sqrt(moMomentum.px()*moMomentum.px()+moMomentum.py()*moMomentum.py()));
		  
		  
		}
		       
	    }
	}
    }

  
  T -> Fill();
  
return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::ResetEvent(PHCompositeNode *topNode)
{

  m_clusterPhi.clear();
  m_clusterEta.clear();
  m_clusterE.clear();
  m_clusterPt.clear();
  m_clusterEtIso.clear();
  m_clusterChisq.clear();
  m_photonPhi.clear();
  m_photonEta.clear();
  m_photonE.clear();
  m_photonPt.clear();
    
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::EndRun(const int runnumber)
{
  std::cout << "isoCluster::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::End(PHCompositeNode *topNode)
{
  std::cout << "isoCluster::End(PHCompositeNode *topNode) This is the End..." << std::endl;
  fout -> cd();
  T -> Write();
  fout -> Close();
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int isoCluster::Reset(PHCompositeNode *topNode)
{
  std::cout << "isoCluster::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void isoCluster::Print(const std::string &what) const
{
  std::cout << "isoCluster::Print(const std::string &what) const Printing info for " << what << std::endl;
}
