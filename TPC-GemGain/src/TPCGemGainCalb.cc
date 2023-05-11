#include "TPCGemGainCalb.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/phool.h>
#include <TNtuple.h>
#include <TFile.h>

/// Tracking includes
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterv2.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterHitAssocv2.h>
#include <trackbase/TrkrHitTruthAssoc.h>
#include <trackbase_historic/SvtxTrack_v2.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertexMap.h>


using namespace std;

//____________________________________________________________________________..
TPCGemGainCalb::TPCGemGainCalb(const std::string &name):
 SubsysReco(name)
{

  fout = new TFile("readback_ntuple.root","RECREATE");

  ntp = new TNtuple("ntp", "ntp", "pt:x:y:z:dcaxy:dcaz:vtxid:nclus:qual");

  //cout << "TPCGemGainCalb::TPCGemGainCalb(const std::string &name) Calling ctor" << endl;
}

//____________________________________________________________________________..
TPCGemGainCalb::~TPCGemGainCalb()
{

}

//____________________________________________________________________________..
int TPCGemGainCalb::InitRun(PHCompositeNode *topNode)
{

  int ret = GetNodes(topNode);

  return ret;
}

//____________________________________________________________________________..
int TPCGemGainCalb::process_event(PHCompositeNode *topNode)
{
  if (Verbosity() >= 1) 
    cout << "TPCGemGainCalb::process_event(PHCompositeNode *topNode) Processing Event" << endl;

  double vertex_x = 0;
  double vertex_y = 0;
  double vertex_z = 0;

  // The vertex map
  std::cout << "Vertex map has " << _vertex_map->size() << " entries" << std::endl;  
  for (auto vert_iter = _vertex_map->begin();
       vert_iter != _vertex_map->end(); 
       ++vert_iter)
    {  
      auto vertex = vert_iter->second;
      vertex_x = vertex->get_x();
      vertex_y = vertex->get_y();
      vertex_z = vertex->get_z();

      std::cout << " vertex (x,y,z) = " << vertex_x << " , " << vertex_y << " , " << vertex_z << std::endl;
    }
  

  // Loop over all SvtxTracks

  int ntracks = 0;
  double mean_pt = 0;
  double mean_clusters = 0;

  float pt, x, y, z, dcaxy, dcaz, vtxid, nclus, qual;
  std::cout << "Track map has " << _track_map->size() << " entries" << std::endl;  
  for (auto phtrk_iter = _track_map->begin();
       phtrk_iter != _track_map->end(); 
       ++phtrk_iter)
    {
      _track = phtrk_iter->second;

      pt = _track->get_pt();
      x = _track->get_x()  ;
      y = _track->get_y();
      z = _track->get_z();
      dcaxy = _track->get_dca3d_xy();
      dcaz = _track->get_dca3d_z();
      vtxid = (float) _track->get_vertex_id();
      nclus = (float) _track-> size_cluster_keys();
      qual = _track->get_chisq() / (float) _track->get_ndf();

      if(Verbosity() > 0)
      std::cout
	<< __LINE__
	<< ": Processing itrack: " << phtrk_iter->first
	<< ": nclus: " << nclus
	<< ": pT " << pt
	<< " : x,y,z " << x 
	<< ", " << y
	<< ", " << z
	<< ", dcaxy " << dcaxy
	<< ", dcaz " << dcaz
	<< ", vtxid " << vtxid
	<< endl;

      ntp->Fill(_track->get_pt(),
		_track->get_x(), 
		_track->get_y(), 
		_track->get_z(), 
		_track->get_dca3d_xy(), 
		_track->get_dca3d_z(), 
		(float) _track->get_vertex_id(), 
		(float) _track->size_cluster_keys(),
		qual
		);
      
      ntracks ++;
      mean_pt += _track->get_pt();
      mean_clusters += _track->size_cluster_keys();
    
      double cluster_avge_adc = 0.0;
      double cluster_avge_wt = 0.0;

      // loop over associated clusters to get hits for track 
      for (SvtxTrack::ConstClusterKeyIter iter = _track->begin_cluster_keys();
	   iter != _track->end_cluster_keys();
	   ++iter)
	{
	  TrkrDefs::cluskey cluster_key = *iter;
	  unsigned int layer = TrkrDefs::getLayer(cluster_key);
	  unsigned int zelement = TrkrDefs::getZElement(cluster_key);
	  unsigned int phielement = TrkrDefs::getPhiElement(cluster_key);

	  TrkrCluster *cluster =  _cluster_map->findCluster(cluster_key);

	  if(Verbosity() > 2)
	    {
	      std::cout << "   cluster " << cluster_key << " layer " << layer << " zelement " << zelement  << " phielement " << phielement << std::endl;
	      
	      if(cluster)
		{
		  double radius = sqrt( cluster->getX()*cluster->getX() + cluster->getY()*cluster->getY() );
		  std::cout << "        cluster radius " << radius << " cluster adc " << cluster->getAdc() << std::endl;
		  if(layer > 6)
		    {
		      cluster_avge_adc += (double) cluster->getAdc();
		      cluster_avge_wt += 1.0;
		    }
		}
	      else
		std::cout << "Failed to find cluster with key :" << cluster_key << std::endl;
	    }

	}
      if(Verbosity() > 2)  std::cout << "   TPC cluster_avge_adc " << cluster_avge_adc/cluster_avge_wt << std::endl;
    }


  std::cout << "---- tracks with > 20 clusters " << ntracks << " mean nclusters " << mean_clusters /(double) ntracks << " mean pT " << mean_pt / (double) ntracks << std::endl;
  
  
  if (Verbosity() > 0)
    cout << "TPCGemGainCalb::process_event(PHCompositeNode *topNode) Leaving process_event" << endl;  
  
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TPCGemGainCalb::EndRun(PHCompositeNode *topNode)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TPCGemGainCalb::End(PHCompositeNode *topNode)
{
  fout->Write();
  //ntp->Write();
  //fout->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..

int  TPCGemGainCalb::GetNodes(PHCompositeNode* topNode)
{
  //---------------------------------
  // Get Objects off of the Node Tree
  //---------------------------------

  _cluster_map = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!_cluster_map)
  {
    cerr << PHWHERE << " ERROR: Can't find node TRKR_CLUSTER" << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }


  _vertex_map = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");
  if (!_vertex_map)
    {
      cerr << PHWHERE << " ERROR: Can't find SvtxVertexMap." << endl;
      return Fun4AllReturnCodes::ABORTEVENT;
    }

    
  _track_map = findNode::getClass<SvtxTrackMap>(topNode,  "SvtxTrackMap");
  if (!_track_map)
  {
    cerr << PHWHERE << " ERROR: Can't find SvtxTrackMap: " << endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

