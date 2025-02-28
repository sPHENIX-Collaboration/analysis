#include "TrackletHistogram.h"

TrackletHistogram::TrackletHistogram(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,
    std::string output_file_name_suffix_in,

    std::pair<bool, TH1D*> vtxZReweight_in,
    bool BcoFullDiffCut_in,
    bool INTT_vtxZ_QA_in,
    bool isWithRotate_in,
    std::pair<bool, std::pair<double, double>> isClusQA_in
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),

    vtxZReweight(vtxZReweight_in),
    BcoFullDiffCut(BcoFullDiffCut_in),
    INTT_vtxZ_QA(INTT_vtxZ_QA_in),
    isWithRotate(isWithRotate_in),
    isClusQA(isClusQA_in)
{
    PrepareInputRootFile();

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;

    // note : for the output
    tracklet_histogram_fill = new TrackletHistogramFill(
        process_id,
        runnumber,
        output_directory,
        output_file_name_suffix
    );
    tracklet_histogram_fill -> SetvtxZReweight(vtxZReweight.first);
    
    if (vtxZReweight.first) {
        tracklet_histogram_fill -> SetVtxZReweightHist(vtxZReweight.second);
    }

    tracklet_histogram_fill -> SetBcoFullDiffCut(BcoFullDiffCut);
    tracklet_histogram_fill -> SetINTT_vtxZ_QA(INTT_vtxZ_QA);
    tracklet_histogram_fill -> SetWithRotate(isWithRotate);
    tracklet_histogram_fill -> SetClusQA(isClusQA);
    
    tracklet_histogram_fill -> PrepareOutPutFileName();
    tracklet_histogram_fill -> PrepareOutPutRootFile();
    tracklet_histogram_fill -> PrepareHistograms();
}



std::map<std::string, int> TrackletHistogram::GetInputTreeBranchesMap(TTree * m_tree_in)
{
    std::map<std::string, int> branch_map;
    TObjArray * branch_list = m_tree_in -> GetListOfBranches();
    for (int i = 0; i < branch_list -> GetEntries(); i++)
    {
        TBranch * branch = dynamic_cast<TBranch*>(branch_list->At(i));
        branch_map[branch -> GetName()] = 1;
    }
    return branch_map;
}

