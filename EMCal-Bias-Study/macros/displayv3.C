// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TH2F.h>
#include <TF1.h>
#include <TFile.h>
#include <TDirectory.h>
#include <TList.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TPaletteAxis.h>

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
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::map;
namespace fs = std::filesystem;

namespace myAnalysis {
    void analyze(const string &output);

    unordered_map<string,TH1*> m_hists;
}

void myAnalysis::analyze(const string &output) {
    cout << "###############" << endl;
    cout << "Analysis" << endl;
    string outputDir = fs::absolute(output).parent_path().string();
    string outputStem = fs::absolute(output).filename().stem().string();
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    unordered_map<string,string> tf = {{"default","/gpfs02/sphenix/user/anarde/EMCal-Bias-Study/calo-v8/output/test-59279.root"}
                                     , {"r0","/gpfs02/sphenix/user/anarde/EMCal-Bias-Study/calo-v8/output/test-59280.root"}
                                     , {"r1","/gpfs02/sphenix/user/anarde/EMCal-Bias-Study/calo-v8/output/test-59281.root"}
                                     , {"r2","/gpfs02/sphenix/user/anarde/EMCal-Bias-Study/calo-v8/output/test-59282.root"}
                                      };

    TH1::AddDirectory(kFALSE);

    string histName = "hCEMC";

    for (const auto &[name, fileName] : tf) {
        TFile* tfile = TFile::Open(fileName.c_str());
        if (!tfile || tfile->IsZombie()) {
            cout << "Error: Could not open ROOT file: " << fileName << endl;
            return;
        }

        string hName = histName + "-" + name;

        m_hists[hName] = (TH1*)tfile->Get(histName.c_str());
        if (!m_hists[hName]) {
            cout << "Error: Histogram '" << histName << "' not found in file: " << tfile->GetName() << endl;
            return;
        }

        tfile->Close();
    }

    TCanvas* c1 = new TCanvas("c1");
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(3000, 2000);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    gPad->SetGrid(0,1);

    c1->Print((output + "[").c_str(), "pdf portrait");

    Int_t nRebin = 82;
    Int_t lineWidth = 4;

    m_hists["hCEMC-default"]->Rebin(nRebin);
    m_hists["hCEMC-r0"]->Rebin(nRebin);
    m_hists["hCEMC-r1"]->Rebin(nRebin);
    m_hists["hCEMC-r2"]->Rebin(nRebin);

    m_hists["hCEMC-r0"]->Divide(m_hists["hCEMC-default"]);
    m_hists["hCEMC-r1"]->Divide(m_hists["hCEMC-default"]);
    m_hists["hCEMC-r2"]->Divide(m_hists["hCEMC-default"]);

    m_hists["hCEMC-r1"]->GetYaxis()->SetTitle("Ratio");
    m_hists["hCEMC-r1"]->GetXaxis()->SetTitleOffset(1);
    m_hists["hCEMC-r1"]->GetYaxis()->SetTitleOffset(1);
    m_hists["hCEMC-r1"]->GetYaxis()->SetRangeUser(0,1.6);

    m_hists["hCEMC-r1"]->SetLineColor(kRed);
    m_hists["hCEMC-r2"]->SetLineColor(kBlue);

    m_hists["hCEMC-r0"]->SetLineWidth(lineWidth);
    m_hists["hCEMC-r1"]->SetLineWidth(lineWidth);
    m_hists["hCEMC-r2"]->SetLineWidth(lineWidth);

    Double_t xshift = 0.25;
    Double_t yshift = -0.02;
    auto leg = new TLegend(0.5+xshift,.7+yshift,0.7+xshift,.9+yshift);
    leg->SetFillStyle(0);
    leg->AddEntry(m_hists["hCEMC-r0"],"r0/default","f");
    leg->AddEntry(m_hists["hCEMC-r1"],"r1/default","f");
    leg->AddEntry(m_hists["hCEMC-r2"],"r2/default","f");

    m_hists["hCEMC-r1"]->Draw();
    m_hists["hCEMC-r0"]->Draw("same");
    m_hists["hCEMC-r2"]->Draw("same");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/hCEMC-ratio.png").c_str());

    m_hists["hCEMC-r1"]->Divide(m_hists["hCEMC-r0"]);
    m_hists["hCEMC-r2"]->Divide(m_hists["hCEMC-r0"]);

    delete leg;
    yshift = 0.02;
    leg = new TLegend(0.5+xshift,.7+yshift,0.7+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->AddEntry(m_hists["hCEMC-r1"],"r1/r0","f");
    leg->AddEntry(m_hists["hCEMC-r2"],"r2/r0","f");

    m_hists["hCEMC-r1"]->Draw();
    m_hists["hCEMC-r2"]->Draw("same");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/hCEMC-r0-ratio.png").c_str());

    c1->Print((output+ "]").c_str(), "pdf portrait");
}

void displayv3(const string &output="plots.pdf") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "output: " << output << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // if(myAnalysis::readFile(input)) return ;
    myAnalysis::analyze(output);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc > 2){
        cout << "usage: ./displayv3 [output]" << endl;
        cout << "output: output pdf file" << endl;
        return 1;
    }

    string output = "plots.pdf";

    if(argc >= 2) {
        output = argv[1];
    }

    displayv3(output);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
