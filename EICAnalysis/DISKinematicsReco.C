#include "DISKinematicsReco.h"
#include "PidCandidatev1.h"
#include "TruthTrackerHepMC.h"
#include "TrackProjectionTools.h"
#include "TrackProjectorPlaneECAL.h"
/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack_FastSim.h>

#include <trackbase_historic/SvtxTrackMap_v1.h>

#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <g4jets/JetMap.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerv1.h>
#include <calobase/RawTowerDefs.h>

#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4main/PHG4Particle.h>

#include <g4eval/CaloEvalStack.h>
#include <g4eval/CaloRawTowerEval.h>

/* ROOT includes */
#include <TLorentzVector.h>
#include <TString.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TFile.h>
#include <TDatabasePDG.h>

using namespace std;

DISKinematicsReco::DISKinematicsReco(std::string filename) :
  SubsysReco("DISKinematicsReco" ),
  _mproton( 0.938272 ),
  _save_towers(false),
  _save_tracks(false),
  _do_process_geant4_cluster(false),
  _do_process_truth(false),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _tree_event_cluster(nullptr),
  _tree_event_truth(nullptr),
  _beam_electron_ptotal(10),
  _beam_hadron_ptotal(250),
  _trackproj(nullptr)
{

}

int
DISKinematicsReco::Init(PHCompositeNode *topNode)
{
  _topNode = topNode;

  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* Add PidCandidate properties to map that defines output tree */
  float dummy = 0;
  vector< float > vdummy;

  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_id , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_prob , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_posx , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_posy , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_posz , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_ecore , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_et_iso , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_eta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_pt , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_ntower , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_cluster_caloid , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_id , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_quality , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_eta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_ptotal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_ptrans , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_charge , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_dca , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_section , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_cemc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_femc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_eemc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_ihcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_ohcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_fhcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_e3x3_ehcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_cluster_dr , vdummy ) );
  
   _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_theta2cluster , vdummy ) );
   _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_eta2cluster , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_phi2cluster , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_p2cluster , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_x2cluster , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_y2cluster , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_track_z2cluster , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_pid_prob_electron , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_pid_prob_pion , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_pid_prob_kaon , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_pid_prob_proton , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_pid , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_ptotal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_eta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_charge , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_evtgen_is_scattered_lepton , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_reco_x_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_reco_y_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_reco_q2_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( PidCandidate::em_reco_w_e , vdummy ) );

  /* Add branches to map that defines output tree for event-wise properties */
  /*
    runnr
    eventnr
    eventweight
    cal_px
    cal_py
    cal_pz
    cal_etotal
    cal_et
    cal_empz
    cal_pt --> "pt_miss" ?
    cal_pt_phi = atan2(sum(py)/sum(px)) --> "pt_miss_angle" ?
  */
  _map_event_branches.insert( make_pair( "em_ncandidates" , dummy ) );

  _map_event_branches.insert( make_pair( "Et_miss" , dummy ) );
  _map_event_branches.insert( make_pair( "Et_miss_phi" , dummy ) );

  _map_event_branches.insert( make_pair( "evtgen_process_id" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_s" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_x" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_Q2" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_y" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_W" , dummy ) );

  /* Create tree for information about full event */
  _tree_event_cluster = new TTree("event_cluster", "a Tree with global event information and EM candidates");
  _tree_event_cluster->Branch("event", &_ievent, "event/I");

  /* Add event branches */
  for ( map< string , float >::iterator iter = _map_event_branches.begin();
        iter != _map_event_branches.end();
        ++iter)
    {
      _tree_event_cluster->Branch( (iter->first).c_str(),
                                   &(iter->second) );
    }

  /* Add EM candidate branches */
  for ( map< PidCandidate::PROPERTY , vector<float> >::iterator iter = _map_em_candidate_branches.begin();
        iter != _map_em_candidate_branches.end();
        ++iter)
    {
      _tree_event_cluster->Branch( PidCandidate::get_property_info( (iter->first) ).first.c_str(),
                                   &(iter->second) );
    }

  /* create clone of tree for truth particle candidates */
  _tree_event_truth = _tree_event_cluster->CloneTree();
  _tree_event_truth->SetName("event_truth");
  _tree_event_truth->SetTitle("a Tree with global event information and truth particle candidates");
  
 
  return 0;
}

int
DISKinematicsReco::InitRun(PHCompositeNode *topNode)
{
  if(_do_process_geant4_cluster)
    _trackproj=new TrackProjectorPlaneECAL( topNode );
   return 0;
}

int
DISKinematicsReco::process_event(PHCompositeNode *topNode)
{
  /* Find electron candidates based on calorimeter cluster from full Geant4 detector simulation */
  if ( _do_process_geant4_cluster )
    {
      /* Reset branch map */
      ResetBranchMap();

      /* Create map to collect electron candidates.
       * Use energy as 'key' to the map because energy is unique for each jet, while there are sometimes multiple jets (same energy,
       * different jet ID) in the input jet collection. Also, map automatically sorts entries by key, i.e. this gives list of tau candidates
       * sorted by energy. */
      type_map_tcan electronCandidateMap;

      /* Collect EM candidates from calorimeter cluster */
      CollectEmCandidatesFromCluster( electronCandidateMap );

      /* Calculate kinematics for each em candidate */
      AddReconstructedKinematics( electronCandidateMap, "cluster" );

      /* Add information about em candidats to output tree */
      WriteCandidatesToTree( electronCandidateMap );

      /* Add global event information */
      AddGlobalCalorimeterInformation();
      AddTruthEventInformation();

      /* fill event information tree */
      _tree_event_cluster->Fill();
    }

  /* Find electron candidates based on truth particle inforamtion */
  if ( _do_process_truth )
    {
      /* reset branches, fill with truth particle candidates, and fill different tree */
      ResetBranchMap();

      /* Collect electron candidates based on TRUTH information */
      type_map_tcan electronTruthCandidateMap;

      CollectEmCandidatesFromTruth( electronTruthCandidateMap );
      AddReconstructedKinematics( electronTruthCandidateMap, "truth" );
      WriteCandidatesToTree( electronTruthCandidateMap );
      AddTruthEventInformation();

      /* fill event information tree */
      _tree_event_truth->Fill();
    }

  /* count up event number */
  _ievent ++;

  return 0;
}



int
DISKinematicsReco::CollectEmCandidatesFromCluster( type_map_tcan& electronCandidateMap )
{
 
  /* Loop over all clusters in EEMC, CEMC, and FEMC to find electron candidates */
  vector< string > v_ecals;
  v_ecals.push_back("EEMC");
  v_ecals.push_back("CEMC");
  v_ecals.push_back("FEMC");
  for ( unsigned idx = 0; idx < v_ecals.size(); idx++ )
    {
      CaloEvalStack * caloevalstack = new CaloEvalStack(_topNode, v_ecals.at( idx ) );

      string clusternodename = "CLUSTER_" + v_ecals.at( idx );
      RawClusterContainer *clusterList = findNode::getClass<RawClusterContainer>(_topNode,clusternodename.c_str());
      SvtxTrackMap *trackmap = findNode::getClass<SvtxTrackMap>(_topNode,"TrackMap");

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

          InsertCandidateFromCluster( electronCandidateMap , cluster , caloevalstack , trackmap);
        }
    }
        
  return 0;
}


