#include "LeptoquarksReco.h"
#include "PidCandidatev1.h"
#include "TruthTrackerHepMC.h"

/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <trackbase_historic/SvtxTrackMap_v1.h>
#include <trackbase_historic/SvtxVertexMap_v1.h>
//#include <trackbase_historic/SvtxClusterMap_v1.h>

#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <g4jets/JetMap.h>

#include <calobase/RawTowerGeomContainer.h>
#include <calobase/RawTowerContainer.h>
#include <calobase/RawTowerGeom.h>
#include <calobase/RawTowerv1.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4Particle.h>

#include <g4eval/CaloRawTowerEval.h>
#include <g4eval/SvtxEvalStack.h>


/* ROOT includes */
#include <TLorentzVector.h>
#include <TString.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TFile.h>

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
  SubsysReco("LeptoquarksReco" ),
  _save_towers(false),
  _save_tracks(false),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _t_event(nullptr),
  _ntp_tower(nullptr),
  _ntp_track(nullptr),
  _ebeam_E(0),
  _pbeam_E(0),
  _tau_jet_emin(5.0),
  _jetcolname("AntiKt_Tower_r05")
{

}

int
LeptoquarksReco::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* Add PidCandidate properties to map that defines output tree */
  float dummy = 0;
  vector< float > vdummy;

  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_id , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_pid , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_etotal , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_ptotal , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_theta , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_eta , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_phi , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_decay_prong , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_decay_hcharged , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::evtgen_decay_lcharged , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_eta , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_phi , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_etotal , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_etrans , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ptotal , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ptrans , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_minv , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_mtrans , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ncomp , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ncomp_above_0p1 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ncomp_above_1 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ncomp_above_10 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jet_ncomp_emcal , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_econe_r01 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_econe_r02 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_econe_r03 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_econe_r04 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_econe_r05 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_r90 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_rms , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_radius , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_econe_r01 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_econe_r02 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_econe_r03 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_econe_r04 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_econe_r05 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_r90 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_rms , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::jetshape_emcal_radius , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_count_r02 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_chargesum_r02 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_rmax_r02 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_count_r04 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_chargesum_r04 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_rmax_r04 , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_count_R , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_chargesum_R , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_rmax_R , vdummy ) );
  _map_tau_candidate_branches.insert( make_pair( PidCandidate::tracks_vertex , vdummy ) );
  
  /* Add branches to map that defines output tree for event-wise properties */
  _map_event_branches.insert( make_pair( "Et_miss" , dummy ) );
  _map_event_branches.insert( make_pair( "Et_miss_phi" , dummy ) );
  _map_event_branches.insert( make_pair( "reco_tau_found" , dummy ) );
  _map_event_branches.insert( make_pair( "reco_tau_is_tau" , dummy ) );
  _map_event_branches.insert( make_pair( "reco_tau_eta" , dummy ) );
  _map_event_branches.insert( make_pair( "reco_tau_phi" , dummy ) );
  _map_event_branches.insert( make_pair( "reco_tau_ptotal" , dummy ) );
  _map_event_branches.insert( make_pair( "is_lq_event" , dummy ) );


  /* Create tree for information about full event */
  _t_event = new TTree("event", "a Tree with global event information and tau candidates");
  _t_event->Branch("event", &_ievent, "event/I");

  /* Add event branches */
  for ( map< string , float >::iterator iter = _map_event_branches.begin();
        iter != _map_event_branches.end();
        ++iter)
    {
      _t_event->Branch( (iter->first).c_str(),
                        &(iter->second) );
    }

  /* Add tau candidate branches */
  for ( map< PidCandidate::PROPERTY , vector< float > >::iterator iter = _map_tau_candidate_branches.begin();
        iter != _map_tau_candidate_branches.end();
        ++iter)
    {
      _t_event->Branch(PidCandidate::get_property_info( (iter->first) ).first.c_str(),
		       &(iter->second) );
    }


  /* Create additional tree for towers */
  if ( _save_towers )
    {
      _ntp_tower = new TNtuple("ntp_tower","towers from all all tau candidates",
                               "ievent:jet_id:evtgen_pid:evtgen_etotal:evtgen_eta:evtgen_phi:evtgen_decay_prong:evtgen_decay_hcharged:evtgen_decay_lcharged:jet_eta:jet_phi:jet_etotal:tower_calo_id:tower_eta:tower_phi:tower_delta_r:tower_e");
    }

  /* Create additional tree for tracks */
  if ( _save_tracks )
    {
      _ntp_track = new TNtuple("ntp_track","tracks from all all tau candidates",
                               "ievent:jet_id:evtgen_pid:evtgen_etotal:evtgen_eta:evtgen_phi:evtgen_decay_prong:evtgen_decay_hcharged:evtgen_decay_lcharged:jet_eta:jet_phi:jet_etotal:track_quality:track_eta:track_phi:track_delta_r:track_p");
    }

  return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
  /* Reset branch map */
  ResetBranchMap();

  /* Create map to collect tau candidates.
   * Use energy as 'key' to the map because energy is unique for each jet, while there are sometimes multiple jets (same energy,
   * different jet ID) in the input jet collection. Also, map automatically sorts entries by key, i.e. this gives list of tau candidates
   * sorted by energy. */
  type_map_tcan tauCandidateMap;

  /* Get reco jets collection */
  JetMap* recojets = findNode::getClass<JetMap>(topNode,_jetcolname.c_str());
  if (!recojets)
    {
      cerr << PHWHERE << " ERROR: Can't find " << _jetcolname << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(topNode,"SvtxTrackMap");
  if (!trackmap)
    {
      cout<< PHWHERE << "SvtxTrackMap node not found on node tree"
           << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get vertex collection with all vertices in this event */
  SvtxVertexMap* vertexmap =
    findNode::getClass<SvtxVertexMap>(topNode,"SvtxVertexMap");
  if (!vertexmap)
    {
      cout<< PHWHERE << "SvtxVertexMap node not found on node tree"
          << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }
    
  
    /* Get collection of truth particles from event generator */
  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode,"PHHepMCGenEventMap");
  if (!genevtmap) {
    cerr << PHWHERE << " ERROR: Can't find PHHepMCGenEventMap" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /* create map of all tower collections and tower geometry collections used */
  type_map_cdata map_calotower;

  /* Select calorimeter to include */
  vector< RawTowerDefs::CalorimeterId > v_caloids;
  v_caloids.push_back( RawTowerDefs::CEMC );
  v_caloids.push_back( RawTowerDefs::HCALIN );
  v_caloids.push_back( RawTowerDefs::HCALOUT );
  v_caloids.push_back( RawTowerDefs::FEMC );
  v_caloids.push_back( RawTowerDefs::FHCAL );
  v_caloids.push_back( RawTowerDefs::EEMC );

  /* Fill map with calorimeter data */
  for ( unsigned i = 0; i < v_caloids.size(); i++ )
    {
      /* Get collection of towers */
      string towers_name = "TOWER_CALIB_";
      towers_name.append( RawTowerDefs::convert_caloid_to_name( v_caloids.at( i ) ) );

      RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode,towers_name);
      if (!towers) {
        cerr << PHWHERE << " ERROR: Can't find " << towers_name << endl;
        return Fun4AllReturnCodes::ABORTEVENT;
      }

      /* get geometry collection */
      string towergeom_name = "TOWERGEOM_";
      towergeom_name.append( RawTowerDefs::convert_caloid_to_name( v_caloids.at( i ) ) );

      RawTowerGeomContainer *geom = findNode::getClass<RawTowerGeomContainer>(topNode,towergeom_name);
      if (!geom) {
        cerr << PHWHERE << " ERROR: Can't find " << towergeom_name << endl;
        return Fun4AllReturnCodes::ABORTEVENT;
      }

      /* Insert tower and geometry collections in map */
      map_calotower.insert( make_pair( v_caloids.at( i ), make_pair( towers, geom ) ) );
    }


  /* Loop over all jets to collect tau candidate objects */
  for (JetMap::Iter iter = recojets->begin();
       iter != recojets->end();
       ++iter)
    {
      /* skip jets below energy threshold */
      if ( (iter->second)->get_e() < _tau_jet_emin )
        continue;

      /* create new tau candidate */
      PidCandidatev1 *tc = new PidCandidatev1();
      tc->set_candidate_id( (iter->second)->get_id() );
      tc->set_property( PidCandidate::jet_id , (iter->second)->get_id() );

      tc->set_property( PidCandidate::evtgen_pid, (int)0 );

      /* add tau candidate to collection */
      tauCandidateMap.insert( make_pair( (iter->second)->get_e(), tc ) );
    }
  
  /* vertex information */
  SvtxEvalStack *svtxevalstack = new SvtxEvalStack(topNode);
  
  /* Add jet information to tau candidates */
  AddJetInformation( tauCandidateMap, recojets, &map_calotower );

  /* Add jet structure information to tau candidates */
  AddJetStructureInformation( tauCandidateMap, &map_calotower );

  /* Add track information to tau candidates */
  AddTrackInformation( tauCandidateMap, trackmap, vertexmap, svtxevalstack, recojets->get_par());  

  /* Add tag for true Tau particle jet to tau candidates */
  AddTrueTauTag( tauCandidateMap, genevtmap );

  /* Add information about tau candidats to output tree */
  WritePidCandidatesToTree( tauCandidateMap );
  
  /* Add global event information to separate tree */
  AddGlobalEventInformation( tauCandidateMap, &map_calotower );

  /* fill event information tree */
  _t_event->Fill();

  /* count up event number */
  _ievent ++;

  return 0;
}

