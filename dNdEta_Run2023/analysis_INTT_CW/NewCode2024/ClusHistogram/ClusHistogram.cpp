#include "ClusHistogram.h"

ClusHistogram::ClusHistogram(
    int process_id_in,
        int runnumber_in,
        int run_nEvents_in,
        std::string input_directory_in,
        std::string input_file_name_in,
        std::string output_directory_in,

        std::string output_file_name_suffix_in,
        std::pair<double, double> vertexXYIncm_in,

        std::pair<bool, TH1D*> vtxZReweight_in,
        bool BcoFullDiffCut_in,
        bool INTT_vtxZ_QA_in,
        std::pair<bool, std::pair<double, double>> isClusQA_in, // note : {adc, phi size}
        bool HaveGeoOffsetTag_in,
        std::pair<bool, int> SetRandomHits_in,
        bool RandInttZ_in,
        bool ColMulMask_in,
        int c_type_in // note : constructor type
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),

    output_file_name_suffix(output_file_name_suffix_in),
    vertexXYIncm(vertexXYIncm_in),

    vtxZReweight(vtxZReweight_in),
    BcoFullDiffCut(BcoFullDiffCut_in),
    INTT_vtxZ_QA(INTT_vtxZ_QA_in),
    isClusQA(isClusQA_in),
    HaveGeoOffsetTag(HaveGeoOffsetTag_in),
    SetRandomHits(SetRandomHits_in),
    RandInttZ(RandInttZ_in),
    ColMulMask(ColMulMask_in),

    c_type(c_type_in) // note : the constructor switch
{
    PrepareInputRootFile();

    h1D_INTT_vtxZ_reweighting = (vtxZReweight.first) ? (TH1D*)vtxZReweight.second->Clone() : nullptr;

    nCentrality_bin = centrality_edges.size() - 1;

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;

    if (HaveGeoOffsetTag == false){
        for (int layer_i = B0L0_index; layer_i <= B1L1_index; layer_i++){
            double N_layer_ladder = (layer_i == B0L0_index || layer_i == B0L1_index) ? nLadder_inner : nLadder_outer;
            for (int phi_i = 0; phi_i < N_layer_ladder; phi_i++){
                geo_offset_map[Form("%i_%i", layer_i, phi_i)] = {0, 0, 0};
            }
        }
    }

    if (RandInttZ && INTT_vtxZ_QA){
        std::cout << "RandInttZ and INTT_vtxZ_QA cannot be true at the same time" << std::endl;
        exit(1);
    }

    if (SetRandomHits.first || RandInttZ){
        rand3 = new TRandom3(0);
    }

    if (c_type == 0){
        PrepareOutPutFileName();
        PrepareOutPutRootFile();
        PrepareHistograms();
    }

}

std::map<std::string, int> ClusHistogram::GetInputTreeBranchesMap(TTree * m_tree_in)
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

void ClusHistogram::PrepareInputRootFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    if (!file_in || file_in -> IsZombie() || file_in == nullptr) {
        std::cout << "Error: cannot open file: " << input_file_name << std::endl;
        exit(1);
    }

    tree_in = (TTree*)file_in -> Get("EventTree");
    
    std::map<std::string, int> branch_map = GetInputTreeBranchesMap(tree_in);
    if(branch_map.find("event") != branch_map.end()){tree_in -> SetBranchStatus("event",0);}

    // if(branch_map.find("ClusEta_INTTz") != branch_map.end()){tree_in -> SetBranchStatus("ClusEta_INTTz",0);}
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
    
    if(branch_map.find("TrackletPair") != branch_map.end()) {tree_in -> SetBranchStatus("TrackletPair", 0);}
    if(branch_map.find("TrackletPairRotate") != branch_map.end()) {tree_in -> SetBranchStatus("TrackletPairRotate", 0);}


    // note: for reading 
    ClusX = 0;
    ClusY = 0;
    ClusZ = 0;
    ClusLayer = 0;
    ClusLadderZId = 0;
    ClusLadderPhiId = 0;
    ClusAdc = 0;
    ClusPhiSize = 0;
    ClusEta_INTTz = 0;
    
    // evt_TrackletPair_vec = 0;
    // evt_TrackletPairRotate_vec = 0;

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
    if (runnumber != -1) {
        if(branch_map.find("InttBcoFullDiff_next") != branch_map.end()){
            tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next);
        }
    }

    // note : trigger tag
    if (runnumber != -1){ // note : for data
        if(branch_map.find("MBDNSg2") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);}
        if(branch_map.find("MBDNSg2_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ10cm", &MBDNSg2_vtxZ10cm);}
        if(branch_map.find("MBDNSg2_vtxZ30cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ30cm", &MBDNSg2_vtxZ30cm);}
        if(branch_map.find("MBDNSg2_vtxZ60cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ60cm", &MBDNSg2_vtxZ60cm);}
    }

    tree_in -> SetBranchAddress("ClusX", &ClusX);
    tree_in -> SetBranchAddress("ClusY", &ClusY);
    tree_in -> SetBranchAddress("ClusZ", &ClusZ);
    tree_in -> SetBranchAddress("ClusLayer", &ClusLayer);
    tree_in -> SetBranchAddress("ClusLadderZId", &ClusLadderZId);
    tree_in -> SetBranchAddress("ClusLadderPhiId", &ClusLadderPhiId);
    tree_in -> SetBranchAddress("ClusAdc", &ClusAdc);
    tree_in -> SetBranchAddress("ClusPhiSize", &ClusPhiSize);
    if(branch_map.find("ClusEta_INTTz") != branch_map.end()) {tree_in -> SetBranchAddress("ClusEta_INTTz", &ClusEta_INTTz);}

    // note : INTT vertex Z
    if(branch_map.find("INTTvtxZ") != branch_map.end()) {tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);}
    if(branch_map.find("INTTvtxZError") != branch_map.end()) {tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);}
    if(branch_map.find("NgroupTrapezoidal") != branch_map.end()) {tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);}
    if(branch_map.find("NgroupCoarse") != branch_map.end()) {tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);}
    if(branch_map.find("TrapezoidalFitWidth") != branch_map.end()) {tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);}
    if(branch_map.find("TrapezoidalFWHM") != branch_map.end()) {tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);}

    // note : the tracklet pair
    // tree_in -> SetBranchAddress("TrackletPair", &evt_TrackletPair_vec);
    // tree_in -> SetBranchAddress("TrackletPairRotate", &evt_TrackletPairRotate_vec);

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

