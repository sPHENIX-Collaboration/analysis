//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in VertexCompare.h.
//
// VertexCompare(const std::string &name = "VertexCompare")
// everything is keyed to VertexCompare, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// VertexCompare::~VertexCompare()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int VertexCompare::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int VertexCompare::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int VertexCompare::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int VertexCompare::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int VertexCompare::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int VertexCompare::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int VertexCompare::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void VertexCompare::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "VertexCompare.h"

#include <fun4all/Fun4AllReturnCodes.h>

#include <phool/PHCompositeNode.h>
#include <phool/getClass.h>
#include <phool/sphenix_constants.h>

#include <trackbase/ActsGeometry.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterContainer.h>
#include <trackbase/TrkrClusterCrossingAssoc.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxPHG4ParticleMap.h>
#include <trackbase_historic/TrackAnalysisUtils.h>
#include <trackbase_historic/TrackSeed.h>
#include <trackbase_historic/TrackSeedContainer.h>
#include <trackbase_historic/TrackSeedHelper.h>

#include <g4detectors/PHG4TpcGeom.h>
#include <g4detectors/PHG4TpcGeomContainer.h>

#include <mvtx/SegmentationAlpide.h>
#include <mvtx/MvtxPixelDefs.h>

#include <trackbase/InttDefs.h>
#include <trackbase/MvtxDefs.h>
#include <trackbase/TrkrDefs.h>

#include <ffarawobjects/Gl1Packet.h>
#include <ffarawobjects/Gl1RawHit.h>

#include <globalvertex/GlobalVertex.h>
#include <globalvertex/GlobalVertexMap.h>
#include <globalvertex/MbdVertex.h>
#include <globalvertex/MbdVertexMap.h>
#include <globalvertex/SvtxVertex.h>
#include <globalvertex/SvtxVertexMap.h>

#include <mbd/MbdOut.h>
#include <mbd/MbdPmtContainer.h>
#include <mbd/MbdPmtHit.h>

#include <calotrigger/MinimumBiasInfo.h>
#include <centrality/CentralityInfo.h>

#include "g4eval/SvtxClusterEval.h"
#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxHitEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4HitContainer.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4MCProcessDefs.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <map>

#include <TDatabasePDG.h>
#include <TParticlePDG.h>

namespace
{
template <class Container> void Clean(Container &c) { Container().swap(c); }
} // namespace

