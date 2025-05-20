// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TH2F.h>
#include <TH3F.h>
#include <TF1.h>
#include <TFile.h>
#include <TProfile.h>
#include <TLine.h>
#include <TLegend.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TColor.h>

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
namespace fs = std::filesystem;

namespace myAnalysis {
    void analyze(const string &output, const string &outputRoot, const string &input_hists);
    void setEMCalDim(TH1* hist);
    Double_t getMeanY(TH1* h, const Double_t start, const Double_t end);

    Int_t m_nsector = 64;
    Int_t m_nchannel_per_sector = 384;
    Int_t m_nchannel_per_ib = 64;
    Int_t m_nphi = 256;
    Int_t m_neta = 96;
    Int_t m_nib = 384;
    Int_t m_nib_per_sector = 6;

    Int_t m_bins_offset   = 300;
    Double_t m_offset_low  = -2000;
    Double_t m_offset_high = 1000;

    Int_t m_bins_offsetDelta   = 580;
    Double_t m_offsetDelta_low  = -800;
    Double_t m_offsetDelta_high = 2100;

    Int_t m_bins_calib = 100;
    Double_t m_calib_low  = 0;
    Double_t m_calib_high = 5;

    Int_t m_bins_offsetDivCosmic = 120;
    Double_t m_offsetDivCosmic_low  = -8;
    Double_t m_offsetDivCosmic_high = 4;

    unordered_map<string,TH1*> m_hists;

    Int_t m_verbosity = 1;
}

void myAnalysis::setEMCalDim(TH1* hist) {
    hist->GetXaxis()->SetLimits(0,256);
    hist->GetXaxis()->SetNdivisions(32, false);
    hist->GetXaxis()->SetLabelSize(0.04);
    hist->GetXaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetTickSize(0.01);
    hist->GetYaxis()->SetLabelSize(0.04);
    hist->GetYaxis()->SetLimits(0,96);
    hist->GetYaxis()->SetNdivisions(12, false);
    hist->GetYaxis()->SetTitleOffset(0.5);
    hist->GetXaxis()->SetTitleOffset(1);
}

Double_t myAnalysis::getMeanY(TH1* h, const Double_t start, const Double_t end) {
    Int_t bin_start = h->FindBin(start);
    Int_t bin_end = h->FindBin(end)-1;

    h->GetXaxis()->SetRange(bin_start, bin_end);

    Double_t mean = h->GetMean(2);

    h->GetXaxis()->SetRange(1, h->GetNbinsX());
    return mean;
}

