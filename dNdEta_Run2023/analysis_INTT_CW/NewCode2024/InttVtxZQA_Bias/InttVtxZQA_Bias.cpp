#include "InttVtxZQA_Bias.h"

InttVtxZQA_Bias::InttVtxZQA_Bias(
    int process_id_in,
    int runnumber_in,
    int run_nEvents_in,
    std::string input_directory_in,
    std::string input_file_name_in,
    std::string output_directory_in,

    std::string output_file_name_suffix_in,

    std::pair<bool, int> ApplyEvtBcoFullDiffCut_in
):
    process_id(process_id_in),
    runnumber(runnumber_in),
    run_nEvents(run_nEvents_in),
    input_directory(input_directory_in),
    input_file_name(input_file_name_in),
    output_directory(output_directory_in),
    output_file_name_suffix(output_file_name_suffix_in),
    ApplyEvtBcoFullDiffCut(ApplyEvtBcoFullDiffCut_in)
{
    PrepareOutPutFileName();
    PrepareInputFile();

    run_nEvents = (run_nEvents == -1) ? tree_in->GetEntries() : run_nEvents;
    run_nEvents = (run_nEvents > tree_in->GetEntries()) ? tree_in->GetEntries() : run_nEvents;
    nCentrality_bin = centrality_edges.size() - 1;

    PrepareOutputFile();
    PrepareHist();

}

void InttVtxZQA_Bias::PrepareOutPutFileName()
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

    output_filename = "InttVtxZBias";
    output_filename = (runnumber != -1) ? "Data_" + output_filename : "MC_" + output_filename;

    output_filename += (runnumber != -1 && ApplyEvtBcoFullDiffCut.first) ? Form("_EvtBcoFullDiffCut%d", ApplyEvtBcoFullDiffCut.second) : "";

    output_filename += output_file_name_suffix;
    output_filename += (runnumber != -1) ? Form("_%s_%s.root",runnumber_str.c_str(),job_index.c_str()) : Form("_%s.root",job_index.c_str());
}

std::map<std::string, int> InttVtxZQA_Bias::GetInputTreeBranches(TTree * m_tree_in)
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

