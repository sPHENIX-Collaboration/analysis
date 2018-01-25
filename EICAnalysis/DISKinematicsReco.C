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
  vector< float > vdummy;

  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_id , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_prob , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_posx , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_posy , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_posz , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_ecore , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_et_iso , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_pt , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_ntower , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_cluster_caloid , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_id , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_quality , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_ptotal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_ptrans , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_charge , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_dca , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_section , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_cemc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_femc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_eemc , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_ihcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_ohcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_fhcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_e3x3_ehcal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_track_cluster_dr , vdummy ) );

  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_evtgen_pid , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_evtgen_ptotal , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_evtgen_theta , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_evtgen_phi , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_evtgen_charge , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_reco_x_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_reco_y_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_reco_q2_e , vdummy ) );
  _map_em_candidate_branches.insert( make_pair( TauCandidate::em_reco_w_e , vdummy ) );

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
  _map_event_branches.insert( make_pair( "evtgen_x" , dummy ) );
  _map_event_branches.insert( make_pair( "evtgen_Q2" , dummy ) );

  /* Create tree for information about full event */
  _tree_event = new TTree("event", "a Tree with global event information and EM candidates");
  _tree_event->Branch("event", &_ievent, "event/I");

  /* Add event branches */
  for ( map< string , float >::iterator iter = _map_event_branches.begin();
        iter != _map_event_branches.end();
        ++iter)
    {
      _tree_event->Branch( (iter->first).c_str(),
                           &(iter->second) );
    }

  /* Add EM candidate branches */
  for ( map< TauCandidate::PROPERTY , vector<float> >::iterator iter = _map_em_candidate_branches.begin();
        iter != _map_em_candidate_branches.end();
        ++iter)
    {
      _tree_event->Branch( TauCandidate::get_property_info( (iter->first) ).first.c_str(),
                           &(iter->second) );
    }

  return 0;
}

int
DISKinematicsReco::process_event(PHCompositeNode *topNode)
{
  /* Reset branch map */
  ResetBranchMap();

  /* Create map to collect electron candidates.
   * Use energy as 'key' to the map because energy is unique for each jet, while there are sometimes multiple jets (same energy,
   * different jet ID) in the input jet collection. Also, map automatically sorts entries by key, i.e. this gives list of tau candidates
   * sorted by energy. */
  type_map_tcan electronCandidateMap;

  /* Collect EM candidates */
  CollectEmCandidates( electronCandidateMap );

  /* Calculate kinematics for each em candidate */
  AddReconstructedKinematics( electronCandidateMap );

  /* Add information about em candidats to output tree */
  WriteCandidatesToTree( electronCandidateMap );

  /* Add global event information */
  AddGlobalCalorimeterInformation();
  AddTruthEventInformation();

  /* fill event information tree */
  _tree_event->Fill();

  /* count up event number */
  _ievent ++;

  return 0;
}

