#include "DISKinematicsReco.h"
#include "TauCandidatev1.h"
#include "TruthTrackerHepMC.h"

/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <g4hough/SvtxTrackMap_v1.h>

#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <g4jets/JetMap.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerv1.h>

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

using namespace std;

DISKinematicsReco::DISKinematicsReco(std::string filename) :
  SubsysReco("DISKinematicsReco" ),
  _save_towers(false),
  _save_tracks(false),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _e_candidate(nullptr),
  _tree_event(nullptr),
  _ebeam_E(10),
  _pbeam_E(250)
{

}

int
DISKinematicsReco::Init(PHCompositeNode *topNode)
{
  _topNode = topNode;

  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* Add TauCandidate properties to map that defines output tree */
  float dummy = 0;
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_pid , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_etotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_id , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_energy , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_ecore , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_et_iso , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_prob , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::cluster_ntower , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_id , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_quality , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_ptotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_ptrans , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_charge , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_cemc , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_femc , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_eemc , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_ihcal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_ohcal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_fhcal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::track_e3x3_ehcal , dummy ) );

  /* Create tree for information about electron candidates */
  _e_candidate = new TTree("candidates", "a Tree with electron candidates");
  _e_candidate->Branch("event", &_ievent, "event/I");
  for ( map< TauCandidate::PROPERTY , float >::iterator iter = _map_treebranches.begin();
        iter != _map_treebranches.end();
        ++iter)
    {
      _e_candidate->Branch(TauCandidate::get_property_info( (iter->first) ).first.c_str(),
                           &(iter->second),
                           TauCandidate::get_property_info( (iter->first) ).first.c_str());
    }


  /* Add branches to map that defines output tree for event-wise properties */
  _map_eventbranches.insert( make_pair( "reco_electron_found" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_is_electron" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_eta" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_phi" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_ptotal" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_x" , dummy ) );
  _map_eventbranches.insert( make_pair( "reco_electron_Q2" , dummy ) );
  _map_eventbranches.insert( make_pair( "evtgen_process_id" , dummy ) );
  _map_eventbranches.insert( make_pair( "evtgen_x" , dummy ) );
  _map_eventbranches.insert( make_pair( "evtgen_Q2" , dummy ) );

  /* Create tree for information about full event */
  _tree_event = new TTree("event", "a Tree with global event information");
  _tree_event->Branch("event", &_ievent, "event/I");
  for ( map< string , float >::iterator iter = _map_eventbranches.begin();
        iter != _map_eventbranches.end();
        ++iter)
    {
      _tree_event->Branch( (iter->first).c_str(),
                           &(iter->second),
                           (iter->first).c_str() );
    }

  return 0;
}

int
DISKinematicsReco::process_event(PHCompositeNode *topNode)
{
  /* Create map to collect electron candidates.
   * Use energy as 'key' to the map because energy is unique for each jet, while there are sometimes multiple jets (same energy,
   * different jet ID) in the input jet collection. Also, map automatically sorts entries by key, i.e. this gives list of tau candidates
   * sorted by energy. */
  type_map_tcan electronCandidateMap;

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if (!trackmap)
    {
      cout << PHWHERE << "SvtxTrackMap node not found on node tree"
           << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get track collection with all h-going tracks in this event */
  SvtxTrackMap* trackmap_eta_plus =
    findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap_FastSimEtaPlus");
  if (!trackmap_eta_plus)
    {
      cout << PHWHERE << "SvtxTrackMap_FastSimEtaPlus node not found on node tree"
           << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get track collection with all e-going tracks in this event */
  SvtxTrackMap* trackmap_eta_minus =
    findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap_FastSimEtaMinus");
  if (!trackmap_eta_minus)
    {
      cout << PHWHERE << "SvtxTrackMap_FastSimEtaMinus node not found on node tree"
           << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get collection of truth particles from event generator */
  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode,"PHHepMCGenEventMap");
  if (!genevtmap) {
    cerr << PHWHERE << " ERROR: Can't find PHHepMCGenEventMap" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /* Loop over all clusters in EEMC, CEMC, and FEMC to find electron candidates */
  vector< string > v_ecals;
  v_ecals.push_back("EEMC");
  v_ecals.push_back("CEMC");
  v_ecals.push_back("FEMC");

  for ( unsigned idx = 0; idx < v_ecals.size(); idx++ )
    {
      CaloEvalStack * caloevalstack = new CaloEvalStack(topNode, v_ecals.at( idx ) );

      string clusternodename = "CLUSTER_" + v_ecals.at( idx );
      RawClusterContainer *clusterList = findNode::getClass<RawClusterContainer>(topNode,clusternodename.c_str());
      if (!clusterList) {
        cerr << PHWHERE << " ERROR: Can't find node " << clusternodename << endl;
        return false;
      }

      for (unsigned int k = 0; k < clusterList->size(); ++k)
        {
          RawCluster *cluster = clusterList->getCluster(k);

          /* Check if cluster energy is below threshold */
          float e_cluster_threshold = 0.1;
          if ( cluster->get_energy() < e_cluster_threshold )
            continue;

          InsertCandidateFromCluster( electronCandidateMap , cluster , caloevalstack );
        }
    }

  /* Loop over all tracks to collect electron candidate objects */
  /* Loop over tracks
   * (float) track->get_eta(),     //eta of the track
   * (float) track->get_phi(),     //phi of the track
   * (float) track->get_pt(),      //transverse momentum of track
   * (float) track->get_p(),       //total momentum of track
   * (float) track->get_charge(),  //electric charge of track
   * (float) track->get_quality()  //track quality */
  /* barrel tracking */
  //  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
  //       track_itr != trackmap->end(); track_itr++)
  //    {
  //      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
  //    } // end loop over reco tracks //
  //
  //  /* h-going tracking */
  //  for (SvtxTrackMap::ConstIter track_itr = trackmap_eta_plus->begin();
  //       track_itr != trackmap_eta_plus->end(); track_itr++)
  //    {
  //      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
  //    } // end loop over reco tracks //
  //
  //  /* e-going tracking */
  //  for (SvtxTrackMap::ConstIter track_itr = trackmap_eta_minus->begin();
  //       track_itr != trackmap_eta_minus->end(); track_itr++)
  //    {
  //      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
  //    } // end loop over reco tracks //

  /* Add calorimeter information to tau candidates */
  //  AddCalorimeterInformation( electronCandidateMap, &map_calotower );

  /* Add track information to tau candidates */
  //  AddTrackInformation( electronCandidateMap, trackmap );

  /* Add tag for true Tau particle jet to tau candidates */
  //  AddTrueElectronTag( electronCandidateMap, genevtmap );

  /* Add information about tau candidats to output tree */
  WriteCandidatesToTree( electronCandidateMap );

  /* Add global event information */
  AddGlobalEventInformation( electronCandidateMap, genevtmap );

  /* count up event number */
  _ievent ++;

  return 0;
}


int
DISKinematicsReco::InsertCandidateFromCluster( type_map_tcan& candidateMap , RawCluster *cluster , CaloEvalStack *caloevalstack )
{
  /* create new electron candidate */
  TauCandidatev1 *tc = new TauCandidatev1();
  tc->set_candidate_id( candidateMap.size()+1 );

  /* set some initial cluster properties */
  float theta = atan2( cluster->get_r() , cluster->get_z() );
  float eta =  -log(tan(theta/2.0));

  tc->set_property( TauCandidate::cluster_id, cluster->get_id() );
  tc->set_property( TauCandidate::cluster_energy, cluster->get_energy() );
  tc->set_property( TauCandidate::cluster_ecore, cluster->get_ecore() );
  tc->set_property( TauCandidate::cluster_et_iso, cluster->get_et_iso() );
  tc->set_property( TauCandidate::cluster_prob, cluster->get_prob() );
  tc->set_property( TauCandidate::cluster_eta, eta );
  tc->set_property( TauCandidate::cluster_phi, cluster->get_phi() );
  tc->set_property( TauCandidate::cluster_ntower, (unsigned)cluster->getNTowers() );

  /* set tau candidate MC truth properties */
  tc->set_property( TauCandidate::evtgen_pid, (int)0 );
  tc->set_property( TauCandidate::evtgen_etotal, (float)NAN );
  tc->set_property( TauCandidate::evtgen_eta, (float)NAN );
  tc->set_property( TauCandidate::evtgen_phi, (float)NAN );

  /* If matching truth primary particle found: update truth information */
  CaloRawClusterEval* clustereval = caloevalstack->get_rawcluster_eval();

  PHG4Particle* primary = clustereval->max_truth_primary_particle_by_energy(cluster);

  if ( primary )
    {
      float gpx = primary->get_px();
      float gpy = primary->get_py();
      float gpz = primary->get_pz();
      float gpt = sqrt(gpx * gpx + gpy * gpy);

      float gphi = NAN;
      float geta = NAN;

      if (gpt != 0.0) geta = asinh(gpz / gpt);
      gphi = atan2(gpy, gpx);

      tc->set_property( TauCandidate::evtgen_pid, primary->get_pid() );
      tc->set_property( TauCandidate::evtgen_etotal, (float)primary->get_e() );
      tc->set_property( TauCandidate::evtgen_eta, geta );
      tc->set_property( TauCandidate::evtgen_phi, gphi );
    }

  /* add tau candidate to collection */
  candidateMap.insert( make_pair( tc->get_candidate_id(), tc ) );

  return 0;
}


int
DISKinematicsReco::InsertCandidateFromTrack( type_map_tcan& candidateMap , SvtxTrack* track )
{
  //    /* skip tracks below momentum threshold */
  //    /* also do a cut on track quality here? */
  //    if ( track->get_p() < _ele_track_pmin )
  //      continue;

  /* create new electron candidate */
  TauCandidatev1 *tc = new TauCandidatev1();
  tc->set_candidate_id( track->get_id() );

  /* set some initial track properties */
  tc->set_property( TauCandidate::track_id, (uint)track->get_id() );
  tc->set_property( TauCandidate::track_quality, track->get_quality() );
  tc->set_property( TauCandidate::track_eta, track->get_eta() );
  tc->set_property( TauCandidate::track_phi, track->get_phi() );
  tc->set_property( TauCandidate::track_ptotal, track->get_p() );
  tc->set_property( TauCandidate::track_ptrans, track->get_pt() );
  tc->set_property( TauCandidate::track_charge, track->get_charge() );
  tc->set_property( TauCandidate::track_e3x3_cemc, track->get_cal_energy_3x3(SvtxTrack::CEMC) );
  tc->set_property( TauCandidate::track_e3x3_ihcal, track->get_cal_energy_3x3(SvtxTrack::HCALIN) );
  tc->set_property( TauCandidate::track_e3x3_ohcal, track->get_cal_energy_3x3(SvtxTrack::HCALOUT) );

  // Use the track states to project to the FEMC / FHCAL / EEMC and generate
  // energy sums.
  float e3x3_femc = 0;
  float e3x3_fhcal = 0;
  float e3x3_eemc = 0;

  for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
       state_itr != track->end_states(); state_itr++) {

    SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);
    //std::cout << " State found at pathlength = " << temp->get_pathlength() << std::endl;
    //getCluster(topNode, temp->get_name(), temp->get_eta(), temp->get_phi());

    if( (temp->get_name()=="FHCAL") )
      e3x3_fhcal = getE33( _topNode , "FHCAL" , temp->get_x() , temp->get_y() );

    if( (temp->get_name()=="FEMC") )
      e3x3_femc = getE33( _topNode , "FEMC" , temp->get_x() , temp->get_y() );

    if( (temp->get_name()=="EEMC") )
      e3x3_eemc = getE33( _topNode , "EEMC" , temp->get_x() , temp->get_y() );

  }

  /* set candidate properties */
  tc->set_property( TauCandidate::track_e3x3_fhcal, e3x3_fhcal );
  tc->set_property( TauCandidate::track_e3x3_femc, e3x3_femc );
  tc->set_property( TauCandidate::track_e3x3_eemc, e3x3_eemc );

  /* set tau candidate MC truth properties */
  tc->set_property( TauCandidate::evtgen_pid, (int)0 );
  tc->set_property( TauCandidate::evtgen_etotal, (float)NAN );
  tc->set_property( TauCandidate::evtgen_eta, (float)NAN );
  tc->set_property( TauCandidate::evtgen_phi, (float)NAN );

  /* add tau candidate to collection */
  candidateMap.insert( make_pair( track->get_p(), tc ) );

  return 0;
}


int
DISKinematicsReco::AddTrueElectronTag( type_map_tcan& electronCandidateMap, PHHepMCGenEventMap *genevtmap )
{
  return 0;
}


int
DISKinematicsReco::AddCalorimeterInformation( type_map_tcan& electronCandidateMap, type_map_cdata* map_towers )
{
  return 0;
}


int
DISKinematicsReco::AddTrackInformation( type_map_tcan& electronCandidateMap, SvtxTrackMap* trackmap )
{
  return 0;
}


int
DISKinematicsReco::WriteCandidatesToTree( type_map_tcan& electronCandidateMap )
{
  /* Loop over all tau candidates */
  for (type_map_tcan::iterator iter = electronCandidateMap.begin();
       iter != electronCandidateMap.end();
       ++iter)
    {
      /* update information in map and fill tree */
      for ( map< TauCandidate::PROPERTY , float >::iterator iter_prop = _map_treebranches.begin();
            iter_prop != _map_treebranches.end();
            ++iter_prop)
        {
          switch ( TauCandidate::get_property_info( (iter_prop->first) ).second ) {

          case TauCandidate::type_float :
            (iter_prop->second) = (iter->second)->get_property_float( (iter_prop->first) );
            break;

          case TauCandidate::type_int :
            (iter_prop->second) = (iter->second)->get_property_int( (iter_prop->first) );
            break;

          case TauCandidate::type_uint :
            (iter_prop->second) = (iter->second)->get_property_uint( (iter_prop->first) );
            break;

          case TauCandidate::type_unknown :
            break;
          }
        }
      _e_candidate->Fill();
    }

  return 0;
}


TauCandidate*
DISKinematicsReco::FindMinDeltaRCandidate( type_map_tcan *candidates, const float eta_ref, const float phi_ref )
{
  /* TauCandidate with eta, phi closest to reference */
  TauCandidate* best_candidate = NULL;

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
DISKinematicsReco::AddGlobalEventInformation( type_map_tcan& electronCandidateMap , PHHepMCGenEventMap* geneventmap )
{
  /* Loop over electron candidates and find electron */
  TauCandidate* the_electron = NULL;
  for (type_map_tcan::iterator iter = electronCandidateMap.begin();
       iter != electronCandidateMap.end();
       ++iter)
    {
      if ( ( iter->second)->get_property_int( TauCandidate::evtgen_pid ) == 11 )
        the_electron = iter->second;
    }

  /* update event information tree variables */
  /* @TODO make this better protected against errors- if 'find' returns NULL pointer,
     this will lead to a SEGMENTATION FAULT */
  if ( the_electron )
    {
      ( _map_eventbranches.find( "reco_electron_found" ) )->second = 1;
      ( _map_eventbranches.find( "reco_electron_is_electron" ) )->second =
	( the_electron->get_property_int( TauCandidate::evtgen_pid ) == 11 );
      ( _map_eventbranches.find( "reco_electron_eta" ) )->second =
	the_electron->get_property_float( TauCandidate::cluster_eta );
      ( _map_eventbranches.find( "reco_electron_phi" ) )->second =
	the_electron->get_property_float( TauCandidate::cluster_phi );
      ( _map_eventbranches.find( "reco_electron_ptotal" ) )->second =
	the_electron->get_property_float( TauCandidate::cluster_energy );
    }
  else
    {
      ( _map_eventbranches.find( "reco_electron_found" ) )->second = 0;
      ( _map_eventbranches.find( "reco_electron_is_electron" ) )->second = NAN;
      ( _map_eventbranches.find( "reco_electron_eta" ) )->second = NAN;
      ( _map_eventbranches.find( "reco_electron_phi" ) )->second = NAN;
      ( _map_eventbranches.find( "reco_electron_ptotal" ) )->second = NAN;
    }

  /* Add reco kinematics based on scattered electron data */
  float e0_E = _ebeam_E;
  float p0_E = _pbeam_E;
  //float e1_px = particle_e1->momentum().px();
  //float e1_py = particle_e1->momentum().py();
  //float e1_pz = particle_e1->momentum().pz();
  //float e1_p = sqrt(e1_px*e1_px + e1_py*e1_py + e1_pz*e1_pz);
  float e1_E = the_electron->get_property_float( TauCandidate::cluster_energy );
  float e1_eta = the_electron->get_property_float( TauCandidate::cluster_eta );
  float e1_theta = 2.0 * atan( exp( -1 * e1_eta ) );
  //float e1_phi = the_electron->get_property_float( TauCandidate::cluster_phi );

  /* for purpose of calculations, 'theta' angle of the scattered electron is defined as angle
     between 'scattered electron' and 'direction of incoming electron'. Since initial electron
     has 'theta = Pi' in coordinate system, we need to use 'theta_rel = Pi - theta' for calculations
     of kinematics. */
  float e1_theta_rel = M_PI - e1_theta;

  /* event kinematics */
  float dis_s = 4 * e0_E * p0_E;

  //      float dis_y = 1 - ( e1_E / (2*e0_E) ) * ( 1 - cos( e1_theta_rel ) );
  float dis_y = 1 - (e1_E/e0_E) * pow( cos( e1_theta_rel / 2. ), 2 );

  float dis_Q2 = 2 * e0_E * e1_E * ( 1 - cos( e1_theta_rel ) );

  float dis_x = dis_Q2 / ( dis_s * dis_y );

  ( _map_eventbranches.find( "reco_electron_x" ) )->second = dis_x;
  ( _map_eventbranches.find( "reco_electron_Q2" ) )->second = dis_Q2;

  /* Add truth kinematics */
  int embedding_id = 1;
  PHHepMCGenEvent *genevt = geneventmap->get(embedding_id);
  if (!genevt)
    {
      std::cout <<PHWHERE<<" - Fatal error - node PHHepMCGenEventMap missing subevent with embedding ID "<< embedding_id;
      std::cout <<". Print PHHepMCGenEventMap:";
      geneventmap->identify();
      return Fun4AllReturnCodes::ABORTRUN;
    }

  HepMC::GenEvent* theEvent = genevt->getEvent();

  if ( !theEvent )
    {
      cout << "Missing GenEvent!" << endl;
      return Fun4AllReturnCodes::ABORTRUN;
    }

  int true_process_id = theEvent->signal_process_id();
  //float ev_x1 = -999;
  float ev_x2 = -999;
  float ev_Q2 = -999;

  if ( theEvent->pdf_info() )
    {
      //ev_x1 = theEvent->pdf_info()->x1();
      ev_x2 = theEvent->pdf_info()->x2();
      ev_Q2 = theEvent->pdf_info()->scalePDF();
    }

  ( _map_eventbranches.find( "evtgen_process_id" ) )->second = true_process_id;
  ( _map_eventbranches.find( "evtgen_x" ) )->second = ev_x2;
  ( _map_eventbranches.find( "evtgen_Q2" ) )->second = ev_Q2;

  /* fill event information tree */
  _tree_event->Fill();

  return 0;
}


float
DISKinematicsReco::getE33( PHCompositeNode *topNode, string detName, float tkx, float tky )
{
  float twr_sum = 0;

  string towernodename = "TOWER_CALIB_" + detName;
  // Grab the towers
  RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(topNode, towernodename.c_str());
  if (!towers)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return -1;
    }
  string towergeomnodename = "TOWERGEOM_" + detName;
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(topNode, towergeomnodename.c_str());
  if (! towergeom)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
      return -1;
    }

  // Locate the central tower
  float r_dist = 9999.0;
  int twr_j = -1;
  int twr_k = -1;
  RawTowerDefs::CalorimeterId calo_id_ = RawTowerDefs::convert_name_to_caloid( detName );

  RawTowerContainer::ConstRange begin_end  = towers->getTowers();
  RawTowerContainer::ConstIterator itr = begin_end.first;
  for (; itr != begin_end.second; ++itr)
    {
      RawTowerDefs::keytype towerid = itr->first;
      RawTowerGeom *tgeo = towergeom->get_tower_geometry(towerid);

      float x = tgeo->get_center_x();
      float y = tgeo->get_center_y();

      float temp_rdist = sqrt(pow(tkx-x,2) + pow(tky-y,2)) ;
      if(temp_rdist< r_dist){
        r_dist = temp_rdist;
        twr_j = RawTowerDefs::decode_index1(towerid);
        twr_k = RawTowerDefs::decode_index2(towerid);
      }

      if( (fabs(tkx-x)<(tgeo->get_size_x()/2.0)) &&
          (fabs(tky-y)<(tgeo->get_size_y()/2.0)) ) break;

    }

  // Use the central tower to sum up the 3x3 energy
  if(twr_j>=0 && twr_k>=0){
    for(int ij = -1; ij <=1; ij++){
      for(int ik = -1; ik <=1; ik++){
        RawTowerDefs::keytype temp_towerid = RawTowerDefs::encode_towerid( calo_id_ , twr_j + ij , twr_k + ik );
        RawTower *rawtower = towers->getTower(temp_towerid);
        if(rawtower) twr_sum += rawtower->get_energy();
      }
    }
  }

  return twr_sum;
}


int
DISKinematicsReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();

  if ( _e_candidate )
    _e_candidate->Write();

  if ( _tree_event )
    _tree_event->Write();

  _tfile->Close();

  return 0;
}
