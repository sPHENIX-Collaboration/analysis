// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>
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
#include <TChain.h>
#include <TMath.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TProfile.h>
#include <TPaveStats.h>

// -- sPHENIX Style
#include <sPhenixStyle.C>

// tower mapping
#include <calobase/TowerInfoDefs.h>

// utils
#include "myUtils.C"

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
using std::unique_ptr;
namespace fs = std::filesystem;

namespace myAnalysis {
    void make_plots(const string &outputDir);
    Int_t readHists();
    Int_t readCalib(const string &input);
    void initHists();
    map<string,TH1*> m_hists;

    Int_t m_bins_calib = 200;
    Double_t m_calib_low  = 0;
    Double_t m_calib_high = 10;

    Int_t m_iter = 0;

    Bool_t m_saveFig = true;
}

void myAnalysis::initHists() {
    // dummy hists for labeling
    m_hists["h2DummySector"] = new TH2F("h2DummySector","", myUtils::m_nsector/2, 0, myUtils::m_nsector/2, 2, 0, 2);
    m_hists["h2DummyIB"] = new TH2F("h2DummyIB","", myUtils::m_nsector/2, 0, myUtils::m_nsector/2, myUtils::m_nib*2/myUtils::m_nsector, 0, myUtils::m_nib*2/myUtils::m_nsector);

    for(Int_t i = 0; i < myUtils::m_nsector; ++i) {
        Int_t x = i % (myUtils::m_nsector / 2) + 1;
        Int_t y = (i < (myUtils::m_nsector / 2)) ? 2 : 1;
        static_cast<TH2*>(m_hists["h2DummySector"])->SetBinContent(x,y,i);
    }

    for(Int_t i = 0; i < myUtils::m_nib; ++i) {
        Int_t val = i % myUtils::m_nib_per_sector;
        Int_t x = (i / myUtils::m_nib_per_sector) % (myUtils::m_nsector / 2) + 1;
        Int_t y = (i < (myUtils::m_nib / 2)) ? myUtils::m_nib_per_sector - val : myUtils::m_nib_per_sector + val + 1;
        static_cast<TH2*>(m_hists["h2DummyIB"])->SetBinContent(x,y,val);
    }
}

