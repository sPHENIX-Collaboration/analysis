#include "RICHParticleID.h"
#include "SetupDualRICHAnalyzer.h"
#include "PIDProbabilities.h"

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
#include <TH1.h>

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

RICHParticleID::RICHParticleID(std::string tracksname, std::string richname) :
  SubsysReco("RICHParticleID" ),
  _ievent(0),
  _trackmap_name(tracksname),
  _richhits_name(richname),
  _trackstate_name("RICH"),
  _refractive_index(1),
  _acquire(nullptr),
  _particleid(nullptr)
{

}


int
RICHParticleID::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  /* create acquire object */
  _acquire = new SetupDualRICHAnalyzer();

  /* create particleid object */
  _particleid = new PIDProbabilities();

  /* Throw warning if refractive index is not greater than 1 */
  if ( _refractive_index <= 1 )
    cout << PHWHERE << " WARNING: Refractive index for radiator volume is " << _refractive_index << endl;

  return 0;
}


int
RICHParticleID::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  /* Get truth information (temporary) */
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* Get all photon hits in RICH for this event */
  PHG4HitContainer* richhits = findNode::getClass<PHG4HitContainer>(topNode,_richhits_name);

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name);

  /* Check if collections found */
  if (!truthinfo) {
    cout << PHWHERE << "PHG4TruthInfoContainer not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!richhits) {
    cout << PHWHERE << "PHG4HitContainer not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!trackmap) {
    cout << PHWHERE << "SvtxTrackMap node not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin(); track_itr != trackmap->end(); track_itr++)
    {
      
      SvtxTrack_FastSim* track_j = dynamic_cast<SvtxTrack_FastSim*>(track_itr->second);

      /* Fill momv object which is the normalized momentum vector of the track in the RICH (i.e. its direction) */
      double momv[3] = {0.,0.,0.};

      bool use_reconstructed_momentum = true;
      bool use_truth_momentum = false;
      bool use_emission_momentum = false;

      if (use_reconstructed_momentum) {
	/* 'Continue' with next track if RICH not found in state list for this track */
	if ( ! _acquire->get_momentum_from_track_state( track_j, _trackstate_name, momv ) )
	  {
	    cout << "RICH state found in state list for momentum; next iteration" << endl;
	    continue;
	  }
      }
      if (use_truth_momentum) {
	/* Fill with truth momentum instead of reco */
	if ( ! _acquire->get_true_momentum( truthinfo, track_j, momv) )
	  {
	    cout << "No truth momentum found for track; next iteration" << endl;
	    continue;
	  }
      }
      if (use_emission_momentum) {
	/* Fill with vector constructed from emission points (from truth) */
	if ( ! _acquire->get_emission_momentum( truthinfo, richhits, track_j, momv) )
	  {
	    cout << "No truth momentum from emission points found for track; next iteration" << endl;
	    continue;
	  }
      }

      double momv_norm = sqrt( momv[0]*momv[0] + momv[1]*momv[1] + momv[2]*momv[2] );
      momv[0] /= momv_norm;
      momv[1] /= momv_norm;
      momv[2] /= momv_norm;

      cout << "Momentum vector: " << momv[0] << " " << momv[1] << " " << momv[2] << endl;

      /* Get mean emission point from track in RICH */
      double m_emi[3] = {0.,0.,0.};

      bool use_reconstructed_point = true;
      bool use_approximate_point = false;
      
      if (use_reconstructed_point) {
	/* 'Continue' with next track if RICH not found in state list for this track */
	if ( ! _acquire->get_position_from_track_state( track_j, _trackstate_name, m_emi ) )
	  {
	    cout << "RICH state found in state list for position; next iteration" << endl;
	    continue;
	  }
      }
      if (use_approximate_point) {
	m_emi[0] = ((220.)/momv[2])*momv[0];
	m_emi[1] = ((220.)/momv[2])*momv[1];
	m_emi[2] = ((220.)/momv[2])*momv[2];	
      }

      cout << "Emission point: " << m_emi[0] << " " << m_emi[1] << " " << m_emi[2] << endl;

      
      /* Vector of reconstructed angles to pass to PID */
      vector<float> angles;

      
      /* Loop over all G4Hits in container (RICH photons in event) */
      PHG4HitContainer::ConstRange rich_hits_begin_end = richhits->getHits();
      PHG4HitContainer::ConstIterator rich_hits_iter;
      
      for (rich_hits_iter = rich_hits_begin_end.first; rich_hits_iter !=  rich_hits_begin_end.second; ++rich_hits_iter)
	{
	  
	  PHG4Hit *hit_i = rich_hits_iter->second;

	  /* Calculate reconstructed emission angle for output, fill Cherenkov array */
	  double _theta_reco = _acquire->calculate_emission_angle( m_emi, momv, hit_i );
	  angles.push_back(_theta_reco);
	  
	} // END loop over photons
      

      /* Calculate particle probabilities */
      double probs[4] = {0.,0.,0.,0.};
      cout << "PARTICLE PROBS" << endl;
      if ( !_particleid->particle_probs( angles, momv_norm, _refractive_index, probs ) )
	{
	  cout << "No particle ID: ParticleID::particle_probs gives no output" << endl;
	  continue;
	}

      /* Set particle probabilities */
      /*      track_j->set_pid_probability(SvtxTrack::ELECTRON, probs[0]);
      track_j->set_pid_probability(SvtxTrack::CHARGEDPION, probs[1]);
      track_j->set_pid_probability(SvtxTrack::CHARGEDKAON, probs[2]);
      track_j->set_pid_probability(SvtxTrack::PROTON, probs[3]);
      
      cout << track_j->get_pid_probability(SvtxTrack::ELECTRON) << endl;
      cout << track_j->get_pid_probability(SvtxTrack::CHARGEDPION) << endl;
      cout << track_j->get_pid_probability(SvtxTrack::CHARGEDKAON) << endl;
      cout << track_j->get_pid_probability(SvtxTrack::PROTON) << endl;
      cout << "PARTICLE PROBS DONE" << endl;
      */
      
      for (int m=0;m<4;m++)
	cout << probs[m] << endl;


    } // END loop over tracks
  
  return 0;
}

int
RICHParticleID::End(PHCompositeNode *topNode)
{
  return 0;
}