// Returns average of values while excluding outliers //
float Average(vector<float> v)
{
  double sum = 0;
  int med_i = v.size()/2;
  vector<float> temp_v;
  for(int i=0;(unsigned)i<v.size();i++)
    if (v[i] > (v[med_i] - v[med_i]*0.2) && v[i] < (v[med_i] + v[med_i]*0.2)) temp_v.push_back(v[i]);
  
  for(int j=0;(unsigned)j<temp_v.size();j++) sum = sum + temp_v[j];

  return sum/temp_v.size();
}

int
LeptoquarksReco::AddTrueTauTag( type_map_tcan& tauCandidateMap, PHHepMCGenEventMap *genevtmap )
{
  /* Look for leptoquark and tau particle in the event */
  TruthTrackerHepMC truth;
  truth.set_hepmc_geneventmap( genevtmap );

  int pdg_lq = 39; // leptoquark                                                                                                                                                                          
  int pdg_tau = 15; // tau lepton                                                                                                                                                                         
  int pdg_parton = 0;
  int pdg_electron = 11; // electron

  // Check if electron exists  //                                                                                                                                                                
  HepMC::GenParticle* particle_electron = NULL;
  
  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
    {
      PHHepMCGenEvent *genevt = iter->second;
      HepMC::GenEvent *theEvent = genevt->getEvent();
            
      // Loop over all truth particles in event generator collection 
      for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
	    p != theEvent->particles_end(); ++p ) {
	// If final state electron then tag it //
	if((*p)->pdg_id() == pdg_electron && (*p)->status() == 1) particle_electron = (*p);
      }
    }
            
  /* Search for leptoquark in event */
  HepMC::GenParticle* particle_lq = truth.FindParticle( pdg_lq );

  // Tag event as LQ event or not //
  if (particle_lq) ( _map_event_branches.find( "is_lq_event" ) )->second = 1;
  else ( _map_event_branches.find( "is_lq_event" ) )->second = 0;

  /* Search for lq->tau decay in event */
  HepMC::GenParticle* particle_tau = NULL;

  // Find tau that comes from LQ or just any tau in event //
  if(particle_lq) particle_tau = truth.FindDaughterParticle( pdg_tau, particle_lq );
  else particle_tau = truth.FindParticle(pdg_tau);
  
  /* Search for lq->quark decay in event.
   * Loop over all quark PDG codes until finding a matching quark. */
  HepMC::GenParticle* particle_quark = NULL;
  for ( int pdg_quark = 1; pdg_quark < 7; pdg_quark++ )
    {
      /* try quark */
      particle_quark = truth.FindDaughterParticle( pdg_quark, particle_lq );
      if (particle_quark)
        {
          pdg_parton = pdg_quark;
          break;
        }

      /* try anti-quark */
      particle_quark = truth.FindDaughterParticle( -pdg_quark, particle_lq );
      if (particle_quark)
        {
          pdg_parton = -pdg_quark;
          break;
        }
    }

  /* If TAU in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this tau */
  if( particle_tau )
    {
      PidCandidate* best_match = FindMinDeltaRCandidate( &tauCandidateMap,
                                                         particle_tau->momentum().eta(),
                                                         particle_tau->momentum().phi() );

      
      /* set is_tau = TRUE for PidCandiate with smallest delta_R within reasonable range*/
      if ( best_match )
        {
	  /* Check: If PidCandidate::Evtgen_pid has already been set to a value != 0, exit function. */
	  if( best_match->get_property_int( PidCandidate::evtgen_pid ) != 0 )
	    {
	      cout << "ERROR: Try to set PidCandidate::evtgen_pid for PidCandidate with evtgen_pid != 0" << endl;
	      return -1;
	    }

          /* Update PidCandidate entry */
          best_match->set_property( PidCandidate::evtgen_pid, pdg_tau );
          best_match->set_property( PidCandidate::evtgen_etotal, (float)particle_tau->momentum().e() );
          best_match->set_property( PidCandidate::evtgen_eta, (float)particle_tau->momentum().eta() );
          best_match->set_property( PidCandidate::evtgen_phi, (float)particle_tau->momentum().phi() );
	  
          /* Check particle decay if end-vertex found */
          if ( particle_tau->end_vertex() )
            {
              /* Add information about tau decay */
              uint tau_decay_prong = 0;
              uint tau_decay_hcharged = 0;
              uint tau_decay_lcharged = 0;

	      /* Count how many charged particles (hadrons and leptons) a given particle decays into. */
              truth.FindDecayParticles( particle_tau, tau_decay_prong, tau_decay_hcharged, tau_decay_lcharged );


              /* Update tau candidate entry */
              best_match->set_property( PidCandidate::evtgen_decay_prong, tau_decay_prong );
              best_match->set_property( PidCandidate::evtgen_decay_hcharged, tau_decay_hcharged );
              best_match->set_property( PidCandidate::evtgen_decay_lcharged, tau_decay_lcharged );
            }
        }
    }

  /* If QUARK (->jet) in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this quark */
  if( particle_quark )
    {
      PidCandidate* best_match = FindMinDeltaRCandidate( &tauCandidateMap,
                                                         particle_quark->momentum().eta(),
                                                         particle_quark->momentum().phi() );
      
      /* set is_uds = TRUE for PidCandiate with smallest delta_R if found */
      if ( best_match )
        {
	  /* Check: If PidCandidate::Evtgen_pid has already been set to a value != 0, exit function. */
	  if( best_match->get_property_int( PidCandidate::evtgen_pid ) != 0 )
	    {
	      cout << "ERROR: Try to set PidCandidate::evtgen_pid for PidCandidate with evtgen_pid != 0" << endl;
	      return -1;
	    }
	  /* Set properties of PidCandidate */
          best_match->set_property( PidCandidate::evtgen_pid, pdg_parton );
          best_match->set_property( PidCandidate::evtgen_etotal, (float)particle_quark->momentum().e() );
          best_match->set_property( PidCandidate::evtgen_eta, (float)particle_quark->momentum().eta() );
          best_match->set_property( PidCandidate::evtgen_phi, (float)particle_quark->momentum().phi() );
        }
    }

  if( particle_electron )
    {
      //cout<<"ELECTRON"<<endl;
      PidCandidate* best_match = FindMinDeltaRCandidate( &tauCandidateMap,
                                                         particle_electron->momentum().eta(),
                                                         particle_electron->momentum().phi() );

      /* set electron = TRUE for PidCandiate with smallest delta_R if found */
      if ( best_match )
        {
          /* Check: If PidCandidate::Evtgen_pid has already been set to a value != 0, exit function. */
          if( best_match->get_property_int( PidCandidate::evtgen_pid ) != 0 )
            {
              cout << "ERROR: Try to set PidCandidate::evtgen_pid for PidCandidate with evtgen_pid != 0" << endl;
              return -1;
            }

          /* Set properties of PidCandidate */
          best_match->set_property( PidCandidate::evtgen_pid, pdg_electron );
          best_match->set_property( PidCandidate::evtgen_etotal, (float)particle_electron->momentum().e() );
          best_match->set_property( PidCandidate::evtgen_eta, (float)particle_electron->momentum().eta() );
          best_match->set_property( PidCandidate::evtgen_phi, (float)particle_electron->momentum().phi() );
        }
    }


  return 0;
}


