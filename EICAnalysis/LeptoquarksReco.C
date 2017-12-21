#include "LeptoquarksReco.h"
#include "TauCandidatev1.h"
#include "TruthTrackerHepMC.h"

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
  _t_candidate(nullptr),
  _ntp_jet(nullptr),
  _ntp_jet2(nullptr),
  _ntp_tower(nullptr),
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

  /* Add TauCandidate properties to map that defines output tree */
  float dummy = 0;
  _map_treebranches.insert( make_pair( TauCandidate::jet_id , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_is_tau , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_is_uds , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_etotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_decay_prong , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_decay_hcharged , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_tau_decay_lcharged , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_uds_etotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_uds_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_uds_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_phi , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_etotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_etrans , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_ptotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_ptrans , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jet_mass , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_econe_r01 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_econe_r02 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_econe_r03 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_econe_r04 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_econe_r05 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_r90 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_rms , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::jetshape_radius , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_count_r02 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_chargesum_r02 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_rmax_r02 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_count_r04 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_chargesum_r04 , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::tracks_rmax_r04 , dummy ) );

  /* Create tree for information about tau candidates */
  _t_candidate = new TTree("candidates", "a Tree with tau candidates");
  _t_candidate->Branch("event", &_ievent, "event/I");
  for ( map< TauCandidate::PROPERTY , float >::iterator iter = _map_treebranches.begin();
        iter != _map_treebranches.end();
        ++iter)
    {
      //cout << "ADDING BRANCH " << TauCandidate::get_property_info( (iter->first) ).first << "\tOF TYPE\t" << TauCandidate::get_property_type( TauCandidate::get_property_info( (iter->first) ).second ) << endl;

      _t_candidate->Branch(TauCandidate::get_property_info( (iter->first) ).first.c_str(),
                           &(iter->second),
                           TauCandidate::get_property_info( (iter->first) ).first.c_str());
    }

  /* NTuple to store tau candidate information */
  _ntp_jet = new TNtuple("ntp_jet","all jet information from LQ events",
                         "event:jet_id:isMaxEnergyJet:isMinDeltaRJet:jet_eta:jet_phi:delta_R:jet_mass:jet_p:jet_pT:jet_eT:jet_e:jet_px:jet_py:jet_pz");

  _ntp_jet2 = new TNtuple("ntp_jet2","all tau candidate (jet) information from LQ events",
                          "ievent:jet_id:is_tau:is_uds:tau_etotal:tau_eta:tau_phi:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_etotal:uds_eta:uds_phi:jet_eta:jet_phi:jet_etotal:jet_etrans:jet_ptotal:jet_ptrans:jet_mass:jetshape_econe_r1:jetshape_econe_r2:jetshape_econe_r3:jetshape_econe_r4:jetshape_econe_r5:jetshape_econe_r6:jetshape_econe_r7:jetshape_econe_r8:jetshape_econe_r9:jetshape_econe_r10:jetshape_r90:jetshape_rms:jetshape_radius:tracks_count_r1:tracks_chargesum_r1:tracks_rmax_r1:tracks_count_r2:tracks_chargesum_r2:tracks_rmax_r2");

  if ( _save_towers )
    {
      _ntp_tower = new TNtuple("ntp_tower","towers from all all tau candidates",
                               "ievent:jet_id:is_tau:is_uds:tau_etotal:tau_eta:tau_phi:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_etotal:uds_eta:uds_phi:jet_eta:jet_phi:jet_etotal:tower_calo_id:tower_eta:tower_phi:tower_delta_r:tower_e");
    }

  if ( _save_tracks )
    {
      _ntp_track = new TNtuple("ntp_track","tracks from all all tau candidates",
                               "ievent:jet_id:is_tau:is_uds:tau_etotal:tau_eta:tau_phi:tau_decay_prong:tau_decay_hcharged:tau_decay_lcharged:uds_etotal:uds_eta:uds_phi:jet_eta:jet_phi:jet_etotal:track_quality:track_eta:track_phi:track_delta_r:track_p");
    }

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
      TauCandidatev1 *tc = new TauCandidatev1();

      tc->set_candidate_id( (iter->second)->get_id() );

      tc->set_property( TauCandidate::jet_id , (iter->second)->get_id() );
      tc->set_property( TauCandidate::jet_eta , (iter->second)->get_eta() );
      tc->set_property( TauCandidate::jet_phi , (iter->second)->get_phi() );
      tc->set_property( TauCandidate::jet_etotal , (iter->second)->get_e() );
      tc->set_property( TauCandidate::jet_etrans , (iter->second)->get_et() );
      tc->set_property( TauCandidate::jet_ptotal , (iter->second)->get_p() );
      tc->set_property( TauCandidate::jet_ptrans , (iter->second)->get_pt() );
      tc->set_property( TauCandidate::jet_mass , (iter->second)->get_mass() );

      tc->set_property( TauCandidate::evtgen_is_tau, (uint)0 );
      tc->set_property( TauCandidate::evtgen_is_uds, (uint)0 );

      tauCandidateMap.insert( make_pair( (iter->second)->get_e(), tc ) );
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
  TruthTrackerHepMC truth;
  truth.set_hepmc_geneventmap( genevtmap );

  int pdg_lq = 39; // leptoquark
  int pdg_tau = 15; // tau lepton

  HepMC::GenParticle* particle_lq = truth.FindParticle( pdg_lq );
  HepMC::GenParticle* particle_tau = truth.FindDaughterParticle( pdg_tau, particle_lq );

  /* loop over all quark PDG codes until finding a matching quark */
  HepMC::GenParticle* particle_quark = NULL;
  for ( int pdg_quark = 1; pdg_quark < 7; pdg_quark++ )
    {
      /* try quark */
      particle_quark = truth.FindDaughterParticle( pdg_quark, particle_lq );
      if (particle_quark)
	break;

      /* try anti-quark */
      particle_quark = truth.FindDaughterParticle( -pdg_quark, particle_lq );
      if (particle_quark)
	break;
    }

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
          float eta = (iter->second)->get_property_float( TauCandidate::jet_eta );
          float phi = (iter->second)->get_property_float( TauCandidate::jet_phi );

          float delta_R = CalculateDeltaR( eta, phi, tau_eta, tau_phi );

          if ( delta_R < min_delta_R )
            {
              min_delta_R_iter = iter;
              min_delta_R = delta_R;
            }
        }

      /* set is_tau = TRUE for TauCandiate with smallest delta_R within reasonable range*/
      if ( min_delta_R_iter != tauCandidateMap.end() && min_delta_R<0.5)
        {
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_is_tau, (uint)1 );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_etotal, (float)particle_tau->momentum().e() );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_eta, (float)tau_eta );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_phi, (float)tau_phi );

          /* Add information about tau decay */
          uint tau_decay_prong = 0;
          uint tau_decay_hcharged = 0;
          uint tau_decay_lcharged = 0;

          /* Loop over all particle at end vertex */
          if ( particle_tau->end_vertex() )
            {
              /* Function that calls itself instead of doing the same loop. Still need to debug */
              //  FinDecayParticles(particle_tau, tau_decay_prong, tau_decay_hcharged, tau_decay_lcharged);

              // Loop over particles at vertex of tau
              for ( HepMC::GenVertex::particle_iterator tau_decay
                      = particle_tau->end_vertex()->
                      particles_begin(HepMC::children);
                    tau_decay != particle_tau->end_vertex()->
                      particles_end(HepMC::children);
                    ++tau_decay )
                {
                  // check if particle decays further
                  if(!(*tau_decay)->end_vertex()){

                    // Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle
                    TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*tau_decay)->pdg_id() );

                    // Check if particle is charged
                    if ( pdg_p->Charge() != 0 )
                      {
                        tau_decay_prong += 1;

                        // Check if particle is lepton
                        if ( string( pdg_p->ParticleClass() ) == "Lepton" )
                          tau_decay_lcharged += 1;

                        // Check if particle is hadron, i.e. Meson or Baryon
                        else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
                                  ( string( pdg_p->ParticleClass() ) == "Baryon" ) )
                          tau_decay_hcharged += 1;
                      }
                  }

                  // loop over decay if particle decays further
                  else if((*tau_decay)->end_vertex()){

                    //further decay loop
                    for ( HepMC::GenVertex::particle_iterator second_tau_decay
                            = (*tau_decay)->end_vertex()->
                            particles_begin(HepMC::children);
                          second_tau_decay != (*tau_decay)->end_vertex()->
                            particles_end(HepMC::children);
                          ++second_tau_decay )
                      {

                        // Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle
                        TParticlePDG * pdg_p2 = TDatabasePDG::Instance()->GetParticle( (*second_tau_decay)->pdg_id() );

                        // Check if particle is charged
                        if ( pdg_p2->Charge() != 0 )
                          {
                            tau_decay_prong += 1;

                            // Check if particle is lepton
                            if ( string( pdg_p2->ParticleClass() ) == "Lepton" )
                              tau_decay_lcharged += 1;

                            // Check if particle is hadron, i.e. Meson or Baryon
                            else if ( ( string( pdg_p2->ParticleClass() ) == "Meson"  ) ||
                                      ( string( pdg_p2->ParticleClass() ) == "Baryon" ) )
                              tau_decay_hcharged += 1;
                          }
                      }
                  }
                }

              /* Update TauCandidate entry */
              (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_decay_prong, tau_decay_prong );
              (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_decay_hcharged, tau_decay_hcharged );
              (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_tau_decay_lcharged, tau_decay_lcharged );
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
          float eta = (iter->second)->get_property_float( TauCandidate::jet_eta );
          float phi = (iter->second)->get_property_float( TauCandidate::jet_phi );

          float delta_R = CalculateDeltaR( eta, phi, quark_eta, quark_phi );

          if ( delta_R < min_delta_R )
            {
              min_delta_R_iter = iter;
              min_delta_R = delta_R;
            }
        }

      /* set is_uds = TRUE for TauCandiate with smallest delta_R within reasonable range*/
      if ( min_delta_R_iter != tauCandidateMap.end() && min_delta_R<0.5)
        {
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_is_uds, (uint)1 );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_uds_etotal, (float)particle_quark->momentum().e() );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_uds_eta, (float)quark_eta );
          (min_delta_R_iter->second)->set_property( TauCandidate::evtgen_uds_phi, (float)quark_phi );
        }
    }

  return 0;
}


