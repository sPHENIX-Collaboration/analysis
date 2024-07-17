// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
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

    UInt_t  ntowers   = 24576;
    Float_t threshold = 150;
}

void myAnalysis::plots(const string& i_input, const string &output) {
    TFile input(i_input.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.05);

    c1->Print((output + "[").c_str(), "pdf portrait");

    auto hHotTowers    = (TH1F*)input.Get("hHotTowers");
    auto hHotTowersS1  = (TH1F*)input.Get("hHotTowersS1");
    auto hHotTowersS2  = (TH1F*)input.Get("hHotTowersS2");
    auto hHotTowersS3  = (TH1F*)input.Get("hHotTowersS3");

    auto h2HotTowers   = (TH2F*)input.Get("h2HotTowers");
    auto h2HotTowersS1 = (TH2F*)input.Get("h2HotTowersS1");
    auto h2HotTowersS2 = (TH2F*)input.Get("h2HotTowersS2");
    auto h2HotTowersS3 = (TH2F*)input.Get("h2HotTowersS3");

    auto hHotTowersMasked  = (TH1F*)input.Get("hHotTowersMasked");
    auto h2HotTowersMasked = (TH2F*)input.Get("h2HotTowersMasked");

    auto hHotTowerStatus = (TH1F*)input.Get("hHotTowerStatus");

    vector<TH1F*> hHotTowersVec  = {hHotTowers, hHotTowersS1, hHotTowersS2, hHotTowersS3, hHotTowersMasked};
    vector<TH2F*> h2HotTowersVec = {h2HotTowers, h2HotTowersS1, h2HotTowersS2, h2HotTowersS3, h2HotTowersMasked};
    vector<string> label         = {"Status #neq 0", "Status = 1", "Status = 2", "Status = 3", ""};

    c1->cd();

    TLine* line = new TLine(0, threshold, ntowers, threshold);
    line->SetLineColor(kRed);
    line->SetLineStyle(9);
    line->SetLineWidth(3);

    TLatex latex;
    latex.SetTextSize(0.05);

    for(UInt_t i = 0; i < hHotTowersVec.size(); ++i) {
        hHotTowersVec[i]->Draw();
        line->Draw("same");
        latex.DrawLatexNDC(0.39,0.87, label[i].c_str());

        c1->Print((string(hHotTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print((output).c_str(), "pdf portrait");
    }

    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.12);

    for(UInt_t i = 0; i < h2HotTowersVec.size(); ++i) {
        h2HotTowersVec[i]->SetMaximum(500);
        h2HotTowersVec[i]->GetYaxis()->SetTitleOffset(0.9);
        h2HotTowersVec[i]->Draw("colz1");

        c1->Print((string(h2HotTowersVec[i]->GetName()) + ".png").c_str());
        c1->Print((output).c_str(), "pdf portrait");
    }

    h2HotTowers->SetMinimum(threshold);
    h2HotTowers->Draw("colz1");

    c1->Print((string(h2HotTowers->GetName()) + "-threshold.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    h2HotTowersMasked->SetMinimum(threshold);
    h2HotTowersMasked->Draw("colz1");

    c1->Print((string(h2HotTowersMasked->GetName()) + "-threshold.png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->SetRightMargin(.05);
    gPad->SetLogy();

    hHotTowerStatus->GetYaxis()->SetTitleOffset(0.9);
    hHotTowerStatus->Draw();

    c1->Print((string(hHotTowerStatus->GetName()) + ".png").c_str());
    c1->Print((output).c_str(), "pdf portrait");

    c1->Print((output + "]").c_str(), "pdf portrait");

    UInt_t ctr[3] = {0};
    for(UInt_t i = 1; i <= ntowers; ++i) {
        if(hHotTowers->GetBinContent(i) > 0)                ++ctr[0];
        if(hHotTowers->GetBinContent(i) >= threshold)       ++ctr[1];
        if(hHotTowersMasked->GetBinContent(i) >= threshold) ++ctr[2];
    }

    cout << "Bad Towers" << endl
         << "All: "                                            << ctr[0] << ", " << ctr[0]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << ": "               << ctr[1] << ", " << ctr[1]*100./ntowers << " %" << endl
         << "Threshold >= " << threshold << " AND Dead Mask: " << ctr[2] << ", " << ctr[2]*100./ntowers << " %" << endl;

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

    string output = "test.pdf";

    if(argc >= 3) {
        output = argv[2];
    }

    display(argv[1], output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