int
LeptoquarksReco::AddJetInformation( type_map_tcan& tauCandidateMap, JetMap* recojets, type_map_cdata* map_calotower )
{
  /* Loop over tau candidates */
  for (type_map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      Jet* jetx = recojets->get( (iter->second)->get_property_uint( PidCandidate::jet_id ) );

      /* calculate transverse mass of jet */
      float jet_mtrans = sqrt( pow( jetx->get_mass(), 2 ) +
                               pow( jetx->get_pt(), 2 ) );

      /* count jet ncomp above thresholds */
      unsigned int jet_ncomp_above_0p1 = 0;
      unsigned int jet_ncomp_above_1 = 0;
      unsigned int jet_ncomp_above_10 = 0;

      for (Jet::ConstIter jcompiter = jetx->begin_comp(); jcompiter != jetx->end_comp(); ++jcompiter)
        {
          RawTowerDefs::CalorimeterId calo_id = RawTowerDefs::NONE;

          switch ( jcompiter->first )
            {
            case Jet::CEMC_TOWER:
              calo_id = RawTowerDefs::CEMC;
              break;
            case Jet::HCALIN_TOWER:
              calo_id = RawTowerDefs::HCALIN;
              break;
            case Jet::HCALOUT_TOWER:
              calo_id = RawTowerDefs::HCALOUT;
              break;
            default:
              break;
            }

          /* continue if no calorimeter id found */
          if ( calo_id == RawTowerDefs::NONE )
            continue;

          /* get tower container from map, find tower in tower container, get tower energy */
          float e_component = 0;
          if ( map_calotower->find( calo_id ) != map_calotower->end() )
            e_component = ( ( ( map_calotower->find( calo_id ) )->second ).first )->getTower( jcompiter->second )->get_energy();

          /* check if energy is above threshold and count up matching counters accordingly */
          if ( e_component > 0.1 )
            jet_ncomp_above_0p1++;
          if ( e_component > 1 )
            jet_ncomp_above_1++;
          if ( e_component > 10 )
            jet_ncomp_above_10++;
        }

      /* set tau candidate jet properties */
      (iter->second)->set_property( PidCandidate::jet_eta , jetx->get_eta() );
      (iter->second)->set_property( PidCandidate::jet_phi , jetx->get_phi() );
      (iter->second)->set_property( PidCandidate::jet_etotal , jetx->get_e() );
      (iter->second)->set_property( PidCandidate::jet_etrans , jetx->get_et() );
      (iter->second)->set_property( PidCandidate::jet_ptotal , jetx->get_p() );
      (iter->second)->set_property( PidCandidate::jet_ptrans , jetx->get_pt() );
      (iter->second)->set_property( PidCandidate::jet_minv , jetx->get_mass() );
      (iter->second)->set_property( PidCandidate::jet_mtrans , jet_mtrans );
      (iter->second)->set_property( PidCandidate::jet_ncomp , (uint)jetx->size_comp() );
      (iter->second)->set_property( PidCandidate::jet_ncomp_above_0p1 , jet_ncomp_above_0p1 );
      (iter->second)->set_property( PidCandidate::jet_ncomp_above_1 , jet_ncomp_above_1 );
      (iter->second)->set_property( PidCandidate::jet_ncomp_above_10 , jet_ncomp_above_10 );
      (iter->second)->set_property( PidCandidate::jet_ncomp_emcal , (uint)jetx->count_comp( Jet::CEMC_TOWER ) );
    }

  return 0;
}



