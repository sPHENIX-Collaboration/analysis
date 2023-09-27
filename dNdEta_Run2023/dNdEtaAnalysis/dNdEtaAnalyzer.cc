//____________________________________________________________________________..
//
// This is a template for a Fun4All SubsysReco module with all methods from the
// $OFFLINE_MAIN/include/fun4all/SubsysReco.h baseclass
// You do not have to implement all of them, you can just remove unused methods
// here and in dNdEtaAnalyzer.h.
//
// dNdEtaAnalyzer(const std::string &name = "dNdEtaAnalyzer")
// everything is keyed to dNdEtaAnalyzer, duplicate names do work but it makes
// e.g. finding culprits in logs difficult or getting a pointer to the module
// from the command line
//
// dNdEtaAnalyzer::~dNdEtaAnalyzer()
// this is called when the Fun4AllServer is deleted at the end of running. Be
// mindful what you delete - you do loose ownership of object you put on the
// node tree
//
// int dNdEtaAnalyzer::Init(PHCompositeNode *topNode)
// This method is called when the module is registered with the Fun4AllServer.
// You can create historgrams here or put objects on the node tree but be aware
// that modules which haven't been registered yet did not put antyhing on the
// node tree
//
// int dNdEtaAnalyzer::InitRun(PHCompositeNode *topNode)
// This method is called when the first event is read (or generated). At
// this point the run number is known (which is mainly interesting for raw data
// processing). Also all objects are on the node tree in case your module's
// action depends on what else is around. Last chance to put nodes under the DST
// Node We mix events during readback if branches are added after the first
// event
//
// int dNdEtaAnalyzer::process_event(PHCompositeNode *topNode)
// called for every event. Return codes trigger actions, you find them in
// $OFFLINE_MAIN/include/fun4all/Fun4AllReturnCodes.h
//   everything is good:
//     return Fun4AllReturnCodes::EVENT_OK
//   abort event reconstruction, clear everything and process next event:
//     return Fun4AllReturnCodes::ABORT_EVENT;
//   proceed but do not save this event in output (needs output manager
//   setting):
//     return Fun4AllReturnCodes::DISCARD_EVENT;
//   abort processing:
//     return Fun4AllReturnCodes::ABORT_RUN
// all other integers will lead to an error and abort of processing
//
// int dNdEtaAnalyzer::ResetEvent(PHCompositeNode *topNode)
// If you have internal data structures (arrays, stl containers) which needs
// clearing after each event, this is the place to do that. The nodes under the
// DST node are cleared by the framework
//
// int dNdEtaAnalyzer::EndRun(const int runnumber)
// This method is called at the end of a run when an event from a new run is
// encountered. Useful when analyzing multiple runs (raw data). Also called at
// the end of processing (before the End() method)
//
// int dNdEtaAnalyzer::End(PHCompositeNode *topNode)
// This is called at the end of processing. It needs to be called by the macro
// by Fun4AllServer::End(), so do not forget this in your macro
//
// int dNdEtaAnalyzer::Reset(PHCompositeNode *topNode)
// not really used - it is called before the dtor is called
//
// void dNdEtaAnalyzer::Print(const std::string &what) const
// Called from the command line - useful to print information when you need it
//
//____________________________________________________________________________..

#include "dNdEtaAnalyzer.h"

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
dNdEtaAnalyzer::dNdEtaAnalyzer(const std::string &name, const bool &isData, const int &inputFileListIndex, const int &nEvtPerFile)
    : SubsysReco(name), _get_truth_pv(true), _get_g4particle(true), _get_reco_cluster(true), _get_mvtx_hit(true), _get_centrality(true), IsData(isData), InputFileListIndex(inputFileListIndex),
      NEvtPerFile(nEvtPerFile), svtx_evalstack(nullptr), truth_eval(nullptr), clustereval(nullptr), hiteval(nullptr), dst_clustermap(nullptr), clusterhitmap(nullptr), hitsets(nullptr),
      _tgeometry(nullptr), m_truth_info(nullptr), m_CentInfo(nullptr)
{
    std::cout << "dNdEtaAnalyzer::dNdEtaAnalyzer(const std::string &name) "
                 "Calling ctor"
              << std::endl;
}

