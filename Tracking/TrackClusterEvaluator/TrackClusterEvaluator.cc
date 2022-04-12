
#include "TrackClusterEvaluator.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/Fun4AllServer.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>

#include <trackbase/ActsSurfaceMaps.h>
#include <trackbase/ActsTrackingGeometry.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/TrkrHit.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>

#include <trackbase_historic/ActsTransformations.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/SvtxVertexMap.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxTrackEval.h>
#include <g4eval/SvtxTruthEval.h>

#include <phool/PHCompositeNode.h>

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>

//____________________________________________________________________________..
TrackClusterEvaluator::TrackClusterEvaluator(const std::string &name)
  : SubsysReco(name)
{

}

//____________________________________________________________________________..
TrackClusterEvaluator::~TrackClusterEvaluator()
{

}

//____________________________________________________________________________..
int TrackClusterEvaluator::Init(PHCompositeNode*)
{

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TrackClusterEvaluator::InitRun(PHCompositeNode *topNode)
{
  int returnval = getNodes(topNode);
  event = m_proc * m_nevent;
  m_outfile = new TFile(m_outfilename.c_str(), "RECREATE");
  setupTrees();

  return returnval;
}

//____________________________________________________________________________..
int TrackClusterEvaluator::process_event(PHCompositeNode *topNode)
{
  if (!m_svtxevalstack)
  {
    m_svtxevalstack = new SvtxEvalStack(topNode);
    m_svtxevalstack->set_strict(false);
    m_svtxevalstack->set_verbosity(Verbosity());
    m_svtxevalstack->set_use_initial_vertex(true);
    m_svtxevalstack->set_use_genfit_vertex(false);
    m_svtxevalstack->next_event(topNode);
  }
  else
  {
    m_svtxevalstack->next_event(topNode);
  }

  if (Verbosity() > 0)
  {
    std::cout << "Analyzing truth " << std::endl;
  }
  if (m_truthContainer)
  {
    processTruthTracks(topNode);
  }
  if (Verbosity() > 0)
  {
    std::cout << "Analyzing reco " << std::endl;
  }
  if (m_trackMap)
  {
    processRecoTracks(topNode);
  }

  event++;

  return Fun4AllReturnCodes::EVENT_OK;
}

void TrackClusterEvaluator::processTruthTracks(PHCompositeNode *topNode)
{
  SvtxTruthEval *trutheval = m_svtxevalstack->get_truth_eval();
  SvtxClusterEval *clustereval = m_svtxevalstack->get_cluster_eval();
  SvtxTrackEval *trackeval = m_svtxevalstack->get_track_eval();
  auto surfmaps = findNode::getClass<ActsSurfaceMaps>(topNode, "ActsSurfaceMaps");
  auto tgeometry = findNode::getClass<ActsTrackingGeometry>(topNode, "ActsTrackingGeometry");

  PHG4TruthInfoContainer::ConstRange range = m_truthContainer->GetParticleRange();

  if (m_scanForPrimaries)
  {
    range = m_truthContainer->GetPrimaryParticleRange();
  }

  gntracks = m_truthContainer->GetNumPrimaryVertexParticles();

  ActsTransformations actsTransformer;

  for (PHG4TruthInfoContainer::ConstIterator iter = range.first;
       iter != range.second; ++iter)
  {
    PHG4Particle *g4particle = iter->second;
    resetTreeValues();

    if (m_scanForEmbedded)
    {
      if (trutheval->get_embed(g4particle) <= 0)
      {
        continue;
      }
    }

    gtrackID = g4particle->get_track_id();
  
    gflavor = g4particle->get_pid();

    std::set<TrkrDefs::cluskey> g4clusters = clustereval->all_clusters_from(g4particle);
    gnmaps = 0;
    gnintt = 0;
    gntpc = 0;
    gnmms = 0;

    for (const auto &g4cluster : g4clusters)
    {
      auto cluster = m_clusterContainer->findCluster(g4cluster);
      std::shared_ptr<TrkrCluster> truthCluster = clustereval->max_truth_cluster_by_energy(g4cluster);

      gclusterkeys.push_back(g4cluster);
      auto global = actsTransformer.getGlobalPosition(cluster, surfmaps, tgeometry);
      if (truthCluster)
      {
        tgclusterx.push_back(truthCluster->getX());
        tgclustery.push_back(truthCluster->getY());
        tgclusterz.push_back(truthCluster->getZ());
      }
      else
      {
        tgclusterx.push_back(NAN);
        tgclustery.push_back(NAN);
        tgclusterz.push_back(NAN);
	}

      gclusterx.push_back(global(0));
      gclustery.push_back(global(1));
      gclusterz.push_back(global(2));
      gclusterrphierr.push_back(cluster->getActsLocalError(0, 0));
      gclusterzerr.push_back(cluster->getActsLocalError(1, 1));

      unsigned int layer = TrkrDefs::getLayer(g4cluster);
      if (layer < 3)
      {
        gnmaps++;
      }
      else if (layer < 7)
      {
        gnintt++;
      }
      else if (layer < 55)
      {
        gntpc++;
      }
      else
      {
        gnmms++;
      }
    }

    gpx = g4particle->get_px();
    gpy = g4particle->get_py();
    gpz = g4particle->get_pz();
    gpt = sqrt(gpx * gpx + gpy * gpy);
    TVector3 gv(gpx, gpy, gpz);
    geta = gv.Eta();
    gphi = gv.Phi();

    PHG4VtxPoint *vtx = trutheval->get_vertex(g4particle);
    gvx = vtx->get_x();
    gvy = vtx->get_y();
    gvz = vtx->get_z();
    gvt = vtx->get_t();

    gembed = trutheval->get_embed(g4particle);
    gprimary = trutheval->is_primary(g4particle);

    auto track = trackeval->best_track_from(g4particle);
    if (track)
    {
      trackID = track->get_id();

      px = track->get_px();
      py = track->get_py();
      pz = track->get_pz();
      quality = track->get_quality();
      TVector3 v(px, py, pz);
      pt = v.Pt();
      phi = v.Phi();
      eta = v.Eta();
      charge = track->get_charge();
      dca3dxy = track->get_dca3d_xy();
      dca3dz = track->get_dca3d_z();
      nmaps = 0;
      nintt = 0;
      ntpc = 0;
      nmms = 0;

      for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
           iter != track->end_cluster_keys();
           ++iter)
      {
        TrkrDefs::cluskey ckey = *iter;
        auto tcluster = m_clusterContainer->findCluster(ckey);
        std::shared_ptr<TrkrCluster> truthCluster = clustereval->max_truth_cluster_by_energy(ckey);
        unsigned int layer = TrkrDefs::getLayer(ckey);
        if (layer < 3)
        {
          nmaps++;
        }
        else if (layer < 7)
        {
          nintt++;
        }
        else if (layer < 55)
        {
          ntpc++;
        }
        else
        {
          nmms++;
        }

        clusterkeys.push_back(ckey);
        auto glob = actsTransformer.getGlobalPosition(tcluster, surfmaps, tgeometry);
	
        if (truthCluster)
        {
          tclusterx.push_back(truthCluster->getX());
          tclustery.push_back(truthCluster->getY());
          tclusterz.push_back(truthCluster->getZ());
        }
        else
        {
          tclusterx.push_back(NAN);
          tclustery.push_back(NAN);
          tclusterz.push_back(NAN);
	  }
        clusterx.push_back(glob(0));
        clustery.push_back(glob(1));
        clusterz.push_back(glob(2));
        clusterrphierr.push_back(tcluster->getActsLocalError(0, 0));
        clusterzerr.push_back(tcluster->getActsLocalError(1, 1));
      }

      pcax = track->get_x();
      pcay = track->get_y();
      pcaz = track->get_z();
    }

    auto matchedTracks = trackeval->all_tracks_from(g4particle);
  
    for(const auto& track : matchedTracks)
      {
	dtrackID = track->get_id();

	dpx = track->get_px();
	dpy = track->get_py();
	dpz = track->get_pz();
	dpt = track->get_pt();
	dpcax = track->get_x();
	dpcay = track->get_y();
	dpcaz = track->get_z();
	ddca3dxy = track->get_dca3d_xy();
	ddca3dz = track->get_dca3d_z();
	TVector3 vec;
	vec.SetXYZ(dpx,dpy,dpz);
	deta = vec.Eta();
	dphi = vec.Phi();
	dcharge = track->get_charge();
	dquality = track->get_quality();
	
	int nmmaps = 0, nmintt=0, nmtpc=0, nmmms=0;
	for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
         iter != track->end_cluster_keys();
         ++iter)
	  {
	    TrkrDefs::cluskey ckey = *iter;
	   
	    auto tcluster = m_clusterContainer->findCluster(ckey);
	    unsigned int layer = TrkrDefs::getLayer(ckey);
	    dclusterkeys.push_back(ckey);
	    auto glob = actsTransformer.getGlobalPosition(tcluster, surfmaps, tgeometry);
	    dclusterx.push_back(glob(0));
	    dclustery.push_back(glob(1));
	    dclusterz.push_back(glob(2));
	    if (layer < 3)
	      { nmmaps++; }
	    else if (layer < 7)
	      { nmintt++; }
	    else if (layer < 55)
	      { nmtpc++; }
	    else
	      { nmmms++; }
	    
	  }

	dnmaps = nmmaps;
	dnintt = nmintt;
	dntpc = nmtpc;
	dnmms = nmmms;
	m_duplicatetree->Fill();

	/// Reset vectors
	dclusterrphierr.clear();
	dclusterzerr.clear();
	dclusterkeys.clear();
	dclusterx.clear();
	dclustery.clear();
	dclusterz.clear();
      }

    m_truthtree->Fill();

  }
}