int
DISKinematicsReco::CollectEmCandidatesFromTruth( type_map_tcan& candidateMap )
{
  /* Get collection of truth particles from event generator */
  PHHepMCGenEventMap *geneventmap = findNode::getClass<PHHepMCGenEventMap>(_topNode,"PHHepMCGenEventMap");
  if (!geneventmap) {
    //std::cout << PHWHERE << " WARNING: Can't find requested PHHepMCGenEventMap" << endl;
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

  /* Look for beam particles and scattered lepton */
  TruthTrackerHepMC truth;
  truth.set_hepmc_geneventmap( geneventmap );
  //HepMC::GenParticle* particle_beam_l = truth.FindBeamLepton();
  //HepMC::GenParticle* particle_beam_h = truth.FindBeamHadron();
  HepMC::GenParticle* particle_scattered_l = truth.FindScatteredLepton();
  /* loop over all particles */
  for (HepMC::GenEvent::particle_const_iterator p = theEvent->particles_begin();
       p != theEvent->particles_end(); ++p)
    {
      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*p)->pdg_id() );
      int charge = -999;
      if ( pdg_p )
	{
	  /* NOTE: TParticlePDG::Charge() returns charge in units of |e|/3 (see ROOT documentation) */
	  charge = pdg_p->Charge() / 3;
	}

      /* skip particles that are not stable final state particles (status 1) */
      if ( (*p)->status() != 1 )
        continue;
      /* create new pid candidate */
      PidCandidatev1 *tc = new PidCandidatev1();
      tc->set_candidate_id( candidateMap.size()+1 );

      float mom_eta = -1 * log ( tan( (*p)->momentum().theta() / 2.0 ) );

      tc->set_property( PidCandidate::em_evtgen_pid, (*p)->pdg_id() );
      tc->set_property( PidCandidate::em_evtgen_ptotal, (float) (*p)->momentum().e() );
      tc->set_property( PidCandidate::em_evtgen_theta, (float) (*p)->momentum().theta() );
      tc->set_property( PidCandidate::em_evtgen_phi, (float) (*p)->momentum().phi() );
      tc->set_property( PidCandidate::em_evtgen_eta, mom_eta );
      tc->set_property( PidCandidate::em_evtgen_charge, charge );
      tc->set_property( PidCandidate::em_evtgen_is_scattered_lepton, (uint)0 );

      /* is scattered lepton? */
      if ( particle_scattered_l &&
	   (*p) == particle_scattered_l )
	tc->set_property( PidCandidate::em_evtgen_is_scattered_lepton, (uint)1 );

      /* add pid candidate to collection */
      candidateMap.insert( make_pair( tc->get_candidate_id(), tc ) );
    }

  return 0;
}


