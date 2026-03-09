#include "../../../EtaDistReader.h"
// #include "../../../../sPhenixStyle.C"

void HIJING_398_reco_tracklet()
{
    string run_type = "MC";

    // note : the new generated folder will be under the input_directory1
    string input_directory1 = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/TrackletReco_condor/merged_file";
    string input_file1 = "merged_hist_half_corr.root";
    string out_folder_directory1 = input_directory1 + "/eta_closer_out_test_MultiZBin_half_corr";
    bool Mbin_Z_evt_map_name_bool1 = 0;

    string input_directory2 = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/TrackletReco_condor/merged_file";
    string input_file2 = "merged_hist_half_test.root";
    string out_folder_directory2 = input_directory2 + "/eta_closer_out_test_MultiZBin_half_test";
    bool Mbin_Z_evt_map_name_bool2 = 0;

    // note : key is the eta bin ID, value is the vector 
    // note : the full map
    vector<pair<int,vector<int>>> included_eta_z_map = {
        {11,                     {9, 10, 11}},         // note : eta bin 11: -1.0 to -0.8
        {12,                  {8, 9, 10, 11}},         // note : eta bin 12: -0.8 to -0.6
        {13,                  {8, 9, 10, 11}},         // note : eta bin 13: -0.6 to -0.4
        {14,               {7, 8, 9, 10, 11}},         // note : eta bin 14: -0.4 to -0.2
        {15,               {7, 8, 9, 10, 11}},        // note : eta bin 15: -0.2 to 0.0
        {16,            {6, 7, 8, 9, 10, 11}},       // note : eta bin 16: 0.0  to 0.2
        {17,            {6, 7, 8, 9, 10, 11}},      // note : eta bin 17: 0.2  to 0.4
        {18,            {6, 7, 8, 9, 10, 11}},      // note : eta bin 18: 0.4  to 0.6
        {19,         {5, 6, 7, 8, 9, 10, 11}},    // note : eta bin 19: 0.6  to 0.8
        {20,         {5, 6, 7, 8, 9, 10, 11}},    // note : eta bin 20: 0.8  to 1.0
        {21,      {4, 5, 6, 7, 8, 9, 10, 11}},  // note : eta bin 21: 1.0  to 1.2
        {22,      {4, 5, 6, 7, 8, 9, 10}},  // note : eta bin 22: 1.2  to 1.4
        {23,   {3, 4, 5, 6, 7, 8, 9}},  // note : eta bin 23: 1.4  to 1.6
        {24,{2, 3, 4, 5, 6, 7}},  // note : eta bin 24: 1.6  to 1.8
        {25,{2, 3, 4, 5}},      // note : eta bin 25: 1.8  to 2.0
        // {26,{}},        // note : eta bin 26: 2.0  to 2.2
    };

    // note : focus on the zvtx peak region
    // vector<pair<int,vector<int>>> included_eta_z_map = {
    //     // {15,{6}},  // note : eta bin 15: -0.2 to 0.0
    //     {16,{6}},  // note : eta bin 16: 0.0  to 0.2
    //     {17,{6}},  // note : eta bin 17: 0.2  to 0.4
    //     {18,{6}},  // note : eta bin 18: 0.4  to 0.6
    //     {19,{6}},  // note : eta bin 19: 0.6  to 0.8
    //     {20,{6}},  // note : eta bin 20: 0.8  to 1.0
    //     {21,{6}},  // note : eta bin 21: 1.0  to 1.2
    //     {22,{6}},  // note : eta bin 22: 1.2  to 1.4
    //     {23,{6}},  // note : eta bin 23: 1.4  to 1.6
    //     {24,{6}},  // note : eta bin 24: 1.6  to 1.8
    //     {25,{6}},  // note : eta bin 25: 1.8  to 2.0
    //     // {26,{}},  // note : eta bin 26: 2.0  to 2.2
    // };

    EtaDistReader * file_half_corr = new EtaDistReader(run_type, out_folder_directory1, included_eta_z_map, Form("%s/%s",input_directory1.c_str(), input_file1.c_str()), Mbin_Z_evt_map_name_bool1);
    file_half_corr -> FindCoveredRegion();    
    file_half_corr -> FinaldNdEta();
    file_half_corr -> EndRun();
    EtaDistReader * file_half_test = new EtaDistReader(run_type, out_folder_directory2, included_eta_z_map, Form("%s/%s",input_directory2.c_str(), input_file2.c_str()), Mbin_Z_evt_map_name_bool2);
    file_half_test -> Set_alpha_corr_map(file_half_corr->Get_alpha_corr_map()[0], file_half_corr->Get_alpha_corr_map()[1]);
    file_half_test -> FinaldNdEta();
    file_half_test -> EndRun();
    vector<string> centrality_region = file_half_corr -> Get_centrality_region();

    // vector<TH1F *> DeltaPhi_Multi_stack_1D_1 = file_half_corr -> GetDeltaPhi_Multi_stack_1D();
    vector<TH1F *> dNdeta_1D_MC_1 = file_half_corr -> GetdNdeta_1D_MC();
    vector<TH1F *> dNdeta_1D_reco_single_1 = file_half_corr -> GetdNdeta_1D_reco_single();
    vector<TH1F *> dNdeta_1D_reco_multi_1 = file_half_corr -> GetdNdeta_1D_reco_multi();

    // vector<TH1F *> DeltaPhi_Multi_stack_1D_2 = file_half_test -> GetDeltaPhi_Multi_stack_1D();
    // vector<TH1F *> dNdeta_1D_MC_2 = file_half_test -> GetdNdeta_1D_MC();
    // vector<TH1F *> dNdeta_1D_reco_single_2 = file_half_test -> GetdNdeta_1D_reco_single();
    // vector<TH1F *> dNdeta_1D_reco_multi_2 = file_half_test -> GetdNdeta_1D_reco_multi();

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    SetsPhenixStyle();

    TCanvas * c2 = new TCanvas("c2","",950,800);
    c2 -> cd();
    c2 -> Print(Form("%s/dNdeta_comp_single.pdf(",out_folder_directory1.c_str()));
    for (int i = 0; i < dNdeta_1D_MC_1.size(); i++)
    {
        dNdeta_1D_MC_1[i] -> Draw("hist");
        // dNdeta_1D_MC_2[i] -> Draw("hist same");

        // note : true Z
        dNdeta_1D_reco_single_1[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_1[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_1[i] -> Draw("p same");

        // note : reco Z
        // dNdeta_1D_reco_single_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        // dNdeta_1D_reco_single_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        // dNdeta_1D_reco_single_2[i] -> Draw("p same");
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));

        c2 -> Print(Form("%s/dNdeta_comp_single.pdf",out_folder_directory1.c_str()));
        c2 -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_single.pdf)",out_folder_directory1.c_str()));


    c2 -> Print(Form("%s/dNdeta_comp_multi.pdf(",out_folder_directory1.c_str()));
    for (int i = 0; i < dNdeta_1D_MC_1.size(); i++)
    {
        dNdeta_1D_MC_1[i] -> Draw("hist");
        // dNdeta_1D_MC_2[i] -> Draw("hist same");

        // note : true Z
        dNdeta_1D_reco_multi_1[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_1[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_1[i] -> Draw("p same");
        // note : reco Z
        // dNdeta_1D_reco_multi_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        // dNdeta_1D_reco_multi_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        // dNdeta_1D_reco_multi_2[i] -> Draw("p same");
        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));

        c2 -> Print(Form("%s/dNdeta_comp_multi.pdf",out_folder_directory1.c_str()));
        c2 -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_multi.pdf)",out_folder_directory1.c_str()));



}