void TrackletHistogram::PrepareInputRootFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    if (!file_in || file_in -> IsZombie() || file_in == nullptr) {
        std::cout << "Error: cannot open file: " << input_file_name << std::endl;
        exit(1);
    }

    tree_in = (TTree*)file_in -> Get("EventTree");
    
    std::map<std::string, int> branch_map = GetInputTreeBranchesMap(tree_in);
    if(branch_map.find("event") != branch_map.end()){tree_in -> SetBranchStatus("event",0);}

    if(branch_map.find("ClusEta_INTTz") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_INTTz",0);}
    if(branch_map.find("ClusEta_MBDz") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_MBDz",0);}
    if(branch_map.find("ClusPhi_AvgPV") != branch_map.end()){tree_in -> SetBranchStatus("ClusPhi_AvgPV",0);}
    if(branch_map.find("ClusEta_TrueXYZ") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_TrueXYZ",0);}
    if(branch_map.find("ClusPhi_TrueXY") != branch_map.end()){tree_in -> SetBranchStatus("ClusPhi_TrueXY",0);}

    
    if(branch_map.find("INTT_BCO") != branch_map.end()){tree_in -> SetBranchStatus("INTT_BCO",0);}
    if(branch_map.find("ClusTrkrHitSetKey") != branch_map.end()){tree_in -> SetBranchStatus("ClusTrkrHitSetKey",0);}
    if(branch_map.find("ClusTimeBucketId") != branch_map.end()){tree_in -> SetBranchStatus("ClusTimeBucketId",0);}
    
    if(branch_map.find("GL1Packet_BCO") != branch_map.end()){tree_in -> SetBranchStatus("GL1Packet_BCO",0);}
    if(branch_map.find("ncoll") != branch_map.end()){tree_in -> SetBranchStatus("ncoll",0);}
    if(branch_map.find("npart") != branch_map.end()){tree_in -> SetBranchStatus("npart",0);}
    if(branch_map.find("centrality_bimp") != branch_map.end()){tree_in -> SetBranchStatus("centrality_bimp",0);}
    if(branch_map.find("centrality_impactparam") != branch_map.end()){tree_in -> SetBranchStatus("centrality_impactparam",0);}
    if(branch_map.find("clk") != branch_map.end()){tree_in -> SetBranchStatus("clk",0);}
    if(branch_map.find("femclk") != branch_map.end()){tree_in -> SetBranchStatus("femclk",0);}
    if(branch_map.find("is_min_bias_wozdc") != branch_map.end()){tree_in -> SetBranchStatus("is_min_bias_wozdc",0);}
    if(branch_map.find("MBD_south_npmt") != branch_map.end()){tree_in -> SetBranchStatus("MBD_south_npmt",0);}
    if(branch_map.find("MBD_north_npmt") != branch_map.end()){tree_in -> SetBranchStatus("MBD_north_npmt",0);}
    if(branch_map.find("MBD_nhitsoverths_south") != branch_map.end()){tree_in -> SetBranchStatus("MBD_nhitsoverths_south",0);}
    if(branch_map.find("MBD_nhitsoverths_north") != branch_map.end()){tree_in -> SetBranchStatus("MBD_nhitsoverths_north",0);}


    // note: for reading 
    ClusX = 0;
    ClusY = 0;
    ClusZ = 0;
    ClusLayer = 0;
    ClusLadderZId = 0;
    ClusLadderPhiId = 0;
    ClusAdc = 0;
    ClusPhiSize = 0;
    
    evt_TrackletPair_vec = 0;
    evt_TrackletPairRotate_vec = 0;

    PrimaryG4P_Pt = 0;
    PrimaryG4P_Eta = 0;
    PrimaryG4P_Phi = 0;
    PrimaryG4P_E = 0;
    PrimaryG4P_PID = 0;
    PrimaryG4P_isChargeHadron = 0;

    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_south_charge_sum", &MBD_south_charge_sum);
    tree_in -> SetBranchAddress("MBD_north_charge_sum", &MBD_north_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);
    tree_in -> SetBranchAddress("MBD_charge_asymm", &MBD_charge_asymm);
    tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);

    // note : trigger tag
    tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);
    tree_in -> SetBranchAddress("MBDNSg2_vtxZ10cm", &MBDNSg2_vtxZ10cm);
    tree_in -> SetBranchAddress("MBDNSg2_vtxZ30cm", &MBDNSg2_vtxZ30cm);
    tree_in -> SetBranchAddress("MBDNSg2_vtxZ60cm", &MBDNSg2_vtxZ60cm);

    tree_in -> SetBranchAddress("ClusX", &ClusX);
    tree_in -> SetBranchAddress("ClusY", &ClusY);
    tree_in -> SetBranchAddress("ClusZ", &ClusZ);
    tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    tree_in -> SetBranchAddress("ClusLadderZId", &ClusLadderZId);
    tree_in -> SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);
    tree_in -> SetBranchAddress("ClusAdc", &ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);

    // note : INTT vertex Z
    tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
    tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);
    tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);
    tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);

    // note : the tracklet pair
    tree_in -> SetBranchAddress("TrackletPair", &evt_TrackletPair_vec);
    tree_in -> SetBranchAddress("TrackletPairRotate", &evt_TrackletPairRotate_vec);

    // note : MC
    if (runnumber == -1){
        tree_in -> SetBranchAddress("TruthPV_trig_x", &TruthPV_trig_x);
        tree_in -> SetBranchAddress("TruthPV_trig_y", &TruthPV_trig_y);
        tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);
        tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);
        tree_in -> SetBranchAddress("NPrimaryG4P", &NPrimaryG4P);
        tree_in -> SetBranchAddress("PrimaryG4P_Pt", &PrimaryG4P_Pt);
        tree_in -> SetBranchAddress("PrimaryG4P_Eta", &PrimaryG4P_Eta);
        tree_in -> SetBranchAddress("PrimaryG4P_Phi", &PrimaryG4P_Phi);
        tree_in -> SetBranchAddress("PrimaryG4P_E", &PrimaryG4P_E);
        tree_in -> SetBranchAddress("PrimaryG4P_PID", &PrimaryG4P_PID);
        tree_in -> SetBranchAddress("PrimaryG4P_isChargeHadron", &PrimaryG4P_isChargeHadron);
    }
}