int
DISKinematicsReco::InsertCandidateFromCluster( type_map_tcan& candidateMap , RawCluster *cluster , CaloEvalStack *caloevalstack, SvtxTrackMap *trackmap )
{

  /* create new electron candidate */
  PidCandidatev1 *tc = new PidCandidatev1();
  tc->set_candidate_id( candidateMap.size()+1 );

  /* set some initial cluster properties */
  float theta = atan2( cluster->get_r() , cluster->get_z() );
  float eta =  -1 * log( tan( theta / 2.0 ) );
  float pt = cluster->get_energy() * sin( theta );

  /* get calorimeter ID where towers of cluster are found */
  unsigned caloid = 0;
  RawCluster::TowerConstIterator rtiter = cluster->get_towers().first;
  caloid = RawTowerDefs::decode_caloid( rtiter->first );
  _trackproj->set_detector(_trackproj->get_detector_from_cluster(cluster));
  //cout << "Calo ID: " << caloid << " -> " << RawTowerDefs::convert_caloid_to_name( RawTowerDefs::decode_caloid( rtiter->first ) ) << endl;

  tc->set_property( PidCandidate::em_cluster_id, cluster->get_id() );
  tc->set_property( PidCandidate::em_cluster_prob, cluster->get_prob() );
  tc->set_property( PidCandidate::em_cluster_posx, cluster->get_x() );
  tc->set_property( PidCandidate::em_cluster_posy, cluster->get_y() );
  tc->set_property( PidCandidate::em_cluster_posz, cluster->get_z() );
  tc->set_property( PidCandidate::em_cluster_e, cluster->get_energy() );
  tc->set_property( PidCandidate::em_cluster_ecore, cluster->get_ecore() );
  tc->set_property( PidCandidate::em_cluster_et_iso, cluster->get_et_iso() );
  tc->set_property( PidCandidate::em_cluster_theta, theta );
  tc->set_property( PidCandidate::em_cluster_phi, cluster->get_phi() );
  tc->set_property( PidCandidate::em_cluster_eta, eta );
  tc->set_property( PidCandidate::em_cluster_pt, pt );
  tc->set_property( PidCandidate::em_cluster_ntower, (unsigned)cluster->getNTowers() );
  tc->set_property( PidCandidate::em_cluster_caloid, caloid );
  
  /* get track projection helper class */
  TrackProjectionTools tpt( _topNode );

  /* find matching reco track */
  float best_track_dr = NAN;
  SvtxTrack* best_track = NULL; //tpt.FindClosestTrack( cluster, best_track_dr ); /* @TODO switch track finding back on as soon as we understand it better */

  /* IF matching track found: set track properties */
  if ( best_track )
    {
      /* set some initial track properties */
      float theta = 2.0 * atan( exp( -1 * best_track->get_eta() ) );

      tc->set_property( PidCandidate::em_track_id, (uint)best_track->get_id() );
      tc->set_property( PidCandidate::em_track_quality, best_track->get_quality() );
      tc->set_property( PidCandidate::em_track_theta, theta );
      tc->set_property( PidCandidate::em_track_phi, best_track->get_phi() );
      tc->set_property( PidCandidate::em_track_eta, best_track->get_eta() );
      tc->set_property( PidCandidate::em_track_ptotal, best_track->get_p() );
      tc->set_property( PidCandidate::em_track_ptrans, best_track->get_pt() );
      tc->set_property( PidCandidate::em_track_charge, best_track->get_charge() );
      tc->set_property( PidCandidate::em_track_dca, best_track->get_dca() );
      tc->set_property( PidCandidate::em_track_section, (uint)0 );
      tc->set_property( PidCandidate::em_track_e3x3_cemc, best_track->get_cal_energy_3x3(SvtxTrack::CEMC) );
      tc->set_property( PidCandidate::em_track_e3x3_ihcal, best_track->get_cal_energy_3x3(SvtxTrack::HCALIN) );
      tc->set_property( PidCandidate::em_track_e3x3_ohcal, best_track->get_cal_energy_3x3(SvtxTrack::HCALOUT) );
      tc->set_property( PidCandidate::em_track_cluster_dr, best_track_dr );

      // Use the track states to project to the FEMC / FHCAL / EEMC and generate
      // energy sums.
      float e3x3_femc = NAN;
      float e3x3_fhcal = NAN;
      float e3x3_eemc = NAN;

      for (SvtxTrack::ConstStateIter state_itr = best_track->begin_states();
           state_itr != best_track->end_states(); state_itr++) {

        SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

        if( (temp->get_name()=="FHCAL") )
          e3x3_fhcal = tpt.getE33Forward( "FHCAL" , temp->get_x() , temp->get_y() );

        if( (temp->get_name()=="FEMC") )
          e3x3_femc = tpt.getE33Forward( "FEMC" , temp->get_x() , temp->get_y() );

        if( (temp->get_name()=="EEMC") )
          e3x3_eemc = tpt.getE33Forward( "EEMC" , temp->get_x() , temp->get_y() );
      }

      /* set candidate properties */
      tc->set_property( PidCandidate::em_track_e3x3_fhcal, e3x3_fhcal );
      tc->set_property( PidCandidate::em_track_e3x3_femc, e3x3_femc );
      tc->set_property( PidCandidate::em_track_e3x3_eemc, e3x3_eemc );

      /* Get information on track from PID detectors */
      tc->set_property( PidCandidate::em_pid_prob_electron, (float)0.0 );
      tc->set_property( PidCandidate::em_pid_prob_pion, (float)0.0 );
      tc->set_property( PidCandidate::em_pid_prob_kaon, (float)0.0 );
      tc->set_property( PidCandidate::em_pid_prob_proton, (float)0.0 );
    }

  /* set em candidate MC truth properties */
  tc->set_property( PidCandidate::em_evtgen_pid, (int)NAN );
  tc->set_property( PidCandidate::em_evtgen_ptotal, (float)NAN );
  tc->set_property( PidCandidate::em_evtgen_theta, (float)NAN );
  tc->set_property( PidCandidate::em_evtgen_phi, (float)NAN );
  tc->set_property( PidCandidate::em_evtgen_eta, (float)NAN );
  tc->set_property( PidCandidate::em_evtgen_charge, (int)NAN );

  /* If matching truth primary particle found: update truth information */
  CaloRawClusterEval* clustereval = caloevalstack->get_rawcluster_eval();
  PHG4Particle* primary = clustereval->max_truth_primary_particle_by_energy(cluster);

  if ( primary )
    {
      /* get particle momenta and theta, phi angles */
      float gpx = primary->get_px();
      float gpy = primary->get_py();
      float gpz = primary->get_pz();
      float gpt = sqrt(gpx * gpx + gpy * gpy);
      float gptotal = sqrt(gpx * gpx + gpy * gpy + gpz * gpz);
      //float ge = (float)primary->get_e();

      float gphi = NAN;
      float gtheta = NAN;
      float geta = NAN;

      if (gpt != 0.0)
	{
	  gtheta = atan2( gpt, gpz );
	  geta = -1 * log( tan( gtheta / 2.0 ) );
	}

      gphi = atan2(gpy, gpx);

      /* get charge based on PDG code of particle */
      TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( primary->get_pid() );
      int gcharge = -999;
      if ( pdg_p )
	{
	  /* NOTE: TParticlePDG::Charge() returns charge in units of |e|/3 (see ROOT documentation) */
	  gcharge = pdg_p->Charge() / 3;
	}

      tc->set_property( PidCandidate::em_evtgen_pid, primary->get_pid() );
      tc->set_property( PidCandidate::em_evtgen_ptotal, gptotal );
      tc->set_property( PidCandidate::em_evtgen_theta, gtheta );
      tc->set_property( PidCandidate::em_evtgen_phi, gphi );
      tc->set_property( PidCandidate::em_evtgen_eta, geta );
      tc->set_property( PidCandidate::em_evtgen_charge, gcharge );


      /* Track pointing to cluster */

      bool fill_in_truth = false;

      /* Try to find a track which best points to the current cluster */ 
      /* TODO: Depending on cluster theta, deltaR changes (currently -1) */

      SvtxTrack_FastSim* the_track = _trackproj->get_best_track(trackmap, cluster);
      
      /* Test if a best track was found */
      if(the_track!=NULL) //Fill in reconstructed info
	{
	  /* Get track position and momentum */
	  
	  SvtxTrackState *the_state = _trackproj->get_best_state(the_track, cluster);
	  double posv[3] = {0.,0.,0.};
	  posv[0] = the_state->get_x();
	  posv[1] = the_state->get_y();
	  posv[2] = the_state->get_z();
	  /* Extrapolate track2cluster values */
	  float track2cluster_theta = atan(sqrt(posv[0]*posv[0]+
						posv[1]*posv[1]) / posv[2]);
	  float track2cluster_eta = -log(abs(tan(track2cluster_theta/2)));
	  if(tan(track2cluster_theta/2)<0)
	    track2cluster_eta*=-1;
	  float track2cluster_phi = atan(posv[1]/posv[0]);
	  float track2cluster_x = posv[0];
	  float track2cluster_y = posv[1];
	  float track2cluster_z = posv[2];

	  /* Fills in weird values right now */
	  float track2cluster_p = 0;


	  tc->set_property( PidCandidate::em_track_theta2cluster, track2cluster_theta );
	  tc->set_property( PidCandidate::em_track_eta2cluster, track2cluster_eta );
	  tc->set_property( PidCandidate::em_track_phi2cluster, track2cluster_phi );
	  tc->set_property( PidCandidate::em_track_p2cluster, track2cluster_p );
	  tc->set_property( PidCandidate::em_track_x2cluster, track2cluster_x );
	  tc->set_property( PidCandidate::em_track_y2cluster, track2cluster_y);
	  tc->set_property( PidCandidate::em_track_z2cluster, track2cluster_z);
	  
	  
	  tc->set_property( PidCandidate::em_track_id, (uint)primary->get_track_id() );
	  tc->set_property( PidCandidate::em_track_quality, (float)100.0 );
	  tc->set_property( PidCandidate::em_track_theta,2*atan( exp(-the_track->get_eta()) ));
	  tc->set_property( PidCandidate::em_track_phi, the_track->get_phi() );
	  tc->set_property( PidCandidate::em_track_eta, the_track->get_eta() );
	  tc->set_property( PidCandidate::em_track_ptotal, the_track->get_p() );
	  tc->set_property( PidCandidate::em_track_ptrans, the_track->get_pt() );
	  tc->set_property( PidCandidate::em_track_charge, the_track->get_charge() );
	  tc->set_property( PidCandidate::em_track_dca, NAN );
	  tc->set_property( PidCandidate::em_track_section, (uint)0 );
	  tc->set_property( PidCandidate::em_track_e3x3_cemc, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ihcal, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ohcal, NAN );
	  tc->set_property( PidCandidate::em_track_cluster_dr, (float)0.0 );

	  tc->set_property( PidCandidate::em_pid_prob_electron, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_pion, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_kaon, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_proton, (float)0.0 );
	  
	  
	  /*
	     cout << "------------------------------------------" << endl;
	     cout << "Track Projection Successful!" << endl;
	     cout << "Cluster Detector = " << _trackproj->get_detector() << "EMC" << endl;
	     cout << "Cluster X: " << cluster->get_x() << " | Track X: " << posv[0] << endl;
	     cout << "Cluster Y: " << cluster->get_y() << " | Track Y: " << posv[1] << endl;
	     cout << "Cluster Z: " << cluster->get_z() << " | Track Z: " << posv[2] << endl;
	  */
	  
	  
	}
      else if(fill_in_truth) //Fill in truth
	{
	  tc->set_property( PidCandidate::em_track_theta2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_eta2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_phi2cluster, NAN );
	  tc->set_property( PidCandidate::em_track_p2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_x2cluster, NAN );
	  tc->set_property( PidCandidate::em_track_y2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_z2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_id, (uint)primary->get_track_id() );
	  tc->set_property( PidCandidate::em_track_quality, (float)100.0 );
	  tc->set_property( PidCandidate::em_track_theta, gtheta );
	  tc->set_property( PidCandidate::em_track_phi, gphi );
	  tc->set_property( PidCandidate::em_track_eta, geta );
	  tc->set_property( PidCandidate::em_track_ptotal, gptotal );
	  tc->set_property( PidCandidate::em_track_ptrans, gpt );
	  tc->set_property( PidCandidate::em_track_charge, gcharge );
	  tc->set_property( PidCandidate::em_track_dca, NAN );
	  tc->set_property( PidCandidate::em_track_section, (uint)0 );
	  tc->set_property( PidCandidate::em_track_e3x3_cemc, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ihcal, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ohcal, NAN );
	  tc->set_property( PidCandidate::em_track_cluster_dr, (float)0.0 );
	  
	  /* Get information on track from PID detectors */
	  tc->set_property( PidCandidate::em_pid_prob_electron, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_pion, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_kaon, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_proton, (float)0.0 );
	  
	  if ( abs( primary->get_pid() ) == 11 )
	    tc->set_property( PidCandidate::em_pid_prob_electron, (float)1.0 );
	  
	  if ( abs( primary->get_pid() ) == 211 )
	    tc->set_property( PidCandidate::em_pid_prob_pion, (float)1.0 );
	  
	  if ( abs( primary->get_pid() ) == 321 )
	    tc->set_property( PidCandidate::em_pid_prob_kaon, (float)1.0 );
	  
	  if ( abs( primary->get_pid() ) == 2212 )
	    tc->set_property( PidCandidate::em_pid_prob_proton, (float)1.0 );
	}
      else
	{
	  tc->set_property( PidCandidate::em_track_theta2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_eta2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_phi2cluster, NAN );
	  tc->set_property( PidCandidate::em_track_p2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_x2cluster, NAN );
	  tc->set_property( PidCandidate::em_track_y2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_z2cluster, NAN);
	  tc->set_property( PidCandidate::em_track_id,(uint)0);
	  tc->set_property( PidCandidate::em_track_quality, (float)0.0 );
	  tc->set_property( PidCandidate::em_track_theta, NAN );
	  tc->set_property( PidCandidate::em_track_phi, NAN );
	  tc->set_property( PidCandidate::em_track_eta, NAN );
	  tc->set_property( PidCandidate::em_track_ptotal, NAN );
	  tc->set_property( PidCandidate::em_track_ptrans, NAN );
	  tc->set_property( PidCandidate::em_track_charge, 0 );
	  tc->set_property( PidCandidate::em_track_dca, NAN );
	  tc->set_property( PidCandidate::em_track_section, (uint)0 );
	  tc->set_property( PidCandidate::em_track_e3x3_cemc, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ihcal, NAN );
	  tc->set_property( PidCandidate::em_track_e3x3_ohcal, NAN );
	  tc->set_property( PidCandidate::em_track_cluster_dr, (float)0.0 );
	  
	  /* Get information on track from PID detectors */
	  tc->set_property( PidCandidate::em_pid_prob_electron, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_pion, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_kaon, (float)0.0 );
	  tc->set_property( PidCandidate::em_pid_prob_proton, (float)0.0 );
	  
	}
    }
 
  /* add tau candidate to collection */
  candidateMap.insert( make_pair( tc->get_candidate_id(), tc ) );
  return 0;
}