int
LeptoquarksReco::AddJetStructureInformation( map_tcan& tauCandidateMap, JetMap* recojets, vector<RawTowerContainer*> v_towers, vector<RawTowerGeomContainer*> v_tower_geoms )
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
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      /* get jet axis */
      float jet_eta = (iter->second)->get_property_float( TauCandidate::jet_eta );
      float jet_phi = (iter->second)->get_property_float( TauCandidate::jet_phi );
      float jet_e =   (iter->second)->get_property_float( TauCandidate::jet_etotal );

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

      /* Loop over all tower (and geometry) collections */
      for ( unsigned calo_id = 0; calo_id < v_towers.size(); calo_id++ )
        {
          /* define tower iterator */
          RawTowerContainer::ConstRange begin_end = (v_towers.at(calo_id))->getTowers();
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
              RawTowerGeom * tower_geom = (v_tower_geoms.at(calo_id))->get_tower_geometry(tower -> get_key());
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
                  float tower_data[39] = {(float) _ievent,
                                          (float) (iter->second)->get_property_uint( TauCandidate::jet_id ),
                                          (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_tau ),
                                          (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_uds ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_etotal ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_eta ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_phi ),
                                          (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_prong ),
                                          (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_hcharged ),
                                          (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_lcharged ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_etotal ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_eta ),
                                          (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_phi ),
                                          (float) (iter->second)->get_property_float( TauCandidate::jet_eta ),
                                          (float) (iter->second)->get_property_float( TauCandidate::jet_phi ),
                                          (float) (iter->second)->get_property_float( TauCandidate::jet_etotal ),
                                          (float) calo_id,
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
                }
              if ( delta_R <= delta_R_cutoff_r2 )
                {
                  er2 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r3 )
                {
                  er3 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r4 )
                {
                  er4 += tower_energy;
                }
              if ( delta_R <= delta_R_cutoff_r5 )
                {
                  er5 += tower_energy;
                }

              if ( delta_R <= delta_R_cutoff_r5 )
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

                if(delta_R < r_i*delta_R_cutoff_r5/n_steps) {
                  e_tower_sum = e_tower_sum + tower_energy;
                  r90 = r_i*delta_R_cutoff_r5/n_steps;
                }
              }

            if (e_tower_sum >= 0.9*jet_e) break;
          }
      }

      /* set tau candidate properties */
      (iter->second)->set_property( TauCandidate::jetshape_econe_r01, er1 );
      (iter->second)->set_property( TauCandidate::jetshape_econe_r02, er2 );
      (iter->second)->set_property( TauCandidate::jetshape_econe_r03, er3 );
      (iter->second)->set_property( TauCandidate::jetshape_econe_r04, er4 );
      (iter->second)->set_property( TauCandidate::jetshape_econe_r05, er5 );
      (iter->second)->set_property( TauCandidate::jetshape_r90, r90 );
      (iter->second)->set_property( TauCandidate::jetshape_rms, rms );
      (iter->second)->set_property( TauCandidate::jetshape_radius, radius );
    }

  return 0;
}

