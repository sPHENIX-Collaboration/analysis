// #include "sPhenixStyle.C"
#include "../../EtaDistReader.h"
#include "../../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

// note : vector<vector<string>> input_file_full_info { {data_type, full_input_file_directory, full_out_folder_directory, abbreviation_of_file} }
// note : vector<vector<string>> input_file_full_info { 
// note :     {
// note :         data_type, 
// note :         full_input_file_directory, 
// note :         full_out_folder_directory, 
// note :         abbreviation_of_file // note : corr. / test / data / data_z_shifted <------> Original / Inner cluster #phi rotated by #pi
// note :     } 
// note : }

// note: ana_mode 1: focus on the correction and the dNdeta distributions
// note: ana_mode 2: focus on the delta_phi distributions 
void main_code(int ana_mode, vector<vector<string>> input_file_full_info, string final_full_output_folder_directory)
{
    // note : if deriving the correction is needed, then that one has to be placed in the first place. 
    // note : and it has to be MC, for sure

    // note : always MC first, let's try in this way

    // note: ana_mode 1: focus on the correction and the dNdeta distributions
    // note: ana_mode 2: focus on the delta_phi distributions 

    for (int i = 0; i < input_file_full_info.size(); i++)
    {
        if (input_file_full_info[i].size() != 4) {
            cout<<"The input_file_full_info does not have the correct size, please check!"<<endl;
            exit(1);
        }
    }

    if (ana_mode == 1 && input_file_full_info[0][0] != "MC") {
        cout<<"the ana_mode tag is ON, but the first data_type is not MC, please check!"<<endl;
        exit(1);
    }

    system(Form("if [ ! -d %s ]; then mkdir %s; fi", final_full_output_folder_directory.c_str(), final_full_output_folder_directory.c_str()));

    string color_code[5] = {"#fedc97", "#b5b682", "#7c9885", "#28666e", "#033f63"};
    vector<string> color_code_vec = {"#c48045", "#D8364D", "#7c9885", "#28666e", "#000000"};

    vector<pair<int,vector<int>>> included_eta_z_map = ana_map_version::evt_tracklet_included_eta_z_map;
    vector<int> fulleta_MC_included_z_bin = ana_map_version::evt_tracklet_fulleta_MC_included_z_bin;

    vector<EtaDistReader *> N_tracklet_file(input_file_full_info.size(), nullptr);
    vector<string> centrality_region; centrality_region.clear();

    int count_MC_file = 0;
    bool found_data_file_tag = false;
    for (int i = 0; i < input_file_full_info.size(); i++){
        if (input_file_full_info[i][0] == "MC") {count_MC_file += 1;}
        if (input_file_full_info[i][0] == "data") {found_data_file_tag = true;}
    }
    string sph_labeling = (found_data_file_tag == true) ? "Work-in-progress" : "Simulation";
    
    vector<vector<TH1F *>> dNdeta_1D_fulleta_MC(count_MC_file);
    vector<vector<TH1F *>> dNdeta_1D_MC(count_MC_file);
    vector<vector<TH1F *>> dNdeta_1D_reco_single_original(input_file_full_info.size());
    vector<vector<TH1F *>> dNdeta_1D_reco_multi_original(input_file_full_info.size());
    vector<vector<TH1F *>> dNdeta_1D_reco_single_postalpha(input_file_full_info.size());
    vector<vector<TH1F *>> dNdeta_1D_reco_multi_postalpha(input_file_full_info.size());
    vector<vector<TH1F *>> DeltaPhi_Multi_stack_1D(input_file_full_info.size());
    
    for (int i = 0; i < input_file_full_info.size(); i++)
    {
        N_tracklet_file[i] = new EtaDistReader(
            input_file_full_info[i][0], // note : the data_type
            input_file_full_info[i][2], // note : full output folder directory
            fulleta_MC_included_z_bin,
            included_eta_z_map,
            input_file_full_info[i][1] // note : full input file directory

        );

        if (i == 0) {centrality_region = N_tracklet_file[i] -> Get_centrality_region();}
        N_tracklet_file[i] -> FindCoveredRegion();
        
        if (ana_mode == 1){
            if (i == 0) {N_tracklet_file[i] -> DeriveAlphaCorrection(0);} // note : for deriving the correction with the "_corr" file

            // note : the correction map is kept in the first file
            N_tracklet_file[i] -> Set_alpha_corr_map(N_tracklet_file[0]->Get_alpha_corr_map()[0], N_tracklet_file[0]->Get_alpha_corr_map()[1]);
            
            N_tracklet_file[i] -> ApplyAlphaCorrection();
            if (i != 0) {N_tracklet_file[i] -> DeriveAlphaCorrection(1);}
            N_tracklet_file[i] -> PrintPlots();
            N_tracklet_file[i] -> EndRun();
        }

        // note : done with the preparation, now starts to extract the distributions

        if (ana_mode == 1)
        {
            if ( input_file_full_info[i][0] == "MC" )
            {
                dNdeta_1D_fulleta_MC[i]            = N_tracklet_file[i] -> GetdNdeta_1D_fulleta_MC();
                dNdeta_1D_MC[i]                    = N_tracklet_file[i] -> GetdNdeta_1D_MC();
            }

            dNdeta_1D_reco_single_original[i]  = N_tracklet_file[i] -> GetdNdeta_1D_reco_single_original(); 
            dNdeta_1D_reco_multi_original[i]   = N_tracklet_file[i] -> GetdNdeta_1D_reco_multi_original(); 
            dNdeta_1D_reco_single_postalpha[i] = N_tracklet_file[i] -> GetdNdeta_1D_reco_single_postalpha(); 
            dNdeta_1D_reco_multi_postalpha[i]  = N_tracklet_file[i] -> GetdNdeta_1D_reco_multi_postalpha();
        }        

        if (ana_mode == 2)
        {
            DeltaPhi_Multi_stack_1D[i] = N_tracklet_file[i] -> GetDeltaPhi_Multi_stack_1D_radian();
        }    
    } // note : end of the preparation for each file

    SetsPhenixStyle();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    TLegend * legend_DeltaPhi = new TLegend(0.25,0.7,0.75,0.8);
    // legend_DeltaPhi -> SetMargin(0);
    legend_DeltaPhi->SetTextSize(0.03);

    double legend_dNdeta_text_size = 0.03;
    double legend_dNdeta_upper_y = 0.9;
    TLegend * legend_dNdeta = new TLegend(
        0.45, // note : x1
        legend_dNdeta_upper_y - (count_MC_file + input_file_full_info.size()) * legend_dNdeta_text_size, // note : y1
        0.8, // note : x2
        legend_dNdeta_upper_y // note : y2
    );
    // legend_dNdeta -> SetMargin(0);
    legend_dNdeta->SetTextSize(legend_dNdeta_text_size);

    TCanvas * c2 = new TCanvas("c2","",950,800);

    if (ana_mode == 1)
    {
        // note : [file_i][for_each_centrality_region]
        // note : for the MC
        for (int file_i = 0; file_i < count_MC_file; file_i++)
        {
            for (int Mbin_i = 0; Mbin_i < dNdeta_1D_fulleta_MC[0].size(); Mbin_i++)
            {
                dNdeta_1D_fulleta_MC[file_i][Mbin_i] -> SetLineColor(TColor::GetColor(color_code_vec[file_i].c_str()));

                // note : the first file
                if (file_i == 0)
                {
                    dNdeta_1D_fulleta_MC[file_i][Mbin_i] -> SetMinimum(0);
                    dNdeta_1D_fulleta_MC[file_i][Mbin_i] -> SetMaximum( dNdeta_1D_fulleta_MC[file_i][Mbin_i] -> GetBinContent(dNdeta_1D_fulleta_MC[file_i][Mbin_i] -> GetMaximumBin()) * 1.8 );
                }
            }

            legend_dNdeta -> AddEntry(dNdeta_1D_fulleta_MC[file_i][0], Form("%s true",input_file_full_info[file_i][3].c_str()), "f");
        }

        // note : for the reco distributions
        for (int file_i = 0; file_i < input_file_full_info.size(); file_i++)
        {
            for (int Mbin_i = 0; Mbin_i < dNdeta_1D_reco_single_original[0].size(); Mbin_i++)
            {
                dNdeta_1D_reco_single_original[file_i][Mbin_i] -> SetMarkerColor(TColor::GetColor( color_code_vec[file_i].c_str() ));
                dNdeta_1D_reco_single_original[file_i][Mbin_i] -> SetLineColor(TColor::GetColor( color_code_vec[file_i].c_str() ));

                dNdeta_1D_reco_multi_original[file_i][Mbin_i] -> SetMarkerColor(TColor::GetColor( color_code_vec[file_i].c_str() ));
                dNdeta_1D_reco_multi_original[file_i][Mbin_i] -> SetLineColor(TColor::GetColor( color_code_vec[file_i].c_str() ));

                dNdeta_1D_reco_single_postalpha[file_i][Mbin_i] -> SetMarkerColor(TColor::GetColor( color_code_vec[file_i].c_str() ));
                dNdeta_1D_reco_single_postalpha[file_i][Mbin_i] -> SetLineColor(TColor::GetColor( color_code_vec[file_i].c_str() ));

                dNdeta_1D_reco_multi_postalpha[file_i][Mbin_i] -> SetMarkerColor(TColor::GetColor( color_code_vec[file_i].c_str() ));
                dNdeta_1D_reco_multi_postalpha[file_i][Mbin_i] -> SetLineColor(TColor::GetColor( color_code_vec[file_i].c_str() ));
            }

            legend_dNdeta -> AddEntry(dNdeta_1D_reco_single_original[file_i][0], Form("%s reco",input_file_full_info[file_i][3].c_str()), "ep");
        }


        c2 -> cd();
        c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf(",final_full_output_folder_directory.c_str()));
        for (int i = 0; i < dNdeta_1D_fulleta_MC[0].size(); i++)
        {
            for (int true_i = 0; true_i < count_MC_file; true_i++)
            {
                if (true_i == 0) {
                    dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist");
                }
                else {dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist same");}
            }

            for (int reco_i = 0; reco_i < input_file_full_info.size(); reco_i++)
            {
                dNdeta_1D_reco_single_original[reco_i][i] -> Draw("p same");
            }

            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));

            legend_dNdeta -> Draw("same");

            c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf",final_full_output_folder_directory.c_str()));
            c2 -> Clear();
            
        }
        c2 -> Print(Form("%s/dNdeta_comp_single_original.pdf)",final_full_output_folder_directory.c_str()));
        c2 -> Clear();

        c2 -> cd();
        c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf(",final_full_output_folder_directory.c_str()));
        for (int i = 0; i < dNdeta_1D_fulleta_MC[0].size(); i++)
        {
            for (int true_i = 0; true_i < count_MC_file; true_i++)
            {
                if (true_i == 0) {
                    dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist");
                }
                else {dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist same");}
            }

            for (int reco_i = 0; reco_i < input_file_full_info.size(); reco_i++)
            {
                dNdeta_1D_reco_multi_original[reco_i][i] -> Draw("p same");
            }

            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));

            legend_dNdeta -> Draw("same");

            c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf",final_full_output_folder_directory.c_str()));
            c2 -> Clear();
            
        }
        c2 -> Print(Form("%s/dNdeta_comp_multi_original.pdf)",final_full_output_folder_directory.c_str()));
        c2 -> Clear();


        c2 -> cd();
        c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf(",final_full_output_folder_directory.c_str()));
        for (int i = 0; i < dNdeta_1D_fulleta_MC[0].size(); i++)
        {
            for (int true_i = 0; true_i < count_MC_file; true_i++)
            {
                if (true_i == 0) {
                    dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist");
                }
                else {dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist same");}
            }

            for (int reco_i = 0; reco_i < input_file_full_info.size(); reco_i++)
            {
                dNdeta_1D_reco_single_postalpha[reco_i][i] -> Draw("p same");
            }

            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));

            legend_dNdeta -> Draw("same");

            c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf",final_full_output_folder_directory.c_str()));
            c2 -> Clear();
            
        }
        c2 -> Print(Form("%s/dNdeta_comp_single_postalpha.pdf)",final_full_output_folder_directory.c_str()));
        c2 -> Clear();

        c2 -> cd();
        c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf(",final_full_output_folder_directory.c_str()));
        for (int i = 0; i < dNdeta_1D_fulleta_MC[0].size(); i++)
        {
            for (int true_i = 0; true_i < count_MC_file; true_i++)
            {
                if (true_i == 0) {
                    dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist");
                }
                else {dNdeta_1D_fulleta_MC[true_i][i] -> Draw("hist same");}
            }

            for (int reco_i = 0; reco_i < input_file_full_info.size(); reco_i++)
            {
                dNdeta_1D_reco_multi_postalpha[reco_i][i] -> Draw("p same");
            }

            draw_text -> DrawLatex(0.21, 0.90, Form("Centrality : %s",centrality_region[i].c_str()));
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));

            legend_dNdeta -> Draw("same");

            c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf",final_full_output_folder_directory.c_str()));
            c2 -> Clear();
            
        }
        c2 -> Print(Form("%s/dNdeta_comp_multi_postalpha.pdf)",final_full_output_folder_directory.c_str()));
        c2 -> Clear();
    }

    // note : the default is just two files for this inner phi rotate test
    if (ana_mode == 2)
    {
        if (DeltaPhi_Multi_stack_1D[0].size() != DeltaPhi_Multi_stack_1D[1].size())
        {
            cout<<"The size of the two vectors are not the same"<<endl;
            return;
        }    

        c2 -> cd();
        c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf(",final_full_output_folder_directory.c_str()));
        for (int i = 0; i < DeltaPhi_Multi_stack_1D[0].size(); i++)
        {
            for (int file_i = 0; file_i < DeltaPhi_Multi_stack_1D.size(); file_i++)
            {
                if (i == 0)
                {
                    legend_DeltaPhi -> AddEntry(DeltaPhi_Multi_stack_1D[file_i][0], input_file_full_info[file_i][3].c_str(), "f");
                }

                DeltaPhi_Multi_stack_1D[file_i][i] -> SetFillColorAlpha(3, 0.0);
                DeltaPhi_Multi_stack_1D[file_i][i] -> GetXaxis() -> SetTitle("#Delta#phi [radian]");
                DeltaPhi_Multi_stack_1D[file_i][i] -> GetYaxis() -> SetTitle("Entry");
                DeltaPhi_Multi_stack_1D[file_i][i] -> SetLineColor(TColor::GetColor(color_code_vec[file_i].c_str()));

                if (file_i == 0) {
                    DeltaPhi_Multi_stack_1D[file_i][i] -> SetMinimum(0);
                    DeltaPhi_Multi_stack_1D[file_i][i] -> SetMaximum( DeltaPhi_Multi_stack_1D[file_i][i] -> GetBinContent( DeltaPhi_Multi_stack_1D[file_i][i] -> GetMaximumBin()) * 1.5 );
                    DeltaPhi_Multi_stack_1D[file_i][i] -> Draw("hist");
                }
                else {
                    DeltaPhi_Multi_stack_1D[file_i][i] -> Draw("hist same");
                }
            }

            draw_text -> DrawLatex(0.21, 0.90, Form("%s",DeltaPhi_Multi_stack_1D[0][i]->GetTitle()));
            string MBin_text  = "MBin";      int MBin_int   = stoi(string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).substr(string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).find(MBin_text.c_str()) + MBin_text.length(),  string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).find("_")    - (string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).find(MBin_text.c_str())+MBin_text.length()) ));
            string EtaBin_text = "EtaBin";   int EtaBin_int = stoi(string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).substr(string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).find(EtaBin_text.c_str()) + EtaBin_text.length(),  string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).length() - (string(DeltaPhi_Multi_stack_1D[0][i]->GetTitle()).find(EtaBin_text.c_str())+EtaBin_text.length()) ));

            draw_text -> DrawLatex(0.21, 0.86, Form("Centrality: %s, #eta region: %s",centrality_region[MBin_int].c_str(), N_tracklet_file[0]->Get_eta_region_txt(EtaBin_int).c_str()));
            
            legend_DeltaPhi -> Draw("same");
            ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} %s", sph_labeling.c_str()));

            c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf",final_full_output_folder_directory.c_str()));
            c2 -> Clear();
        }
        c2 -> Print(Form("%s/DeltaPhi_PhiRotate_comp.pdf)",final_full_output_folder_directory.c_str()));
        c2 -> Clear();

    }

    return;
}