int
DISKinematicsReco::AddGlobalCalorimeterInformation()
{
  /* Missing transverse energy, transverse energy direction, and Energy sums, x and y components separately */
  float Et_miss = 0;
  float Et_miss_phi = 0;
  float Ex_sum = 0;
  float Ey_sum = 0;

  /* energy threshold for considering tower */
  float tower_emin = 0.0;

  /* Loop over all tower (and geometry) collections */
  vector < string > calo_names;
  calo_names.push_back( "CEMC" );
  calo_names.push_back( "HCALIN" );
  calo_names.push_back( "HCALOUT" );
  calo_names.push_back( "FEMC" );
  calo_names.push_back( "FHCAL" );
  calo_names.push_back( "EEMC" );

  for ( unsigned i = 0; i < calo_names.size(); i++ )
    {

      string towernodename = "TOWER_CALIB_" + calo_names.at( i );
      RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(_topNode, towernodename.c_str());
      if (!towers)
        {
          std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
          return -1;
        }

      string towergeomnodename = "TOWERGEOM_" + calo_names.at( i );
      RawTowerGeomContainer *towergeoms = findNode::getClass<RawTowerGeomContainer>(_topNode, towergeomnodename.c_str());
      if (! towergeoms)
        {
          cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
          return -1;
        }

      /* define tower iterator */
      RawTowerContainer::ConstRange begin_end = towers->getTowers();
      RawTowerContainer::ConstIterator rtiter;

      /* loop over all tower in CEMC calorimeter */
      for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter)
        {
          /* get tower energy */
          RawTower *tower = rtiter->second;
          float tower_energy = tower->get_energy();

          /* check if tower above energy treshold */
          if ( tower_energy < tower_emin )
            continue;

          /* get eta and phi of tower and check angle delta_R w.r.t. jet axis */
          RawTowerGeom * tower_geom = towergeoms->get_tower_geometry(tower -> get_key());
          float tower_eta = tower_geom->get_eta();
          float tower_phi = tower_geom->get_phi();

          /* from https://en.wikipedia.org/wiki/Pseudorapidity:
             p_x = p_T * cos( phi )
             p_y = p_T * sin( phi )
             p_z = p_T * sinh( eta )
             |p| = p_T * cosh( eta )
          */

          /* calculate 'transverse' tower energy */
          float tower_energy_t = tower_energy / cosh( tower_eta );

          /* add energy components of this tower to total energy components */
          Ex_sum += tower_energy_t * cos( tower_phi );
          Ey_sum += tower_energy_t * sin( tower_phi );
        }
    }

  /* calculate Et_miss */
  Et_miss = sqrt( Ex_sum * Ex_sum + Ey_sum * Ey_sum );
  Et_miss_phi = atan( Ey_sum / Ex_sum );

  /* set event branch paraemters */
  ( _map_event_branches.find( "Et_miss" ) )->second = Et_miss;
  ( _map_event_branches.find( "Et_miss_phi" ) )->second = Et_miss_phi;

  return 0;
}

