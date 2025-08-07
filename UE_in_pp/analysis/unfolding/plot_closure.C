#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TLine.h>
#include <TColor.h>
#include <TROOT.h>
#include <vector>
#include <string>
#include <sstream>

void draw_unfolded_spectra_one_unfold(TH1D* truth, TH1D* measure, TH1D* unfold, bool jet, std::string leg_tags, int max_iter, std::pair<double, double> x_range, const char* output_name = nullptr)
{
    TCanvas* canvas = new TCanvas("canvas", "", 600, 800);

    TPad* pad1 = new TPad("pad1", "", 0, 0.5, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();
    if (jet) pad1->SetLogy(1);
    else pad1->SetLogx(1);

    truth->SetStats(0);
    measure->SetStats(0);
    truth->SetLineColor(2);
    truth->SetMarkerColor(2);
    measure->SetLineColor(1);
    measure->SetMarkerColor(1);
    truth->GetXaxis()->SetLabelSize(0);
    measure->GetXaxis()->SetLabelSize(0);
    truth->GetXaxis()->SetRangeUser(x_range.first, x_range.second);
    unfold->SetStats(0);
    unfold->SetLineColor(3); // Use TColor::GetColor if needed
    unfold->SetMarkerColor(3);
    unfold->GetXaxis()->SetLabelSize(0);

    truth->Draw();
    measure->Draw("same");
    unfold->Draw("same");

    TLegend* leg = new TLegend(0.77, 0.65, 0.9, 0.9);
    leg->AddEntry(truth, "Truth", "lp");
    leg->AddEntry(measure, "Measured", "lp");
    leg->AddEntry(unfold, leg_tags.c_str(), "lp");
    leg->SetTextSize(0.04);
    leg->Draw();

    canvas->cd();
    TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1, 0.5);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    pad2->cd();
    if (!jet) pad2->SetLogx(1);

    TH1D* ratio = (TH1D*)unfold->Clone("ratio");
    ratio->Divide(truth);
    ratio->GetYaxis()->SetTitle("Unfolded/Truth Ratio");
    ratio->GetYaxis()->SetNdivisions(510);
    ratio->GetYaxis()->SetRangeUser(0.5, 1.5);
    ratio->GetYaxis()->SetTitleSize(25);
    ratio->GetYaxis()->SetTitleFont(43);
    ratio->GetYaxis()->SetTitleOffset(1.5);
    ratio->GetYaxis()->SetLabelFont(43);
    ratio->GetYaxis()->SetLabelSize(25);
    ratio->GetXaxis()->SetTitle(jet ? "p_{T} [GeV]" : "#SigmaE_{T} [GeV]");
    ratio->GetXaxis()->SetTitleSize(25);
    ratio->GetXaxis()->SetTitleFont(43);
    ratio->GetXaxis()->SetTitleOffset(0);
    ratio->GetXaxis()->SetLabelFont(43);
    ratio->GetXaxis()->SetLabelSize(25);
    ratio->GetXaxis()->SetRangeUser(x_range.first, x_range.second);

    ratio->Draw("ep");

    TLine* line0 = new TLine(x_range.first, 1.0, x_range.second, 1.0);
    TLine* line1 = new TLine(x_range.first, 0.95, x_range.second, 0.95);
    TLine* line2 = new TLine(x_range.first, 1.05, x_range.second, 1.05);
    line0->SetLineStyle(1);
    line1->SetLineStyle(2);
    line2->SetLineStyle(2);
    line0->Draw("same");
    line1->Draw("same");
    line2->Draw("same");

    canvas->Update();
    canvas->Draw();
    if (output_name) canvas->SaveAs(output_name);
}

