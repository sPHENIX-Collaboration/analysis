#include "RICHParticleID.h"
#include "dualrich_analyzer.h"

// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4VtxPoint.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4hough/SvtxTrackMap_v1.h>
#include <g4hough/SvtxTrack_FastSim.h>
#include <g4hough/SvtxTrackState.h>

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>
#include <TDatabasePDG.h>

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

RICHParticleID::RICHParticleID(std::string tracksname, std::string richname, std::string filename) :
  SubsysReco("RICHParticleID" ),
  _ievent(0),
  _trackmap_name(tracksname),
  _richhits_name(richname),
  _trackstate_name("RICH"),
  _refractive_index(1),
  _foutname(filename),
  _fout_root(nullptr),
  _analyzer(nullptr),
  _pdg(nullptr)
{

}


int
RICHParticleID::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  /* create output file */
  _fout_root = new TFile(_foutname.c_str(), "RECREATE");

  /* create output tree */
  reset_tree_vars();
  init_tree();

  /* create analyzer object */
  _analyzer = new eic_dual_rich();

  /* access to PDG databse information */
  _pdg = new TDatabasePDG();

  /* Throw warning if refractive index is not greater than 1 */
  if ( _refractive_index <= 1 )
    cout << PHWHERE << " WARNING: Refractive index for radiator volume is " << _refractive_index << endl;

  return 0;
}


int
RICHParticleID::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  /* Get all photon hits in RICH for this event */
  PHG4HitContainer* richhits =
    findNode::getClass<PHG4HitContainer>(topNode,_richhits_name);

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap =
    findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name);
  /* Check if trackmap found */
  if (!trackmap) {
    cout << PHWHERE << "SvtxTrackMap node not found on node tree"
         << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /* get truth info node */
  PHG4TruthInfoContainer* truthinfo =
    findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* abort if no truth container found */
  assert(truthinfo);

  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
       track_itr != trackmap->end(); track_itr++) {

    SvtxTrack_FastSim* track_j = dynamic_cast<SvtxTrack_FastSim*>(track_itr->second);

    /* Get mean emission point from track in RICH */
    double m_emi[3] = {0.,0.,0.};

    /* 'Continue' with next track if RICH not found in state list for this track */
    if ( ! get_position_from_track_state( track_j, _trackstate_name, m_emi ) )
      continue;

    /* Fill momv object which is the normalized momentum vector of the track in the RICH (i.e. its direction) */
    double momv[3] = {0.,0.,0.};

    /* 'Continue' with next track if RICH not found in state list for this track */
    if ( ! get_momentum_from_track_state( track_j, _trackstate_name, momv ) )
      continue;

    double momv_norm = sqrt( momv[0]*momv[0] + momv[1]*momv[1] + momv[2]*momv[2] );
    momv[0] /= momv_norm;
    momv[1] /= momv_norm;
    momv[2] /= momv_norm;

    /* Calculate true emission angle for output tree */
    _theta_true = 0;
    _mass_true = 0;

    /* If truth info found use it to calculate truth emission angle and truth mass */
    if ( truthinfo )
      {
        _theta_true = calculate_true_emission_angle( truthinfo , track_j , _refractive_index );
	_mass_true = calculate_true_mass( truthinfo, track_j );
      }


    /* Loop over all G4Hits in container (i.e. RICH photons in event) */
    PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
    PHG4HitContainer::ConstIterator rich_hits_iter;

    for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
      {
        PHG4Hit *hit_i = rich_hits_iter->second;
	PHG4Particle* particle = NULL;
	PHG4Particle* parent = NULL;
	if ( truthinfo )
          {  
            particle = truthinfo->GetParticle( hit_i->get_trkid() );
            parent = truthinfo->GetParticle( particle->get_parent_id() );
	  }
	
	_hit_x0 =  hit_i->get_x(0);
        _hit_y0 =  hit_i->get_y(0);
        _hit_z0 =  hit_i->get_z(0);

	_track_px = particle->get_px();
        _track_py = particle->get_py();
        _track_pz = particle->get_pz();

        _mtrack_px = parent->get_px();
        _mtrack_py = parent->get_py();
        _mtrack_pz = parent->get_pz();

        _track_e = particle->get_e();
        _mtrack_e = parent->get_e();
        _edep = hit_i->get_edep();

	_bankid = 0;
        _volumeid = hit_i->get_detid();
        _hitid = hit_i->get_hit_id();
	_pid = particle->get_pid();
        _mpid = parent->get_pid();
        _trackid = particle->get_track_id();
        _mtrackid = parent->get_track_id();
	_otrackid = _mtrackid;

	_ptot = momv_norm;

        /* Set reconstructed emission angle and reconstructed mass for output tree */
        _theta_reco = calculate_emission_angle( m_emi, momv, hit_i );
	_mass_reco = calculate_reco_mass( momv_norm, _theta_reco, _refractive_index );

        /* Fill tree */
        _tree_rich->Fill();

      } // END loop over photons

  } // END loop over tracks

  return 0;
}