int
DISKinematicsReco::WriteCandidatesToTree( type_map_tcan& electronCandidateMap )
{
  /* Get number of electron candidates */
  ( _map_event_branches.find( "em_ncandidates" ) )->second = electronCandidateMap.size();

  /* Loop over all EM candidates and add them to tree */
  for (type_map_tcan::iterator iter = electronCandidateMap.begin();
       iter != electronCandidateMap.end();
       ++iter)
    {
      /* update information in map and fill tree */
      for ( map< PidCandidate::PROPERTY , vector<float> >::iterator iter_prop = _map_em_candidate_branches.begin();
            iter_prop != _map_em_candidate_branches.end();
            ++iter_prop)
        {
          switch ( PidCandidate::get_property_info( (iter_prop->first) ).second ) {

          case PidCandidate::type_float :
            (iter_prop->second).push_back( (iter->second)->get_property_float( (iter_prop->first) ) );
            break;

          case PidCandidate::type_int :
            (iter_prop->second).push_back( (iter->second)->get_property_int( (iter_prop->first) ) );
            break;

          case PidCandidate::type_uint :
            (iter_prop->second).push_back( (iter->second)->get_property_uint( (iter_prop->first) ) );
            break;

          case PidCandidate::type_unknown :
            break;
          }
        }
    }

  return 0;
}