int
LeptoquarksReco::AddJetStructureInformation( type_map_tcan& tauCandidateMap, type_map_cdata* map_towers )
{
  /* Cone size around jet axis within which to look for tracks */
  float delta_R_cutoff_r1 = 0.1;
  float delta_R_cutoff_r2 = 0.2;
  float delta_R_cutoff_r3 = 0.3;
  float delta_R_cutoff_r4 = 0.4;
  float delta_R_cutoff_r5 = 0.5;

  /* energy threshold for considering tower */
  float tower_emin = 0.0;

  /* number of steps for finding r90 */
  int n_steps = 50;

  /* Loop over tau candidates */
  for (type_map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      /* get jet axis */
      float jet_eta = (iter->second)->get_property_float( PidCandidate::jet_eta );
      float jet_phi = (iter->second)->get_property_float( PidCandidate::jet_phi );
      float jet_e =   (iter->second)->get_property_float( PidCandidate::jet_etotal );

      /* collect jet structure properties */
      float er1 = 0;
      float er2 = 0;
      float er3 = 0;
      float er4 = 0;
      float er5 = 0;
      float r90 = 0;
      float radius = 0;
      float rms = 0;
      float rms_esum = 0;

      /* collect jet structure properties- EMCal ONLY */
      float emcal_er1 = 0;
      float emcal_er2 = 0;
      float emcal_er3 = 0;
      float emcal_er4 = 0;
      float emcal_er5 = 0;
      float emcal_r90 = 0;
      float emcal_radius = 0;
      float emcal_rms = 0;
      float emcal_rms_esum = 0;

      /* Loop over all tower (and geometry) collections */
      for (type_map_cdata::iterator iter_calo = map_towers->begin();
           iter_calo != map_towers->end();
           ++iter_calo)
        {
          /* define tower iterator */
          RawTowerContainer::ConstRange begin_end = ((iter_calo->second).first)->getTowers();
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
              RawTowerGeom * tower_geom = ((iter_calo->second).second)->get_tower_geometry(tower -> get_key());
              float tower_eta = tower_geom->get_eta();
              float tower_phi = tower_geom->get_phi();

              /* If accounting for displaced vertex, need to calculate eta and phi:
                 double r = tower_geom->get_center_radius();
                 double phi = atan2(tower_geom->get_center_y(), tower_geom->get_center_x());
                 double z0 = tower_geom->get_center_z();
                 double z = z0 - vtxz;
                 double eta = asinh(z/r); // eta after shift from vertex
              */

              float delta_R = CalculateDeltaR( tower_eta , tower_phi, jet_eta, jet_phi );

              /* if save_towers set true: add tower to tree */
              if ( _save_towers )
                {
                  float tower_data[17] = {(float) _ievent,
                                          (float) (iter->second)->get_property_uint( PidCandidate::jet_id ),
                                          (float) (iter->second)->get_property_int( PidCandidate::evtgen_pid ),
                                          (float) (iter->second)->get_property_float( PidCandidate::evtgen_etotal ),
                                          (float) (iter->second)->get_property_float( PidCandidate::evtgen_eta ),
                                          (float) (iter->second)->get_property_float( PidCandidate::evtgen_phi ),
                                          (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_prong ),
                                          (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_hcharged ),
                                          (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_lcharged ),
                                          (float) (iter->second)->get_property_float( PidCandidate::jet_eta ),
                                          (float) (iter->second)->get_property_float( PidCandidate::jet_phi ),
                                          (float) (iter->second)->get_property_float( PidCandidate::jet_etotal ),
                                          (float) (iter_calo->first),
                                          (float) tower_eta,
                                          (float) tower_phi,
                                          (float) delta_R,
                                          (float) tower_energy
                  };

                  _ntp_tower->Fill(tower_data);
                }

              /* check delta R distance tower from jet axis */
              if ( delta_R <= delta_R_cutoff_r1 )
                {
                  er1 += tower_energy;
                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    emcal_er1 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r2 )
                {
                  er2 += tower_energy;
                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    emcal_er2 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r3 )
                {
                  er3 += tower_energy;
                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    emcal_er3 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r4 )
                {
                  er4 += tower_energy;
                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    emcal_er4 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r5 )
                {
                  er5 += tower_energy;
                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    emcal_er5 += tower_energy;
                }

              if ( delta_R <= delta_R_cutoff_r5 )
                {
                  rms += tower_energy*delta_R*delta_R;
                  rms_esum += tower_energy;

                  radius += tower_energy*delta_R;

                  if ( (iter_calo->first) == RawTowerDefs::CEMC )
                    {
                      emcal_rms += tower_energy*delta_R*delta_R;
                      emcal_rms_esum += tower_energy;
                      emcal_radius += tower_energy*delta_R;
                    }
                }
            }
        }

      /* finalize calculation of rms and radius */
      if ( rms_esum > 0 )
        {
          radius /= rms_esum;
          rms    /= rms_esum;
          rms     = sqrt( rms );
        }
      else
        {
          radius = -1;
          rms = -1;
        }
      if ( emcal_rms_esum > 0 )
        {
          emcal_radius /= emcal_rms_esum;
          emcal_rms    /= emcal_rms_esum;
          emcal_rms     = sqrt( emcal_rms );
        }
      else
        {
          emcal_radius = -1;
          emcal_rms = -1;
        }

      /* Search for cone angle that contains 90% of jet energy */
      for(int r_i = 1; r_i<n_steps+1; r_i++){
        float e_tower_sum = 0;

        /* Loop over all tower (and geometry) collections */
        for (type_map_cdata::iterator iter_calo = map_towers->begin();
             iter_calo != map_towers->end();
             ++iter_calo)
          {
            /* define tower iterator */
            RawTowerContainer::ConstRange begin_end = ((iter_calo->second).first)->getTowers();
            RawTowerContainer::ConstIterator rtiter;

            if (e_tower_sum >= 0.9*jet_e) break;

            for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter)
              {
                RawTower *tower = rtiter->second;
                float tower_energy = tower->get_energy();

                /* check if tower is above minimum tower energy required */
                if ( tower_energy < tower_emin )
                  continue;

                RawTowerGeom * tower_geom = ((iter_calo->second).second)->get_tower_geometry(tower -> get_key());
                float tower_eta = tower_geom->get_eta();
                float tower_phi = tower_geom->get_phi();

                float delta_R = CalculateDeltaR( tower_eta , tower_phi, jet_eta, jet_phi );

                if(delta_R < r_i*delta_R_cutoff_r5/n_steps) {
                  e_tower_sum = e_tower_sum + tower_energy;
                  r90 = r_i*delta_R_cutoff_r5/n_steps;
                }
              }

            if (e_tower_sum >= 0.9*jet_e) break;
          }
      }

      /* set tau candidate properties */
      (iter->second)->set_property( PidCandidate::jetshape_econe_r01, er1 );
      (iter->second)->set_property( PidCandidate::jetshape_econe_r02, er2 );
      (iter->second)->set_property( PidCandidate::jetshape_econe_r03, er3 );
      (iter->second)->set_property( PidCandidate::jetshape_econe_r04, er4 );
      (iter->second)->set_property( PidCandidate::jetshape_econe_r05, er5 );
      (iter->second)->set_property( PidCandidate::jetshape_r90, r90 );
      (iter->second)->set_property( PidCandidate::jetshape_rms, rms );
      (iter->second)->set_property( PidCandidate::jetshape_radius, radius );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_econe_r01, emcal_er1 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_econe_r02, emcal_er2 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_econe_r03, emcal_er3 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_econe_r04, emcal_er4 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_econe_r05, emcal_er5 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_r90, emcal_r90 );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_rms, emcal_rms );
      (iter->second)->set_property( PidCandidate::jetshape_emcal_radius, emcal_radius );
    }

  return 0;
}

