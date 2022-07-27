#include "ExclusiveReco.h"
#include "TruthTrackerHepMC.h"
#include "TrackProjectorPlaneECAL.h"
#include "DVMPHelper.h"
/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <trackbase_historic/SvtxTrackMap_v1.h>

#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerv1.h>
#include <calobase/RawTowerDefs.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <g4eval/CaloEvalStack.h>

#include <g4main/PHG4Particle.h>


/* ROOT includes */
#include <TString.h>
#include <TTree.h>
#include <TFile.h>

using namespace std;

ExclusiveReco::ExclusiveReco(std::string filename) :
  SubsysReco("ExclusiveReco" ),
  _mproton( 0.938272 ),
  _save_towers(false),
  _save_tracks(false),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _tree_invariant_mass(nullptr),
  _tree_event_reco(nullptr),
  _tree_event_truth(nullptr),
  _beam_electron_ptotal(10),
  _beam_hadron_ptotal(250),
  _trackproj(nullptr)
{

}

int
ExclusiveReco::Init(PHCompositeNode *topNode)
{
  _topNode = topNode;

  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* Create tree for information about full event */
  _tree_invariant_mass = new TTree("event_exclusive", "A tree with exclusive event information");
  _tree_invariant_mass->Branch("event", &_ievent, "event/I");

  _tree_invariant_mass->Branch("reco_inv", &_vect1);
  _tree_invariant_mass->Branch("true_inv", &_vect2);
  _tree_invariant_mass->Branch("reco_inv_decay", &_vect3);
  _tree_invariant_mass->Branch("reco_inv_scatter", &_vect4);
  _tree_invariant_mass->Branch("true_inv_decay", &_vect5);
  _tree_invariant_mass->Branch("true_inv_scatter", &_vect6);

  /* Create tree for information about reco event */
  _tree_event_reco = new TTree("event_reco", "A tree with reco particle information");
  _tree_event_reco->Branch("event", &_ievent, "event/I");
  _tree_event_reco->Branch("eta", &reco_eta);
  _tree_event_reco->Branch("phi", &reco_phi);
  _tree_event_reco->Branch("ptotal", &reco_ptotal);
  _tree_event_reco->Branch("charge", &reco_charge);
  _tree_event_reco->Branch("cluster_e", &reco_cluster_e);
  _tree_event_reco->Branch("is_scattered_lepton", &reco_is_scattered_lepton);
  
  
  /* Create tree for information about reco event */
  _tree_event_truth = new TTree("event_truth", "A tree with truth particle information");
  _tree_event_truth->Branch("event", &_ievent, "event/I");
  _tree_event_truth->Branch("geta", &true_eta);
  _tree_event_truth->Branch("gphi", &true_phi);
  _tree_event_truth->Branch("gptotal", &true_ptotal);
  _tree_event_truth->Branch("pid",&true_pid);
  _tree_event_truth->Branch("is_scattered_lepton",&is_scattered_lepton);
  
  return 0;
}

int
ExclusiveReco::InitRun(PHCompositeNode *topNode)
{
   _trackproj=new TrackProjectorPlaneECAL( topNode );
   return 0;
}

int
ExclusiveReco::process_event(PHCompositeNode *topNode)
{
  /* Calculate invariant mass of a DVMP event */
  if ( _do_process_dvmp )
    {
      AddInvariantMassInformation();
    }
  /* count up event number */
  _ievent ++;

  return 0;
}