// note : for testing, probably
void TrackletWrapper()
{
    // int ana_mode = 2;

    // vector<vector<string>> input_file_full_info = {
    //     {
    //         "MC",
    //         "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet/complete_file/merged_file_folder/merged_hist_half_corr.root",
    //         "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet/complete_file/merged_file_folder/Eta_DistReader_output",
    //         "Corr. Original"
    //     },
    //     {
    //         "MC",
    //         "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_inner_phi_rotate/complete_file/merged_file_folder/merged_hist_half_corr.root",
    //         "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_inner_phi_rotate/complete_file/merged_file_folder/Eta_DistReader_output",
    //         "Corr. Inner cluster #phi rotated by #pi"
    //     }
    // };
    
    // string final_full_output_folder_directory = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_inner_phi_rotate/complete_file/merged_file_folder/TrackletWrapper_output";

    int ana_mode = 1;

    string MC_tracklet_original_reweight_complete_folder = "/gpfs/mnt/gpfs02/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_Original_MC_reweight/complete_file";
    string MC_tracklet_complete_folder = "/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet/complete_file";
    string data_mother_folder = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test";

    vector<vector<string>> input_file_full_info = {
        // {
        //     "MC",
        //     Form("%s/merged_file_folder/merged_hist_half_corr.root",MC_tracklet_complete_folder.c_str()),
        //     Form("%s/merged_file_folder/Half_corr_Eta_DistReader_output",MC_tracklet_complete_folder.c_str()),
        //     "Corr."
        // },
        // {
        //     "MC",
        //     Form("%s/merged_file_folder/merged_hist_half_test.root",MC_tracklet_complete_folder.c_str()),
        //     Form("%s/merged_file_folder/Half_test_Eta_DistReader_output",MC_tracklet_complete_folder.c_str()),
        //     "Test"
        // },
        // {
        //     "MC",
        //     Form("%s/merged_file_folder/merged_hist_half_corr.root",MC_tracklet_original_reweight_complete_folder.c_str()),
        //     Form("%s/merged_file_folder/Half_corr_Eta_DistReader_output",MC_tracklet_original_reweight_complete_folder.c_str()),
        //     "Corr. Orig. weighted"
        // },
        // {
        //     "data",
        //     Form("%s/evt_tracklet/complete_file/merged_file_folder/merged_hist_full.root",data_mother_folder.c_str()),
        //     Form("%s/evt_tracklet/complete_file/merged_file_folder/Full_data_Eta_DistReader_output",data_mother_folder.c_str()),
        //     "Data original"
        // },
        // {
        //     "data",
        //     Form("%s/evt_tracklet_INTTz_shifted/complete_file/merged_file_folder/merged_hist_full.root",data_mother_folder.c_str()),
        //     Form("%s/evt_tracklet_INTTz_shifted/complete_file/merged_file_folder/Full_data_Eta_DistReader_output",data_mother_folder.c_str()),
        //     "Data INTTz shifted"
        // }

        {
            "MC",
            Form("/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_ZvtxRange_n300mm_to_n60mm/complete_file/merged_file_folder/merged_hist_full.root"),
            Form("/sphenix/user/ChengWei/sPH_dNdeta/Sim_Ntuple_HIJING_new_20240424_HR_test/evt_tracklet_ZvtxRange_n300mm_to_n60mm/complete_file/merged_file_folder/EtaDistReader_output"),
            "MC"
        },
        {
            "data",
            Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_tracklet_ZvtxRange_n300mm_to_n60mm/complete_file/merged_file_folder/merged_hist_full.root"),
            Form("/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_tracklet_ZvtxRange_n300mm_to_n60mm/complete_file/merged_file_folder/EtaDistReader_output"),
            "data"
        }

    };
    
    // string final_full_output_folder_directory = Form("%s/evt_tracklet_INTTz_shifted/complete_file/merged_file_folder/TrackletWrapper_output",data_mother_folder.c_str());
    string final_full_output_folder_directory = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_tracklet_ZvtxRange_n300mm_to_n60mm/complete_file/merged_file_folder/EtaDistReader_output";

    main_code(3, input_file_full_info, final_full_output_folder_directory);
}