int
DISKinematicsReco::CollectEmCandidates( type_map_tcan& electronCandidateMap )
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
  //float eta =  -log(tan(theta/2.0));
  float pt = cluster->get_energy() * sin( theta );

  tc->set_property( TauCandidate::em_cluster_id, cluster->get_id() );
  tc->set_property( TauCandidate::em_cluster_prob, cluster->get_prob() );
  tc->set_property( TauCandidate::em_cluster_posx, cluster->get_x() );
  tc->set_property( TauCandidate::em_cluster_posy, cluster->get_y() );
  tc->set_property( TauCandidate::em_cluster_posz, cluster->get_z() );
  tc->set_property( TauCandidate::em_cluster_e, cluster->get_energy() );
  tc->set_property( TauCandidate::em_cluster_ecore, cluster->get_ecore() );
  tc->set_property( TauCandidate::em_cluster_et_iso, cluster->get_et_iso() );
  tc->set_property( TauCandidate::em_cluster_theta, theta );
  tc->set_property( TauCandidate::em_cluster_phi, cluster->get_phi() );
  tc->set_property( TauCandidate::em_cluster_pt, pt );
  tc->set_property( TauCandidate::em_cluster_ntower, (unsigned)cluster->getNTowers() );
  tc->set_property( TauCandidate::em_cluster_caloid, (unsigned)0 );

  /* find matching reco track */
  float best_track_dr = NAN;
  SvtxTrack* best_track = FindClosestTrack( cluster, best_track_dr );

  /* IF matching track found: set track properties */
  if ( best_track )
    {
      /* set some initial track properties */
      float theta = 2.0 * atan( exp( -1 * best_track->get_eta() ) );

      tc->set_property( TauCandidate::em_track_id, (uint)best_track->get_id() );
      tc->set_property( TauCandidate::em_track_quality, best_track->get_quality() );
      tc->set_property( TauCandidate::em_track_theta, theta );
      tc->set_property( TauCandidate::em_track_phi, best_track->get_phi() );
      tc->set_property( TauCandidate::em_track_ptotal, best_track->get_p() );
      tc->set_property( TauCandidate::em_track_ptrans, best_track->get_pt() );
      tc->set_property( TauCandidate::em_track_charge, best_track->get_charge() );
      tc->set_property( TauCandidate::em_track_dca, best_track->get_dca() );
      tc->set_property( TauCandidate::em_track_section, (uint)0 );
      tc->set_property( TauCandidate::em_track_e3x3_cemc, best_track->get_cal_energy_3x3(SvtxTrack::CEMC) );
      tc->set_property( TauCandidate::em_track_e3x3_ihcal, best_track->get_cal_energy_3x3(SvtxTrack::HCALIN) );
      tc->set_property( TauCandidate::em_track_e3x3_ohcal, best_track->get_cal_energy_3x3(SvtxTrack::HCALOUT) );
      tc->set_property( TauCandidate::em_track_cluster_dr, best_track_dr );

      // Use the track states to project to the FEMC / FHCAL / EEMC and generate
      // energy sums.
      float e3x3_femc = NAN;
      float e3x3_fhcal = NAN;
      float e3x3_eemc = NAN;

      for (SvtxTrack::ConstStateIter state_itr = best_track->begin_states();
           state_itr != best_track->end_states(); state_itr++) {

        SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

        if( (temp->get_name()=="FHCAL") )
          e3x3_fhcal = getE33Forward( "FHCAL" , temp->get_x() , temp->get_y() );

        if( (temp->get_name()=="FEMC") )
          e3x3_femc = getE33Forward( "FEMC" , temp->get_x() , temp->get_y() );

        if( (temp->get_name()=="EEMC") )
          e3x3_eemc = getE33Forward( "EEMC" , temp->get_x() , temp->get_y() );
      }

      /* set candidate properties */
      tc->set_property( TauCandidate::em_track_e3x3_fhcal, e3x3_fhcal );
      tc->set_property( TauCandidate::em_track_e3x3_femc, e3x3_femc );
      tc->set_property( TauCandidate::em_track_e3x3_eemc, e3x3_eemc );
    }

  /* set em candidate MC truth properties */
  tc->set_property( TauCandidate::em_evtgen_pid, (int)NAN );
  tc->set_property( TauCandidate::em_evtgen_ptotal, (float)NAN );
  tc->set_property( TauCandidate::em_evtgen_theta, (float)NAN );
  tc->set_property( TauCandidate::em_evtgen_phi, (float)NAN );
  tc->set_property( TauCandidate::em_evtgen_charge, (int)NAN );

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
      float gtheta = NAN;

      if (gpt != 0.0) gtheta = asinh(gpz / gpt);
      gphi = atan2(gpy, gpx);

      tc->set_property( TauCandidate::em_evtgen_pid, primary->get_pid() );
      tc->set_property( TauCandidate::em_evtgen_ptotal, (float)primary->get_e() );
      tc->set_property( TauCandidate::em_evtgen_theta, gtheta );
      tc->set_property( TauCandidate::em_evtgen_phi, gphi );
      tc->set_property( TauCandidate::em_evtgen_charge, (int)NAN );
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
      for ( map< TauCandidate::PROPERTY , vector<float> >::iterator iter_prop = _map_em_candidate_branches.begin();
            iter_prop != _map_em_candidate_branches.end();
            ++iter_prop)
        {
          switch ( TauCandidate::get_property_info( (iter_prop->first) ).second ) {

          case TauCandidate::type_float :
            (iter_prop->second).push_back( (iter->second)->get_property_float( (iter_prop->first) ) );
            break;

          case TauCandidate::type_int :
            (iter_prop->second).push_back( (iter->second)->get_property_int( (iter_prop->first) ) );
            break;

          case TauCandidate::type_uint :
            (iter_prop->second).push_back( (iter->second)->get_property_uint( (iter_prop->first) ) );
            break;

          case TauCandidate::type_unknown :
            break;
          }
        }
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
DISKinematicsReco::AddReconstructedKinematics( type_map_tcan& em_candidates )
{
  /* Loop over all EM candidates */
  for (type_map_tcan::iterator iter = em_candidates.begin();
       iter != em_candidates.end();
       ++iter)
    {
      /* Add reco kinematics based on scattered electron data */
      TauCandidate* the_electron = iter->second;

      float e0_E = _ebeam_E;
      float p0_E = _pbeam_E;
      //float e1_px = particle_e1->momentum().px();
      //float e1_py = particle_e1->momentum().py();
      //float e1_pz = particle_e1->momentum().pz();
      //float e1_p = sqrt(e1_px*e1_px + e1_py*e1_py + e1_pz*e1_pz);
      float e1_E = the_electron->get_property_float( TauCandidate::em_cluster_e );
      float e1_theta = the_electron->get_property_float( TauCandidate::em_cluster_theta );
      //      float e1_theta = 2.0 * atan( exp( -1 * e1_eta ) );
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

      float dis_W = 0;

      the_electron->set_property( TauCandidate::em_reco_x_e, dis_x );
      the_electron->set_property( TauCandidate::em_reco_y_e, dis_y );
      the_electron->set_property( TauCandidate::em_reco_q2_e, dis_Q2 );
      the_electron->set_property( TauCandidate::em_reco_w_e, dis_W );
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
  //float ev_x1 = -999;
  float ev_x2 = -999;
  float ev_Q2 = -999;

  if ( theEvent->pdf_info() )
    {
      //ev_x1 = theEvent->pdf_info()->x1();
      ev_x2 = theEvent->pdf_info()->x2();
      ev_Q2 = theEvent->pdf_info()->scalePDF();
    }

  ( _map_event_branches.find( "evtgen_process_id" ) )->second = true_process_id;
  ( _map_event_branches.find( "evtgen_x" ) )->second = ev_x2;
  ( _map_event_branches.find( "evtgen_Q2" ) )->second = ev_Q2;

  return 0;
}


SvtxTrack* DISKinematicsReco::FindClosestTrack( RawCluster* cluster, float& best_track_dr )
{

  /* best matching track */
  SvtxTrack* best_track = NULL;

  /* find name of calorimeter for this cluster */
  string caloname = "NONE";

  /* C++11 range loop */
  for (auto& towit : cluster->get_towermap() )
    {
      caloname = RawTowerDefs::convert_caloid_to_name( RawTowerDefs::decode_caloid(towit.first) );
      break;
    }

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(_topNode,"SvtxTrackMap_FastSim");
  if (!trackmap)
    {
      cout << PHWHERE << "SvtxTrackMap node not found on node tree"
           << endl;
    }

  /* Loop over all tracks from BARREL tracking and see if one points to the same
   * cluster as the reference clusters (i.e. matching ID in the same calorimeter) */
  //for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
  //     track_itr != trackmap->end(); track_itr++)
  //  {
  //    SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);
  //
  //    if ( caloname == "CEMC" &&
  //         track->get_cal_cluster_id(SvtxTrack::CEMC) == cluster->get_id() )
  //      {
  //        best_track = track;
  //      }
  //  }

  /* If track found with barrel tracking, return it here- if not, proceed with forward tracking below. */
  if ( best_track )
    return best_track;

  /* Cluster / track matching for forward calorimeters and tracking */
  float max_dr = 10;
  best_track_dr = NAN;

  /* cluster position for easy reference */
  float cx = cluster->get_x();
  float cy = cluster->get_y();

  /* If track map found: Loop over all tracks to find best match for cluster (forward calorimeters)*/
  if ( trackmap &&
       ( caloname == "FEMC" || caloname == "EEMC" ) )
    {
      for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++)
        {
          /* get pointer to track */
          SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);

          /* distance between track and cluster */
          float dr = max_dr;

          /* loop over track states (projections) sotred for this track */
          for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
               state_itr != track->end_states(); state_itr++)
            {
              /* get pointer to current track state */
              SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

              /* check if track state projection name matches calorimeter where cluster was found */
              if( (temp->get_name()==caloname) )
                {
                  dr = sqrt( pow( cx - temp->get_x(), 2 ) + pow( cy - temp->get_y(), 2 ) );
                  break;
                }
            }

          /* check dr and update best_track and best_track_dr if this track is closest to cluster */
          if ( ( best_track_dr != best_track_dr ) ||
	       ( dr < max_dr &&
		 dr < best_track_dr )
	       )
            {
              best_track = track;
              best_track_dr = dr;
            }
        }
    }


  /* If track found with barrel tracking, return it here- if not, proceed with alternative barrel cluster-track matching below. */
  if ( best_track )
    return best_track;

  /* Cluster / track matching for barrel calorimeters and tracking */
  float max_dr_barrel = 10;
  float best_track_dr_barrel = 100 * max_dr_barrel;

  float ctheta = atan2( cluster->get_r() , cluster->get_z() );
  float ceta =  -log( tan( ctheta / 2.0 ) );
  float cphi = cluster->get_phi();

  /* If track map found: Loop over all tracks to find best match for cluster (barrel calorimeters)*/
  if ( trackmap &&
       ( caloname == "CEMC" ) )
    {
      for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++)
        {
          /* get pointer to track */
          SvtxTrack* track =  dynamic_cast<SvtxTrack*>(track_itr->second);

          /* distance between track and cluster */
          float dr = max_dr_barrel;

          /* loop over track states (projections) sotred for this track */
          for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
               state_itr != track->end_states(); state_itr++)
            {
              /* get pointer to current track state */
              SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

              /* check if track state projection name matches calorimeter where cluster was found */
              if( (temp->get_name()==caloname) )
                {
                  dr = sqrt( pow( ceta - temp->get_eta(), 2 ) + pow( cphi - temp->get_phi(), 2 ) );
                  break;
                }
            }

          /* check dr and update best_track and best_track_dr if this track is closest to cluster */
          if ( dr < max_dr_barrel &&
               dr < best_track_dr_barrel )
            {
              best_track = track;
              best_track_dr_barrel = dr;
            }
        }
    }

  return best_track;
}