void TrackClusterEvaluator::processRecoTracks(PHCompositeNode *topNode)
{
  SvtxTruthEval *trutheval = m_svtxevalstack->get_truth_eval();
  SvtxClusterEval *clustereval = m_svtxevalstack->get_cluster_eval();
  SvtxTrackEval *trackeval = m_svtxevalstack->get_track_eval();
  auto surfmaps = findNode::getClass<ActsSurfaceMaps>(topNode, "ActsSurfaceMaps");
  auto tgeometry = findNode::getClass<ActsTrackingGeometry>(topNode, "ActsTrackingGeometry");

  ActsTransformations actsTransformer;

  for (const auto &[key, track] : *m_trackMap)
  {
    resetTreeValues();
    if(Verbosity() > 0)
      track->identify();

    trackID = track->get_id(); 
   
    px = track->get_px();
    py = track->get_py();
    pz = track->get_pz();
    quality = track->get_quality();
    TVector3 v(px, py, pz);
    pt = v.Pt();
    phi = v.Phi();
    eta = v.Eta();
    charge = track->get_charge();
    dca3dxy = track->get_dca3d_xy();
    dca3dz = track->get_dca3d_z();
    nmaps = 0;
    nintt = 0;
    ntpc = 0;
    nmms = 0;

    for (SvtxTrack::ConstClusterKeyIter iter = track->begin_cluster_keys();
         iter != track->end_cluster_keys();
         ++iter)
    {
      TrkrDefs::cluskey ckey = *iter;
      auto tcluster = m_clusterContainer->findCluster(ckey);
      auto truthCluster = clustereval->max_truth_cluster_by_energy(ckey);
      unsigned int layer = TrkrDefs::getLayer(ckey);
      if (layer < 3)
      {
        nmaps++;
      }
      else if (layer < 7)
      {
        nintt++;
      }
      else if (layer < 55)
      {
        ntpc++;
      }
      else
      {
        nmms++;
      }

      clusterkeys.push_back(ckey);
      auto glob = actsTransformer.getGlobalPosition(tcluster, surfmaps, tgeometry);
      clusterx.push_back(glob(0));
      clustery.push_back(glob(1));
      clusterz.push_back(glob(2));
      if (truthCluster)
      {
        tclusterx.push_back(truthCluster->getX());
        tclustery.push_back(truthCluster->getY());
        tclusterz.push_back(truthCluster->getZ());
      }
      else
      {
        tclusterx.push_back(NAN);
        tclustery.push_back(NAN);
        tclusterz.push_back(NAN);
	}
      clusterrphierr.push_back(tcluster->getActsLocalError(0, 0));
      clusterzerr.push_back(tcluster->getActsLocalError(1, 1));
    }

    pcax = track->get_x();
    pcay = track->get_y();
    pcaz = track->get_z();
   
    if (m_trackMatch)
    {
      auto allpart = trackeval->all_truth_particles(track);
 
      PHG4Particle *g4particle = trackeval->max_truth_particle_by_nclusters(track);
   
      if(g4particle) {
	auto matched_track = trackeval->best_track_from(g4particle);

	if(matched_track)
	  {
	    matchedTrackID = matched_track->get_id();
	 
	  }
	
        auto matchedtracks = trackeval->all_tracks_from(g4particle);
	for(const auto ttrack : matchedtracks)
	  {
	    matchedRecoTracksID.push_back(ttrack->get_id());
	  }
	if(matchedtracks.size() > 1)
	  { isDuplicate = 1; }
      }
      if (m_scanForEmbedded)
      {
        if (trutheval->get_embed(g4particle) <= 0) continue;
      }

      gtrackID = g4particle->get_track_id();
      gflavor = g4particle->get_pid();

      std::set<TrkrDefs::cluskey> g4clusters = clustereval->all_clusters_from(g4particle);

      gnmaps = 0;
      gnintt = 0;
      gntpc = 0;
      gnmms = 0;
      for (const auto &g4cluster : g4clusters)
      {
        auto cluster = m_clusterContainer->findCluster(g4cluster);
        gclusterkeys.push_back(g4cluster);
        auto truthCluster = clustereval->max_truth_cluster_by_energy(g4cluster);
        auto global = actsTransformer.getGlobalPosition(cluster, surfmaps, tgeometry);
        gclusterx.push_back(global(0));
        gclustery.push_back(global(1));
        gclusterz.push_back(global(2));
	 if (truthCluster)
        {
          tgclusterx.push_back(truthCluster->getX());
          tgclustery.push_back(truthCluster->getY());
          tgclusterz.push_back(truthCluster->getZ());
        }
        else
        {
          tgclusterx.push_back(NAN);
          tgclustery.push_back(NAN);
          tgclusterz.push_back(NAN);
	  }
        gclusterrphierr.push_back(cluster->getActsLocalError(0, 0));
        gclusterzerr.push_back(cluster->getActsLocalError(1, 1));

        unsigned int layer = TrkrDefs::getLayer(g4cluster);
        if (layer < 3)
        {
          gnmaps++;
        }
        else if (layer < 7)
        {
          gnintt++;
        }
        else if (layer < 55)
        {
          gntpc++;
        }
        else
        {
          gnmms++;
        }
      }
   
      gpx = g4particle->get_px();
      gpy = g4particle->get_py();
      gpz = g4particle->get_pz();

      TVector3 tpart(gpx, gpy, gpz);
      gpt = tpart.Pt();
      geta = tpart.Eta();
      gphi = tpart.Phi();

      PHG4VtxPoint *vtx = trutheval->get_vertex(g4particle);
      gvx = vtx->get_x();
      gvy = vtx->get_y();
      gvz = vtx->get_z();
      gvt = vtx->get_t();

      gembed = trutheval->get_embed(g4particle);
      gprimary = trutheval->is_primary(g4particle);
    }
    m_recotree->Fill();
  }
}

