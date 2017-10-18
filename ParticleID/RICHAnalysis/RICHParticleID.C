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
#include "g4main/PHG4TruthInfoContainer.h"

#include <g4hough/SvtxTrackMap_v1.h>
#include <g4hough/SvtxTrack_FastSim.h>
#include <g4hough/SvtxTrackState.h>

// ROOT includes
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TMath.h>

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

RICHParticleID::RICHParticleID(std::string tracksname, std::string richname, std::string filename) :
  SubsysReco("RICHParticleID" ),
  _ievent(0),
  _trackmap_name(tracksname),
  _richhits_name(richname),
  _foutname(filename),
  _fout_root(nullptr)
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

  cout << "Track map size: " << trackmap->size() << endl;
  cout << "RICH hits size: " << richhits->size() << endl;

  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin();
       track_itr != trackmap->end(); track_itr++) {

    SvtxTrack_FastSim* track_j = dynamic_cast<SvtxTrack_FastSim*>(track_itr->second);


    /* Get mean emission point from track in RICH */
    double m_emi[3] = {0.,0.,0.};

    /* 'Continue' with next track if RICH not found in state list for this track */
    if ( ! get_position_from_track_state( track_j, "RICH", m_emi ) )
      continue;


    /* Fill momv object which is the normalized momentum vector of the track in the RICH (i.e. its direction) */
    double momv[3] = {0.,0.,0.};

    /* 'Continue' with next track if RICH not found in state list for this track */
    if ( ! get_momentum_from_track_state( track_j, "RICH", momv ) )
      continue;

    double momv_norm = sqrt( momv[0]*momv[0] + momv[1]*momv[1] + momv[2]*momv[2] );
    momv[0] /= momv_norm;
    momv[1] /= momv_norm;
    momv[2] /= momv_norm;


    /* Loop over all G4Hits in container (i.e. RICH photons in event) */
    PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
    PHG4HitContainer::ConstIterator rich_hits_iter;

    for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
      {
        PHG4Hit *hit_i = rich_hits_iter->second;

        float theta_c = calculate_emission_angle( m_emi, momv, hit_i );

        /* Set reconstructed emission angle for output tree */
        _theta_reco = theta_c;

        /* Calculate true emission angle for output tree */
        _theta_true = 0;

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

  /* Troubleshoot */
  cout << "Parameters into ind_ray():" << endl;
  cout << "Ex, Ey, Ez = " << Ex << " " << Ey << " " << Ez << endl;
  cout << "Dx, Dy, Dz = " << Dx << " " << Dy << " " << Dz << endl;
  cout << "vx, vy, vz = " << vx << " " << vy << " " << vz << endl;

  /* Set mirror parameters */
  double R_mirror = 195; // cm
  _analyzer->set_mirror(cx, cy, cz, R_mirror);

  /* Call algorithm to determine emission angle of photon i w.r.t. track j */
  float theta_c = _analyzer->ind_ray(Ex, Ey, Ez, Dx, Dy, Dz, vx, vy, vz, select_radiator); //Indirect Ray Tracing

  return theta_c;
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
  
  cout << "in position function, array = " << arr_pos[0] << " " << arr_pos[1] << " " << arr_pos[2] << endl;

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

  cout << "in momentum function, array = " << arr_mom[0] << " " << arr_mom[1] << " " << arr_mom[2] << endl;

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
  _theta_true = -1;
  _theta_reco = -1;

  return;
}


int
RICHParticleID::init_tree()
{
  _tree_rich = new TTree("pid_rich","RICH ParticleID info");

  _tree_rich->Branch("event", &_ievent, "Event number /I");
  _tree_rich->Branch("theta_true", &_theta_true, "True emission angle /F");
  _tree_rich->Branch("theta_reco", &_theta_reco, "Reconstructed emission angle /F");

  return 0;
}