int
LeptoquarksReco::AddTrackInformation( map_tcan& tauCandidateMap, SvtxTrackMap* trackmap )
{
  /* Cone size around jet axis within which to look for tracks */
  float delta_R_cutoff_1 = 0.2;
  float delta_R_cutoff_2 = 0.4;

  /* Loop over tau candidates */
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
       ++iter)
    {
      uint tracks_count_r1 = 0;
      int tracks_chargesum_r1 = 0;
      float tracks_rmax_r1 = 0;

      uint tracks_count_r2 = 0;
      int tracks_chargesum_r2 = 0;
      float tracks_rmax_r2 = 0;

      float jet_eta = (iter->second)->get_property_float( TauCandidate::jet_eta );
      float jet_phi = (iter->second)->get_property_float( TauCandidate::jet_phi );

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

        /* if save_tracks set true: add track to tree */
        if ( _save_tracks )
          {
            float track_data[39] = {(float) _ievent,
                                    (float) (iter->second)->get_property_uint( TauCandidate::jet_id ),
                                    (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_tau ),
                                    (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_uds ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_etotal ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_eta ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_phi ),
                                    (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_prong ),
                                    (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_hcharged ),
                                    (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_lcharged ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_etotal ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_eta ),
                                    (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_phi ),
                                    (float) (iter->second)->get_property_float( TauCandidate::jet_eta ),
                                    (float) (iter->second)->get_property_float( TauCandidate::jet_phi ),
                                    (float) (iter->second)->get_property_float( TauCandidate::jet_etotal ),
                                    (float) track->get_quality(),
                                    (float) track_eta,
                                    (float) track_phi,
                                    (float) delta_R,
                                    (float) track->get_p()
            };

            _ntp_track->Fill(track_data);
          }

        /* If track within search cone, update track information for tau candidate */
        if ( delta_R < delta_R_cutoff_1 )
          {
            tracks_count_r1++;
            tracks_chargesum_r1 += track_charge;

            if ( delta_R > tracks_rmax_r1 )
              tracks_rmax_r1 = delta_R;
          }

        if ( delta_R < delta_R_cutoff_2 )
          {
            tracks_count_r2++;
            tracks_chargesum_r2 += track_charge;

            if ( delta_R > tracks_rmax_r2 )
              tracks_rmax_r2 = delta_R;
          }

      } // end loop over reco tracks //

      /* Set track-based properties for tau candidate */
      (iter->second)->set_property( TauCandidate::tracks_count_r02, tracks_count_r1 );
      (iter->second)->set_property( TauCandidate::tracks_chargesum_r02, tracks_chargesum_r1 );
      (iter->second)->set_property( TauCandidate::tracks_rmax_r02, tracks_rmax_r1 );
      (iter->second)->set_property( TauCandidate::tracks_count_r04, tracks_count_r2 );
      (iter->second)->set_property( TauCandidate::tracks_chargesum_r04, tracks_chargesum_r2 );
      (iter->second)->set_property( TauCandidate::tracks_rmax_r04, tracks_rmax_r2 );

    } // end loop over tau  candidates

  return 0;
}