int
LeptoquarksReco::AddTrackInformation( type_map_tcan& tauCandidateMap, SvtxTrackMap* trackmap, SvtxVertexMap* vertexmap, SvtxEvalStack *svtxevalstack, double R_max )
{
  
  // Pointers for tracks //
  SvtxTrackEval* trackeval = svtxevalstack->get_track_eval();
  SvtxTruthEval* trutheval = svtxevalstack->get_truth_eval();

  /* Loop over tau candidates */
  for (type_map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      uint tracks_count_r02 = 0;
      int tracks_chargesum_r02 = 0;
      float tracks_rmax_r02 = 0;

      uint tracks_count_r04 = 0;
      int tracks_chargesum_r04 = 0;
      float tracks_rmax_r04 = 0;

      uint tracks_count_R = 0;
      int tracks_chargesum_R = 0;
      float tracks_rmax_R = 0;

      vector<float> tracks_vertex;
      vector<float> temp_vertex;

      float jet_eta = (iter->second)->get_property_float( PidCandidate::jet_eta );
      float jet_phi = (iter->second)->get_property_float( PidCandidate::jet_phi );
      
      /* Loop over tracks
       * (float) track->get_eta(),     //eta of the track
       * (float) track->get_phi(),     //phi of the track
       * (float) track->get_pt(),      //transverse momentum of track
       * (float) track->get_p(),       //total momentum of track
       * (float) track->get_charge(),  //electric charge of track
       * (float) track->get_quality()  //track quality */
  
      //Loop over tracks in event //
    for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++) {

        SvtxTrack* track = dynamic_cast<SvtxTrack*>(track_itr->second);

	// Get track variables //
        float track_eta = track->get_eta();
        float track_phi = track->get_phi();
        int track_charge = track->get_charge();
	double gvx,gvy,gvz;
	  
        float delta_R = CalculateDeltaR( track_eta, track_phi, jet_eta, jet_phi );

	PHG4Particle* g4particle = trackeval->max_truth_particle_by_nclusters(track);

	// Get true vertex distances //	  
	PHG4VtxPoint* vtx = trutheval->get_vertex(g4particle);
	gvx = vtx->get_x();
	gvy = vtx->get_y();
	gvz = vtx->get_z();
	

	
	// If charged track is within jet then use its vertex //
	if(delta_R < 0.5 && trutheval->is_primary(g4particle)) tracks_vertex.push_back(sqrt(pow(gvx,2)+pow(gvy,2)+pow(gvz,2)));
	
	/* if save_tracks set true: add track to tree */
        if ( _save_tracks )
          {
            float track_data[17] = {(float) _ievent,
                                    (float) (iter->second)->get_property_uint( PidCandidate::jet_id ),
                                    (float) (iter->second)->get_property_int( PidCandidate::evtgen_pid ),
                                    (float) (iter->second)->get_property_float( PidCandidate::evtgen_etotal ),
                                    (float) (iter->second)->get_property_float( PidCandidate::evtgen_eta ),
                                    (float) (iter->second)->get_property_float( PidCandidate::evtgen_phi ),
                                    (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_prong ),
                                    (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_hcharged ),
                                    (float) (iter->second)->get_property_uint( PidCandidate::evtgen_decay_lcharged ),
                                    (float) (iter->second)->get_property_float( PidCandidate::jet_eta ),
                                    (float) (iter->second)->get_property_float( PidCandidate::jet_phi ),
                                    (float) (iter->second)->get_property_float( PidCandidate::jet_etotal ),
                                    (float) track->get_quality(),
                                    (float) track_eta,
                                    (float) track_phi,
                                    (float) delta_R,
                                    (float) track->get_p()
            };

            _ntp_track->Fill(track_data);
          }

        /* If track within search cone, update track information for tau candidate */
        if ( delta_R < 0.2 )
          {
            tracks_count_r02++;
            tracks_chargesum_r02 += track_charge;

            if ( delta_R > tracks_rmax_r02 )
              tracks_rmax_r02 = delta_R;
          }

        if ( delta_R < 0.4 )
          {
            tracks_count_r04++;
            tracks_chargesum_r04 += track_charge;

            if ( delta_R > tracks_rmax_r04 )
              tracks_rmax_r04 = delta_R;
          }

	if ( delta_R < R_max )
          {
            tracks_count_R++;
            tracks_chargesum_R += track_charge;

            if ( delta_R > tracks_rmax_R )
              tracks_rmax_R = delta_R;
          }

    } // end loop over reco tracks //
    // sort vertex array in increasing order //
    std::sort(tracks_vertex.begin(),tracks_vertex.end());
    
    // Compute average vertex distance of tracks in jet //
    float avg = Average(tracks_vertex);

    /* Set track-based properties for tau candidate */
      (iter->second)->set_property( PidCandidate::tracks_count_r02, tracks_count_r02 );
      (iter->second)->set_property( PidCandidate::tracks_chargesum_r02, tracks_chargesum_r02 );
      (iter->second)->set_property( PidCandidate::tracks_rmax_r02, tracks_rmax_r02 );
      (iter->second)->set_property( PidCandidate::tracks_count_r04, tracks_count_r04 );
      (iter->second)->set_property( PidCandidate::tracks_chargesum_r04, tracks_chargesum_r04 );
      (iter->second)->set_property( PidCandidate::tracks_rmax_r04, tracks_rmax_r04 );
      (iter->second)->set_property( PidCandidate::tracks_count_R, tracks_count_R );
      (iter->second)->set_property( PidCandidate::tracks_chargesum_R, tracks_chargesum_R );
      (iter->second)->set_property( PidCandidate::tracks_rmax_R, tracks_rmax_R );
      if(avg == avg) (iter->second)->set_property( PidCandidate::tracks_vertex, avg);

    } // end loop over tau  candidates

  return 0;
}