PidCandidate*
DISKinematicsReco::FindMinDeltaRCandidate( type_map_tcan *candidates, const float eta_ref, const float phi_ref )
{
  /* PidCandidate with eta, phi closest to reference */
  PidCandidate* best_candidate = NULL;

  return best_candidate;
}


float
DISKinematicsReco::CalculateDeltaR( float eta1, float phi1, float eta2, float phi2 )
{
  /* Particles at phi = PI+x and phi = PI-x are actually close to each other in phi, but simply calculating
   * the difference in phi would give a large distance (because phi ranges from -PI to +PI in the convention
   * used. Account for this by subtracting 2PI is particles fall within this border area. */
  if( ( phi1 < -0.9*TMath::Pi()) && ( phi2 >  0.9*TMath::Pi() ) ) phi2 = phi2 - 2*TMath::Pi();
  if( ( phi1 >  0.9*TMath::Pi()) && ( phi2 < -0.9*TMath::Pi() ) ) phi1 = phi1 - 2*TMath::Pi();

  float delta_R = sqrt( pow( eta2 - eta1, 2 ) + pow( phi2 - phi1, 2 ) );

  return delta_R;
}

int
DISKinematicsReco::AddReconstructedKinematics( type_map_tcan& em_candidates , string mode )
{
  /* Loop over all EM candidates */
  for (type_map_tcan::iterator iter = em_candidates.begin();
       iter != em_candidates.end();
       ++iter)
    {
      /* Add reco kinematics based on scattered electron data */
      PidCandidate* the_electron = iter->second;

      float e0_E = _beam_electron_ptotal;
      float p0_E = _beam_hadron_ptotal;

      /* get scattered particle kinematicsbased on chosen mode */
      float e1_E = NAN;
      float e1_theta = NAN;

      if ( mode == "cluster" )
        {
          e1_E = the_electron->get_property_float( PidCandidate::em_cluster_e );
          e1_theta = the_electron->get_property_float( PidCandidate::em_cluster_theta );
        }
      else if ( mode == "truth" )
        {
          e1_E = the_electron->get_property_float( PidCandidate::em_evtgen_ptotal );
          e1_theta = the_electron->get_property_float( PidCandidate::em_evtgen_theta );
        }
      else
        {
          cout << "WARNING: Unknown mode " << mode << " selected." << endl;
          return -1;
        }

      /* for purpose of calculations, 'theta' angle of the scattered electron is defined as angle
         between 'scattered electron' and 'direction of incoming electron'. Since initial electron
         has 'theta = Pi' in coordinate system, we need to use 'theta_rel = Pi - theta' for calculations
         of kinematics. */
      float e1_theta_rel = M_PI - e1_theta;

      /* event kinematics */
      float dis_s = 4.0 * e0_E * p0_E;

      float dis_Q2 = 2.0 * e0_E * e1_E * ( 1 - cos( e1_theta_rel ) );

      /* ePHENIX LOI definition of y: */
      float dis_y = 1.0 - ( e1_E / e0_E ) + ( dis_Q2 / ( 4.0 * e0_E * e0_E ) );

      /* G. Wolf Hera Physics definitions of y: */
      //float dis_y = 1 - ( e1_E / (2*e0_E) ) * ( 1 - cos( e1_theta_rel ) );

      float dis_x = dis_Q2 / ( dis_s * dis_y );

      float dis_W2 = _mproton*_mproton + dis_Q2 * ( ( 1 / dis_x ) - 1 );
      float dis_W = sqrt( dis_W2 );

      the_electron->set_property( PidCandidate::em_reco_x_e, dis_x );
      the_electron->set_property( PidCandidate::em_reco_y_e, dis_y );
      the_electron->set_property( PidCandidate::em_reco_q2_e, dis_Q2 );
      the_electron->set_property( PidCandidate::em_reco_w_e, dis_W );
    }

  return 0;
}

