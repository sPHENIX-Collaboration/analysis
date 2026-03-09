#include "../../../../EtaDistReader.h"
#include "../../../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;
// #include "../../../../sPhenixStyle.C"

int merged_tracklet_ana()
{
   string run_type_data = "data";

    string input_directory1 = ana_map_version::data_input_directory + "/evt_tracklet/complete_file/" + ana_map_version::data_tracklet_merged_folder;
    string input_file1 = "merged_hist_full.root";
    string out_folder_directory1 = input_directory1 + "/eta_closer_out_test_MultiZBin";
    bool Mbin_Z_evt_map_name_bool1 = 0;

    string input_directory2 = ana_map_version::data_input_directory + "/evt_tracklet_inner_phi_rotate/complete_file/" + ana_map_version::data_tracklet_merged_folder;
    string input_file2 = "merged_hist_full.root";
    string out_folder_directory2 = input_directory2 + "/eta_closer_out_test_MultiZBin";
    bool Mbin_Z_evt_map_name_bool2 = 0;

    vector<pair<int,vector<int>>> included_eta_z_map = ana_map_version::evt_tracklet_included_eta_z_map;
    vector<int> fulleta_MC_included_z_bin = ana_map_version::evt_tracklet_fulleta_MC_included_z_bin;


    EtaDistReader * file_half_corr                  = new EtaDistReader(run_type_data, out_folder_directory1, fulleta_MC_included_z_bin, included_eta_z_map, Form("%s/%s",input_directory1.c_str(), input_file1.c_str()), Mbin_Z_evt_map_name_bool1);
    vector<string> centrality_region = file_half_corr -> Get_centrality_region();
    file_half_corr -> FindCoveredRegion();    


    EtaDistReader * file_half_inner_phi_rotate_corr = new EtaDistReader(run_type_data, out_folder_directory2, fulleta_MC_included_z_bin, included_eta_z_map, Form("%s/%s",input_directory2.c_str(), input_file2.c_str()), Mbin_Z_evt_map_name_bool2);


    vector<TH1F *> DeltaPhi_Multi_stack_1D_1 = file_half_corr -> GetDeltaPhi_Multi_stack_1D_radian();
    // vector<TH1F *> dNdeta_1D_fulleta_MC_1  = file_half_corr -> GetdNdeta_1D_fulleta_MC();
    // vector<TH1F *> dNdeta_1D_MC_1          = file_half_corr -> GetdNdeta_1D_MC();
    // vector<TH1F *> dNdeta_1D_reco_single_1 = file_half_corr -> GetdNdeta_1D_reco_single();
    // vector<TH1F *> dNdeta_1D_reco_multi_1  = file_half_corr -> GetdNdeta_1D_reco_multi();

    vector<TH1F *> DeltaPhi_Multi_stack_1D_2 = file_half_inner_phi_rotate_corr -> GetDeltaPhi_Multi_stack_1D_radian();
    // vector<TH1F *> dNdeta_1D_fulleta_MC_2  = file_half_inner_phi_rotate_corr -> GetdNdeta_1D_fulleta_MC();
    // vector<TH1F *> dNdeta_1D_MC_2          = file_half_inner_phi_rotate_corr -> GetdNdeta_1D_MC();
    // vector<TH1F *> dNdeta_1D_reco_single_2 = file_half_inner_phi_rotate_corr -> GetdNdeta_1D_reco_single();
    // vector<TH1F *> dNdeta_1D_reco_multi_2  = file_half_inner_phi_rotate_corr -> GetdNdeta_1D_reco_multi();

    SetsPhenixStyle();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TLegend * legend = new TLegend(0.25,0.7,0.75,0.8);
    // legend -> SetMargin(0);
    legend->SetTextSize(0.03);

    TCanvas * c2 = new TCanvas("c2","",950,800);
    
    
    if (DeltaPhi_Multi_stack_1D_1.size() != DeltaPhi_Multi_stack_1D_2.size())
    {
        cout<<"The size of the two vectors are not the same"<<endl;
        return 0;
    }

    c2 -> cd();
    c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf(",out_folder_directory2.c_str()));
    for (int i = 0; i < DeltaPhi_Multi_stack_1D_1.size(); i++)
    {
        if (i == 0)
        {
            legend -> AddEntry(DeltaPhi_Multi_stack_1D_1[0],"Original","f");
            legend -> AddEntry(DeltaPhi_Multi_stack_1D_2[0],"Inner cluster #phi rotated by #pi","f");
        }

        DeltaPhi_Multi_stack_1D_1[i] -> SetFillColorAlpha(3, 0.0);
        DeltaPhi_Multi_stack_1D_2[i] -> SetFillColorAlpha(3, 0.0);
        DeltaPhi_Multi_stack_1D_1[i] -> GetXaxis() -> SetTitle("#Delta#phi [radian]");
        DeltaPhi_Multi_stack_1D_1[i] -> GetYaxis() -> SetTitle("Entry");
        DeltaPhi_Multi_stack_1D_1[i] -> SetLineColor(TColor::GetColor("#1A3947"));
        DeltaPhi_Multi_stack_1D_2[i] -> SetLineColor(TColor::GetColor("#c48045"));

        DeltaPhi_Multi_stack_1D_1[i] -> SetMinimum(0);
        DeltaPhi_Multi_stack_1D_1[i] -> SetMaximum( DeltaPhi_Multi_stack_1D_1[i] -> GetBinContent( DeltaPhi_Multi_stack_1D_1[i] -> GetMaximumBin()) * 1.5 );

        DeltaPhi_Multi_stack_1D_1[i] -> Draw("hist");
        DeltaPhi_Multi_stack_1D_2[i] -> Draw("hist same");
        draw_text -> DrawLatex(0.21, 0.90, Form("%s",DeltaPhi_Multi_stack_1D_1[i]->GetTitle()));
        
        string MBin_text  = "MBin";      int MBin_int   = stoi(string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).substr(string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).find(MBin_text.c_str()) + MBin_text.length(),  string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).find("_")    - (string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).find(MBin_text.c_str())+MBin_text.length()) ));
        string EtaBin_text = "EtaBin";   int EtaBin_int = stoi(string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).substr(string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).find(EtaBin_text.c_str()) + EtaBin_text.length(),  string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).length() - (string(DeltaPhi_Multi_stack_1D_1[i]->GetTitle()).find(EtaBin_text.c_str())+EtaBin_text.length()) ));

        draw_text -> DrawLatex(0.21, 0.86, Form("Centrality: %s, eta region: %s",centrality_region[MBin_int].c_str(), file_half_corr->Get_eta_region_txt(EtaBin_int).c_str()));
        
        legend -> Draw("same");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));

        c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf",out_folder_directory2.c_str()));
        c2 -> Clear();
    }
    c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf)",out_folder_directory2.c_str()));
    c2 -> Clear();

    return 0;
}