double RICHParticleID::calculate_emission_angle( double m_emi[3], double momv[3], PHG4Hit *hit_i )
{
  /* Input parameters for indirect ray tracing algorithm */
  double Ex = m_emi[0];
  double Ey = m_emi[1];
  double Ez = m_emi[2];

  double Dx = hit_i->get_x(0);
  double Dy = hit_i->get_y(0);
  double Dz = hit_i->get_z(0);

  double vx = momv[0];
  double vy = momv[1];
  double vz = momv[2];

  int sec = hit_i->get_detid();
  double cx = -18.5*TMath::Sin(sec*TMath::Pi()/4); // mirror center of each octant
  double cy = 18.5*TMath::Cos(sec*TMath::Pi()/4);
  double cz = 75;

  int select_radiator=0;

  /* Set mirror parameters */
  double R_mirror = 195; // cm
  _analyzer->set_mirror(cx, cy, cz, R_mirror);

  /* Call algorithm to determine emission angle of photon i w.r.t. track j */
  float theta_c = _analyzer->ind_ray(Ex, Ey, Ez, Dx, Dy, Dz, vx, vy, vz, select_radiator); //Indirect Ray Tracing

  return theta_c;
}


double RICHParticleID::calculate_true_emission_angle( PHG4TruthInfoContainer* truthinfo, SvtxTrack_FastSim * track, double index )
{
  /* Get truth particle associated with track */
  PHG4Particle* particle = truthinfo->GetParticle( track->get_truth_track_id() );

  /* Get particle ID */
  int pid = particle->get_pid();

  /* Get particle mass */
  double mass = _pdg->GetParticle(pid)->Mass();

  /* Get particle total momentum */
  double ptotal = sqrt( track->get_px() * track->get_px() +
                        track->get_py() * track->get_py() +
                        track->get_pz() * track->get_pz() );

  /* Calculate beta = v/c */
  double beta = ptotal / sqrt( mass * mass + ptotal * ptotal );

  /* Calculate emission angle for Cerenkov light */
  double theta_c = acos( 1 / ( index * beta ) );

  return theta_c;
}


double RICHParticleID::calculate_reco_mass( double mom, double theta_reco, double index )
{
  /* Calculate beta from reco angle */
  double beta = 1/( index * cos(theta_reco) );

  /* Calculate mass from beta */
  if (beta<1 && beta>0)
    {
      double mass = mom * sqrt( 1/beta - 1  );
      return mass;
    }
  else
    return 0;
}


double RICHParticleID::calculate_true_mass( PHG4TruthInfoContainer* truthinfo, SvtxTrack_FastSim * track )
{
  /* Get truth particle associated with track */
  PHG4Particle* particle = truthinfo->GetParticle( track->get_truth_track_id() );

  /* Get particle ID */
  int pid = particle->get_pid();

  /* Get particle mass */
  double mass = _pdg->GetParticle(pid)->Mass();

  return mass;
}


bool
RICHParticleID::get_position_from_track_state(  SvtxTrack_FastSim * track, string statename, double arr_pos[3] )
{
  arr_pos[0] = 0;
  arr_pos[1] = 0;
  arr_pos[2] = 0;

  /* Use the track states to project to the RICH */
  for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
       state_itr != track->end_states(); state_itr++) {

    SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

    if( (temp->get_name()==statename) ){
      arr_pos[0] = temp->get_x();
      arr_pos[1] = temp->get_y();
      arr_pos[2] = temp->get_z();
      return true;
    }
  }

  return false;
}


