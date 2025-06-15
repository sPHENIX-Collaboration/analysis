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
    // void initHists();
    // void saveHists(const string &outputDir);
    // void findBestPulseWidth();

    // unordered_map<string, Int_t> m_best_pulse; // key: sector,ib ; value: best width
    // unordered_map<string,Int_t> m_runInfo; // key: width,offset ; value: run number
    // unordered_map<Int_t,string> m_runInfoRev; // key: run number ; value: width,offset
    map<string,TH1*> m_hists;

    // Double_t kFactor  = 4.42e-4; // Units: [1 / mV]

    // unordered_set<string> m_knownBadIB = {"4.1","10.3","10.5","25.2","50.1","54.4"}; // sector.IB

    // Int_t m_bins_gain = 80;
    // Double_t m_gain_low = 0;
    // Double_t m_gain_high = 4;

    // Int_t m_bins_saturate = 800;
    // Double_t m_saturate_low  = 0;  // GeV
    // Double_t m_saturate_high = 80; // GeV

    // Double_t m_noiseThreshold = 100; // ADC

    Bool_t m_saveFig = true;
    // Int_t m_verbosity = 2;
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

    // m_hists["h_InvMass_2024"]->Scale(1.*1e4/m_hists["h_event_2024"]->GetBinContent(1));
    // m_hists["h_InvMass_2025"]->Scale(1.*1e4/m_hists["h_event_2025"]->GetBinContent(1));
    m_hists["h_InvMass_2024"]->Scale(1./m_hists["h_InvMass_2024"]->Integral(1, m_hists["h_InvMass_2024"]->FindBin(1)-1));
    m_hists["h_InvMass_2025"]->Scale(1./m_hists["h_InvMass_2025"]->Integral(1, m_hists["h_InvMass_2025"]->FindBin(1)-1));

    m_hists["h_InvMass_2025"]->Draw("hist");
    // m_hists["h_InvMass_2025"]->SetTitle("Invariant Mass; M_{#gamma#gamma} [GeV]; Counts / 10k Events / 5 MeV");
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

    // for (const auto &[name, hist] : m_hists) {
    //     if(name.starts_with("h2CEMC")) {
    //         Int_t run = std::stoi(myUtils::split(name,'_')[1]);
    //         vector<string> width_offset = myUtils::split(m_runInfoRev[run],',');
    //         stringstream title;
    //         title << "EMCal [ADC], Run: " << run << ", Pulse Width: " << width_offset[0] << " ns, Offset: " << width_offset[1];

    //         hist->SetTitle(title.str().c_str());

    //         hist->Draw("COLZ1");
    //         m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    //         m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    //         hist->SetMaximum(1.64e4);

    //         c1->Print(output.c_str(), "pdf portrait");
    //         if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
    //     }
    // }

    // // ----------------------------------------

    // m_hists["h2ResponseDefault"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2ResponseDefault"]->SetMaximum(1.64e4);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseDefault.png").c_str());

    // // ----------------------------------------

    // m_hists["h2ResponseNew"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2ResponseNew"]->SetMaximum(1.64e4);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseNew.png").c_str());

    // // ----------------------------------------

    // m_hists["h2GainFactor"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2GainFactor"]->SetTitle("Gain");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor.png").c_str());

    // m_hists["h2GainFactor"]->SetMinimum(0.1);
    // m_hists["h2GainFactor"]->SetMaximum(0.9);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor-zoom.png").c_str());

    // // ----------------------------------------

    // m_hists["h2ResponseRatio"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatio.png").c_str());

    // m_hists["h2ResponseRatio"]->SetMinimum(0.1);
    // m_hists["h2ResponseRatio"]->SetMaximum(0.9);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatio-zoom.png").c_str());

    // // ----------------------------------------

    // c1->SetCanvasSize(1400, 1000);
    // c1->SetLeftMargin(.14f);
    // c1->SetRightMargin(.05f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);
    // gStyle->SetOptStat(1111);

    // m_hists["hResponseRatio"]->Draw("hist e");
    // m_hists["hResponseRatio"]->SetStats();
    // m_hists["hResponseRatio"]->SetLineColor(kRed);
    // m_hists["hResponseRatio"]->SetMarkerColor(kRed);
    // m_hists["hResponseRatio"]->GetXaxis()->SetTitleOffset(0.9f);
    // m_hists["hResponseRatio"]->GetXaxis()->SetRangeUser(0,1.5);

    // gPad->Update();

    // TPaveStats* st = static_cast<TPaveStats*>(m_hists["hResponseRatio"]->FindObject("stats"));
    // Double_t xlow = 0.68;
    // Double_t ylow = 0.62;
    // st->SetX1NDC(xlow);
    // st->SetY1NDC(ylow);
    // st->SetX2NDC(xlow+0.27);
    // st->SetY2NDC(ylow+0.28);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatio.png").c_str());

    // gPad->SetLogy();

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatio-logy.png").c_str());

    // gPad->SetLogy(0);

    // // ----------------------------------------
    // c1->SetCanvasSize(2900, 1000);
    // c1->SetLeftMargin(.06f);
    // c1->SetRightMargin(.1f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);

    // m_hists["h2ResponseRatioDivGain"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatioDivGain.png").c_str());

    // m_hists["h2ResponseRatioDivGain"]->SetMinimum(0.4);
    // m_hists["h2ResponseRatioDivGain"]->SetMaximum(1.1);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatioDivGain-zoom.png").c_str());

    // // ----------------------------------------

    // c1->SetCanvasSize(1400, 1000);
    // c1->SetLeftMargin(.14f);
    // c1->SetRightMargin(.05f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);
    // gStyle->SetOptFit(1111); // Shows Probability, Chi2/NDF, Errors, Values

    // TFitResultPtr fitResult = myUtils::doGausFit(m_hists["hResponseRatioDivGain"], 0.87, 1.08);

    // m_hists["hResponseRatioDivGain"]->Draw("e");
    // m_hists["hResponseRatioDivGain"]->SetStats();
    // m_hists["hResponseRatioDivGain"]->SetMarkerColor(kBlack);
    // m_hists["hResponseRatioDivGain"]->GetXaxis()->SetTitleOffset(0.9f);
    // m_hists["hResponseRatioDivGain"]->GetXaxis()->SetRangeUser(0,2);

    // gPad->Update();

    // st = static_cast<TPaveStats*>(m_hists["hResponseRatioDivGain"]->FindObject("stats"));
    // xlow = 0.65;
    // ylow = 0.5;
    // st->SetX1NDC(xlow);
    // st->SetY1NDC(ylow);
    // st->SetX2NDC(xlow+0.3);
    // st->SetY2NDC(ylow+0.4);
    // st->SetTextSize(0.03f);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain.png").c_str());

    // gPad->SetLogy();

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain-logy.png").c_str());

    // // ----------------------------------------

    // gPad->SetLogy(0);
    // gPad->SetLogz();

    // c1->SetCanvasSize(1400, 1000);
    // c1->SetLeftMargin(.11f);
    // c1->SetRightMargin(.13f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);

    // m_hists["hResponseRatioVsGain"]->Draw("COLZ1");
    // m_hists["hResponseRatioVsGain"]->GetXaxis()->SetRangeUser(0.2,1.2);
    // m_hists["hResponseRatioVsGain"]->GetYaxis()->SetRangeUser(0.2,1.2);
    // m_hists["hResponseRatioVsGain"]->GetYaxis()->SetTitleOffset(0.85f);
    // m_hists["hResponseRatioVsGain"]->GetXaxis()->SetTitleOffset(0.9f);

    // TH1* px = static_cast<TH2*>(m_hists["hResponseRatioVsGain"])->ProfileX();
    // px->SetLineColor(kRed);
    // px->SetMarkerColor(kRed);
    // px->Draw("same");

    // TF1* tf = new TF1("tf","x",0,2);
    // tf->SetLineStyle(kDashed);
    // tf->Draw("same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioVsGain.png").c_str());

    // // ----------------------------------------

    // c1->SetCanvasSize(1400, 1000);
    // c1->SetLeftMargin(.13f);
    // c1->SetRightMargin(.05f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);
    // gPad->SetGrid(0,0);

    // cout << "Saturate Hists Entries" << endl;
    // cout << "hSaturate: " << m_hists["hSaturate"]->Integral(1, m_bins_saturate) << endl;
    // cout << "hSaturateV2: " << m_hists["hSaturateV2"]->Integral(1, m_bins_saturate) << endl;
    // cout << "hSaturateV3: " << m_hists["hSaturateV3"]->Integral(1, m_bins_saturate) << endl;
    // cout << "hSaturateV4: " << m_hists["hSaturateV4"]->Integral(1, m_bins_saturate) << endl;

    // Double_t normalize = m_hists["hSaturateV3"]->Integral(1, m_bins_saturate) / m_hists["hSaturateV4"]->Integral(1, m_bins_saturate);

    // m_hists["hSaturateV4"]->Scale(normalize);

    // cout << "hSaturateV4 (after normalization): " << m_hists["hSaturateV4"]->Integral(1, m_bins_saturate) << endl;

    // m_hists["hSaturate"]->Draw("hist e");
    // m_hists["hSaturate"]->GetXaxis()->SetTitleOffset(0.9f);
    // m_hists["hSaturate"]->GetYaxis()->SetRangeUser(0,4e3);
    // m_hists["hSaturate"]->Rebin(10);

    // m_hists["hSaturateV2"]->Draw("hist e same");
    // m_hists["hSaturateV2"]->Rebin(10);
    // m_hists["hSaturateV2"]->SetLineColor(kRed);
    // m_hists["hSaturateV2"]->SetMarkerColor(kRed);

    // m_hists["hSaturateV3"]->Draw("hist e same");
    // m_hists["hSaturateV3"]->Rebin(10);
    // m_hists["hSaturateV3"]->SetLineColor(kBlue);
    // m_hists["hSaturateV3"]->SetMarkerColor(kBlue);

    // m_hists["hSaturateV4"]->Draw("hist e same");
    // m_hists["hSaturateV4"]->Rebin(10);
    // m_hists["hSaturateV4"]->SetLineColor(kMagenta);
    // m_hists["hSaturateV4"]->SetMarkerColor(kMagenta);

    // stringstream legA, legB, legC, legD;

    // legA << "Before Temperature Correction";
    // legB << "After Temperature Correction";
    // legC << "After Adjusting Gain (Expected)";
    // legD << "After Adjusting Gain (Observed)";

    // Double_t xshift = -0.08;
    // Double_t yshift = 0.03;

    // auto leg = new TLegend(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    // leg->SetFillStyle(0);
    // leg->SetTextSize(0.04f);
    // leg->AddEntry(m_hists["hSaturate"],legA.str().c_str(),"lpe");
    // leg->AddEntry(m_hists["hSaturateV2"],legB.str().c_str(),"lpe");
    // leg->AddEntry(m_hists["hSaturateV3"],legC.str().c_str(),"lpe");
    // leg->AddEntry(m_hists["hSaturateV4"],legD.str().c_str(),"lpe");
    // leg->Draw("same");

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/hSaturate-overlay.png").c_str());

    // // ----------------------------------------

    // c1->SetCanvasSize(2900, 1000);
    // c1->SetLeftMargin(.06f);
    // c1->SetRightMargin(.1f);
    // c1->SetTopMargin(.1f);
    // c1->SetBottomMargin(.12f);
    // gPad->SetLogz(0);
    // gPad->SetGrid();

    // m_hists["h2SaturateV3"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2SaturateV3"]->SetMinimum(10);
    // m_hists["h2SaturateV3"]->SetMaximum(60);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV3.png").c_str());

    // // ----------------------------------------

    // m_hists["h2SaturateV4"]->Draw("COLZ1");
    // m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    // m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    // m_hists["h2SaturateV4"]->SetMinimum(10);
    // m_hists["h2SaturateV4"]->SetMaximum(60);

    // c1->Print(output.c_str(), "pdf portrait");
    // if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV4.png").c_str());

    // // ----------------------------------------

    // c1->Print((output + "]").c_str(), "pdf portrait");
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
    // fs::create_directories(plotDir);
    fs::create_directories(outputDir+"/images");

    TH1::AddDirectory(kFALSE);

    if(myAnalysis::readHists()) return;

    myAnalysis::make_plots(outputDir);

    // if(!myUtils::readCSV(input_raw_info, myAnalysis::readRunInfo)) return;

    // if(myAnalysis::m_verbosity > 1) {
    //     cout << "####################" << endl;
    //     for (const auto& [key, value] : myAnalysis::m_runInfo) {
    //         cout << "Key: " << key << ", Run: " << value << endl;
    //     }
    //     cout << "Total: " << myAnalysis::m_runInfo.size() << endl;
    //     cout << "####################" << endl;
    // }

    // // do not skip the first line as it's not the usual header
    // if(!myUtils::readCSV(input_raw, myAnalysis::readRun, false)) return;

    // if(myAnalysis::m_verbosity > 1) {
    //     cout << "####################" << endl;
    //     for (const auto& [key, value] : myAnalysis::m_hists) {
    //         cout << "Key: " << key << ", Run: " << value->GetName() << endl;
    //     }
    //     cout << "Total: " << myAnalysis::m_hists.size() << endl;
    //     cout << "####################" << endl;
    // }

    // myAnalysis::findBestPulseWidth();

    // myAnalysis::initHists();
    // myAnalysis::analyze();
    // myAnalysis::saveHists(outputDir);
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
