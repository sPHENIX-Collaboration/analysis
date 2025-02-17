// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>

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
#include <TDatime.h>
#include <TGaxis.h>

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
    void plots(const string& i_input, const string &outputDir);

    UInt_t threshold = 400; // nRuns above which tower is frequently bad
}

void myAnalysis::plots(const string& i_input, const string &outputDir) {

    string outputSigmaDir = outputDir + "/sigma/";

    TFile input(i_input.c_str());

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1500, 1000);
    c1->SetLeftMargin(.13);
    c1->SetTopMargin(.08);
    c1->SetRightMargin(.05);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);
    gStyle->SetTitleXOffset(1);
    gStyle->SetTitleYOffset(1);

    auto hSigma = (TH1F*)input.Get("hSigmaHot");
    auto hSigmaFreqHot = (TH1F*)input.Get("hSigmaFreqHot");
    auto hSigmaTypeA = (TH1F*)input.Get("hSigmaTypeA");
    auto hSigmaTypeB = (TH1F*)input.Get("hSigmaTypeB");
    auto hSigmaTypeC = (TH1F*)input.Get("hSigmaTypeC");

    // threshold = h2HotTowerFrequency_dummy->GetEntries()/2;
    cout << "threshold: " << threshold << endl;
    c1->cd();

    gPad->SetLogy();

    hSigma->Rebin(5);
    hSigmaTypeA->Rebin(5);
    hSigmaTypeB->Rebin(5);
    hSigmaTypeC->Rebin(5);

    auto sigma_threshold = new TLine(5, 0, 5, hSigma->GetMaximum()*1.05);
    sigma_threshold->SetLineColor(kOrange);
    sigma_threshold->SetLineWidth(3);

    hSigma->SetTitle("Towers");
    // hSigma->SetLineColor(kBlack);
    hSigma->GetXaxis()->SetRangeUser(0,30);
    hSigma->GetYaxis()->SetRangeUser(1,1e7);
    hSigma->Draw();
    sigma_threshold->Draw("same");

    hSigmaTypeA->SetLineColor(kRed);
    hSigmaTypeA->Draw("same");

    hSigmaTypeB->SetLineColor(kGreen+2);
    hSigmaTypeB->Draw("same");

    hSigmaTypeC->SetLineColor(kBlue);
    hSigmaTypeC->Draw("same");

    auto leg = new TLegend(0.5,.5,0.7,.9);
    leg->SetFillStyle(0);
    leg->AddEntry(hSigma,("Flagged Hot < " + to_string(threshold) + " Runs").c_str(),"f");
    leg->AddEntry(hSigmaTypeA,"Hot Type A","f");
    leg->AddEntry(hSigmaTypeB,"Hot Type B","f");
    leg->AddEntry(hSigmaTypeC,"Hot Type C","f");
    leg->AddEntry(sigma_threshold,"Sigma Threshold","l");
    leg->Draw("same");

    c1->Print((outputSigmaDir + string(hSigma->GetName()) + "-HotType.png").c_str());

    input.Close();
}

void displayHotType(const string &input, const string &outputDir=".") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // make the output directories
    std::filesystem::create_directories(outputDir + "/sigma");

    myAnalysis::plots(input, outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./displayHotType input [output]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }

    displayHotType(argv[1], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
