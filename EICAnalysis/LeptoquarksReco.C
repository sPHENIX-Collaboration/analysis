#include "LeptoquarksReco.h"
#include "TauCandidate.h"

/* STL includes */
#include <cassert>

/* Fun4All includes */
#include <g4hough/SvtxTrackMap_v1.h>

//#include <phhepmc/PHHepMCGenEvent.h>
//#include <phhepmc/PHHepMCGenEventMap.h>

#include <phool/getClass.h>

#include <fun4all/Fun4AllReturnCodes.h>

#include <g4jets/JetMap.h>

#include <g4cemc/RawTowerGeomContainer.h>
#include <g4cemc/RawTowerContainer.h>
#include <g4cemc/RawTowerGeom.h>
#include <g4cemc/RawTowerv1.h>

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4main/PHG4Particle.h>

#include <g4eval/CaloRawTowerEval.h>

/* ROOT includes */
#include <TLorentzVector.h>
#include <TDatabasePDG.h>
#include <TString.h>
#include <TNtuple.h>
#include <TFile.h>
#include <TDatabasePDG.h>

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
  SubsysReco("LeptoquarksReco" ),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _ntp_jet(nullptr),
  _ntp_jet2(nullptr),
  _ebeam_E(0),
  _pbeam_E(0),
  _jetcolname("AntiKt_Tower_r05")
{

}

int
LeptoquarksReco::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* NTuple to store tau candidate information */
  _ntp_jet = new TNtuple("ntp_jet","all jet information from LQ events",
                         "event:jet_id:isMaxEnergyJet:isMinDeltaRJet:jet_eta:jet_phi:delta_R:jet_mass:jet_p:jet_pT:jet_eT:jet_e:jet_px:jet_py:jet_pz");

  _ntp_jet2 = new TNtuple("ntp_jet2","all tau candidate (jet) information from LQ events",
                          "ievent:jet_id:is_tau:is_uds:tau_etotal:tau_eta:tau_phi:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_etotal:uds_eta:uds_phi:jet_eta:jet_phi:jet_etotal:jet_etrans:jet_ptotal:jet_ptrans:jet_mass:jetshape_econe_r1:jetshape_econe_r2:jetshape_econe_r3:jetshape_r90:jetshape_rms:jetshape_radius:tracks_count:tracks_chargesum:tracks_rmax");

  return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
  /* set some values- @TODO: should this be done elsewhere? Member variable? */
  double _tau_jet_emin = 5;

  /* Create map to collect tau candidates.
   * Use energy as 'key' to the map because energy is unique for each jet, while there are sometimes multiple jets (same energy,
   * different jet ID) in the input jet collection. Also, map automatically sorts entries by key, i.e. this gives list of tau candidates
   * sorted by energy. */
  map_tcan tauCandidateMap;

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
      cout << PHWHERE << "SvtxTrackMap node not found on node tree"
           << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

  /* Get collection of truth particles from event generator */
  PHHepMCGenEventMap *genevtmap = findNode::getClass<PHHepMCGenEventMap>(topNode,"PHHepMCGenEventMap");
  if (!genevtmap) {
    cerr << PHWHERE << " ERROR: Can't find PHHepMCGenEventMap" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /* create vectors of all tower collections and tower geometry collections used */
  vector<string> v_calonames;
  v_calonames.push_back("CEMC");
  v_calonames.push_back("HCALIN");
  v_calonames.push_back("HCALOUT");

  vector<RawTowerContainer*> v_towers;
  vector<RawTowerGeomContainer*> v_tower_geoms;

  for ( unsigned i = 0; i < v_calonames.size(); i++ )
    {
      /* Get collection of CEMC towers */
      string towers_name = "TOWER_CALIB_";
      towers_name.append( v_calonames.at(i) );

      RawTowerContainer *towers = findNode::getClass<RawTowerContainer>(topNode,towers_name);
      if (!towers) {
        cerr << PHWHERE << " ERROR: Can't find " << towers_name << endl;
        return Fun4AllReturnCodes::ABORTEVENT;
      }

      v_towers.push_back( towers );

      /* get CEMC geometry collection */
      string towergeom_name = "TOWERGEOM_";
      towergeom_name.append( v_calonames.at(i) );

      RawTowerGeomContainer *geom = findNode::getClass<RawTowerGeomContainer>(topNode,towergeom_name);
      if (!geom) {
        cerr << PHWHERE << " ERROR: Can't find " << towergeom_name << endl;
        return Fun4AllReturnCodes::ABORTEVENT;
      }

      v_tower_geoms.push_back(geom);
    }

  /* Loop over all jets to collect tau candidate objects */
  for (JetMap::Iter iter = recojets->begin();
       iter != recojets->end();
       ++iter)
    {
      /* skip jets below energy threshold */
      if ( (iter->second)->get_e() < _tau_jet_emin )
        continue;

      /* add tau candidate jet to collection */
      tauCandidateMap.insert( make_pair( (iter->second)->get_e(), TauCandidate( iter->second ) ) );
    }

  /* Add tag for true Tau particle jet to tau candidates */
  AddTrueTauTag( tauCandidateMap, genevtmap );

  /* Add jet structure information to tau candidates */
  AddJetStructureInformation( tauCandidateMap, recojets, v_towers, v_tower_geoms );

  /* Add track information to tau candidates */
  AddTrackInformation( tauCandidateMap, trackmap );

  /* Add information about tau candidats to output tree */
  WriteTauCandidatesToTree( tauCandidateMap );

  /* count up event number */
  _ievent ++;

  return 0;
}

