#include "FastPid_RICH.h"
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

// ROOT includes

// other C++ includes
#include <cassert>
#include <algorithm>

using namespace std;

FastPid_RICH::FastPid_RICH(std::string tracksname, std::string richname) :
  SubsysReco("FastPid_RICH" ),
  _ievent(0),
  _trackmap_name(tracksname),
  _trackproj(nullptr)
{

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

      /* Check if track_j is a null pointer- this can happen if returned track object is NOT of
	 type SvtxTrack_FastSim for dynamic_cast. */
      if (track_j == NULL)
        continue;

      /* project track to RICH volume and calculate parametrized particle ID response */
      /* @TODO: Implement parametrization */

      /* Get mean emission point from track in RICH */
      double m_emi[3] = {0.,0.,0.};

      if ( ! _trackproj->get_projected_position( track_j, m_emi ) )
	{
	  cout << "RICH track projection position NOT FOUND; next iteration" << endl;
	  continue;
	}

      /* Get truth particle associated with track */
      PHG4Particle* particle = truthinfo->GetParticle( track_j->get_truth_track_id() );

      /* Get particle ID */
      int pid = particle->get_pid();

      cout << "Found particle with true PID " << pid << " in RICH" << endl;

    } // END loop over tracks

  return 0;
}

int
FastPid_RICH::End(PHCompositeNode *topNode)
{
  return 0;
}