Int_t myAnalysis::readCalib(const string &input) {
    // string input = "output/EMCAL_ADC_to_Etower_2025_initial_v3.root";
    string ttree = "Multiple";
    unique_ptr<TChain> chain = myUtils::setupTChain(input, ttree);
    if(!chain) {
       cout << "ERROR TChain Setup Failed: " << input << ", TTree: " << ttree << endl;
       return 1;
    }

    chain->Add(input.c_str());

    Float_t calib;
    Int_t key;

    chain->SetBranchAddress("IID", &key);
    chain->SetBranchAddress("FCEMC_calib_ADC_to_ETower", &calib);

    stringstream h2Name, h2Title;
    stringstream hName, hTitle;

    h2Name << "h2Calib_" << m_iter;
    h2Title << "EMCal Calibration [MeV/ADC], Iter: " << m_iter << "; Tower Index #phi; Tower Index #eta";

    hName << "hCalib_" << m_iter;
    hTitle << "EMCal Calibration, Iter: " << m_iter << "; Calibration [MeV/ADC]; Counts";

    m_hists[h2Name.str()] = new TH2F(h2Name.str().c_str(),h2Title.str().c_str(), myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    m_hists[hName.str()] = new TH1F(hName.str().c_str(), hTitle.str().c_str(), m_bins_calib, m_calib_low, m_calib_high);

    Int_t min_phi = 9999;
    Int_t max_phi = 0;

    Int_t min_eta = 9999;
    Int_t max_eta = 0;

    Float_t min_calib = 9999;
    Float_t max_calib = 0;

    for(UInt_t i = 0; i < chain->GetEntries(); ++i) {
        chain->GetEntry(i);

        Int_t iphi = static_cast<Int_t>(TowerInfoDefs::getCaloTowerPhiBin(static_cast<UInt_t>(key)));
        Int_t ieta = static_cast<Int_t>(TowerInfoDefs::getCaloTowerEtaBin(static_cast<UInt_t>(key)));

        min_phi = std::min(min_phi, iphi);
        max_phi = std::max(max_phi, iphi);

        min_eta = std::min(min_eta, ieta);
        max_eta = std::max(max_eta, ieta);

        // GeV -> MeV
        calib *= 1e3f;

        if(calib) {
            min_calib = std::min(min_calib, calib);
            max_calib = std::max(max_calib, calib);
            static_cast<TH2*>(m_hists[h2Name.str()])->SetBinContent(iphi+1, ieta+1, calib);
            m_hists[hName.str()]->Fill(calib);
        }
    }

    cout << "========================" << endl;
    cout << "Read Calib Stats - Iter: " << m_iter++ << endl;
    cout << "Calib [MeV/ADC] Min: " << min_calib << ", Max: " << max_calib << endl;
    cout << "Phi Min: " << min_phi << ", Max: " << max_phi << endl;
    cout << "Eta Min: " << min_eta << ", Max: " << max_eta << endl;
    cout << "========================" << endl;

    return 0;
}

Int_t myAnalysis::readHists() {
    // Read Hist from input
    string input_2024 = "output/test-2024.root";
    string input_2025 = "output/test-2025.root";

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

    c1->SetCanvasSize(1200, 1000);
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

    for (const auto &[name, hist] : m_hists) {
        if(name.starts_with("h2")) myUtils::setEMCalDim(hist);
    }

    c1->Print((output + "[").c_str(), "pdf portrait");

    m_hists["h_InvMass_2024"]->Draw();
    m_hists["h_InvMass_2024"]->SetTitle("Invariant Mass: 2024 Au+Au, Run: 54912; m_{#gamma#gamma} [GeV]; Counts");
    m_hists["h_InvMass_2024"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2024"]->GetXaxis()->SetRangeUser(0,1);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2024.png").c_str());

    // ----------------------------------------

    c1->SetLeftMargin(.17f);

    m_hists["h_InvMass_2025"]->Draw();
    m_hists["h_InvMass_2025"]->SetTitle("; m_{#gamma#gamma} [GeV]; Counts");
    m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2025"]->GetYaxis()->SetTitleOffset(1.7f);
    m_hists["h_InvMass_2025"]->GetXaxis()->SetRangeUser(0,0.5);

    stringstream info;

    info << "Au+Au 2025";

    TLatex latex;
    latex.SetTextSize(0.04f);
    latex.DrawLatexNDC(0.85,0.91,"6/10/25");
    latex.DrawLatexNDC(0.5,0.8,"#bf{#it{sPHENIX}} Preliminary");
    latex.DrawLatexNDC(0.5,0.8-0.05,"Au+Au 2025 #sqrt{s_{NN}} = 200 GeV");
    latex.DrawLatexNDC(0.5,0.8-0.05*2,"|z| < 20 cm, N_{Clusters} < 300");
    latex.DrawLatexNDC(0.5,0.8-0.05*3.3,"1.3 GeV < p_{T,1} < 4 GeV");
    latex.DrawLatexNDC(0.5,0.8-0.05*4.3,"0.7 GeV < p_{T,2} < 4 GeV");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2025.png").c_str());

    // ----------------------------------------

    c1->SetLeftMargin(.14f);

    m_hists["h_InvMass_2024"]->Scale(1./m_hists["h_InvMass_2024"]->Integral(1, m_hists["h_InvMass_2024"]->FindBin(1)-1));
    m_hists["h_InvMass_2025"]->Scale(1./m_hists["h_InvMass_2025"]->Integral(1, m_hists["h_InvMass_2025"]->FindBin(1)-1));

    m_hists["h_InvMass_2025"]->Draw("hist");
    m_hists["h_InvMass_2025"]->SetTitle("Invariant Mass; m_{#gamma#gamma} [GeV]; Normalized Counts");
    m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    m_hists["h_InvMass_2025"]->GetYaxis()->SetTitleOffset(1.5f);
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

    c1->SetLeftMargin(.1f);

    gPad->SetLogy();

    stringstream hName;

    for(Int_t i = 0; i < m_iter; ++i) {
        hName.str("");
        hName << "hCalib_" << i;
        m_hists[hName.str()]->Draw();
        m_hists[hName.str()]->GetXaxis()->SetTitleOffset(0.9f);
        m_hists[hName.str()]->GetYaxis()->SetTitleOffset(0.9f);

        c1->Print(output.c_str(), "pdf portrait");
        if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + ".png").c_str());
    }

    gPad->SetLogy(0);

    // ----------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gPad->SetGrid();

    for(Int_t i = 0; i < m_iter; ++i) {
        hName.str("");
        hName << "h2Calib_" << i;

        m_hists[hName.str()]->Draw("COLZ1");
        m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
        m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

        m_hists[hName.str()]->SetMinimum(1);
        m_hists[hName.str()]->SetMaximum(5);

        c1->Print(output.c_str(), "pdf portrait");
        if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + ".png").c_str());
    }

    for(Int_t i = 0; i < m_iter; ++i) {
        hName.str("");
        hName << "h2Calib_" << i;

        m_hists[hName.str()]->Draw("COLZ1");

        m_hists[hName.str()]->SetMinimum(1);
        m_hists[hName.str()]->SetMaximum(5);

        m_hists[hName.str()]->GetYaxis()->SetRangeUser(0,8);
        m_hists[hName.str()]->GetYaxis()->SetNdivisions(8, false);

        c1->Print(output.c_str(), "pdf portrait");
        if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + "-zoom-South-IB-5.png").c_str());
    }



    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2Calib.png").c_str());

    // m_hists["h2Calib"]->SetMaximum(5);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2Calib-zoom.png").c_str());

    // m_hists["h2Calib"]->Draw("COLZ1");
    // m_hists["h2Calib"]->GetYaxis()->SetRangeUser(0,8);
    // m_hists["h2Calib"]->GetYaxis()->SetNdivisions(8, false);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2Calib-zoom-South-IB-5.png").c_str());

    // ----------------------------------------

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void display(const string &input, const string &outputDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: " << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "save figs: " << myAnalysis::m_saveFig << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // create output directories
    fs::create_directories(outputDir);
    fs::create_directories(outputDir+"/images");

    TH1::AddDirectory(kFALSE);

    myAnalysis::initHists();

    if(myAnalysis::readHists()) return;
    if(!myUtils::readCSV(input, myAnalysis::readCalib, false)) return;

    myAnalysis::make_plots(outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./" << argv[0] << " input [outputDir]" << endl;
        cout << "input: list of calib files" << endl;
        cout << "outputDir: output directory" << endl;
        return 1;
    }

    string outputDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }

    display(argv[1], outputDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
