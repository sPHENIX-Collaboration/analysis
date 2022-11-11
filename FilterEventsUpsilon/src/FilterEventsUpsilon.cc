
#include "FilterEventsUpsilon.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include "TFile.h"

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v2.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase/TrkrDefs.h>
#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <phool/getClass.h>
#include <phool/recoConsts.h>
#include <phool/PHCompositeNode.h>
#include <phool/PHIODataNode.h>
#include <phool/PHNodeIterator.h>
#include <phool/PHRandomSeed.h>
#include <fun4all/Fun4AllReturnCodes.h>

typedef PHIODataNode<PHObject> PHObjectNode_t;

using namespace std;

//==============================================================

FilterEventsUpsilon::FilterEventsUpsilon(const std::string &name) : SubsysReco(name)
{
  outnodename = "SvtxTrackMap_ee";
  EventNumber=0;
}

//-------------------------------------------------------------------------------

int FilterEventsUpsilon::Init(PHCompositeNode *topNode)
{

  PHNodeIterator iter(topNode);
  PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode) { cerr << PHWHERE << " ERROR: DST node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  SvtxTrackMap_v2* eePairs = new SvtxTrackMap_v2();

  PHCompositeNode *PairNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", outnodename));
  if (!PairNode)
  {
    PHObjectNode_t *PairNode = new PHIODataNode<PHObject>(eePairs,outnodename.c_str(),"PHObject");
    dstNode->addNode(PairNode);
    cout << PHWHERE << " INFO: added " << outnodename << endl;
  }
  else { cout << PHWHERE << " INFO: " << outnodename << " already exists." << endl; }

  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEventsUpsilon::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEventsUpsilon::GetNodes(PHCompositeNode *topNode)
{

  _topNode = topNode;

  _trackmap_ee = findNode::getClass<SvtxTrackMap>(topNode, outnodename);
  if(!_trackmap_ee) { cerr << PHWHERE << "ERROR: Output SvtxTrackMap_ee node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!_trackmap) { cerr << PHWHERE << "ERROR: SvtxTrackMap nodei not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _vtxmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if(!_vtxmap) { cout << "ERROR: SvtxVertexMap node not found!" << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!_global_vtxmap) { cerr << PHWHERE << "ERROR: GlobalVertexMap node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
  if(!_cemc_clusters) { cerr << PHWHERE << "ERROR: CLUSTER_CEMC node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------

RawCluster* FilterEventsUpsilon::MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta) {

  RawCluster* returnCluster = NULL;
  double track_eta = 99999.;
  double track_phi = 99999.;
  dphi = 99999.;
  deta = 99999.;

  vector<double> proj;
  for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
  {
    SvtxTrackState *trackstate = stateiter->second;
    if(trackstate) { proj.push_back(trackstate->get_pathlength()); }
  }
  double pathlength = proj[proj.size()-3]; // CEMC is next next to last

  SvtxTrackState* trackstate = track->get_state(pathlength); // at CEMC inner face
  if(trackstate) {
    double track_x = trackstate->get_x();
    double track_y = trackstate->get_y();
    double track_z = trackstate->get_z();
    double track_r = sqrt(track_x*track_x+track_y*track_y);
    track_eta = asinh( track_z / track_r );
    track_phi = atan2( track_y, track_x );
  } else { return returnCluster; }


  if(track_eta == 99999. || track_phi == 99999.) { return returnCluster; }
  double dist = 99999.;

    RawClusterContainer::Range begin_end = cemc_clusters->getClusters();
    RawClusterContainer::Iterator iter;
    for (iter = begin_end.first; iter != begin_end.second; ++iter)
    {
      RawCluster* cluster = iter->second;
      if(!cluster) continue;
      else {
        double cemc_ecore = cluster->get_ecore();
        if(cemc_ecore<1.0) continue;
        double cemc_x = cluster->get_x();
        double cemc_y = cluster->get_y();
        double cemc_z = cluster->get_z();
        double cemc_r = cluster->get_r();
        double cemc_eta = asinh( cemc_z / cemc_r );
        double cemc_phi = atan2( cemc_y, cemc_x );
        double tmpdist = sqrt(pow((cemc_eta-track_eta),2)+pow((cemc_phi-track_phi),2));
        if(tmpdist<dist) {
          dist = tmpdist; returnCluster = cluster; dphi = fabs(cemc_phi-track_phi); deta = fabs(cemc_eta-track_eta);
        }
      }
    }

  return returnCluster;
}

//--------------------------------------------------------------------------------

int FilterEventsUpsilon::process_event(PHCompositeNode *topNode) {

  EventNumber++;

  GetNodes(topNode);

  for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;

    double px = track->get_px();
    double py = track->get_py();
    double pz = track->get_pz();
    double pt = sqrt(px * px + py * py);
      if(pt<2.0) continue;
    double mom = sqrt(px * px + py * py + pz * pz);

    double cemc_dphi = 99999.;
    double cemc_deta = 99999.;
    RawCluster* clus = MatchClusterCEMC(track, _cemc_clusters, cemc_dphi, cemc_deta);
      if(!clus) continue;
    double cemc_ecore = clus->get_ecore();
      if(cemc_ecore/mom < 0.7) continue;

    SvtxTrack* tmp =_trackmap_ee->insert(track);
    if(!tmp) cout << "ERROR: Failed to insert a track." << endl;

  }

  return Fun4AllReturnCodes::EVENT_OK;
}

int FilterEventsUpsilon::End(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