void ClusHistogram::PrepareOutPutFileName()
{
    std::string job_index = std::to_string( process_id );
    int job_index_len = 5;
    job_index.insert(0, job_index_len - job_index.size(), '0');

    std::string runnumber_str = std::to_string( runnumber );
    if (runnumber != -1){
        int runnumber_str_len = 8;
        runnumber_str.insert(0, runnumber_str_len - runnumber_str.size(), '0');
    }

    if (output_file_name_suffix.size() > 0 && output_file_name_suffix[0] != '_') {
        output_file_name_suffix = "_" + output_file_name_suffix;
    }

    output_filename = "ClusHist";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;
    output_filename += (vtxZReweight.first) ? "_vtxZReweight" : "";
    output_filename += (BcoFullDiffCut && runnumber != -1) ? "_BcoFullDiffCut" : "";
    output_filename += (INTT_vtxZ_QA) ? "_VtxZQA" : "";
    output_filename += (isClusQA.first) ? Form("_ClusQAAdc%.0fPhiSize%.0f",isClusQA.second.first,isClusQA.second.second) : "";
    output_filename += (ColMulMask) ? "_ColMulMask" : "";
    output_filename += (HaveGeoOffsetTag) ? "_GeoOffset" : "";
    output_filename += (SetRandomHits.first) ? Form("_Rand%dHits",SetRandomHits.second) : "";
    output_filename += (RandInttZ) ? "_RandInttZ" : "";
    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}


void ClusHistogram::PrepareOutPutRootFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
    tree_out_par = new TTree("tree_par", "used parameters");

    tree_out_par -> Branch("process_id", &process_id);
    tree_out_par -> Branch("runnumber", &runnumber);
    tree_out_par -> Branch("vtxZReweight", &vtxZReweight.first);
    tree_out_par -> Branch("BcoFullDiffCut", &BcoFullDiffCut);
    tree_out_par -> Branch("INTT_vtxZ_QA", &INTT_vtxZ_QA);
    tree_out_par -> Branch("isClusQA", &isClusQA.first);
    tree_out_par -> Branch("isClusQA_adc", &isClusQA.second.first);
    tree_out_par -> Branch("isClusQA_phiSize", &isClusQA.second.second);

    tree_out_par -> Branch("centrality_edges", &centrality_edges);
    tree_out_par -> Branch("nCentrality_bin", &nCentrality_bin);

    tree_out_par -> Branch("CentralityFineEdge_min", &CentralityFineEdge_min);
    tree_out_par -> Branch("CentralityFineEdge_max", &CentralityFineEdge_max);
    tree_out_par -> Branch("nCentralityFineBin", &nCentralityFineBin);

    tree_out_par -> Branch("EtaEdge_min", &EtaEdge_min);
    tree_out_par -> Branch("EtaEdge_max", &EtaEdge_max);
    tree_out_par -> Branch("nEtaBin", &nEtaBin);

    tree_out_par -> Branch("VtxZEdge_min", &VtxZEdge_min);
    tree_out_par -> Branch("VtxZEdge_max", &VtxZEdge_max);
    tree_out_par -> Branch("nVtxZBin", &nVtxZBin);

    tree_out_par -> Branch("typeA_sensorZID", &typeA_sensorZID);
    tree_out_par -> Branch("cut_GlobalMBDvtxZ", &cut_GlobalMBDvtxZ);   
    tree_out_par -> Branch("cut_vtxZDiff", &cut_vtxZDiff);
    tree_out_par -> Branch("cut_TrapezoidalFitWidth", &cut_TrapezoidalFitWidth);
    tree_out_par -> Branch("cut_TrapezoidalFWHM", &cut_TrapezoidalFWHM);
    tree_out_par -> Branch("cut_INTTvtxZError", &cut_INTTvtxZError);

    tree_out_par -> Branch("cut_InttBcoFullDIff_next", &cut_InttBcoFullDIff_next);
}

