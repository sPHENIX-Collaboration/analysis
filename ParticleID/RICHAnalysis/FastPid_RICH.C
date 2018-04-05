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
  _trackproj(nullptr)
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

      cout << "FOUND PARTICLE with true PID = " << pid << endl;

      /* Set particle probabilities */
      pidinfo_j->set_probability(PidInfo::ELECTRON, 0);
      pidinfo_j->set_probability(PidInfo::CHARGEDPION, 0);
      pidinfo_j->set_probability(PidInfo::CHARGEDKAON, 0);
      pidinfo_j->set_probability(PidInfo::PROTON, 0);

      if ( abs( pid ) == 11 )
        pidinfo_j->set_probability(PidInfo::ELECTRON, 1);
      else if ( abs( pid ) == 211 )
        pidinfo_j->set_probability(PidInfo::CHARGEDPION, 1);
      else if ( abs( pid ) == 2212 )
        pidinfo_j->set_probability(PidInfo::PROTON, 1);

      //cout << "Probability (electron):     " << pidinfo_j->get_probability(PidInfo::ELECTRON) << endl;
      //cout << "Probability (charged pion): " << pidinfo_j->get_probability(PidInfo::CHARGEDPION) << endl;
      //cout << "Probability (charged kaon): " << pidinfo_j->get_probability(PidInfo::CHARGEDKAON) << endl;
      //cout << "Probability (proton):       " << pidinfo_j->get_probability(PidInfo::PROTON) << endl;

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