int
LeptoquarksReco::AddTrueTauTag( map_tcan& tauCandidateMap, PHHepMCGenEventMap *genevtmap )
{
  /* Look for leptoquark and tau particle in the event */
  //  bool   lq_found = false;
  HepMC::GenParticle* particle_lq = NULL;
  HepMC::GenParticle* particle_tau = NULL;
  HepMC::GenParticle* particle_quark = NULL;

  /* --> Loop over all truth events in event generator collection */
  for (PHHepMCGenEventMap::ReverseIter iter = genevtmap->rbegin(); iter != genevtmap->rend(); ++iter)
    {
      PHHepMCGenEvent *genevt = iter->second;
      HepMC::GenEvent *theEvent = genevt->getEvent();

      /* check if GenEvent object found */
      if ( !theEvent )
        {
          cout << "ERROR: Missing GenEvent!" << endl;
          return -1;
        }

      /* Loop over all truth particles in event generator collection
       *
       * particle   ...   PGD code
       * -------------------------
       * LQ_ue      ...    39
       * tau-       ...    15
       * pi+        ...    211
       * pi-        ...   -211
       * u          ...    2
       * d          ...    1 */
      for ( HepMC::GenEvent::particle_iterator p = theEvent->particles_begin();
            p != theEvent->particles_end(); ++p ) {

        /* check particle ID == LQ_ue */
        if ( (*p)->pdg_id() == 39 )
          {
            particle_lq = (*p);

            /* Where does it end (=decay)? */
            if ( particle_lq->end_vertex() )
              {
                /* Loop over mother particles at production vertex */
                for ( HepMC::GenVertex::particle_iterator lq_child
                        = particle_lq->end_vertex()->
                        particles_begin(HepMC::children);
                      lq_child != particle_lq->end_vertex()->
                        particles_end(HepMC::children);
                      ++lq_child )
                  {
                    /* Is child a tau? */
                    if ( (*lq_child)->pdg_id() == 15 )
                      {
                        particle_tau = (*lq_child);
                        UpdateFinalStateParticle( particle_tau );
                      }
                    /* Is child a quark? */
                    else if ( abs((*lq_child)->pdg_id()) > 0 && abs((*lq_child)->pdg_id() < 7) )
                      {
                        particle_quark = (*lq_child);
                        UpdateFinalStateParticle( particle_quark );
                      }
                    /* What else could it be? */
                    else
                      {
                        cerr << PHWHERE << " ERROR: Didn't expect to find a leptoquark child with pdg_id " << (*lq_child)->pdg_id() << endl;
                        return Fun4AllReturnCodes::ABORTEVENT;
                      }
                  }
              }

            /* end loop if leptoquark found */
            break;
          } // end if leptoquark //
      } // end loop over all particles in event //
    }// end loop over genevtmap //

  /* If TAU in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this tau */
  if( particle_tau )
    {
      double tau_eta = particle_tau->momentum().eta();
      double tau_phi = particle_tau->momentum().phi();

      /* Event record uses 0 < phi < 2Pi, while Fun4All uses -Pi < phi < Pi.
       * Therefore, correct angle for 'wraparound' at phi == Pi */
      if(tau_phi>TMath::Pi()) tau_phi = tau_phi-2*TMath::Pi();

      /* find jet with smallest delta_R from tau */
      float min_delta_R = 100;
      map_tcan::iterator min_delta_R_iter = tauCandidateMap.end();

      for (map_tcan::iterator iter = tauCandidateMap.begin();
           iter != tauCandidateMap.end();
           ++iter)
        {
          float eta = (iter->second).get_jet_eta();
          float phi = (iter->second).get_jet_phi();

          float delta_R = CalculateDeltaR( eta, phi, tau_eta, tau_phi );

          if ( delta_R < min_delta_R )
            {
              min_delta_R_iter = iter;
              min_delta_R = delta_R;
            }
        }

      /* set is_tau = TRUE for TauCandiate with smallest delta_R */
      if ( min_delta_R_iter != tauCandidateMap.end() )
        {
          (min_delta_R_iter->second).set_is_tau( true );
          (min_delta_R_iter->second).set_tau_etotal( particle_tau->momentum().e() );
          (min_delta_R_iter->second).set_tau_eta( tau_eta );
          (min_delta_R_iter->second).set_tau_phi( tau_phi );

	  /* Add information about tau decay */
	  int tau_decay_prong = 0;
	  int tau_decay_hcharged = 0;
	  int tau_decay_lcharged = 0;

	  /* Loop over all particle at end vertex */
	  if ( particle_tau->end_vertex() )
	    {
	      for ( HepMC::GenVertex::particle_iterator tau_decay
		      = particle_tau->end_vertex()->
		      particles_begin(HepMC::children);
		    tau_decay != particle_tau->end_vertex()->
		      particles_end(HepMC::children);
		    ++tau_decay )
		{
		  /* Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle */
		  TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*tau_decay)->pdg_id() );

		  /* Check if particle is charged */
		  if ( pdg_p->Charge() != 0 )
		    {
		      tau_decay_prong += 1;

		      /* Check if particle is lepton */
		      if ( string( pdg_p->ParticleClass() ) == "Lepton" )
			tau_decay_lcharged += 1;

		      /* Check if particle is hadron, i.e. Meson or Baryon */
		      else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
				( string( pdg_p->ParticleClass() ) == "Baryon" ) )
			tau_decay_hcharged += 1;
		    }
		}

	      /* Update TauCandidate entry */
	      (min_delta_R_iter->second).set_tau_decay_prong( tau_decay_prong );
	      (min_delta_R_iter->second).set_tau_decay_hcharged( tau_decay_hcharged );
	      (min_delta_R_iter->second).set_tau_decay_lcharged( tau_decay_lcharged );
	    }
        }
    }


  /* If QUARK (->jet) in event: Tag the tau candidate (i.e. jet) with smalles delta_R from this quark */
  /* @TODO: This is a copy from the loop to tag the tau candidate with smallest delta_R w.r.t. final state tau-
   * instead of copying the code, make a function that can be called for both (GetMinDeltaRElement or similar) */
  if( particle_quark )
    {
      double quark_eta = particle_quark->momentum().eta();
      double quark_phi = particle_quark->momentum().phi();

      /* Event record uses 0 < phi < 2Pi, while Fun4All uses -Pi < phi < Pi.
       * Therefore, correct angle for 'wraparound' at phi == Pi */
      if(quark_phi>TMath::Pi()) quark_phi = quark_phi-2*TMath::Pi();

      /* find jet with smallest delta_R from quark */
      float min_delta_R = 100;
      map_tcan::iterator min_delta_R_iter = tauCandidateMap.end();

      for (map_tcan::iterator iter = tauCandidateMap.begin();
           iter != tauCandidateMap.end();
           ++iter)
        {
          float eta = (iter->second).get_jet_eta();
          float phi = (iter->second).get_jet_phi();

          float delta_R = CalculateDeltaR( eta, phi, quark_eta, quark_phi );

          if ( delta_R < min_delta_R )
            {
              min_delta_R_iter = iter;
              min_delta_R = delta_R;
            }
        }

      /* set is_uds = TRUE for TauCandiate with smallest delta_R */
      if ( min_delta_R_iter != tauCandidateMap.end() )
        {
          (min_delta_R_iter->second).set_is_uds( true );
          (min_delta_R_iter->second).set_uds_etotal( particle_tau->momentum().e() );
          (min_delta_R_iter->second).set_uds_eta( quark_eta );
          (min_delta_R_iter->second).set_uds_phi( quark_phi );
        }
    }

  return 0;
}


