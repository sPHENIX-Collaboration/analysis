// Tell emacs that this is a C++ source
//  -*- C++ -*-.
#ifndef DNDETAANALYZER_H
#define DNDETAANALYZER_H

#include <fun4all/SubsysReco.h>

#include <g4eval/SvtxEvalStack.h>
#include <g4eval/SvtxClusterEval.h>
#include <g4eval/SvtxHitEval.h>
#include <g4eval/SvtxTruthEval.h>
#include <g4main/PHG4Hit.h>
#include <g4main/PHG4Particle.h>
#include <g4main/PHG4TruthInfoContainer.h>
#include <g4main/PHG4VtxPoint.h>
#include <phool/getClass.h>

#include <trackbase/MvtxDefs.h>
#include <phool/getClass.h>
#include <trackbase/TrkrCluster.h>
#include <trackbase/TrkrClusterv4.h>
#include <trackbase/TrkrClusterContainerv4.h>
#include <trackbase/TrkrHitSet.h>
#include <trackbase/TrkrHitSetContainer.h>
#include <trackbase/TrkrDefs.h>
#include <trackbase/TrkrClusterHitAssoc.h>
#include <trackbase/ActsGeometry.h>
#include <trackbase_historic/SvtxTrackMap.h>
#include <trackbase_historic/SvtxTrack.h>
#include <trackbase_historic/SvtxVertexMap.h>
#include <trackbase_historic/SvtxVertex.h>
#include <trackbase_historic/ActsTransformations.h>
#include <centrality/CentralityInfo.h>
// #include <centrality/CentralityInfov1.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <math.h>

#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TLorentzVector.h>

class PHCompositeNode;
class SvtxTrack;
class SvtxTrackMap;
class TrkrHitSetContainer;
class TrkrClusterContainer;
class CentralityInfo;

class dNdEtaAnalyzer : public SubsysReco
{
public:
    dNdEtaAnalyzer(const std::string &name = "dNdEtaAnalyzer", const std::string &outputfile = "MVTXdNdEta.root", const bool &isData = false, const int &inputFileListIndex = 0, const int &nEvtPerFile = 0);

    ~dNdEtaAnalyzer() override;

    /** Called during initialization.
        Typically this is where you can book histograms, and e.g.
        register them to Fun4AllServer (so they can be output to file
        using Fun4AllServer::dumpHistos() method).
     */
    int Init(PHCompositeNode *topNode) override;

    /** Called for first event when run number is known.
        Typically this is where you may want to fetch data from
        database, because you know the run number. A place
        to book histograms which have to know the run number.
     */
    int InitRun(PHCompositeNode *topNode) override;

    /** Called for each event.
        This is where you do the real work.
     */
    int process_event(PHCompositeNode *topNode) override;

    /// Clean up internals after each event.
    int ResetEvent(PHCompositeNode *topNode) override;

    /// Called at the end of each run.
    int EndRun(const int runnumber) override;

    /// Called at the end of all processing.
    int End(PHCompositeNode *topNode) override;

    /// Reset
    int Reset(PHCompositeNode * /*topNode*/) override;

    void Print(const std::string &what = "ALL") const override;
    void GetTruthPV(bool b) { _get_truth_pv = b; }
    void GetG4Particle(bool b) { _get_g4particle = b; }
    void GetRecoCluster(bool b) { _get_reco_cluster = b; }
    void GetMVTXhits(bool b) { _get_mvtx_hit = b; }
    void GetCentrality(bool b) { _get_centrality = b; }

private:
    void ResetVectors();
    void GetTruthPVInfo(PHCompositeNode *topNode);
    void GetG4ParticleInfo(PHCompositeNode *topNode);
    void GetRecoClusterInfo(PHCompositeNode *topNode);
    void GetMVTXhitsInfo(PHCompositeNode *topNode);
    void GetCentralityInfo(PHCompositeNode *topNode);
    PHG4Particle* GetG4PAncestor(PHG4Particle *p);

    bool _get_truth_pv;
    bool _get_g4particle;
    bool _get_reco_cluster;
    bool _get_mvtx_hit;
    bool _get_centrality;

    unsigned int eventNum = 0;
    std::string _outputFile;
    bool IsData;
    int InputFileListIndex;
    int NEvtPerFile;