int
LeptoquarksReco::WritePidCandidatesToTree( type_map_tcan& tauCandidateMap )
{
  /* Loop over all tau candidates and add them to tree*/
  for (type_map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      /* update information in map and fill tree */
      for ( map< PidCandidate::PROPERTY , vector<float> >::iterator iter_prop = _map_tau_candidate_branches.begin();
            iter_prop != _map_tau_candidate_branches.end();
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
LeptoquarksReco::FindMinDeltaRCandidate( type_map_tcan *candidates, const float eta_ref, const float phi_ref )
{
  /* PidCandidate with eta, phi closest to reference */
  PidCandidate* best_candidate = NULL;

  float eta_ref_local = eta_ref;
  float phi_ref_local = phi_ref;

  /* Event record uses 0 < phi < 2Pi, while Fun4All uses -Pi < phi < Pi.
   * Therefore, correct angle for 'wraparound' at phi == Pi */
  if( phi_ref_local > TMath::Pi() ) phi_ref_local = phi_ref_local - 2*TMath::Pi();

  /* find jet with smallest delta_R from quark */
  float min_delta_R = 100;
  type_map_tcan::iterator min_delta_R_iter = candidates->end();

  for (type_map_tcan::iterator iter = candidates->begin();
       iter != candidates->end();
       ++iter)
    {
      float eta = (iter->second)->get_property_float( PidCandidate::jet_eta );
      float phi = (iter->second)->get_property_float( PidCandidate::jet_phi );

      float delta_R = CalculateDeltaR( eta, phi, eta_ref_local, phi_ref_local );
      //cout<<delta_R<<endl;
      if ( delta_R < min_delta_R )
        {
          min_delta_R_iter = iter;            ;
          min_delta_R = delta_R;
        }
    }

  /* set best_candidate to PidCandiate with smallest delta_R within reasonable range*/
  if ( min_delta_R_iter != candidates->end() && min_delta_R < 0.5 )
    best_candidate = min_delta_R_iter->second;
  //cout<<"Min R: "<<min_delta_R<<endl;
  return best_candidate;
}


float
LeptoquarksReco::CalculateDeltaR( float eta1, float phi1, float eta2, float phi2 )
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
LeptoquarksReco::AddGlobalEventInformation( type_map_tcan& tauCandidateMap, type_map_cdata* map_towers )
{
  /* missing transverse momentum */
  //float pt_miss = 0;

  /* direction of missing transverse momentum */
  //float pt_miss_phi = 0;

  /* Missing transverse energy, transverse energy direction, and Energy sums, x and y components separately */
  float Et_miss = 0;
  float Et_miss_phi = 0;
  float Ex_sum = 0;
  float Ey_sum = 0;

  /* energy threshold for considering tower */
  float tower_emin = 0.0;

  /* Loop over all tower (and geometry) collections */
  for (type_map_cdata::iterator iter_calo = map_towers->begin();
       iter_calo != map_towers->end();
       ++iter_calo)
    {

      /* define tower iterator */
      RawTowerContainer::ConstRange begin_end = ((iter_calo->second).first)->getTowers();
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
          RawTowerGeom * tower_geom = ((iter_calo->second).second)->get_tower_geometry(tower -> get_key());
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
  
  /* calculate Et_miss and phi angle*/
  Et_miss = sqrt( Ex_sum * Ex_sum + Ey_sum * Ey_sum );
  Et_miss_phi = atan2( Ey_sum , Ex_sum );

  /* Loop over tau candidates and find tau jet*/
  PidCandidate* the_tau = NULL;
  for (type_map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      if ( ( iter->second)->get_property_int( PidCandidate::evtgen_pid ) == 15 )
        the_tau = iter->second;
    }
  
  /* update event information tree variables */
  /* @TODO make this better protected against errors- if 'find' returns NULL pointer,
     this will lead to a SEGMENTATION FAULT */
  ( _map_event_branches.find( "Et_miss" ) )->second = Et_miss;
  ( _map_event_branches.find( "Et_miss_phi" ) )->second = Et_miss_phi;

  // If tau is found, write variables //
  if ( the_tau )
    {
      ( _map_event_branches.find( "reco_tau_found" ) )->second = 1;
      ( _map_event_branches.find( "reco_tau_is_tau" ) )->second =
        the_tau->get_property_int( PidCandidate::evtgen_pid );
      ( _map_event_branches.find( "reco_tau_eta" ) )->second =
        the_tau->get_property_float( PidCandidate::jet_eta );
      ( _map_event_branches.find( "reco_tau_phi" ) )->second =
        the_tau->get_property_float( PidCandidate::jet_phi );
      ( _map_event_branches.find( "reco_tau_ptotal" ) )->second =
        the_tau->get_property_float( PidCandidate::jet_ptotal );
    }
  else
    {
      ( _map_event_branches.find( "reco_tau_found" ) )->second = 0;
      ( _map_event_branches.find( "reco_tau_is_tau" ) )->second = NAN;
      ( _map_event_branches.find( "reco_tau_eta" ) )->second = NAN;
      ( _map_event_branches.find( "reco_tau_phi" ) )->second = NAN;
      ( _map_event_branches.find( "reco_tau_ptotal" ) )->second = NAN;
    }

  return 0;
}


void
LeptoquarksReco::ResetBranchMap()
{
  /* Event branches */
  for ( map< string , float >::iterator iter = _map_event_branches.begin();
        iter != _map_event_branches.end();
        ++iter)
    {
      (iter->second) = NAN;
    }

  /* Tau candidate branches */
  for ( map< PidCandidate::PROPERTY , vector<float> >::iterator iter = _map_tau_candidate_branches.begin();
        iter != _map_tau_candidate_branches.end();
        ++iter)
    {
      (iter->second).clear();
    }

  return;
}


int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();

  if ( _t_event )
    _t_event->Write();

  if ( _ntp_tower )
    _ntp_tower->Write();

  if ( _ntp_track )
    _ntp_track->Write();

  _tfile->Close();

  return 0;
}
