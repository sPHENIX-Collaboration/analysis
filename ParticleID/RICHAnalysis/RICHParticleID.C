#include "RICHParticleID.h"
#include "PidInfo_RICH_v1.h"
#include "PidInfoContainer.h"
#include "TrackProjectorPid.h"
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

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
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
  _detector(richname),
  _trackmap_name(tracksname),
  _refractive_index(1),
  _pidinfos(nullptr),
  _trackproj(nullptr),
  _acquire(nullptr),
  _particleid(nullptr),
  _radius(220.)
{
  _richhits_name = "G4HIT_" + _detector;
  _pidinfo_node_name = "PIDINFO_" + _detector;
}


int
RICHParticleID::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  /* create particleid object */
  _particleid = new PIDProbabilities();

  /* Throw warning if refractive index is not greater than 1 */
  if ( _refractive_index <= 1 )
    cout << PHWHERE << " WARNING: Refractive index for radiator volume is " << _refractive_index << endl;

  return 0;
}


int
RICHParticleID::InitRun(PHCompositeNode *topNode)
{
  /* Create nodes for ParticleID */
  try
    {
      CreateNodes(topNode);
    }
  catch (std::exception &e)
    {
      std::cout << PHWHERE << ": " << e.what() << std::endl;
      throw;
    }

  /* create track projector object */
  _trackproj = new TrackProjectorPid( topNode );

  /* create acquire object */
  _acquire = new SetupDualRICHAnalyzer();

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

      bool use_reconstructed_momentum = true;
      bool use_truth_momentum = false;
      bool use_emission_momentum = false;

      bool use_reconstructed_point = true;
      bool use_approximate_point = false;


      SvtxTrack* track_j = dynamic_cast<SvtxTrack*>(track_itr->second);

      /* Check if track_j is a null pointer. */
      if (track_j == NULL)
        continue;

      /* See if there's already a PidInfo object for this track. If yes, retrieve it- if not, create a new one. */
      PidInfo *pidinfo_j = _pidinfos->getPidInfo( track_j->get_id() );
      if (!pidinfo_j)
        {
          pidinfo_j = new PidInfo_RICH_v1( track_j->get_id() );
          _pidinfos->AddPidInfo( pidinfo_j );
	}

      /* Fill momv object which is the normalized momentum vector of the track in the RICH (i.e. its direction) */
      double momv[3] = {0.,0.,0.};

      if (use_reconstructed_momentum) {
	/* 'Continue' with next track if RICH projection not found for this track */
	if ( ! _trackproj->get_projected_momentum( track_j, momv, TrackProjectorPid::SPHERE, _radius ) )
	  {
	    cout << "RICH track projection momentum NOT FOUND; next iteration" << endl;
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


      /* Get mean emission point from track in RICH */
      double m_emi[3] = {0.,0.,0.};
      
      if (use_reconstructed_point) {
	/* 'Continue' with next track if RICH projection not found for this track */
	if ( ! _trackproj->get_projected_position( track_j, m_emi, TrackProjectorPid::SPHERE, _radius ) )
	  {
	    cout << "RICH track projection position NOT FOUND; next iteration" << endl;
	    continue;
	  }
      }
      if (use_approximate_point) {
	m_emi[0] = ((_radius)/momv[2])*momv[0];
	m_emi[1] = ((_radius)/momv[2])*momv[1];
	m_emi[2] = ((_radius)/momv[2])*momv[2];	
      }


      /* 'Continue' with next track if track doesn't pass through RICH */
      if ( ! _trackproj->is_in_RICH( momv ) )
	continue;

      
      //cout << "Emission point: " << m_emi[0] << " " << m_emi[1] << " " << m_emi[2] << endl;
      //cout << "Momentum vector: " << momv[0] << " " << momv[1] << " " << momv[2] << endl;
      
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
      long double probs[4] = {0.,0.,0.,0.};
      double momv_magnitude = 0;

      /* Emission point momentum only gives a valid unit vector, not magnitude */
      if ( use_reconstructed_momentum )
	momv_magnitude = momv_norm;
      if ( use_truth_momentum )
	momv_magnitude = momv_norm;
      if ( use_emission_momentum )
	{
	  PHG4Particle* particle = truthinfo->GetParticle( track_j->get_truth_track_id() );
	  double px = particle->get_px();
	  double py = particle->get_py();
	  double pz = particle->get_pz();
	  momv_magnitude = sqrt( px*px + py*py + pz*pz );
	}

      if ( !_particleid->particle_probs( angles, momv_magnitude, _refractive_index, probs ) )
	{
	  cout << "No particle ID: ParticleID::particle_probs gives no output" << endl;
	  continue;
	}

      /* Set particle probabilities */
      pidinfo_j->set_likelihood(PidInfo::ELECTRON, probs[0]);
      pidinfo_j->set_likelihood(PidInfo::CHARGEDPION, probs[1]);
      pidinfo_j->set_likelihood(PidInfo::CHARGEDKAON, probs[2]);
      pidinfo_j->set_likelihood(PidInfo::PROTON, probs[3]);
      
    } // END loop over tracks

  return 0;
}

int
RICHParticleID::End(PHCompositeNode *topNode)
{
  return 0;
}

void RICHParticleID::CreateNodes(PHCompositeNode *topNode)
{
  PHNodeIterator iter(topNode);

  PHCompositeNode *dstNode = static_cast<PHCompositeNode*>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode)
    {
      std::cerr << PHWHERE << "DST Node missing, doing nothing." << std::endl;
      throw std::runtime_error("Failed to find DST node in RICHParticleID::CreateNodes");
    }

  PHNodeIterator dstiter(dstNode);
  PHCompositeNode *DetNode = dynamic_cast<PHCompositeNode*>(dstiter.findFirst("PHCompositeNode",_detector ));
  if(!DetNode){
    DetNode = new PHCompositeNode(_detector);
    dstNode->addNode(DetNode);
  }

  _pidinfos = new PidInfoContainer();
  PHIODataNode<PHObject> *pidInfoNode = new PHIODataNode<PHObject>(_pidinfos, _pidinfo_node_name.c_str(), "PHObject");
  DetNode->addNode(pidInfoNode);
}