int
ExclusiveReco::AddInvariantMassInformation()
{
  /* First, add truth particle information */
  // ------------------------------------------------------------------------//
  //true vectors are defined in header file
  true_eta.clear(); true_phi.clear(); true_ptotal.clear(); true_pid.clear(); is_scattered_lepton.clear();
  // -----------------------------------------------------------------------//
  /* Get collection of truth particles from event generator */
  PHHepMCGenEventMap *geneventmap = findNode::getClass<PHHepMCGenEventMap>(_topNode,"PHHepMCGenEventMap");
  if (!geneventmap) {
    std::cout << PHWHERE << " WARNING: Can't find requested PHHepMCGenEventMap" << endl;
    return -1;
  }

  /* Add truth kinematics */
  int embedding_id = 1;
  PHHepMCGenEvent *genevt = geneventmap->get(embedding_id);
  if (!genevt)
    {
      std::cout << PHWHERE << "WARNING: Node PHHepMCGenEventMap missing subevent with embedding ID "<< embedding_id;
      std::cout <<". Print PHHepMCGenEventMap:";
      geneventmap->identify();
      return -1;
    }

  HepMC::GenEvent* theEvent = genevt->getEvent();

  if ( !theEvent )
    {
      std::cout << PHWHERE << "WARNING: Missing requested GenEvent!" << endl;
      return -1;
    }

  /* Look for scattered lepton */
  TruthTrackerHepMC truth;
  truth.set_hepmc_geneventmap( geneventmap );
  HepMC::GenParticle* particle_scattered_l = truth.FindScatteredLepton();
  /* loop over all particles */
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    { 
      /* skip particles that are not stable final state particles (status 1) */
      if ( (*p)->status() != 1 )
        continue;

      float mom_eta = -1 * log ( tan( (*p)->momentum().theta() / 2.0 ) );

      true_eta.push_back(mom_eta);
      true_phi.push_back( (*p)->momentum().phi() );
      true_ptotal.push_back( (*p)->momentum().e() );
      true_pid.push_back( (*p)->pdg_id() );
      
      if ( particle_scattered_l &&
	   (*p) == particle_scattered_l )
	is_scattered_lepton.push_back(true);
      else
	is_scattered_lepton.push_back(false);
    }
  /* Second, add reconstructed particle information */
  // --------------------------------------------------------------------------
  //reco vectors are defined in header file
  reco_eta.clear(); reco_phi.clear(); reco_ptotal.clear(); reco_cluster_e.clear(); reco_charge.clear(); reco_is_scattered_lepton.clear();
  // --------------------------------------------------------------------------
  vector< string > v_ecals;
  v_ecals.push_back("EEMC");
  v_ecals.push_back("CEMC");
  v_ecals.push_back("FEMC");
  for ( unsigned idx = 0; idx < v_ecals.size(); idx++ )
    {
      string clusternodename = "CLUSTER_" + v_ecals.at( idx );
      RawClusterContainer *clusterList = findNode::getClass<RawClusterContainer>(_topNode,clusternodename.c_str());
      SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(_topNode,"SvtxTrackMap");
      if (!clusterList) {
        cerr << PHWHERE << " ERROR: Can't find node " << clusternodename << endl;
        return false;
      }
      if(!trackmap) {
	cerr << PHWHERE << " ERROR: Can't find node SvtxTrackMap" << endl;
	return false;
      }

      for (unsigned int k = 0; k < clusterList->size(); ++k)
        {
          RawCluster *cluster = clusterList->getCluster(k);
          /* Check if cluster energy is below threshold */
          float e_cluster_threshold = 0.3;
          if ( cluster->get_energy() < e_cluster_threshold )
            continue;

	  _trackproj->set_detector(_trackproj->get_detector_from_cluster(cluster));
	  
	  SvtxTrack *best_track = _trackproj->get_best_track(trackmap,cluster);
	  if(best_track!=NULL)
	    {
	      reco_eta.push_back(best_track->get_eta());
	      reco_phi.push_back(best_track->get_phi());
	      // See if particle is in EEMC, if so, swap ptotal with cluster e
	      RawCluster::TowerConstIterator rtiter = cluster->get_towers().first;
	      const char * cc = RawTowerDefs::convert_caloid_to_name( RawTowerDefs::decode_caloid( rtiter->first )).c_str();
	      if(strcmp(cc,"EEMC")==0)
		{
		  reco_ptotal.push_back(cluster->get_energy());
		}
	      else
		{
		  reco_ptotal.push_back(best_track->get_p());
		}
	      reco_charge.push_back(best_track->get_charge());
	      reco_cluster_e.push_back(cluster->get_energy());
	   
	      // Look at every truth particle
	      for(unsigned i = 0 ; i < true_eta.size() ; i++)
		{
		  // Compute difference in reco particle eta and phi w/ true
		  float eta_diff = abs(best_track->get_eta()-true_eta.at(i));
		  float phi_diff = abs(best_track->get_phi()-true_phi.at(i));
		  
		  // If the differences in these values is very small
		  if(eta_diff<0.1&&phi_diff<0.1)
		    {
		      bool b = is_scattered_lepton.at(i)&&best_track->get_charge()==-1;
		      if(b)
			reco_is_scattered_lepton.push_back(true);
		      else
			reco_is_scattered_lepton.push_back(false);
		    }
		  else
		    {
		      reco_is_scattered_lepton.push_back(false);
		    }
		}
	    }
	  else
	    {
	      reco_eta.push_back(NAN);
	      reco_phi.push_back(NAN);
	      reco_ptotal.push_back(NAN);
	      reco_charge.push_back(0);
	      reco_cluster_e.push_back(NAN);
	      reco_is_scattered_lepton.push_back(false);
	    }
	} 
    }

  // At this point, we have all the truth and reco event information we need to fiddle around with measuring the invariant mass //
  DVMPHelper * dvmp = new DVMPHelper(reco_eta,reco_phi,reco_ptotal,reco_charge,reco_cluster_e,reco_is_scattered_lepton,true_eta,true_phi,true_ptotal,true_pid,is_scattered_lepton);

  // 1) Invariant Mass of all reconstructed e- e+ pairs
  // 2) Invariant Mass of all truth e- e+ pairs
  // 3) Invariant Mass of reco decay e- e+ pairs (**USES TRUTH INFO**)
  // 4) Invariant Mass of reco scattered e- e+ pairs (**USES TRUTH INFO**)
  // 5) Invariant Mass of truth decay e- e+ pairs
  // 6) Invariant Mass of truth scattered e- e+ pairs

  _vect1 = dvmp->calculateInvariantMass_1();
  _vect2 = dvmp->calculateInvariantMass_2();
  _vect3 = dvmp->calculateInvariantMass_3();
  _vect4 = dvmp->calculateInvariantMass_4();
  _vect5 = dvmp->calculateInvariantMass_5();
  _vect6 = dvmp->calculateInvariantMass_6();

  _tree_event_reco->Fill();
  _tree_event_truth->Fill();
  _tree_invariant_mass->Fill();
  return 0;
}

int
ExclusiveReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();

  if ( _tree_invariant_mass )
    {
      _tree_event_reco->Write();
      _tree_event_truth->Write();
      _tree_invariant_mass->Write();
    }

  _tfile->Close();

  return 0;
}