void InttVtxZQA_Bias::PrepareInputFile()
{
    file_in = TFile::Open(Form("%s/%s", input_directory.c_str(), input_file_name.c_str()));
    tree_in = (TTree*)file_in->Get(tree_name.c_str());

    std::map<std::string, int> branch_map = GetInputTreeBranches(tree_in);

    tree_in -> SetBranchStatus("*", 0);

    tree_in -> SetBranchStatus("is_min_bias", 1);
    tree_in -> SetBranchStatus("MBD_centrality", 1);
    tree_in -> SetBranchStatus("MBD_z_vtx", 1);
    tree_in -> SetBranchStatus("MBD_charge_sum", 1);

    tree_in -> SetBranchStatus("INTTvtxZ", 1);
    tree_in -> SetBranchStatus("INTTvtxZError", 1);
    tree_in -> SetBranchStatus("NgroupTrapezoidal", 1);
    tree_in -> SetBranchStatus("NgroupCoarse", 1);
    tree_in -> SetBranchStatus("TrapezoidalFitWidth", 1);
    tree_in -> SetBranchStatus("TrapezoidalFWHM", 1);

    tree_in -> SetBranchStatus("NClus", 1);
    tree_in -> SetBranchStatus("NClus_Layer1", 1);
    
    // note : for data
    if (branch_map.find("MBDNSg2") != branch_map.end()) {
        tree_in -> SetBranchStatus("MBDNSg2", 1);
        m_withTrig = true;
    }
    if (branch_map.find("MBDNSg2_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchStatus("MBDNSg2_vtxZ10cm", 1);}
    if (branch_map.find("MBDNSg2_vtxZ30cm") != branch_map.end()) {tree_in -> SetBranchStatus("MBDNSg2_vtxZ30cm", 1);}
    
    if (branch_map.find("InttBcoFullDiff_next") != branch_map.end()) {tree_in -> SetBranchStatus("InttBcoFullDiff_next", 1); }
    
    // note :for MC
    if (branch_map.find("NTruthVtx") != branch_map.end()) {tree_in -> SetBranchStatus("NTruthVtx", 1);}
    if (branch_map.find("TruthPV_trig_z") != branch_map.end()) {tree_in -> SetBranchStatus("TruthPV_trig_z", 1);}

    // Division : ---SetBranchAddress-----------------------------------------------------------------------------------------------
    tree_in -> SetBranchAddress("is_min_bias", &is_min_bias);
    tree_in -> SetBranchAddress("MBD_centrality", &MBD_centrality);
    tree_in -> SetBranchAddress("MBD_z_vtx", &MBD_z_vtx);
    tree_in -> SetBranchAddress("MBD_charge_sum", &MBD_charge_sum);

    tree_in -> SetBranchAddress("INTTvtxZ", &INTTvtxZ);
    tree_in -> SetBranchAddress("INTTvtxZError", &INTTvtxZError);
    tree_in -> SetBranchAddress("NgroupTrapezoidal", &NgroupTrapezoidal);
    tree_in -> SetBranchAddress("NgroupCoarse", &NgroupCoarse);
    tree_in -> SetBranchAddress("TrapezoidalFitWidth", &TrapezoidalFitWidth);
    tree_in -> SetBranchAddress("TrapezoidalFWHM", &TrapezoidalFWHM);

    tree_in -> SetBranchAddress("NClus", &NClus);
    tree_in -> SetBranchAddress("NClus_Layer1", &NClus_Layer1);

    // note : for data
    if (branch_map.find("MBDNSg2") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2", &MBDNSg2);}
    if (branch_map.find("MBDNSg2_vtxZ10cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ10cm", &MBDNSg2_vtxZ10cm);}
    if (branch_map.find("MBDNSg2_vtxZ30cm") != branch_map.end()) {tree_in -> SetBranchAddress("MBDNSg2_vtxZ30cm", &MBDNSg2_vtxZ30cm);}

    if (branch_map.find("InttBcoFullDiff_next") != branch_map.end()) {tree_in -> SetBranchAddress("InttBcoFullDiff_next", &InttBcoFullDiff_next); }

    // note : for MC
    if (branch_map.find("NTruthVtx") != branch_map.end()) {tree_in -> SetBranchAddress("NTruthVtx", &NTruthVtx);}
    if (branch_map.find("TruthPV_trig_z") != branch_map.end()) {tree_in -> SetBranchAddress("TruthPV_trig_z", &TruthPV_trig_z);}
}

void InttVtxZQA_Bias::PrepareOutputFile()
{
    file_out = new TFile(Form("%s/%s", output_directory.c_str(), output_filename.c_str()), "RECREATE");
}

void InttVtxZQA_Bias::PrepareHist()
{

    h1D_centrality_bin = new TH1D("h1D_centrality_bin","h1D_centrality_bin;Centrality [%];Entries",nCentrality_bin,&centrality_edges[0]); // note : the 0-5%

    h1D_NoQA_map.clear();
    h2D_NoQA_map.clear();

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_centrality",
            new TH1D("NoQA_h1D_centrality","NoQA_h1D_centrality;Centrality [%];Entries", nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    );

    for (int i = 0; i < nCentrality_bin; i++)
    {
        h1D_NoQA_map.insert(
            std::make_pair(
                Form("NoQA_h1D_NClus_Mbin%d",i),
                new TH1D(Form("NoQA_h1D_NClus_Mbin%d", i),Form("NoQA_h1D_NClus_Mbin%d;NClus;Entries", i), 50, Mbin_NClus_edges[i].first, Mbin_NClus_edges[i].second)
            )
        );
    }

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_NClus",
            new TH1D("NoQA_h1D_NClus","NoQA_h1D_NClus;INTT NClus;Entries", 200,0,10000)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_TrapezoidalFitWidth",
            new TH1D("NoQA_h1D_TrapezoidalFitWidth","NoQA_h1D_TrapezoidalFitWidth;INTT vtxZ Dist Fit Width [cm];Entries", 100, 0, 15)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_TrapezoidalFWHM",
            new TH1D("NoQA_h1D_TrapezoidalFWHM","NoQA_h1D_TrapezoidalFWHM;INTT vtxZ Dist FWHM [cm];Entries", 100, 0, 15)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_INTTvtxZError",
            new TH1D("NoQA_h1D_INTTvtxZError","NoQA_h1D_INTTvtxZError;INTT vtxZ StdDev [cm];Entries", 100, 0, 15)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_NgroupTrapezoidal",
            new TH1D("NoQA_h1D_NgroupTrapezoidal","NoQA_h1D_NgroupTrapezoidal;N group (Trapezoidal);Entries", 15, 0, 15)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_NgroupCoarse",
            new TH1D("NoQA_h1D_NgroupCoarse","NoQA_h1D_NgroupCoarse;N group (Coarse);Entries", 15, 0, 15)
        )
    );

    h1D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h1D_VtxZDiffMbdIntt",
            new TH1D("NoQA_h1D_VtxZDiffMbdIntt","NoQA_h1D_VtxZDiffMbdIntt;MBD vtxZ - INTT vtxZ [cm];Entries", 100, -15, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQANoINTTz_h2D_MBDvtxZ_centrality",
            new TH2D("NoQANoINTTz_h2D_MBDvtxZ_centrality","NoQANoINTTz_h2D_MBDvtxZ_centrality;MBD vtxZ;Centrality [%]", 8, -40,40, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_MBDvtxZ_centrality",
            new TH2D("NoQA_h2D_MBDvtxZ_centrality","NoQA_h2D_MBDvtxZ_centrality;MBD vtxZ;Centrality [%]", 8, -40,40, nCentralityFineBin, CentralityFineEdge_min, CentralityFineEdge_max)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_NClus_INTTvtxZ",
            new TH2D("NoQA_h2D_NClus_INTTvtxZ","NoQA_h2D_NClus_INTTvtxZ;NClus (INTT);INTT vtxZ [cm]",200, 0, 10000, 200, vtxZ_range.first, vtxZ_range.second)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_NClus_TrapezoidalFitWidth",
            new TH2D("NoQA_h2D_NClus_TrapezoidalFitWidth","NoQA_h2D_NClus_TrapezoidalFitWidth;NClus;INTT vtxZ Dist Fit Width [cm]", 100, 0, 10000, 100, 0, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_NClus_TrapezoidalFWHM",
            new TH2D("NoQA_h2D_NClus_TrapezoidalFWHM","NoQA_h2D_NClus_TrapezoidalFWHM;NClus;INTT vtxZ Dist FWHM [cm]", 100, 0, 10000, 100, 0, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_NClus_INTTvtxZError",
            new TH2D("NoQA_h2D_NClus_INTTvtxZError","NoQA_h2D_NClus_INTTvtxZError;NClus;INTT vtxZ StdDev [cm]", 100, 0, 10000, 100, 0, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_NClus_VtxZDiffMbdIntt",
            new TH2D("NoQA_h2D_NClus_VtxZDiffMbdIntt","NoQA_h2D_NClus_VtxZDiffMbdIntt;NClus;MBD vtxZ - INTT vtxZ [cm]", 100, 0, 10000, 100, -15,15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_TrapezoidalFitWidth_TrapezoidalFWHM",
            new TH2D("NoQA_h2D_TrapezoidalFitWidth_TrapezoidalFWHM","NoQA_h2D_TrapezoidalFitWidth_TrapezoidalFWHM;INTT vtxZ Dist Fit Width [cm];INTT vtxZ Dist FWHM [cm]", 100, 0, 15, 100, 0, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_TrapezoidalFitWidth_VtxZDiffMbdIntt",
            new TH2D("NoQA_h2D_TrapezoidalFitWidth_VtxZDiffMbdIntt","NoQA_h2D_TrapezoidalFitWidth_VtxZDiffMbdIntt;INTT vtxZ Dist Fit Width [cm];MBD vtxZ - INTT vtxZ [cm]", 100, 0, 15, 100, -15, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_TrapezoidalFWHM_VtxZDiffMbdIntt",
            new TH2D("NoQA_h2D_TrapezoidalFWHM_VtxZDiffMbdIntt","NoQA_h2D_TrapezoidalFWHM_VtxZDiffMbdIntt;INTT vtxZ Dist FWHM [cm];MBD vtxZ - INTT vtxZ [cm]", 100, 0, 15, 100, -15, 15)
        )
    );

    h2D_NoQA_map.insert(
        std::make_pair(
            "NoQA_h2D_TrapezoidalFitWidth_INTTvtxZError",
            new TH2D("NoQA_h2D_TrapezoidalFitWidth_INTTvtxZError","NoQA_h2D_TrapezoidalFitWidth_INTTvtxZError;INTT vtxZ Dist Fit Width [cm];INTT vtxZ StdDev [cm]", 100, 0, 15, 100, 0, 15)
        )
    );

    for (auto &hist : h1D_NoQA_map)
    {
        std::string YTitle = hist.second -> GetYaxis() -> GetTitle();
        std::string XTitle = hist.second -> GetXaxis() -> GetTitle();

        std::string YTitle_post;

        if (YTitle.find("Entries") != std::string::npos) // note : found the (Entries)
        {
            YTitle_post = Form("Entries  (/%.2f)", hist.second -> GetBinWidth(1));
            hist.second -> GetYaxis() -> SetTitle(YTitle_post.c_str());
        }
    }


    h1D_PostQA_map.clear();
    h2D_PostQA_map.clear();

    for (auto &hist : h1D_NoQA_map)
    {
        h1D_PostQA_map[ReplaceBy(hist.first, "NoQA", "PostQA")] = (TH1D*)hist.second -> Clone(ReplaceBy(hist.first, "NoQA", "PostQA").c_str());
        h1D_PostQA_map[ReplaceBy(hist.first, "NoQA", "PostQA")] -> SetTitle(ReplaceBy(hist.second -> GetTitle(), "NoQA", "PostQA").c_str());
    }

    for (auto &hist : h2D_NoQA_map)
    {
        h2D_PostQA_map[ReplaceBy(hist.first, "NoQA", "PostQA")] = (TH2D*)hist.second -> Clone(ReplaceBy(hist.first, "NoQA", "PostQA").c_str());
        h2D_PostQA_map[ReplaceBy(hist.first, "NoQA", "PostQA")] -> SetTitle(ReplaceBy(hist.second -> GetTitle(), "NoQA", "PostQA").c_str());
    }

}

void InttVtxZQA_Bias::PrepareEvent()
{
    for (int i = 0; i < run_nEvents; i++)
    {
        tree_in -> GetEntry(i);

        if (i % 500 == 0) {std::cout << "Processing Event " << i << " / " << run_nEvents << std::endl;}

        // =======================================================================================================================================================
        // note : hard cut

        // note : for data
        if (runnumber != -1 && ApplyEvtBcoFullDiffCut.first && InttBcoFullDiff_next <= ApplyEvtBcoFullDiffCut.second) {continue;}
        if (runnumber != -1 && MBDNSg2 != 1) {continue;} // todo: assume MC no trigger

        // note : for MC
        // if (runnumber == -1 && NTruthVtx != 1) {continue;}

        // note : both data and MC
        if (is_min_bias != 1) {continue;}
        if (MBD_z_vtx != MBD_z_vtx) {continue;}
        if (MBD_centrality != MBD_centrality) {continue;}
        if (MBD_centrality < 0 || MBD_centrality > 100) {continue;}

        h2D_NoQA_map["NoQANoINTTz_h2D_MBDvtxZ_centrality"] -> Fill(MBD_z_vtx, MBD_centrality);

        if (INTTvtxZ != INTTvtxZ) {continue;}

        h2D_NoQA_map["NoQA_h2D_MBDvtxZ_centrality"] -> Fill(MBD_z_vtx, MBD_centrality);

        if (MBD_z_vtx < -10 || MBD_z_vtx > 10) {continue;} // todo: the hard cut 10 cm 
        if (MBD_centrality > 70) {continue;} // todo : the hard cut > 70% centrality interval
        // =======================================================================================================================================================

        int Mbin = h1D_centrality_bin -> Fill(MBD_centrality);
        Mbin = (Mbin == -1) ? -1 : Mbin - 1;
        if (Mbin == -1) {
            std::cout << "Mbin == -1, MBD_centrality = " << MBD_centrality << std::endl;
            continue;
        }

        h1D_NoQA_map[Form("NoQA_h1D_NClus_Mbin%d",Mbin)] -> Fill(NClus);
        h1D_NoQA_map["NoQA_h1D_centrality"] -> Fill(MBD_centrality);
        h1D_NoQA_map["NoQA_h1D_NClus"] -> Fill(NClus);
        h1D_NoQA_map["NoQA_h1D_TrapezoidalFitWidth"] -> Fill(TrapezoidalFitWidth);
        h1D_NoQA_map["NoQA_h1D_TrapezoidalFWHM"] -> Fill(TrapezoidalFWHM);
        h1D_NoQA_map["NoQA_h1D_INTTvtxZError"] -> Fill(INTTvtxZError);
        h1D_NoQA_map["NoQA_h1D_NgroupTrapezoidal"] -> Fill(NgroupTrapezoidal);
        h1D_NoQA_map["NoQA_h1D_NgroupCoarse"] -> Fill(NgroupCoarse);
        h1D_NoQA_map["NoQA_h1D_VtxZDiffMbdIntt"] -> Fill(MBD_z_vtx - INTTvtxZ);
        h2D_NoQA_map["NoQA_h2D_NClus_INTTvtxZ"] -> Fill(NClus, INTTvtxZ);
        h2D_NoQA_map["NoQA_h2D_NClus_TrapezoidalFitWidth"] -> Fill(NClus, TrapezoidalFitWidth);
        h2D_NoQA_map["NoQA_h2D_NClus_TrapezoidalFWHM"] -> Fill(NClus, TrapezoidalFWHM);
        h2D_NoQA_map["NoQA_h2D_NClus_INTTvtxZError"] -> Fill(NClus, INTTvtxZError);
        h2D_NoQA_map["NoQA_h2D_NClus_VtxZDiffMbdIntt"] -> Fill(NClus, MBD_z_vtx - INTTvtxZ);

        h2D_NoQA_map["NoQA_h2D_TrapezoidalFitWidth_TrapezoidalFWHM"] -> Fill(TrapezoidalFitWidth, TrapezoidalFWHM);
        h2D_NoQA_map["NoQA_h2D_TrapezoidalFitWidth_VtxZDiffMbdIntt"] -> Fill(TrapezoidalFitWidth, MBD_z_vtx - INTTvtxZ);
        h2D_NoQA_map["NoQA_h2D_TrapezoidalFWHM_VtxZDiffMbdIntt"] -> Fill(TrapezoidalFWHM, MBD_z_vtx - INTTvtxZ);
        h2D_NoQA_map["NoQA_h2D_TrapezoidalFitWidth_INTTvtxZError"] -> Fill(TrapezoidalFitWidth, INTTvtxZError);

        // =======================================================================================================================================================
        // note : optional cut
        if ((MBD_z_vtx - INTTvtxZ < cut_vtxZDiff.first || MBD_z_vtx - INTTvtxZ > cut_vtxZDiff.second) ) {continue;}
        if ((TrapezoidalFitWidth < cut_TrapezoidalFitWidth.first || TrapezoidalFitWidth > cut_TrapezoidalFitWidth.second)){continue;}
        if ((TrapezoidalFWHM < cut_TrapezoidalFWHM.first || TrapezoidalFWHM > cut_TrapezoidalFWHM.second)){continue;}
        if ((INTTvtxZError < cut_INTTvtxZError.first || INTTvtxZError > cut_INTTvtxZError.second)){continue;}

        // =======================================================================================================================================================

        h1D_PostQA_map[Form("PostQA_h1D_NClus_Mbin%d",Mbin)] -> Fill(NClus);
        h1D_PostQA_map["PostQA_h1D_centrality"] -> Fill(MBD_centrality);
        h1D_PostQA_map["PostQA_h1D_NClus"] -> Fill(NClus);
        h1D_PostQA_map["PostQA_h1D_TrapezoidalFitWidth"] -> Fill(TrapezoidalFitWidth);
        h1D_PostQA_map["PostQA_h1D_TrapezoidalFWHM"] -> Fill(TrapezoidalFWHM);
        h1D_PostQA_map["PostQA_h1D_INTTvtxZError"] -> Fill(INTTvtxZError);
        h1D_PostQA_map["PostQA_h1D_NgroupTrapezoidal"] -> Fill(NgroupTrapezoidal);
        h1D_PostQA_map["PostQA_h1D_NgroupCoarse"] -> Fill(NgroupCoarse);
        h1D_PostQA_map["PostQA_h1D_VtxZDiffMbdIntt"] -> Fill(MBD_z_vtx - INTTvtxZ);
        h2D_PostQA_map["PostQA_h2D_NClus_INTTvtxZ"] -> Fill(NClus, INTTvtxZ);
        h2D_PostQA_map["PostQA_h2D_NClus_TrapezoidalFitWidth"] -> Fill(NClus, TrapezoidalFitWidth);
        h2D_PostQA_map["PostQA_h2D_NClus_TrapezoidalFWHM"] -> Fill(NClus, TrapezoidalFWHM);
        h2D_PostQA_map["PostQA_h2D_NClus_INTTvtxZError"] -> Fill(NClus, INTTvtxZError);
        h2D_PostQA_map["PostQA_h2D_NClus_VtxZDiffMbdIntt"] -> Fill(NClus, MBD_z_vtx - INTTvtxZ);

        h2D_PostQA_map["PostQA_h2D_TrapezoidalFitWidth_TrapezoidalFWHM"] -> Fill(TrapezoidalFitWidth, TrapezoidalFWHM);
        h2D_PostQA_map["PostQA_h2D_TrapezoidalFitWidth_VtxZDiffMbdIntt"] -> Fill(TrapezoidalFitWidth, MBD_z_vtx - INTTvtxZ);
        h2D_PostQA_map["PostQA_h2D_TrapezoidalFWHM_VtxZDiffMbdIntt"] -> Fill(TrapezoidalFWHM, MBD_z_vtx - INTTvtxZ);
        h2D_PostQA_map["PostQA_h2D_TrapezoidalFitWidth_INTTvtxZError"] -> Fill(TrapezoidalFitWidth, INTTvtxZError);
 
    } // note : end of the event loop
}

void InttVtxZQA_Bias::EndRun()
{
    file_out -> cd();
    for (auto &hist : h1D_NoQA_map)
    {
        hist.second -> Write();
    }

    for (auto &hist : h2D_NoQA_map)
    {
        hist.second -> Write();
    }

    for (auto &hist : h1D_PostQA_map)
    {
        hist.second -> Write();
    }

    for (auto &hist : h2D_PostQA_map)
    {
        hist.second -> Write();
    }


    file_out -> Close();
}

std::string InttVtxZQA_Bias::ReplaceBy(const std::string& input_str, const std::string& target_str, const std::string& new_str) {
    if (target_str.empty()) {
        return input_str; // Avoid infinite loop when replacing empty string
    }



    std::string output_str = input_str;
    size_t pos = 0;

    while ((pos = output_str.find(target_str, pos)) != std::string::npos) {
        output_str.replace(pos, target_str.length(), new_str);
        pos += new_str.length(); // Move past the replaced part
    }

    std::cout << "ReplaceBy: " << input_str << " -> " << output_str << std::endl;

    return output_str;
}