GlobalVertex::VTXTYPE trkType = GlobalVertex::SVTX;
GlobalVertex::VTXTYPE mbdType = GlobalVertex::MBD;
//____________________________________________________________________________..
VertexCompare::VertexCompare(const std::string &name)
    : SubsysReco(name)
{
    std::cout << "VertexCompare::VertexCompare(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
VertexCompare::~VertexCompare()
{
    delete svtx_evalstack;
    svtx_evalstack = nullptr;
    clustereval = nullptr;
    hiteval = nullptr;
    truth_eval = nullptr;

    if (outFile)
    {
        if (outFile->IsOpen())
        {
            outFile->Close();
        }
        delete outFile;
        outFile = nullptr;
        outTree = nullptr;
    }

    std::cout << "VertexCompare::~VertexCompare() Calling dtor" << std::endl;
}

//____________________________________________________________________________..
int VertexCompare::Init(PHCompositeNode *topNode)
{
    outFile = new TFile(outFileName.c_str(), "RECREATE");
    outTree = new TTree("VTX", "VTX");
    outTree->SetAutoFlush(-10 * 1024 * 1024);
    outTree->SetAutoSave(-100 * 1024 * 1024);
    outTree->SetMaxVirtualSize(64 * 1024 * 1024);

    outTree->Branch("counter", &counter, "counter/I");
    outTree->Branch("is_min_bias", &is_min_bias);
    outTree->Branch("firedTriggers", &firedTriggers);
    outTree->Branch("gl1bco", &gl1bco);
    outTree->Branch("gl1BunchCrossing", &gl1BunchCrossing);
    outTree->Branch("bcotr", &bcotr);
    outTree->Branch("centrality_mbd", &centrality_mbd_);
    outTree->Branch("n_MBDVertex", &n_MBDVertex, "n_MBDVertex/i");
    outTree->Branch("mbdVertex", &mbdVertex);
    outTree->Branch("mbdVertexId", &mbdVertexId);
    outTree->Branch("mbdVertexCrossing", &mbdVertexCrossing);
    outTree->Branch("MBD_charge_sum", &MBD_charge_sum);
    outTree->Branch("nSvtxVertices", &nSvtxVertices);
    outTree->Branch("nSvtxVertices_validCrossing", &nSvtxVertices_validCrossing);
    outTree->Branch("trackerVertexId", &trackerVertexId);
    outTree->Branch("trackerVertexX", &trackerVertexX);
    outTree->Branch("trackerVertexY", &trackerVertexY);
    outTree->Branch("trackerVertexZ", &trackerVertexZ);
    outTree->Branch("trackerVertexChisq", &trackerVertexChisq);
    outTree->Branch("trackerVertexNdof", &trackerVertexNdof);
    outTree->Branch("trackerVertexNTracks", &trackerVertexNTracks);
    outTree->Branch("trackerVertexCrossing", &trackerVertexCrossing);
    outTree->Branch("trackerVertexTrackIDs", &trackerVertexTrackIDs);
    outTree->Branch("nTracks", &nTracks, "nTracks/i");
    // outTree->Branch("n_TRKVertex", &n_TRKVertex, "n_TRKVertex/i");
    outTree->Branch("hasMBD", &hasMBD, "hasMBD/O");
    outTree->Branch("hasTRK", &hasTRK, "hasTRK/O");

    if (writeTrackBranches_)
    {
        outTree->Branch("nRecoTracks", &nRecoTracks);
        outTree->Branch("track_deltapt", &track_deltapt);
        outTree->Branch("track_deltaeta", &track_deltaeta);
        outTree->Branch("track_deltaphi", &track_deltaphi);
        outTree->Branch("track_nhits", &track_nhits);
        outTree->Branch("track_nmaps", &track_nmaps);
        outTree->Branch("track_nintt", &track_nintt);
        outTree->Branch("track_ntpc", &track_ntpc);
        outTree->Branch("track_nmms", &track_nmms);
        outTree->Branch("track_ntpc1", &track_ntpc1);
        outTree->Branch("track_ntpc11", &track_ntpc11);
        outTree->Branch("track_ntpc2", &track_ntpc2);
        outTree->Branch("track_ntpc3", &track_ntpc3);
        outTree->Branch("track_pidedx", &track_pidedx);
        outTree->Branch("track_kdedx", &track_kdedx);
        outTree->Branch("track_prdedx", &track_prdedx);
        outTree->Branch("track_vx", &track_vx);
        outTree->Branch("track_vy", &track_vy);
        outTree->Branch("track_vz", &track_vz);
        outTree->Branch("track_dca2d", &track_dca2d);
        outTree->Branch("track_dca2dsigma", &track_dca2dsigma);
        outTree->Branch("track_dca3dxy", &track_dca3dxy);
        outTree->Branch("track_dca3dxysigma", &track_dca3dxysigma);
        outTree->Branch("track_dca3dz", &track_dca3dz);
        outTree->Branch("track_dca3dzsigma", &track_dca3dzsigma);
        outTree->Branch("track_hlxpt", &track_hlxpt);
        outTree->Branch("track_hlxeta", &track_hlxeta);
        outTree->Branch("track_hlxphi", &track_hlxphi);
        outTree->Branch("track_hlxX0", &track_hlxX0);
        outTree->Branch("track_hlxY0", &track_hlxY0);
        outTree->Branch("track_hlxZ0", &track_hlxZ0);
        outTree->Branch("track_hlxcharge", &track_hlxcharge);
        outTree->Branch("track_id", &track_id);
        outTree->Branch("track_x", &track_x);
        outTree->Branch("track_y", &track_y);
        outTree->Branch("track_z", &track_z);
        outTree->Branch("track_px", &track_px);
        outTree->Branch("track_py", &track_py);
        outTree->Branch("track_pz", &track_pz);
        outTree->Branch("track_pt", &track_pt);
        outTree->Branch("track_eta", &track_eta);
        outTree->Branch("track_phi", &track_phi);
        outTree->Branch("track_dedx", &track_dedx);
        outTree->Branch("track_charge", &track_charge);
        outTree->Branch("track_crossing", &track_crossing);
        outTree->Branch("track_vertex_id", &track_vertex_id);
        outTree->Branch("track_chisq", &track_chisq);
        outTree->Branch("track_ndf", &track_ndf);
        outTree->Branch("track_quality", &track_quality);
        outTree->Branch("track_silseed_id", &track_silseed_id);
        outTree->Branch("track_silseed_x", &track_silseed_x);
        outTree->Branch("track_silseed_y", &track_silseed_y);
        outTree->Branch("track_silseed_z", &track_silseed_z);
        outTree->Branch("track_silseed_pt", &track_silseed_pt);
        outTree->Branch("track_silseed_eta", &track_silseed_eta);
        outTree->Branch("track_silseed_phi", &track_silseed_phi);
        outTree->Branch("track_silseed_crossing", &track_silseed_crossing);
        outTree->Branch("track_silseed_charge", &track_silseed_charge);
        outTree->Branch("track_silseed_nMvtx", &track_silseed_nMvtx);
        outTree->Branch("track_silseed_nIntt", &track_silseed_nIntt);
        outTree->Branch("track_silseed_clusterKeys", &track_silseed_clusterKeys);
        outTree->Branch("track_cluster_layer", &track_cluster_layer);
        outTree->Branch("track_cluster_globalX", &track_cluster_globalX);
        outTree->Branch("track_cluster_globalY", &track_cluster_globalY);
        outTree->Branch("track_cluster_globalZ", &track_cluster_globalZ);
        outTree->Branch("track_cluster_phi", &track_cluster_phi);
        outTree->Branch("track_cluster_eta", &track_cluster_eta);
        outTree->Branch("track_cluster_r", &track_cluster_r);
    }

    // silicon seed information
    outTree->Branch("nTotalSilSeeds", &nTotalSilSeeds);
    outTree->Branch("nSilSeedsValidCrossing", &nSilSeedsValidCrossing);
    outTree->Branch("silseed_id", &silseed_id);
    outTree->Branch("silseed_assocVtxId", &silseed_assocVtxId);
    outTree->Branch("silseed_x", &silseed_x);
    outTree->Branch("silseed_y", &silseed_y);
    outTree->Branch("silseed_z", &silseed_z);
    outTree->Branch("silseed_pt", &silseed_pt);
    outTree->Branch("silseed_eta", &silseed_eta);
    outTree->Branch("silseed_phi", &silseed_phi);
    outTree->Branch("silseed_eta_vtx", &silseed_eta_vtx);
    outTree->Branch("silseed_phi_vtx", &silseed_phi_vtx);
    outTree->Branch("silseed_crossing", &silseed_crossing);
    outTree->Branch("silseed_charge", &silseed_charge);
    outTree->Branch("silseed_nMvtx", &silseed_nMvtx);
    outTree->Branch("silseed_nIntt", &silseed_nIntt);
    outTree->Branch("silseed_clusterKeys", &silseed_clusterKeys);
    outTree->Branch("silseed_cluster_layer", &silseed_cluster_layer);
    outTree->Branch("silseed_cluster_globalX", &silseed_cluster_globalX);
    outTree->Branch("silseed_cluster_globalY", &silseed_cluster_globalY);
    outTree->Branch("silseed_cluster_globalZ", &silseed_cluster_globalZ);
    outTree->Branch("silseed_cluster_phi", &silseed_cluster_phi);
    outTree->Branch("silseed_cluster_eta", &silseed_cluster_eta);
    outTree->Branch("silseed_cluster_r", &silseed_cluster_r);
    outTree->Branch("silseed_cluster_phiSize", &silseed_cluster_phiSize);
    outTree->Branch("silseed_cluster_zSize", &silseed_cluster_zSize);
    outTree->Branch("silseed_cluster_strobeID", &silseed_cluster_strobeID);
    outTree->Branch("silseed_cluster_timeBucketID", &silseed_cluster_timeBucketID); // only for INTT

    if (writeTpcSeedBranches_)
    {
        outTree->Branch("nTotalTpcSeeds", &nTotalTpcSeeds);
        outTree->Branch("tpcseed_id", &tpcseed_id);
        outTree->Branch("tpcseed_x", &tpcseed_x);
        outTree->Branch("tpcseed_y", &tpcseed_y);
        outTree->Branch("tpcseed_z", &tpcseed_z);
        outTree->Branch("tpcseed_pt", &tpcseed_pt);
        outTree->Branch("tpcseed_eta", &tpcseed_eta);
        outTree->Branch("tpcseed_phi", &tpcseed_phi);
        outTree->Branch("tpcseed_crossing", &tpcseed_crossing);
        outTree->Branch("tpcseed_crossing_estimate", &tpcseed_crossing_estimate);
        outTree->Branch("tpcseed_charge", &tpcseed_charge);
        outTree->Branch("tpcseed_nTpc", &tpcseed_nTpc);
        outTree->Branch("tpcseed_nMms", &tpcseed_nMms);
        outTree->Branch("tpcseed_dedx", &tpcseed_dedx);
        outTree->Branch("tpcseed_clusterKeys", &tpcseed_clusterKeys);
        outTree->Branch("tpcseed_cluster_layer", &tpcseed_cluster_layer);
        outTree->Branch("tpcseed_cluster_globalX", &tpcseed_cluster_globalX);
        outTree->Branch("tpcseed_cluster_globalY", &tpcseed_cluster_globalY);
        outTree->Branch("tpcseed_cluster_globalZ", &tpcseed_cluster_globalZ);
        outTree->Branch("tpcseed_cluster_phi", &tpcseed_cluster_phi);
        outTree->Branch("tpcseed_cluster_eta", &tpcseed_cluster_eta);
        outTree->Branch("tpcseed_cluster_r", &tpcseed_cluster_r);
    }

    // (intt) cluster information
    outTree->Branch("clusterKey", &clusterKey);
    outTree->Branch("cluster_layer", &cluster_layer);
    outTree->Branch("cluster_chip", &cluster_chip);   // for mvtx chip id
    outTree->Branch("cluster_stave", &cluster_stave); // for mvtx stave id
    outTree->Branch("cluster_globalX", &cluster_globalX);
    outTree->Branch("cluster_globalY", &cluster_globalY);
    outTree->Branch("cluster_globalZ", &cluster_globalZ);
    outTree->Branch("cluster_phi", &cluster_phi);
    outTree->Branch("cluster_eta", &cluster_eta);
    outTree->Branch("cluster_r", &cluster_r);
    outTree->Branch("cluster_phiSize", &cluster_phiSize);
    outTree->Branch("cluster_zSize", &cluster_zSize);
    outTree->Branch("cluster_adc", &cluster_adc);
    outTree->Branch("cluster_timeBucketID", &cluster_timeBucketID);
    outTree->Branch("cluster_crossing", &cluster_crossing);
    outTree->Branch("cluster_ladderZId", &cluster_ladderZId);     // for intt ladder z id
    outTree->Branch("cluster_ladderPhiId", &cluster_ladderPhiId); // for intt ladder phi id
    outTree->Branch("cluster_LocalX", &cluster_LocalX);
    outTree->Branch("cluster_LocalY", &cluster_LocalY);
    // cluster truth matching by max_truth_particle_by_energy
    outTree->Branch("cluster_matchedG4P_trackID", &cluster_matchedG4P_trackID);
    outTree->Branch("cluster_matchedG4P_PID", &cluster_matchedG4P_PID);
    outTree->Branch("cluster_matchedG4P_E", &cluster_matchedG4P_E);
    outTree->Branch("cluster_matchedG4P_pT", &cluster_matchedG4P_pT);
    outTree->Branch("cluster_matchedG4P_eta", &cluster_matchedG4P_eta);
    outTree->Branch("cluster_matchedG4P_phi", &cluster_matchedG4P_phi);

    outTree->Branch("mvtx_seedcluster_key", &mvtx_seedcluster_key);
    outTree->Branch("mvtx_seedcluster_layer", &mvtx_seedcluster_layer);
    outTree->Branch("mvtx_seedcluster_chip", &mvtx_seedcluster_chip);
    outTree->Branch("mvtx_seedcluster_stave", &mvtx_seedcluster_stave);
    outTree->Branch("mvtx_seedcluster_globalX", &mvtx_seedcluster_globalX);
    outTree->Branch("mvtx_seedcluster_globalY", &mvtx_seedcluster_globalY);
    outTree->Branch("mvtx_seedcluster_globalZ", &mvtx_seedcluster_globalZ);
    outTree->Branch("mvtx_seedcluster_phi", &mvtx_seedcluster_phi);
    outTree->Branch("mvtx_seedcluster_eta", &mvtx_seedcluster_eta);
    outTree->Branch("mvtx_seedcluster_r", &mvtx_seedcluster_r);
    outTree->Branch("mvtx_seedcluster_phiSize", &mvtx_seedcluster_phiSize);
    outTree->Branch("mvtx_seedcluster_zSize", &mvtx_seedcluster_zSize);
    outTree->Branch("mvtx_seedcluster_strobeID", &mvtx_seedcluster_strobeID);
    outTree->Branch("mvtx_seedcluster_matchedcrossing", &mvtx_seedcluster_matchedcrossing);
    outTree->Branch("mvtx_seedcluster_hitX", &mvtx_seedcluster_hitX);
    outTree->Branch("mvtx_seedcluster_hitY", &mvtx_seedcluster_hitY);
    outTree->Branch("mvtx_seedcluster_hitZ", &mvtx_seedcluster_hitZ);
    outTree->Branch("mvtx_seedcluster_hitrow", &mvtx_seedcluster_hitrow);
    outTree->Branch("mvtx_seedcluster_hitcol", &mvtx_seedcluster_hitcol);

    if (isSimulation)
    {
        outTree->Branch("nTruthVertex", &nTruthVertex);
        outTree->Branch("TruthVertex_isEmbeded", &TruthVertex_isEmbeded);
        outTree->Branch("TruthVertexX", &TruthVertexX);
        outTree->Branch("TruthVertexY", &TruthVertexY);
        outTree->Branch("TruthVertexZ", &TruthVertexZ);
        outTree->Branch("TruthVertexT", &TruthVertexT);
        outTree->Branch("TruthVertex_crossing", &TruthVertex_crossing);

        outTree->Branch("silseed_ngmvtx", &silseed_ngmvtx);
        outTree->Branch("silseed_ngintt", &silseed_ngintt);
        outTree->Branch("silseed_cluster_gcluster_key", &silseed_cluster_gcluster_key);
        outTree->Branch("silseed_cluster_gcluster_layer", &silseed_cluster_gcluster_layer);
        outTree->Branch("silseed_cluster_gcluster_X", &silseed_cluster_gcluster_X);
        outTree->Branch("silseed_cluster_gcluster_Y", &silseed_cluster_gcluster_Y);
        outTree->Branch("silseed_cluster_gcluster_Z", &silseed_cluster_gcluster_Z);
        outTree->Branch("silseed_cluster_gcluster_r", &silseed_cluster_gcluster_r);
        outTree->Branch("silseed_cluster_gcluster_phi", &silseed_cluster_gcluster_phi);
        outTree->Branch("silseed_cluster_gcluster_eta", &silseed_cluster_gcluster_eta);
        outTree->Branch("silseed_cluster_gcluster_edep", &silseed_cluster_gcluster_edep);
        outTree->Branch("silseed_cluster_gcluster_adc", &silseed_cluster_gcluster_adc);
        outTree->Branch("silseed_cluster_gcluster_phiSize", &silseed_cluster_gcluster_phiSize);
        outTree->Branch("silseed_cluster_gcluster_zSize", &silseed_cluster_gcluster_zSize);
        outTree->Branch("hasSvtxPHG4ParticleMap", &hasSvtxPHG4ParticleMap);
        outTree->Branch("svtxPHG4ParticleMapProcessed", &svtxPHG4ParticleMapProcessed);
        outTree->Branch("silseed_f4a_nMatched", &silseed_f4a_nMatched);
        outTree->Branch("silseed_f4a_truthTrackID", &silseed_f4a_truthTrackID);
        outTree->Branch("silseed_f4a_truthWeight", &silseed_f4a_truthWeight);
        outTree->Branch("silseed_f4a_bestTrackID", &silseed_f4a_bestTrackID);
        outTree->Branch("silseed_f4a_bestWeight", &silseed_f4a_bestWeight);
        outTree->Branch("silseed_f4a_bestG4P_PID", &silseed_f4a_bestG4P_PID);
        outTree->Branch("silseed_f4a_bestG4P_E", &silseed_f4a_bestG4P_E);
        outTree->Branch("silseed_f4a_bestG4P_pT", &silseed_f4a_bestG4P_pT);
        outTree->Branch("silseed_f4a_bestG4P_eta", &silseed_f4a_bestG4P_eta);
        outTree->Branch("silseed_f4a_bestG4P_phi", &silseed_f4a_bestG4P_phi);
        outTree->Branch("silseed_f4a_bestG4P_ancestor_trackID", &silseed_f4a_bestG4P_ancestor_trackID);
        outTree->Branch("silseed_f4a_bestG4P_ancestor_PID", &silseed_f4a_bestG4P_ancestor_PID);

        outTree->Branch("mvtx_seedcluster_matchedG4P_trackID", &mvtx_seedcluster_matchedG4P_trackID);
        outTree->Branch("mvtx_seedcluster_matchedG4P_PID", &mvtx_seedcluster_matchedG4P_PID);
        outTree->Branch("mvtx_seedcluster_matchedG4P_E", &mvtx_seedcluster_matchedG4P_E);
        outTree->Branch("mvtx_seedcluster_matchedG4P_pT", &mvtx_seedcluster_matchedG4P_pT);
        outTree->Branch("mvtx_seedcluster_matchedG4P_eta", &mvtx_seedcluster_matchedG4P_eta);
        outTree->Branch("mvtx_seedcluster_matchedG4P_phi", &mvtx_seedcluster_matchedG4P_phi);
        outTree->Branch("mvtx_seedcluster_matchedG4P_ancestor_trackID", &mvtx_seedcluster_matchedG4P_ancestor_trackID);
        outTree->Branch("mvtx_seedcluster_matchedG4P_ancestor_PID", &mvtx_seedcluster_matchedG4P_ancestor_PID);

        outTree->Branch("N_PrimaryPHG4Ptcl", &N_PrimaryPHG4Ptcl);
        outTree->Branch("N_sPHENIXPrimary", &N_sPHENIXPrimary);
        outTree->Branch("N_AllPHG4Ptcl", &N_AllPHG4Ptcl);

        outTree->Branch("PrimaryPHG4Ptcl_pT", &PrimaryPHG4Ptcl_pT);
        outTree->Branch("PrimaryPHG4Ptcl_eta", &PrimaryPHG4Ptcl_eta);
        outTree->Branch("PrimaryPHG4Ptcl_phi", &PrimaryPHG4Ptcl_phi);
        outTree->Branch("PrimaryPHG4Ptcl_E", &PrimaryPHG4Ptcl_E);
        outTree->Branch("PrimaryPHG4Ptcl_PID", &PrimaryPHG4Ptcl_PID);
        outTree->Branch("PrimaryPHG4Ptcl_trackID", &PrimaryPHG4Ptcl_trackID);
        outTree->Branch("PrimaryPHG4Ptcl_originTrackID", &PrimaryPHG4Ptcl_originTrackID);
        outTree->Branch("PrimaryPHG4Ptcl_originVtxID", &PrimaryPHG4Ptcl_originVtxID);
        outTree->Branch("PrimaryPHG4Ptcl_originVtxT", &PrimaryPHG4Ptcl_originVtxT);
        outTree->Branch("PrimaryPHG4Ptcl_originCrossing", &PrimaryPHG4Ptcl_originCrossing);
        outTree->Branch("PrimaryPHG4Ptcl_originIsEmbeded", &PrimaryPHG4Ptcl_originIsEmbeded);
        outTree->Branch("PrimaryPHG4Ptcl_ParticleClass", &PrimaryPHG4Ptcl_ParticleClass);
        outTree->Branch("PrimaryPHG4Ptcl_isStable", &PrimaryPHG4Ptcl_isStable);
        outTree->Branch("PrimaryPHG4Ptcl_charge", &PrimaryPHG4Ptcl_charge);
        outTree->Branch("PrimaryPHG4Ptcl_isChargedHadron", &PrimaryPHG4Ptcl_isChargedHadron);
        outTree->Branch("PrimaryPHG4Ptcl_ancestor_trackID", &PrimaryPHG4Ptcl_ancestor_trackID);
        outTree->Branch("PrimaryPHG4Ptcl_ancestor_PID", &PrimaryPHG4Ptcl_ancestor_PID);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_X", &PrimaryPHG4Ptcl_truthcluster_X);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_Y", &PrimaryPHG4Ptcl_truthcluster_Y);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_Z", &PrimaryPHG4Ptcl_truthcluster_Z);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_edep", &PrimaryPHG4Ptcl_truthcluster_edep);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_adc", &PrimaryPHG4Ptcl_truthcluster_adc);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_r", &PrimaryPHG4Ptcl_truthcluster_r);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_phi", &PrimaryPHG4Ptcl_truthcluster_phi);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_eta", &PrimaryPHG4Ptcl_truthcluster_eta);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_phisize", &PrimaryPHG4Ptcl_truthcluster_phisize);
        outTree->Branch("PrimaryPHG4Ptcl_truthcluster_zsize", &PrimaryPHG4Ptcl_truthcluster_zsize);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_globalX", &PrimaryPHG4Ptcl_recocluster_globalX);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_globalY", &PrimaryPHG4Ptcl_recocluster_globalY);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_globalZ", &PrimaryPHG4Ptcl_recocluster_globalZ);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_r", &PrimaryPHG4Ptcl_recocluster_r);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_phi", &PrimaryPHG4Ptcl_recocluster_phi);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_eta", &PrimaryPHG4Ptcl_recocluster_eta);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_phisize", &PrimaryPHG4Ptcl_recocluster_phisize);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_zsize", &PrimaryPHG4Ptcl_recocluster_zsize);
        outTree->Branch("PrimaryPHG4Ptcl_recocluster_adc", &PrimaryPHG4Ptcl_recocluster_adc);

        outTree->Branch("sPHENIXPrimary_pT", &sPHENIXPrimary_pT);
        outTree->Branch("sPHENIXPrimary_eta", &sPHENIXPrimary_eta);
        outTree->Branch("sPHENIXPrimary_phi", &sPHENIXPrimary_phi);
        outTree->Branch("sPHENIXPrimary_E", &sPHENIXPrimary_E);
        outTree->Branch("sPHENIXPrimary_PID", &sPHENIXPrimary_PID);
        outTree->Branch("sPHENIXPrimary_trackID", &sPHENIXPrimary_trackID);
        outTree->Branch("sPHENIXPrimary_originTrackID", &sPHENIXPrimary_originTrackID);
        outTree->Branch("sPHENIXPrimary_originVtxID", &sPHENIXPrimary_originVtxID);
        outTree->Branch("sPHENIXPrimary_originVtxT", &sPHENIXPrimary_originVtxT);
        outTree->Branch("sPHENIXPrimary_originCrossing", &sPHENIXPrimary_originCrossing);
        outTree->Branch("sPHENIXPrimary_originIsEmbeded", &sPHENIXPrimary_originIsEmbeded);
        outTree->Branch("sPHENIXPrimary_ParticleClass", &sPHENIXPrimary_ParticleClass);
        outTree->Branch("sPHENIXPrimary_isStable", &sPHENIXPrimary_isStable);
        outTree->Branch("sPHENIXPrimary_charge", &sPHENIXPrimary_charge);
        outTree->Branch("sPHENIXPrimary_isChargedHadron", &sPHENIXPrimary_isChargedHadron);
        outTree->Branch("sPHENIXPrimary_ancestor_trackID", &sPHENIXPrimary_ancestor_trackID);
        outTree->Branch("sPHENIXPrimary_ancestor_PID", &sPHENIXPrimary_ancestor_PID);
        outTree->Branch("sPHENIXPrimary_truthcluster_X", &sPHENIXPrimary_truthcluster_X);
        outTree->Branch("sPHENIXPrimary_truthcluster_Y", &sPHENIXPrimary_truthcluster_Y);
        outTree->Branch("sPHENIXPrimary_truthcluster_Z", &sPHENIXPrimary_truthcluster_Z);
        outTree->Branch("sPHENIXPrimary_truthcluster_edep", &sPHENIXPrimary_truthcluster_edep);
        outTree->Branch("sPHENIXPrimary_truthcluster_adc", &sPHENIXPrimary_truthcluster_adc);
        outTree->Branch("sPHENIXPrimary_truthcluster_r", &sPHENIXPrimary_truthcluster_r);
        outTree->Branch("sPHENIXPrimary_truthcluster_phi", &sPHENIXPrimary_truthcluster_phi);
        outTree->Branch("sPHENIXPrimary_truthcluster_eta", &sPHENIXPrimary_truthcluster_eta);
        outTree->Branch("sPHENIXPrimary_truthcluster_phisize", &sPHENIXPrimary_truthcluster_phisize);
        outTree->Branch("sPHENIXPrimary_truthcluster_zsize", &sPHENIXPrimary_truthcluster_zsize);
        outTree->Branch("sPHENIXPrimary_recocluster_globalX", &sPHENIXPrimary_recocluster_globalX);
        outTree->Branch("sPHENIXPrimary_recocluster_globalY", &sPHENIXPrimary_recocluster_globalY);
        outTree->Branch("sPHENIXPrimary_recocluster_globalZ", &sPHENIXPrimary_recocluster_globalZ);
        outTree->Branch("sPHENIXPrimary_recocluster_r", &sPHENIXPrimary_recocluster_r);
        outTree->Branch("sPHENIXPrimary_recocluster_phi", &sPHENIXPrimary_recocluster_phi);
        outTree->Branch("sPHENIXPrimary_recocluster_eta", &sPHENIXPrimary_recocluster_eta);
        outTree->Branch("sPHENIXPrimary_recocluster_phisize", &sPHENIXPrimary_recocluster_phisize);
        outTree->Branch("sPHENIXPrimary_recocluster_zsize", &sPHENIXPrimary_recocluster_zsize);
        outTree->Branch("sPHENIXPrimary_recocluster_adc", &sPHENIXPrimary_recocluster_adc);

        outTree->Branch("AllPHG4Ptcl_pT", &AllPHG4Ptcl_pT);
        outTree->Branch("AllPHG4Ptcl_eta", &AllPHG4Ptcl_eta);
        outTree->Branch("AllPHG4Ptcl_phi", &AllPHG4Ptcl_phi);
        outTree->Branch("AllPHG4Ptcl_E", &AllPHG4Ptcl_E);
        outTree->Branch("AllPHG4Ptcl_PID", &AllPHG4Ptcl_PID);
        outTree->Branch("AllPHG4Ptcl_trackID", &AllPHG4Ptcl_trackID);
        outTree->Branch("AllPHG4Ptcl_originTrackID", &AllPHG4Ptcl_originTrackID);
        outTree->Branch("AllPHG4Ptcl_originVtxID", &AllPHG4Ptcl_originVtxID);
        outTree->Branch("AllPHG4Ptcl_originVtxT", &AllPHG4Ptcl_originVtxT);
        outTree->Branch("AllPHG4Ptcl_originCrossing", &AllPHG4Ptcl_originCrossing);
        outTree->Branch("AllPHG4Ptcl_originIsEmbeded", &AllPHG4Ptcl_originIsEmbeded);
        outTree->Branch("AllPHG4Ptcl_ancestor_trackID", &AllPHG4Ptcl_ancestor_trackID);
        outTree->Branch("AllPHG4Ptcl_ancestor_PID", &AllPHG4Ptcl_ancestor_PID);
        outTree->Branch("AllPHG4Ptcl_truthcluster_X", &AllPHG4Ptcl_truthcluster_X);
        outTree->Branch("AllPHG4Ptcl_truthcluster_Y", &AllPHG4Ptcl_truthcluster_Y);
        outTree->Branch("AllPHG4Ptcl_truthcluster_Z", &AllPHG4Ptcl_truthcluster_Z);
        outTree->Branch("AllPHG4Ptcl_truthcluster_edep", &AllPHG4Ptcl_truthcluster_edep);
        outTree->Branch("AllPHG4Ptcl_truthcluster_adc", &AllPHG4Ptcl_truthcluster_adc);
        outTree->Branch("AllPHG4Ptcl_truthcluster_r", &AllPHG4Ptcl_truthcluster_r);
        outTree->Branch("AllPHG4Ptcl_truthcluster_phi", &AllPHG4Ptcl_truthcluster_phi);
        outTree->Branch("AllPHG4Ptcl_truthcluster_eta", &AllPHG4Ptcl_truthcluster_eta);
        outTree->Branch("AllPHG4Ptcl_truthcluster_phisize", &AllPHG4Ptcl_truthcluster_phisize);
        outTree->Branch("AllPHG4Ptcl_truthcluster_zsize", &AllPHG4Ptcl_truthcluster_zsize);
        outTree->Branch("AllPHG4Ptcl_recocluster_globalX", &AllPHG4Ptcl_recocluster_globalX);
        outTree->Branch("AllPHG4Ptcl_recocluster_globalY", &AllPHG4Ptcl_recocluster_globalY);
        outTree->Branch("AllPHG4Ptcl_recocluster_globalZ", &AllPHG4Ptcl_recocluster_globalZ);
        outTree->Branch("AllPHG4Ptcl_recocluster_r", &AllPHG4Ptcl_recocluster_r);
        outTree->Branch("AllPHG4Ptcl_recocluster_phi", &AllPHG4Ptcl_recocluster_phi);
        outTree->Branch("AllPHG4Ptcl_recocluster_eta", &AllPHG4Ptcl_recocluster_eta);
        outTree->Branch("AllPHG4Ptcl_recocluster_phisize", &AllPHG4Ptcl_recocluster_phisize);
        outTree->Branch("AllPHG4Ptcl_recocluster_zsize", &AllPHG4Ptcl_recocluster_zsize);
        outTree->Branch("AllPHG4Ptcl_recocluster_adc", &AllPHG4Ptcl_recocluster_adc);
    }

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int VertexCompare::InitRun(PHCompositeNode *topNode) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
int VertexCompare::process_event(PHCompositeNode *topNode)
{
    std::cout << "VertexCompare::process_event - Processing event " << counter << std::endl;

    if (truthOnlyOutput_)
    {
        m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
        if (!m_truth_info)
        {
            std::cout << "VertexCompare::process_event - [ERROR/WARNING] - can't find G4TruthInfoContainer node G4TruthInfo" << std::endl;
            return Fun4AllReturnCodes::EVENT_OK;
        }

        FillTruthParticleTree();
        outTree->Fill();
        ++counter;
        Cleanup();
        return Fun4AllReturnCodes::EVENT_OK;
    }

    PHNodeIterator dstiter(topNode);
    PHCompositeNode *dstNode = dynamic_cast<PHCompositeNode *>(dstiter.findFirst("PHCompositeNode", "DST"));

    MbdVertexMap *m_dst_mbdvertexmap = findNode::getClass<MbdVertexMap>(topNode, "MbdVertexMap");
    SvtxVertexMap *m_dst_vertexmap = findNode::getClass<SvtxVertexMap>(topNode, "SvtxVertexMap");

    auto globalvertexmap = findNode::getClass<GlobalVertexMap>(topNode, "GlobalVertexMap");

    clustermap = findNode::getClass<TrkrClusterContainer>(dstNode, clusterContainerName);
    clusterhitassoc = findNode::getClass<TrkrClusterHitAssoc>(dstNode, clusterHitAssocName);
    clustercrossingassoc = findNode::getClass<TrkrClusterCrossingAssoc>(dstNode, "TRKR_CLUSTERCROSSINGASSOC");
    geometry = findNode::getClass<ActsGeometry>(topNode, geometryNodeName);
    tpcgeom = findNode::getClass<PHG4TpcGeomContainer>(topNode, "TPCGEOMCONTAINER");
    silseedmap = findNode::getClass<TrackSeedContainer>(topNode, seedContainerName);
    tpcseedmap = findNode::getClass<TrackSeedContainer>(topNode, tpcSeedContainerName);
    svtxTrackMap = findNode::getClass<SvtxTrackMap>(topNode, svtxTrackMapName);
    gl1PacketInfo = findNode::getClass<Gl1Packet>(topNode, gl1NodeName);
    m_mbdout = findNode::getClass<MbdOut>(topNode, mbdOutNodeName);
    minimumbiasinfo = findNode::getClass<MinimumBiasInfo>(topNode, "MinimumBiasInfo");
    m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");

    if (!m_dst_mbdvertexmap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find MbdVertexMap node " << "MbdVertexMap" << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!m_dst_vertexmap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find SvtxVertexMap node " << "SvtxVertexMap" << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!globalvertexmap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find GlobalVertexMap node " << "GlobalVertexMap" << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!clustermap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find cluster map node " << clusterContainerName << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!clusterhitassoc)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find cluster hit association node " << clusterHitAssocName << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!clustercrossingassoc)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find cluster crossing association node TRKR_CLUSTERCROSSINGASSOC" << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!geometry)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find ActsGeometry node " << geometryNodeName << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (writeTrackBranches_ && !tpcgeom)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - cannot find TPC geometry node TPCGEOMCONTAINER for track dEdx" << std::endl;
    }
    if (!silseedmap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find silicon seed map node " << seedContainerName << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (writeTpcSeedBranches_ && !tpcseedmap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - can't find TPC seed map node " << tpcSeedContainerName << "; will use TPC seeds from SvtxTrackMap" << std::endl;
    }
    if (!svtxTrackMap)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [ERROR/WARNING] - can't find SvtxTrackMap node " << svtxTrackMapName << std::endl;
        // return Fun4AllReturnCodes::ABORTEVENT;
    }
    if (!gl1PacketInfo)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - can't find GL1 node " << gl1NodeName << std::endl;
        // return Fun4AllReturnCodes::EVENT_OK;
    }
    else
    {
        gl1BunchCrossing = gl1PacketInfo->getBunchNumber();
        uint64_t triggervec = gl1PacketInfo->getScaledVector();
        gl1bco = gl1PacketInfo->getBCO();
        auto lbshift = gl1bco << 24U;
        bcotr = lbshift >> 24U;
        for (int i = 0; i < 64; ++i)
        {
            bool trig_decision = ((triggervec & 0x1U) == 0x1U);
            if (trig_decision)
            {
                firedTriggers.push_back(i);
            }
            triggervec = (triggervec >> 1U) & 0xffffffffU;
        }
    }
    if (!m_mbdout)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - can't find MbdOut node " << mbdOutNodeName << std::endl;
        // return Fun4AllReturnCodes::EVENT_OK;
    }
    else
    {
        MBD_charge_sum = m_mbdout->get_q(0) + m_mbdout->get_q(1);
    }

    is_min_bias = (minimumbiasinfo) ? minimumbiasinfo->isAuAuMinimumBias() : false;

    // centrality information
    if (!m_CentInfo)
    {
        std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - can't find CentralityInfo node " << "CentralityInfo" << std::endl;
        centrality_mbd_ = -1.;
        // return Fun4AllReturnCodes::EVENT_OK;
    }
    else
    {
        if (m_CentInfo->has_centrality_bin(CentralityInfo::PROP::mbd_NS))
        {
            centrality_mbd_ = m_CentInfo->get_centrality_bin(CentralityInfo::PROP::mbd_NS);
        }
        else
        {
            std::cout << "[WARNING/ERROR] No centrality information found in CentralityInfo. Setting centrality_mbd_ to -2. Please check!" << std::endl;
            m_CentInfo->identify();
            centrality_mbd_ = -2.;
        }
    }

    // mbdVertex = trackerVertexX = trackerVertexY = trackerVertexZ = trackerVertexChisq = std::numeric_limits<float>::quiet_NaN();
    // mbdVertex = std::numeric_limits<float>::quiet_NaN();
    // nTracks = n_MBDVertex = n_TRKVertex = std::numeric_limits<unsigned int>::quiet_NaN();
    nTracks = n_MBDVertex = n_TRKVertex = 0;

    hasMBD = false;
    hasTRK = false;

    // std::cout << "Number of vertices in GlobalVertexMap: " << globalvertexmap->size() << std::endl;
    for (GlobalVertexMap::ConstIter iter = globalvertexmap->begin(); iter != globalvertexmap->end(); ++iter)
    {
        GlobalVertex *gvertex = iter->second;

        if (gvertex->count_vtxs(mbdType) != 0)
        {
            // std::cout << "Found MBD vertex in GlobalVertexMap." << std::endl;
            hasMBD = true;

            auto mbditer = gvertex->find_vertexes(mbdType);
            auto mbdvertexvector = mbditer->second;

            n_MBDVertex += mbdvertexvector.size();
            for (auto &vertex : mbdvertexvector)
            {
                MbdVertex *m_dst_vertex = m_dst_mbdvertexmap->find(vertex->get_id())->second;

                mbdVertexId.push_back(m_dst_vertex->get_id());
                mbdVertex.push_back(m_dst_vertex->get_z());
                mbdVertexCrossing.push_back(m_dst_vertex->get_beam_crossing());

                // std::cout << "MBD vertex z: " << m_dst_vertex->get_z() << std::endl;
            }
        }

        if (gvertex->count_vtxs(trkType) != 0)
        {
            hasTRK = true;

            auto trkiter = gvertex->find_vertexes(trkType);
            auto trkvertexvector = trkiter->second;

            n_TRKVertex += trkvertexvector.size();
            for (auto &vertex : trkvertexvector)
            {
                SvtxVertex *m_dst_vertex = m_dst_vertexmap->find(vertex->get_id())->second;
                // std::cout << "Tracker vertex z: " << m_dst_vertex->get_z() << ", crossing: " << m_dst_vertex->get_beam_crossing() << std::endl;

                trackerVertexId.push_back(m_dst_vertex->get_id());
                trackerVertexX.push_back(m_dst_vertex->get_x());
                trackerVertexY.push_back(m_dst_vertex->get_y());
                trackerVertexZ.push_back(m_dst_vertex->get_z());
                trackerVertexChisq.push_back(m_dst_vertex->get_chisq());
                trackerVertexNdof.push_back(m_dst_vertex->get_ndof());
                trackerVertexNTracks.push_back(m_dst_vertex->size_tracks());
                trackerVertexCrossing.push_back(m_dst_vertex->get_beam_crossing());

                // loop over tracks associated with this vertex and save their track IDs
                std::vector<int> trackIDs;
                trackIDs.clear();
                for (auto trackiter = m_dst_vertex->begin_tracks(); trackiter != m_dst_vertex->end_tracks(); ++trackiter)
                {
                    trackIDs.push_back(*trackiter);
                }

                // print out for debugging
                // std::cout << "Tracker vertex ID " << m_dst_vertex->get_id() << " with crossing " << m_dst_vertex->get_beam_crossing() << " m_dst_vertex->size_tracks() = " << m_dst_vertex->size_tracks() << " trackIDs.size() = " << trackIDs.size() << ": [";
                // for (const auto &trackID : trackIDs)
                // {
                //     std::cout << trackID << " ";
                // }
                // std::cout << "]" << std::endl;

                trackerVertexTrackIDs.push_back(trackIDs);

                // if (m_dst_vertex->get_beam_crossing() != 0)
                //     continue;
                // if (m_dst_vertex->size_tracks() > nTracks)
                // {
                //     trackerVertexX = m_dst_vertex->get_x();
                //     trackerVertexY = m_dst_vertex->get_y();
                //     trackerVertexZ = m_dst_vertex->get_z();
                //     trackerVertexChisq = m_dst_vertex->get_chisq();
                //     trackerVertexNdof = m_dst_vertex->get_ndof();
                //     nTracks = m_dst_vertex->size_tracks();
                // }
                // if (nTracks == 0)
                //     hasTRK = false;
            }
        }
    }

    nSvtxVertices = trackerVertexId.size();
    // count vertices with valid crossing from trackerVertexCrossing (not SHORT_MAX)
    nSvtxVertices_validCrossing = std::count_if(trackerVertexCrossing.begin(), trackerVertexCrossing.end(), [](short crossing) { return crossing != std::numeric_limits<short>::max(); });

    // loop over all vertices in MbdVertexMap and fill all vertices to ntuple
    // n_MBDVertex = m_dst_mbdvertexmap->size();
    // for (const auto& [key, vertex] : *m_dst_mbdvertexmap)
    // {
    //     mbdVertexId.push_back(vertex->get_id());
    //     mbdVertex.push_back(vertex->get_z());
    //     mbdVertexCrossing.push_back(vertex->get_beam_crossing());
    // }

    // loop over all vertices in SvtxVertexMap and fill all vertices to ntuple
    // nSvtxVertices = m_dst_vertexmap->size();
    // for (const auto& [key, vertex] : *m_dst_vertexmap)
    // {
    //     trackerVertexId.push_back(vertex->get_id());
    //     trackerVertexX.push_back(vertex->get_x());
    //     trackerVertexY.push_back(vertex->get_y());
    //     trackerVertexZ.push_back(vertex->get_z());
    //     trackerVertexChisq.push_back(vertex->get_chisq()); //! For vertex from PHSimpleVertexFinder, the chisq and ndof are alway 0, need to check if this is intended
    //     trackerVertexNdof.push_back(vertex->get_ndof());
    //     trackerVertexNTracks.push_back(vertex->size_tracks());
    //     trackerVertexCrossing.push_back(vertex->get_beam_crossing());
    // }

    // simulation setup
    if (isSimulation)
    {
        svtxPHG4ParticleMap = findNode::getClass<SvtxPHG4ParticleMap>(topNode, svtxPHG4ParticleMapName);
        if (!svtxPHG4ParticleMap)
        {
            std::cout << "[WARNING/ERROR] VertexCompare::process_event - [ERROR/WARNING] - can't find SvtxPHG4ParticleMap node " << svtxPHG4ParticleMapName << std::endl;
        }
        hasSvtxPHG4ParticleMap = (svtxPHG4ParticleMap != nullptr);
        svtxPHG4ParticleMapProcessed = (svtxPHG4ParticleMap != nullptr && svtxPHG4ParticleMap->processed());
        // svtxPHG4ParticleMap->identify();

        m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
        if (!m_truth_info)
        {
            std::cout << "[WARNING/ERROR] VertexCompare::process_event - [ERROR/WARNING] - can't find G4TruthInfoContainer node " << "G4TruthInfo" << std::endl;
            // return Fun4AllReturnCodes::ABORTEVENT;
        }

        if (!svtx_evalstack)
        {
            svtx_evalstack = new SvtxEvalStack(topNode);
            svtx_evalstack->set_strict(false);
            // svtx_evalstack->do_caching(false);
            clustereval = svtx_evalstack->get_cluster_eval();
            hiteval = svtx_evalstack->get_hit_eval();
            truth_eval = svtx_evalstack->get_truth_eval();
        }

        svtx_evalstack->next_event(topNode);
    }

    FillSiliconSeedTree();
    if (writeTpcSeedBranches_)
    {
        FillTpcSeedTree();
    }
    if (writeTrackBranches_)
    {
        FillTrackTree();
    }

    FillClusterTree();

    if (isSimulation)
    {
        FillTruthParticleTree();
    }

    outTree->Fill();

    ++counter;

    Cleanup();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void VertexCompare::FillTrackTree()
{
    const int kMissingInt = std::numeric_limits<int>::max();
    const float kMissingFloat = std::numeric_limits<float>::quiet_NaN();

    if (!svtxTrackMap)
    {
        return;
    }

    float layerThicknesses[4] = {0.0, 0.0, 0.0, 0.0};
    bool canCalculateDedx = false;
    if (clustermap && geometry && tpcgeom)
    {
        auto *inner1 = tpcgeom->GetLayerCellGeom(7);
        auto *inner2 = tpcgeom->GetLayerCellGeom(8);
        auto *middle = tpcgeom->GetLayerCellGeom(27);
        auto *outer = tpcgeom->GetLayerCellGeom(50);
        if (inner1 && inner2 && middle && outer)
        {
            layerThicknesses[0] = inner1->get_thickness();
            layerThicknesses[1] = inner2->get_thickness();
            layerThicknesses[2] = middle->get_thickness();
            layerThicknesses[3] = outer->get_thickness();
            canCalculateDedx = true;
        }
    }

    const auto count_seed_hits = [](const TrackSeed *seed, int &nhits, int &nmaps, int &nintt, int &ntpc, int &nmms, int &ntpc1, int &ntpc11, int &ntpc2, int &ntpc3)
    {
        if (!seed)
        {
            return;
        }

        nhits += seed->size_cluster_keys();
        for (auto iter = seed->begin_cluster_keys(); iter != seed->end_cluster_keys(); ++iter)
        {
            const auto clusterKey = *iter;
            const unsigned int layer = TrkrDefs::getLayer(clusterKey);
            switch (TrkrDefs::getTrkrId(clusterKey))
            {
            case TrkrDefs::TrkrId::mvtxId:
                ++nmaps;
                break;
            case TrkrDefs::TrkrId::inttId:
                ++nintt;
                break;
            case TrkrDefs::TrkrId::tpcId:
                ++ntpc;
                if ((layer - 7U) < 8U)
                {
                    ++ntpc11;
                }
                if ((layer - 7U) < 16U)
                {
                    ++ntpc1;
                }
                else if ((layer - 7U) < 32U)
                {
                    ++ntpc2;
                }
                else if ((layer - 7U) < 48U)
                {
                    ++ntpc3;
                }
                break;
            case TrkrDefs::TrkrId::micromegasId:
                ++nmms;
                break;
            default:
                break;
            }
        }
    };

    const auto fill_seed_clusters = [this](const TrackSeed *seed,
                                           std::vector<unsigned int> &layers,
                                           std::vector<float> &globalX,
                                           std::vector<float> &globalY,
                                           std::vector<float> &globalZ,
                                           std::vector<float> &phis,
                                           std::vector<float> &etas,
                                           std::vector<float> &radii)
    {
        if (!seed || !clustermap || !geometry)
        {
            return;
        }

        for (auto iter = seed->begin_cluster_keys(); iter != seed->end_cluster_keys(); ++iter)
        {
            const auto clusterKey = *iter;
            auto *cluster = clustermap->findCluster(clusterKey);
            if (!cluster)
            {
                continue;
            }

            const auto globalpos = geometry->getGlobalPosition(clusterKey, cluster);
            layers.push_back(TrkrDefs::getLayer(clusterKey));
            globalX.push_back(globalpos.x());
            globalY.push_back(globalpos.y());
            globalZ.push_back(globalpos.z());
            phis.push_back(std::atan2(globalpos.y(), globalpos.x()));
            TVector3 posvec(globalpos.x(), globalpos.y(), globalpos.z());
            etas.push_back(posvec.Eta());
            radii.push_back((globalpos.y() > 0) ? std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()) : -std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()));
        }
    };

    for (auto trackIter = svtxTrackMap->begin(); trackIter != svtxTrackMap->end(); ++trackIter)
    {
        SvtxTrack *track = trackIter->second;
        if (!track)
        {
            continue;
        }

        TrackSeed *tpc_seed = track->get_tpc_seed();
        TrackSeed *silicon_seed = track->get_silicon_seed();

        int nhits = 0;
        int nmaps = 0;
        int nintt = 0;
        int ntpc = 0;
        int nmms = 0;
        int ntpc1 = 0;
        int ntpc11 = 0;
        int ntpc2 = 0;
        int ntpc3 = 0;
        count_seed_hits(tpc_seed, nhits, nmaps, nintt, ntpc, nmms, ntpc1, ntpc11, ntpc2, ntpc3);
        count_seed_hits(silicon_seed, nhits, nmaps, nintt, ntpc, nmms, ntpc1, ntpc11, ntpc2, ntpc3);

        std::vector<unsigned int> cluster_layers;
        std::vector<float> cluster_globalX;
        std::vector<float> cluster_globalY;
        std::vector<float> cluster_globalZ;
        std::vector<float> cluster_phi;
        std::vector<float> cluster_eta;
        std::vector<float> cluster_r;
        fill_seed_clusters(tpc_seed, cluster_layers, cluster_globalX, cluster_globalY, cluster_globalZ, cluster_phi, cluster_eta, cluster_r);
        fill_seed_clusters(silicon_seed, cluster_layers, cluster_globalX, cluster_globalY, cluster_globalZ, cluster_phi, cluster_eta, cluster_r);

        const float px = track->get_px();
        const float py = track->get_py();
        const float pz = track->get_pz();
        const TVector3 momentum(px, py, pz);
        const float pt = momentum.Pt();
        const float eta = momentum.Eta();
        const float phi = momentum.Phi();

        const float cvxx = track->get_error(3, 3);
        const float cvxy = track->get_error(3, 4);
        const float cvxz = track->get_error(3, 5);
        const float cvyy = track->get_error(4, 4);
        const float cvyz = track->get_error(4, 5);
        const float cvzz = track->get_error(5, 5);

        const double pt2 = static_cast<double>(px) * px + static_cast<double>(py) * py;
        const double p2 = pt2 + static_cast<double>(pz) * pz;

        float deltapt = kMissingFloat;
        if (pt2 > 0.)
        {
            const double arg = (static_cast<double>(cvxx) * px * px + 2. * static_cast<double>(cvxy) * px * py + static_cast<double>(cvyy) * py * py) / pt2;
            if (std::isfinite(arg) && arg >= 0.)
            {
                deltapt = std::sqrt(arg);
            }
        }

        float deltaeta = kMissingFloat;
        if (pt2 > 0. && p2 > 0.)
        {
            const double numerator =
                static_cast<double>(cvzz) * pt2 * pt2 +
                static_cast<double>(pz) *
                    (-2. * (static_cast<double>(cvxz) * px + static_cast<double>(cvyz) * py) * pt2 +
                     static_cast<double>(cvxx) * px * px * pz +
                     static_cast<double>(cvyy) * py * py * pz +
                     2. * static_cast<double>(cvxy) * px * py * pz);
            const double arg = numerator / (pt2 * pt2 * p2);
            if (std::isfinite(arg) && arg >= 0.)
            {
                deltaeta = std::sqrt(arg);
            }
        }

        float deltaphi = kMissingFloat;
        if (pt2 > 0.)
        {
            const double arg =
                (static_cast<double>(cvyy) * px * px - 2. * static_cast<double>(cvxy) * px * py + static_cast<double>(cvxx) * py * py) /
                (pt2 * pt2);
            if (std::isfinite(arg) && arg >= 0.)
            {
                deltaphi = std::sqrt(arg);
            }
        }

        const int vertexID = track->get_vertex_id();
        float vx = kMissingFloat;
        float vy = kMissingFloat;
        float vz = kMissingFloat;
        auto vertexIter = std::find(trackerVertexId.begin(), trackerVertexId.end(), vertexID);
        if (vertexIter != trackerVertexId.end())
        {
            const auto vertexIndex = std::distance(trackerVertexId.begin(), vertexIter);
            vx = trackerVertexX[vertexIndex];
            vy = trackerVertexY[vertexIndex];
            vz = trackerVertexZ[vertexIndex];
        }

        float dedx = kMissingFloat;
        if (canCalculateDedx && tpc_seed)
        {
            dedx = TrackAnalysisUtils::calc_dedx(tpc_seed, clustermap, geometry, layerThicknesses);
        }

        float hlxpt = kMissingFloat;
        float hlxeta = kMissingFloat;
        float hlxphi = kMissingFloat;
        float hlxX0 = kMissingFloat;
        float hlxY0 = kMissingFloat;
        float hlxZ0 = kMissingFloat;
        int hlxcharge = 0;
        if (tpc_seed)
        {
            hlxpt = tpc_seed->get_pt();
            hlxeta = tpc_seed->get_eta();
            hlxphi = tpc_seed->get_phi();
            hlxX0 = tpc_seed->get_X0();
            hlxY0 = tpc_seed->get_Y0();
            hlxZ0 = tpc_seed->get_Z0();
            if (std::isfinite(tpc_seed->get_qOverR()) && tpc_seed->get_qOverR() != 0.)
            {
                hlxcharge = (tpc_seed->get_qOverR() > 0.) ? 1 : -1;
            }
        }

        track_deltapt.push_back(deltapt);
        track_deltaeta.push_back(deltaeta);
        track_deltaphi.push_back(deltaphi);
        track_nhits.push_back(nhits);
        track_nmaps.push_back(nmaps);
        track_nintt.push_back(nintt);
        track_ntpc.push_back(ntpc);
        track_nmms.push_back(nmms);
        track_ntpc1.push_back(ntpc1);
        track_ntpc11.push_back(ntpc11);
        track_ntpc2.push_back(ntpc2);
        track_ntpc3.push_back(ntpc3);
        track_pidedx.push_back(kMissingFloat);
        track_kdedx.push_back(kMissingFloat);
        track_prdedx.push_back(kMissingFloat);
        track_vx.push_back(vx);
        track_vy.push_back(vy);
        track_vz.push_back(vz);
        track_dca2d.push_back(track->get_dca2d());
        track_dca2dsigma.push_back(track->get_dca2d_error());
        track_dca3dxy.push_back(track->get_dca3d_xy());
        track_dca3dxysigma.push_back(track->get_dca3d_xy_error());
        track_dca3dz.push_back(track->get_dca3d_z());
        track_dca3dzsigma.push_back(track->get_dca3d_z_error());
        track_hlxpt.push_back(hlxpt);
        track_hlxeta.push_back(hlxeta);
        track_hlxphi.push_back(hlxphi);
        track_hlxX0.push_back(hlxX0);
        track_hlxY0.push_back(hlxY0);
        track_hlxZ0.push_back(hlxZ0);
        track_hlxcharge.push_back(hlxcharge);

        track_id.push_back(track->get_id());
        track_x.push_back(track->get_x());
        track_y.push_back(track->get_y());
        track_z.push_back(track->get_z());
        track_px.push_back(px);
        track_py.push_back(py);
        track_pz.push_back(pz);
        track_pt.push_back(pt);
        track_eta.push_back(eta);
        track_phi.push_back(phi);
        track_dedx.push_back(dedx);
        track_charge.push_back(track->get_charge());
        track_crossing.push_back(track->get_crossing());
        track_vertex_id.push_back((track->get_vertex_id() == std::numeric_limits<unsigned int>::max()) ? kMissingInt : static_cast<int>(track->get_vertex_id()));
        track_chisq.push_back(track->get_chisq());
        track_ndf.push_back(static_cast<int>(track->get_ndf()));
        track_quality.push_back(track->get_quality());
        track_cluster_layer.push_back(cluster_layers);
        track_cluster_globalX.push_back(cluster_globalX);
        track_cluster_globalY.push_back(cluster_globalY);
        track_cluster_globalZ.push_back(cluster_globalZ);
        track_cluster_phi.push_back(cluster_phi);
        track_cluster_eta.push_back(cluster_eta);
        track_cluster_r.push_back(cluster_r);

        if (!silicon_seed)
        {
            track_silseed_id.push_back(kMissingInt);
            track_silseed_x.push_back(kMissingFloat);
            track_silseed_y.push_back(kMissingFloat);
            track_silseed_z.push_back(kMissingFloat);
            track_silseed_pt.push_back(kMissingFloat);
            track_silseed_eta.push_back(kMissingFloat);
            track_silseed_phi.push_back(kMissingFloat);
            track_silseed_crossing.push_back(kMissingInt);
            track_silseed_charge.push_back(kMissingInt);
            track_silseed_nMvtx.push_back(0);
            track_silseed_nIntt.push_back(0);
            track_silseed_clusterKeys.push_back(std::vector<uint64_t>());
            continue;
        }

        const auto seed_pos = TrackSeedHelper::get_xyz(silicon_seed);
        const std::vector<uint64_t> seed_cluskeys(silicon_seed->begin_cluster_keys(), silicon_seed->end_cluster_keys());

        int nMvtx = 0;
        int nIntt = 0;
        for (const auto cluskey : seed_cluskeys)
        {
            if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::mvtxId)
            {
                ++nMvtx;
            }
            else if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::inttId)
            {
                ++nIntt;
            }
        }

        track_silseed_id.push_back(silseedmap ? static_cast<int>(silseedmap->find(silicon_seed)) : kMissingInt);
        track_silseed_x.push_back(seed_pos.x());
        track_silseed_y.push_back(seed_pos.y());
        track_silseed_z.push_back(seed_pos.z());
        track_silseed_pt.push_back(silicon_seed->get_pt());
        track_silseed_eta.push_back(silicon_seed->get_eta());
        track_silseed_phi.push_back(silicon_seed->get_phi());
        track_silseed_crossing.push_back(silicon_seed->get_crossing());
        track_silseed_charge.push_back((silicon_seed->get_qOverR() > 0) ? 1 : -1);
        track_silseed_nMvtx.push_back(nMvtx);
        track_silseed_nIntt.push_back(nIntt);
        track_silseed_clusterKeys.push_back(seed_cluskeys);
    }

    nRecoTracks = track_id.size();
    nTracks = nRecoTracks;

    std::cout << "Total number of tracks in this event(trigger frame): " << nRecoTracks << std::endl;
}

