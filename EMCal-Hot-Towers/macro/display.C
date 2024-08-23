// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TDirectory.h>
#include <TKey.h>
#include <TFile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>

// -- sPHENIX Style
#include "sPhenixStyle.C"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::min;
using std::max;
using std::ofstream;

namespace myAnalysis {
    void plots(const string& i_input, const string &output);

    UInt_t ntowers = 24576;
    UInt_t threshold;
    Float_t zMax = 1000;
}

void myAnalysis::plots(const string& i_input, const string &output) {
    TFile input(i_input.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.13);
    c1->SetTopMargin(.08);
    c1->SetRightMargin(.05);

    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1);

    auto hBadTowers    = (TH1F*)input.Get("hBadTowers");
    auto hBadTowersDead  = (TH1F*)input.Get("hBadTowersDead");
    auto hBadTowersHot  = (TH1F*)input.Get("hBadTowersHot");
    auto hBadTowersCold  = (TH1F*)input.Get("hBadTowersCold");

    auto h2BadTowers   = (TH2F*)input.Get("h2BadTowers");
    auto h2BadTowersDead = (TH2F*)input.Get("h2BadTowersDead");
    auto h2BadTowersHot = (TH2F*)input.Get("h2BadTowersHot");
    auto h2BadTowersCold = (TH2F*)input.Get("h2BadTowersCold");

    auto hSigma = (TH1F*)input.Get("hSigma");
    auto hSigmaFreqHot = (TH1F*)input.Get("hSigmaFreqHot");

    auto hHotTowerStatus = (TH1F*)input.Get("hHotTowerStatus");

    auto h2HotTowerFrequency_dummy = (TH2F*)input.Get("h2HotTowerFrequency/h2HotTowerFrequency_27_7");

    threshold = h2HotTowerFrequency_dummy->GetEntries()/2;
    cout << "threshold: " << threshold << endl;


    vector<TH1F*> hBadTowersVec  = {hBadTowers, hBadTowersDead, hBadTowersHot, hBadTowersCold};
    vector<TH2F*> h2BadTowersVec = {h2BadTowers, h2BadTowersDead, h2BadTowersHot, h2BadTowersCold};
    // vector<string> label         = {"Status #neq 0", "Dead", "Hot", "Cold"};

    string dirName = "hHotTowerSigma";
    TDirectory* dir = (TDirectory*) input.Get(dirName.c_str());
    if (!dir)
    {
      cerr << "Directory not found: " << dirName << endl;
      input.Close();
      return;
    }

    TIter nextkey(dir->GetListOfKeys());
    TKey* key;

    string outputSigma = "sigma.pdf";
    stringstream name;

    c1->Print((outputSigma + "[").c_str(), "pdf portrait");
    c1->cd();

    gPad->SetLogy();

    hSigma->Rebin(5);

    auto sigma_threshold = new TLine(5, 0, 5, hSigma->GetMaximum()*1.05);
    sigma_threshold->SetLineColor(kOrange);
    sigma_threshold->SetLineWidth(3);

    hSigma->SetTitle("Towers");
    hSigma->SetLineColor(kBlue);
    hSigma->Draw();
    sigma_threshold->Draw("same");
    c1->Print((string(hSigma->GetName()) + ".png").c_str());

    hSigmaFreqHot->Rebin(5);

    hSigmaFreqHot->SetLineColor(kRed);
    hSigmaFreqHot->Draw("same");

    auto leg = new TLegend(0.5,.75,0.7,.9);
    leg->SetFillStyle(0);
    leg->AddEntry(hSigma,"Flagged Hot #leq 50% of Runs","f");
    leg->AddEntry(hSigmaFreqHot,"Flagged Hot > 50% of Runs","f");
    leg->AddEntry(sigma_threshold,"Sigma Threshold","l");
    leg->Draw("same");

    c1->Print((string(hSigma->GetName()) + ".png").c_str());
    c1->Print(outputSigma.c_str(), "pdf portrait");

    gPad->SetLogy(0);

    while ((key = (TKey*) nextkey())) {
        name.str("");
        name << dirName << "/" << key->GetName();

        auto h = (TH1F*)input.Get(name.str().c_str());
        h->Rebin(5);

        sigma_threshold = new TLine(5, 0, 5, h->GetMaximum()*1.05);
        sigma_threshold->SetLineColor(kRed);
        sigma_threshold->SetLineWidth(3);
        h->Draw();
        sigma_threshold->Draw("same");

        c1->Print((string(h->GetName()) + "-sigma.png").c_str());
        c1->Print(outputSigma.c_str(), "pdf portrait");
    }

    c1->Print((outputSigma + "]").c_str(), "pdf portrait");

    c1->Print((output + "[").c_str(), "pdf portrait");

    TLine* line = new TLine(0, threshold, ntowers, threshold);
    line->SetLineColor(kRed);
    line->SetLineStyle(9);
    line->SetLineWidth(3);

    TLatex latex;
    latex.SetTextSize(0.05);

    for(UInt_t i = 0; i < hBadTowersVec.size(); ++i) {

        hBadTowersVec[i]->Draw();
        line->Draw("same");

        c1->Print((string(hBadTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print(output.c_str(), "pdf portrait");
    }

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetGrid();

    for(UInt_t i = 0; i < h2BadTowersVec.size(); ++i) {
        h2BadTowersVec[i]->SetMaximum(zMax);

        h2BadTowersVec[i]->GetXaxis()->SetLimits(0,256);
        h2BadTowersVec[i]->GetXaxis()->SetNdivisions(32, false);
        h2BadTowersVec[i]->GetXaxis()->SetLabelSize(0.04);
        h2BadTowersVec[i]->GetXaxis()->SetTickSize(0.01);
        h2BadTowersVec[i]->GetYaxis()->SetTickSize(0.01);
        h2BadTowersVec[i]->GetYaxis()->SetLabelSize(0.04);
        h2BadTowersVec[i]->GetYaxis()->SetLimits(0,96);
        h2BadTowersVec[i]->GetYaxis()->SetNdivisions(12, false);
        h2BadTowersVec[i]->GetYaxis()->SetTitleOffset(0.5);

        h2BadTowersVec[i]->Draw("colz1");

        c1->Print((string(h2BadTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print((output).c_str(), "pdf portrait");
    }

    h2BadTowers->SetMinimum(threshold);
    h2BadTowers->Draw("colz1");

    c1->Print((string(h2BadTowers->GetName()) + "-threshold.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2BadTowersHot->SetMinimum(threshold);
    h2BadTowersHot->Draw("colz1");

    c1->Print((string(h2BadTowersHot->GetName()) + "-threshold.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->SetRightMargin(.05);
    gPad->SetLogy();

    hHotTowerStatus->GetYaxis()->SetTitleOffset(0.9);
    hHotTowerStatus->Draw();

    c1->Print((string(hHotTowerStatus->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->Print((output + "]").c_str(), "pdf portrait");

    UInt_t ctr[4] = {0};

    for(UInt_t i = 1; i <= ntowers; ++i) {
        UInt_t towerIndex = i-1;
        if(hBadTowers->GetBinContent(i) > 0)             ++ctr[0];
        if(hBadTowers->GetBinContent(i) >= threshold)    ++ctr[1];
        if(hBadTowersHot->GetBinContent(i) > 0)          ++ctr[2];
        if(hBadTowersHot->GetBinContent(i) >= threshold) ++ctr[3];
    }

    cout << "Bad Towers" << endl
         << "All: "                              << ctr[0] << ", " << ctr[0]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[1] << ", " << ctr[1]*100./ntowers << " %" << endl << endl;

    cout << "Hot Towers" << endl
         << "All: "                              << ctr[2] << ", " << ctr[2]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": " << ctr[3] << ", " << ctr[3]*100./ntowers << " %" << endl;

    input.Close();
}

void display(const string &input, const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    myAnalysis::plots(input, output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./display input [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output  = "plots.pdf";

    if(argc >= 3) {
        output = argv[2];
    }

    display(argv[1], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
