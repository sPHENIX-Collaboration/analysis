#include "FastPid_RICH.h"
#include "PidInfo_RICH_v1.h"
#include "PidInfoContainer.h"
#include "TrackProjectorPid.h"

// Fun4All includes
#include <fun4all/Fun4AllServer.h>
#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/getClass.h>
#include <phool/PHCompositeNode.h>

#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>

#include <g4hough/SvtxTrackMap.h>
#include <g4hough/SvtxTrack.h>
#include <g4hough/SvtxTrackState.h>

// ROOT includes

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

FastPid_RICH::FastPid_RICH(std::string tracksname, std::string richname) :
  SubsysReco("FastPid_RICH" ),
  _ievent(0),
  _detector(richname),
  _trackmap_name(tracksname),
  _pidinfos(nullptr),
  _trackproj(nullptr),
  _radius(220.)
{
  _pidinfo_node_name = "PIDINFO_" + _detector;
}


int
FastPid_RICH::Init(PHCompositeNode *topNode)
{
  _verbose = false;
  _ievent = 0;

  return 0;
}


int
FastPid_RICH::InitRun(PHCompositeNode *topNode)
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

  return 0;
}


int
FastPid_RICH::process_event(PHCompositeNode *topNode)
{
  _ievent ++;

  /* Get truth information (temporary) */
  PHG4TruthInfoContainer* truthinfo = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");

  /* Get track collection with all tracks in this event */
  SvtxTrackMap* trackmap = findNode::getClass<SvtxTrackMap>(topNode,_trackmap_name);

  /* Check if collections found */
  if (!truthinfo) {
    cout << PHWHERE << "PHG4TruthInfoContainer not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }
  if (!trackmap) {
    cout << PHWHERE << "SvtxTrackMap node not found on node tree" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  /* Loop over tracks */
  for (SvtxTrackMap::ConstIter track_itr = trackmap->begin(); track_itr != trackmap->end(); track_itr++)
    {
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

      /* project track to RICH volume and calculate parametrized particle ID response */
      /* Get mean emission point from track in RICH volume */
      SvtxTrackState *state_j_at_rich = _trackproj->project_track( track_j, TrackProjectorPid::SPHERE, _radius );
      if ( ! state_j_at_rich )
        {
          cout << "RICH track projection position NOT FOUND; next iteration" << endl;
	  continue;
        }

      /* Attach track state to PidInfo object */
      pidinfo_j->set_track_state( state_j_at_rich );

      /* Get truth particle associated with track */
      PHG4Particle* particle = truthinfo->GetParticle( track_j->get_truth_track_id() );

      /* Get particle ID */
      int pid = particle->get_pid();

      /* Use parametrized particle ID based on position and momentum at point of track projection */
      /* @TODO: Implement parametrized particle ID */
      { // beginning of parametrized RICH response

	/* approximate RICH acceptance */
	float eta_min = 1.45;
	float eta_max = 3.5;

	/* approximate momentum range for pion / kaon separation */
	float p_min = 3.;
	float p_max = 50.;

	/* check if track projected state in RICH acceptance */
	if ( pidinfo_j->get_track_state()->get_eta() > eta_min && pidinfo_j->get_track_state()->get_eta() < eta_max )
	  {
	    if ( pidinfo_j->get_track_state()->get_p() > p_min && pidinfo_j->get_track_state()->get_p() < p_max )
	      {
		/* identified kaon */
		if ( abs( pid ) == 211 )
		  pidinfo_j->set_likelihood(PidInfo::CHARGEDPION, 1);

		/* identified kaon */
		else if ( abs( pid ) == 321 )
		  pidinfo_j->set_likelihood(PidInfo::CHARGEDKAON, 1);
	      }
	  }
      } // end of parametrized RICH response

      /* print some information to screen */
      cout << "True PID: " << pid << endl;
      cout << "Position: " << pidinfo_j->get_track_state()->get_x() << ", " << pidinfo_j->get_track_state()->get_y() << ", " << pidinfo_j->get_track_state()->get_z() << endl;
      cout << "Momentum: " << pidinfo_j->get_track_state()->get_px() << ", " << pidinfo_j->get_track_state()->get_py() << ", " << pidinfo_j->get_track_state()->get_pz() << endl;
      cout << "Eta, |p|: " << pidinfo_j->get_track_state()->get_eta() << ", " << pidinfo_j->get_track_state()->get_p() << endl;
      cout << "Likelihood (electron):     " << pidinfo_j->get_likelihood(PidInfo::ELECTRON) << endl;
      cout << "Likelihood (charged pion): " << pidinfo_j->get_likelihood(PidInfo::CHARGEDPION) << endl;
      cout << "Likelihood (charged kaon): " << pidinfo_j->get_likelihood(PidInfo::CHARGEDKAON) << endl;
      cout << "Likelihood (proton):       " << pidinfo_j->get_likelihood(PidInfo::PROTON) << endl;

    } // END loop over tracks

  return 0;
}

int
FastPid_RICH::End(PHCompositeNode *topNode)
{
  return 0;
}

void FastPid_RICH::CreateNodes(PHCompositeNode *topNode)
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
