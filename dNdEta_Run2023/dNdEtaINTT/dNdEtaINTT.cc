//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in dNdEtaINTT.h.
//
// dNdEtaINTT(const std::string &name = "dNdEtaINTT")
// everything is keyed to dNdEtaINTT, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// dNdEtaINTT::~dNdEtaINTT()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the node tree
//
// int dNdEtaINTT::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer. You
// can create historgrams here or put objects on the node tree but be aware that
// modules which haven't been registered yet did not put antyhing on the node tree
//
// int dNdEtaINTT::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's action
// depends on what else is around. Last chance to put nodes under the DST Node
// We mix events during readback if branches are added after the first event
//
// int dNdEtaINTT::process_event(PHCompositeNode *topNode)
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
// int dNdEtaINTT::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs clearing
// after each event, this is the place to do that. The nodes under the DST node are cleared
// by the framework
//
// int dNdEtaINTT::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int dNdEtaINTT::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int dNdEtaINTT::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void dNdEtaINTT::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "dNdEtaINTT.h"

#include <fun4all/Fun4AllReturnCodes.h>
#include <fun4all/PHTFileServer.h>
#include <phool/PHCompositeNode.h>

namespace
{
template <class T> void CleanVec(std::vector<T> &v)
{
    std::vector<T>().swap(v);
    v.shrink_to_fit();
}
} // namespace