void VertexCompare::FillSiliconSeedTree()
{
    constexpr int kUnassociatedVertexId = -std::numeric_limits<int>::max();

    std::vector<float> hit_x;
    std::vector<float> hit_y;
    std::vector<float> hit_z;
    std::vector<int> hit_rows;
    std::vector<int> hit_cols;

    std::vector<int> ancestor_trackIDs;
    std::vector<int> ancestor_PIDs;

    std::map<unsigned int, unsigned int> svtxTrackIdBySiliconSeed; // silicon seed ID -> SvtxTrack ID
    if (svtxTrackMap && doTruthMatching_)
    {
        for (auto trackIter = svtxTrackMap->begin(); trackIter != svtxTrackMap->end(); ++trackIter)
        {
            SvtxTrack *track = trackIter->second;
            if (!track || !track->get_silicon_seed())
            {
                continue;
            }

            const auto silicon_seed_id = silseedmap->find(track->get_silicon_seed());

            // print out the svtx track properties and its associated silicon seed properties for debugging --> they are the same, verified
            // std::cout << "SvtxTrack ID: " << track->get_id() << ", associated silicon seed ID: " << silicon_seed_id << ", pt: " << track->get_pt() << ", eta: " << track->get_eta() << ", phi: " << track->get_phi() << std::endl;
            // std::cout << "Associated silicon seed properties, seed ID: " << silicon_seed_id << ", pt: " << track->get_silicon_seed()->get_pt() << ", eta: " << track->get_silicon_seed()->get_eta() << ", phi: " << track->get_silicon_seed()->get_phi() << std::endl;

            svtxTrackIdBySiliconSeed.emplace(static_cast<unsigned int>(silicon_seed_id), track->get_id());
        }
    }

    // helper function to find the vertex index based on the seed crossing and index
    // look up trackerVertexCrossing and trackerVertexTrackIDs, the seed crossing should match the trackerVertexCrossing
    // then from trackerVertexTrackIDs see which of the vertex is the seed associated with, return the vertex index
    // If none of the vertex's trackID list contains the seed track ID, it means the seed is not associated with any vertex
    // In that case, return -1 and the seed eta/phi w.r.t vertex will be set to some large or small value to indicate they are not associated with any vertex
    const auto find_vertex_index_for_crossing = [this](int seed_id, int crossing) -> int
    {
        // first get all vertex indices with the same crossing
        std::vector<int> candidate_vertex_indices;
        for (size_t ivtx = 0; ivtx < trackerVertexCrossing.size(); ++ivtx)
        {
            if (trackerVertexCrossing[ivtx] == crossing)
            {
                candidate_vertex_indices.push_back(static_cast<int>(ivtx));
            }
        }

        // then check if the seed track ID is in any of the candidate vertex track ID list
        for (const auto &vtx_index : candidate_vertex_indices)
        {
            const auto &trackIDs = trackerVertexTrackIDs[vtx_index];
            if (std::find(trackIDs.begin(), trackIDs.end(), seed_id) != trackIDs.end())
            {
                return vtx_index;
            }
        }
        return -1;
    };

    std::vector<std::pair<TrackSeed *, int>> seeds;
    std::map<int, int> first_associated_vertex_index_by_crossing;
    for (auto iter = silseedmap->begin(); iter != silseedmap->end(); ++iter)
    {
        auto *seed = *iter;
        if (!seed)
        {
            continue;
        }

        const int seed_id = silseedmap->find(seed);
        seeds.emplace_back(seed, seed_id);

        const int crossing = seed->get_crossing();
        if (first_associated_vertex_index_by_crossing.find(crossing) != first_associated_vertex_index_by_crossing.end())
        {
            continue;
        }

        const int vtx_index = find_vertex_index_for_crossing(seed_id, crossing);
        if (vtx_index >= 0)
        {
            first_associated_vertex_index_by_crossing.emplace(crossing, vtx_index);
        }
    }

    // loop over all silicon seeds
    // make a map of vector of pair of (seed id, associated vertex id) for each unique crossing for debugging purpose
    std::map<int, std::vector<std::pair<int, int>>> crossing_SeedIdVertexId_map;
    for (const auto &[seed, seed_id] : seeds)
    {
        if (!seed)
        {
            // std::cout << "VertexCompare::FillSiliconSeedTree - [ERROR] - silicon seed pointer is null, skip" << std::endl;
            continue;
        }
        silseed_id.push_back(seed_id);
        const auto si_pos = TrackSeedHelper::get_xyz(seed);
        silseed_x.push_back(si_pos.x());
        silseed_y.push_back(si_pos.y());
        silseed_z.push_back(si_pos.z());
        silseed_crossing.push_back(seed->get_crossing());
        if (seed->get_crossing() != SHRT_MAX)
        {
            ++nSilSeedsValidCrossing;
        }

        silseed_pt.push_back(seed->get_pt());
        silseed_eta.push_back(seed->get_eta());
        silseed_phi.push_back(seed->get_phi());
        {
            const int associated_vtx_index = find_vertex_index_for_crossing(seed_id, seed->get_crossing());
            int eta_phi_vtx_index = associated_vtx_index;
            if (eta_phi_vtx_index < 0)
            {
                const auto fallback_it = first_associated_vertex_index_by_crossing.find(seed->get_crossing());
                if (fallback_it != first_associated_vertex_index_by_crossing.end())
                {
                    eta_phi_vtx_index = fallback_it->second;
                }
            }

            if (associated_vtx_index >= 0)
            {
                silseed_assocVtxId.push_back(trackerVertexId[associated_vtx_index]);
            }
            else
            {
                silseed_assocVtxId.push_back(kUnassociatedVertexId);
            }

            if (eta_phi_vtx_index >= 0)
            {
                TVector3 seed_from_vtx(si_pos.x() - trackerVertexX[eta_phi_vtx_index], si_pos.y() - trackerVertexY[eta_phi_vtx_index], si_pos.z() - trackerVertexZ[eta_phi_vtx_index]);
                silseed_eta_vtx.push_back(seed_from_vtx.Eta());
                silseed_phi_vtx.push_back(seed_from_vtx.Phi());
            }
            else
            {
                // set to some minimum value to indicate they are not associated with any vertex
                silseed_eta_vtx.push_back(-1 * std::numeric_limits<float>::max());
                silseed_phi_vtx.push_back(-1 * std::numeric_limits<float>::max());
            }
        }
        silseed_charge.push_back((seed->get_qOverR() > 0) ? 1 : -1);
        // filling crossing seed id map for debugging
        crossing_SeedIdVertexId_map[seed->get_crossing()].push_back(std::make_pair(seed_id, silseed_assocVtxId.back()));

        std::vector<uint64_t> seed_cluskeys(seed->begin_cluster_keys(), seed->end_cluster_keys());
        silseed_clusterKeys.push_back(seed_cluskeys);
        silseed_cluster_layer.push_back(std::vector<unsigned int>());
        silseed_cluster_globalX.push_back(std::vector<float>());
        silseed_cluster_globalY.push_back(std::vector<float>());
        silseed_cluster_globalZ.push_back(std::vector<float>());
        silseed_cluster_phi.push_back(std::vector<float>());
        silseed_cluster_eta.push_back(std::vector<float>());
        silseed_cluster_r.push_back(std::vector<float>());
        silseed_cluster_phiSize.push_back(std::vector<int>());
        silseed_cluster_zSize.push_back(std::vector<int>());
        silseed_cluster_strobeID.push_back(std::vector<int>());
        silseed_cluster_timeBucketID.push_back(std::vector<int>());
        if (isSimulation)
        {
            silseed_cluster_gcluster_key.push_back(std::vector<uint64_t>());
            silseed_cluster_gcluster_layer.push_back(std::vector<unsigned int>());
            silseed_cluster_gcluster_X.push_back(std::vector<float>());
            silseed_cluster_gcluster_Y.push_back(std::vector<float>());
            silseed_cluster_gcluster_Z.push_back(std::vector<float>());
            silseed_cluster_gcluster_r.push_back(std::vector<float>());
            silseed_cluster_gcluster_phi.push_back(std::vector<float>());
            silseed_cluster_gcluster_eta.push_back(std::vector<float>());
            silseed_cluster_gcluster_edep.push_back(std::vector<float>());
            silseed_cluster_gcluster_adc.push_back(std::vector<int>());
            silseed_cluster_gcluster_phiSize.push_back(std::vector<float>());
            silseed_cluster_gcluster_zSize.push_back(std::vector<float>());
        }
        int nMvtx = 0, nIntt = 0;
        int ngMvtx = 0, ngIntt = 0;
        for (auto cluskey : seed_cluskeys)
        {
            auto *cluster = clustermap->findCluster(cluskey);

            if (!cluster)
            {
                // std::cout << "SiliconSeedAnalyzer::process_event - [ERROR] - cluster pointer is null, skip" << std::endl;
                continue;
            }

            unsigned int layer = TrkrDefs::getLayer(cluskey);
            silseed_cluster_layer[silseed_cluster_layer.size() - 1].push_back(layer);

            auto globalpos = geometry->getGlobalPosition(cluskey, cluster);
            silseed_cluster_globalX[silseed_cluster_globalX.size() - 1].push_back(globalpos.x());
            silseed_cluster_globalY[silseed_cluster_globalY.size() - 1].push_back(globalpos.y());
            silseed_cluster_globalZ[silseed_cluster_globalZ.size() - 1].push_back(globalpos.z());
            float phi = std::atan2(globalpos.y(), globalpos.x());
            silseed_cluster_phi[silseed_cluster_phi.size() - 1].push_back(phi);
            float r = (globalpos.y() > 0) ? std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()) : -std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y());
            silseed_cluster_r[silseed_cluster_r.size() - 1].push_back(r);
            TVector3 posvec(globalpos.x(), globalpos.y(), globalpos.z());
            silseed_cluster_eta[silseed_cluster_eta.size() - 1].push_back(posvec.Eta());
            float phisize = cluster->getPhiSize();
            if (phisize <= 0)
            {
                phisize += 256;
            }
            silseed_cluster_phiSize[silseed_cluster_phiSize.size() - 1].push_back(phisize);
            float zsize = cluster->getZSize();
            silseed_cluster_zSize[silseed_cluster_zSize.size() - 1].push_back(zsize);
            if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::inttId)
            {
                ++nIntt;
                silseed_cluster_strobeID[silseed_cluster_strobeID.size() - 1].push_back(std::numeric_limits<int>::min());
                silseed_cluster_timeBucketID[silseed_cluster_timeBucketID.size() - 1].push_back(InttDefs::getTimeBucketId(cluskey));
            }
            else if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::mvtxId)
            {
                ++nMvtx;
                silseed_cluster_strobeID[silseed_cluster_strobeID.size() - 1].push_back(MvtxDefs::getStrobeId(cluskey));
                silseed_cluster_timeBucketID[silseed_cluster_timeBucketID.size() - 1].push_back(std::numeric_limits<int>::min());

                // save mvtx seed cluster info
                mvtx_seedcluster_key.push_back(cluskey);
                mvtx_seedcluster_layer.push_back(layer);
                mvtx_seedcluster_chip.push_back(MvtxDefs::getChipId(cluskey));
                mvtx_seedcluster_stave.push_back(MvtxDefs::getStaveId(cluskey));
                mvtx_seedcluster_globalX.push_back(globalpos.x());
                mvtx_seedcluster_globalY.push_back(globalpos.y());
                mvtx_seedcluster_globalZ.push_back(globalpos.z());
                mvtx_seedcluster_phi.push_back(phi);
                mvtx_seedcluster_eta.push_back(posvec.Eta());
                mvtx_seedcluster_r.push_back(r);
                mvtx_seedcluster_phiSize.push_back(phisize);
                mvtx_seedcluster_zSize.push_back(zsize);
                mvtx_seedcluster_strobeID.push_back(MvtxDefs::getStrobeId(cluskey));
                mvtx_seedcluster_matchedcrossing.push_back(seed->get_crossing());
                // get hitrow and hitcol from cluster hit assoc
                if (clusterhitassoc)
                {
                    Clean(hit_x);
                    Clean(hit_y);
                    Clean(hit_z);
                    Clean(hit_rows);
                    Clean(hit_cols);

                    TrkrClusterHitAssoc::ConstRange hitrange = clusterhitassoc->getHits(cluskey);
                    for (TrkrClusterHitAssoc::ConstIterator hititer = hitrange.first; hititer != hitrange.second; ++hititer)
                    {
                        TrkrDefs::hitkey hitkey = hititer->second;

                        int hitrow = MvtxDefs::getRow(hitkey);
                        int hitcol = MvtxDefs::getCol(hitkey);
                        hit_rows.push_back(hitrow);
                        hit_cols.push_back(hitcol);

                        MvtxPixelDefs::pixelkey pixelkey = MvtxPixelDefs::gen_pixelkey_from_coors(layer, MvtxDefs::getStaveId(cluskey), MvtxDefs::getChipId(cluskey), hitrow, hitcol);
                        uint32_t hitsetkey = MvtxPixelDefs::get_hitsetkey(pixelkey);

                        float localX = std::numeric_limits<float>::quiet_NaN();
                        float localZ = std::numeric_limits<float>::quiet_NaN();
                        SegmentationAlpide::detectorToLocal(hitrow, hitcol, localX, localZ);
                        Acts::Vector2 local(localX * Acts::UnitConstants::cm, localZ * Acts::UnitConstants::cm);

                        const auto &surface = geometry->maps().getSiliconSurface(hitsetkey);
                        auto glob = surface->localToGlobal(geometry->geometry().getGeoContext(), local, Acts::Vector3());

                        hit_x.push_back(glob.x() / Acts::UnitConstants::cm);
                        hit_y.push_back(glob.y() / Acts::UnitConstants::cm);
                        hit_z.push_back(glob.z() / Acts::UnitConstants::cm);
                    }

                    mvtx_seedcluster_hitX.push_back(hit_x);
                    mvtx_seedcluster_hitY.push_back(hit_y);
                    mvtx_seedcluster_hitZ.push_back(hit_z);
                    mvtx_seedcluster_hitrow.push_back(hit_rows);
                    mvtx_seedcluster_hitcol.push_back(hit_cols);
                }
                else
                {
                    mvtx_seedcluster_hitX.push_back(std::vector<float>());
                    mvtx_seedcluster_hitY.push_back(std::vector<float>());
                    mvtx_seedcluster_hitZ.push_back(std::vector<float>());
                    mvtx_seedcluster_hitrow.push_back(std::vector<int>());
                    mvtx_seedcluster_hitcol.push_back(std::vector<int>());
                }

                // if simulation, get matched G4 particle info
                if (isSimulation)
                {
                    Clean(ancestor_trackIDs);
                    Clean(ancestor_PIDs);

                    PHG4Particle *ptcl = clustereval->max_truth_particle_by_energy(cluskey); // alternatively, can also try max_truth_particle_by_cluster_energy
                    if (ptcl)
                    {
                        mvtx_seedcluster_matchedG4P_trackID.push_back(ptcl->get_track_id());
                        mvtx_seedcluster_matchedG4P_PID.push_back(ptcl->get_pid());
                        mvtx_seedcluster_matchedG4P_E.push_back(ptcl->get_e());
                        ROOT::Math::PtEtaPhiEVector g4p4(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());
                        mvtx_seedcluster_matchedG4P_pT.push_back(g4p4.Pt());
                        mvtx_seedcluster_matchedG4P_eta.push_back(g4p4.Eta());
                        mvtx_seedcluster_matchedG4P_phi.push_back(g4p4.Phi());

                        // get ancestor info
                        PHG4Particle *ancestor = m_truth_info->GetParticle(ptcl->get_parent_id());
                        while (ancestor != nullptr)
                        {
                            ancestor_trackIDs.push_back(ancestor->get_track_id());
                            ancestor_PIDs.push_back(ancestor->get_pid());
                            ancestor = m_truth_info->GetParticle(ancestor->get_parent_id());
                        }
                        mvtx_seedcluster_matchedG4P_ancestor_trackID.push_back(ancestor_trackIDs);
                        mvtx_seedcluster_matchedG4P_ancestor_PID.push_back(ancestor_PIDs);
                    }
                    else
                    {
                        std::cout << "VertexCompare::FillSiliconSeedTree - [WARNING] - no matched G4 particle found for cluster " << cluskey << std::endl;
                        mvtx_seedcluster_matchedG4P_trackID.push_back(std::numeric_limits<int>::max());
                        mvtx_seedcluster_matchedG4P_PID.push_back(std::numeric_limits<int>::max());
                        mvtx_seedcluster_matchedG4P_E.push_back(std::numeric_limits<float>::quiet_NaN());
                        mvtx_seedcluster_matchedG4P_pT.push_back(std::numeric_limits<float>::quiet_NaN());
                        mvtx_seedcluster_matchedG4P_eta.push_back(std::numeric_limits<float>::quiet_NaN());
                        mvtx_seedcluster_matchedG4P_phi.push_back(std::numeric_limits<float>::quiet_NaN());
                        mvtx_seedcluster_matchedG4P_ancestor_trackID.push_back(std::vector<int>());
                        mvtx_seedcluster_matchedG4P_ancestor_PID.push_back(std::vector<int>());
                    }
                }
            }
            else
            {
                std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - cluster is neither INTT nor MVTX. Please check." << std::endl;
                silseed_cluster_strobeID[silseed_cluster_strobeID.size() - 1].push_back(std::numeric_limits<int>::max());
                silseed_cluster_timeBucketID[silseed_cluster_timeBucketID.size() - 1].push_back(std::numeric_limits<int>::max());
            }

            // Fill branch for simulation
            if (isSimulation)
            {
                std::pair<TrkrDefs::cluskey, std::shared_ptr<TrkrCluster>> truthclus = clustereval->max_truth_cluster_by_energy(cluskey);
                const auto truth_key = truthclus.first;
                const auto &truth_cluster = truthclus.second;
                if (truth_cluster)
                {
                    const float gx = truth_cluster->getX();
                    const float gy = truth_cluster->getY();
                    const float gz = truth_cluster->getZ();
                    TVector3 gpos(gx, gy, gz);

                    silseed_cluster_gcluster_key.back().push_back(truth_key);
                    silseed_cluster_gcluster_layer.back().push_back(TrkrDefs::getLayer(truth_key));
                    silseed_cluster_gcluster_X.back().push_back(gx);
                    silseed_cluster_gcluster_Y.back().push_back(gy);
                    silseed_cluster_gcluster_Z.back().push_back(gz);
                    silseed_cluster_gcluster_r.back().push_back(std::sqrt(gx * gx + gy * gy));
                    silseed_cluster_gcluster_phi.back().push_back(gpos.Phi());
                    silseed_cluster_gcluster_eta.back().push_back(gpos.Eta());
                    silseed_cluster_gcluster_edep.back().push_back(truth_cluster->getError(0, 0));
                    silseed_cluster_gcluster_adc.back().push_back(truth_cluster->getAdc());
                    silseed_cluster_gcluster_phiSize.back().push_back(truth_cluster->getSize(1, 1));
                    silseed_cluster_gcluster_zSize.back().push_back(truth_cluster->getSize(2, 2));

                    if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::mvtxId)
                    {
                        ++ngMvtx;
                    }
                    else if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::inttId)
                    {
                        ++ngIntt;
                    }
                }
                else
                {
                    silseed_cluster_gcluster_key.back().push_back(0);
                    silseed_cluster_gcluster_layer.back().push_back(std::numeric_limits<unsigned int>::max());
                    silseed_cluster_gcluster_X.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_Y.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_Z.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_r.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_phi.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_eta.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_edep.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_adc.back().push_back(-1 * std::numeric_limits<int>::max());
                    silseed_cluster_gcluster_phiSize.back().push_back(-1 * std::numeric_limits<float>::max());
                    silseed_cluster_gcluster_zSize.back().push_back(-1 * std::numeric_limits<float>::max());
                }
            } //
        } // end loop over clusters associated with the seed
        silseed_nMvtx.push_back(nMvtx);
        silseed_nIntt.push_back(nIntt);
        if (isSimulation)
        {
            silseed_ngmvtx.push_back(ngMvtx);
            silseed_ngintt.push_back(ngIntt);
        }

        // get F4A truth matching information if it is simulation
        if (isSimulation && doTruthMatching_)
        {
            std::vector<int> truth_track_ids;
            std::vector<float> truth_weights;
            std::vector<int> best_ancestor_track_ids;
            std::vector<int> best_ancestor_pids;

            int best_track_id = std::numeric_limits<int>::max();
            float best_weight = -1 * std::numeric_limits<float>::max();
            int best_pid = std::numeric_limits<int>::max();
            float best_e = -1 * std::numeric_limits<float>::max();
            float best_pt = -1 * std::numeric_limits<float>::max();
            float best_eta = -1 * std::numeric_limits<float>::max();
            float best_phi = -1 * std::numeric_limits<float>::max();

            const auto svtx_track_id_iter = svtxTrackIdBySiliconSeed.find(static_cast<unsigned int>(seed_id));
            if (svtx_track_id_iter != svtxTrackIdBySiliconSeed.end() && svtxPHG4ParticleMap)
            {
                const unsigned int svtx_track_id = svtx_track_id_iter->second;
                // std::cout << "Seed ID " << seed_id << " is associated with SvtxTrack ID " << svtx_track_id << std::endl;
                const auto &truth_set = svtxPHG4ParticleMap->get(svtx_track_id);

                for (auto weight_iter = truth_set.rbegin(); weight_iter != truth_set.rend(); ++weight_iter)
                {
                    const float weight = weight_iter->first;
                    const auto &truth_ids = weight_iter->second;
                    for (auto truth_id_iter = truth_ids.rbegin(); truth_id_iter != truth_ids.rend(); ++truth_id_iter)
                    {
                        truth_track_ids.push_back(*truth_id_iter);
                        truth_weights.push_back(weight);
                    }
                }

                if (!truth_track_ids.empty())
                {
                    best_weight = truth_weights.front();
                    best_track_id = truth_track_ids.front();

                    PHG4Particle *best_particle = (m_truth_info) ? m_truth_info->GetParticle(best_track_id) : nullptr;
                    if (best_particle)
                    {
                        best_pid = best_particle->get_pid();
                        best_e = best_particle->get_e();

                        ROOT::Math::PxPyPzEVector best_p4(best_particle->get_px(), best_particle->get_py(), best_particle->get_pz(), best_particle->get_e());
                        best_pt = best_p4.Pt();
                        best_eta = best_p4.Eta();
                        best_phi = best_p4.Phi();

                        PHG4Particle *ancestor = m_truth_info->GetParticle(best_particle->get_parent_id());
                        while (ancestor != nullptr)
                        {
                            best_ancestor_track_ids.push_back(ancestor->get_track_id());
                            best_ancestor_pids.push_back(ancestor->get_pid());
                            ancestor = m_truth_info->GetParticle(ancestor->get_parent_id());
                        }
                    }
                }
                // else
                // {
                //     std::cout << "VertexCompare::FillSiliconSeedTree - [WARNING] - no truth track matched for seed ID " << seed_id << std::endl;
                // }
            }

            silseed_f4a_nMatched.push_back(static_cast<int>(truth_track_ids.size()));
            silseed_f4a_truthTrackID.push_back(truth_track_ids);
            silseed_f4a_truthWeight.push_back(truth_weights);
            silseed_f4a_bestTrackID.push_back(best_track_id);
            silseed_f4a_bestWeight.push_back(best_weight);
            silseed_f4a_bestG4P_PID.push_back(best_pid);
            silseed_f4a_bestG4P_E.push_back(best_e);
            silseed_f4a_bestG4P_pT.push_back(best_pt);
            silseed_f4a_bestG4P_eta.push_back(best_eta);
            silseed_f4a_bestG4P_phi.push_back(best_phi);
            silseed_f4a_bestG4P_ancestor_trackID.push_back(best_ancestor_track_ids);
            silseed_f4a_bestG4P_ancestor_PID.push_back(best_ancestor_pids);
        }

    } // end loop over seeds
    nTotalSilSeeds = silseed_id.size();
    if (isSimulation && doTruthMatching_)
    {
        int nSilSeedsValidCrossing_noTruthMatch = 0;
        for (size_t i = 0; i < silseed_id.size(); ++i)
        {
            if (silseed_crossing[i] != SHRT_MAX && silseed_f4a_nMatched[i] < 1)
            {
                ++nSilSeedsValidCrossing_noTruthMatch;
            }
        }

        std::cout << "Total silicon seeds in this event: " << nTotalSilSeeds << " with valid crossing: " << nSilSeedsValidCrossing << " and valid crossing but no truth match: " << nSilSeedsValidCrossing_noTruthMatch << std::endl;
    }
    else if (isSimulation)
    {
        std::cout << "Total silicon seeds in this event: " << nTotalSilSeeds << " with valid crossing: " << nSilSeedsValidCrossing << std::endl;
    }
    else
    {
        std::cout << "Total silicon seeds in this event: " << nTotalSilSeeds << " with valid crossing: " << nSilSeedsValidCrossing << std::endl;
    }

    // now print out the crossing seed id map for debugging
    // std::cout << "Crossing to seed ID mapping:" << std::endl;
    // for (const auto &[crossing, seed_id_vertex_id_pairs] : crossing_SeedIdVertexId_map)
    // {
    //     std::cout << "  Crossing " << crossing << ": Seed IDs [";
    //     for (size_t i = 0; i < seed_id_vertex_id_pairs.size(); ++i)
    //     {
    //         std::cout << "(" << seed_id_vertex_id_pairs[i].first << ", " << seed_id_vertex_id_pairs[i].second << ")";
    //         if (i != seed_id_vertex_id_pairs.size() - 1)
    //         {
    //             std::cout << ", ";
    //         }
    //     }
    //     std::cout << "]" << std::endl;
    // }
}