void TrackClusterEvaluator::clearVectors()
{
  matchedRecoTracksID.clear();
  dclusterkeys.clear();
  dclusterx.clear();
  dclustery.clear();
  dclusterz.clear();
  dclusterrphierr.clear();
  dclusterzerr.clear();
  tgclusterx.clear();
  tgclustery.clear();
  tgclusterz.clear();
  tclusterx.clear();
  tclustery.clear();
  tclusterz.clear();
  gclusterkeys.clear();
  gclusterx.clear();
  gclustery.clear();
  gclusterz.clear();
  gclusterrphierr.clear();
  gclusterzerr.clear();
  clusterkeys.clear();
  clusterx.clear();
  clustery.clear();
  clusterz.clear();
  clusterrphierr.clear();
  clusterzerr.clear();
}

//____________________________________________________________________________..
int TrackClusterEvaluator::ResetEvent(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TrackClusterEvaluator::EndRun(const int runnumber)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TrackClusterEvaluator::End(PHCompositeNode *topNode)
{
  m_outfile->cd();
  m_truthtree->Write();
  m_recotree->Write();
  m_outfile->Write();
  m_outfile->Close();

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int TrackClusterEvaluator::Reset(PHCompositeNode *topNode)
{
  return Fun4AllReturnCodes::EVENT_OK;
}

int TrackClusterEvaluator::getNodes(PHCompositeNode *topNode)
{
  m_truthContainer = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
  if (!m_truthContainer)
  {
    std::cout << "No truth info available, can't continue." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_trackMap = findNode::getClass<SvtxTrackMap>(topNode, m_trackMapName);
  std::cout << "Accessing map name " << m_trackMapName << std::endl;
  if (!m_trackMap)
  {
    std::cout << "No track map available, can't continue. " << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  m_clusterContainer = findNode::getClass<TrkrClusterContainer>(topNode, "TRKR_CLUSTER");
  if (!m_clusterContainer)
  {
    std::cout << "No cluster container available, can't continue." << std::endl;
    return Fun4AllReturnCodes::ABORTEVENT;
  }

  return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void TrackClusterEvaluator::Print(const std::string &what) const
{
  std::cout << "TrackClusterEvaluator::Print(const std::string &what) const Printing info for " << what << std::endl;
}

void TrackClusterEvaluator::setupTrees()
{
  m_recotree = new TTree("recotracks", "a tree with reconstructed tracks");
  m_truthtree = new TTree("truthtracks", "a tree with truth tracks");
  m_duplicatetree = new TTree("duplicatetracks","a tree with truth->reco-duplicates");
  
  m_duplicatetree->Branch("event", &event, "event/I");
  m_duplicatetree->Branch("gntracks", &gntracks, "gntracks/I");
  m_duplicatetree->Branch("gtrackID", &gtrackID, "gtrackID/I");
  m_duplicatetree->Branch("gflavor", &gflavor, "gflavor/I");
  m_duplicatetree->Branch("gnmaps", &gnmaps, "gnmaps/I");
  m_duplicatetree->Branch("gnintt", &gnintt, "gnintt/I");
  m_duplicatetree->Branch("gntpc", &gntpc, "gntpc/I");
  m_duplicatetree->Branch("gnmms", &gnmms, "gnmms/I");
  m_duplicatetree->Branch("gpx", &gpx, "gpx/F");
  m_duplicatetree->Branch("gpy", &gpy, "gpy/F");
  m_duplicatetree->Branch("gpz", &gpz, "gpz/F");
  m_duplicatetree->Branch("gpt", &gpt, "gpt/F");
  m_duplicatetree->Branch("geta", &geta, "geta/F");
  m_duplicatetree->Branch("gphi", &gphi, "gphi/F");
  m_duplicatetree->Branch("gvx", &gvx, "gvx/F");
  m_duplicatetree->Branch("gvy", &gvy, "gvy/F");
  m_duplicatetree->Branch("gvz", &gvz, "gvz/F");
  m_duplicatetree->Branch("gvt", &gvt, "gvt/F");
  m_duplicatetree->Branch("gembed", &gembed, "gembed/I");
  m_duplicatetree->Branch("gprimary", &gprimary, "gprimary/I");
  m_duplicatetree->Branch("gclusterkeys", &gclusterkeys);
  m_duplicatetree->Branch("gclusterx", &gclusterx);
  m_duplicatetree->Branch("gclustery", &gclustery);
  m_duplicatetree->Branch("gclusterz", &gclusterz);
  m_duplicatetree->Branch("tgclusterx", &tgclusterx);
  m_duplicatetree->Branch("tgclustery", &tgclustery);
  m_duplicatetree->Branch("tgclusterz", &tgclusterz);
  m_duplicatetree->Branch("gclusterrphierr", &gclusterrphierr);
  m_duplicatetree->Branch("gclusterzerr", &gclusterzerr);
  m_duplicatetree->Branch("dtrackID", &dtrackID, "dtrackID/I");
  m_duplicatetree->Branch("dpx", &dpx, "dpx/F");
  m_duplicatetree->Branch("dpy", &dpy, "dpy/F");
  m_duplicatetree->Branch("dpz", &dpz, "dpz/F");
  m_duplicatetree->Branch("dpt", &dpt, "dpt/F");
  m_duplicatetree->Branch("deta", &deta, "deta/F");
  m_duplicatetree->Branch("dphi", &dphi, "dphi/F");
  m_duplicatetree->Branch("dcharge", &dcharge, "dcharge/I");
  m_duplicatetree->Branch("dquality", &dquality, "dquality/F");
  m_duplicatetree->Branch("dnmaps", &dnmaps, "dnmaps/I");
  m_duplicatetree->Branch("dnintt", &dnintt, "dnintt/I");
  m_duplicatetree->Branch("dntpc", &dntpc, "dntpc/I");
  m_duplicatetree->Branch("dnmms", &dnmms, "dnmms/I");
  m_duplicatetree->Branch("ddca3dxy", &ddca3dxy, "ddca3dxy/F");
  m_duplicatetree->Branch("ddca3dz", &ddca3dz, "ddca3dz/F");
  m_duplicatetree->Branch("dpcax", &dpcax, "dpcax/F");
  m_duplicatetree->Branch("dpcay", &dpcay, "dpcay/F");
  m_duplicatetree->Branch("dpcaz", &dpcaz, "dpcaz/F");
  m_duplicatetree->Branch("dclusterkeys", &dclusterkeys);
  m_duplicatetree->Branch("dclusterx", &dclusterx);
  m_duplicatetree->Branch("dclustery", &dclustery);
  m_duplicatetree->Branch("dclusterz", &dclusterz);
  m_duplicatetree->Branch("dclusterrphierr", &dclusterrphierr);
  m_duplicatetree->Branch("dclusterzerr", &dclusterzerr);


  m_truthtree->Branch("event", &event, "event/I");
  m_truthtree->Branch("gntracks", &gntracks, "gntracks/I");
  m_truthtree->Branch("gtrackID", &gtrackID, "gtrackID/I");
  m_truthtree->Branch("gflavor", &gflavor, "gflavor/I");
  m_truthtree->Branch("gnmaps", &gnmaps, "gnmaps/I");
  m_truthtree->Branch("gnintt", &gnintt, "gnintt/I");
  m_truthtree->Branch("gntpc", &gntpc, "gntpc/I");
  m_truthtree->Branch("gnmms", &gnmms, "gnmms/I");
  m_truthtree->Branch("gpx", &gpx, "gpx/F");
  m_truthtree->Branch("gpy", &gpy, "gpy/F");
  m_truthtree->Branch("gpz", &gpz, "gpz/F");
  m_truthtree->Branch("gpt", &gpt, "gpt/F");
  m_truthtree->Branch("geta", &geta, "geta/F");
  m_truthtree->Branch("gphi", &gphi, "gphi/F");
  m_truthtree->Branch("gvx", &gvx, "gvx/F");
  m_truthtree->Branch("gvy", &gvy, "gvy/F");
  m_truthtree->Branch("gvz", &gvz, "gvz/F");
  m_truthtree->Branch("gvt", &gvt, "gvt/F");
  m_truthtree->Branch("gembed", &gembed, "gembed/I");
  m_truthtree->Branch("gprimary", &gprimary, "gprimary/I");
  m_truthtree->Branch("gclusterkeys", &gclusterkeys);
  m_truthtree->Branch("gclusterx", &gclusterx);
  m_truthtree->Branch("gclustery", &gclustery);
  m_truthtree->Branch("gclusterz", &gclusterz);
  m_truthtree->Branch("tgclusterx", &tgclusterx);
  m_truthtree->Branch("tgclustery", &tgclustery);
  m_truthtree->Branch("tgclusterz", &tgclusterz);
  m_truthtree->Branch("gclusterrphierr", &gclusterrphierr);
  m_truthtree->Branch("gclusterzerr", &gclusterzerr);
  m_truthtree->Branch("trackID", &trackID, "trackID/I");
  m_truthtree->Branch("px", &px, "px/F");
  m_truthtree->Branch("py", &py, "py/F");
  m_truthtree->Branch("pz", &pz, "pz/F");
  m_truthtree->Branch("pt", &pt, "pt/F");
  m_truthtree->Branch("eta", &eta, "eta/F");
  m_truthtree->Branch("phi", &phi, "phi/F");
  m_truthtree->Branch("charge", &charge, "charge/I");
  m_truthtree->Branch("quality", &quality, "quality/F");
  m_truthtree->Branch("nmaps", &nmaps, "nmaps/I");
  m_truthtree->Branch("nintt", &nintt, "nintt/I");
  m_truthtree->Branch("ntpc", &ntpc, "ntpc/I");
  m_truthtree->Branch("nmms", &nmms, "nmms/I");
  m_truthtree->Branch("dca3dxy", &dca3dxy, "dca3dxy/F");
  m_truthtree->Branch("dca3dz", &dca3dz, "dca3dz/F");
  m_truthtree->Branch("pcax", &pcax, "pcax/F");
  m_truthtree->Branch("pcay", &pcay, "pcay/F");
  m_truthtree->Branch("pcaz", &pcaz, "pcaz/F");
  m_truthtree->Branch("clusterkeys", &clusterkeys);
  m_truthtree->Branch("clusterx", &clusterx);
  m_truthtree->Branch("clustery", &clustery);
  m_truthtree->Branch("clusterz", &clusterz);
  m_truthtree->Branch("tclusterx", &tclusterx);
  m_truthtree->Branch("tclustery", &tclustery);
  m_truthtree->Branch("tclusterz", &tclusterz);
  m_truthtree->Branch("clusterrphierr", &clusterrphierr);
  m_truthtree->Branch("clusterzerr", &clusterzerr);

  m_recotree->Branch("event", &event, "event/I");
  m_recotree->Branch("trackID", &trackID, "trackID/I");
  m_recotree->Branch("isDuplicate",&isDuplicate, "isDuplicate/I");
  m_recotree->Branch("matchedRecoTracksID", &matchedRecoTracksID);
  m_recotree->Branch("px", &px, "px/F");
  m_recotree->Branch("py", &py, "py/F");
  m_recotree->Branch("pz", &pz, "pz/F");
  m_recotree->Branch("pt", &pt, "pt/F");
  m_recotree->Branch("eta", &eta, "eta/F");
  m_recotree->Branch("phi", &phi, "phi/F");
  m_recotree->Branch("charge", &charge, "charge/I");
  m_recotree->Branch("quality", &quality, "quality/F");
  m_recotree->Branch("nmaps", &nmaps, "nmaps/I");
  m_recotree->Branch("nintt", &nintt, "nintt/I");
  m_recotree->Branch("ntpc", &ntpc, "ntpc/I");
  m_recotree->Branch("nmms", &nmms, "nmms/I");
  m_recotree->Branch("dca3dxy", &dca3dxy, "dca3dxy/F");
  m_recotree->Branch("dca3dz", &dca3dz, "dca3dz/F");
  m_recotree->Branch("pcax", &pcax, "pcax/F");
  m_recotree->Branch("pcay", &pcay, "pcay/F");
  m_recotree->Branch("pcaz", &pcaz, "pcaz/F");
  m_recotree->Branch("clusterkeys", &clusterkeys);
  m_recotree->Branch("clusterx", &clusterx);
  m_recotree->Branch("clustery", &clustery);
  m_recotree->Branch("clusterz", &clusterz);
  m_recotree->Branch("tclusterx", &tclusterx);
  m_recotree->Branch("tclustery", &tclustery);
  m_recotree->Branch("tclusterz", &tclusterz);
  m_recotree->Branch("clusterrphierr", &clusterrphierr);
  m_recotree->Branch("clusterzerr", &clusterzerr);
  m_recotree->Branch("matchedTrackID",&matchedTrackID, "matchedTrackID/I");
  m_recotree->Branch("gntracks", &gntracks, "gntracks/I");
  m_recotree->Branch("gtrackID", &gtrackID, "gtrackID/I");
  m_recotree->Branch("gnmaps", &gnmaps, "gnmaps/I");
  m_recotree->Branch("gnintt", &gnintt, "gnintt/I");
  m_recotree->Branch("gntpc", &gntpc, "gntpc/I");
  m_recotree->Branch("gnmms", &gnmms, "gnmms/I");
  m_recotree->Branch("gpx", &gpx, "gpx/F");
  m_recotree->Branch("gpy", &gpy, "gpy/F");
  m_recotree->Branch("gpz", &gpz, "gpz/F");
  m_recotree->Branch("gpt", &gpt, "gpt/F");
  m_recotree->Branch("geta", &geta, "geta/F");
  m_recotree->Branch("gphi", &gphi, "gphi/F");
  m_recotree->Branch("gvx", &gvx, "gvx/F");
  m_recotree->Branch("gvy", &gvy, "gvy/F");
  m_recotree->Branch("gvz", &gvz, "gvz/F");
  m_recotree->Branch("gvt", &gvt, "gvt/F");
  m_recotree->Branch("gembed", &gembed, "gembed/I");
  m_recotree->Branch("gprimary", &gprimary, "gprimary/I");
  m_recotree->Branch("gclusterkeys", &gclusterkeys);
  m_recotree->Branch("gclusterx", &gclusterx);
  m_recotree->Branch("gclustery", &gclustery);
  m_recotree->Branch("gclusterz", &gclusterz);
  m_recotree->Branch("tgclusterx", &tgclusterx);
  m_recotree->Branch("tgclustery", &tgclustery);
  m_recotree->Branch("tgclusterz", &tgclusterz);
  m_recotree->Branch("gclusterrphierr", &gclusterrphierr);
  m_recotree->Branch("gclusterzerr", &gclusterzerr);
}

void TrackClusterEvaluator::resetTreeValues()
{
  clearVectors();
  matchedTrackID = -9999;
  isDuplicate = -9999;
  gflavor = -9999;
  gntracks = -9999;
  gtrackID = -9999;
  gnmaps = -9999;
  gnintt = -9999;
  gntpc = -9999;
  gnmms = -9999;
  gpx = -9999;
  gpy = -9999;
  gpz = -9999;
  gpt = -9999;
  geta = -9999;
  gphi = -9999;
  gvx = -9999;
  gvy = -9999;
  gvz = -9999;
  gvt = -9999;
  gembed = -9999;
  gprimary = -9999;
  trackID = -9999;
  px = -9999;
  py = -9999;
  pz = -9999;
  pt = -9999;
  eta = -9999;
  phi = -9999;
  charge = -9999;
  quality = -9999;
  nmaps = -9999;
  nintt = -9999;
  ntpc = -9999;
  nmms = -9999;
  dca3dxy = -9999;
  dca3dz = -9999;
  pcax = -9999;
  pcay = -9999;
  pcaz = -9999;
  dtrackID = -9999;
  dpx = -9999;
  dpy = -9999;
  dpz = -9999;
  dpt = -9999;
  deta = -9999;
  dphi = -9999;
  dcharge = -9999;
  dquality = -9999;
  dnmaps = -9999;
  dnintt = -9999;
  dntpc = -9999;
  dnmms = -9999;
  ddca3dxy = -9999;
  ddca3dz = -9999;
  dpcax = -9999;
  dpcay = -9999;
  dpcaz = -9999;
}