bool
RICHParticleID::get_momentum_from_track_state(  SvtxTrack_FastSim * track, string statename, double arr_mom[3] )
{
  arr_mom[0] = 0;
  arr_mom[1] = 0;
  arr_mom[2] = 0;

  /* Use the track states to project to the RICH */
  for (SvtxTrack::ConstStateIter state_itr = track->begin_states();
       state_itr != track->end_states(); state_itr++) {

    SvtxTrackState *temp = dynamic_cast<SvtxTrackState*>(state_itr->second);

    if( (temp->get_name()==statename) ){
      arr_mom[0] = temp->get_px();
      arr_mom[1] = temp->get_py();
      arr_mom[2] = temp->get_pz();
      return true;
    }
  }

  return false;
}


int
RICHParticleID::End(PHCompositeNode *topNode)
{
  _fout_root->cd();
  _tree_rich->Write();
  _fout_root->Close();

  return 0;
}


void
RICHParticleID::reset_tree_vars()
{
  _hit_x0 = -999;
  _hit_y0 = -999;
  _hit_z0 = -999;

  _track_px = -999;
  _track_py = -999;
  _track_pz = -999;

  _mtrack_px = -999;
  _mtrack_py = -999;
  _mtrack_pz = -999;

  _track_e = -999;
  _mtrack_e = -999;
  _edep = -999;

  _bankid = -999;
  _volumeid = -999;
  _hitid = -999;
  _pid = -999;
  _mpid = -999;
  _trackid = -999;
  _mtrackid = -999;
  _otrackid = -999;

  _ptot = -999;

  _theta_true = -999;
  _theta_reco = -999;
  _mass_true = -999;
  _mass_reco = -999;

  return;
}


int
RICHParticleID::init_tree()
{
  _tree_rich = new TTree("pid_rich","RICH ParticleID info");

  _tree_rich->Branch("event", &_ievent, "Event number /I");
  _tree_rich->Branch("hit_x", &_hit_x0, "Global x-hit /D");
  _tree_rich->Branch("hit_y", &_hit_y0, "Global y-hit /D");
  _tree_rich->Branch("hit_z", &_hit_z0, "Global z-hit /D");
  _tree_rich->Branch("px", &_track_px, "Particle x-momentum /D");
  _tree_rich->Branch("py", &_track_py, "Particle y-momentum /D");
  _tree_rich->Branch("pz", &_track_pz, "Particle z-momentum /D");
  _tree_rich->Branch("mpx", &_mtrack_px, "Mother x-momentum /D");
  _tree_rich->Branch("mpy", &_mtrack_py, "Mother y-momentum /D");
  _tree_rich->Branch("mpz", &_mtrack_pz, "Mother z-momentum /D");
  _tree_rich->Branch("e", &_track_e, "Track energy /D");
  _tree_rich->Branch("me", &_mtrack_e, "Mother track energy /D");
  _tree_rich->Branch("edep", &_edep, "Energy deposited in material /D");
  _tree_rich->Branch("bankid", &_bankid, "Bank ID /I");
  _tree_rich->Branch("volumeid", &_volumeid, "Volume ID /I");
  _tree_rich->Branch("hitid", &_hitid, "Hit ID /I");
  _tree_rich->Branch("pid", &_pid, "Particle ID /I");
  _tree_rich->Branch("mpid", &_mpid, "Mother ID /I");
  _tree_rich->Branch("trackid", &_trackid, "Track ID /I");
  _tree_rich->Branch("mtrackid", &_mtrackid, "Mother track ID /I");
  _tree_rich->Branch("otrackid", &_otrackid, "Original track ID /I");

  _tree_rich->Branch("ptot", &_ptot, "Total momentum /D");
  _tree_rich->Branch("theta_true", &_theta_true, "True emission angle /F");
  _tree_rich->Branch("theta_reco", &_theta_reco, "Reconstructed emission angle /F");
  _tree_rich->Branch("mass_true", &_mass_true, "True particle mass /F");
  _tree_rich->Branch("mass_reco", &_mass_reco, "Reconstructed particle mass /F");

  return 0;
}