int
LeptoquarksReco::WriteTauCandidatesToTree( map_tcan& tauCandidateMap )
{

  /* Loop over all tau candidates */
  for (map_tcan::iterator iter = tauCandidateMap.begin();
       iter != tauCandidateMap.end();
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
      _t_candidate->Fill();


      float jet_data[18] = {(float) _ievent,  //event number
                            (float) (iter->second)->get_property_uint( TauCandidate::jet_id ),       //jet id
                            (float) 0, //is_max_energy_jet,                //is this the maximum energy jet?
                            (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_tau ),              //is this the minimum R jet?
                            (float) (iter->second)->get_property_float( TauCandidate::jet_eta ),                  //
                            (float) (iter->second)->get_property_float( TauCandidate::jet_phi ),
                            (float) 0, //delta_R,
                            (float) (iter->second)->get_property_float( TauCandidate::jet_mass ),
                            (float) (iter->second)->get_property_float( TauCandidate::jet_ptotal ),
                            (float) (iter->second)->get_property_float( TauCandidate::jet_ptrans ),
                            (float) (iter->second)->get_property_float( TauCandidate::jet_etrans ),
                            (float) (iter->second)->get_property_float( TauCandidate::jet_etotal ),
                            (float) 0, //jet_px,
                            (float) 0, //jet_py,
                            (float) 0  //jet_pz
      };

      _ntp_jet->Fill(jet_data);

      float jet2_data[39] = {(float) _ievent,
                             (float) (iter->second)->get_property_uint( TauCandidate::jet_id ),
                             (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_tau ),
                             (float) (iter->second)->get_property_uint( TauCandidate::evtgen_is_uds ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_etotal ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_eta ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_tau_phi ),
                             (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_prong ),
                             (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_hcharged ),
                             (float) (iter->second)->get_property_uint( TauCandidate::evtgen_tau_decay_lcharged ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_etotal ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_eta ),
                             (float) (iter->second)->get_property_float( TauCandidate::evtgen_uds_phi ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_eta ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_phi ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_etotal ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_etrans ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_ptotal ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_ptrans ),
                             (float) (iter->second)->get_property_float( TauCandidate::jet_mass ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_econe_r01 ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_econe_r02 ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_econe_r03 ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_econe_r04 ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_econe_r05 ),
                             (float) 0, //jetshpae_econe_r6
                             (float) 0, //jetshpae_econe_r7
                             (float) 0, //jetshpae_econe_r8
                             (float) 0, //jetshpae_econe_r9
                             (float) 0, //jetshpae_econe_r10
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_r90 ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_rms ),
                             (float) (iter->second)->get_property_float( TauCandidate::jetshape_radius ),
                             (float) (iter->second)->get_property_uint( TauCandidate::tracks_count_r02 ),
                             (float) (iter->second)->get_property_int( TauCandidate::tracks_chargesum_r02 ),
                             (float) (iter->second)->get_property_float( TauCandidate::tracks_rmax_r02 ),
                             (float) (iter->second)->get_property_uint( TauCandidate::tracks_count_r04 ),
                             (float) (iter->second)->get_property_int( TauCandidate::tracks_chargesum_r04 ),
                             (float) (iter->second)->get_property_float( TauCandidate::tracks_rmax_r04 )
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


int
LeptoquarksReco::End(PHCompositeNode *topNode)
{
  _tfile->cd();

  if ( _t_candidate )
    _t_candidate->Write();

  _ntp_jet->Write();
  _ntp_jet2->Write();

  if ( _ntp_tower )
    _ntp_tower->Write();

  if ( _ntp_track )
    _ntp_track->Write();

  _tfile->Close();

  return 0;
}

/*
  void
  LeptoquarksReco::FindDecayParticles( HepMC::GenParticle *particle_tau, int &tau_decay_prong, int &tau_decay_hcharged, int &tau_decay_lcharged){

  for ( HepMC::GenVertex::particle_iterator tau_decay
  = particle_tau->end_vertex()->
  particles_begin(HepMC::children);
  tau_decay != particle_tau->end_vertex()->
  particles_end(HepMC::children);
  ++tau_decay )
  {

  if(!(*tau_decay)->end_vertex()){

  // Get entry from TParticlePDG because HepMC::GenPArticle does not provide charge or class of particle
  TParticlePDG * pdg_p = TDatabasePDG::Instance()->GetParticle( (*tau_decay)->pdg_id() );

  // Check if particle is charged
  if ( pdg_p->Charge() != 0 )
  {
  tau_decay_prong += 1;

  // Check if particle is lepton
  if ( string( pdg_p->ParticleClass() ) == "Lepton" )
  tau_decay_lcharged += 1;

  // Check if particle is hadron, i.e. Meson or Baryon
  else if ( ( string( pdg_p->ParticleClass() ) == "Meson"  ) ||
  ( string( pdg_p->ParticleClass() ) == "Baryon" ) )
  tau_decay_hcharged += 1;
  }
  }

  //HepMC::GenVertex::particle_iterator secondary_decay = (*tau_decay)->end_vertex;
  else if((*tau_decay)->end_vertex()) FindDecayProducts((*tau_decay), tau_decay_prong, tau_decay_hcharged, tau_decay_lcharged);
  }

  }
*/