//____________________________________________________________________________..
void VertexCompare::FillTpcSeedTree()
{
    const float kMissingFloat = std::numeric_limits<float>::quiet_NaN();

    std::vector<std::pair<TrackSeed *, unsigned int>> seeds;

    if (tpcseedmap)
    {
        for (auto iter = tpcseedmap->begin(); iter != tpcseedmap->end(); ++iter)
        {
            auto *seed = *iter;
            if (seed)
            {
                seeds.emplace_back(seed, static_cast<unsigned int>(tpcseedmap->find(seed)));
            }
        }
    }
    else if (svtxTrackMap)
    {
        std::vector<TrackSeed *> seen_seeds;
        for (auto trackIter = svtxTrackMap->begin(); trackIter != svtxTrackMap->end(); ++trackIter)
        {
            SvtxTrack *track = trackIter->second;
            TrackSeed *seed = track ? track->get_tpc_seed() : nullptr;
            if (!seed || std::find(seen_seeds.begin(), seen_seeds.end(), seed) != seen_seeds.end())
            {
                continue;
            }

            seen_seeds.push_back(seed);
            seeds.emplace_back(seed, static_cast<unsigned int>(seeds.size()));
        }
    }

    float layerThicknesses[4] = {0.0, 0.0, 0.0, 0.0};
    bool canCalculateDedx = false;
    if (clustermap && geometry && tpcgeom)
    {
        auto *inner1 = tpcgeom->GetLayerCellGeom(7);
        auto *inner2 = tpcgeom->GetLayerCellGeom(8);
        auto *middle = tpcgeom->GetLayerCellGeom(27);
        auto *outer = tpcgeom->GetLayerCellGeom(50);
        if (inner1 && inner2 && middle && outer)
        {
            layerThicknesses[0] = inner1->get_thickness();
            layerThicknesses[1] = inner2->get_thickness();
            layerThicknesses[2] = middle->get_thickness();
            layerThicknesses[3] = outer->get_thickness();
            canCalculateDedx = true;
        }
    }

    for (const auto &[seed, seed_id] : seeds)
    {
        tpcseed_id.push_back(seed_id);

        const auto seed_pos = TrackSeedHelper::get_xyz(seed);
        tpcseed_x.push_back(seed_pos.x());
        tpcseed_y.push_back(seed_pos.y());
        tpcseed_z.push_back(seed_pos.z());
        tpcseed_pt.push_back(seed->get_pt());
        tpcseed_eta.push_back(seed->get_eta());
        tpcseed_phi.push_back(seed->get_phi());
        tpcseed_crossing.push_back(seed->get_crossing());
        tpcseed_crossing_estimate.push_back(seed->get_crossing_estimate());
        tpcseed_charge.push_back((seed->get_qOverR() > 0) ? 1 : -1);
        tpcseed_dedx.push_back(canCalculateDedx ? TrackAnalysisUtils::calc_dedx(seed, clustermap, geometry, layerThicknesses) : kMissingFloat);

        std::vector<uint64_t> seed_cluskeys(seed->begin_cluster_keys(), seed->end_cluster_keys());
        tpcseed_clusterKeys.push_back(seed_cluskeys);
        tpcseed_cluster_layer.push_back(std::vector<unsigned int>());
        tpcseed_cluster_globalX.push_back(std::vector<float>());
        tpcseed_cluster_globalY.push_back(std::vector<float>());
        tpcseed_cluster_globalZ.push_back(std::vector<float>());
        tpcseed_cluster_phi.push_back(std::vector<float>());
        tpcseed_cluster_eta.push_back(std::vector<float>());
        tpcseed_cluster_r.push_back(std::vector<float>());

        int nTpc = 0;
        int nMms = 0;
        for (const auto cluskey : seed_cluskeys)
        {
            if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::tpcId)
            {
                ++nTpc;
            }
            else if (TrkrDefs::getTrkrId(cluskey) == TrkrDefs::micromegasId)
            {
                ++nMms;
            }

            auto *cluster = clustermap ? clustermap->findCluster(cluskey) : nullptr;
            if (!cluster || !geometry)
            {
                continue;
            }

            const auto globalpos = geometry->getGlobalPosition(cluskey, cluster);
            tpcseed_cluster_layer.back().push_back(TrkrDefs::getLayer(cluskey));
            tpcseed_cluster_globalX.back().push_back(globalpos.x());
            tpcseed_cluster_globalY.back().push_back(globalpos.y());
            tpcseed_cluster_globalZ.back().push_back(globalpos.z());
            tpcseed_cluster_phi.back().push_back(std::atan2(globalpos.y(), globalpos.x()));
            TVector3 posvec(globalpos.x(), globalpos.y(), globalpos.z());
            tpcseed_cluster_eta.back().push_back(posvec.Eta());
            tpcseed_cluster_r.back().push_back((globalpos.y() > 0) ? std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()) : -std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()));
        }

        tpcseed_nTpc.push_back(nTpc);
        tpcseed_nMms.push_back(nMms);
    }

    nTotalTpcSeeds = tpcseed_id.size();
    std::cout << "Total TPC seeds in this event: " << nTotalTpcSeeds << std::endl;
}