/*
RawCluster*
void FastTrackingEval:: getCluster( PHCompositeNode *topNode, string detName, float eta, float phi, int secondFlag){

  RawCluster* cluster_min_dr = NULL;

  // pull the clusters
  string clusternodename = "CLUSTER_" + detName;
  RawClusterContainer *clusterList = findNode::getClass<RawClusterContainer>(topNode,clusternodename.c_str());
  if (!clusterList) {
    cerr << PHWHERE << " ERROR: Can't find node " << clusternodename << endl;
    return;
  }

  // loop over all clusters and find nearest
  double min_r = DBL_MAX;
  double min_e = -9999.0;
  int min_n = -1;

  for (unsigned int k = 0; k < clusterList->size(); ++k) {

    RawCluster *cluster = clusterList->getCluster(k);

    double dphi = atan2(sin(phi-cluster->get_phi()),cos(phi-cluster->get_phi()));
    double deta = eta-cluster->get_eta();
    double r = sqrt(pow(dphi,2)+pow(deta,2));

    if (r < min_r) {
      min_r = r;
      min_e = cluster->get_energy();
      min_n = cluster->getNTowers();
    }
  }

  if(detName == "FEMC") {
    if(!secondFlag){
      femc_cldr = min_r;
      femc_clE = min_e;
      femc_clN = min_n;
    }
    else{
      femc_cldr2 = min_r;
      femc_clE2 = min_e;
      femc_clN2 = min_n;
    }
  }
  else{
    if(!secondFlag){
      fhcal_cldr = min_r;
      fhcal_clE = min_e;
      fhcal_clN = min_n;
    }
    else{
      fhcal_cldr2 = min_r;
      fhcal_clE2 = min_e;
      fhcal_clN2 = min_n;
    }
  }

  return cluster_min_dr;
}
*/