//____________________________________________________________________________..
dNdEtaAnalyzer::~dNdEtaAnalyzer() { std::cout << "dNdEtaAnalyzer::~dNdEtaAnalyzer() Calling dtor" << std::endl; }

//____________________________________________________________________________..
int dNdEtaAnalyzer::Init(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::Init(PHCompositeNode *topNode) Initializing" << std::endl
              << "Running on Data or simulation? -> IsData = " << IsData << std::endl
              << "Initial eventnum = " << InputFileListIndex * NEvtPerFile << std::endl
              << "Number of events per file = " << NEvtPerFile << std::endl;

    PHTFileServer::get().open(Form("/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/%s.root", Name().c_str()), "RECREATE");

    outtree = new TTree("EventTree", "EventTree");
    outtree->Branch("event", &event_);
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
    outtree->Branch("TruthPV_mostNpart_x", &TruthPV_mostNpart_x_);
    outtree->Branch("TruthPV_mostNpart_y", &TruthPV_mostNpart_y_);
    outtree->Branch("TruthPV_mostNpart_z", &TruthPV_mostNpart_z_);
    outtree->Branch("TruthPV_mostNpart_Npart", &TruthPV_mostNpart_Npart_);
    outtree->Branch("MVTXL1_occupancy", &MVTXL1_occupancy_);
    outtree->Branch("NMVTXClus", &NMVTXClus_);
    outtree->Branch("ClusLayer", &ClusLayer_);
    // outtree->Branch("ClusHitcount", &ClusHitcount_);
    outtree->Branch("ClusX", &ClusX_);
    outtree->Branch("ClusY", &ClusY_);
    outtree->Branch("ClusZ", &ClusZ_);
    outtree->Branch("ClusR", &ClusR_);
    outtree->Branch("ClusPhi", &ClusPhi_);
    outtree->Branch("ClusEta", &ClusEta_);
    outtree->Branch("ClusAdc", &ClusAdc_);
    outtree->Branch("ClusPhiSize", &ClusPhiSize_);
    outtree->Branch("ClusZSize", &ClusZSize_);
    // outtree->Branch("G4PfromClus_PID", &G4PfromClus_PID_);
    // outtree->Branch("G4PfromClus_TrackID", &G4PfromClus_TrackID_);
    // outtree->Branch("G4PfromClus_VtxID", &G4PfromClus_VtxID_);
    // outtree->Branch("G4PfromClus_ParentID", &G4PfromClus_ParentID_);
    // outtree->Branch("G4PfromClus_PrimaryID", &G4PfromClus_PrimaryID_);
    // outtree->Branch("G4PfromClus_Px", &G4PfromClus_Px_);
    // outtree->Branch("G4PfromClus_Py", &G4PfromClus_Py_);
    // outtree->Branch("G4PfromClus_Pz", &G4PfromClus_Pz_);
    // outtree->Branch("G4PfromClus_Pt", &G4PfromClus_Pt_);
    // outtree->Branch("G4PfromClus_Eta", &G4PfromClus_Eta_);
    // outtree->Branch("G4PfromClus_Phi", &G4PfromClus_Phi_);
    // outtree->Branch("G4PfromClus_E", &G4PfromClus_E_);
    // outtree->Branch("G4PfromClus_MomPID", &G4PfromClus_MomPID_);
    // outtree->Branch("G4PfromClus_MomTrackID", &G4PfromClus_MomTrackID_);
    // outtree->Branch("G4PfromClus_MomVtxID", &G4PfromClus_MomVtxID_);
    // outtree->Branch("G4PfromClus_MomParentID", &G4PfromClus_MomParentID_);
    // outtree->Branch("G4PfromClus_MomPrimaryID", &G4PfromClus_MomPrimaryID_);
    // outtree->Branch("G4PfromClus_GMomPID", &G4PfromClus_GMomPID_);
    // outtree->Branch("G4PfromClus_GMomTrackID", &G4PfromClus_GMomTrackID_);
    // outtree->Branch("G4PfromClus_GMomVtxID", &G4PfromClus_GMomVtxID_);
    // outtree->Branch("G4PfromClus_GMomParentID", &G4PfromClus_GMomParentID_);
    // outtree->Branch("G4PfromClus_GMomPrimaryID", &G4PfromClus_GMomPrimaryID_);
    // outtree->Branch("G4PfromClus_AncPID", &G4PfromClus_AncPID_);
    // outtree->Branch("G4PfromClus_AncTrackID", &G4PfromClus_AncTrackID_);
    // outtree->Branch("G4PfromClus_AncVtxID", &G4PfromClus_AncVtxID_);
    // outtree->Branch("G4PfromClus_AncParentID", &G4PfromClus_AncParentID_);
    // outtree->Branch("G4PfromClus_AncPrimaryID", &G4PfromClus_AncPrimaryID_);
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
    //
    outtree->Branch("G4PartPID", &G4PartPID_);
    outtree->Branch("G4PartTrackID", &G4PartTrackID_);
    outtree->Branch("G4PartVtxID", &G4PartVtxID_);
    outtree->Branch("G4PartParentID", &G4PartParentID_);
    outtree->Branch("G4PartPrimaryID", &G4PartPrimaryID_);
    outtree->Branch("G4PartPt", &G4PartPt_);
    outtree->Branch("G4PartEta", &G4PartEta_);
    outtree->Branch("G4PartPhi", &G4PartPhi_);
    outtree->Branch("G4PartE", &G4PartE_);
    //
    outtree->Branch("TrkrHitLayerID", &TrkrHitLayerID_);
    outtree->Branch("TrkrHitStaveID", &TrkrHitStaveID_);
    outtree->Branch("TrkrHitChipID", &TrkrHitChipID_);
    outtree->Branch("TrkrHitRow", &TrkrHitRow_);
    outtree->Branch("TrkrHitColumn", &TrkrHitColumn_);
    outtree->Branch("NValidG4hit", &NValidG4hit_);
    outtree->Branch("NValidMvtxG4hit", &NValidMvtxG4hit_);
    outtree->Branch("G4HitX", &G4HitX_);
    outtree->Branch("G4HitY", &G4HitY_);
    outtree->Branch("G4HitZ", &G4HitZ_);
    outtree->Branch("G4HitT", &G4HitT_);
    outtree->Branch("G4HitLayer", &G4HitLayer_);
    outtree->Branch("G4Hit_IsSameClus", &G4Hit_IsSameClus_);
    outtree->Branch("HitfromG4P_X", &HitfromG4P_X_);
    outtree->Branch("HitfromG4P_Y", &HitfromG4P_Y_);
    outtree->Branch("HitfromG4P_Z", &HitfromG4P_Z_);

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::InitRun(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::InitRun(PHCompositeNode *topNode) Initializing for Run XXX" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::process_event(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::process_event(PHCompositeNode *topNode) Processing Event" << InputFileListIndex * NEvtPerFile + eventNum << std::endl;

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
        if (_get_mvtx_hit)
            GetMVTXhitsInfo(topNode);
        if (_get_truth_pv)
            GetTruthPVInfo(topNode);
        if (_get_g4particle)
            GetG4ParticleInfo(topNode);
        if (_get_centrality)
            GetCentralityInfo(topNode);
    }

    if (_get_reco_cluster)
        GetRecoClusterInfo(topNode);

    event_ = InputFileListIndex * NEvtPerFile + eventNum;
    outtree->Fill();
    eventNum++;

    ResetVectors();

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::ResetEvent(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::ResetEvent(PHCompositeNode *topNode) Resetting internal structures, prepare for next event" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::EndRun(const int runnumber)
{
    std::cout << "dNdEtaAnalyzer::EndRun(const int runnumber) Ending Run for Run " << runnumber << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::End(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::End(PHCompositeNode *topNode) This is the End - Output to " << Form("/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/%s.root", Name().c_str()) << std::endl;

    PHTFileServer::get().cd(Form("/sphenix/user/hjheng/TrackletAna/data/MVTXRecoClusters/%s.root", Name().c_str()));
    outtree->Write("", TObject::kOverwrite);

    delete svtx_evalstack;

    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
int dNdEtaAnalyzer::Reset(PHCompositeNode *topNode)
{
    std::cout << "dNdEtaAnalyzer::Reset(PHCompositeNode *topNode) being Reset" << std::endl;
    return Fun4AllReturnCodes::EVENT_OK;
}

//____________________________________________________________________________..
void dNdEtaAnalyzer::Print(const std::string &what) const
{
    std::cout << "dNdEtaAnalyzer::Print(const std::string &what) const "
                 "Printing info for "
              << what << std::endl;
}
//____________________________________________________________________________..
void dNdEtaAnalyzer::GetCentralityInfo(PHCompositeNode *topNode)
{
    std::cout << "Get centrality info." << std::endl;

    // Centrality info
    m_CentInfo = findNode::getClass<CentralityInfo>(topNode, "CentralityInfo");
    if (!m_CentInfo)
    {
        std::cout << PHWHERE << "Error, can't find CentralityInfov1" << std::endl;
        return;
    }

    centrality_bimp_ = m_CentInfo->get_centile(CentralityInfo::PROP::bimp);
    centrality_impactparam_ = m_CentInfo->get_quantity(CentralityInfo::PROP::bimp);
    centrality_mbd_ = m_CentInfo->get_centile(CentralityInfo::PROP::mbd_NS);
    centrality_mbdquantity_ = m_CentInfo->get_quantity(CentralityInfo::PROP::mbd_NS);
    std::cout << "Centrality: (bimp,impactparam) = (" << centrality_bimp_ << ", " << centrality_impactparam_ << "); (mbd,mbdquantity) = (" << centrality_mbd_ << ", " << centrality_mbdquantity_ << ")"
              << std::endl;
}
//____________________________________________________________________________..
void dNdEtaAnalyzer::GetMVTXhitsInfo(PHCompositeNode *topNode)
{
    std::cout << "Get MVTX hits info." << std::endl;
    int _NNullG4hit = 0, _NValidG4hit = 0, _NValidMvtxG4hit = 0, _NValidNonMvtxG4hit = 0;
    uint64_t previousCluster = 0;
    TrkrHitSetContainer::ConstRange hitset_range = hitsets->getHitSets(TrkrDefs::TrkrId::mvtxId);
    // TrkrHitSetContainer::ConstRange hitset_range = hitsets->getHitSets(TrkrDefs::TrkrId::inttId);
    // TrkrHitSetContainer::ConstRange hitset_range = hitsets->getHitSets(TrkrDefs::TrkrId::tpcId);
    // TrkrHitSetContainer::ConstRange hitset_range = hitsetcontainer->getHitSets();
    for (TrkrHitSetContainer::ConstIterator hitset_iter = hitset_range.first; hitset_iter != hitset_range.second; ++hitset_iter)
    {
        TrkrDefs::hitsetkey hitset_key = hitset_iter->first;
        TrkrHitSet::ConstRange hit_range = hitset_iter->second->getHits();
        for (TrkrHitSet::ConstIterator hit_iter = hit_range.first; hit_iter != hit_range.second; ++hit_iter)
        {
            TrkrDefs::hitkey hitKey = hit_iter->first;

            TrkrHitLayerID_.push_back(TrkrDefs::getLayer(hitset_key));
            TrkrHitStaveID_.push_back(MvtxDefs::getStaveId(hitset_key));
            TrkrHitChipID_.push_back(MvtxDefs::getChipId(hitset_key));
            TrkrHitRow_.push_back(MvtxDefs::getRow(hitKey));
            TrkrHitColumn_.push_back(MvtxDefs::getCol(hitKey));

            // Match to PHG4hit
            PHG4Hit *hit = hiteval->max_truth_hit_by_energy(hitKey, TrkrDefs::TrkrId::mvtxId); // PHG4Hit
            // PHG4Hit *hit = hiteval->max_truth_hit_by_energy(hitKey,
            // TrkrDefs::TrkrId::inttId); // PHG4Hit PHG4Hit *hit =
            // hiteval->max_truth_hit_by_energy(hitKey,
            // TrkrDefs::TrkrId::tpcId); // PHG4Hit
            if (hit == nullptr)
            {
                _NNullG4hit++;
                G4HitX_.push_back(NAN);
                G4HitY_.push_back(NAN);
                G4HitZ_.push_back(NAN);
                G4HitT_.push_back(NAN);
                G4Hit_IsSameClus_.push_back(-1);
                G4HitLayer_.push_back(-1);
            }
            else
            {
                G4HitX_.push_back(hit->get_avg_x());
                G4HitY_.push_back(hit->get_avg_y());
                G4HitZ_.push_back(hit->get_avg_z());
                G4HitT_.push_back(hit->get_avg_t());
                G4HitLayer_.push_back(hit->get_layer());

                TrkrDefs::cluskey clusKey = clustereval->best_cluster_from(hit);
                std::vector<int> layerID_mvtx{0, 1, 2};
                // std::vector<int> layerID_mvtx{3, 4, 5, 6};
                // std::vector<int> layerID_mvtx;
                // for (int l=7;l<7+48;l++){ layerID_mvtx.push_back(l); }

                std::vector<int>::iterator it = std::find(layerID_mvtx.begin(), layerID_mvtx.end(), hit->get_layer());
                if (clusKey == previousCluster)
                {
                    G4Hit_IsSameClus_.push_back(1);
                }
                else
                {
                    G4Hit_IsSameClus_.push_back(0);
                    previousCluster = clusKey;
                    _NValidG4hit++;
                    if (it != layerID_mvtx.end())
                    {
                        _NValidMvtxG4hit++;
                    }
                    else
                    {
                        _NValidNonMvtxG4hit++;
                    }
                }
            }
        }
    }
    std::cout << "NNullG4hit = " << _NNullG4hit << "; NValidG4hit = " << _NValidG4hit << "; NValidMvtxG4hit = " << _NValidMvtxG4hit << "; NValidNonMvtxG4hit = " << _NValidNonMvtxG4hit << std::endl;
    NValidG4hit_ = _NValidG4hit;
    NValidMvtxG4hit_ = _NValidMvtxG4hit;
}
//____________________________________________________________________________..
PHG4Particle *dNdEtaAnalyzer::GetG4PAncestor(PHG4Particle *p)
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
void dNdEtaAnalyzer::GetRecoClusterInfo(PHCompositeNode *topNode)
{
    std::cout << "Get reconstructed cluster info." << std::endl;

    std::vector<int> _NMVTXClus = {0, 0, 0};
    std::vector<int> G4PfromClus_IsValid = {0, 0};
    std::map<int, unsigned int> AncG4P_cluster_count;
    AncG4P_cluster_count.clear();

    // Reference:
    // https://github.com/sPHENIX-Collaboration/coresoftware/blob/master/simulation/g4simulation/g4eval/SvtxEvaluator.cc#L1731-L1984
    // for (const auto &hitsetkey : dst_clustermap->getHitSetKeys())
    for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::mvtxId))
    {
        auto range = dst_clustermap->getClusters(hitsetkey);
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            // std::cout << "----------" << std::endl;
            TrkrDefs::cluskey ckey = iter->first;
            TrkrCluster *cluster = dst_clustermap->findCluster(ckey);

            // unsigned int trkrId = TrkrDefs::getTrkrId(ckey);
            // if (trkrId != TrkrDefs::mvtxId)
            //     continue; // we want only MVTX clusters

            _NMVTXClus[TrkrDefs::getLayer(ckey)]++;
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

    NMVTXClus_ = _NMVTXClus[0] + _NMVTXClus[1] + _NMVTXClus[2];
    MVTXL1_occupancy_ = _NMVTXClus[0];
    std::cout << "NMVTXClus (total,0,1,2)=(" << NMVTXClus_ << "," << _NMVTXClus[0] << "," << _NMVTXClus[1] << "," << _NMVTXClus[2] << ")" << std::endl;
    std::cout << "Size of G4PfromClus_PID_=" << G4PfromClus_PID_.size() << "; Number of non-null PHG4Particle ptr=" << G4PfromClus_IsValid[0]
              << ", number of null PHG4Particle ptr=" << G4PfromClus_IsValid[1] << std::endl;

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
void dNdEtaAnalyzer::GetTruthPVInfo(PHCompositeNode *topNode)
{
    std::cout << "Get truth primary vertex info." << std::endl;

    auto vrange = m_truth_info->GetPrimaryVtxRange();
    const auto prange = m_truth_info->GetPrimaryParticleRange();

    std::map<int, unsigned int> vertex_particle_count;
    vertex_particle_count.clear();
    std::map<int, unsigned int> vertex_hit_count;
    vertex_hit_count.clear();
    // std::vector<int> layerID_mvtx{0, 1, 2};
    for (auto iter = prange.first; iter != prange.second; ++iter) // process all primary paricle
    {
        ++vertex_particle_count[iter->second->get_vtx_id()];

        // PHG4Particle *particle = iter->second;
        std::set<PHG4Hit *> g4hits = truth_eval->all_truth_hits(iter->second);
        for (auto hit : g4hits)
        {
            std::vector<int> layerID_mvtx{0, 1, 2};
            std::vector<int>::iterator it = std::find(layerID_mvtx.begin(), layerID_mvtx.end(), hit->get_layer());
            if (it != layerID_mvtx.end())
            {
                ++vertex_hit_count[iter->second->get_vtx_id()];
                HitfromG4P_X_.push_back(hit->get_avg_x());
                HitfromG4P_Y_.push_back(hit->get_avg_y());
                HitfromG4P_Z_.push_back(hit->get_avg_z());
            }
        }
    }

    std::map<int, unsigned int> vertex_cluster_count;
    vertex_cluster_count.clear();
    for (const auto &hitsetkey : dst_clustermap->getHitSetKeys(TrkrDefs::mvtxId))
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
    int TruthPV_idx = 0, tmp_Npart = -999;
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

            if ((int)vertex_particle_count[point_id] > tmp_Npart)
            {
                tmp_Npart = vertex_particle_count[point_id];
                TruthPV_mostNpart_x_ = point->get_x();
                TruthPV_mostNpart_y_ = point->get_y();
                TruthPV_mostNpart_z_ = point->get_z();
                TruthPV_mostNpart_Npart_ = vertex_particle_count[point_id];
            }

            // std::cout << "TruthPV_idx = " << TruthPV_idx << "(x,y,z,t,embed)
            // = (" << point->get_x() << "," << point->get_y() << "," <<
            // point->get_z() << "," << point->get_t() << "," <<
            // m_truth_info->isEmbededVtx(point_id) << ")" << std::endl;
            std::cout << "TruthPV_idx = " << TruthPV_idx << "(x,y,z,t,Npart,NMvtxhits,NMvtxClus,embed) = (" << point->get_x() << "," << point->get_y() << "," << point->get_z() << "," << point->get_t()
                      << "," << vertex_particle_count[point_id] << "," << vertex_hit_count[point_id] << "," << vertex_cluster_count[point_id] << "," << m_truth_info->isEmbededVtx(point_id) << ")"
                      << std::endl;
            TruthPV_idx++;
        }
    }

    NTruthVtx_ = (int)TruthPV_x_.size();
}

//____________________________________________________________________________..
void dNdEtaAnalyzer::GetG4ParticleInfo(PHCompositeNode *topNode)
{
    std::cout << "Get truth G4Particle info." << std::endl;

    // const auto prange = m_truth_info->GetPrimaryParticleRange();
    const auto prange = m_truth_info->GetParticleRange();
    for (auto iter = prange.first; iter != prange.second; ++iter)
    {
        PHG4Particle *particle = iter->second;
        // particle->identify();

        G4PartPID_.push_back(particle->get_pid());
        G4PartTrackID_.push_back(particle->get_track_id());
        G4PartVtxID_.push_back(particle->get_vtx_id());
        G4PartParentID_.push_back(particle->get_parent_id());
        G4PartPrimaryID_.push_back(particle->get_primary_id());
        TLorentzVector g4part;
        g4part.SetPxPyPzE(particle->get_px(), particle->get_py(), particle->get_pz(), particle->get_e());
        G4PartE_.push_back(particle->get_e());
        G4PartPt_.push_back(g4part.Pt());
        G4PartEta_.push_back(g4part.Eta());
        G4PartPhi_.push_back(g4part.Phi());

        // printf("(PID,TrackID,ParentID,PrimaryID)=(%d,%d,%d,%d),
        // (pt,eta,phi,E)=(%f,%f,%f,%f)\n", particle->get_pid(),
        // particle->get_track_id(), particle->get_parent_id(),
        // particle->get_primary_id(), g4part.Pt(), g4part.Eta(), g4part.Phi(),
        // particle->get_e());
    }
    // To-Do: add decayed particle (simulated particles)
}

//____________________________________________________________________________..
void dNdEtaAnalyzer::ResetVectors()
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
    CleanVec(TruthPV_x_);
    CleanVec(TruthPV_y_);
    CleanVec(TruthPV_z_);
    CleanVec(TruthPV_Npart_);
    CleanVec(TruthPV_Nhits_);
    CleanVec(TruthPV_NClus_);
    CleanVec(TruthPV_t_);
    CleanVec(TruthPV_embed_);
    CleanVec(G4PartPID_);
    CleanVec(G4PartTrackID_);
    CleanVec(G4PartVtxID_);
    CleanVec(G4PartParentID_);
    CleanVec(G4PartPrimaryID_);
    CleanVec(G4PartPt_);
    CleanVec(G4PartEta_);
    CleanVec(G4PartPhi_);
    CleanVec(G4PartE_);
    CleanVec(G4PfromClus_PID_);
    CleanVec(G4PfromClus_TrackID_);
    CleanVec(G4PfromClus_VtxID_);
    CleanVec(G4PfromClus_ParentID_);
    CleanVec(G4PfromClus_PrimaryID_);
    CleanVec(G4PfromClus_Px_);
    CleanVec(G4PfromClus_Py_);
    CleanVec(G4PfromClus_Pz_);
    CleanVec(G4PfromClus_Pt_);
    CleanVec(G4PfromClus_Eta_);
    CleanVec(G4PfromClus_Phi_);
    CleanVec(G4PfromClus_E_);
    CleanVec(G4PfromClus_MomPID_);
    CleanVec(G4PfromClus_MomTrackID_);
    CleanVec(G4PfromClus_MomVtxID_);
    CleanVec(G4PfromClus_MomParentID_);
    CleanVec(G4PfromClus_MomPrimaryID_);
    CleanVec(G4PfromClus_GMomPID_);
    CleanVec(G4PfromClus_GMomTrackID_);
    CleanVec(G4PfromClus_GMomVtxID_);
    CleanVec(G4PfromClus_GMomParentID_);
    CleanVec(G4PfromClus_GMomPrimaryID_);
    CleanVec(G4PfromClus_AncPID_);
    CleanVec(G4PfromClus_AncTrackID_);
    CleanVec(G4PfromClus_AncVtxID_);
    CleanVec(G4PfromClus_AncParentID_);
    CleanVec(G4PfromClus_AncPrimaryID_);
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
    CleanVec(TrkrHitLayerID_);
    CleanVec(TrkrHitStaveID_);
    CleanVec(TrkrHitChipID_);
    CleanVec(TrkrHitRow_);
    CleanVec(TrkrHitColumn_);
    CleanVec(G4HitX_);
    CleanVec(G4HitY_);
    CleanVec(G4HitZ_);
    CleanVec(G4HitT_);
    CleanVec(G4HitLayer_);
    CleanVec(G4Hit_IsSameClus_);
    CleanVec(HitfromG4P_X_);
    CleanVec(HitfromG4P_Y_);
    CleanVec(HitfromG4P_Z_);
}
