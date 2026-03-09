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
#include <iterator>

void draw_profile(TH1D* truth, std::vector<TH1D*> unfold, bool jet, std::vector<std::string> leg_tags, int max_iter, const char* output_name = nullptr)
{
    TCanvas* canvas = new TCanvas("canvas", "", 600, 800);

    //std::vector<std::string> colors = {"kBlack","kRed","kBlue","kGreen+2","kBlue-3","kMagneta+1"};

    TPad* pad1 = new TPad("pad1", "", 0, 0.5, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();

    truth->SetStats(0);
    truth->SetLineColor(max_iter + 2);
    truth->SetMarkerColor(max_iter + 2);
    truth->GetXaxis()->SetLabelSize(0);

    for (int i = 0; i < max_iter; ++i) {
        unfold[i]->SetStats(0);
        if (i >= 4) {
            unfold[i]->SetLineColor(i + 2); // Use TColor::GetColor if needed
            unfold[i]->SetMarkerColor(i + 2);
        } else {
            unfold[i]->SetLineColor(i + 1); // Use TColor::GetColor if needed
            unfold[i]->SetMarkerColor(i + 1);
        }
        unfold[i]->GetXaxis()->SetLabelSize(0);
        unfold[i]->GetXaxis()->SetRangeUser(20, 63);
        unfold[i]->GetYaxis()->SetRangeUser(0,10);
    }


    unfold[0]->GetYaxis()->SetTitle("<#SigmaE_{T}> [GeV]");
    unfold[0]->GetXaxis()->SetTitle("p_{T,lead} [GeV]");
    unfold[0]->Draw();
    for (int i = 1; i < max_iter; ++i) {
        unfold[i]->Draw("same");
    }
    truth->Draw("same");

    TLegend* leg = new TLegend(0.77, 0.65, 0.9, 0.9);
    for (int i = 0; i < max_iter; ++i) {
        leg->AddEntry(unfold[i], leg_tags[i].c_str(), "lp");
    }
    leg->AddEntry(truth, "Pythia8", "lp");
    leg->SetTextSize(0.04);
    leg->Draw();

    canvas->cd();
    TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1, 0.5);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    pad2->cd();
    
    std::vector<TH1D*> ratios;
    TH1D* r0 = (TH1D*)truth->Clone("truth_ratio");
    r0->Divide(unfold[0]);
    for (int i = 1; i < r0->GetNbinsX() + 1; i++) {
        r0->SetBinError(i,0);
    }
    for (int i = 1; i < max_iter; ++i) {
        TH1D* ratio = (TH1D*)unfold[i]->Clone(Form("ratio%d", i));
        ratio->Divide(unfold[0]);
        for (int j = 1; j < ratio->GetNbinsX() + 1; j++) {
            ratio->SetBinError(j,0);
        }
        ratios.push_back(ratio);
    }

    //TH1D* r0 = ratios[0];
    r0->GetYaxis()->SetTitle("Var/Nominal Ratio");
    r0->GetYaxis()->SetNdivisions(510);
    r0->GetYaxis()->SetRangeUser(0, 2);
    r0->GetYaxis()->SetTitleSize(25);
    r0->GetYaxis()->SetTitleFont(43);
    r0->GetYaxis()->SetTitleOffset(1.5);
    r0->GetYaxis()->SetLabelFont(43);
    r0->GetYaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetTitle("p_{T} [GeV]");
    r0->GetXaxis()->SetTitleSize(25);
    r0->GetXaxis()->SetTitleFont(43);
    r0->GetXaxis()->SetTitleOffset(0);
    r0->GetXaxis()->SetLabelFont(43);
    r0->GetXaxis()->SetLabelSize(25);
    r0->GetXaxis()->SetRangeUser(17, 63);

    r0->Draw("hist");
    for (int i = 1; i < ratios.size(); ++i) {
        ratios[i]->Draw("hist, same");
    }

    TLine* line0 = new TLine(17, 1.0, 63, 1.0);
    TLine* line1 = new TLine(17, 0.95, 63, 0.95);
    TLine* line2 = new TLine(17, 1.05, 63, 1.05);
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

void plot_result() {
    gROOT->LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
    gROOT->ProcessLine("SetsPhenixStyle()");

    // want nominal and JES and JER systematics for reweight trim 10 and iteration 4
    // 
    std::vector<std::string> syst = {"calib_dijet_reweight_trim_5","calib_dijet_jesdown_reweight_trim_5","calib_dijet_jesup_reweight_trim_5","calib_dijet_jerdown_reweight_trim_5","calib_dijet_jerup_reweight_trim_5","calib_dijet_trim_5"};
    std::vector<std::string> syst_tags = {"Nominal","JES Down", "JES Up", "JER Down", "JER Up", "No reweight"};
    //std::string trim = "";

    //TFile* f = TFile::Open("output_unfolded_data_run21_iter_3_1000toys.root");
    TFile* f = TFile::Open("output_unfolded_data_run21_iter_3_notoys.root");

    TH2D* h_truth_2D;
    TProfile* h_truth_prof;
    TH1D* h_truth;
    std::vector<TH2D*> h_unfold_2D;
    std::vector<TProfile*> h_unfold_prof;
    std::vector<TH1D*> h_unfold;

    h_truth_2D = (TH2D*)f->Get("h_truth_calib_dijet");
    h_truth_prof = h_truth_2D->ProfileX("truth_prof_calib_dijet",1,h_truth_2D->GetNbinsY()-1);
    int tnbins = h_truth_prof->GetNbinsX();
    const TAxis* txaxis = h_truth_prof->GetXaxis();
    std::vector<double> tedges(tnbins + 1);
    for (int b = 0; b <= tnbins; ++b) { tedges[b] = txaxis->GetBinLowEdge(b + 1); }
    tedges[tnbins] = txaxis->GetBinUpEdge(tnbins);
    h_truth = new TH1D("truth_hist_calib_dijet", "", tnbins, tedges.data());
    for (int b = 1; b <= tnbins; ++b) {
        h_truth->SetBinContent(b, h_truth_prof->GetBinContent(b));
        h_truth->SetBinError(b, h_truth_prof->GetBinError(b)); }
    for (int i = 0; i < syst.size(); i++) {
        h_unfold_2D.push_back((TH2D*)f->Get(("h_unfold_"+syst[i]+"_8").c_str()));
        h_unfold_prof.push_back(h_unfold_2D[i]->ProfileX(("unfold_prof_"+syst[i]+"_8").c_str(),1,h_truth_2D->GetNbinsY()-1));
        int nbins = h_unfold_prof[i]->GetNbinsX();
        const TAxis* xaxis = h_unfold_prof[i]->GetXaxis();
        std::vector<double> edges(nbins + 1);
        for (int b = 0; b <= nbins; ++b) { edges[b] = xaxis->GetBinLowEdge(b + 1); }
        edges[nbins] = xaxis->GetBinUpEdge(nbins);
        TH1D* hist = new TH1D(("unfold_hist_"+syst[i]+"_8").c_str(), "", nbins, edges.data());
        for (int b = 1; b <= nbins; ++b) {
            hist->SetBinContent(b, h_unfold_prof[i]->GetBinContent(b));
            if (i == 0) { hist->SetBinError(b, h_unfold_prof[i]->GetBinError(b)); }
            else { hist->SetBinError(b, 0.0); }
        }
        h_unfold.push_back(hist);
    }

    draw_profile(h_truth, h_unfold, true, syst_tags, (int)syst.size(), "run21_figure/h_profile_notoys_w_syst_w_pythia.png");

}
