
#include "FilterEvents.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <TFile.h>

#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrackMap_v2.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap_v1.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackbase_historic/TrackSeed_v1.h>
#include <trackbase_historic/SvtxTrackSeed_v1.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <g4vertex/GlobalVertexMap.h>
#include <g4vertex/GlobalVertex.h>
#include <calobase/RawClusterContainer.h>
#include <calobase/RawCluster.h>
#include <calobase/RawClusterv1.h>

#include <KFParticle.h>

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

FilterEvents::FilterEvents(const std::string &name) : SubsysReco(name)
{
  outnodename_trackmap = "SvtxTrackMap_ee";
  outnodename_cemc_clusters = "CLUSTER_CEMC_ee";
  EventNumber=0;
  goodEventNumber=0;

  pt_cut = 2;
  dca_cut = 0;
  chi2ndof_cut = 999.9;
  CEMC_use = true;
}

//-------------------------------------------------------------------------------

int FilterEvents::Init(PHCompositeNode *topNode)
{

cout << "FilterEvents::Init started..." << endl;

  PHNodeIterator iter(topNode);
  //PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  PHCompositeNode *dstNode = static_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", "DST"));
  if (!dstNode) { cerr << PHWHERE << " ERROR: DST node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

//  PHNodeIterator dstiter(dstNode);
//  PHCompositeNode *svtxNode = dynamic_cast<PHCompositeNode *>(dstiter.findFirst("PHCompositeNode", "SVTX"));
//  if (!svtxNode) { cerr << PHWHERE << " ERROR: SVTX node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
//
//  PHCompositeNode *cemcNode = dynamic_cast<PHCompositeNode *>(dstiter.findFirst("PHCompositeNode", "CEMC"));
//  if (!cemcNode) { cerr << PHWHERE << " ERROR: CEMC node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  SvtxTrackMap_v2* trackmap = new SvtxTrackMap_v2();
  PHCompositeNode *trackmapNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", outnodename_trackmap));
  if (!trackmapNode)
  {
    PHObjectNode_t *trackmapNode = new PHIODataNode<PHObject>(trackmap,outnodename_trackmap.c_str(),"PHObject");
      dstNode->addNode(trackmapNode);
    //svtxNode->addNode(trackmapNode);
    cout << PHWHERE << " INFO: added " << outnodename_trackmap << " node." << endl;
  }
  else { cout << PHWHERE << " INFO: " << outnodename_trackmap << " node already exists." << endl; }

  if(CEMC_use){
     _cemc_clusters_ee = new RawClusterContainer();
     PHIODataNode<PHObject> *clusterNode = new PHIODataNode<PHObject>(_cemc_clusters_ee, outnodename_cemc_clusters, "PHObject");
     dstNode->addNode(clusterNode);
  //cemcNode->addNode(clusterNode);
  }
//  PHCompositeNode *cemcclusNode = dynamic_cast<PHCompositeNode *>(iter.findFirst("PHCompositeNode", outnodename_cemc_clusters));
//  if (!cemcclusNode)
//  {
//    PHObjectNode_t *cemcclusNode = new PHIODataNode<PHObject>(cemc_clusters,outnodename_cemc_clusters.c_str(),"PHObject");
//    dstNode->addNode(cemcclusNode);
//    cout << PHWHERE << " INFO: added " << outnodename_cemc_clusters << " node." << endl;
//  }
//  else { cout << PHWHERE << " INFO: " << outnodename_cemc_clusters << " node already exists." << endl; }

cout << "FilterEvents::Init ended." << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEvents::InitRun(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------------------

int FilterEvents::GetNodes(PHCompositeNode *topNode)
{

  _topNode = topNode;

  //topNode->print();

  _trackmap_ee = findNode::getClass<SvtxTrackMap>(topNode, outnodename_trackmap);
  if(!_trackmap_ee) { cerr << PHWHERE << "ERROR: Output SvtxTrackMap_ee node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
//    else {cout << "Found SvtxTrackMap_ee node." << endl; }

  if (CEMC_use){
    _cemc_clusters_ee = findNode::getClass<RawClusterContainer>(topNode, outnodename_cemc_clusters);
    if(!_cemc_clusters_ee) { cerr << PHWHERE << "ERROR: CLUSTER_CEMC_ee node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
//    else {cout << "Found CLUSTER_CEMC_ee node." << endl; }
  }

  _trackmap = findNode::getClass<SvtxTrackMap>(topNode, "SvtxTrackMap");
  if(!_trackmap) { cerr << PHWHERE << "ERROR: SvtxTrackMap node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  
  _vtxmap = findNode::getClass<SvtxVertexMap_v1>(topNode, "SvtxVertexMap");
  if(!_vtxmap) { cout << "ERROR: SvtxVertexMap node not found!" << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackseedcontainer_svtx = findNode::getClass<TrackSeedContainer>(topNode, "SvtxTrackSeedContainer");
  if(!_trackseedcontainer_svtx) { cerr << PHWHERE << "ERROR: SvtxTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackseedcontainer_silicon = findNode::getClass<TrackSeedContainer>(topNode, "SiliconTrackSeedContainer");
  if(!_trackseedcontainer_silicon) { cerr << PHWHERE << "ERROR: SiliconTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trackseedcontainer_tpc = findNode::getClass<TrackSeedContainer>(topNode, "TpcTrackSeedContainer");
  if(!_trackseedcontainer_tpc) { cerr << PHWHERE << "ERROR: TpcTrackSeedContainer node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  _trkrclusters  = findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");
  if(!_trkrclusters) { cerr << PHWHERE << "ERROR: TRKR_CLUSTER node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  if (CEMC_use){
    _cemc_clusters = findNode::getClass<RawClusterContainer>(topNode, "CLUSTER_CEMC");
    if(!_cemc_clusters) { cerr << PHWHERE << "ERROR: CLUSTER_CEMC node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }
  }
  return Fun4AllReturnCodes::EVENT_OK;
}

//--------------------------------------------------------------------

TVector3 FilterEvents::GetProjectionCEMC(SvtxTrack* track) {

  TVector3 projection; // 0,0,0

  vector<double> proj;
  for (SvtxTrack::StateIter stateiter = track->begin_states(); stateiter != track->end_states(); ++stateiter)
  {
    SvtxTrackState *trackstate = stateiter->second;
    if(trackstate) { proj.push_back(trackstate->get_pathlength()); }
  }
  double pathlength = proj[proj.size()-3]; // CEMC is next next to last, usually 93.5

  SvtxTrackState* trackstate = track->get_state(pathlength); // at CEMC inner face
  if(trackstate) {
    double track_x = trackstate->get_x();
    double track_y = trackstate->get_y();
    double track_z = trackstate->get_z();
    projection.SetX(track_x);
    projection.SetY(track_y);
    projection.SetZ(track_z);
  }

  return projection;
}

//--------------------------------------------------------------------

RawCluster* FilterEvents::MatchClusterCEMC(SvtxTrack* track, RawClusterContainer* cemc_clusters, double &dphi, double &deta, double Zvtx) {

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
    double track_z = trackstate->get_z() - Zvtx;
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
        double cemc_z = cluster->get_z() - Zvtx;
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

int FilterEvents::process_event(PHCompositeNode *topNode) {

  EventNumber++;
  int ngood = 0;
  bool goodevent = false;

cout << "getting nodes..." << endl;
  GetNodes(topNode);

  GlobalVertexMap* _global_vtxmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");
  if(!_global_vtxmap) { cerr << PHWHERE << "ERROR: GlobalVertexMap node not found." << endl; return Fun4AllReturnCodes::ABORTEVENT; }

  double Zvtx = 0.;
  for (GlobalVertexMap::Iter iter = _global_vtxmap->begin(); iter != _global_vtxmap->end(); ++iter)
  {
    GlobalVertex *vtx = iter->second;
    if(vtx->get_id()==1) { Zvtx = vtx->get_z(); } // BBC vertex
  }
  cout << "Global BBC vertex Z = " << Zvtx << endl;

  std::vector<RawCluster *> goodclusters;
  //std::vector<TrkrCluster*> vclussilicon;
  //std::vector<TrkrCluster*> vclustpc;
  std::map<TrkrDefs::cluskey,TrkrCluster*> m_clusters;
  //std::map<TrkrDefs::cluskey> vcluskeytpc;
  std::vector<SvtxTrackSeed_v1*> v_svtx_trackseed;
  std::vector<TrackSeed*> v_silicon_trackseed;
  std::vector<TrackSeed*> v_tpc_trackseed;
 
  cout << "Total number of tracks = " << _trackmap->size() << endl;
  if (CEMC_use) cout << "Total number of CEMC clusters = " << _cemc_clusters->size() << endl;
  cout << "Total number of svtx track seeds: " << _trackseedcontainer_svtx->size() << endl;
  cout << "Total number of silicon track seeds: " << _trackseedcontainer_silicon->size() << endl;
  cout << "Total number of tpc track seeds: " << _trackseedcontainer_tpc->size() << endl;
  cout << "Total number of TRKR clusters: " << _trkrclusters->size() << endl;

  // Start loop over tracks;
  
  for (SvtxTrackMap::Iter iter = _trackmap->begin(); iter != _trackmap->end(); ++iter)
  {
    SvtxTrack *track = iter->second;

    double px = track->get_px();
    double py = track->get_py();
    double pz = track->get_pz();
    double pt = sqrt(px * px + py * py);
    double chi2ndf = track->get_chisq()/track->get_ndf(); 
    double dca;
    double dcaxy;
    double dcaz, dcaxy_sig, dcaz_sig;
    
    get_dca(track, _vtxmap, dca, dcaxy);
    get_dca_SvtxEval(track, _vtxmap, dcaxy, dcaz, dcaxy_sig, dcaz_sig);
      if(pt < pt_cut) continue;
      if(chi2ndf > chi2ndof_cut) continue;
      if(abs(dca) < dca_cut) continue;

    double mom = sqrt(px * px + py * py + pz * pz);

    double cemc_dphi = 99999.;
    double cemc_deta = 99999.;
    if (CEMC_use) 
    {
      RawCluster* clus;
      clus = MatchClusterCEMC(track, _cemc_clusters, cemc_dphi, cemc_deta, Zvtx);
      if(!clus && CEMC_use) continue;

      double cemc_ecore = 0.;
      cemc_ecore = clus->get_ecore();
      if(cemc_ecore/mom < 0.7) continue;
    }

    ngood++;
    goodevent = true;
    SvtxTrack* tmp =_trackmap_ee->insert(track);
    if(!tmp) cout << "ERROR: Failed to insert a track." << endl;

    cout << " Track: " << pt << endl;

    TrackSeed* trackseed_silicon = track->get_silicon_seed();
    double trackseed_silicon_pt = trackseed_silicon->get_pt();
    cout << "   Silicon seed: " << trackseed_silicon << " " << trackseed_silicon_pt << " " << trackseed_silicon->size_cluster_keys() << endl; 
    TrackSeed_v1* tmpseedsilicon = (TrackSeed_v1*)trackseed_silicon->CloneMe();
    v_silicon_trackseed.push_back(tmpseedsilicon);

    TrackSeed* trackseed_tpc = track->get_tpc_seed();
    double trackseed_tpc_pt = trackseed_tpc->get_pt();
    cout << "   TPC seed: " << trackseed_tpc << " "  << trackseed_tpc_pt << " " << trackseed_tpc->size_cluster_keys() << endl; 
    TrackSeed_v1* tmpseedtpc = (TrackSeed_v1*)trackseed_tpc->CloneMe();
    v_tpc_trackseed.push_back(tmpseedtpc);

// Find SVTX seed for this track
    for(TrackSeedContainer::ConstIter seediter = _trackseedcontainer_svtx->begin(); seediter != _trackseedcontainer_svtx->end(); ++seediter)
    {
      SvtxTrackSeed_v1* seed = (SvtxTrackSeed_v1*)*seediter;
      //TrackSeed* seed = *seediter;
      bool foundsiliconseed = false;
      bool foundtpcseed = false;
      unsigned int siliconid = seed->get_silicon_seed_index(); 
      unsigned int tpcid = seed->get_tpc_seed_index();
      //cout << "   SVTX seed: " << seed << " " << siliconid << " " << tpcid << endl;
      TrackSeed* tmptpcseed  = _trackseedcontainer_tpc->get(tpcid);
      double tmptpcseed_pt = tmptpcseed->get_pt();
      TrackSeed* tmpsiliconseed  = _trackseedcontainer_silicon->get(siliconid);
      double tmpsiliconseed_pt = tmpsiliconseed->get_pt();
      if(tmpsiliconseed_pt == trackseed_silicon_pt) {foundsiliconseed = true; cout << "   Found silicon seed " << tmpsiliconseed << endl;}
      if(tmptpcseed_pt == trackseed_tpc_pt) {foundtpcseed = true; cout << "   Found tpc seed " << tmptpcseed << endl;}
      if(foundsiliconseed && foundtpcseed) { 
        cout << "     THIS IS THE ONE: " << seed << " " << siliconid << " " << tpcid << endl; 
        SvtxTrackSeed_v1* tmpseed = (SvtxTrackSeed_v1*)seed->CloneMe();
        v_svtx_trackseed.push_back(tmpseed);
      }
    }

// Find all TRKR clusters for this track
    for(auto clusiter = trackseed_silicon->begin_cluster_keys(); clusiter != trackseed_silicon->end_cluster_keys(); ++clusiter)
    {
      //auto key = *clusiter;
      TrkrDefs::cluskey key = *clusiter;
      TrkrCluster* clus = _trkrclusters->findCluster(key);
      bool isinserted = false;
      for(std::map<TrkrDefs::cluskey,TrkrCluster*>::iterator it = m_clusters.begin(); it != m_clusters.end(); it++) {if(clus==it->second) {isinserted=true; break;}}
      if(!isinserted) {TrkrCluster* newclus = (TrkrClusterv4*)clus->CloneMe(); m_clusters.insert(std::make_pair(key,newclus));}
    }
    for(auto clusiter = trackseed_tpc->begin_cluster_keys(); clusiter != trackseed_tpc->end_cluster_keys(); ++clusiter)
    {
      //auto key = *clusiter;
      TrkrDefs::cluskey key = *clusiter;
      TrkrCluster* clus = _trkrclusters->findCluster(key);
      bool isinserted = false;
      for(std::map<TrkrDefs::cluskey,TrkrCluster*>::iterator it = m_clusters.begin(); it != m_clusters.end(); it++) {if(clus==it->second) {isinserted=true; break;}}
      if(!isinserted) {TrkrCluster* newclus = (TrkrClusterv4*)clus->CloneMe(); m_clusters.insert(std::make_pair(key,newclus));}
    }

    TVector3 proj = GetProjectionCEMC(track);
    double track_x = proj(0);
    double track_y = proj(1);
    double track_z = proj(2) - Zvtx;
    double track_r = sqrt(track_x*track_x+track_y*track_y);
    double track_eta = asinh( track_z / track_r );
    double track_phi = atan2( track_y, track_x );

  if (CEMC_use){
// Find all CEMC clusters around this track projection
      RawClusterContainer::Range begin_end = _cemc_clusters->getClusters();
      RawClusterContainer::Iterator clusiter;
      for (clusiter = begin_end.first; clusiter != begin_end.second; ++clusiter)
      {
        RawCluster* cluster = clusiter->second;
        if(!cluster) { cout << "ERROR: bad cluster pointer = " << cluster << endl; continue; }
        else {
          double cemc_ecore = cluster->get_ecore();
          if(cemc_ecore<1.0) continue;
          double cemc_x = cluster->get_x();
          double cemc_y = cluster->get_y();
          double cemc_z = cluster->get_z() - Zvtx;
          double cemc_r = cluster->get_r();
          double cemc_eta = asinh( cemc_z / cemc_r );
          double cemc_phi = atan2( cemc_y, cemc_x );
          double dist = sqrt(pow(cemc_phi-track_phi,2)+pow(cemc_eta-track_eta,2));
          if(dist<0.1) { 
            bool isinserted = false;
            for(unsigned int i=0; i<goodclusters.size(); i++) {if(cluster==goodclusters[i]) {isinserted=true; break;}}
            if(!isinserted) {RawCluster* newcluster = (RawClusterv1*)cluster->CloneMe(); goodclusters.push_back(newcluster);}
          }
        }
      } // end loop over cemc clusters

   }
  } // end loop over tracks

  cout << "   Number of CEMC clusters for output = " << goodclusters.size() << endl;
  cout << "   Number of svtx seeds for output = " << v_svtx_trackseed.size() << endl;
  cout << "   Number of tpc seeds for output = " << v_tpc_trackseed.size() << endl;
  cout << "   Number of silicon seeds for output = " << v_silicon_trackseed.size() << endl;

// Fill in selected CEMC clusters
  if (CEMC_use){
    _cemc_clusters->Reset();
    for (unsigned int cl = 0; cl < goodclusters.size(); cl++) { _cemc_clusters->AddCluster(goodclusters[cl]); }
  }

  cout << "   Number of TRKR clusters and keys for output: " << m_clusters.size() << " " << m_clusters.size() << endl;

  _trkrclusters->Reset();
  for(std::map<TrkrDefs::cluskey,TrkrCluster*>::iterator it = m_clusters.begin(); it != m_clusters.end(); it++) 
     { 
       //std::cout << it->first << std::endl;
       _trkrclusters->addClusterSpecifyKey(it->first,it->second); 
     }
  cout << "   New TRKR_CLUSTER size = " << _trkrclusters->size() << endl;


  _trackseedcontainer_svtx->Reset();
  for(unsigned int cl = 0; cl < v_svtx_trackseed.size(); cl++) 
    { _trackseedcontainer_svtx->insert(v_svtx_trackseed[cl]); }

/*
  _trackseedcontainer_silicon->Reset();
  for(unsigned int cl = 0; cl < v_silicon_trackseed.size(); cl++) 
    { _trackseedcontainer_silicon->insert(v_silicon_trackseed[cl]); }
  _trackseedcontainer_tpc->Reset();
  for(unsigned int cl = 0; cl < v_tpc_trackseed.size(); cl++) 
    { _trackseedcontainer_tpc->insert(v_tpc_trackseed[cl]); }
*/

  cout << "      New numbers of seeds = " << _trackseedcontainer_svtx->size() << " " << _trackseedcontainer_silicon->size() << " " << _trackseedcontainer_tpc->size() << endl;
  if(ngood>=2) { goodEventNumber++; }

  cout << ngood << " " << EventNumber << " " << goodEventNumber << endl;
  if(goodevent) {return Fun4AllReturnCodes::EVENT_OK;}  else {return Fun4AllReturnCodes::ABORTEVENT;}
}

//---------------------------------------------------------------------------

int FilterEvents::End(PHCompositeNode *topNode)
{
  cout << "Number of scanned events = " << EventNumber << endl;
  cout << "Number of good events = " << goodEventNumber << endl;
  return Fun4AllReturnCodes::EVENT_OK;
}

void FilterEvents::get_dca(SvtxTrack* track, SvtxVertexMap_v1* vertexmap,
                                  double& DCA, double& DCAxy)
{
    KFParticle::SetField(-1.4e0);
    
    float trackParameters[6] = {track->get_x(),
                                track->get_y(),
                                track->get_z(),
                                track->get_px(),
                                track->get_py(),
                                track->get_pz()};

    float trackCovariance[21];
    unsigned int iterate = 0;
    for (unsigned int i = 0; i < 6; ++i)
    {
        for (unsigned int j = 0; j <= i; ++j)
        {
            trackCovariance[iterate] = track->get_error(i, j);
            ++iterate;
        }   
    }

    KFParticle particle;
    particle.Create(trackParameters, trackCovariance, (Int_t) track->get_charge(), -1);
    particle.NDF() = track->get_ndf();
    particle.Chi2() = track->get_chisq();
    particle.SetId(track->get_id());

    auto vtxid = track->get_vertex_id();
    auto svtxVertex = vertexmap->get(vtxid);
    if(!svtxVertex)
    { 
      return; 
    }


    float vertexParameters[6] = {svtxVertex->get_x(),
                                 svtxVertex->get_y(),
                                 svtxVertex->get_z(), 0, 0, 0};

    float vertexCovariance[21];
    iterate = 0;
    for (unsigned int i = 0; i < 3; ++i)
    {    
        for (unsigned int j = 0; j <= i; ++j)
        {
            vertexCovariance[iterate] = svtxVertex->get_error(i, j);
            ++iterate;
        }
    }

    KFParticle vertex;
    vertex.Create(vertexParameters, vertexCovariance, 0, -1);
    vertex.NDF() = svtxVertex->get_ndof();
    vertex.Chi2() = svtxVertex->get_chisq();

    DCA = particle.GetDistanceFromVertex(vertex);
    DCAxy = particle.GetDistanceFromVertexXY(vertex);
}

void FilterEvents::get_dca_SvtxEval(SvtxTrack* track, SvtxVertexMap_v1* vertexmap,
			    double& dca3dxy, double& dca3dz,
			    double& dca3dxysigma, double& dca3dzsigma)
{
  Acts::Vector3 pos(track->get_x(),
		    track->get_y(),
		    track->get_z());
  Acts::Vector3 mom(track->get_px(),
		    track->get_py(),
		    track->get_pz());

  auto vtxid = track->get_vertex_id();
  auto svtxVertex = vertexmap->get(vtxid);
  if(!svtxVertex)
    { return; }
  Acts::Vector3 vertex(svtxVertex->get_x(),
		       svtxVertex->get_y(),
		       svtxVertex->get_z());

  pos -= vertex;

  Acts::ActsSymMatrix<3> posCov;
  for(int i = 0; i < 3; ++i)
    {
      for(int j = 0; j < 3; ++j)
	{
	  posCov(i, j) = track->get_error(i, j);
	} 
    }
  
  Acts::Vector3 r = mom.cross(Acts::Vector3(0.,0.,1.));
  float phi = atan2(r(1), r(0));
  
  Acts::RotationMatrix3 rot;
  Acts::RotationMatrix3 rot_T;
  rot(0,0) = cos(phi);
  rot(0,1) = -sin(phi);
  rot(0,2) = 0;
  rot(1,0) = sin(phi);
  rot(1,1) = cos(phi);
  rot(1,2) = 0;
  rot(2,0) = 0;
  rot(2,1) = 0;
  rot(2,2) = 1;
  
  rot_T = rot.transpose();

  Acts::Vector3 pos_R = rot * pos;
  Acts::ActsSymMatrix<3> rotCov = rot * posCov * rot_T;

  dca3dxy = pos_R(0);
  dca3dz = pos_R(2);
  dca3dxysigma = sqrt(rotCov(0,0));
  dca3dzsigma = sqrt(rotCov(2,2));
  
}