void draw_unfolded_spectra(TH1D* truth, TH1D* measure, std::vector<TH1D*> unfold, bool jet, std::vector<std::string> leg_tags, int max_iter, std::pair<double, double> x_range, const char* output_name = nullptr)
{
    TCanvas* canvas = new TCanvas("canvas", "", 600, 800);

    TPad* pad1 = new TPad("pad1", "", 0, 0.5, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();
    if (jet) pad1->SetLogy(1);
    else pad1->SetLogx(1);

    truth->SetStats(0);
    measure->SetStats(0);
    truth->SetLineColor(2);
    truth->SetMarkerColor(2);
    measure->SetLineColor(1);
    measure->SetMarkerColor(1);
    truth->GetXaxis()->SetLabelSize(0);
    measure->GetXaxis()->SetLabelSize(0);
    truth->GetXaxis()->SetRangeUser(x_range.first, x_range.second);

    for (int i = 0; i < max_iter; ++i) {
        unfold[i]->SetStats(0);
        unfold[i]->SetLineColor(i + 3); // Use TColor::GetColor if needed
        unfold[i]->SetMarkerColor(i + 3);
        unfold[i]->GetXaxis()->SetLabelSize(0);
    }

    truth->Draw();
    measure->Draw("same");
    for (int i = 0; i < max_iter; ++i) {
        unfold[i]->Draw("same");
    }

    TLegend* leg = new TLegend(0.77, 0.65, 0.9, 0.9);
    leg->AddEntry(truth, "Truth", "lp");
    leg->AddEntry(measure, "Measured", "lp");
    for (int i = 0; i < max_iter; ++i) {
        leg->AddEntry(unfold[i], leg_tags[i].c_str(), "lp");
    }
    leg->SetTextSize(0.04);
    leg->Draw();

    canvas->cd();
    TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1, 0.5);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    pad2->cd();
    if (!jet) pad2->SetLogx(1);

    std::vector<TH1D*> ratios;
    for (int i = 0; i < max_iter; ++i) {
        TH1D* ratio = (TH1D*)unfold[i]->Clone(Form("ratio%d", i));
        ratio->Divide(truth);
        ratios.push_back(ratio);
    }

    TH1D* r0 = ratios[0];
    r0->GetYaxis()->SetTitle("Unfolded/Truth Ratio");
    r0->GetYaxis()->SetNdivisions(510);
    r0->GetYaxis()->SetRangeUser(0.5, 1.5);
    r0->GetYaxis()->SetTitleSize(25);
    r0->GetYaxis()->SetTitleFont(43);
    r0->GetYaxis()->SetTitleOffset(1.5);
    r0->GetYaxis()->SetLabelFont(43);
    r0->GetYaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetTitle(jet ? "p_{T} [GeV]" : "#SigmaE_{T} [GeV]");
    r0->GetXaxis()->SetTitleSize(25);
    r0->GetXaxis()->SetTitleFont(43);
    r0->GetXaxis()->SetTitleOffset(0);
    r0->GetXaxis()->SetLabelFont(43);
    r0->GetXaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetRangeUser(x_range.first, x_range.second);

    r0->Draw("ep");
    for (int i = 1; i < max_iter; ++i) {
        ratios[i]->Draw("ep same");
    }

    TLine* line0 = new TLine(x_range.first, 1.0, x_range.second, 1.0);
    TLine* line1 = new TLine(x_range.first, 0.95, x_range.second, 0.95);
    TLine* line2 = new TLine(x_range.first, 1.05, x_range.second, 1.05);
    line0->SetLineStyle(1);
    line1->SetLineStyle(2);
    line2->SetLineStyle(2);
    line0->Draw("same");
    line1->Draw("same");
    line2->Draw("same");

    canvas->Update();
    canvas->Draw();
    if (output_name) canvas->SaveAs(output_name);
}

