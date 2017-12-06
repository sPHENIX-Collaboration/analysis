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

using namespace std;

LeptoquarksReco::LeptoquarksReco(std::string filename) :
  SubsysReco("LeptoquarksReco" ),
  _ievent(0),
  _filename(filename),
  _tfile(nullptr),
  _ntp_jet(nullptr),
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

  return 0;
}

int
LeptoquarksReco::process_event(PHCompositeNode *topNode)
{
  /* count up event number */
  _ievent ++;

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
  AddJetStructureInformation( tauCandidateMap, recojets );

  /* Add track information to tau candidates */
  AddTrackInformation( tauCandidateMap, trackmap );

  /* Add information about tau candidats to output tree */
  WriteTauCandidatesToTree( tauCandidateMap );

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
                    else if ( (*lq_child)->pdg_id() > 0 && (*lq_child)->pdg_id() < 7 )
                      {
			particle_quark = (*lq_child);
			UpdateFinalStateParticle( particle_quark );
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
          float temp_phi_tau = tau_phi;

          /* Particles at phi = PI+x and phi = PI-x are actually close to each other in phi, but simply calculating
           * the difference in phi would give a large distance (because phi ranges from -PI to +PI in the convention
           * used. Account for this by subtracting 2PI is particles fall within this border area. */
          if((tau_phi < -0.9*TMath::Pi()) && (phi > 0.9*TMath::Pi())) phi = phi-2*TMath::Pi();
          if((tau_phi > 0.9*TMath::Pi()) && (phi < -0.9*TMath::Pi())) temp_phi_tau = tau_phi-2*TMath::Pi();

          float delta_R = sqrt(pow(eta-tau_eta,2)+pow(phi-temp_phi_tau,2));

          if ( delta_R < min_delta_R )
            {
              min_delta_R_iter = iter;
              min_delta_R = delta_R;
            }
        }
      /* set is_tau = TRUE for TauCandiate with smallest delta_R */
      if ( min_delta_R_iter != tauCandidateMap.end() )
        (min_delta_R_iter->second).set_is_tau( true );
    }

  return 0;
}


int
LeptoquarksReco::AddJetStructureInformation( map_tcan& tauCandidateMap, JetMap* recojets )
{
  /* Cone size around jet axis within which to look for tracks */
  //float delta_R_cutoff = 0.5;

  /* Loop over tau candidates */
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      /* @TODO: Implement adding jet structure properties to Tau candidates */
    }

  return 0;
}

int
LeptoquarksReco::AddTrackInformation( map_tcan& tauCandidateMap, SvtxTrackMap* trackmap )
{
  /* Cone size around jet axis within which to look for tracks */
  float delta_R_cutoff = 0.5;

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
        double temp_jet_phi = jet_phi;

        /* Particles at phi = PI+x and phi = PI-x are actually close to each other in phi, but simply calculating
         * the difference in phi would give a large distance (because phi ranges from -PI to +PI in the convention
         * used. Account for this by subtracting 2PI is particles fall within this border area. */
        if((jet_phi < -0.9*TMath::Pi()) && (track_phi > 0.9*TMath::Pi())) track_phi = track_phi-2*TMath::Pi();
        if((jet_phi > 0.9*TMath::Pi()) && (track_phi < -0.9*TMath::Pi())) temp_jet_phi = jet_phi-2*TMath::Pi();

        float delta_R = sqrt(pow(track_eta-jet_eta,2)+pow(track_phi-temp_jet_phi,2));

        /* If track within search cone, update track information for tau candidate */
        if ( delta_R < delta_R_cutoff )
          {
            tracks_count++;
            tracks_chargesum += track_charge;

            if ( delta_R > tracks_rmax )
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

    }

  return 0;
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
  _tfile->Close();

  return 0;
}
