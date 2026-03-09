#include <iostream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLatex.h>

#include "sPhenixStyle.h"

void TH2F_division(TH2F * hist_MC, TH2F * hist_reco, TH2F * hist_out)
{
    for (int i = 1; i <= hist_MC -> GetNbinsX(); i++)
    {
        for (int j = 1; j <= hist_MC -> GetNbinsY(); j++)
        {
            if (hist_reco -> GetBinContent(i,j) != 0 && hist_MC -> GetBinContent(i,j) != 0)
            {
                hist_out -> SetBinContent(i,j, hist_reco -> GetBinContent(i,j)/hist_MC -> GetBinContent(i,j));
            }
        }
    }

    return;
}

int main()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/zvtx_cut_scan_2/complete_file";
    string ouput_directory = input_directory + "/plot_selected_trials";
    double required_zvtx_diff = 4.0; // note : mm
    int N_trial_x = 30;
    vector<pair<int,int>> selected_trial = {
        {4,7}
    };

    TFile * file_in = nullptr;
    TTree * tree = nullptr;
    int eID_in;
    int in_nclu_inner;
    int in_nclu_outer;
    bool good_zvtx_tag;
    int Centrality_bin;
    double reco_zvtx;
    double MC_true_zvtx;

    int index_x;
    int index_y;
    double deltaphi_cut;
    double dca_cut;
    double zvtx_dist_mean;
    double zvtx_dist_width;


    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    TLatex * draw_text = new TLatex();
    draw_text -> SetNDC();
    draw_text -> SetTextSize(0.025);

    map<int,int> centrality_map = {
        {5, 0},
        {15, 1},
        {25, 2},
        {35, 3},
        {45, 4},
        {55, 5},
        {65, 6},
        {75, 7},
        {85, 8},
        {95, 9}
    };

    vector<string> centrality_region = {
        "0-5%",
        "5-15%",
        "15-25%",
        "25-35%",
        "35-45%",
        "45-55%",
        "55-65%",
        "65-75%",
        "75-85%",
        "85-95%",
        "0-95%"
    };

    vector<double> z_region = { // todo: if the z region is changed, the following vector should be updated
        -420, // note unit : mm
        -380,
        -340,
        -300,
        -260,
        -220, // note : this part is the peak region
        -180, // note : this part is the peak region
        -140,
        -100,
        -60,
        -20,
        20
    };

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    // note : required zvtx tag
    TH2F * true_zvtx_Mbin_Truezpos = new TH2F("","true_zvtx_Mbin_Truezpos;MBin;True zvtx [mm]", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    TH2F * reco_zvtx_Mbin_Truezpos = new TH2F("","reco_zvtx_Mbin_Truezpos;MBin;True zvtx [mm]", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 

    // note : all the zvtx, doesn't require good_zvtx_tag
    TH2F * true_zvtx_Mbin_Truezpos_inclusive = new TH2F("","true_zvtx_Mbin_Truezpos_inclusive;MBin;True zvtx [mm]", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    
    TH2F * ratio_Mbin_Truezpos = new TH2F("","ratio_Mbin_Truezpos;MBin;True zvtx [mm]",                     centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    TH2F * ratio_Mbin_Truezpos_inclusive = new TH2F("","ratio_Mbin_Truezpos_inclusive;MBin;True zvtx [mm]", centrality_region.size(), 0, centrality_region.size(), z_region.size() - 1, &z_region[0]); 
    

    for (int i = 0; i < selected_trial.size(); i++)
    {
        cout<<endl;
        cout<<"checking file: "<<i<<endl;

        file_in = TFile::Open(Form("%s/zvtx_dist_info_%i_%i_%i.root", input_directory.c_str(), selected_trial[i].second * N_trial_x + selected_trial[i].first, selected_trial[i].first, selected_trial[i].second));
        tree = (TTree*)file_in -> Get("tree");
        tree -> Scan("*");
        tree -> SetBranchAddress("index_x", &index_x);         
        tree -> SetBranchAddress("index_y", &index_y);         
        tree -> SetBranchAddress("deltaphi_cut", &deltaphi_cut);    
        tree -> SetBranchAddress("dca_cut", &dca_cut);         
        tree -> SetBranchAddress("zvtx_dist_mean", &zvtx_dist_mean);  
        tree -> SetBranchAddress("zvtx_dist_width", &zvtx_dist_width); 
        tree -> GetEntry(0);
        file_in -> Close();

        file_in = TFile::Open(Form("%s/INTT_zvtx_%i_%i_%i.root", input_directory.c_str(), selected_trial[i].second * N_trial_x + selected_trial[i].first, selected_trial[i].first, selected_trial[i].second));
        tree = (TTree*)file_in -> Get("tree_Z");
        tree -> SetBranchStatus("*", 0);
        tree -> SetBranchStatus("nclu_inner",1);
        tree -> SetBranchStatus("nclu_outer",1);
        tree -> SetBranchStatus("LB_Gaus_Mean_mean", 1);
        tree -> SetBranchStatus("good_zvtx_tag", 1);
        tree -> SetBranchStatus("Centrality_bin",1);
        tree -> SetBranchStatus("MC_true_zvtx",1);
        tree -> SetBranchStatus("eID",1);

        tree -> SetBranchAddress("nclu_inner", &in_nclu_inner);
        tree -> SetBranchAddress("nclu_outer", &in_nclu_outer);
        tree -> SetBranchAddress("LB_Gaus_Mean_mean", &reco_zvtx);
        tree -> SetBranchAddress("good_zvtx_tag", &good_zvtx_tag);
        tree -> SetBranchAddress("Centrality_bin", &Centrality_bin);
        tree -> SetBranchAddress("MC_true_zvtx", &MC_true_zvtx);
        tree -> SetBranchAddress("eID", &eID_in);

        for (int event_i = 0; event_i < tree -> GetEntries(); event_i++)
        {
            tree -> GetEntry(event_i);
            
            true_zvtx_Mbin_Truezpos_inclusive -> Fill(centrality_map[Centrality_bin], MC_true_zvtx);
            true_zvtx_Mbin_Truezpos_inclusive -> Fill(centrality_map.size() - 1, MC_true_zvtx);

            if (good_zvtx_tag == 1 && in_nclu_inner > 0 && in_nclu_outer > 0)
            {
                true_zvtx_Mbin_Truezpos -> Fill(centrality_map[Centrality_bin], MC_true_zvtx);
                true_zvtx_Mbin_Truezpos -> Fill(centrality_map.size() - 1, MC_true_zvtx);
                if (fabs(reco_zvtx - MC_true_zvtx) < required_zvtx_diff)
                {
                    reco_zvtx_Mbin_Truezpos -> Fill(centrality_map[Centrality_bin], MC_true_zvtx);
                    reco_zvtx_Mbin_Truezpos -> Fill(centrality_map.size() - 1, MC_true_zvtx);
                }
            }
            // else 
            // {
            //     if (event_i < 2000 && centrality_map[Centrality_bin] == 0) {cout<<"event_i: "<<event_i<<" true zvtx : "<<MC_true_zvtx<<endl;}
            // }
        }

        // TH2F_division(true_zvtx_Mbin_Truezpos,           reco_zvtx_Mbin_Truezpos, ratio_Mbin_Truezpos);
        // TH2F_division(true_zvtx_Mbin_Truezpos_inclusive, reco_zvtx_Mbin_Truezpos, ratio_Mbin_Truezpos_inclusive);

        ratio_Mbin_Truezpos =           (TH2F *) reco_zvtx_Mbin_Truezpos -> Clone("ratio_Mbin_Truezpos");
        ratio_Mbin_Truezpos_inclusive = (TH2F *) reco_zvtx_Mbin_Truezpos -> Clone("ratio_Mbin_Truezpos_inclusive");
        ratio_Mbin_Truezpos -> Divide(ratio_Mbin_Truezpos, true_zvtx_Mbin_Truezpos, 1, 1, "B");
        ratio_Mbin_Truezpos_inclusive -> Divide(ratio_Mbin_Truezpos_inclusive, true_zvtx_Mbin_Truezpos_inclusive, 1, 1, "B");

        gStyle->SetPaintTextFormat("1.0f");

        c1 -> cd();
        true_zvtx_Mbin_Truezpos -> SetMarkerSize(1.0);
        true_zvtx_Mbin_Truezpos -> Draw("colz0");
        true_zvtx_Mbin_Truezpos -> Draw("HIST TEXT45 SAME");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
        draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("#Delta#Phi: %.3f deg., DCA: %.3f mm", deltaphi_cut, dca_cut));
        c1 -> Print(Form("%s/true_zvtx_Mbin_Truezpos.pdf", ouput_directory.c_str()));
        c1 -> Clear();

        c1 -> cd();
        reco_zvtx_Mbin_Truezpos -> SetMarkerSize(1.0);
        reco_zvtx_Mbin_Truezpos -> Draw("colz0");
        reco_zvtx_Mbin_Truezpos -> Draw("HIST TEXT45 SAME");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
        draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("#Delta#Phi: %.3f deg., DCA: %.3f mm", deltaphi_cut, dca_cut));
        c1 -> Print(Form("%s/reco_zvtx_Mbin_Truezpos.pdf", ouput_directory.c_str()));
        c1 -> Clear();

        gStyle->SetPaintTextFormat("1.3f");

        c1 -> cd();
        ratio_Mbin_Truezpos -> SetMarkerSize(0.7);
        ratio_Mbin_Truezpos -> Draw("colz0");
        ratio_Mbin_Truezpos -> Draw("HIST TEXT45E SAME");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
        draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("#Delta#Phi: %.3f deg., DCA: %.3f mm", deltaphi_cut, dca_cut));
        c1 -> Print(Form("%s/ratio_Mbin_Truezpos.pdf", ouput_directory.c_str()));
        c1 -> Clear();

        c1 -> cd();
        ratio_Mbin_Truezpos_inclusive -> SetMarkerSize(0.7);
        ratio_Mbin_Truezpos_inclusive -> Draw("colz0");
        ratio_Mbin_Truezpos_inclusive -> Draw("HIST TEXT45E SAME");
        ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
        draw_text->DrawLatex(0.16, 1 - gPad->GetTopMargin() + 0.01, Form("#Delta#Phi: %.3f deg., DCA: %.3f mm", deltaphi_cut, dca_cut));
        c1 -> Print(Form("%s/ratio_Mbin_Truezpos_inclusive.pdf", ouput_directory.c_str()));
        c1 -> Clear();


        true_zvtx_Mbin_Truezpos -> Reset("ICESM");
        reco_zvtx_Mbin_Truezpos -> Reset("ICESM");
        true_zvtx_Mbin_Truezpos_inclusive -> Reset("ICESM");
        ratio_Mbin_Truezpos -> Reset("ICESM");
        ratio_Mbin_Truezpos_inclusive -> Reset("ICESM");
        file_in -> Close();
    }

    return 0;
}