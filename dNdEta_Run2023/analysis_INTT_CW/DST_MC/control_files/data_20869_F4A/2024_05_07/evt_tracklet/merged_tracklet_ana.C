#include "../../../../EtaDistReader.h"
// #include "../../../../sPhenixStyle.C"

int merged_tracklet_ana()
{
    string run_type_MC = "MC";
    string run_type_data = "data";

    // note : the new generated folder will be under the input_directory1
    string input_directory1 = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/evt_tracklet/complete_file/merged_file_folder";
    string input_file1 = "merged_hist_half_corr.root";
    string out_folder_directory1 = input_directory1 + "/eta_closer_out_test_MultiZBin_half_corr";
    bool Mbin_Z_evt_map_name_bool1 = 0;

    string input_directory2 = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR/evt_tracklet/complete_file/merged_file_folder";
    string input_file2 = "merged_hist_half_test.root";
    string out_folder_directory2 = input_directory2 + "/eta_closer_out_test_MultiZBin_half_test";
    bool Mbin_Z_evt_map_name_bool2 = 0;

    string input_directory3 = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey/evt_tracklet/complete_file/merged_file_folder";
    string input_file3 = "merged_file.root";
    string out_folder_directory3 = input_directory3 + "/eta_closer_out_test_MultiZBin";
    bool Mbin_Z_evt_map_name_bool3 = 0;

    // note : key is the eta bin ID, value is the vector 
    // note : the full map
    // vector<pair<int,vector<int>>> included_eta_z_map = {
    //     {11,                     {9, 10, 11}},         // note : eta bin 11: -1.0 to -0.8
    //     {12,                  {8, 9, 10, 11}},         // note : eta bin 12: -0.8 to -0.6
    //     {13,                  {8, 9, 10, 11}},         // note : eta bin 13: -0.6 to -0.4
    //     {14,               {7, 8, 9, 10, 11}},         // note : eta bin 14: -0.4 to -0.2
    //     {15,               {7, 8, 9, 10, 11}},        // note : eta bin 15: -0.2 to 0.0
    //     {16,            {6, 7, 8, 9, 10, 11}},       // note : eta bin 16: 0.0  to 0.2
    //     {17,            {6, 7, 8, 9, 10, 11}},      // note : eta bin 17: 0.2  to 0.4
    //     {18,            {6, 7, 8, 9, 10, 11}},      // note : eta bin 18: 0.4  to 0.6
    //     {19,         {5, 6, 7, 8, 9, 10, 11}},    // note : eta bin 19: 0.6  to 0.8
    //     {20,         {5, 6, 7, 8, 9, 10, 11}},    // note : eta bin 20: 0.8  to 1.0
    //     {21,      {4, 5, 6, 7, 8, 9, 10, 11}},  // note : eta bin 21: 1.0  to 1.2
    //     {22,      {4, 5, 6, 7, 8, 9, 10}},  // note : eta bin 22: 1.2  to 1.4
    //     {23,   {3, 4, 5, 6, 7, 8, 9}},  // note : eta bin 23: 1.4  to 1.6
    //     {24,{2, 3, 4, 5, 6, 7}},  // note : eta bin 24: 1.6  to 1.8
    //     {25,{2, 3, 4, 5}},      // note : eta bin 25: 1.8  to 2.0
    //     // {26,{}},        // note : eta bin 26: 2.0  to 2.2
    // };

    vector<pair<int,vector<int>>> included_eta_z_map = {
        {9,                   {9}}, // note : eta bin 11: -1.1 to -0.9 
        {10,                {8,9}}, // note : eta bin 12: -0.9 to -0.7
        {11,                {8,9}}, // note : eta bin 11: -0.7 to -0.5
        {12,            {7, 8, 9}}, // note : eta bin 12: -0.5 to -0.3
        {13,            {7, 8, 9}}, // note : eta bin 13: -0.3 to -0.1
        {14,         {6, 7, 8, 9}}, // note : eta bin 14: -0.1 to  0.1
        {15,         {6, 7, 8, 9}}, // note : eta bin 15:  0.1 to  0.3
        {16,         {6, 7, 8, 9}}, // note : eta bin 16:  0.3 to  0.5
        {17,      {5, 6, 7, 8, 9}}, // note : eta bin 17:  0.5 to  0.7
        {18,      {5, 6, 7, 8, 9}}, // note : eta bin 18:  0.7 to  0.9
        {19,      {5, 6, 7, 8, 9}}, // note : eta bin 19:  0.9 to  1.1
        {20,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 20:  1.1 to  1.3
        {21,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 21:  1.3 to  1.5
        {22,   {4, 5, 6, 7, 8, 9}}, // note : eta bin 22:  1.5 to  1.7
        {23,   {4, 5, 6, 7}},       // note : eta bin 23:  1.7 to  1.9
        {24,   {4, 5}}              // note : eta bin 24:  1.9 to  2.1
    };

    vector<int> fulleta_MC_included_z_bin = {
        4, 5, 6, 7, 8, 9
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

    EtaDistReader * file_half_corr = new EtaDistReader(run_type_MC, out_folder_directory1, fulleta_MC_included_z_bin, included_eta_z_map, Form("%s/%s",input_directory1.c_str(), input_file1.c_str()), Mbin_Z_evt_map_name_bool1);
    vector<string> centrality_region = file_half_corr -> Get_centrality_region();
    file_half_corr -> FindCoveredRegion();    
    file_half_corr -> DeriveAlphaCorrection(0);
    file_half_corr -> Set_alpha_corr_map(file_half_corr->Get_alpha_corr_map()[0], file_half_corr->Get_alpha_corr_map()[1]);
    file_half_corr -> ApplyAlphaCorrection();
    file_half_corr -> PrintPlots();
    file_half_corr -> EndRun();

    EtaDistReader * file_half_test = new EtaDistReader(run_type_MC, out_folder_directory2, fulleta_MC_included_z_bin, included_eta_z_map, Form("%s/%s",input_directory2.c_str(), input_file2.c_str()), Mbin_Z_evt_map_name_bool2);
    file_half_test -> Set_alpha_corr_map(file_half_corr->Get_alpha_corr_map()[0], file_half_corr->Get_alpha_corr_map()[1]);
    file_half_test -> ApplyAlphaCorrection();
    file_half_test -> DeriveAlphaCorrection(1);
    file_half_test -> PrintPlots();
    file_half_test -> EndRun();

    EtaDistReader * file_half_data = new EtaDistReader(run_type_data, out_folder_directory3, fulleta_MC_included_z_bin, included_eta_z_map, Form("%s/%s",input_directory3.c_str(), input_file3.c_str()), Mbin_Z_evt_map_name_bool3);
    file_half_data -> Set_alpha_corr_map(file_half_corr->Get_alpha_corr_map()[0], file_half_corr->Get_alpha_corr_map()[1]);
    file_half_data -> ApplyAlphaCorrection();
    file_half_data -> DeriveAlphaCorrection(1);
    file_half_data -> PrintPlots();
    file_half_data -> EndRun();

    vector<TH1F *> dNdeta_1D_fulleta_MC_1            = file_half_corr -> GetdNdeta_1D_fulleta_MC();
    vector<TH1F *> dNdeta_1D_MC_1                    = file_half_corr -> GetdNdeta_1D_MC();
    vector<TH1F *> dNdeta_1D_reco_single_original_1  = file_half_corr -> GetdNdeta_1D_reco_single_original();
    vector<TH1F *> dNdeta_1D_reco_multi_original_1   = file_half_corr -> GetdNdeta_1D_reco_multi_original();
    vector<TH1F *> dNdeta_1D_reco_single_postalpha_1 = file_half_corr -> GetdNdeta_1D_reco_single_postalpha();
    vector<TH1F *> dNdeta_1D_reco_multi_postalpha_1  = file_half_corr -> GetdNdeta_1D_reco_multi_postalpha();

    vector<TH1F *> dNdeta_1D_fulleta_MC_2            = file_half_test -> GetdNdeta_1D_fulleta_MC();
    vector<TH1F *> dNdeta_1D_MC_2                    = file_half_test -> GetdNdeta_1D_MC();
    vector<TH1F *> dNdeta_1D_reco_single_original_2  = file_half_test -> GetdNdeta_1D_reco_single_original();
    vector<TH1F *> dNdeta_1D_reco_multi_original_2   = file_half_test -> GetdNdeta_1D_reco_multi_original();
    vector<TH1F *> dNdeta_1D_reco_single_postalpha_2 = file_half_test -> GetdNdeta_1D_reco_single_postalpha();
    vector<TH1F *> dNdeta_1D_reco_multi_postalpha_2  = file_half_test -> GetdNdeta_1D_reco_multi_postalpha();

    vector<TH1F *> dNdeta_1D_reco_single_original_3  = file_half_data -> GetdNdeta_1D_reco_single_original();
    vector<TH1F *> dNdeta_1D_reco_multi_original_3   = file_half_data -> GetdNdeta_1D_reco_multi_original();
    vector<TH1F *> dNdeta_1D_reco_single_postalpha_3 = file_half_data -> GetdNdeta_1D_reco_single_postalpha();
    vector<TH1F *> dNdeta_1D_reco_multi_postalpha_3  = file_half_data -> GetdNdeta_1D_reco_multi_postalpha();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TLegend * legend = new TLegend(0.6,0.75,0.8,0.9);
    // legend -> SetMargin(0);
    legend->SetTextSize(0.03);

    SetsPhenixStyle();

    TCanvas * c2 = new TCanvas("c2","",950,800);
    
    
    c2 -> cd();
    c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf(",out_folder_directory3.c_str()));
    for (int i = 0; i < dNdeta_1D_fulleta_MC_1.size(); i++)
    {
        dNdeta_1D_fulleta_MC_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_fulleta_MC_2[i] -> SetLineColor(TColor::GetColor("#c48045"));

        // cout<<i<<" test, dNdeta_1D_fulleta_MC_1[i] -> GetMaximum() : "<<dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin())<<" "<<dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()<<endl;

        dNdeta_1D_fulleta_MC_1[i] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC_1[i] -> SetMaximum( dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()) * 1.8 );

        dNdeta_1D_fulleta_MC_1[i] -> Draw("hist");
        dNdeta_1D_fulleta_MC_2[i] -> Draw("hist same");

        // note : alpha corrections derived
        dNdeta_1D_reco_single_original_1[i] -> SetMarkerColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_single_original_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_single_original_1[i] -> Draw("p same");

        // note : alpha corrections tested
        dNdeta_1D_reco_single_original_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_single_original_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_single_original_2[i] -> Draw("p same");

        // note : data
        dNdeta_1D_reco_single_original_3[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_original_3[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_original_3[i] -> Draw("p same");

        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} Work-in-progress"));

        legend -> AddEntry(dNdeta_1D_fulleta_MC_1[0], "corr. true", "f");
        legend -> AddEntry(dNdeta_1D_reco_single_original_1[0], "corr. reco.", "ep");
        legend -> AddEntry(dNdeta_1D_fulleta_MC_2[0], "test true", "f");
        legend -> AddEntry(dNdeta_1D_reco_single_original_2[0], "test reco.", "ep");
        legend -> AddEntry(dNdeta_1D_reco_single_original_3[0], "data", "ep");
        legend -> Draw("same");

        c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf",out_folder_directory3.c_str()));
        c2 -> Clear();
        legend -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf)",out_folder_directory3.c_str()));
    c2 -> Clear();


    c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf(",out_folder_directory3.c_str()));
    for (int i = 0; i < dNdeta_1D_fulleta_MC_1.size(); i++)
    {
        dNdeta_1D_fulleta_MC_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_fulleta_MC_2[i] -> SetLineColor(TColor::GetColor("#c48045"));

        dNdeta_1D_fulleta_MC_1[i] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC_1[i] -> SetMaximum( dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()) * 1.8 );

        dNdeta_1D_fulleta_MC_1[i] -> Draw("hist");
        dNdeta_1D_fulleta_MC_2[i] -> Draw("hist same");

        // note : alpha corrections derived
        dNdeta_1D_reco_multi_original_1[i] -> SetMarkerColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_multi_original_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_multi_original_1[i] -> Draw("p same");
        // note : alpha correction test
        dNdeta_1D_reco_multi_original_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi_original_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi_original_2[i] -> Draw("p same");

        // note : data
        dNdeta_1D_reco_multi_original_3[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_original_3[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_original_3[i] -> Draw("p same");

        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));

        legend -> AddEntry(dNdeta_1D_fulleta_MC_1[0], "corr. true", "f");
        legend -> AddEntry(dNdeta_1D_reco_multi_original_1[0], "corr. reco.", "ep");
        legend -> AddEntry(dNdeta_1D_fulleta_MC_2[0], "test true", "f");
        legend -> AddEntry(dNdeta_1D_reco_multi_original_2[0], "test reco.", "ep");
        legend -> AddEntry(dNdeta_1D_reco_multi_original_3[0], "data", "ep");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} Work-in-progress"));
        legend -> Draw("same");

        c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf",out_folder_directory3.c_str()));
        c2 -> Clear();
        legend -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf)",out_folder_directory3.c_str()));
    c2 -> Clear();


    // note : post alpha correction
    c2 -> cd();
    c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf(",out_folder_directory3.c_str()));
    for (int i = 0; i < dNdeta_1D_fulleta_MC_1.size(); i++)
    {
        dNdeta_1D_fulleta_MC_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_fulleta_MC_2[i] -> SetLineColor(TColor::GetColor("#c48045"));

        // cout<<i<<" test, dNdeta_1D_fulleta_MC_1[i] -> GetMaximum() : "<<dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin())<<" "<<dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()<<endl;

        dNdeta_1D_fulleta_MC_1[i] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC_1[i] -> SetMaximum( dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()) * 1.8 );

        dNdeta_1D_fulleta_MC_1[i] -> Draw("hist");
        dNdeta_1D_fulleta_MC_2[i] -> Draw("hist same");

        // note : alpha corrections derived
        dNdeta_1D_reco_single_postalpha_1[i] -> SetMarkerColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_single_postalpha_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_single_postalpha_1[i] -> Draw("p same");

        // note : alpha corrections tested
        dNdeta_1D_reco_single_postalpha_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_single_postalpha_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_single_postalpha_2[i] -> Draw("p same");

        // note : data
        dNdeta_1D_reco_single_postalpha_3[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_postalpha_3[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_single_postalpha_3[i] -> Draw("p same");

        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} Work-in-progress"));

        legend -> AddEntry(dNdeta_1D_fulleta_MC_1[0], "corr. true", "f");
        legend -> AddEntry(dNdeta_1D_reco_single_postalpha_1[0], "corr. reco.", "ep");
        legend -> AddEntry(dNdeta_1D_fulleta_MC_2[0], "test true", "f");
        legend -> AddEntry(dNdeta_1D_reco_single_postalpha_2[0], "test reco.", "ep");
        legend -> AddEntry(dNdeta_1D_reco_single_postalpha_3[0], "data", "ep");
        legend -> Draw("same");

        c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf",out_folder_directory3.c_str()));
        c2 -> Clear();
        legend -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf)",out_folder_directory3.c_str()));
    c2 -> Clear();


    c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf(",out_folder_directory3.c_str()));
    for (int i = 0; i < dNdeta_1D_fulleta_MC_1.size(); i++)
    {
        dNdeta_1D_fulleta_MC_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_fulleta_MC_2[i] -> SetLineColor(TColor::GetColor("#c48045"));

        dNdeta_1D_fulleta_MC_1[i] -> SetMinimum(0);
        dNdeta_1D_fulleta_MC_1[i] -> SetMaximum( dNdeta_1D_fulleta_MC_1[i] -> GetBinContent(dNdeta_1D_fulleta_MC_1[i] -> GetMaximumBin()) * 1.8 );

        dNdeta_1D_fulleta_MC_1[i] -> Draw("hist");
        dNdeta_1D_fulleta_MC_2[i] -> Draw("hist same");

        // note : alpha corrections derived
        dNdeta_1D_reco_multi_postalpha_1[i] -> SetMarkerColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_multi_postalpha_1[i] -> SetLineColor(TColor::GetColor("#D8364D"));
        dNdeta_1D_reco_multi_postalpha_1[i] -> Draw("p same");
        // note : alpha correction test
        dNdeta_1D_reco_multi_postalpha_2[i] -> SetMarkerColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi_postalpha_2[i] -> SetLineColor(TColor::GetColor("#c48045"));
        dNdeta_1D_reco_multi_postalpha_2[i] -> Draw("p same");

        // note : data
        dNdeta_1D_reco_multi_postalpha_3[i] -> SetMarkerColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_postalpha_3[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        dNdeta_1D_reco_multi_postalpha_3[i] -> Draw("p same");

        draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));

        legend -> AddEntry(dNdeta_1D_fulleta_MC_1[0], "corr. true", "f");
        legend -> AddEntry(dNdeta_1D_reco_multi_postalpha_1[0], "corr. reco.", "ep");
        legend -> AddEntry(dNdeta_1D_fulleta_MC_2[0], "test true", "f");
        legend -> AddEntry(dNdeta_1D_reco_multi_postalpha_2[0], "test reco.", "ep");
        legend -> AddEntry(dNdeta_1D_reco_multi_postalpha_3[0], "data", "ep");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX INTT}} Work-in-progress"));
        legend -> Draw("same");

        c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf",out_folder_directory3.c_str()));
        c2 -> Clear();
        legend -> Clear();
    }
    c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf)",out_folder_directory3.c_str()));
    c2 -> Clear();

    TFile * file_out = new TFile(Form("%s/hist_for_AN.root",out_folder_directory3.c_str()),"RECREATE");
    file_out -> cd();

    for (int i = 0; i < dNdeta_1D_fulleta_MC_1.size(); i++) {dNdeta_1D_fulleta_MC_1[i]->Write(Form("dNdeta_1D_truth_corr_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_fulleta_MC_2.size(); i++) {dNdeta_1D_fulleta_MC_2[i]->Write(Form("dNdeta_1D_truth_test_Mbin%i",i));}

    for (int i = 0; i < dNdeta_1D_reco_multi_original_1.size(); i++)     {dNdeta_1D_reco_multi_original_1[i]->Write(Form("dNdeta_1D_MCreco_loose_NoCorrection_corr_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_multi_postalpha_1.size(); i++)   {dNdeta_1D_reco_multi_postalpha_1[i]->Write(Form("dNdeta_1D_MCreco_loose_PostCorrection_corr_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_original_1.size(); i++)   {dNdeta_1D_reco_single_original_1[i]->Write(Form("dNdeta_1D_MCreco_tight_NoCorrection_corr_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_postalpha_1.size(); i++) {dNdeta_1D_reco_single_postalpha_1[i]->Write(Form("dNdeta_1D_MCreco_tight_PostCorrection_corr_Mbin%i",i));}
    
    for (int i = 0; i < dNdeta_1D_reco_multi_original_2.size(); i++)     {dNdeta_1D_reco_multi_original_2[i]->Write(Form("dNdeta_1D_MCreco_loose_NoCorrection_test_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_multi_postalpha_2.size(); i++)   {dNdeta_1D_reco_multi_postalpha_2[i]->Write(Form("dNdeta_1D_MCreco_loose_PostCorrection_test_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_original_2.size(); i++)   {dNdeta_1D_reco_single_original_2[i]->Write(Form("dNdeta_1D_MCreco_tight_NoCorrection_test_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_postalpha_2.size(); i++) {dNdeta_1D_reco_single_postalpha_2[i]->Write(Form("dNdeta_1D_MCreco_tight_PostCorrection_test_Mbin%i",i));}
    

    for (int i = 0; i < dNdeta_1D_reco_multi_original_3.size(); i++)     {dNdeta_1D_reco_multi_original_3[i]->Write(Form("dNdeta_1D_Datareco_loose_NoCorrection_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_multi_postalpha_3.size(); i++)   {dNdeta_1D_reco_multi_postalpha_3[i]->Write(Form("dNdeta_1D_Datareco_loose_PostCorrection_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_original_3.size(); i++)   {dNdeta_1D_reco_single_original_3[i]->Write(Form("dNdeta_1D_Datareco_tight_NoCorrection_Mbin%i",i));}
    for (int i = 0; i < dNdeta_1D_reco_single_postalpha_3.size(); i++) {dNdeta_1D_reco_single_postalpha_3[i]->Write(Form("dNdeta_1D_Datareco_tight_PostCorrection_Mbin%i",i));}

    file_out -> Close();

    return 0;
}