int
LeptoquarksReco::AddJetStructureInformation( map_tcan& tauCandidateMap, JetMap* recojets, vector<RawTowerContainer*> v_towers, vector<RawTowerGeomContainer*> v_tower_geoms )
{
  /* Cone size around jet axis within which to look for tracks */
  float delta_R_cutoff_r1 = 0.2;
  float delta_R_cutoff_r2 = 0.5;
  float delta_R_cutoff_r3 = 1.0;

  /* energy threshold for considering tower */
  float tower_emin = 0.1;

  /* number of steps for finding r90 */
  int n_steps = 50;

  /* Loop over tau candidates */
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      /* get jet axis */
      float jet_eta = (iter->second).get_jet_eta();
      float jet_phi = (iter->second).get_jet_phi();
      float jet_e = (iter->second).get_jet_etotal();

      /* collect jet structure properties */
      float er1 = 0;
      float er2 = 0;
      float er3 = 0;
      float r90 = 0;
      float radius = 0;
      float rms = 0;
      float rms_esum = 0;

      /* Loop over all tower (and geometry) collections */
      for ( unsigned i = 0; i < v_towers.size(); i++ )
        {
          /* define tower iterator */
          RawTowerContainer::ConstRange begin_end = (v_towers.at(i))->getTowers();
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
              RawTowerGeom * tower_geom = (v_tower_geoms.at(i))->get_tower_geometry(tower -> get_key());
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

              if ( delta_R <= delta_R_cutoff_r1 )
                {
                  er1 += tower_energy;
                }
              else if ( delta_R <= delta_R_cutoff_r2 )
                {
                  er2 += tower_energy;
                }
              else if ( delta_R <= delta_R_cutoff_r3 )
                {
                  er3 += tower_energy;
                }

              if ( delta_R <= delta_R_cutoff_r2 )
                {
                  rms += tower_energy*delta_R*delta_R;
                  rms_esum += tower_energy;

                  radius += tower_energy*delta_R;
                }
            }
        }

      /* finalize calculation of rms and radius */
      radius /= rms_esum;

      rms /= rms_esum;
      rms = sqrt( rms );

      /* normalize energies in rings */
      //float ersum = er1 + er2 + er3;
      //er1 /= ersum;
      //er2 /= ersum;
      //er3 /= ersum;

      /* Search for cone angle that contains 90% of jet energy */
      for(int r_i = 1; r_i<n_steps+1; r_i++){
        float e_tower_sum = 0;

        /* Loop over all tower (and geometry) collections */
        for ( unsigned i = 0; i < v_towers.size(); i++ )
          {
            /* define tower iterator */
            RawTowerContainer::ConstRange begin_end = (v_towers.at(i))->getTowers();
            RawTowerContainer::ConstIterator rtiter;

            if (e_tower_sum >= 0.9*jet_e) break;

            for (rtiter = begin_end.first; rtiter !=  begin_end.second; ++rtiter)
              {
                RawTower *tower = rtiter->second;
                float tower_energy = tower->get_energy();

		/* check if tower is above minimum tower energy required */
		if ( tower_energy < tower_emin )
		  continue;

		RawTowerGeom * tower_geom = (v_tower_geoms.at(i))->get_tower_geometry(tower -> get_key());
                float tower_eta = tower_geom->get_eta();
                float tower_phi = tower_geom->get_phi();

		float delta_R = CalculateDeltaR( tower_eta , tower_phi, jet_eta, jet_phi );

                if(delta_R < r_i*delta_R_cutoff_r2/n_steps) {
                  e_tower_sum = e_tower_sum + tower_energy;
                  r90 = r_i*delta_R_cutoff_r2/n_steps;
                }
              }

            if (e_tower_sum >= 0.9*jet_e) break;
          }
      }

      /* set tau candidate properties */
      (iter->second).set_jetshape_econe( delta_R_cutoff_r1, er1 );
      (iter->second).set_jetshape_econe( delta_R_cutoff_r2, er2 );
      (iter->second).set_jetshape_econe( delta_R_cutoff_r3, er3 );
      (iter->second).set_jetshape_r90( r90 );
      (iter->second).set_jetshape_rms( rms );
      (iter->second).set_jetshape_radius( radius );
    }

  return 0;
}