    TTree *outtree;
    int event_;
    float centrality_bimp_, centrality_impactparam_, centrality_mbd_, centrality_mbdquantity_;
    int MVTXL1_occupancy_;
    // Truth primary vertex information
    float TruthPV_trig_x_, TruthPV_trig_y_, TruthPV_trig_z_, TruthPV_mostNpart_x_, TruthPV_mostNpart_y_, TruthPV_mostNpart_z_;
    int NTruthVtx_, TruthPV_trig_Npart_, TruthPV_mostNpart_Npart_;
    std::vector<float> TruthPV_x_, TruthPV_y_, TruthPV_z_, TruthPV_t_, TruthPV_embed_;
    std::vector<int> TruthPV_Npart_, TruthPV_Nhits_, TruthPV_NClus_;
    // Truth G4Particle information
    std::vector<int> G4PartPID_, G4PartTrackID_, G4PartVtxID_, G4PartParentID_, G4PartPrimaryID_;
    std::vector<float> G4PartPx_, G4PartPy_, G4PartPz_, G4PartPt_, G4PartEta_, G4PartPhi_, G4PartE_;
    // Reconstructed cluster information & Matching
    int NMVTXClus_;
    std::vector<int> ClusLayer_, ClusHitcount_;
    std::vector<float> ClusX_, ClusY_, ClusZ_, ClusR_, ClusPhi_, ClusEta_;
    std::vector<unsigned int> ClusAdc_;
    std::vector<float> ClusPhiSize_, ClusZSize_;
    std::vector<int> G4PfromClus_PID_, G4PfromClus_TrackID_, G4PfromClus_VtxID_, G4PfromClus_ParentID_, G4PfromClus_PrimaryID_;
    std::vector<float> G4PfromClus_Px_, G4PfromClus_Py_, G4PfromClus_Pz_, G4PfromClus_Pt_, G4PfromClus_Eta_, G4PfromClus_Phi_, G4PfromClus_E_;
    std::vector<int> G4PfromClus_MomPID_, G4PfromClus_MomTrackID_, G4PfromClus_MomVtxID_, G4PfromClus_MomParentID_, G4PfromClus_MomPrimaryID_;
    std::vector<int> G4PfromClus_GMomPID_, G4PfromClus_GMomTrackID_, G4PfromClus_GMomVtxID_, G4PfromClus_GMomParentID_, G4PfromClus_GMomPrimaryID_;
    std::vector<int> G4PfromClus_AncPID_, G4PfromClus_AncTrackID_, G4PfromClus_AncVtxID_, G4PfromClus_AncParentID_, G4PfromClus_AncPrimaryID_;
    std::vector<float> UniqueAncG4P_Px_, UniqueAncG4P_Py_, UniqueAncG4P_Pz_, UniqueAncG4P_Pt_, UniqueAncG4P_Eta_, UniqueAncG4P_Phi_, UniqueAncG4P_E_;
    std::vector<int> UniqueAncG4P_PID_, UniqueAncG4P_TrackPID_, UniqueAncG4P_VtxPID_, UniqueAncG4P_ParentPID_, UniqueAncG4P_PrimaryPID_;
    std::vector<double> UniqueAncG4P_IonCharge_;
    std::vector<int> UniqueAncG4P_TrackID_, UniqueAncG4P_NClus_;
    // MVTX hit information
    int NValidG4hit_, NValidMvtxG4hit_;
    std::vector<unsigned int> TrkrHitLayerID_, TrkrHitStaveID_, TrkrHitChipID_;
    std::vector<uint16_t> TrkrHitRow_, TrkrHitColumn_;
    std::vector<float> G4HitX_, G4HitY_, G4HitZ_, G4HitT_;
    std::vector<int> G4Hit_IsSameClus_, G4Hit_IsMvtx_, G4HitLayer_;
    std::vector<float> HitfromG4P_X_, HitfromG4P_Y_, HitfromG4P_Z_;

    // must initialize as null pointer otherwise weird things happen...
    SvtxEvalStack *svtx_evalstack = nullptr;
    SvtxTruthEval *truth_eval = nullptr;
    SvtxClusterEval *clustereval = nullptr;
    SvtxHitEval *hiteval = nullptr;

    TrkrClusterContainerv4 *dst_clustermap = nullptr;
    TrkrClusterHitAssoc *clusterhitmap = nullptr;
    TrkrHitSetContainer *hitsets = nullptr;
    ActsGeometry *_tgeometry = nullptr;
    PHG4TruthInfoContainer *m_truth_info = nullptr;
    CentralityInfo *m_CentInfo = nullptr;
};

#endif // DNDETAANALYZER_H