void myAnalysis::analyze(const string &output, const string &outputRoot, const string &input) {
    string outputDir = fs::absolute(output).parent_path().string();
    fs::create_directories(outputDir);

    // open calib file
    // Open the ROOT file
    TFile* file = TFile::Open(input.c_str());

    if (!file || file->IsZombie()) {
        cout << "Error: Could not open ROOT file." << endl;
        return; // Indicate an error
    }

    m_hists["h2Offset"]      = static_cast<TH2*>(file->Get("h2Offset"));
    m_hists["h2Calib"]     = static_cast<TH2*>(file->Get("h2Calib"));
    m_hists["h2CosmicMPV"] = static_cast<TH2*>(file->Get("h2CosmicMPV"));
    m_hists["h2OffsetDivCosmicCalib"] = static_cast<TH2*>(file->Get("h2OffsetDivCosmicCalib"));
    m_hists["h2OffsetDivCosmicCalib_px"] = static_cast<TH2*>(m_hists["h2OffsetDivCosmicCalib"])->ProfileX();
    m_hists["h2DummySector"] = static_cast<TH2*>(file->Get("h2DummySector"));
    m_hists["h2DummyIB"] = static_cast<TH2*>(file->Get("h2DummyIB"));

    m_hists["h2OffsetNew"] = new TH2F("h2OffsetNew","Offset [mV]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2OffsetNewDelta"] = new TH2F("h2OffsetNewDelta","#Delta Offset [mV]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2OffsetNewDeltaNeg"] = new TH2F("h2OffsetNewDeltaNeg","#Delta Offset [mV]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);

    m_hists["hOffset"] = new TH1F("hOffset","Tower; Offset [V]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hOffsetNew"] = new TH1F("hOffsetNew","Tower; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hOffsetNewDelta"] = new TH1F("hOffsetNewDelta","Tower; #Delta Offset [mV]; Counts", m_bins_offsetDelta, m_offsetDelta_low, m_offsetDelta_high);
    m_hists["h2OffsetNewDivCosmicCalib"] = new TH2F("h2OffsetNewDivCosmicCalib","EMCal; EMCal Calibration [MeV/ADC]; Offset/Cosmic MPV [mV]", m_bins_calib, m_calib_low, m_calib_high, m_bins_offsetDivCosmic, m_offsetDivCosmic_low, m_offsetDivCosmic_high);

    Double_t delta = 0.1;
    Double_t target = 2;

    // compute target mean
    Int_t bin_start = m_hists["h2OffsetDivCosmicCalib_px"]->FindBin(target-delta);
    Int_t bin_end = m_hists["h2OffsetDivCosmicCalib_px"]->FindBin(target+delta)-1;

    Double_t targetMean = getMeanY(m_hists["h2OffsetDivCosmicCalib_px"], target-delta, target+delta);
    Double_t initMean = getMeanY(m_hists["h2OffsetDivCosmicCalib_px"], 0, target-delta);

    cout << "Initial Mean: " << initMean << endl;
    cout << "Target Mean: " << targetMean << endl;
    cout << "Ratio: " << targetMean / initMean << endl;

    Double_t min_offset = 9999;
    Double_t max_offset = 0;

    Double_t min_offsetDivCosmic3 = 9999;
    Double_t max_offsetDivCosmic3 = 0;

    Double_t min_offsetDelta = 9999;
    Double_t max_offsetDelta = 0;

    Int_t nprint = 0;
    Double_t max_cosmic = 900;

    for(Int_t i = 1; i <= m_nphi; ++i) {
        for(Int_t j = 1; j <= m_neta; ++j) {
            Double_t offset = m_hists["h2Offset"]->GetBinContent(i,j);
            Double_t calib = m_hists["h2Calib"]->GetBinContent(i,j);
            Double_t cosmic = m_hists["h2CosmicMPV"]->GetBinContent(i,j);

            Double_t offsetNew = offset;

            if (0 < calib && calib < target - delta && 0 < cosmic && cosmic < max_cosmic) {
              offsetNew = targetMean * cosmic;
              if (m_verbosity && offset < -1000 && nprint++ < 5) {
                cout << "iphi: " << i - 1
                     << ", ieta: " << j - 1
                     << ", cosmic MPV: " << cosmic
                     << ", offset: " << offset
                     << " mV, offsetNew: " << offsetNew
                     << " mV, offset/cosmic: " << offset / cosmic
                     << ", offsetNew/cosmic: " << offsetNew / cosmic << endl;
              }
            }

            Double_t offsetDelta = offsetNew-offset;

            min_offset = min(min_offset, offset);
            max_offset = max(max_offset, offset);

            min_offset = min(min_offset, offsetNew);
            max_offset = max(max_offset, offsetNew);

            min_offsetDelta = min(min_offsetDelta, offsetDelta);
            max_offsetDelta = max(max_offsetDelta, offsetDelta);

            Double_t offsetNewDivCosmic = (cosmic) ? offsetNew/cosmic : 0;

            min_offsetDivCosmic3 = min(min_offsetDivCosmic3, offsetNewDivCosmic);
            max_offsetDivCosmic3 = max(max_offsetDivCosmic3, offsetNewDivCosmic);

            m_hists["hOffset"]->Fill(offset);

            m_hists["h2OffsetNew"]->SetBinContent(i,j,offsetNew);

            m_hists["hOffsetNewDelta"]->Fill(offsetDelta);
            m_hists["h2OffsetNewDelta"]->SetBinContent(i,j,offsetDelta);
            if(offset > offsetNew) m_hists["h2OffsetNewDeltaNeg"]->SetBinContent(i,j,offsetDelta);

            m_hists["hOffsetNew"]->Fill(offsetNew);

            if(0 < cosmic && cosmic < max_cosmic && calib) {
                static_cast<TH2*>(m_hists["h2OffsetNewDivCosmicCalib"])->Fill(calib,offsetNewDivCosmic);
            }
        }
    }
    cout << "################" << endl;
    cout << "Offset 3 Min: " << min_offset << ", Max: " << max_offset << endl;
    cout << "Offset Delta Min: " << min_offsetDelta << ", Max: " << max_offsetDelta << endl;

    cout << "OffsetDivCosmic 3 Min: " << min_offsetDivCosmic3 << ", Max: " << max_offsetDivCosmic3 << endl;

    setEMCalDim(m_hists["h2Offset"]);
    setEMCalDim(m_hists["h2OffsetNew"]);
    setEMCalDim(m_hists["h2OffsetNewDelta"]);
    setEMCalDim(m_hists["h2OffsetNewDeltaNeg"]);
    setEMCalDim(m_hists["h2DummySector"]);
    setEMCalDim(m_hists["h2DummyIB"]);

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.1);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.08);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    gPad->SetGrid();

    c1->Print((output + "[").c_str(), "pdf portrait");

    // ---------------------------

    m_hists["h2Offset"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2Offset"]->SetMinimum((Int_t)min_offset);
    m_hists["h2Offset"]->SetMaximum((Int_t)max_offset+1);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2Offset"]->GetName()) + "-3.png").c_str());

    // ---------------------------

    m_hists["h2OffsetNew"]->SetMinimum((Int_t)min_offset);
    m_hists["h2OffsetNew"]->SetMaximum((Int_t)max_offset+1);

    m_hists["h2OffsetNew"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2OffsetNew"]->GetName()) + ".png").c_str());

    // ---------------------------

    m_hists["h2OffsetNewDelta"]->SetMinimum(-1000);
    // m_hists["h2OffsetNew"]->SetMaximum((Int_t)max_offset+1);

    m_hists["h2OffsetNewDelta"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2OffsetNewDelta"]->GetName()) + ".png").c_str());

    // ---------------------------

    m_hists["h2OffsetNewDeltaNeg"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2OffsetNewDeltaNeg"]->GetName()) + ".png").c_str());

    // ---------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetLogy();
    gPad->SetGrid(0,0);

    m_hists["hOffset"]->Rebin(10);
    m_hists["hOffsetNew"]->Rebin(10);
    m_hists["hOffsetNew"]->SetLineColor(kRed);

    m_hists["hOffsetNew"]->Draw();
    m_hists["hOffset"]->Draw("same");

    m_hists["hOffsetNew"]->GetYaxis()->SetRangeUser(5e-1,1e5);
    m_hists["hOffsetNew"]->GetXaxis()->SetTitleOffset(1);

    auto leg = new TLegend(0.7,.78,0.85,.88);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hOffset"],"Default","f");
    leg->AddEntry(m_hists["hOffsetNew"],"After","f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hOffsetNew"]->GetName()) + ".png").c_str());

    gPad->SetLogy(0);

    m_hists["hOffsetNew"]->GetYaxis()->SetRangeUser(0,2e4);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hOffsetNew"]->GetName()) + "-linear-scale.png").c_str());

    // ---------------------------------

    m_hists["hOffsetNewDelta"]->Rebin(20);
    // m_hists["hOffsetNewDelta"]->SetLineColor(kRed);

    m_hists["hOffsetNewDelta"]->Draw();

    // m_hists["hOffsetNewDelta"]->GetYaxis()->SetRangeUser(5e-1,1e5);
    m_hists["hOffsetNewDelta"]->GetXaxis()->SetTitleOffset(1);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hOffsetNewDelta"]->GetName()) + ".png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.14);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    // gPad->SetLogy(0);

    // m_hists["h2OffsetDivCosmicCalib"]->SetMaximum(600);
    m_hists["h2OffsetDivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2OffsetDivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    m_hists["h2OffsetDivCosmicCalib_px"]->SetLineColor(kRed);
    m_hists["h2OffsetDivCosmicCalib_px"]->SetMarkerColor(kRed);
    m_hists["h2OffsetDivCosmicCalib_px"]->Draw("same");

    m_hists["h2OffsetDivCosmicCalib"]->GetYaxis()->SetRangeUser(-8,4);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2OffsetDivCosmicCalib"]->GetName()) + ".png").c_str());

    m_hists["h2OffsetNewDivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2OffsetNewDivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2OffsetNewDivCosmicCalib_px = static_cast<TH2*>(m_hists["h2OffsetNewDivCosmicCalib"])->ProfileX();
    h2OffsetNewDivCosmicCalib_px->SetLineColor(kRed);
    h2OffsetNewDivCosmicCalib_px->SetMarkerColor(kRed);
    h2OffsetNewDivCosmicCalib_px->Draw("same");

    // m_hists["h2OffsetNewDivCosmicCalib"]->GetYaxis()->SetRangeUser(0,1.2);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2OffsetNewDivCosmicCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    stringstream name, legA, legB;
    Double_t meanA, meanB;
    gPad->SetLogy();
    gPad->SetGrid(0,0);

    name << "h2OffsetDivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2OffsetDivCosmicCalibA_py = static_cast<TH2*>(m_hists["h2OffsetDivCosmicCalib"])->ProjectionY("h2OffsetDivCosmicCalib_py", bin_start, bin_end);
    TH1* h2OffsetDivCosmicCalibB_py = static_cast<TH2*>(m_hists["h2OffsetDivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2OffsetDivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2OffsetDivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2OffsetDivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2OffsetDivCosmicCalibB_py->SetLineColor(kRed);

    h2OffsetDivCosmicCalibB_py->Draw();
    h2OffsetDivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2OffsetDivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2OffsetDivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.75,0.6,.85);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2OffsetDivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2OffsetDivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2OffsetDivCosmicCalibA_py->GetName()) + ".png").c_str());

    name.str("");

    name << "h2OffsetNewDivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2OffsetNewDivCosmicCalibA_py = static_cast<TH2*>(m_hists["h2OffsetNewDivCosmicCalib"])->ProjectionY("h2OffsetNewDivCosmicCalib_py", bin_start, bin_end);
    TH1* h2OffsetNewDivCosmicCalibB_py = static_cast<TH2*>(m_hists["h2OffsetNewDivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2OffsetNewDivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2OffsetNewDivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2OffsetNewDivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2OffsetNewDivCosmicCalibB_py->SetLineColor(kRed);

    h2OffsetNewDivCosmicCalibB_py->Draw();
    h2OffsetNewDivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2OffsetNewDivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2OffsetNewDivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.7,0.6,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2OffsetNewDivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2OffsetNewDivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2OffsetNewDivCosmicCalibA_py->GetName()) + ".png").c_str());

    c1->Print((output + "]").c_str(), "pdf portrait");
    file->Close();
}

void update_Offset(const string &input_hists,
                 const string &output="plots.pdf",
                 const string &outputRoot="test.root") {
    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input hists: "  << input_hists << endl;
    cout << "output: " << output << endl;
    cout << "output root file: " << outputRoot << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    string outputDir = fs::absolute(output).parent_path().string();

    fs::create_directories(outputDir + "/images");

    myAnalysis::analyze(output, outputRoot, input_hists);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./read-Offset input_hists [output] [outputRoot]" << endl;
        cout << "input_hists: input EMCal block info hists" << endl;
        cout << "output: output pdf file" << endl;
        cout << "output root: output root file" << endl;
        return 1;
    }

    string output    = "plots.pdf";
    string outputRoot = "test.root";

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        outputRoot = argv[3];
    }

    update_Offset(argv[1], output, outputRoot);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