int
LeptoquarksReco::AddTrackInformation( map_tcan& tauCandidateMap, SvtxTrackMap* trackmap )
{
  /* Cone size around jet axis within which to look for tracks */
  float delta_R_cutoff_1 = 0.2;
  float delta_R_cutoff_2 = 0.5;

  /* Loop over tau candidates */
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      int tracks_count = 0;
      int tracks_chargesum = 0;
      float tracks_rmax = 0;

      float jet_eta = (iter->second).get_jet_eta();
      float jet_phi = (iter->second).get_jet_phi();

      /* Loop over tracks
       * (float) track->get_eta(),     //eta of the track
       * (float) track->get_phi(),     //phi of the track
       * (float) track->get_pt(),      //transverse momentum of track
       * (float) track->get_p(),       //total momentum of track
       * (float) track->get_charge(),  //electric charge of track
       * (float) track->get_quality()  //track quality */
      for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
           track_itr != trackmap->end(); track_itr++) {

        SvtxTrack* track = dynamic_cast<SvtxTrack*>(track_itr->second);

        float track_eta = track->get_eta();
        float track_phi = track->get_phi();
        int track_charge = track->get_charge();

        float delta_R = CalculateDeltaR( track_eta, track_phi, jet_eta, jet_phi );

        /* If track within search cone, update track information for tau candidate */

        if ( delta_R < delta_R_cutoff_1 )
          {
            tracks_count++;
            tracks_chargesum += track_charge;
	  }

	if ( ( delta_R < delta_R_cutoff_2 ) && ( delta_R > tracks_rmax ) )
	  {
	    tracks_rmax = delta_R;
	  }

      } // end loop over reco tracks //

        /* Set track-based properties for tau candidate */
      (iter->second).set_tracks_count( tracks_count );
      (iter->second).set_tracks_chargesum( tracks_chargesum );
      (iter->second).set_tracks_rmax( tracks_rmax );

    } // end loop over tau  candidates

  return 0;
}