int
DISKinematicsReco::AddTruthEventInformation()
{
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

  int true_process_id = theEvent->signal_process_id();
  float ev_x = -NAN;
  float ev_Q2 = -NAN;
  float ev_W = -NAN;
  float ev_y = -NAN;

  float ev_s = 4 * _beam_electron_ptotal * _beam_hadron_ptotal;


  if ( theEvent->pdf_info() )
    {
      //ev_x = theEvent->pdf_info()->x1();
      ev_x = theEvent->pdf_info()->x2();
      ev_Q2 = theEvent->pdf_info()->scalePDF();
      ev_y = ev_Q2 / ( ev_x * ev_s );
      ev_W = sqrt( _mproton*_mproton + ev_Q2 * ( 1 / ev_x - 1 ) );
    }

  ( _map_event_branches.find( "evtgen_process_id" ) )->second = true_process_id;
  ( _map_event_branches.find( "evtgen_s" ) )->second = ev_s;
  ( _map_event_branches.find( "evtgen_x" ) )->second = ev_x;
  ( _map_event_branches.find( "evtgen_Q2" ) )->second = ev_Q2;
  ( _map_event_branches.find( "evtgen_y" ) )->second = ev_y;
  ( _map_event_branches.find( "evtgen_W" ) )->second = ev_W;

  return 0;
}

void
DISKinematicsReco::ResetBranchMap()
{
  /* Event branches */
  for ( map< string , float >::iterator iter = _map_event_branches.begin();
        iter != _map_event_branches.end();
        ++iter)
    {
      (iter->second) = NAN;
    }

  /* EM candidate branches */
  for ( map< PidCandidate::PROPERTY , vector<float> >::iterator iter = _map_em_candidate_branches.begin();
        iter != _map_em_candidate_branches.end();
        ++iter)
    {
      (iter->second).clear();
    }

  return;
}

int
DISKinematicsReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();

  if ( _tree_event_cluster )
    _tree_event_cluster->Write();

  if ( _tree_event_truth )
    _tree_event_truth->Write();

  _tfile->Close();

  return 0;
}