void TrackletHistogram::MainProcess()
{
    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        if (i % 100 == 0) { std::cout << "Processing event " << i << " / " << run_nEvents << std::endl; }

        tracklet_histogram_fill -> FillHistogram(
            // note : MBD & centrality relevant
            MBD_z_vtx,
            is_min_bias,
            MBD_centrality,
            MBD_south_charge_sum,
            MBD_north_charge_sum,
            // MBD_charge_sum,
            // MBD_charge_asymm,
            InttBcoFullDiff_next,

            // // note : trigger tag
            MBDNSg2,
            // MBDNSg2_vtxZ10cm,
            // MBDNSg2_vtxZ30cm,
            // MBDNSg2_vtxZ60cm,

            // ClusX,
            // ClusY,
            // ClusZ,
            // ClusLayer,
            ClusLadderZId,
            // ClusLadderPhiId,
            ClusAdc,
            ClusPhiSize,

            // note : INTT vertex Z
            INTTvtxZ,
            INTTvtxZError,
            // NgroupTrapezoidal,
            // NgroupCoarse,
            TrapezoidalFitWidth,
            TrapezoidalFWHM,

            // note : the tracklet pair
            evt_TrackletPair_vec,
            evt_TrackletPairRotate_vec,

            // note : MC
            // TruthPV_trig_x,
            // TruthPV_trig_y,
            TruthPV_trig_z,
            NTruthVtx,
            NPrimaryG4P,
            // PrimaryG4P_Pt,
            PrimaryG4P_Eta,
            // PrimaryG4P_Phi,
            // PrimaryG4P_E,
            // PrimaryG4P_PID,
            PrimaryG4P_isChargeHadron
        );
    }
}

void TrackletHistogram::EndRun()
{
    tracklet_histogram_fill -> EndRun();
    file_in -> Close();
}

// h1D_centrality_bin
// h2D_MBDVtxZ_Centrality
// h1D_centrality
// h1D_centrality_MBDVtxZ%d", vtxz_bi
// h2D_InttVtxZ_Centrality
// h1D_centrality_InttVtxZ%d", vtxz_bi

// h2D_TrueEtaVtxZ_Mbin%d", Mbi
// h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbi
// h2D_Inclusive100_TrueEtaVtxZ
// h2D_Inclusive100_TrueEtaVtxZ_FineBin
// h2D_Inclusive70_TrueEtaVtxZ
// h2D_Inclusive70_TrueEtaVtxZ_FineBin
// h1D_TrueEta_Inclusive100_VtxZ%d", vtxz_bi
// h1D_TrueEta_Inclusive70_VtxZ%d", vtxz_bi
// h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, vtxz_bi



// h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d", Mbin, eta_bin, vtxz_bi
// h1D_DeltaPhi_Mbin%d_Eta%d_VtxZ%d_rotated", Mbin, eta_bin, vtxz_bi

// h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d", eta_bin, vtxz_bi
// h1D_DeltaPhi_Inclusive100_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bi
// h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d", eta_bin, vtxz_bi
// h1D_DeltaPhi_Inclusive70_Eta%d_VtxZ%d_rotated", eta_bin, vtxz_bi

// h1D_BestPair_Inclusive70_DeltaEta
// h1D_BestPair_Inclusive70_DeltaPhi
// h1D_BestPair_Inclusive70_ClusPhiSize
// h1D_BestPair_Inclusive70_ClusAdc



// h2D_Inclusive100_BestPairEtaVtxZ
// h2D_Inclusive100_BestPairEtaVtxZ_FineBin
// h2D_Inclusive70_BestPairEtaVtxZ
// h2D_Inclusive70_BestPairEtaVtxZ_FineBin
// 
