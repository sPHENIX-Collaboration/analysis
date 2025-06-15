// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TFile.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TProfile.h>
#include <TPaveStats.h>

// -- sPHENIX Style
#include <sPhenixStyle.C>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::to_string;
using std::vector;
using std::stringstream;
using std::ofstream;
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::map;
using std::unordered_set;
namespace fs = std::filesystem;

namespace myAnalysis {
    void make_plots(const string &outputDir);
    Int_t readHists();
    map<string,TH1*> m_hists;

    Bool_t m_saveFig = true;
}

Int_t myAnalysis::readHists() {
    // Read Hist from input
    string input_2024 = "/gpfs02/sphenix/user/anarde/sPHENIX/analysis-EMCal-Calibration/EMCal-Calibration-Study/output/test-2024.root";
    string input_2025 = "/gpfs02/sphenix/user/anarde/sPHENIX/analysis-EMCal-Calibration/EMCal-Calibration-Study/output/test-2025.root";

    string input = input_2024;

    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists["h_InvMass_2024"] = static_cast<TH1*>(tfile->Get("h_InvMass")->Clone("h_InvMass_2024"));
    m_hists["h_event_2024"]   = static_cast<TH1*>(tfile->Get("h_event")->Clone("h_event_2024"));

    tfile->Close();

    input = input_2025;

    tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists["h_InvMass_2025"] = static_cast<TH1*>(tfile->Get("h_InvMass")->Clone("h_InvMass_2025"));
    m_hists["h_event_2025"]   = static_cast<TH1*>(tfile->Get("h_event")->Clone("h_event_2025"));

    tfile->Close();

    return 0;
}

void myAnalysis::make_plots(const string &outputDir) {
    string output = outputDir + "/plots.pdf";

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13f);
    c1->SetRightMargin(.03f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08f);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    c1->Print((output + "[").c_str(), "pdf portrait");

    m_hists["h_InvMass_2024"]->Draw();
    m_hists["h_InvMass_2024"]->SetTitle("Invariant Mass: 2024 Au+Au, Run: 54912; M_{#gamma#gamma} [GeV]; Counts");
    m_hists["h_InvMass_2024"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2024"]->GetXaxis()->SetRangeUser(0,1);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2024.png").c_str());

    // ----------------------------------------

    m_hists["h_InvMass_2025"]->Draw();
    m_hists["h_InvMass_2025"]->SetTitle("Invariant Mass: 2025 Au+Au; M_{#gamma#gamma} [GeV]; Counts");
    m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2025"]->GetXaxis()->SetRangeUser(0,1);

    stringstream info;

    info << "Runs: 66580, 66592, 66593, 66623, 66624";

    TLatex latex;
    latex.SetTextSize(0.05f);
    latex.DrawLatex(0.2,2e4,info.str().c_str());

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2025.png").c_str());

    // ----------------------------------------

    m_hists["h_InvMass_2024"]->Scale(1./m_hists["h_InvMass_2024"]->Integral(1, m_hists["h_InvMass_2024"]->FindBin(1)-1));
    m_hists["h_InvMass_2025"]->Scale(1./m_hists["h_InvMass_2025"]->Integral(1, m_hists["h_InvMass_2025"]->FindBin(1)-1));

    m_hists["h_InvMass_2025"]->Draw("hist");
    m_hists["h_InvMass_2025"]->SetTitle("Invariant Mass; M_{#gamma#gamma} [GeV]; Normalized Counts");
    m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2025"]->GetYaxis()->SetTitleOffset(1.4f);
    m_hists["h_InvMass_2025"]->GetXaxis()->SetRangeUser(0,1);
    m_hists["h_InvMass_2025"]->GetYaxis()->SetRangeUser(0,9e-3);
    m_hists["h_InvMass_2025"]->SetLineColor(kRed);

    m_hists["h_InvMass_2024"]->Draw("same hist");

    m_hists["h_InvMass_2024"]->SetLineColor(kBlue);

    stringstream legA, legB;

    legA << "Run 2024 Au+Au";
    legB << "Run 2025 Au+Au";

    Double_t xshift = 0.3;
    Double_t yshift = -0.4;

    auto leg = new TLegend(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.06f);
    leg->AddEntry(m_hists["h_InvMass_2024"],legA.str().c_str(),"l");
    leg->AddEntry(m_hists["h_InvMass_2025"],legB.str().c_str(),"l");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_overlay.png").c_str());

    // ----------------------------------------

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void display(const string &outputDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "save figs: " << myAnalysis::m_saveFig << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // create output directories
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    TH1::AddDirectory(kFALSE);

    if(myAnalysis::readHists()) return;

    myAnalysis::make_plots(outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc > 2){
        cout << "usage: ./display [outputDir]" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 2) {
        outputDir = argv[1];
    }

    display(outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
