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

#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>

#include <g4main/PHG4Particle.h>

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
  _ebeam_E(0),
  _pbeam_E(0)
{

}

int
DISKinematicsReco::Init(PHCompositeNode *topNode)
{
  _ievent = 0;

  _tfile = new TFile(_filename.c_str(), "RECREATE");

  /* Add TauCandidate properties to map that defines output tree */
  float dummy = 0;
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_is_ele , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_ele_etotal , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_ele_eta , dummy ) );
  _map_treebranches.insert( make_pair( TauCandidate::evtgen_ele_phi , dummy ) );
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

  /* Loop over all tracks to collect electron candidate objects */
  /* Loop over tracks
   * (float) track->get_eta(),     //eta of the track
   * (float) track->get_phi(),     //phi of the track
   * (float) track->get_pt(),      //transverse momentum of track
   * (float) track->get_p(),       //total momentum of track
   * (float) track->get_charge(),  //electric charge of track
   * (float) track->get_quality()  //track quality */
  /* barrel tracking */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
       track_itr != trackmap->end(); track_itr++)
    {
      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
    } // end loop over reco tracks //

  /* h-going tracking */
  for (SvtxTrackMap::ConstIter track_itr = trackmap_eta_plus->begin();
       track_itr != trackmap_eta_plus->end(); track_itr++)
    {
      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
    } // end loop over reco tracks //

  /* e-going tracking */
  for (SvtxTrackMap::ConstIter track_itr = trackmap_eta_minus->begin();
       track_itr != trackmap_eta_minus->end(); track_itr++)
    {
      InsertCandidateFromTrack( electronCandidateMap , dynamic_cast<SvtxTrack*>(track_itr->second) );
    } // end loop over reco tracks //

  /* Add calorimeter information to tau candidates */
  //  AddCalorimeterInformation( electronCandidateMap, &map_calotower );

  /* Add track information to tau candidates */
  //  AddTrackInformation( electronCandidateMap, trackmap );

  /* Add tag for true Tau particle jet to tau candidates */
  //  AddTrueElectronTag( electronCandidateMap, genevtmap );

  /* Add information about tau candidats to output tree */
  WriteCandidatesToTree( electronCandidateMap );

  /* count up event number */
  _ievent ++;

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
  tc->set_property( TauCandidate::track_e3x3_femc, (float)0 );
  tc->set_property( TauCandidate::track_e3x3_fhcal, (float)0 );
  tc->set_property( TauCandidate::track_e3x3_eemc, (float)0 );

  /* set tau candidate MC truth properties */
  tc->set_property( TauCandidate::evtgen_is_ele, (uint)0 );
  tc->set_property( TauCandidate::evtgen_ele_etotal, (float)0 );
  tc->set_property( TauCandidate::evtgen_ele_eta, (float)0 );
  tc->set_property( TauCandidate::evtgen_ele_phi, (float)0 );

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
DISKinematicsReco::AddGlobalEventInformation( type_map_tcan& electronCandidateMap )
{
  /* fill event information tree */
  _tree_event->Fill();

  return 0;
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