//____________________________________________________________________________..
dNdEtaINTT::dNdEtaINTT(const std::string &name, const std::string &outputfile, const bool &isData, const int &inputFileListIndex, const int &nEvtPerFile)
    : SubsysReco(name), _get_truth_pv(true), _get_reco_cluster(true), _get_centrality(true), _get_trkr_hit(true), _outputFile(outputfile), IsData(isData), InputFileListIndex(inputFileListIndex), NEvtPerFile(nEvtPerFile), svtx_evalstack(nullptr),
      truth_eval(nullptr), clustereval(nullptr), hiteval(nullptr), dst_clustermap(nullptr), clusterhitmap(nullptr), hitsets(nullptr), _tgeometry(nullptr), m_truth_info(nullptr), m_CentInfo(nullptr)
{
    std::cout << "dNdEtaINTT::dNdEtaINTT(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
dNdEtaINTT::~dNdEtaINTT() { std::cout << "dNdEtaINTT::~dNdEtaINTT() Calling dtor" << std::endl; }

//____________________________________________________________________________..
int dNdEtaINTT::Init(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::Init(PHCompositeNode *topNode) Initializing" << std::endl
              << "Running on Data or simulation? -> IsData = " << IsData << std::endl
              << "Initial eventnum = " << InputFileListIndex * NEvtPerFile << std::endl
              << "Number of events per file = " << NEvtPerFile << std::endl;

    PHTFileServer::get().open(_outputFile, "RECREATE");
    outtree = new TTree("EventTree", "EventTree");
    outtree->Branch("event", &event_);
    if (!IsData)
    {
        outtree->Branch("centrality_bimp", &centrality_bimp_);
        outtree->Branch("centrality_impactparam", &centrality_impactparam_);
        outtree->Branch("centrality_mbd", &centrality_mbd_);
        outtree->Branch("centrality_mbdquantity", &centrality_mbdquantity_);
        outtree->Branch("NTruthVtx", &NTruthVtx_);
        outtree->Branch("TruthPV_x", &TruthPV_x_);
        outtree->Branch("TruthPV_y", &TruthPV_y_);
        outtree->Branch("TruthPV_z", &TruthPV_z_);
        outtree->Branch("TruthPV_Npart", &TruthPV_Npart_);
        outtree->Branch("TruthPV_Nhits", &TruthPV_Nhits_);
        outtree->Branch("TruthPV_NClus", &TruthPV_NClus_);
        outtree->Branch("TruthPV_t", &TruthPV_t_);
        outtree->Branch("TruthPV_embed", &TruthPV_embed_);
        outtree->Branch("TruthPV_trig_x", &TruthPV_trig_x_);
        outtree->Branch("TruthPV_trig_y", &TruthPV_trig_y_);
        outtree->Branch("TruthPV_trig_z", &TruthPV_trig_z_);
        outtree->Branch("TruthPV_trig_Npart", &TruthPV_trig_Npart_);

        outtree->Branch("UniqueAncG4P_Px", &UniqueAncG4P_Px_);
        outtree->Branch("UniqueAncG4P_Py", &UniqueAncG4P_Py_);
        outtree->Branch("UniqueAncG4P_Pz", &UniqueAncG4P_Pz_);
        outtree->Branch("UniqueAncG4P_Pt", &UniqueAncG4P_Pt_);
        outtree->Branch("UniqueAncG4P_Eta", &UniqueAncG4P_Eta_);
        outtree->Branch("UniqueAncG4P_Phi", &UniqueAncG4P_Phi_);
        outtree->Branch("UniqueAncG4P_E", &UniqueAncG4P_E_);
        outtree->Branch("UniqueAncG4P_PID", &UniqueAncG4P_PID_);
        outtree->Branch("UniqueAncG4P_TrackPID", &UniqueAncG4P_TrackPID_);
        outtree->Branch("UniqueAncG4P_VtxPID", &UniqueAncG4P_VtxPID_);
        outtree->Branch("UniqueAncG4P_ParentPID", &UniqueAncG4P_ParentPID_);
        outtree->Branch("UniqueAncG4P_PrimaryPID", &UniqueAncG4P_PrimaryPID_);
        outtree->Branch("UniqueAncG4P_IonCharge", &UniqueAncG4P_IonCharge_);
        outtree->Branch("UniqueAncG4P_TrackID", &UniqueAncG4P_TrackID_);
        outtree->Branch("UniqueAncG4P_NClus", &UniqueAncG4P_NClus_);
    }

    outtree->Branch("Layer1_occupancy", &Layer1_occupancy_);
    outtree->Branch("NClus", &NClus_);
    outtree->Branch("NTrkrhits", &NTrkrhits_);
    outtree->Branch("TrkrHitRow", &TrkrHitRow_);
    outtree->Branch("TrkrHitColumn", &TrkrHitColumn_);
    outtree->Branch("ClusLayer", &ClusLayer_);
    outtree->Branch("ClusX", &ClusX_);
    outtree->Branch("ClusY", &ClusY_);
    outtree->Branch("ClusZ", &ClusZ_);
    outtree->Branch("ClusR", &ClusR_);
    outtree->Branch("ClusPhi", &ClusPhi_);
    outtree->Branch("ClusEta", &ClusEta_);
    outtree->Branch("ClusAdc", &ClusAdc_);
    outtree->Branch("ClusPhiSize", &ClusPhiSize_);
    outtree->Branch("ClusZSize", &ClusZSize_);
    outtree->Branch("ClusLadderZId", &ClusLadderZId_);
    outtree->Branch("ClusLadderPhiId", &ClusLadderPhiId_);
    outtree->Branch("ClusTrkrHitSetKey", &ClusTrkrHitSetKey_);
    outtree->Branch("ClusTimeBucketId", &ClusTimeBucketId_);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::InitRun(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::process_event(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::process_event(PHCompositeNode *topNode) Processing Event" << InputFileListIndex * NEvtPerFile + eventNum << std::endl;

    PHNodeIterator nodeIter(topNode);

    if (!svtx_evalstack)
    {
        svtx_evalstack = new SvtxEvalStack(topNode);
        clustereval = svtx_evalstack->get_cluster_eval();
        hiteval = svtx_evalstack->get_hit_eval();
        truth_eval = svtx_evalstack->get_truth_eval();
    }

    svtx_evalstack->next_event(topNode);

    dst_clustermap = findNode::getClass<TrkrClusterContainerv4>(topNode, "TRKR_CLUSTER");
    if (!dst_clustermap)
    {
        std::cout << PHWHERE << "Error, can't find trkr clusters" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    clusterhitmap = findNode::getClass<TrkrClusterHitAssoc>(topNode, "TRKR_CLUSTERHITASSOC");
    if (!clusterhitmap)
    {
        std::cout << PHWHERE << "Error, can't find cluster-hit map" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    hitsets = findNode::getClass<TrkrHitSetContainer>(topNode, "TRKR_HITSET");
    if (!hitsets)
    {
        std::cout << PHWHERE << "Error, can't find cluster-hit map" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    _tgeometry = findNode::getClass<ActsGeometry>(topNode, "ActsGeometry");
    if (!_tgeometry)
    {
        std::cout << PHWHERE << "Error, can't find Acts geometry" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    // Truth information for truth vertex
    m_truth_info = findNode::getClass<PHG4TruthInfoContainer>(topNode, "G4TruthInfo");
    if (!m_truth_info)
    {
        std::cout << PHWHERE << "Error, can't find G4TruthInfo" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

    if (!IsData)
    {
        std::cout << "Running on simulation" << std::endl;
        if (_get_truth_pv)
            GetTruthPVInfo(topNode);
    }

    if (_get_centrality)
        GetCentralityInfo(topNode);

    if (_get_trkr_hit)
        GetTrkrHitInfo(topNode);

    if (_get_reco_cluster)
        GetRecoClusterInfo(topNode);

    event_ = InputFileListIndex * NEvtPerFile + eventNum;
    outtree->Fill();
    eventNum++;

    ResetVectors();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::EndRun(const int runnumber)
{
    std::cout << "dNdEtaINTT::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::End(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::End(PHCompositeNode *topNode) This is the End - Output to " << _outputFile << std::endl;

    PHTFileServer::get().cd(_outputFile);
    outtree->Write("", TObject::kOverwrite);

    delete svtx_evalstack;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaINTT::Reset(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void dNdEtaINTT::Print(const std::string &what) const { std::cout << "dNdEtaINTT::Print(const std::string &what) const Printing info for " << what << std::endl; }
//____________________________________________________________________________..
void dNdEtaINTT::GetCentralityInfo(PHCompositeNode *topNode)
{
    std::cout << "Get centrality info." << std::endl;

    // Centrality info
    m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
    if (!m_CentInfo)
    {
        std::cout << PHWHERE << "Error, can't find CentralityInfov1" << std::endl;
        return;
    }

    if (!IsData)
    {
        centrality_bimp_ = m_CentInfo->get_centile(CentralityInfo::PROP::bimp);
        centrality_impactparam_ = m_CentInfo->get_quantity(CentralityInfo::PROP::bimp);
    }
    centrality_mbd_ = m_CentInfo->get_centile(CentralityInfo::PROP::mbd_NS);
    centrality_mbdquantity_ = m_CentInfo->get_quantity(CentralityInfo::PROP::mbd_NS);
    std::cout << "Centrality: (bimp,impactparam) = (" << centrality_bimp_ << ", " << centrality_impactparam_ << "); (mbd,mbdquantity) = (" << centrality_mbd_ << ", " << centrality_mbdquantity_ << ")" << std::endl;
}
//____________________________________________________________________________..
PHG4Particle *dNdEtaINTT::GetG4PAncestor(PHG4Particle *p)
{
    // std::cout << "In GetG4Ancestor: " << std::endl << "start with original particle: ";
    // p->identify();

    PHG4Particle *ancestor = m_truth_info->GetParticle(p->get_parent_id());
    PHG4Particle *prevpart = p;
    while (ancestor != nullptr)
    {
        // ancestor->identify();
        prevpart = ancestor;
        ancestor = m_truth_info->GetParticle(ancestor->get_parent_id());
    }
    return prevpart;
}
//____________________________________________________________________________..
void dNdEtaINTT::GetTrkrHitInfo(PHCompositeNode *topNode)
{
    std::cout << "Get TrkrHit info." << std::endl;

    TrkrHitSetContainer::ConstRange hitset_range = hitsets->getHitSets(TrkrDefs::TrkrId::inttId);
    for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first; hitset_iter != hitset_range.second; ++hitset_iter)
    {
        TrkrHitSet::ConstRange hit_range = hitset_iter->second->getHits();
        for (TrkrHitSet::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; ++hit_iter)
        {
            TrkrDefs::hitkey hitKey = hit_iter->first;

            TrkrHitRow_.push_back(InttDefs::getRow(hitKey));
            TrkrHitColumn_.push_back(InttDefs::getCol(hitKey));
        }
    }
    NTrkrhits_ = TrkrHitRow_.size();
}
//____________________________________________________________________________..
void dNdEtaINTT::GetRecoClusterInfo(PHCompositeNode *topNode)
{
    std::cout << "Get reconstructed cluster info." << std::endl;

    std::vector<int> _NClus = {0, 0};
    std::vector<int> G4PfromClus_IsValid = {0, 0};
    std::map<int, unsigned int> AncG4P_cluster_count;
    AncG4P_cluster_count.clear();

    // Reference:
    // https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/simulation/g4simulation/g4eval/SvtxEvaluator.cc#L1731-L1984
    // for (const auto &hitsetkey : dst_clustermap->getHitSetKeys())
    for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::inttId))
    {
        auto range = dst_clustermap->getClusters(hitsetkey);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            // std::cout << "----------" << std::endl;
            TrkrDefs::cluskey ckey = iter->first;
            TrkrCluster *cluster = dst_clustermap->findCluster(ckey);

            unsigned int trkrId = TrkrDefs::getTrkrId(ckey);
            if (trkrId != TrkrDefs::inttId)
                continue; // we want only INTT clusters

            int layer = (TrkrDefs::getLayer(ckey) == 3 || TrkrDefs::getLayer(ckey) == 4) ? 0 : 1;
            _NClus[layer]++;
            if (cluster == nullptr)
            {
                std::cout << "cluster is nullptr, ckey=" << ckey << std::endl;
            }
            auto globalpos = _tgeometry->getGlobalPosition(ckey, cluster);
            ClusLayer_.push_back(TrkrDefs::getLayer(ckey));
            ClusX_.push_back(globalpos(0));
            ClusY_.push_back(globalpos(1));
            ClusZ_.push_back(globalpos(2));
            ClusAdc_.push_back(cluster->getAdc());
            TVector3 pos(globalpos(0), globalpos(1), globalpos(2));
            ClusR_.push_back(pos.Perp());
            ClusPhi_.push_back(pos.Phi());
            ClusEta_.push_back(pos.Eta());
            ClusPhiSize_.push_back(cluster->getPhiSize());
            ClusZSize_.push_back(cluster->getZSize());
            ClusLadderZId_.push_back(InttDefs::getLadderZId(ckey));
            ClusLadderPhiId_.push_back(InttDefs::getLadderPhiId(ckey));
            ClusTrkrHitSetKey_.push_back(hitsetkey);
            ClusTimeBucketId_.push_back(InttDefs::getTimeBucketId(ckey));
            if (!IsData)
            {
                // std::cout << "This cluster: (x,y,z)=(" << globalpos(0) << "," << globalpos(1) << "," << globalpos(2) << ")" << std::endl;
                PHG4Particle *g4p = clustereval->max_truth_particle_by_cluster_energy(ckey);
                // if (g4p) g4p->identify();

                if (g4p)
                {
                    G4PfromClus_PID_.push_back(g4p->get_pid());
                    G4PfromClus_IsValid[0]++;

                    PHG4Particle *g4pancestor = GetG4PAncestor(g4p);

                    if (g4pancestor && AncG4P_cluster_count[g4pancestor->get_track_id()] == 0)
                    {
                        UniqueAncG4P_PID_.push_back(g4pancestor->get_pid());
                        UniqueAncG4P_TrackPID_.push_back(g4pancestor->get_track_id());
                        UniqueAncG4P_VtxPID_.push_back(g4pancestor->get_vtx_id());
                        UniqueAncG4P_ParentPID_.push_back(g4pancestor->get_parent_id());
                        UniqueAncG4P_PrimaryPID_.push_back(g4pancestor->get_primary_id());
                        UniqueAncG4P_IonCharge_.push_back(g4pancestor->get_IonCharge());
                        TLorentzVector g4pAncvec;
                        g4pAncvec.SetPxPyPzE(g4pancestor->get_px(), g4pancestor->get_py(), g4pancestor->get_pz(), g4pancestor->get_e());
                        UniqueAncG4P_E_.push_back(g4pancestor->get_e());
                        UniqueAncG4P_Pt_.push_back(g4pAncvec.Pt());
                        UniqueAncG4P_Eta_.push_back(g4pAncvec.Eta());
                        UniqueAncG4P_Phi_.push_back(g4pAncvec.Phi());

                        ++AncG4P_cluster_count[g4pancestor->get_track_id()];
                    }
                }
                else
                {
                    G4PfromClus_PID_.push_back(INT_MAX);
                    G4PfromClus_IsValid[1]++;
                }
            }
        }
    }

    NClus_ = _NClus[0] + _NClus[1];
    Layer1_occupancy_ = _NClus[0];
    std::cout << "Number of clusters (total,0,1)=(" << NClus_ << "," << _NClus[0] << "," << _NClus[1] << ")" << std::endl;
    std::cout << "Size of G4PfromClus_PID_=" << G4PfromClus_PID_.size() << "; Number of non-null PHG4Particle ptr=" << G4PfromClus_IsValid[0] << ", number of null PHG4Particle ptr=" << G4PfromClus_IsValid[1] << std::endl;

    int uniqueAncG4P = 0, N_MatchedClus = 0;
    for (auto it = AncG4P_cluster_count.cbegin(); it != AncG4P_cluster_count.cend(); ++it)
    {
        // std::cout << "(Ancestor G4P trackID, matched cluster counts)=(" << it->first << "," << it->second << ")" << std::endl;
        uniqueAncG4P++;
        N_MatchedClus += it->second;
        UniqueAncG4P_TrackID_.push_back(it->first);
        UniqueAncG4P_NClus_.push_back(it->second);
    }
    std::cout << "(# of unique ancestor G4P, # of matched clusters, ratio)=(" << uniqueAncG4P << "," << N_MatchedClus << "," << (float)uniqueAncG4P / N_MatchedClus << ")" << std::endl;
}
//____________________________________________________________________________..
void dNdEtaINTT::GetTruthPVInfo(PHCompositeNode *topNode)
{
    std::cout << "Get truth primary vertex info." << std::endl;

    auto vrange = m_truth_info->GetPrimaryVtxRange();
    const auto prange = m_truth_info->GetPrimaryParticleRange();

    std::map<int, unsigned int> vertex_particle_count;
    vertex_particle_count.clear();
    std::map<int, unsigned int> vertex_hit_count;
    vertex_hit_count.clear();

    for (auto iter = prange.first; iter != prange.second; ++iter) // process all primary paricle
    {
        ++vertex_particle_count[iter->second->get_vtx_id()];

        // PHG4Particle *particle = iter->second;
        std::set<PHG4Hit *> g4hits = truth_eval->all_truth_hits(iter->second);
        for (auto hit : g4hits)
        {
            std::vector<int> layerID{3, 4, 5, 6};
            std::vector<int>::iterator it = std::find(layerID.begin(), layerID.end(), hit->get_layer());
            if (it != layerID.end())
            {
                ++vertex_hit_count[iter->second->get_vtx_id()];
            }
        }
    }

    std::map<int, unsigned int> vertex_cluster_count;
    vertex_cluster_count.clear();
    for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::inttId))
    {
        auto range = dst_clustermap->getClusters(hitsetkey);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            PHG4Particle *g4particle = clustereval->max_truth_particle_by_cluster_energy(iter->first);
            if (g4particle)
            {
                if (g4particle->get_parent_id() != 0) // primary particles
                    continue;

                ++vertex_cluster_count[g4particle->get_vtx_id()];
            }
        }
    }

    // int TruthPV_idx = 0;
    int TruthPV_idx = 0;
    for (auto iter = vrange.first; iter != vrange.second; ++iter) // process all primary vertices
    {
        const int point_id = iter->first;
        PHG4VtxPoint *point = iter->second;

        if (point)
        {
            TruthPV_x_.push_back(point->get_x());
            TruthPV_y_.push_back(point->get_y());
            TruthPV_z_.push_back(point->get_z());
            TruthPV_Npart_.push_back(vertex_particle_count[point_id]);
            TruthPV_Nhits_.push_back(vertex_hit_count[point_id]);
            TruthPV_NClus_.push_back(vertex_cluster_count[point_id]);
            TruthPV_t_.push_back(point->get_t());
            TruthPV_embed_.push_back(m_truth_info->isEmbededVtx(point_id));

            if (m_truth_info->isEmbededVtx(point_id) == 0)
            {
                TruthPV_trig_x_ = point->get_x();
                TruthPV_trig_y_ = point->get_y();
                TruthPV_trig_z_ = point->get_z();
                TruthPV_trig_Npart_ = vertex_particle_count[point_id];
            }

            // std::cout << "TruthPV_idx = " << TruthPV_idx << "(x,y,z,t,embed)
            // = (" << point->get_x() << "," << point->get_y() << "," <<
            // point->get_z() << "," << point->get_t() << "," <<
            // m_truth_info->isEmbededVtx(point_id) << ")" << std::endl;
            std::cout << "TruthPV_idx = " << TruthPV_idx << "(x,y,z,t,Npart,NIntthits,NInttClus,embed) = (" << point->get_x() << "," << point->get_y() << "," << point->get_z() << "," << point->get_t() << "," << vertex_particle_count[point_id] << ","
                      << vertex_hit_count[point_id] << "," << vertex_cluster_count[point_id] << "," << m_truth_info->isEmbededVtx(point_id) << ")" << std::endl;
            TruthPV_idx++;
        }
    }

    NTruthVtx_ = (int)TruthPV_x_.size();
}
//____________________________________________________________________________..
void dNdEtaINTT::ResetVectors()
{
    // CleanVec(ClusHitcount_);
    CleanVec(ClusLayer_);
    CleanVec(ClusX_);
    CleanVec(ClusY_);
    CleanVec(ClusZ_);
    CleanVec(ClusR_);
    CleanVec(ClusPhi_);
    CleanVec(ClusEta_);
    CleanVec(ClusAdc_);
    CleanVec(ClusPhiSize_);
    CleanVec(ClusZSize_);
    CleanVec(ClusLadderZId_);
    CleanVec(ClusLadderPhiId_);
    CleanVec(ClusTrkrHitSetKey_);
    CleanVec(ClusTimeBucketId_);
    CleanVec(TrkrHitRow_);
    CleanVec(TrkrHitColumn_);
    CleanVec(TruthPV_x_);
    CleanVec(TruthPV_y_);
    CleanVec(TruthPV_z_);
    CleanVec(TruthPV_Npart_);
    CleanVec(TruthPV_Nhits_);
    CleanVec(TruthPV_NClus_);
    CleanVec(TruthPV_t_);
    CleanVec(TruthPV_embed_);
    CleanVec(G4PfromClus_PID_);
    CleanVec(UniqueAncG4P_Px_);
    CleanVec(UniqueAncG4P_Py_);
    CleanVec(UniqueAncG4P_Pz_);
    CleanVec(UniqueAncG4P_Pt_);
    CleanVec(UniqueAncG4P_Eta_);
    CleanVec(UniqueAncG4P_Phi_);
    CleanVec(UniqueAncG4P_E_);
    CleanVec(UniqueAncG4P_TrackID_);
    CleanVec(UniqueAncG4P_NClus_);
    CleanVec(UniqueAncG4P_IonCharge_);
    CleanVec(UniqueAncG4P_PID_);
    CleanVec(UniqueAncG4P_TrackPID_);
    CleanVec(UniqueAncG4P_VtxPID_);
    CleanVec(UniqueAncG4P_ParentPID_);
    CleanVec(UniqueAncG4P_PrimaryPID_);
}