void plot_closure() {
    gROOT->LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
    gROOT->ProcessLine("SetsPhenixStyle()");

    std::vector<std::string> full_leg_tags = { "Unfold, iter 1"};
    std::vector<std::string> half_leg_tags = { "Unfold, iter 1", "Unfold, iter 2", "Unfold, iter 3", "Unfold, iter 4", "Unfold, iter 5", "Unfold, iter 6", 
    "Unfold, iter 7", "Unfold, iter 8", "Unfold, iter 9", "Unfold, iter 10"};
    std::vector<std::string> syst = {"calib_dijet","calib_dijet_jesdown","calib_dijet_jesup","calib_dijet_jerdown","calib_dijet_jerup","calib_dijet_half1","calib_dijet_half2"};;
    std::vector<std::string> trim = {"","_trim_5","_trim_10","_reweight","_reweight_trim_5","_reweight_trim_10"};

    TFile* f = TFile::Open("output_closure_sim_run21_iter_3_1000toys.root");

    /*
    TH2D* h_truth_2D[7][6];
    TH2D* h_measure_2D[7][6];
    TH2D* h_full_unfold_2D[7][6];
    TH1D* hj_truth[7][6];
    TH1D* hc_truth[7][6];
    TH1D* hj_measure[7][6];
    TH1D* hc_measure[7][6];
    TH1D* hj_full_unfold[7][6];
    TH1D* hc_full_unfold[7][6];

    TH2D* h_half_unfold_2D[6][10];
    TH1D* hj_half_unfold[6][10];
    TH1D* hc_half_unfold[6][10];
    */

    std::vector<std::vector<TH2D*>> h_truth_2D(7, std::vector<TH2D*>(6, nullptr));
    std::vector<std::vector<TH2D*>> h_measure_2D(7, std::vector<TH2D*>(6, nullptr));
    std::vector<std::vector<TH2D*>> h_full_unfold_2D(7, std::vector<TH2D*>(6, nullptr));

    std::vector<std::vector<TH1D*>> hj_truth(7, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hc_truth(7, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hj_measure(7, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hc_measure(7, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hj_full_unfold(7, std::vector<TH1D*>(6, nullptr));
    std::vector<std::vector<TH1D*>> hc_full_unfold(7, std::vector<TH1D*>(6, nullptr));

    std::vector<std::vector<TH2D*>> h_half_unfold_2D(6, std::vector<TH2D*>(10, nullptr));
    std::vector<std::vector<TH1D*>> hj_half_unfold(6, std::vector<TH1D*>(10, nullptr));
    std::vector<std::vector<TH1D*>> hc_half_unfold(6, std::vector<TH1D*>(10, nullptr));


    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            h_truth_2D[i][j] = (TH2D*)f->Get(("h_truth_"+syst[i]+trim[j]).c_str());
            h_measure_2D[i][j] = (TH2D*)f->Get(("h_measure_"+syst[i]+trim[j]).c_str());
            h_full_unfold_2D[i][j] = (TH2D*)f->Get(("h_unfold_full_"+syst[i]+trim[j]+"_1").c_str());
            hj_truth[i][j] = h_truth_2D[i][j]->ProjectionX(("hj_truth_"+syst[i]+trim[j]).c_str());
            hc_truth[i][j] = h_truth_2D[i][j]->ProjectionY(("hc_truth_"+syst[i]+trim[j]).c_str());
            hj_measure[i][j] = h_measure_2D[i][j]->ProjectionX(("hj_measure_"+syst[i]+trim[j]).c_str());
            hc_measure[i][j] = h_measure_2D[i][j]->ProjectionY(("hc_measure_"+syst[i]+trim[j]).c_str());
            hj_full_unfold[i][j] = h_full_unfold_2D[i][j]->ProjectionX(("hj_full_unfold_"+syst[i]+trim[j]).c_str());
            hc_full_unfold[i][j] = h_full_unfold_2D[i][j]->ProjectionY(("hc_full_unfold_"+syst[i]+trim[j]).c_str());
        }
    }

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 10; j++) {
            h_half_unfold_2D[i][j] = (TH2D*)f->Get(("h_unfold_half"+trim[i]+"_"+to_string(j+1)).c_str());
            hj_half_unfold[i][j] = h_half_unfold_2D[i][j]->ProjectionX(("hj_half_unfold"+trim[i]+to_string(j)).c_str());
            hc_half_unfold[i][j] = h_half_unfold_2D[i][j]->ProjectionY(("hc_half_unfold"+trim[i]+to_string(j)).c_str());
        }
    }
    
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 6; j++) {
            string jet_outfile = "run21_figure/h_jet_spectrum_" + syst[i] + trim[j] + "_full_closure_iter_3_1000toys.png";
            string et_outfile = "run21_figure/h_et_spectrum_" + syst[i] + trim[j] + "_full_closure_iter_3_1000toys.png";
            draw_unfolded_spectra_one_unfold(hj_truth[i][j], hj_measure[i][j], hj_full_unfold[i][j], true, full_leg_tags[0], 1, std::make_pair(14.0, 82.0), jet_outfile.c_str());
            draw_unfolded_spectra_one_unfold(hc_truth[i][j], hc_measure[i][j], hc_full_unfold[i][j], false, full_leg_tags[0], 1, std::make_pair(0.1,35), et_outfile.c_str());
        }
    }
    
    for (int i = 0; i < 6; i++) {
        string jet_outfile = "run21_figure/h_jet_spectrum" + trim[i] + "_half_closure_iter_3_1000toys.png";
        string et_outfile = "run21_figure/h_et_spectrum" + trim[i] + "_half_closure_iter_3_1000toys.png";
        draw_unfolded_spectra(hj_truth[5][i], hj_measure[6][i], hj_half_unfold[i], true, half_leg_tags, 6, std::make_pair(14.0, 82.0), jet_outfile.c_str());
        draw_unfolded_spectra(hc_truth[5][i], hc_measure[6][i], hc_half_unfold[i], false, half_leg_tags, 6, std::make_pair(0.1, 35), et_outfile.c_str());
    }

    f->Close();
}