float
DISKinematicsReco::getE33Barrel( string detName, float phi, float eta ){

  string towernodename = "TOWER_CALIB_" + detName;
  // Grab the towers
  RawTowerContainer* towerList = findNode::getClass<RawTowerContainer>(_topNode, towernodename.c_str());
  if (!towerList)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return -1;
    }
  string towergeomnodename = "TOWERGEOM_" + detName;
  RawTowerGeomContainer *towergeo = findNode::getClass<RawTowerGeomContainer>(_topNode, towergeomnodename.c_str());
  if (! towergeo)
    {
      cout << PHWHERE << ": Could not find node " << towergeomnodename.c_str() << endl;
      return -1;
    }

  // calculate 3x3 and 5x5 tower energies
  int binphi = towergeo->get_phibin(phi);
  int bineta = towergeo->get_etabin(eta);

  float energy_3x3 = 0.0;
  float energy_5x5 = 0.0;

  for (int iphi = binphi-2; iphi <= binphi+2; ++iphi) {
    for (int ieta = bineta-2; ieta <= bineta+2; ++ieta) {

      // wrap around
      int wrapphi = iphi;
      if (wrapphi < 0) {
        wrapphi = towergeo->get_phibins() + wrapphi;
      }
      if (wrapphi >= towergeo->get_phibins()) {
        wrapphi = wrapphi - towergeo->get_phibins();
      }

      // edges
      if (ieta < 0) continue;
      if (ieta >= towergeo->get_etabins()) continue;

      RawTower* tower = towerList->getTower(ieta,wrapphi);
      if (tower) {
        energy_5x5 += tower->get_energy();
        if (abs(iphi - binphi)<=1 and abs(ieta - bineta)<=1 )
          energy_3x3 += tower->get_energy();
      }

    }
  }

  return energy_3x3;
}


float
DISKinematicsReco::getE33Forward( string detName, float tkx, float tky )
{
  float twr_sum = 0;

  string towernodename = "TOWER_CALIB_" + detName;
  // Grab the towers
  RawTowerContainer* towers = findNode::getClass<RawTowerContainer>(_topNode, towernodename.c_str());
  if (!towers)
    {
      std::cout << PHWHERE << ": Could not find node " << towernodename.c_str() << std::endl;
      return -1;
    }
  string towergeomnodename = "TOWERGEOM_" + detName;
  RawTowerGeomContainer *towergeom = findNode::getClass<RawTowerGeomContainer>(_topNode, towergeomnodename.c_str());
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
  for ( map< TauCandidate::PROPERTY , vector<float> >::iterator iter = _map_em_candidate_branches.begin();
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

  if ( _tree_event )
    _tree_event->Write();

  _tfile->Close();

  return 0;
}
