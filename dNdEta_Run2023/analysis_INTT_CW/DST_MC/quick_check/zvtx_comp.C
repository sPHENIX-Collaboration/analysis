#include "sPhenixStyle.C"
#include "../ana_map_folder/ana_map_v1.h"
namespace ana_map_version = ANA_MAP_V3;

#include "../ReadINTTZ/ReadINTTZ.C"
#include "../ReadINTTZ/ReadINTTZCombine.C"

double gaus_func(double *x, double *par)
{
    // note : par[0] : size
    // note : par[1] : mean
    // note : par[2] : width
    // note : par[3] : offset 
    return par[0] * TMath::Gaus(x[0],par[1],par[2]) + par[3];
}

void zvtx_comp()
{
    string private_gen_zvtx_file = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/20869/condor_zvtx_folder/file_merged.root";
    string F4A_gen_zvtx_file = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_vtxZ/complete_file/merged_file.root";    
    string output_folder = "/sphenix/user/ChengWei/INTT/INTT_commissioning/ZeroField/F4A_20869/2024_05_07/folder_Data_CombinedNtuple_Run20869_HotDead_BCO_ADC_Survey_test/evt_vtxZ/complete_file";
    int Nclus_cut = 1000;

    TFile * file_private_gen_zvtx = TFile::Open(private_gen_zvtx_file.c_str());
    TTree * tree_private_gen_zvtx = (TTree *)file_private_gen_zvtx -> Get("tree_Z");
    ReadINTTZ * private_gen_zvtx = new ReadINTTZ(tree_private_gen_zvtx);

    TFile * file_F4A_gen_zvtx = TFile::Open(F4A_gen_zvtx_file.c_str());
    TTree * tree_F4A_gen_zvtx = (TTree *)file_F4A_gen_zvtx -> Get("tree_Z");
    ReadINTTZCombine * F4A_gen_zvtx = new ReadINTTZCombine(tree_F4A_gen_zvtx);

    TH1F * zvtx_diff = new TH1F("","zvtx_diff;Private - F4A [cm];Entry",100,-2,2);
    TH2F * zvtx_correlation = new TH2F("","zvtx_correlation;Private [cm];F4A [cm]",100,-50,10,100,-50,10);
    TH2F * zvtx_diff_nclus = new TH2F("","zvtx_diff_nclus;NClus;Private - F4A [cm]",100,0,9000,100,-2,2);
    TH2F * zvtx_diff_privateZ = new TH2F("","zvtx_diff_privateZ;Private reco. Z [cm];Private - F4A [cm]",200,-50,10,200,-2,2);
    TH2F * F4AMBDz_privateINTTz_diff_nclus = new TH2F("","F4AMBDz_privateINTTz_diff_nclus;NClus;Private INTT recoZ - F4A MBD recoZ [cm]",200,0,9000,200,-10,10);
    TH2F * F4AMBDz_privateINTTz_diff_F4AMBDz = new TH2F("","F4AMBDz_privateINTTz_diff_F4AMBDz;F4A MBD recoZ [cm];Private INTT recoZ - F4A MBD recoZ [cm]",200,-50,10,200,-10,10);

    string unit_text = "cm";
    double unit_correction = 0.1;

    int private_gen_eID_offset = 0;

    for (int event_i = 0; event_i < 400000; event_i ++)
    {
        private_gen_zvtx -> LoadTree(event_i + 1 + private_gen_eID_offset );
        private_gen_zvtx -> GetEntry(event_i + 1 + private_gen_eID_offset );

        F4A_gen_zvtx -> LoadTree(event_i);
        F4A_gen_zvtx -> GetEntry(event_i);

        // note : MC, but the event is not the minimum bias event
        if (F4A_gen_zvtx -> is_min_bias_wozdc == 0) {continue;}

        // note : to get rid of the nan value
        if (F4A_gen_zvtx -> Centrality_float != F4A_gen_zvtx -> Centrality_float) {continue;}
        if (F4A_gen_zvtx -> MBD_reco_z       != F4A_gen_zvtx -> MBD_reco_z)       {continue;}

        if (F4A_gen_zvtx -> nclu_inner <= 0)   {continue;}
        if (F4A_gen_zvtx -> nclu_outer <= 0)   {continue;}
        if ((F4A_gen_zvtx -> nclu_inner + F4A_gen_zvtx -> nclu_outer) < Nclus_cut) {continue;}

        double MBD_charge_assy = (F4A_gen_zvtx -> MBD_north_charge_sum - F4A_gen_zvtx -> MBD_south_charge_sum) / (F4A_gen_zvtx -> MBD_north_charge_sum + F4A_gen_zvtx -> MBD_south_charge_sum);
        if ( MBD_charge_assy < -1 * ana_map_version::MBD_assy_ratio_cut || MBD_charge_assy >  ana_map_version::MBD_assy_ratio_cut) {continue;}

        if (F4A_gen_zvtx -> LB_Gaus_Width_width < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l || F4A_gen_zvtx -> LB_Gaus_Width_width > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r) {continue;}
        if (F4A_gen_zvtx -> LB_cut_peak_width <   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l || F4A_gen_zvtx -> LB_cut_peak_width >   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r) {continue;} 
        
        if (private_gen_zvtx -> LB_Gaus_Width_width < ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_l || private_gen_zvtx -> LB_Gaus_Width_width > ana_map_version::INTT_zvtx_recohist_gaus_fit_width_cut_r) {continue;}
        if (private_gen_zvtx -> LB_cut_peak_width <   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_l || private_gen_zvtx -> LB_cut_peak_width >   ana_map_version::INTT_zvtx_recohist_cutgroup_width_cut_r) {continue;} 

        // note : the INTTF4A_gen_zvtx, MBDF4A_gen_zvtx diff cut
        // if ( (F4A_gen_zvtx -> LB_Gaus_Mean_mean - F4A_gen_zvtx -> MBD_reco_z) < ana_map_version::INTTz_MBDz_diff_cut_l || (F4A_gen_zvtx -> LB_Gaus_Mean_mean - F4A_gen_zvtx -> MBD_reco_z) > ana_map_version::INTTz_MBDz_diff_cut_r ) {continue;}


        if (private_gen_zvtx -> bco_full  != F4A_gen_zvtx -> bco_full) {
            cout<<endl;
            cout<<"private_gen_eID_offset: "<<private_gen_eID_offset<<endl;
            cout<<"at the event : "<<event_i<<", the bco_full is not the same"<<endl;
            cout<<"private_gen_zvtx -> bco_full : "<<private_gen_zvtx -> bco_full<<endl;
            cout<<"F4A_gen_zvtx -> bco_full : "<<F4A_gen_zvtx -> bco_full<<endl;
            private_gen_eID_offset += 1;
            continue;
        }

        double private_z = private_gen_zvtx -> LB_Gaus_Mean_mean * unit_correction; 
        double F4A_z     = F4A_gen_zvtx -> LB_Gaus_Mean_mean * unit_correction;
        double F4A_MBD_z = F4A_gen_zvtx -> MBD_reco_z * unit_correction;
        double zvtx_diff_value = private_z - F4A_z;
        double nclus = F4A_gen_zvtx -> nclu_inner + F4A_gen_zvtx -> nclu_outer;

        zvtx_diff -> Fill(zvtx_diff_value);
        zvtx_correlation -> Fill(private_z, F4A_z);
        zvtx_diff_nclus -> Fill(nclus, zvtx_diff_value);
        zvtx_diff_privateZ -> Fill(private_z, zvtx_diff_value);
        F4AMBDz_privateINTTz_diff_nclus -> Fill(nclus, private_z - F4A_MBD_z);
        F4AMBDz_privateINTTz_diff_F4AMBDz -> Fill(F4A_MBD_z, private_z - F4A_MBD_z);
    }

    SetsPhenixStyle();

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.03);

    // double legend_text_size = 0.03;
    // double legend_upper_y = 0.9;
    // TLegend * legend = new TLegend(
    //     0.6, // note : x1
    //     legend_upper_y - (input_file_full_info.size()) * (legend_text_size + 0.005), // note : y1
    //     0.8, // note : x2
    //     legend_upper_y // note : y2
    // );
    // // legend -> SetMargin(0);
    // legend->SetTextSize(legend_text_size);

    TCanvas * c1 = new TCanvas("c1","",950,800);

    TLine * coord_line = new TLine();
    coord_line -> SetLineWidth(1);
    coord_line -> SetLineColor(16);
    coord_line -> SetLineStyle(2);

    TF1 * gaus_fit = new TF1("gaus_fit", gaus_func, zvtx_diff->GetXaxis()->GetXmin(), zvtx_diff->GetXaxis()->GetXmax(),4);
    gaus_fit -> SetLineColor(2);
    gaus_fit -> SetLineWidth(2);
    gaus_fit -> SetNpx(1000);

    TF1 * linear_fit = new TF1("linear_fit","pol1", -500, 500);
    linear_fit -> SetLineColor(2);
    linear_fit -> SetLineWidth(1);
    linear_fit -> SetLineStyle(9);

    c1 -> cd();
    gaus_fit -> SetParameters(zvtx_diff -> GetBinContent( zvtx_diff -> GetMaximumBin() ), zvtx_diff -> GetBinCenter( zvtx_diff -> GetMaximumBin() ), zvtx_diff -> GetStdDev() * 0.6, 0);
    gaus_fit -> SetParLimits(0,0,100000);  // note : size 
    gaus_fit -> SetParLimits(2,0,10000);   // note : Width
    gaus_fit -> SetParLimits(3,0,10000);   // note : offset
    zvtx_diff -> Fit(gaus_fit, "N", "", zvtx_diff -> GetBinCenter( zvtx_diff -> GetMaximumBin() ) - (0.4 * zvtx_diff -> GetStdDev() ), zvtx_diff -> GetBinCenter( zvtx_diff -> GetMaximumBin() ) + (0.4 * zvtx_diff -> GetStdDev() ) );
    gaus_fit -> SetRange( gaus_fit->GetParameter(1) - gaus_fit->GetParameter(2) * 0.5, gaus_fit->GetParameter(1) + gaus_fit->GetParameter(2) * 0.5 ); 
    
    zvtx_diff -> Draw("hist");
    gaus_fit -> Draw("lsame");
    draw_text -> DrawLatex(0.21, 0.83+0.04, Form("Entries: %.0f", zvtx_diff -> GetEntries()));
    draw_text -> DrawLatex(0.21, 0.79+0.04, Form("Mean: %.3f %s", zvtx_diff -> GetMean(), unit_text.c_str() ));
    draw_text -> DrawLatex(0.21, 0.75+0.04, Form("Width: %.3f %s", zvtx_diff -> GetStdDev(), unit_text.c_str() ));
    draw_text -> DrawLatex(0.21, 0.75, Form("Gaus mean: %.3f %s",gaus_fit -> GetParameter(1), unit_text.c_str() ));
    draw_text -> DrawLatex(0.21, 0.69, Form("NClus > %i", Nclus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));

    c1 -> Print(Form("%s/comp_w_private_zvtx_diff.pdf", output_folder.c_str()));
    c1 -> Clear();


    c1 -> cd();
    zvtx_correlation -> Draw("colz0"); 
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    zvtx_correlation -> Fit(linear_fit, "NQ");
    linear_fit -> Draw("l same");
    draw_text -> DrawLatex(0.21, 0.83, Form("NClus > %i", Nclus_cut));
    draw_text -> DrawLatex(0.21, 0.87, Form("Y = %.2fx + %.2f", linear_fit -> GetParameter(1), linear_fit -> GetParameter(0)));
    c1 -> Print(Form("%s/comp_w_private_zvtx_correlation.pdf",output_folder.c_str()));
    c1 -> Clear();

    c1 -> cd();
    zvtx_diff_nclus -> Draw("colz0"); 
    draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", Nclus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    c1 -> Print(Form("%s/comp_w_private_zvtx_diff_nclus.pdf",output_folder.c_str()));
    c1 -> Clear();

    c1 -> cd();
    zvtx_diff_privateZ -> Draw("colz0"); 
    draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", Nclus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    c1 -> Print(Form("%s/comp_w_private_zvtx_diff_privateZ.pdf",output_folder.c_str()));
    c1 -> Clear();

    c1 -> cd();
    F4AMBDz_privateINTTz_diff_nclus -> Draw("colz0"); 
    draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", Nclus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    c1 -> Print(Form("%s/comp_w_private_F4AMBDz_privateINTTz_diff_nclus.pdf",output_folder.c_str()));
    c1 -> Clear();

    c1 -> cd();
    F4AMBDz_privateINTTz_diff_F4AMBDz -> Draw("colz0"); 
    draw_text -> DrawLatex(0.21, 0.87, Form("NClus > %i", Nclus_cut));
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} Work-in-progress"));
    c1 -> Print(Form("%s/comp_w_private_F4AMBDz_privateINTTz_diff_F4AMBDz.pdf",output_folder.c_str()));
    c1 -> Clear();


}