//____________________________________________________________________________..
void VertexCompare::FillClusterTree()
{
    for (const auto &det : {TrkrDefs::TrkrId::inttId})
    {
        for (const auto &hitsetkey : clustermap->getHitSetKeys(det))
        {
            auto range = clustermap->getClusters(hitsetkey);
            for (auto iter = range.first; iter != range.second; ++iter)
            {
                uint64_t key = iter->first;
                clusterKey.push_back(key);
                unsigned int layer = TrkrDefs::getLayer(key);
                cluster_layer.push_back(layer);
                auto *cluster = iter->second;
                auto globalpos = geometry->getGlobalPosition(key, cluster);
                cluster_globalX.push_back(globalpos.x());
                cluster_globalY.push_back(globalpos.y());
                cluster_globalZ.push_back(globalpos.z());
                cluster_phi.push_back(std::atan2(globalpos.y(), globalpos.x()));
                TVector3 posvec(globalpos.x(), globalpos.y(), globalpos.z());
                cluster_eta.push_back(posvec.Eta());
                cluster_r.push_back((globalpos.y() > 0) ? std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()) : -std::sqrt(globalpos.x() * globalpos.x() + globalpos.y() * globalpos.y()));
                int phiSize = cluster->getPhiSize();
                if (phiSize <= 0)
                {
                    phiSize += 256;
                }
                cluster_phiSize.push_back(phiSize);
                int zSize = cluster->getZSize();
                cluster_zSize.push_back(zSize);
                cluster_adc.push_back(cluster->getAdc());
                switch (det)
                {
                case TrkrDefs::TrkrId::mvtxId:
                    cluster_chip.push_back(MvtxDefs::getChipId(key));
                    cluster_stave.push_back(MvtxDefs::getStaveId(key));
                    cluster_timeBucketID.push_back(std::numeric_limits<int>::min());
                    cluster_LocalX.push_back(cluster->getLocalX());
                    cluster_LocalY.push_back(cluster->getLocalY());
                    break;
                case TrkrDefs::TrkrId::inttId:
                    cluster_ladderZId.push_back(InttDefs::getLadderZId(key));
                    cluster_ladderPhiId.push_back(InttDefs::getLadderPhiId(key));
                    cluster_timeBucketID.push_back(InttDefs::getTimeBucketId(key));
                    {
                        int crossing = -std::numeric_limits<int>::max();
                        if (clustercrossingassoc)
                        {
                            auto crossing_range = clustercrossingassoc->getCrossings(key);
                            if (crossing_range.first != crossing_range.second)
                            {
                                crossing = crossing_range.first->second;
                            }
                        }
                        cluster_crossing.push_back(crossing);
                    }
                    // std::cout << "INTT cluster key: " << key << ", timeBucketID: " << InttDefs::getTimeBucketId(key) << ", crossing: " << cluster_crossing.back() << std::endl;
                    cluster_LocalX.push_back(cluster->getLocalX());
                    cluster_LocalY.push_back(cluster->getLocalY());
                    break;
                default:
                    std::cout << "SiliconSeedAnalyzer::process_event - [WARNING] - cluster is neither INTT nor MVTX. Please check." << std::endl;
                    cluster_chip.push_back(std::numeric_limits<int>::max());
                    cluster_stave.push_back(std::numeric_limits<int>::max());
                    cluster_ladderZId.push_back(std::numeric_limits<int>::max());
                    cluster_ladderPhiId.push_back(std::numeric_limits<int>::max());
                    cluster_timeBucketID.push_back(std::numeric_limits<int>::max());
                    cluster_LocalX.push_back(std::numeric_limits<float>::max());
                    cluster_LocalY.push_back(std::numeric_limits<float>::max());
                    break;
                }

                if (isSimulation)
                {
                    PHG4Particle *ptcl_maxE = (clustereval) ? clustereval->max_truth_particle_by_energy(key) : nullptr;
                    if (ptcl_maxE)
                    {
                        cluster_matchedG4P_trackID.push_back(ptcl_maxE->get_track_id());
                        cluster_matchedG4P_PID.push_back(ptcl_maxE->get_pid());
                        cluster_matchedG4P_E.push_back(ptcl_maxE->get_e());

                        ROOT::Math::PxPyPzEVector g4p4(ptcl_maxE->get_px(), ptcl_maxE->get_py(), ptcl_maxE->get_pz(), ptcl_maxE->get_e());
                        cluster_matchedG4P_pT.push_back(g4p4.Pt());
                        cluster_matchedG4P_eta.push_back(g4p4.Eta());
                        cluster_matchedG4P_phi.push_back(g4p4.Phi());
                    }
                    else
                    {
                        cluster_matchedG4P_trackID.push_back(std::numeric_limits<int>::max());
                        cluster_matchedG4P_PID.push_back(std::numeric_limits<int>::max());
                        cluster_matchedG4P_E.push_back(-1 * std::numeric_limits<float>::max());
                        cluster_matchedG4P_pT.push_back(-1 * std::numeric_limits<float>::max());
                        cluster_matchedG4P_eta.push_back(-1 * std::numeric_limits<float>::max());
                        cluster_matchedG4P_phi.push_back(-1 * std::numeric_limits<float>::max());
                    }
                }
            }
        }
    }
}

