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

void draw_reweighted_spectra(TH1D* truth, TH1D* reweight, bool jet, std::pair<double, double> x_range, std::string syst_tag, const char* output_name = nullptr)
{
    TCanvas* canvas = new TCanvas("canvas", "", 600, 800);

    TPad* pad1 = new TPad("pad1", "", 0, 0.5, 1, 1.0);
    pad1->SetBottomMargin(0.02);
    pad1->Draw();
    pad1->cd();
    if (jet) pad1->SetLogy(1);
    else pad1->SetLogx(1);

    vector<double> bins;
    vector<double> bin_size;
    for (int i = 1; i <= truth->GetNbinsX() + 1; i++) {
        bins.push_back(truth->GetBinLowEdge(i));
    }
    for (int i = 1; i < bins.size(); i++) {
        bin_size.push_back(bins[i] - bins[i-1]);
    }

    for (int i = 1; i < truth->GetNbinsX() + 1; i++) {
        truth->SetBinContent(i,truth->GetBinContent(i)/bin_size[i-1]);
        truth->SetBinError(i,truth->GetBinError(i)/bin_size[i-1]);
    }

    bins.clear();
    bin_size.clear();
    for (int i = 1; i <= reweight->GetNbinsX() + 1; i++) {
        bins.push_back(reweight->GetBinLowEdge(i));
    }
    for (int i = 1; i < bins.size(); i++) {
        bin_size.push_back(bins[i] - bins[i-1]);
    }

    for (int i = 1; i < reweight->GetNbinsX() + 1; i++) {
        reweight->SetBinContent(i,reweight->GetBinContent(i)/bin_size[i-1]);
        reweight->SetBinError(i,reweight->GetBinError(i)/bin_size[i-1]);
    }

    truth->SetStats(0);
    reweight->SetStats(0);
    truth->SetLineColor(1);
    truth->SetMarkerColor(1);
    reweight->SetLineColor(4);
    reweight->SetMarkerColor(4);
    truth->GetXaxis()->SetLabelSize(0);
    reweight->GetXaxis()->SetLabelSize(0);
    truth->GetXaxis()->SetRangeUser(x_range.first, x_range.second);

    truth->Draw();
    reweight->Draw("same");

    TLegend* leg = new TLegend(0.67, 0.65, 0.9, 0.9);
    leg->AddEntry("","#it{#bf{sPHENIX}} Internal","");
    leg->AddEntry("",syst_tag.c_str(),"");
    leg->AddEntry(truth, "Pythia8 Truth", "lp");
    leg->AddEntry(reweight, "Reweighted Truth", "lp");
    leg->SetTextSize(0.04);
    leg->Draw();

    canvas->cd();
    TPad* pad2 = new TPad("pad2", "", 0, 0.0, 1, 0.5);
    pad2->SetTopMargin(0.02);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    pad2->cd();
    if (!jet) pad2->SetLogx(1);

    TH1D* r0 = (TH1D*)reweight->Clone("ratio");
    r0->Divide(truth);
    r0->GetYaxis()->SetTitle("Reweight/Truth Ratio");
    r0->GetYaxis()->SetNdivisions(510);
    r0->GetYaxis()->SetRangeUser(0, 2);
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

void plot_reweight() {
    gROOT->LoadMacro("/sphenix/u/egm2153/spring_2023/sPhenixStyle.C");
    gROOT->ProcessLine("SetsPhenixStyle()");

    std::vector<std::string> syst = {"calib_dijet","calib_dijet_jesdown","calib_dijet_jesup","calib_dijet_jerdown","calib_dijet_jerup"};
    std::vector<std::string> syst_tags = {"Nominal", "JES Down", "JES Up", "JER Down", "JER Up"};
    std::vector<std::string> trim = {"_trim_10","_reweight_trim_10"};

    TFile* f = TFile::Open("output_unfolded_data_iter_3_1000toys.root");

    std::vector<std::vector<TH2D*>> h_truth_2D(5, std::vector<TH2D*>(2, nullptr));
    std::vector<std::vector<TH1D*>> hj_truth(5, std::vector<TH1D*>(2, nullptr));
    std::vector<std::vector<TH1D*>> hc_truth(5, std::vector<TH1D*>(2, nullptr));
    
    for (int i = 0; i < syst.size(); i++) {
        for (int j = 0; j < 2; j++) {
            h_truth_2D[i][j] = (TH2D*)f->Get(("h_truth_"+syst[i]+trim[j]).c_str());
            hj_truth[i][j] = h_truth_2D[i][j]->ProjectionX(("hj_truth_"+syst[i]+trim[j]).c_str());
            hc_truth[i][j] = h_truth_2D[i][j]->ProjectionY(("hc_truth_"+syst[i]+trim[j]).c_str());
        }
    }
    
    for (int i = 0; i < syst.size(); i++) {
            string jet_outfile = "run21_figure/h_reweighted_truth_jet_spectrum_" + syst[i] + "_iter_3.png";
            string et_outfile = "run21_figure/h_reweighted_truth_et_spectrum_" + syst[i] + "_iter_3.png";
            draw_reweighted_spectra(hj_truth[i][0], hj_truth[i][1], true, std::make_pair(17.0, 82.0), syst_tags[i], jet_outfile.c_str());
            draw_reweighted_spectra(hc_truth[i][0], hc_truth[i][1], false, std::make_pair(0.1, 35), syst_tags[i], et_outfile.c_str());
    }
    


}