int
LeptoquarksReco::WriteTauCandidatesToTree( map_tcan& tauCandidateMap )
{

  /* Loop over all tau candidates */
  for (map< float, TauCandidate >::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      float jet_data[18] = {(float) _ievent,  //event number
                            (float) (iter->second).get_jet_id(),       //jet id
                            (float) 0, //is_max_energy_jet,                //is this the maximum energy jet?
                            (float) (iter->second).get_is_tau(),              //is this the minimum R jet?
                            (float) (iter->second).get_jet_eta(),                  //
                            (float) (iter->second).get_jet_phi(),
                            (float) 0, //delta_R,
                            (float) (iter->second).get_jet_mass(),
                            (float) (iter->second).get_jet_ptotal(),
                            (float) (iter->second).get_jet_ptrans(),
                            (float) (iter->second).get_jet_etrans(),
                            (float) (iter->second).get_jet_etotal(),
                            (float) 0, //jet_px,
                            (float) 0, //jet_py,
                            (float) 0  //jet_pz
      };

      _ntp_jet->Fill(jet_data);


      float jet2_data[29] = {(float) _ievent,
                             (float) (iter->second).get_jet_id(),
                             (float) (iter->second).get_is_tau(),
                             (float) (iter->second).get_is_uds(),
                             (float) (iter->second).get_tau_etotal(),
                             (float) (iter->second).get_tau_eta(),
                             (float) (iter->second).get_tau_phi(),
                             (float) (iter->second).get_tau_decay_prong(),
                             (float) (iter->second).get_tau_decay_hcharged(),
                             (float) (iter->second).get_tau_decay_lcharged(),
                             (float) (iter->second).get_uds_etotal(),
                             (float) (iter->second).get_uds_eta(),
                             (float) (iter->second).get_uds_phi(),
                             (float) (iter->second).get_jet_eta(),
                             (float) (iter->second).get_jet_phi(),
                             (float) (iter->second).get_jet_etotal(),
                             (float) (iter->second).get_jet_etrans(),
                             (float) (iter->second).get_jet_ptotal(),
                             (float) (iter->second).get_jet_ptrans(),
                             (float) (iter->second).get_jet_mass(),
                             (float) (iter->second).get_jetshape_econe( 0.2 ),
                             (float) (iter->second).get_jetshape_econe( 0.5 ),
                             (float) (iter->second).get_jetshape_econe( 1.0 ),
                             (float) (iter->second).get_jetshape_r90(),
                             (float) (iter->second).get_jetshape_rms(),
			     (float) (iter->second).get_jetshape_radius(),
                             (float) (iter->second).get_tracks_count(),
                             (float) (iter->second).get_tracks_chargesum(),
                             (float) (iter->second).get_tracks_rmax()
      };

      _ntp_jet2->Fill(jet2_data);
    }

  return 0;
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


void
LeptoquarksReco::UpdateFinalStateParticle( HepMC::GenParticle *&particle )
{
  bool final_state = false;
  while ( !final_state )
    {
      /* Loop over all children at the end vertex of this particle */
      for ( HepMC::GenVertex::particle_iterator child
              = particle->end_vertex()->particles_begin(HepMC::children);
            child != particle->end_vertex()->particles_end(HepMC::children);
            ++child )
        {
          /* If there is a child of same particle ID, this was not the final state particle- update pointer to particle and repeat */
          if ( (*child)->pdg_id() == particle->pdg_id() )
            {
              particle = (*child);
              break;
            }
          final_state = true;
        }
    }
  return;
}


int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();
  _ntp_jet->Write();
  _ntp_jet2->Write();
  _tfile->Close();

  return 0;
}