//____________________________________________________________________________..
void VertexCompare::FillTruthParticleTree()
{
    if (!m_truth_info)
    {
        std::cout << "VertexCompare::FillTruthParticleTree - [WARNING] - missing G4TruthInfo, skip filling truth particle branches" << std::endl;
        return;
    }

    // truth primary vertices
    const auto vtx_range = m_truth_info->GetPrimaryVtxRange();
    for (auto iter = vtx_range.first; iter != vtx_range.second; ++iter)
    {
        const int point_id = iter->first;
        PHG4VtxPoint *point = iter->second;
        if (!point)
        {
            continue;
        }

        ++nTruthVertex;
        // if (m_truth_info->isEmbededVtx(point_id) == 0)
        {
            TruthVertex_isEmbeded.push_back(m_truth_info->isEmbededVtx(point_id));
            TruthVertexX.push_back(point->get_x());
            TruthVertexY.push_back(point->get_y());
            TruthVertexZ.push_back(point->get_z());
            TruthVertexT.push_back(point->get_t());
            TruthVertex_crossing.push_back(std::isfinite(point->get_t()) ? static_cast<int>(std::floor(point->get_t() / sphenix_constants::time_between_crossings)) : std::numeric_limits<int>::max());
        }
    }

    auto fill_ancestor_info = [this](PHG4Particle *ptcl, std::vector<int> &ancestor_trackIDs, std::vector<int> &ancestor_PIDs)
    {
        PHG4Particle *ancestor = m_truth_info->GetParticle(ptcl->get_parent_id());
        while (ancestor != nullptr)
        {
            ancestor_trackIDs.push_back(ancestor->get_track_id());
            ancestor_PIDs.push_back(ancestor->get_pid());
            ancestor = m_truth_info->GetParticle(ancestor->get_parent_id());
        }
    };

    auto fill_particle_kinematics = [](PHG4Particle *ptcl, std::vector<float> &out_pT, std::vector<float> &out_eta, std::vector<float> &out_phi, std::vector<float> &out_E, std::vector<int> &out_PID, std::vector<int> &out_trackID)
    {
        ROOT::Math::PxPyPzEVector p4(ptcl->get_px(), ptcl->get_py(), ptcl->get_pz(), ptcl->get_e());
        out_pT.push_back(p4.Pt());
        out_eta.push_back(p4.Eta());
        out_phi.push_back(p4.Phi());
        out_E.push_back(ptcl->get_e());
        out_PID.push_back(ptcl->get_pid());
        out_trackID.push_back(ptcl->get_track_id());
    };

    auto fill_origin_info = [this](PHG4Particle *ptcl, std::vector<int> &out_originTrackID, std::vector<int> &out_originVtxID, std::vector<float> &out_originVtxT, std::vector<int> &out_originCrossing, std::vector<int> &out_originIsEmbeded)
    {
        const int kMissingInt = std::numeric_limits<int>::max();
        const float kMissingFloat = std::numeric_limits<float>::quiet_NaN();

        PHG4Particle *origin = ptcl;
        PHG4Particle *parent = origin ? m_truth_info->GetParticle(origin->get_parent_id()) : nullptr;
        while (parent)
        {
            origin = parent;
            parent = m_truth_info->GetParticle(origin->get_parent_id());
        }

        if (!origin)
        {
            out_originTrackID.push_back(kMissingInt);
            out_originVtxID.push_back(kMissingInt);
            out_originVtxT.push_back(kMissingFloat);
            out_originCrossing.push_back(kMissingInt);
            out_originIsEmbeded.push_back(kMissingInt);
            return;
        }

        const int origin_track_id = origin->get_track_id();
        const int origin_vtx_id = origin->get_vtx_id();
        PHG4VtxPoint *origin_vtx = m_truth_info->GetVtx(origin_vtx_id);
        const float origin_vtx_t = origin_vtx ? origin_vtx->get_t() : kMissingFloat;
        const int origin_crossing = std::isfinite(origin_vtx_t) ? static_cast<int>(std::floor(origin_vtx_t / sphenix_constants::time_between_crossings)) : kMissingInt;

        out_originTrackID.push_back(origin_track_id);
        out_originVtxID.push_back(origin_vtx_id);
        out_originVtxT.push_back(origin_vtx_t);
        out_originCrossing.push_back(origin_crossing);
        out_originIsEmbeded.push_back(m_truth_info->isEmbeded(origin_track_id));
    };

    auto fill_primary_particle_info = [](PHG4Particle *ptcl, std::vector<TString> &out_particleClass, std::vector<bool> &out_isStable, std::vector<double> &out_charge, std::vector<bool> &out_isChargedHadron)
    {
        TString particleClass = "Unknown";
        bool isStable = false;
        double charge = 0;

        TParticlePDG *pdg = TDatabasePDG::Instance()->GetParticle(ptcl->get_pid());
        if (pdg)
        {
            particleClass = TString(pdg->ParticleClass());
            isStable = (pdg->Stable() == 1);
            charge = pdg->Charge();
        }

        bool isHadron = (particleClass.Contains("Baryon") || particleClass.Contains("Meson"));
        bool isChargedHadron = (isStable && (charge != 0) && isHadron);

        out_particleClass.push_back(particleClass);
        out_isStable.push_back(isStable);
        out_charge.push_back(charge);
        out_isChargedHadron.push_back(isChargedHadron);
    };

    auto fill_truthcluster_match_info = [this](PHG4Particle *ptcl,                                        //
                                               std::vector<std::vector<float>> &out_truthcluster_X,       //
                                               std::vector<std::vector<float>> &out_truthcluster_Y,       //
                                               std::vector<std::vector<float>> &out_truthcluster_Z,       //
                                               std::vector<std::vector<float>> &out_truthcluster_edep,    //
                                               std::vector<std::vector<float>> &out_truthcluster_adc,     //
                                               std::vector<std::vector<float>> &out_truthcluster_r,       //
                                               std::vector<std::vector<float>> &out_truthcluster_phi,     //
                                               std::vector<std::vector<float>> &out_truthcluster_eta,     //
                                               std::vector<std::vector<float>> &out_truthcluster_phisize, //
                                               std::vector<std::vector<float>> &out_truthcluster_zsize,   //
                                               std::vector<std::vector<float>> &out_recocluster_globalX,  //
                                               std::vector<std::vector<float>> &out_recocluster_globalY,  //
                                               std::vector<std::vector<float>> &out_recocluster_globalZ,  //
                                               std::vector<std::vector<float>> &out_recocluster_r,        //
                                               std::vector<std::vector<float>> &out_recocluster_phi,      //
                                               std::vector<std::vector<float>> &out_recocluster_eta,      //
                                               std::vector<std::vector<float>> &out_recocluster_phisize,  //
                                               std::vector<std::vector<float>> &out_recocluster_zsize,    //
                                               std::vector<std::vector<float>> &out_recocluster_adc       //
                                        )
    {
        std::vector<float> truthcluster_X;
        std::vector<float> truthcluster_Y;
        std::vector<float> truthcluster_Z;
        std::vector<float> truthcluster_edep;
        std::vector<float> truthcluster_adc;
        std::vector<float> truthcluster_r;
        std::vector<float> truthcluster_phi;
        std::vector<float> truthcluster_eta;
        std::vector<float> truthcluster_phisize;
        std::vector<float> truthcluster_zsize;
        std::vector<float> recocluster_globalX;
        std::vector<float> recocluster_globalY;
        std::vector<float> recocluster_globalZ;
        std::vector<float> recocluster_r;
        std::vector<float> recocluster_phi;
        std::vector<float> recocluster_eta;
        std::vector<float> recocluster_phisize;
        std::vector<float> recocluster_zsize;
        std::vector<float> recocluster_adc;

        if (truth_eval && clustereval)
        {
            const auto truth_clusters = truth_eval->all_truth_clusters(ptcl);

            for (const auto &[ckey, gclus] : truth_clusters)
            {
                if (!gclus)
                {
                    continue;
                }

                // keep tracker truth clusters that have analysis branches below
                if (TrkrDefs::getTrkrId(ckey) != TrkrDefs::mvtxId && TrkrDefs::getTrkrId(ckey) != TrkrDefs::inttId && TrkrDefs::getTrkrId(ckey) != TrkrDefs::tpcId && TrkrDefs::getTrkrId(ckey) != TrkrDefs::micromegasId)
                {
                    continue;
                }

                const float gx = gclus->getX();
                const float gy = gclus->getY();
                const float gz = gclus->getZ();
                const float gedep = gclus->getError(0, 0);
                const float gadc = static_cast<float>(gclus->getAdc());

                TVector3 gpos(gx, gy, gz);
                truthcluster_X.push_back(gx);
                truthcluster_Y.push_back(gy);
                truthcluster_Z.push_back(gz);
                truthcluster_edep.push_back(gedep);
                truthcluster_adc.push_back(gadc);
                truthcluster_r.push_back(std::sqrt(gx * gx + gy * gy));
                truthcluster_phi.push_back(gpos.Phi());
                truthcluster_eta.push_back(gpos.Eta());
                truthcluster_phisize.push_back(gclus->getSize(1, 1));
                truthcluster_zsize.push_back(gclus->getSize(2, 2));

                float x = std::numeric_limits<float>::quiet_NaN();
                float y = std::numeric_limits<float>::quiet_NaN();
                float z = std::numeric_limits<float>::quiet_NaN();
                float r = std::numeric_limits<float>::quiet_NaN();
                float phi = std::numeric_limits<float>::quiet_NaN();
                float eta = std::numeric_limits<float>::quiet_NaN();
                float phisize = std::numeric_limits<float>::quiet_NaN();
                float zsize = std::numeric_limits<float>::quiet_NaN();
                float adc = std::numeric_limits<float>::quiet_NaN();

                const auto [reco_ckey, reco_cluster] = clustereval->reco_cluster_from_truth_cluster(ckey, gclus);
                if (reco_cluster)
                {
                    const auto global = geometry->getGlobalPosition(reco_ckey, reco_cluster);
                    x = global.x();
                    y = global.y();
                    z = global.z();

                    TVector3 reco_pos(x, y, z);
                    r = std::sqrt(x * x + y * y);
                    phi = reco_pos.Phi();
                    eta = reco_pos.Eta();
                    phisize = reco_cluster->getPhiSize();
                    zsize = reco_cluster->getZSize();
                    adc = static_cast<float>(reco_cluster->getAdc());
                }

                recocluster_globalX.push_back(x);
                recocluster_globalY.push_back(y);
                recocluster_globalZ.push_back(z);
                recocluster_r.push_back(r);
                recocluster_phi.push_back(phi);
                recocluster_eta.push_back(eta);
                recocluster_phisize.push_back(phisize);
                recocluster_zsize.push_back(zsize);
                recocluster_adc.push_back(adc);
            }
        }

        out_truthcluster_X.push_back(truthcluster_X);
        out_truthcluster_Y.push_back(truthcluster_Y);
        out_truthcluster_Z.push_back(truthcluster_Z);
        out_truthcluster_edep.push_back(truthcluster_edep);
        out_truthcluster_adc.push_back(truthcluster_adc);
        out_truthcluster_r.push_back(truthcluster_r);
        out_truthcluster_phi.push_back(truthcluster_phi);
        out_truthcluster_eta.push_back(truthcluster_eta);
        out_truthcluster_phisize.push_back(truthcluster_phisize);
        out_truthcluster_zsize.push_back(truthcluster_zsize);
        out_recocluster_globalX.push_back(recocluster_globalX);
        out_recocluster_globalY.push_back(recocluster_globalY);
        out_recocluster_globalZ.push_back(recocluster_globalZ);
        out_recocluster_r.push_back(recocluster_r);
        out_recocluster_phi.push_back(recocluster_phi);
        out_recocluster_eta.push_back(recocluster_eta);
        out_recocluster_phisize.push_back(recocluster_phisize);
        out_recocluster_zsize.push_back(recocluster_zsize);
        out_recocluster_adc.push_back(recocluster_adc);
    };

    std::vector<int> ancestor_trackIDs;
    std::vector<int> ancestor_PIDs;

    // primary PHG4 particles
    const auto primary_range = m_truth_info->GetPrimaryParticleRange();
    for (auto iter = primary_range.first; iter != primary_range.second; ++iter)
    {
        PHG4Particle *ptcl = iter->second;
        if (!ptcl)
        {
            continue;
        }

        Clean(ancestor_trackIDs);
        Clean(ancestor_PIDs);
        fill_particle_kinematics(ptcl, PrimaryPHG4Ptcl_pT, PrimaryPHG4Ptcl_eta, PrimaryPHG4Ptcl_phi, PrimaryPHG4Ptcl_E, PrimaryPHG4Ptcl_PID, PrimaryPHG4Ptcl_trackID);
        fill_origin_info(ptcl, PrimaryPHG4Ptcl_originTrackID, PrimaryPHG4Ptcl_originVtxID, PrimaryPHG4Ptcl_originVtxT, PrimaryPHG4Ptcl_originCrossing, PrimaryPHG4Ptcl_originIsEmbeded);
        fill_primary_particle_info(ptcl, PrimaryPHG4Ptcl_ParticleClass, PrimaryPHG4Ptcl_isStable, PrimaryPHG4Ptcl_charge, PrimaryPHG4Ptcl_isChargedHadron);
        fill_ancestor_info(ptcl, ancestor_trackIDs, ancestor_PIDs);
        PrimaryPHG4Ptcl_ancestor_trackID.push_back(ancestor_trackIDs);
        PrimaryPHG4Ptcl_ancestor_PID.push_back(ancestor_PIDs);
        fill_truthcluster_match_info(ptcl, PrimaryPHG4Ptcl_truthcluster_X, PrimaryPHG4Ptcl_truthcluster_Y, PrimaryPHG4Ptcl_truthcluster_Z, PrimaryPHG4Ptcl_truthcluster_edep, PrimaryPHG4Ptcl_truthcluster_adc, PrimaryPHG4Ptcl_truthcluster_r, PrimaryPHG4Ptcl_truthcluster_phi, PrimaryPHG4Ptcl_truthcluster_eta, PrimaryPHG4Ptcl_truthcluster_phisize, PrimaryPHG4Ptcl_truthcluster_zsize,
                                     PrimaryPHG4Ptcl_recocluster_globalX, PrimaryPHG4Ptcl_recocluster_globalY, PrimaryPHG4Ptcl_recocluster_globalZ, PrimaryPHG4Ptcl_recocluster_r, PrimaryPHG4Ptcl_recocluster_phi, PrimaryPHG4Ptcl_recocluster_eta, PrimaryPHG4Ptcl_recocluster_phisize, PrimaryPHG4Ptcl_recocluster_zsize, PrimaryPHG4Ptcl_recocluster_adc);
    }
    // clean ancestor info vectors before reusing for sPHENIX primary particles
    Clean(ancestor_trackIDs);
    Clean(ancestor_PIDs);

    const auto sPHENIXprimary_particle_range = m_truth_info->GetSPHENIXPrimaryParticleRange();
    if (VertexCompareVerbosity::fillTruthParticle > 5)
    {
        std::cout << "Number of sPHENIX primary particles: " << std::distance(sPHENIXprimary_particle_range.first, sPHENIXprimary_particle_range.second) << std::endl;
    }
    for (auto iter = sPHENIXprimary_particle_range.first; iter != sPHENIXprimary_particle_range.second; ++iter)
    {
        PHG4Particle *ptcl = iter->second;
        if (!ptcl)
        {
            continue;
        }
        // get the particle in truth info container with the same track ID
        PHG4Particle *real_ptcl = m_truth_info->GetParticle(ptcl->get_track_id());

        Clean(ancestor_trackIDs);
        Clean(ancestor_PIDs);
        fill_particle_kinematics(real_ptcl, sPHENIXPrimary_pT, sPHENIXPrimary_eta, sPHENIXPrimary_phi, sPHENIXPrimary_E, sPHENIXPrimary_PID, sPHENIXPrimary_trackID);
        fill_origin_info(real_ptcl, sPHENIXPrimary_originTrackID, sPHENIXPrimary_originVtxID, sPHENIXPrimary_originVtxT, sPHENIXPrimary_originCrossing, sPHENIXPrimary_originIsEmbeded);
        fill_primary_particle_info(real_ptcl, sPHENIXPrimary_ParticleClass, sPHENIXPrimary_isStable, sPHENIXPrimary_charge, sPHENIXPrimary_isChargedHadron);
        fill_ancestor_info(real_ptcl, ancestor_trackIDs, ancestor_PIDs);
        sPHENIXPrimary_ancestor_trackID.push_back(ancestor_trackIDs);
        sPHENIXPrimary_ancestor_PID.push_back(ancestor_PIDs);
        fill_truthcluster_match_info(real_ptcl, sPHENIXPrimary_truthcluster_X, sPHENIXPrimary_truthcluster_Y, sPHENIXPrimary_truthcluster_Z, sPHENIXPrimary_truthcluster_edep, sPHENIXPrimary_truthcluster_adc, sPHENIXPrimary_truthcluster_r, sPHENIXPrimary_truthcluster_phi, sPHENIXPrimary_truthcluster_eta, sPHENIXPrimary_truthcluster_phisize, sPHENIXPrimary_truthcluster_zsize,
                                     sPHENIXPrimary_recocluster_globalX, sPHENIXPrimary_recocluster_globalY, sPHENIXPrimary_recocluster_globalZ, sPHENIXPrimary_recocluster_r, sPHENIXPrimary_recocluster_phi, sPHENIXPrimary_recocluster_eta, sPHENIXPrimary_recocluster_phisize, sPHENIXPrimary_recocluster_zsize, sPHENIXPrimary_recocluster_adc);

        // print out the truth particle info and how many truth and reco clusters are matched for debugging
        if (VertexCompareVerbosity::fillTruthParticle > 5)
        {
            std::cout << "sPHENIX Primary Particle - trackID: " << real_ptcl->get_track_id() << ", PID: " << real_ptcl->get_pid() << ", pT: " << sPHENIXPrimary_pT.back() << ", eta: " << sPHENIXPrimary_eta.back() << ", phi: " << sPHENIXPrimary_phi.back() << ", E: " << sPHENIXPrimary_E.back() << std::endl;
            std::cout << "  Matched truth clusters: " << sPHENIXPrimary_truthcluster_X.back().size() << std::endl;
            std::cout << "  Matched reco clusters: " << sPHENIXPrimary_recocluster_globalX.back().size() << std::endl;
            // flag a particle if it has more reco clusters than truth clusters
            if (sPHENIXPrimary_recocluster_globalX.back().size() > sPHENIXPrimary_truthcluster_X.back().size())
            {
                std::cout << "  *** Particle has more reco clusters than truth clusters ***" << std::endl;
            }
        }
    }
    // again clean it before reusing for all PHG4 particles
    Clean(ancestor_trackIDs);
    Clean(ancestor_PIDs);

    // all PHG4 particles
    const auto all_particle_range = m_truth_info->GetParticleRange();
    for (auto iter = all_particle_range.first; iter != all_particle_range.second; ++iter)
    {
        PHG4Particle *ptcl = iter->second;
        if (!ptcl)
        {
            continue;
        }

        Clean(ancestor_trackIDs);
        Clean(ancestor_PIDs);
        fill_particle_kinematics(ptcl, AllPHG4Ptcl_pT, AllPHG4Ptcl_eta, AllPHG4Ptcl_phi, AllPHG4Ptcl_E, AllPHG4Ptcl_PID, AllPHG4Ptcl_trackID);
        fill_origin_info(ptcl, AllPHG4Ptcl_originTrackID, AllPHG4Ptcl_originVtxID, AllPHG4Ptcl_originVtxT, AllPHG4Ptcl_originCrossing, AllPHG4Ptcl_originIsEmbeded);
        fill_ancestor_info(ptcl, ancestor_trackIDs, ancestor_PIDs);
        AllPHG4Ptcl_ancestor_trackID.push_back(ancestor_trackIDs);
        AllPHG4Ptcl_ancestor_PID.push_back(ancestor_PIDs);
        fill_truthcluster_match_info(ptcl, AllPHG4Ptcl_truthcluster_X, AllPHG4Ptcl_truthcluster_Y, AllPHG4Ptcl_truthcluster_Z, AllPHG4Ptcl_truthcluster_edep, AllPHG4Ptcl_truthcluster_adc, AllPHG4Ptcl_truthcluster_r, AllPHG4Ptcl_truthcluster_phi, AllPHG4Ptcl_truthcluster_eta, AllPHG4Ptcl_truthcluster_phisize, AllPHG4Ptcl_truthcluster_zsize, AllPHG4Ptcl_recocluster_globalX,
                                     AllPHG4Ptcl_recocluster_globalY, AllPHG4Ptcl_recocluster_globalZ, AllPHG4Ptcl_recocluster_r, AllPHG4Ptcl_recocluster_phi, AllPHG4Ptcl_recocluster_eta, AllPHG4Ptcl_recocluster_phisize, AllPHG4Ptcl_recocluster_zsize, AllPHG4Ptcl_recocluster_adc);
    }

    N_PrimaryPHG4Ptcl = PrimaryPHG4Ptcl_trackID.size();
    N_sPHENIXPrimary = sPHENIXPrimary_trackID.size();
    N_AllPHG4Ptcl = AllPHG4Ptcl_trackID.size();
    // final clean up of ancestor info vectors
    Clean(ancestor_trackIDs);
    Clean(ancestor_PIDs);
}

