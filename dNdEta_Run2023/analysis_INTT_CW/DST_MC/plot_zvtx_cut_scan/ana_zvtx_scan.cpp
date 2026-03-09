#include <iostream>
using namespace std;

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TCanvas.h>
#include "sPhenixStyle.h"

int main()
{
    string input_directory = "/sphenix/user/ChengWei/sPH_dNdeta/HIJING_ana398_xvtx-0p04cm_yvtx0p24cm_zvtx-20cm_dummyAlignParams/zvtx_cut_scan_2/complete_file";
    string ouput_directory = input_directory + "/merged_result";

    SetsPhenixStyle();
    TCanvas * c1 = new TCanvas("c1","c1",950,800);
    c1 -> cd();

    // double scan_trial_x = 50;
    // double scan_step_x = 0.01;
    // double scan_trial_y = 50;
    // double scan_step_y = 0.015;

    // TH2F * diff_mean_2D   = new TH2F("","diff_mean;#Delta#Phi cut [degree];DCA cut [mm];Dist. Mean",   int(scan_trial_x), scan_step_x/2., scan_step_x*scan_trial_x + scan_step_x/2., int(scan_trial_y), scan_step_y/2., scan_step_y*scan_trial_y + scan_step_y/2.);
    // diff_mean_2D -> GetXaxis() -> SetNdivisions(505);
    // TH2F * diff_stddev_2D = new TH2F("","diff_mean;#Delta#Phi cut [degree];DCA cut [mm];Dist. StdDev", int(scan_trial_x), scan_step_x/2., scan_step_x*scan_trial_x + scan_step_x/2., int(scan_trial_y), scan_step_y/2., scan_step_y*scan_trial_y + scan_step_y/2.);
    // diff_stddev_2D -> GetXaxis() -> SetNdivisions(505);

    int total_trial = 900;

    double scan_start_x = 0.01;
    double scan_step_x = 0.15;
    double scan_trial_x = 30;
    
    double scan_start_y = 0.015;
    double scan_step_y = 0.15;
    double scan_trial_y = 30;

    TH2F * diff_mean_2D   = new TH2F("","diff_mean;#Delta#Phi cut [degree];DCA cut [mm];Dist. Mean",   int(scan_trial_x), scan_start_x - scan_step_x/2., scan_step_x*(scan_trial_x - 1.) + scan_start_x + scan_step_x/2., int(scan_trial_y), scan_start_y - scan_step_y/2., scan_step_y*(scan_trial_y - 1.) + scan_start_y + scan_step_y/2.);
    diff_mean_2D -> GetXaxis() -> SetNdivisions(505);
    TH2F * diff_stddev_2D = new TH2F("","diff_mean;#Delta#Phi cut [degree];DCA cut [mm];Dist. StdDev", int(scan_trial_x), scan_start_x - scan_step_x/2., scan_step_x*(scan_trial_x - 1.) + scan_start_x + scan_step_x/2., int(scan_trial_y), scan_start_y - scan_step_y/2., scan_step_y*(scan_trial_y - 1.) + scan_start_y + scan_step_y/2.);
    diff_stddev_2D -> GetXaxis() -> SetNdivisions(505);

    TFile * file_in = nullptr;

    double in_dist_mean;
    double in_dist_width;
    double in_delta_phi_cut;
    double in_dca_cut; 

    TLatex * ltx = new TLatex();
    ltx->SetNDC();
    ltx->SetTextSize(0.045);
    ltx->SetTextAlign(31);

    for (int i = 0; i < total_trial; i++)
    {
        cout<<endl;
        cout<<"checking file: "<<i<<endl;
        file_in = TFile::Open(Form("%s/zvtx_dist_info_%i_%i_%i.root", input_directory.c_str(), i, i%int(scan_trial_x), i/int(scan_trial_y)));
        
        TTree * tree = (TTree*)file_in -> Get("tree");
        tree -> SetBranchAddress("deltaphi_cut", &in_delta_phi_cut);
        tree -> SetBranchAddress("dca_cut", &in_dca_cut);
        tree -> SetBranchAddress("zvtx_dist_mean", &in_dist_mean);
        tree -> SetBranchAddress("zvtx_dist_width", &in_dist_width);

        tree -> GetEntry(0);

        cout<<in_delta_phi_cut<<" "<<in_dca_cut<<" "<<in_dist_mean<<" "<<in_dist_width<<endl;

        diff_mean_2D   -> SetBinContent(diff_mean_2D   -> FindBin(in_delta_phi_cut, in_dca_cut), in_dist_mean);
        diff_stddev_2D -> SetBinContent(diff_stddev_2D -> FindBin(in_delta_phi_cut, in_dca_cut), in_dist_width);

        file_in -> Close();
    }

    gStyle->SetPaintTextFormat("1.3f");

    c1 -> cd();
    diff_mean_2D->SetMarkerSize(0.3);
    diff_mean_2D -> Draw("colz0");
    diff_mean_2D -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
    c1 -> Print(Form("%s/diff_mean_2D.pdf", ouput_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    diff_stddev_2D->SetMarkerSize(0.3);
    diff_stddev_2D -> Draw("colz0");
    diff_stddev_2D -> Draw("HIST TEXT45 SAME");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
    c1 -> Print(Form("%s/diff_stddev_2D.pdf", ouput_directory.c_str()));
    c1 -> Clear();

    gPad->SetTheta(45); // default is 30
    gPad->SetPhi(240); // default is 30
    gPad->Update();

    c1 -> cd();
    diff_mean_2D -> Draw("LEGO2Z");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
    c1 -> Print(Form("%s/diff_mean_2D_LEGO.pdf", ouput_directory.c_str()));
    c1 -> Clear();

    c1 -> cd();
    diff_stddev_2D -> Draw("LEGO2Z");
    ltx->DrawLatex(1 - gPad->GetRightMargin(), 1 - gPad->GetTopMargin() + 0.01, Form("#it{#bf{sPHENIX}} simulation"));
    c1 -> Print(Form("%s/diff_stddev_2D_LEGO.pdf", ouput_directory.c_str()));
    c1 -> Clear();

}