void ClusHistogram::PrepareHistograms()
{
    h1D_INTTvtxZ = new TH1D("h1D_INTTvtxZ", "h1D_INTTvtxZ;INTT vtxZ [cm];Entries", 60, -60, 60);

    h1D_centrality_bin = new TH1D("h1D_centrality_bin","h1D_centrality_bin;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%
    h1D_centrality_bin_weighted = new TH1D("h1D_centrality_bin_weighted","h1D_centrality_bin_weighted;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%

    h1D_vtxz_template = new TH1D("h1D_vtxz_template", "h1D_vtxz_template", nVtxZBin, VtxZEdge_min, VtxZEdge_max); // note : coarse

    h1D_GoodColMap_ZId = new TH1D("h1D_GoodColMap_ZId","h1D_GoodColMap_ZId;ClusZ [cm];Entries",nZbin, Zmin, Zmax);

    h1D_centrality_map.clear();
    h1D_centrality_map.insert( std::make_pair(
            "h1D_centrality",
            new TH1D("h1D_centrality", "h1D_centrality;Centrality [%];Entries", nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    );

    for (int VtxZ_bin = 0; VtxZ_bin < nVtxZBin; VtxZ_bin++)
    {
        h1D_centrality_map.insert( std::make_pair(
                Form("h1D_centrality_InttVtxZ%d", VtxZ_bin),
                new TH1D(Form("h1D_centrality_InttVtxZ%d", VtxZ_bin), Form("h1D_centrality_InttVtxZ%d;Centrality [%];Entries",VtxZ_bin), nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
            )
        );
    }

    h1D_NClusEta_map.clear(); // note : {inner, outer, typeA, all} x {Mbin x vtxZ bin}
    for (int VtxZ_bin = 0; VtxZ_bin < nVtxZBin; VtxZ_bin++){
        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){

            h1D_NClusEta_map.insert( // note : all, inner
                std::make_pair(
                    Form("h1D_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin),
                    new TH1D(Form("h1D_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin), Form("h1D_inner_NClusEta_Mbin%d_VtxZ%d; Clus #eta (inner);Entries", Mbin, VtxZ_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                )
            );

            h1D_NClusEta_map.insert( // note : all, outer
                std::make_pair(
                    Form("h1D_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin),
                    new TH1D(Form("h1D_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin), Form("h1D_outer_NClusEta_Mbin%d_VtxZ%d; Clus #eta (outer);Entries", Mbin, VtxZ_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                )
            );

            h1D_NClusEta_map.insert( // note : typeA, inner
                std::make_pair(
                    Form("h1D_typeA_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin),
                    new TH1D(Form("h1D_typeA_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin), Form("h1D_typeA_inner_NClusEta_Mbin%d_VtxZ%d; Clus #eta (inner, type A);Entries", Mbin, VtxZ_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                )
            );

            h1D_NClusEta_map.insert( // note : typeA, outer
                std::make_pair(
                    Form("h1D_typeA_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin),
                    new TH1D(Form("h1D_typeA_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin), Form("h1D_typeA_outer_NClusEta_Mbin%d_VtxZ%d; Clus #eta (outer, type A);Entries", Mbin, VtxZ_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                )
            );
        }
    }

    h2D_NClusEtaVtxZ_map.clear(); // note : {inner, outer, typeA, all} x {Mbin}
    for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++){

        h2D_NClusEtaVtxZ_map.insert( // note : all, inner
            std::make_pair(
                Form("h2D_inner_NClusEta_Mbin%d", Mbin),
                new TH2D(Form("h2D_inner_NClusEta_Mbin%d", Mbin), Form("h2D_inner_NClusEta_Mbin%d; Clus #eta (inner);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : all, outer
            std::make_pair(
                Form("h2D_outer_NClusEta_Mbin%d", Mbin),
                new TH2D(Form("h2D_outer_NClusEta_Mbin%d", Mbin), Form("h2D_outer_NClusEta_Mbin%d; Clus #eta (outer);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : typeA, inner
            std::make_pair(
                Form("h2D_typeA_inner_NClusEta_Mbin%d", Mbin),
                new TH2D(Form("h2D_typeA_inner_NClusEta_Mbin%d", Mbin), Form("h2D_typeA_inner_NClusEta_Mbin%d; Clus #eta (inner, type A);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : typeA, outer
            std::make_pair(
                Form("h2D_typeA_outer_NClusEta_Mbin%d", Mbin),
                new TH2D(Form("h2D_typeA_outer_NClusEta_Mbin%d", Mbin), Form("h2D_typeA_outer_NClusEta_Mbin%d; Clus #eta (outer, type A);INTT vtxZ [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
            )
        );

        // Division: ---Fine Bin-----------------------------------------------------------------------------------------------------------------------------------------
        h2D_NClusEtaVtxZ_map.insert( // note : all, inner
            std::make_pair(
                Form("h2D_inner_NClusEta_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_inner_NClusEta_Mbin%d_FineBin", Mbin), Form("h2D_inner_NClusEta_Mbin%d_FineBin; Clus #eta (inner);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : all, outer
            std::make_pair(
                Form("h2D_outer_NClusEta_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_outer_NClusEta_Mbin%d_FineBin", Mbin), Form("h2D_outer_NClusEta_Mbin%d_FineBin; Clus #eta (outer);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : typeA, inner
            std::make_pair(
                Form("h2D_typeA_inner_NClusEta_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_typeA_inner_NClusEta_Mbin%d_FineBin", Mbin), Form("h2D_typeA_inner_NClusEta_Mbin%d_FineBin; Clus #eta (inner, type A);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max)
            )
        );

        h2D_NClusEtaVtxZ_map.insert( // note : typeA, outer
            std::make_pair(
                Form("h2D_typeA_outer_NClusEta_Mbin%d_FineBin", Mbin),
                new TH2D(Form("h2D_typeA_outer_NClusEta_Mbin%d_FineBin", Mbin), Form("h2D_typeA_outer_NClusEta_Mbin%d_FineBin; Clus #eta (outer, type A);INTT vtxZ [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max)
            )
        );
    } // note : end of the Mbin loop for h2D    
    
    h2D_RecoEvtCount_vtxZCentrality = new TH2D(Form("h2D_RecoEvtCount_vtxZCentrality"), Form("h2D_RecoEvtCount_vtxZCentrality;INTT vtxZ [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]); // note : 0 - 5%

    h2D_InttVtxZ_Centrality = new TH2D(Form("h2D_InttVtxZ_Centrality"), Form("h2D_InttVtxZ_Centrality;INTT vtxZ [cm];Centrality"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max);

    // Division: ---For Truth-----------------------------------------------------------------------------------------------------------------------------------------
    if (runnumber == -1){
        h1D_TrueEta_map.clear();
        h2D_TrueEtaVtxZ_map.clear();

        for (int Mbin = 0; Mbin < nCentrality_bin; Mbin++)
        {
            for (int VtxZ_bin = 0; VtxZ_bin < nVtxZBin; VtxZ_bin++)
            {
                h1D_TrueEta_map.insert( std::make_pair(
                        Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin),
                        new TH1D(Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, VtxZ_bin), Form("h1D_TrueEta_Mbin%d_VtxZ%d;PHG4Particle #eta;Entries", Mbin, VtxZ_bin), nEtaBin, EtaEdge_min, EtaEdge_max)
                    )
                );
            }

            h2D_TrueEtaVtxZ_map.insert( std::make_pair(
                    Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin),
                    new TH2D(Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin), Form("h2D_TrueEtaVtxZ_Mbin%d;PHG4Particle #eta;TruthPV_trig_z [cm]", Mbin), nEtaBin, EtaEdge_min, EtaEdge_max, nVtxZBin, VtxZEdge_min, VtxZEdge_max)
                )
            );

            h2D_TrueEtaVtxZ_map.insert( std::make_pair(
                    Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin),
                    new TH2D(Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin), Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin;PHG4Particle #eta;TruthPV_trig_z [cm]", Mbin), 540, EtaEdge_min, EtaEdge_max, 450, VtxZEdge_min, VtxZEdge_max)
                )
            );
        }

        h2D_TrueEvtCount_vtxZCentrality = new TH2D(Form("h2D_TrueEvtCount_vtxZCentrality"), Form("h2D_TrueEvtCount_vtxZCentrality;TruthPV_trig_z [cm];Centrality [%]"), nVtxZBin, VtxZEdge_min, VtxZEdge_max, nCentrality_bin, &centrality_edges[0]); // note : 0 - 5%
    }
        
}

void ClusHistogram::EvtCleanUp()
{
    evt_sPH_inner_nocolumn_vec.clear();
    evt_sPH_outer_nocolumn_vec.clear();
}

void ClusHistogram::PrepareClusterVec()
{
    if (geo_offset_map.size() == 0){
        std::cout<<"the set Geo Offset is set but no input"<<std::endl;
        exit(1);
    }

    for (int clu_i = 0; clu_i < ClusX -> size(); clu_i++)
    {
        ClusHistogram::clu_info this_clu;

        this_clu.adc = ClusAdc -> at(clu_i);
        this_clu.phi_size = ClusPhiSize -> at(clu_i);
        this_clu.sensorZID = ClusLadderZId -> at(clu_i);
        this_clu.ladderPhiID = ClusLadderPhiId -> at(clu_i);
        this_clu.layerID = ClusLayer -> at(clu_i);

        this_clu.index = clu_i;

        this_clu.x = ClusX -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][0];
        this_clu.y = ClusY -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][1];
        this_clu.z = ClusZ -> at(clu_i) + geo_offset_map[Form("%i_%i",this_clu.layerID,this_clu.ladderPhiID)][2];
        this_clu.eta_INTTz = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {this_clu.x, this_clu.y, this_clu.z});

        // this_clu.eta_INTTz = ClusEta_INTTz -> at(clu_i);
        // this_clu.eta_MBDz  = ClusEta_MBDz  -> at(clu_i);
        // this_clu.phi_avgXY = ClusPhi_AvgPV -> at(clu_i);

        // if (runnumber == -1){
        //     this_clu.eta_TrueXYZ = ClusEta_TrueXYZ -> at(clu_i);
        //     this_clu.phi_TrueXY  = ClusPhi_TrueXY  -> at(clu_i);
        // }
        // todo: test
        // if (this_clu.sensorZID == 0 && (this_clu.layerID == 4) && this_clu.ladderPhiID == 0) {continue;}
        // if (this_clu.sensorZID == 0 && (this_clu.layerID == 6) && this_clu.ladderPhiID == 0) {continue;}
        // if (this_clu.sensorZID == 0) {continue;}

        if (ColMulMask){
            
            // note : start from 1 or -1 for outliers 
            int GoodColMap_ZId = h1D_GoodColMap_ZId -> Fill(ClusZ -> at(clu_i)); // note : to be free from the geometry offset
            if (GoodColMap_ZId == -1) {continue;}

            int GoodColMap_XId = (this_clu.layerID - 3) * 20 + this_clu.ladderPhiID + 1; // note : Layer 4, phiID 9 -> 30

            if (
                h2D_GoodColMap != nullptr && 
                h2D_GoodColMap -> GetBinContent(GoodColMap_XId, GoodColMap_ZId) == 0
            )
            {
                continue;
            }
        }

        if (isClusQA.first && this_clu.adc <= isClusQA.second.first) {continue;} // note : adc
        if (isClusQA.first && this_clu.phi_size > isClusQA.second.second) {continue;} // note : phi size

        std::vector<ClusHistogram::clu_info>* p_evt_sPH_nocolumn_vec =
        (this_clu.layerID == 3 || this_clu.layerID == 4) ? (&evt_sPH_inner_nocolumn_vec) : (&evt_sPH_outer_nocolumn_vec);
        p_evt_sPH_nocolumn_vec -> push_back(this_clu);
    }

    if (SetRandomHits.first){
        if (inner_UniqueClusXYZ_map.size() == 0 || outer_UniqueClusXYZ_map.size() == 0){
            std::cout<<"the Unique Clus XYZ is not set for generating the random hits"<<std::endl;
            exit(1);
        }

        for (int clu_i = 0; clu_i < SetRandomHits.second; clu_i++)
        {
            ClusHistogram::clu_info this_clu;

            int inner_or_outer = (rand() % 2 == 0) ? 0 : 1; // note : 0 -> inner, 1 -> outer

            std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>>* p_UniqueClusXYZ_map = (inner_or_outer == 0) ? (&inner_UniqueClusXYZ_map) : (&outer_UniqueClusXYZ_map);
            std::vector<std::string>* p_UniqueClusXYZ_vec                                                           = (inner_or_outer == 0) ? (&inner_UniqueClusXYZ_vec) : (&outer_UniqueClusXYZ_vec);
            int Rand_index = int(rand3 -> Uniform(0, p_UniqueClusXYZ_vec -> size()));

            double selected_x        = std::get<0>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            double selected_y        = std::get<1>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            double selected_z        = std::get<2>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_sensorZID   = std::get<3>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_layerID     = std::get<4>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_adc         = std::get<5>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_phi_size    = std::get<6>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));
            int selected_ladderPhiID = std::get<7>(p_UniqueClusXYZ_map -> at(p_UniqueClusXYZ_vec -> at(Rand_index)));

            this_clu.x = selected_x;
            this_clu.y = selected_y;
            this_clu.z = selected_z;
            this_clu.eta_INTTz = get_clu_eta({vertexXYIncm.first, vertexXYIncm.second, INTTvtxZ}, {selected_x, selected_y, selected_z});
            this_clu.sensorZID = selected_sensorZID;
            this_clu.layerID = selected_layerID;
            this_clu.adc = selected_adc;
            this_clu.phi_size = selected_phi_size;
            this_clu.ladderPhiID = selected_ladderPhiID;

            this_clu.index = ClusX -> size() + clu_i;

            std::vector<ClusHistogram::clu_info>* p_evt_sPH_nocolumn_vec =
            (inner_or_outer == 0) ? (&evt_sPH_inner_nocolumn_vec) : (&evt_sPH_outer_nocolumn_vec);
            p_evt_sPH_nocolumn_vec -> push_back(this_clu);
        }
    }
}

void ClusHistogram::PrepareUniqueClusXYZ()
{
    std::cout<<"In preparaing the unique cluster XYZ map"<<std::endl;

    inner_UniqueClusXYZ_map.clear();
    outer_UniqueClusXYZ_map.clear();
    inner_UniqueClusXYZ_vec.clear();
    outer_UniqueClusXYZ_vec.clear();

    h2D_RandClusXY_ref = new TH2D("h2D_RandClusXY_ref","h2D_RandClusXY_ref;ClusX [cm];ClusY [cm]",200,-15,15,200,-15,15);
    h1D_RandClusZ_ref = new TH1D("h1D_RandClusZ_ref","h1D_RandClusZ_ref;ClusZ [cm];Entries",100,-30,30);

    if (geo_offset_map.size() == 0){
        std::cout<<"the set Geo Offset is set but no input"<<std::endl;
        exit(1);
    }

    for (int i = 0; i < 500; i++) // todo : but it's limited by the MC sample
    {
        tree_in -> GetEntry(i);

        // std::cout<<111<<std::endl;

        for (int clu_i = 0; clu_i < ClusZ -> size(); clu_i++){
            
            // std::cout<<222<<std::endl;
            if (ClusPhiSize -> at(clu_i) > 1) {continue;} // todo: maybe we don't need it
            // std::cout<<333<<std::endl;

            double clu_x = ClusX -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][0];
            double clu_y = ClusY -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][1];
            double clu_z = ClusZ -> at(clu_i) + geo_offset_map[Form("%i_%i",ClusLayer->at(clu_i),ClusLadderPhiId->at(clu_i))][2];
            
            // std::cout<<4444<<std::endl;

            // std::string clu_key = Form("%.2f_%.2f_%.2f", clu_x, clu_y, clu_z);
            std::string clu_key = Form("%.2f_%.2f_%d", clu_x, clu_y, h1D_RandClusZ_ref->FindBin(clu_z));

            // std::cout<<555<<std::endl;

            std::map<std::string, std::tuple<double, double, double, int, int, int, int, int>>* p_UniqueClusXYZ_map = (ClusLayer -> at(clu_i) == 3 || ClusLayer -> at(clu_i) == 4) ? (&inner_UniqueClusXYZ_map) : (&outer_UniqueClusXYZ_map);
            std::vector<std::string>* p_UniqueClusXYZ_vec                                                           = (ClusLayer -> at(clu_i) == 3 || ClusLayer -> at(clu_i) == 4) ? (&inner_UniqueClusXYZ_vec) : (&outer_UniqueClusXYZ_vec);

            // std::cout<<666<<std::endl;

            if (ColMulMask){
                
                // note : start from 1 or -1 for outliers 
                int GoodColMap_ZId = h1D_GoodColMap_ZId -> Fill(ClusZ -> at(clu_i)); // note : to be free from the geometry offset
                if (GoodColMap_ZId == -1) {continue;}

                int GoodColMap_XId = (ClusLayer->at(clu_i) - 3) * 20 + ClusLadderPhiId->at(clu_i) + 1; // note : Layer 4, phiID 9 -> 30

                if (
                    h2D_GoodColMap != nullptr && 
                    h2D_GoodColMap -> GetBinContent(GoodColMap_XId, GoodColMap_ZId) == 0
                )
                {
                    continue;
                }
            }

            if (p_UniqueClusXYZ_map -> find(clu_key) == p_UniqueClusXYZ_map -> end()){
                p_UniqueClusXYZ_map -> insert( 
                    std::make_pair(
                        clu_key,
                        std::make_tuple(
                            clu_x, clu_y, clu_z, 
                            ClusLadderZId->at(clu_i), 
                            ClusLayer -> at(clu_i),
                            ClusAdc -> at(clu_i),
                            ClusPhiSize -> at(clu_i),
                            ClusLadderPhiId -> at(clu_i)
                        )
                    ) 
                );
                
                // std::cout<<7777<<std::endl;

                p_UniqueClusXYZ_vec -> push_back(clu_key);

                // std::cout<<8888<<std::endl;

                h2D_RandClusXY_ref -> Fill(clu_x, clu_y);
                h1D_RandClusZ_ref -> Fill(clu_z);

                // std::cout<<9999<<std::endl;
            }            
        }
    }
}

double ClusHistogram::CheckGeoOffsetMap()
{
    double sum = 0;
    for (auto &pair : geo_offset_map)
    {
        sum += fabs(pair.second[0]) + fabs(pair.second[1]) + fabs(pair.second[2]);
    }
    return sum;
}

void ClusHistogram::MainProcess()
{
    if (SetRandomHits.first){PrepareUniqueClusXYZ();}

    if (ColMulMask && h2D_GoodColMap == nullptr){
        std::cout<<"The GoodColMap is not set correctly"<<std::endl;
        exit(1);
    }

    if (
        ColMulMask &&
        (
            h1D_GoodColMap_ZId -> GetNbinsX() != h2D_GoodColMap -> GetNbinsY() ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin() - h2D_GoodColMap -> GetYaxis() -> GetXmin()) > 0.0000001 ||
            fabs(h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax() - h2D_GoodColMap -> GetYaxis() -> GetXmax()) > 0.0000001
        )

    ){
        std::cout<<"The setting of h1D_GoodColMap_ZId is different from h2D_GoodColMap"<<std::endl;
        std::cout<<"h1D_GoodColMap_ZId : "<<h1D_GoodColMap_ZId -> GetNbinsX()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmin()<<" "<<h1D_GoodColMap_ZId -> GetXaxis() -> GetXmax()<<std::endl;
        std::cout<<"h2D_GoodColMap : "<<h2D_GoodColMap -> GetNbinsY()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmin()<<" "<<h2D_GoodColMap -> GetYaxis() -> GetXmax()<<std::endl;
        exit(1);
    }

    if (HaveGeoOffsetTag && CheckGeoOffsetMap() <= 0){
        std::cout<<"The geo offset map is not set correctly"<<std::endl;
        exit(1);
    }

    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        EvtCleanUp();

        if (RandInttZ){
            INTTvtxZ = rand3 -> Uniform(VtxZEdge_min,VtxZEdge_max);
            INTTvtxZError = 0;
        }

        if (i % 10 == 0) {std::cout << "Processing event " << i<<", NClus : "<< ClusX -> size() << std::endl;}

        // =======================================================================================================================================================
        // note : hard cut

        // note : for data
        if (runnumber != -1 && BcoFullDiffCut && InttBcoFullDiff_next <= cut_InttBcoFullDIff_next) {continue;}
        if (runnumber != -1 && MBDNSg2 != 1) {continue;} // todo: assume MC no trigger

        // note : for MC
        if (runnumber == -1 && NTruthVtx != 1) {continue;}

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}
        if (INTTvtxZ != INTTvtxZ) {continue;}
        if (MBD_z_vtx < cut_GlobalMBDvtxZ.first || MBD_z_vtx > cut_GlobalMBDvtxZ.second) {continue;} // todo: the hard cut 60 cm 
        // =======================================================================================================================================================

        int Mbin = h1D_centrality_bin -> Fill(MBD_centrality);
        Mbin = (Mbin == -1) ? -1 : Mbin - 1;
        if (Mbin == -1) {
            std::cout << "Mbin == -1, MBD_centrality = " << MBD_centrality << std::endl;
            continue;
        }
        // =======================================================================================================================================================


        // note : for truth
        if (runnumber == -1){
            int TruthVtxZ_bin = h1D_vtxz_template->Fill(TruthPV_trig_z);
            TruthVtxZ_bin = (TruthVtxZ_bin == -1) ? -1 : TruthVtxZ_bin - 1;

            if (TruthVtxZ_bin != -1){
                for (int true_i = 0; true_i < NPrimaryG4P; true_i++){
                    if (PrimaryG4P_isChargeHadron->at(true_i) != 1) { continue; }
                    
                    h1D_TrueEta_map[Form("h1D_TrueEta_Mbin%d_VtxZ%d", Mbin, TruthVtxZ_bin)] -> Fill(PrimaryG4P_Eta->at(true_i));
                    h2D_TrueEtaVtxZ_map[Form("h2D_TrueEtaVtxZ_Mbin%d", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
                    h2D_TrueEtaVtxZ_map[Form("h2D_TrueEtaVtxZ_Mbin%d_FineBin", Mbin)] -> Fill(PrimaryG4P_Eta->at(true_i), TruthPV_trig_z);
                }

                h2D_TrueEvtCount_vtxZCentrality->Fill(TruthPV_trig_z, MBD_centrality);
            }
        }


        // =======================================================================================================================================================
        // note : optional cut
        if (INTT_vtxZ_QA && (MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {continue;}
        if (INTT_vtxZ_QA && (TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){continue;}
        if (INTT_vtxZ_QA && (TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){continue;}
        if (INTT_vtxZ_QA && (INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){continue;}
        // =======================================================================================================================================================
        
        if (vtxZReweight.first && runnumber != -1){
            std::cout<<"Should not have the vtxZ weighting from the data"<<std::endl;
            exit(1);
        }

        double INTTvtxZWeighting;
        if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting != nullptr){
            INTTvtxZWeighting = h1D_INTT_vtxZ_reweighting -> GetBinContent(h1D_INTT_vtxZ_reweighting -> FindBin(INTTvtxZ));
        }
        else if (vtxZReweight.first && h1D_INTT_vtxZ_reweighting == nullptr){
            std::cout << "ApplyVtxZReWeighting is true, but h1D_INTT_vtxZ_reweighting is nullptr" << std::endl;
            exit(1);
        }
        else {
            INTTvtxZWeighting = 1.0;
        }

        // =======================================================================================================================================================
        
        int InttVtxZ_bin = h1D_vtxz_template->Fill(INTTvtxZ);
        InttVtxZ_bin = (InttVtxZ_bin == -1) ? -1 : InttVtxZ_bin - 1;

        // int MBDVtxZ_bin = h1D_vtxz_template->Fill(MBD_z_vtx);
        // MBDVtxZ_bin = (MBDVtxZ_bin == -1) ? -1 : MBDVtxZ_bin - 1;

        // note : everything below is within INTT vtxZ -45 cm  ~ 45 cm
        if (InttVtxZ_bin == -1) {continue;}


        // note : for reconstructed 
        // note : INTT vtxZ range -45 cm ~ 45 cm
        h1D_centrality_bin_weighted -> Fill(MBD_centrality,INTTvtxZWeighting);
        h1D_INTTvtxZ -> Fill(INTTvtxZ, INTTvtxZWeighting);
        h1D_centrality_map["h1D_centrality"] -> Fill(MBD_centrality,INTTvtxZWeighting);

        h2D_RecoEvtCount_vtxZCentrality -> Fill(INTTvtxZ, MBD_centrality, INTTvtxZWeighting);
        h2D_InttVtxZ_Centrality -> Fill(INTTvtxZ, MBD_centrality, INTTvtxZWeighting); // note : fine bins in centrality
        h1D_centrality_map[Form("h1D_centrality_InttVtxZ%d", InttVtxZ_bin)] -> Fill(MBD_centrality,INTTvtxZWeighting);

        PrepareClusterVec();

        // note : inner 
        for (int clu_i = 0; clu_i < evt_sPH_inner_nocolumn_vec.size(); clu_i++){
            ClusHistogram::clu_info this_clu = evt_sPH_inner_nocolumn_vec[clu_i];

            h1D_NClusEta_map[Form("h1D_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, InttVtxZ_bin)] -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
            h2D_NClusEtaVtxZ_map[Form("h2D_inner_NClusEta_Mbin%d", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            h2D_NClusEtaVtxZ_map[Form("h2D_inner_NClusEta_Mbin%d_FineBin", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            
            // note : type A
            if (this_clu.sensorZID == typeA_sensorZID[0] || this_clu.sensorZID == typeA_sensorZID[1]){
                h1D_NClusEta_map[Form("h1D_typeA_inner_NClusEta_Mbin%d_VtxZ%d", Mbin, InttVtxZ_bin)] -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
                h2D_NClusEtaVtxZ_map[Form("h2D_typeA_inner_NClusEta_Mbin%d", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
                h2D_NClusEtaVtxZ_map[Form("h2D_typeA_inner_NClusEta_Mbin%d_FineBin", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            }            
        }

        // note : outer 
        for (int clu_i = 0; clu_i < evt_sPH_outer_nocolumn_vec.size(); clu_i++){
            ClusHistogram::clu_info this_clu = evt_sPH_outer_nocolumn_vec[clu_i];

            h1D_NClusEta_map[Form("h1D_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, InttVtxZ_bin)] -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
            h2D_NClusEtaVtxZ_map[Form("h2D_outer_NClusEta_Mbin%d", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            h2D_NClusEtaVtxZ_map[Form("h2D_outer_NClusEta_Mbin%d_FineBin", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            
            // note : type A
            if (this_clu.sensorZID == typeA_sensorZID[0] || this_clu.sensorZID == typeA_sensorZID[1]){
                h1D_NClusEta_map[Form("h1D_typeA_outer_NClusEta_Mbin%d_VtxZ%d", Mbin, InttVtxZ_bin)] -> Fill(this_clu.eta_INTTz, INTTvtxZWeighting);
                h2D_NClusEtaVtxZ_map[Form("h2D_typeA_outer_NClusEta_Mbin%d", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
                h2D_NClusEtaVtxZ_map[Form("h2D_typeA_outer_NClusEta_Mbin%d_FineBin", Mbin)] -> Fill(this_clu.eta_INTTz, INTTvtxZ, INTTvtxZWeighting);
            }
            
        }


    } // note : end pf event looping
}

double ClusHistogram::get_clu_eta(std::vector<double> vertex, std::vector<double> clu_pos)
{
    double correct_x = clu_pos[0] - vertex[0];
    double correct_y = clu_pos[1] - vertex[1];
    double correct_z = clu_pos[2] - vertex[2];
    double clu_r = sqrt(pow(correct_x,2) + pow(correct_y,2));
    // cout<<"correct info : "<<correct_x<<" "<<correct_y<<" "<<correct_z<<" "<<clu_r<<endl;

    return -0.5 * TMath::Log((sqrt(pow(correct_z,2)+pow(clu_r,2))-(correct_z)) / (sqrt(pow(correct_z,2)+pow(clu_r,2))+(correct_z)));
}

void ClusHistogram::EndRun()
{
    file_out -> cd();
    tree_out_par -> Fill();
    tree_out_par -> Write();

    if (h1D_INTT_vtxZ_reweighting != nullptr){
        h1D_INTT_vtxZ_reweighting -> Write("h1D_Used_INTTvtxZ_reweighting");
    }

    h1D_vtxz_template -> Write();
    h1D_INTTvtxZ -> Write();
    h1D_centrality_bin -> Write();
    h1D_centrality_bin_weighted -> Write();
    h2D_RecoEvtCount_vtxZCentrality -> Write();
    h2D_InttVtxZ_Centrality -> Write();
    if (runnumber == -1) {h2D_TrueEvtCount_vtxZCentrality->Write();}


    for (auto &pair : h2D_NClusEtaVtxZ_map){
        pair.second -> Write();
    }


    if (runnumber == -1){
        for (auto &pair : h2D_TrueEtaVtxZ_map){
            pair.second -> Write();
        }
    }


    for (auto &pair : h1D_NClusEta_map){
        pair.second -> Write();
    }


    for (auto &pair : h1D_centrality_map){
        pair.second -> Write();
    }

    
    if (runnumber == -1){
        for (auto &pair : h1D_TrueEta_map){
            pair.second -> Write();
        }
    }

    if (h1D_RandClusZ_ref != nullptr && h2D_RandClusXY_ref != nullptr){
        h2D_RandClusXY_ref -> Write();
        h1D_RandClusZ_ref -> Write();
    }

    
    file_out -> Close();
}