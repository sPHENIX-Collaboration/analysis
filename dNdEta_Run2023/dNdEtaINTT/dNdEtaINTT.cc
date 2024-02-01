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
#include <trackbase/TrkrHitv2.h>

namespace
{
template <class T> void CleanVec(std::vector<T> &v)
{
    std::vector<T>().swap(v);
    v.shrink_to_fit();
}
} // namespace

//____________________________________________________________________________..
dNdEtaINTT::dNdEtaINTT(const std::string &name, const std::string &outputfile, const bool &isData)
    : SubsysReco(name), _get_truth_pv(true), _get_reco_cluster(true), _get_centrality(true), _get_trkr_hit(true), _outputFile(outputfile), IsData(isData), svtx_evalstack(nullptr), truth_eval(nullptr), clustereval(nullptr), hiteval(nullptr),
      dst_clustermap(nullptr), clusterhitmap(nullptr), hitsets(nullptr), _tgeometry(nullptr), m_truth_info(nullptr), m_CentInfo(nullptr)
{
    std::cout << "dNdEtaINTT::dNdEtaINTT(const std::string &name) Calling ctor" << std::endl;
}

//____________________________________________________________________________..
dNdEtaINTT::~dNdEtaINTT() { std::cout << "dNdEtaINTT::~dNdEtaINTT() Calling dtor" << std::endl; }