//____________________________________________________________________________..
void VertexCompare::Cleanup()
{
    nTotalSilSeeds = 0;
    nSilSeedsValidCrossing = 0;
    MBD_charge_sum = 0;
    N_PrimaryPHG4Ptcl = 0;
    N_sPHENIXPrimary = 0;
    N_AllPHG4Ptcl = 0;
    nTruthVertex = 0;
    hasSvtxPHG4ParticleMap = false;
    svtxPHG4ParticleMapProcessed = false;
    nRecoTracks = 0;
    Clean(track_deltapt);
    Clean(track_deltaeta);
    Clean(track_deltaphi);
    Clean(track_nhits);
    Clean(track_nmaps);
    Clean(track_nintt);
    Clean(track_ntpc);
    Clean(track_nmms);
    Clean(track_ntpc1);
    Clean(track_ntpc11);
    Clean(track_ntpc2);
    Clean(track_ntpc3);
    Clean(track_pidedx);
    Clean(track_kdedx);
    Clean(track_prdedx);
    Clean(track_vx);
    Clean(track_vy);
    Clean(track_vz);
    Clean(track_dca2d);
    Clean(track_dca2dsigma);
    Clean(track_dca3dxy);
    Clean(track_dca3dxysigma);
    Clean(track_dca3dz);
    Clean(track_dca3dzsigma);
    Clean(track_hlxpt);
    Clean(track_hlxeta);
    Clean(track_hlxphi);
    Clean(track_hlxX0);
    Clean(track_hlxY0);
    Clean(track_hlxZ0);
    Clean(track_hlxcharge);
    Clean(track_id);
    Clean(track_x);
    Clean(track_y);
    Clean(track_z);
    Clean(track_px);
    Clean(track_py);
    Clean(track_pz);
    Clean(track_pt);
    Clean(track_eta);
    Clean(track_phi);
    Clean(track_dedx);
    Clean(track_charge);
    Clean(track_crossing);
    Clean(track_vertex_id);
    Clean(track_chisq);
    Clean(track_ndf);
    Clean(track_quality);
    Clean(track_silseed_id);
    Clean(track_silseed_x);
    Clean(track_silseed_y);
    Clean(track_silseed_z);
    Clean(track_silseed_pt);
    Clean(track_silseed_eta);
    Clean(track_silseed_phi);
    Clean(track_silseed_crossing);
    Clean(track_silseed_charge);
    Clean(track_silseed_nMvtx);
    Clean(track_silseed_nIntt);
    Clean(track_silseed_clusterKeys);
    Clean(track_cluster_layer);
    Clean(track_cluster_globalX);
    Clean(track_cluster_globalY);
    Clean(track_cluster_globalZ);
    Clean(track_cluster_phi);
    Clean(track_cluster_eta);
    Clean(track_cluster_r);

    Clean(firedTriggers);
    Clean(TruthVertex_isEmbeded);
    Clean(TruthVertexX);
    Clean(TruthVertexY);
    Clean(TruthVertexZ);
    Clean(TruthVertexT);
    Clean(TruthVertex_crossing);

    Clean(mbdVertex);
    Clean(mbdVertexId);
    Clean(mbdVertexCrossing);

    Clean(trackerVertexId);
    Clean(trackerVertexX);
    Clean(trackerVertexY);
    Clean(trackerVertexZ);
    Clean(trackerVertexChisq);
    Clean(trackerVertexNdof);
    Clean(trackerVertexNTracks);
    Clean(trackerVertexCrossing);
    Clean(trackerVertexTrackIDs);

    Clean(silseed_id);
    Clean(silseed_assocVtxId);
    Clean(silseed_x);
    Clean(silseed_y);
    Clean(silseed_z);
    Clean(silseed_pt);
    Clean(silseed_eta);
    Clean(silseed_phi);
    Clean(silseed_eta_vtx);
    Clean(silseed_phi_vtx);
    Clean(silseed_crossing);
    Clean(silseed_charge);
    Clean(silseed_nMvtx);
    Clean(silseed_nIntt);
    Clean(silseed_clusterKeys);
    Clean(silseed_cluster_layer);
    Clean(silseed_cluster_globalX);
    Clean(silseed_cluster_globalY);
    Clean(silseed_cluster_globalZ);
    Clean(silseed_cluster_phi);
    Clean(silseed_cluster_eta);
    Clean(silseed_cluster_r);
    Clean(silseed_cluster_phiSize);
    Clean(silseed_cluster_zSize);
    Clean(silseed_cluster_strobeID);
    Clean(silseed_cluster_timeBucketID);
    Clean(silseed_ngmvtx);
    Clean(silseed_ngintt);
    Clean(silseed_cluster_gcluster_key);
    Clean(silseed_cluster_gcluster_layer);
    Clean(silseed_cluster_gcluster_X);
    Clean(silseed_cluster_gcluster_Y);
    Clean(silseed_cluster_gcluster_Z);
    Clean(silseed_cluster_gcluster_r);
    Clean(silseed_cluster_gcluster_phi);
    Clean(silseed_cluster_gcluster_eta);
    Clean(silseed_cluster_gcluster_edep);
    Clean(silseed_cluster_gcluster_adc);
    Clean(silseed_cluster_gcluster_phiSize);
    Clean(silseed_cluster_gcluster_zSize);
    Clean(silseed_f4a_nMatched);
    Clean(silseed_f4a_truthTrackID);
    Clean(silseed_f4a_truthWeight);
    Clean(silseed_f4a_bestTrackID);
    Clean(silseed_f4a_bestWeight);
    Clean(silseed_f4a_bestG4P_PID);
    Clean(silseed_f4a_bestG4P_E);
    Clean(silseed_f4a_bestG4P_pT);
    Clean(silseed_f4a_bestG4P_eta);
    Clean(silseed_f4a_bestG4P_phi);
    Clean(silseed_f4a_bestG4P_ancestor_trackID);
    Clean(silseed_f4a_bestG4P_ancestor_PID);

    nTotalTpcSeeds = 0;
    Clean(tpcseed_id);
    Clean(tpcseed_x);
    Clean(tpcseed_y);
    Clean(tpcseed_z);
    Clean(tpcseed_pt);
    Clean(tpcseed_eta);
    Clean(tpcseed_phi);
    Clean(tpcseed_crossing);
    Clean(tpcseed_crossing_estimate);
    Clean(tpcseed_charge);
    Clean(tpcseed_nTpc);
    Clean(tpcseed_nMms);
    Clean(tpcseed_dedx);
    Clean(tpcseed_clusterKeys);
    Clean(tpcseed_cluster_layer);
    Clean(tpcseed_cluster_globalX);
    Clean(tpcseed_cluster_globalY);
    Clean(tpcseed_cluster_globalZ);
    Clean(tpcseed_cluster_phi);
    Clean(tpcseed_cluster_eta);
    Clean(tpcseed_cluster_r);

    Clean(clusterKey);
    Clean(cluster_layer);
    Clean(cluster_chip);
    Clean(cluster_stave);
    Clean(cluster_globalX);
    Clean(cluster_globalY);
    Clean(cluster_globalZ);
    Clean(cluster_phi);
    Clean(cluster_eta);
    Clean(cluster_r);
    Clean(cluster_phiSize);
    Clean(cluster_zSize);
    Clean(cluster_adc);
    Clean(cluster_timeBucketID);
    Clean(cluster_crossing);
    Clean(cluster_ladderZId);
    Clean(cluster_ladderPhiId);
    Clean(cluster_LocalX);
    Clean(cluster_LocalY);
    Clean(cluster_matchedG4P_trackID);
    Clean(cluster_matchedG4P_PID);
    Clean(cluster_matchedG4P_E);
    Clean(cluster_matchedG4P_pT);
    Clean(cluster_matchedG4P_eta);
    Clean(cluster_matchedG4P_phi);

    Clean(mvtx_seedcluster_key);
    Clean(mvtx_seedcluster_layer);
    Clean(mvtx_seedcluster_chip);
    Clean(mvtx_seedcluster_stave);
    Clean(mvtx_seedcluster_globalX);
    Clean(mvtx_seedcluster_globalY);
    Clean(mvtx_seedcluster_globalZ);
    Clean(mvtx_seedcluster_phi);
    Clean(mvtx_seedcluster_eta);
    Clean(mvtx_seedcluster_r);
    Clean(mvtx_seedcluster_phiSize);
    Clean(mvtx_seedcluster_zSize);
    Clean(mvtx_seedcluster_strobeID);
    Clean(mvtx_seedcluster_matchedcrossing);
    Clean(mvtx_seedcluster_hitX);
    Clean(mvtx_seedcluster_hitY);
    Clean(mvtx_seedcluster_hitZ);
    Clean(mvtx_seedcluster_hitrow);
    Clean(mvtx_seedcluster_hitcol);

    Clean(mvtx_seedcluster_matchedG4P_trackID);
    Clean(mvtx_seedcluster_matchedG4P_PID);
    Clean(mvtx_seedcluster_matchedG4P_E);
    Clean(mvtx_seedcluster_matchedG4P_pT);
    Clean(mvtx_seedcluster_matchedG4P_eta);
    Clean(mvtx_seedcluster_matchedG4P_phi);
    Clean(mvtx_seedcluster_matchedG4P_ancestor_trackID);
    Clean(mvtx_seedcluster_matchedG4P_ancestor_PID);

    Clean(PrimaryPHG4Ptcl_pT);
    Clean(PrimaryPHG4Ptcl_eta);
    Clean(PrimaryPHG4Ptcl_phi);
    Clean(PrimaryPHG4Ptcl_E);
    Clean(PrimaryPHG4Ptcl_PID);
    Clean(PrimaryPHG4Ptcl_trackID);
    Clean(PrimaryPHG4Ptcl_originTrackID);
    Clean(PrimaryPHG4Ptcl_originVtxID);
    Clean(PrimaryPHG4Ptcl_originVtxT);
    Clean(PrimaryPHG4Ptcl_originCrossing);
    Clean(PrimaryPHG4Ptcl_originIsEmbeded);
    Clean(PrimaryPHG4Ptcl_ParticleClass);
    Clean(PrimaryPHG4Ptcl_isStable);
    Clean(PrimaryPHG4Ptcl_charge);
    Clean(PrimaryPHG4Ptcl_isChargedHadron);
    Clean(PrimaryPHG4Ptcl_ancestor_trackID);
    Clean(PrimaryPHG4Ptcl_ancestor_PID);
    Clean(PrimaryPHG4Ptcl_truthcluster_X);
    Clean(PrimaryPHG4Ptcl_truthcluster_Y);
    Clean(PrimaryPHG4Ptcl_truthcluster_Z);
    Clean(PrimaryPHG4Ptcl_truthcluster_edep);
    Clean(PrimaryPHG4Ptcl_truthcluster_adc);
    Clean(PrimaryPHG4Ptcl_truthcluster_r);
    Clean(PrimaryPHG4Ptcl_truthcluster_phi);
    Clean(PrimaryPHG4Ptcl_truthcluster_eta);
    Clean(PrimaryPHG4Ptcl_truthcluster_phisize);
    Clean(PrimaryPHG4Ptcl_truthcluster_zsize);
    Clean(PrimaryPHG4Ptcl_recocluster_globalX);
    Clean(PrimaryPHG4Ptcl_recocluster_globalY);
    Clean(PrimaryPHG4Ptcl_recocluster_globalZ);
    Clean(PrimaryPHG4Ptcl_recocluster_r);
    Clean(PrimaryPHG4Ptcl_recocluster_phi);
    Clean(PrimaryPHG4Ptcl_recocluster_eta);
    Clean(PrimaryPHG4Ptcl_recocluster_phisize);
    Clean(PrimaryPHG4Ptcl_recocluster_zsize);
    Clean(PrimaryPHG4Ptcl_recocluster_adc);

    Clean(sPHENIXPrimary_pT);
    Clean(sPHENIXPrimary_eta);
    Clean(sPHENIXPrimary_phi);
    Clean(sPHENIXPrimary_E);
    Clean(sPHENIXPrimary_PID);
    Clean(sPHENIXPrimary_trackID);
    Clean(sPHENIXPrimary_originTrackID);
    Clean(sPHENIXPrimary_originVtxID);
    Clean(sPHENIXPrimary_originVtxT);
    Clean(sPHENIXPrimary_originCrossing);
    Clean(sPHENIXPrimary_originIsEmbeded);
    Clean(sPHENIXPrimary_ParticleClass);
    Clean(sPHENIXPrimary_isStable);
    Clean(sPHENIXPrimary_charge);
    Clean(sPHENIXPrimary_isChargedHadron);
    Clean(sPHENIXPrimary_ancestor_trackID);
    Clean(sPHENIXPrimary_ancestor_PID);
    Clean(sPHENIXPrimary_truthcluster_X);
    Clean(sPHENIXPrimary_truthcluster_Y);
    Clean(sPHENIXPrimary_truthcluster_Z);
    Clean(sPHENIXPrimary_truthcluster_edep);
    Clean(sPHENIXPrimary_truthcluster_adc);
    Clean(sPHENIXPrimary_truthcluster_r);
    Clean(sPHENIXPrimary_truthcluster_phi);
    Clean(sPHENIXPrimary_truthcluster_eta);
    Clean(sPHENIXPrimary_truthcluster_phisize);
    Clean(sPHENIXPrimary_truthcluster_zsize);
    Clean(sPHENIXPrimary_recocluster_globalX);
    Clean(sPHENIXPrimary_recocluster_globalY);
    Clean(sPHENIXPrimary_recocluster_globalZ);
    Clean(sPHENIXPrimary_recocluster_r);
    Clean(sPHENIXPrimary_recocluster_phi);
    Clean(sPHENIXPrimary_recocluster_eta);
    Clean(sPHENIXPrimary_recocluster_phisize);
    Clean(sPHENIXPrimary_recocluster_zsize);
    Clean(sPHENIXPrimary_recocluster_adc);

    Clean(AllPHG4Ptcl_pT);
    Clean(AllPHG4Ptcl_eta);
    Clean(AllPHG4Ptcl_phi);
    Clean(AllPHG4Ptcl_E);
    Clean(AllPHG4Ptcl_PID);
    Clean(AllPHG4Ptcl_trackID);
    Clean(AllPHG4Ptcl_originTrackID);
    Clean(AllPHG4Ptcl_originVtxID);
    Clean(AllPHG4Ptcl_originVtxT);
    Clean(AllPHG4Ptcl_originCrossing);
    Clean(AllPHG4Ptcl_originIsEmbeded);
    Clean(AllPHG4Ptcl_ancestor_trackID);
    Clean(AllPHG4Ptcl_ancestor_PID);
    Clean(AllPHG4Ptcl_truthcluster_X);
    Clean(AllPHG4Ptcl_truthcluster_Y);
    Clean(AllPHG4Ptcl_truthcluster_Z);
    Clean(AllPHG4Ptcl_truthcluster_edep);
    Clean(AllPHG4Ptcl_truthcluster_adc);
    Clean(AllPHG4Ptcl_truthcluster_r);
    Clean(AllPHG4Ptcl_truthcluster_phi);
    Clean(AllPHG4Ptcl_truthcluster_eta);
    Clean(AllPHG4Ptcl_truthcluster_phisize);
    Clean(AllPHG4Ptcl_truthcluster_zsize);
    Clean(AllPHG4Ptcl_recocluster_globalX);
    Clean(AllPHG4Ptcl_recocluster_globalY);
    Clean(AllPHG4Ptcl_recocluster_globalZ);
    Clean(AllPHG4Ptcl_recocluster_r);
    Clean(AllPHG4Ptcl_recocluster_phi);
    Clean(AllPHG4Ptcl_recocluster_eta);
    Clean(AllPHG4Ptcl_recocluster_phisize);
    Clean(AllPHG4Ptcl_recocluster_zsize);
    Clean(AllPHG4Ptcl_recocluster_adc);
}

//____________________________________________________________________________..
int VertexCompare::ResetEvent(PHCompositeNode *topNode) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
int VertexCompare::EndRun(const int runnumber) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
int VertexCompare::End(PHCompositeNode *topNode)
{
    if (outFile)
    {
        outFile->cd();
        outTree->Write("", TObject::kOverwrite);
        outFile->Close();
        delete outFile;
        outFile = nullptr;
        outTree = nullptr;
    }

    delete svtx_evalstack;
    svtx_evalstack = nullptr;
    clustereval = nullptr;
    hiteval = nullptr;
    truth_eval = nullptr;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int VertexCompare::Reset(PHCompositeNode *topNode) { return Fun4AllReturnCodes::EVENT_OK; }

//____________________________________________________________________________..
void VertexCompare::Print(const std::string &what) const {}
