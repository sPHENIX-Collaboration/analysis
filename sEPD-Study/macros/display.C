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
#include <TH3.h>
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
    Int_t readHists(const string &input);
    void initHists();
    map<string,std::unique_ptr<TH1>> m_hists;
    map<string,Int_t> m_ctr;

    // Int_t m_bins_calib = 200;
    // Double_t m_calib_low  = 0;
    // Double_t m_calib_high = 10;

    // Int_t m_iter = 0;

    Bool_t m_saveFig = false;
}

void myAnalysis::initHists() {
    // // dummy hists for labeling
    // // m_hists["h2DummySector"] = new TH2F("h2DummySector","", myUtils::m_nsector/2, 0, myUtils::m_nsector/2, 2, 0, 2);
    // // m_hists["h2DummyIB"] = new TH2F("h2DummyIB","", myUtils::m_nsector/2, 0, myUtils::m_nsector/2, myUtils::m_nib*2/myUtils::m_nsector, 0, myUtils::m_nib*2/myUtils::m_nsector);

    // for(Int_t i = 0; i < myUtils::m_nsector; ++i) {
    //     Int_t x = i % (myUtils::m_nsector / 2) + 1;
    //     Int_t y = (i < (myUtils::m_nsector / 2)) ? 2 : 1;
    //     static_cast<TH2*>(m_hists["h2DummySector"])->SetBinContent(x,y,i);
    // }

    // for(Int_t i = 0; i < myUtils::m_nib; ++i) {
    //     Int_t val = i % myUtils::m_nib_per_sector;
    //     Int_t x = (i / myUtils::m_nib_per_sector) % (myUtils::m_nsector / 2) + 1;
    //     Int_t y = (i < (myUtils::m_nib / 2)) ? myUtils::m_nib_per_sector - val : myUtils::m_nib_per_sector + val + 1;
    //     static_cast<TH2*>(m_hists["h2DummyIB"])->SetBinContent(x,y,val);
    // }
}