//____________________________________________________________________________..
int dNdEtaINTT::Init(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaINTT::Init(PHCompositeNode *topNode) Initializing" << std::endl << "Running on Data or simulation? -> IsData = " << IsData << std::endl;

    PHTFileServer::get().open(_outputFile, "RECREATE");
    outtree = new TTree("EventTree", "EventTree");
    outtree->Branch("event", &event_);
    if (!IsData)
    {
        outtree->Branch("ncoll", &ncoll_);
        outtree->Branch("npart", &npart_);
        outtree->Branch("centrality_bimp", &centrality_bimp_);
        outtree->Branch("centrality_impactparam", &centrality_impactparam_);
        outtree->Branch("centrality_mbd", &centrality_mbd_);
        outtree->Branch("centrality_mbdquantity", &centrality_mbdquantity_);
        outtree->Branch("NTruthVtx", &NTruthVtx_);
        outtree->Branch("TruthPV_trig_x", &TruthPV_trig_x_);
        outtree->Branch("TruthPV_trig_y", &TruthPV_trig_y_);
        outtree->Branch("TruthPV_trig_z", &TruthPV_trig_z_);
        outtree->Branch("NGenPart", &NGenPart_);
        outtree->Branch("UniqueAncG4P_Pt", &UniqueAncG4P_Pt_);
        outtree->Branch("UniqueAncG4P_Eta", &UniqueAncG4P_Eta_);
        outtree->Branch("UniqueAncG4P_Phi", &UniqueAncG4P_Phi_);
        outtree->Branch("UniqueAncG4P_E", &UniqueAncG4P_E_);
        outtree->Branch("UniqueAncG4P_PID", &UniqueAncG4P_PID_);
    }

    outtree->Branch("NClus_Layer1", &NClus_Layer1_);
    outtree->Branch("NClus", &NClus_);
    outtree->Branch("NTrkrhits", &NTrkrhits_);
    outtree->Branch("TrkrHitRow", &TrkrHitRow_);
    outtree->Branch("TrkrHitColumn", &TrkrHitColumn_);
    outtree->Branch("TrkrHitLadderZId", &TrkrHitLadderZId_);
    outtree->Branch("TrkrHitLadderPhiId", &TrkrHitLadderPhiId_);
    outtree->Branch("TrkrHitLayer", &TrkrHitLayer_);
    outtree->Branch("TrkrHitADC", &TrkrHitADC_);
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
    // std::cout << "dNdEtaINTT::process_event(PHCompositeNode *topNode) Processing Event" << InputFileListIndex * NEvtPerFile + eventNum << std::endl;
    std::cout << "dNdEtaINTT::process_event(PHCompositeNode *topNode) Processing Event" << eventNum << std::endl;

    PHNodeIterator nodeIter(topNode);

    if (!svtx_evalstack)
    {
        svtx_evalstack = new SvtxEvalStack(topNode);
        clustereval = svtx_evalstack->get_cluster_eval();
        hiteval = svtx_evalstack->get_hit_eval();
        truth_eval = svtx_evalstack->get_truth_eval();
    }

    svtx_evalstack->next_event(topNode);

    // eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
    // if (!eventheader)
    // {
    //     std::cout << PHWHERE << "Error, can't find EventHeader" << std::endl;
    //     return Fun4AllReturnCodes::ABORTEVENT;
    // }

    // Centrality info
    m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
    if (!m_CentInfo)
    {
        std::cout << PHWHERE << "Error, can't find CentralityInfov1" << std::endl;
        return Fun4AllReturnCodes::ABORTEVENT;
    }

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

        eventheader = findNode::getClass<EventHeader>(topNode, "EventHeader");
        if (!eventheader)
        {
            std::cout << PHWHERE << "Error, can't find EventHeader" << std::endl;
            return Fun4AllReturnCodes::ABORTEVENT;
        }

        if (_get_truth_pv)
            GetTruthPVInfo(topNode);
    }

    if (_get_centrality)
        GetCentralityInfo(topNode);

    if (_get_trkr_hit)
        GetTrkrHitInfo(topNode);

    if (_get_reco_cluster)
        GetRecoClusterInfo(topNode);

    // event_ = InputFileListIndex * NEvtPerFile + eventNum;
    event_ = eventNum;
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
void dNdEtaINTT::GetCentralityInfo(PHCompositeNode *topNode)
{
    std::cout << "Get centrality info." << std::endl;

    if (!IsData)
    {
        centrality_bimp_ = m_CentInfo->get_centile(CentralityInfo::PROP::bimp);
        centrality_impactparam_ = m_CentInfo->get_quantity(CentralityInfo::PROP::bimp);

        // Glauber parameter information
        ncoll_ = eventheader->get_ncoll();
        npart_ = eventheader->get_npart();
    }
    centrality_mbd_ = m_CentInfo->get_centile(CentralityInfo::PROP::mbd_NS);
    centrality_mbdquantity_ = m_CentInfo->get_quantity(CentralityInfo::PROP::mbd_NS);

    std::cout << "Centrality: (bimp,impactparam) = (" << centrality_bimp_ << ", " << centrality_impactparam_ << "); (mbd,mbdquantity) = (" << centrality_mbd_ << ", " << centrality_mbdquantity_ << ")" << std::endl;
    std::cout << "Glauber parameter information: (ncoll,npart) = (" << ncoll_ << ", " << npart_ << ")" << std::endl;
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
            TrkrDefs::hitsetkey hitSetKey = hitset_iter->first;

            TrkrHitRow_.push_back(InttDefs::getRow(hitKey));
            TrkrHitColumn_.push_back(InttDefs::getCol(hitKey));
            TrkrHitLadderZId_.push_back(InttDefs::getLadderZId(hitSetKey));
            TrkrHitLadderPhiId_.push_back(InttDefs::getLadderPhiId(hitSetKey));
            TrkrHitLayer_.push_back(TrkrDefs::getLayer(hitSetKey));
            TrkrHitADC_.push_back(hit_iter->second->getAdc());
        }
    }
    NTrkrhits_ = TrkrHitRow_.size();
}
//____________________________________________________________________________..
void dNdEtaINTT::GetRecoClusterInfo(PHCompositeNode *topNode)
{
    std::cout << "Get reconstructed cluster info." << std::endl;

    std::vector<int> _NClus = {0, 0};
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
                    PHG4Particle *g4pancestor = GetG4PAncestor(g4p);

                    if (g4pancestor && AncG4P_cluster_count[g4pancestor->get_track_id()] == 0)
                    {
                        UniqueAncG4P_PID_.push_back(g4pancestor->get_pid());
                        TLorentzVector g4pAncvec;
                        g4pAncvec.SetPxPyPzE(g4pancestor->get_px(), g4pancestor->get_py(), g4pancestor->get_pz(), g4pancestor->get_e());
                        UniqueAncG4P_E_.push_back(g4pancestor->get_e());
                        UniqueAncG4P_Pt_.push_back(g4pAncvec.Pt());
                        UniqueAncG4P_Eta_.push_back(g4pAncvec.Eta());
                        UniqueAncG4P_Phi_.push_back(g4pAncvec.Phi());

                        ++AncG4P_cluster_count[g4pancestor->get_track_id()];
                    }
                }
            }
        }
    }

    NClus_ = _NClus[0] + _NClus[1];
    NClus_Layer1_ = _NClus[0];
    std::cout << "Number of clusters (total,0,1)=(" << NClus_ << "," << _NClus[0] << "," << _NClus[1] << ")" << std::endl;
    NGenPart_ = UniqueAncG4P_PID_.size();

    int uniqueAncG4P = 0, N_MatchedClus = 0;
    for (auto it = AncG4P_cluster_count.cbegin(); it != AncG4P_cluster_count.cend(); ++it)
    {
        // std::cout << "(Ancestor G4P trackID, matched cluster counts)=(" << it->first << "," << it->second << ")" << std::endl;
        uniqueAncG4P++;
        N_MatchedClus += it->second;
    }
    std::cout << "(# of unique ancestor G4P, # of matched clusters, ratio)=(" << uniqueAncG4P << "," << N_MatchedClus << "," << (float)uniqueAncG4P / N_MatchedClus << ")" << std::endl;
}
//____________________________________________________________________________..
void dNdEtaINTT::GetTruthPVInfo(PHCompositeNode *topNode)
{
    std::cout << "Get truth primary vertex info." << std::endl;

    auto vrange = m_truth_info->GetPrimaryVtxRange();

    int NTruthPV = 0;
    for (auto iter = vrange.first; iter != vrange.second; ++iter) // process all primary vertices
    {
        const int point_id = iter->first;
        PHG4VtxPoint *point = iter->second;

        if (point)
        {
            if (m_truth_info->isEmbededVtx(point_id) == 0)
            {
                TruthPV_trig_x_ = point->get_x();
                TruthPV_trig_y_ = point->get_y();
                TruthPV_trig_z_ = point->get_z();
            }

            NTruthPV++;
        }
    }

    // print out the truth vertex information
    std::cout << "Number of truth primary vertices = " << NTruthPV << "; Triggered truth vtx pos (x,y,z)=(" << TruthPV_trig_x_ << "," << TruthPV_trig_y_ << "," << TruthPV_trig_z_ << ")" << std::endl;

    NTruthVtx_ = NTruthPV;
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
    CleanVec(TrkrHitLadderZId_);
    CleanVec(TrkrHitLadderPhiId_);
    CleanVec(TrkrHitLayer_);
    CleanVec(TrkrHitADC_);
    CleanVec(UniqueAncG4P_Pt_);
    CleanVec(UniqueAncG4P_Eta_);
    CleanVec(UniqueAncG4P_Phi_);
    CleanVec(UniqueAncG4P_E_);
    CleanVec(UniqueAncG4P_PID_);
}