Int_t myAnalysis::readHists(const string &input) {

    fs::path p(input);
    string stem = p.stem().string();
    string run = myUtils::split(stem,'-')[1];

    cout << "Processing: " << run << endl;
    std::unique_ptr<TFile> tfile(TFile::Open(input.c_str()));
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }

    m_hists["hEvent_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hEvent")->Clone(("hEvent_"+run).c_str())));
    m_hists["hCentrality_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hCentrality")->Clone(("hCentrality_"+run).c_str())));
    m_hists["hVtxZ_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hVtxZ")->Clone(("hVtxZ_"+run).c_str())));
    m_hists["hVtxZ_MB_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("hVtxZ_MB")->Clone(("hVtxZ_MB_"+run).c_str())));

    m_hists["h3SEPD_Charge_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("h3SEPD_Charge")->Clone(("h3SEPD_Charge_"+run).c_str())));
    m_hists["h2SEPD_North_Psi2_Q_cent_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("h2SEPD_North_Psi2_Q_cent")->Clone(("h2SEPD_North_Psi2_Q_cent_"+run).c_str())));
    m_hists["h2SEPD_South_Psi2_Q_cent_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("h2SEPD_South_Psi2_Q_cent")->Clone(("h2SEPD_South_Psi2_Q_cent_"+run).c_str())));

    m_hists["h2SEPD_North_Psi3_Q_cent_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("h2SEPD_North_Psi3_Q_cent")->Clone(("h2SEPD_North_Psi3_Q_cent_"+run).c_str())));
    m_hists["h2SEPD_South_Psi3_Q_cent_"+run] = std::unique_ptr<TH1>(static_cast<TH1*>(tfile->Get("h2SEPD_South_Psi3_Q_cent")->Clone(("h2SEPD_South_Psi3_Q_cent_"+run).c_str())));

    tfile->Close();

    return 0;
}

void myAnalysis::make_plots(const string &outputDir) {
    std::unique_ptr<TCanvas> c1 = std::make_unique<TCanvas>();
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

    string output = outputDir + "/plots-EventType.pdf";

    c1->Print((output + "[").c_str(), "pdf portrait");

    for (const auto &[name, hist] : m_hists)
    {
        if(name.starts_with("hEvent")) {
            string run = myUtils::split(name,'_')[1];
            stringstream title;
            title << "Event Type: " << run;

            hist->Draw();
            hist->GetXaxis()->SetTitleOffset(1.f);
            hist->GetYaxis()->SetRangeUser(0, hist->GetMaximum()*1.2);
            hist->SetTitle(title.str().c_str());

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // ------------------------------------------------------

    output = outputDir + "/plots-Centrality.pdf";

    c1->Print((output + "[").c_str(), "pdf portrait");

    for (const auto &[name, hist] : m_hists)
    {
        if(name.starts_with("hCentrality")) {
            string run = myUtils::split(name,'_')[1];
            stringstream title;
            title << "Centrality: " << run;

            hist->Draw();
            hist->GetXaxis()->SetTitleOffset(1.f);
            hist->SetTitle(title.str().c_str());

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // ------------------------------------------------------

    output = outputDir + "/plots-VtxZ.pdf";

    c1->Print((output + "[").c_str(), "pdf portrait");

    for (const auto &[name, hist] : m_hists)
    {
        if(name.starts_with("hVtxZ") && !name.starts_with("hVtxZ_MB")) {
            string run = myUtils::split(name,'_')[1];
            stringstream title;
            title << "Z Vertex: " << run;

            hist->Draw();
            hist->GetXaxis()->SetTitleOffset(1.f);
            // hist->GetYaxis()->SetRangeUser(0, hist->GetMaximum()*1.2);
            hist->SetTitle(title.str().c_str());

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // ------------------------------------------------------

    output = outputDir + "/plots-VtxZ-MB.pdf";

    c1->Print((output + "[").c_str(), "pdf portrait");

    for (const auto &[name, hist] : m_hists)
    {
        if(name.starts_with("hVtxZ_MB")) {
            string run = myUtils::split(name,'_')[2];
            stringstream title;
            title << "Z Vertex (MB): " << run;

            hist->Draw();
            hist->GetXaxis()->SetTitleOffset(1.f);
            hist->SetTitle(title.str().c_str());

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // ------------------------------------------------------

    output = outputDir + "/plots-sEPD-Charge.pdf";

    c1->Print((output + "[").c_str(), "pdf portrait");

    gPad->SetLogz();

    c1->SetLeftMargin(.15f);
    c1->SetRightMargin(.15f);

    for (const auto &[name, hist] : m_hists)
    {
        if(name.starts_with("h3SEPD_Charge")) {
            string run = myUtils::split(name,'_')[2];
            stringstream title;
            title << "sEPD Charge: " << run << "; Total Charge South; Total Charge North";

            std::unique_ptr<TH1> hist_xy = std::unique_ptr<TH1>(dynamic_cast<TH3 *>(hist.get())->Project3D("yx"));

            hist_xy->Draw("COLZ1");
            hist_xy->GetYaxis()->SetTitleOffset(1.6f);
            hist_xy->GetXaxis()->SetTitleOffset(1.f);
            hist_xy->SetTitle(title.str().c_str());

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    c1->Print((output + "]").c_str(), "pdf portrait");

    // m_hists["h_InvMass_2024"]->Draw();
    // m_hists["h_InvMass_2024"]->SetTitle("Invariant Mass: 2024 Au+Au, Run: 54912; m_{#gamma#gamma} [GeV]; Counts");
    // m_hists["h_InvMass_2024"]->GetXaxis()->SetTitleOffset(1.f);
    // m_hists["h_InvMass_2024"]->GetXaxis()->SetRangeUser(0,1);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2024.png").c_str());

    // // ----------------------------------------

    // c1->SetLeftMargin(.17f);

    // m_hists["h_InvMass_2025"]->Draw();
    // m_hists["h_InvMass_2025"]->SetTitle("; m_{#gamma#gamma} [GeV]; Counts");
    // m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    // m_hists["h_InvMass_2025"]->GetYaxis()->SetTitleOffset(1.7f);
    // m_hists["h_InvMass_2025"]->GetXaxis()->SetRangeUser(0,0.5);

    // stringstream info;

    // info << "Au+Au 2025";

    // TLatex latex;
    // latex.SetTextSize(0.04f);
    // latex.DrawLatexNDC(0.85,0.91,"6/10/25");
    // latex.DrawLatexNDC(0.5,0.8,"#bf{#it{sPHENIX}} Preliminary");
    // latex.DrawLatexNDC(0.5,0.8-0.05,"Au+Au 2025 #sqrt{s_{NN}} = 200 GeV");
    // latex.DrawLatexNDC(0.5,0.8-0.05*2,"|z| < 20 cm, N_{Clusters} < 300");
    // latex.DrawLatexNDC(0.5,0.8-0.05*3.3,"1.3 GeV < p_{T,1} < 4 GeV");
    // latex.DrawLatexNDC(0.5,0.8-0.05*4.3,"0.7 GeV < p_{T,2} < 4 GeV");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_2025.png").c_str());

    // // ----------------------------------------

    // c1->SetLeftMargin(.14f);

    // m_hists["h_InvMass_2024"]->Scale(1./m_hists["h_InvMass_2024"]->Integral(1, m_hists["h_InvMass_2024"]->FindBin(1)-1));
    // m_hists["h_InvMass_2025"]->Scale(1./m_hists["h_InvMass_2025"]->Integral(1, m_hists["h_InvMass_2025"]->FindBin(1)-1));

    // m_hists["h_InvMass_2025"]->Draw("hist");
    // m_hists["h_InvMass_2025"]->SetTitle("Invariant Mass; m_{#gamma#gamma} [GeV]; Normalized Counts");
    // m_hists["h_InvMass_2025"]->GetXaxis()->SetTitleOffset(1.f);
    // m_hists["h_InvMass_2025"]->GetYaxis()->SetTitleOffset(1.5f);
    // m_hists["h_InvMass_2025"]->GetXaxis()->SetRangeUser(0,1);
    // m_hists["h_InvMass_2025"]->GetYaxis()->SetRangeUser(0,9e-3);
    // m_hists["h_InvMass_2025"]->SetLineColor(kRed);

    // m_hists["h_InvMass_2024"]->Draw("same hist");

    // m_hists["h_InvMass_2024"]->SetLineColor(kBlue);

    // stringstream legA, legB;

    // legA << "Run 2024 Au+Au";
    // legB << "Run 2025 Au+Au";

    // Double_t xshift = 0.3;
    // Double_t yshift = -0.4;

    // auto leg = new TLegend(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    // leg->SetFillStyle(0);
    // leg->SetTextSize(0.06f);
    // leg->AddEntry(m_hists["h_InvMass_2024"],legA.str().c_str(),"l");
    // leg->AddEntry(m_hists["h_InvMass_2025"],legB.str().c_str(),"l");
    // leg->Draw("same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h_InvMass_overlay.png").c_str());

    // // ----------------------------------------

    // c1->SetLeftMargin(.1f);

    // gPad->SetLogy();

    // stringstream hName;

    // for(Int_t i = 0; i < m_iter; ++i) {
    //     hName.str("");
    //     hName << "hCalib_" << i;
    //     m_hists[hName.str()]->Draw("hist e");
    //     m_hists[hName.str()]->GetXaxis()->SetTitleOffset(0.9f);
    //     m_hists[hName.str()]->GetYaxis()->SetTitleOffset(0.9f);

    //     c1->Print(output.c_str(), "pdf portrait");
    //     if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + ".png").c_str());
    // }

    // gPad->SetLogy(0);
    // c1->SetLeftMargin(.13f);

    // m_hists["hCalib_10"]->Draw("hist e");
    // m_hists["hCalib_10"]->SetLineColor(kRed);
    // m_hists["hCalib_10"]->SetMarkerColor(kRed);
    // m_hists["hCalib_10"]->SetTitle("EMCal Calibration");
    // m_hists["hCalib_10"]->GetXaxis()->SetTitleOffset(0.9f);
    // m_hists["hCalib_10"]->GetYaxis()->SetTitleOffset(1.4f);

    // m_hists["hCalib_2024"]->Draw("hist e same");

    // cout << "2024 Counts: " << m_hists["hCalib_2024"]->Integral(1, m_hists["hCalib_2024"]->GetNbinsX()) << endl;
    // cout << "2025 Counts: " << m_hists["hCalib_10"]->Integral(1, m_hists["hCalib_10"]->GetNbinsX()) << endl;

    // legA.str("");
    // legB.str("");

    // legA << "Run 2024";
    // legB << "Run 2025";

    // xshift = 0.45;
    // yshift = 0;

    // leg = new TLegend(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    // leg->SetFillStyle(0);
    // leg->SetTextSize(0.06f);
    // leg->AddEntry(m_hists["hCalib_2024"],legA.str().c_str(),"l");
    // leg->AddEntry(m_hists["hCalib_10"],legB.str().c_str(),"l");
    // leg->Draw("same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hCalib-overlay.png").c_str());

    // m_hists["hCalib_10"]->GetXaxis()->SetRangeUser(0,5);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hCalib-overlay-zoom.png").c_str());


    // // ----------------------------------------

    // c1->SetCanvasSize(2900, 1000);
    // c1->SetLeftMargin(.06f);
    // c1->SetRightMargin(.1f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);

    // gPad->SetGrid();

    // m_hists["h2Calib_2024"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2Calib_2024"]->SetMinimum(0.5);
    // m_hists["h2Calib_2024"]->SetMaximum(5);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2Calib_2024.png").c_str());

    // // ----------------------------------------

    // for(Int_t i = 0; i < m_iter; ++i) {
    //     hName.str("");
    //     hName << "h2Calib_" << i;

    //     m_hists[hName.str()]->Draw("COLZ1");
    //     m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    //     m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    //     m_hists[hName.str()]->SetMinimum(0.5);
    //     m_hists[hName.str()]->SetMaximum(5);

    //     c1->Print(output.c_str(), "pdf portrait");
    //     if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + ".png").c_str());
    // }

    // for(Int_t i = 0; i < m_iter; ++i) {
    //     hName.str("");
    //     hName << "h2Calib_" << i;

    //     m_hists[hName.str()]->Draw("COLZ1");

    //     m_hists[hName.str()]->SetMinimum(0.5);
    //     m_hists[hName.str()]->SetMaximum(5);

    //     m_hists[hName.str()]->GetYaxis()->SetRangeUser(0,8);
    //     m_hists[hName.str()]->GetYaxis()->SetNdivisions(8, false);

    //     c1->Print(output.c_str(), "pdf portrait");
    //     if (m_saveFig) c1->Print((outputDir + "/images/" + hName.str() + "-zoom-South-IB-5.png").c_str());
    // }

    // // ----------------------------------------

    // c1->Print((output + "]").c_str(), "pdf portrait");
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

    if(!myUtils::readCSV(input, myAnalysis::readHists, false)) return;

    myAnalysis::make_plots(outputDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 3){
        cout << "usage: ./" << argv[0] << " input [outputDir]" << endl;
        cout << "input: list of root files" << endl;
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
