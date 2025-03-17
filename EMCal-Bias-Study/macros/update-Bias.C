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

    Int_t m_bins_bias1   = 460;
    Double_t m_bias1_low  = 67;
    Double_t m_bias1_high = 90;

    Int_t m_bins_bias2   = 1780;
    Double_t m_bias2_low  = 67;
    Double_t m_bias2_high = 156;

    Int_t m_bins_bias3   = 4240;
    Double_t m_bias3_low  = 4;
    Double_t m_bias3_high = 216;

    Int_t m_bins_calib = 100;
    Double_t m_calib_low  = 0;
    Double_t m_calib_high = 5;

    Int_t m_bins_vbDivCosmic = 200;
    Double_t m_vbDivCosmic_low  = 0;
    Double_t m_vbDivCosmic_high = 5;

    unordered_map<string,TH1*> m_hists;

    Int_t m_verbosity = 0;
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

    m_hists["h2Bias"]      = (TH2F*)file->Get("h2Bias");
    m_hists["h2Calib"]     = (TH2F*)file->Get("h2Calib");
    m_hists["h2CosmicMPV"] = (TH2F*)file->Get("h2CosmicMPV");
    m_hists["h2VbDivCosmicCalib"] = (TH2F*)file->Get("h2VbDivCosmicCalib");
    m_hists["h2VbDivCosmicCalib_px"] = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProfileX();
    m_hists["h2DummySector"] = (TH2F*)file->Get("h2DummySector");
    m_hists["h2DummyIB"] = (TH2F*)file->Get("h2DummyIB");

    m_hists["h2BiasNew1"] = new TH2F("h2BiasNew1","Bias [V]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2BiasNew2"] = new TH2F("h2BiasNew2","Bias [V]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);
    m_hists["h2BiasNew3"] = new TH2F("h2BiasNew3","Bias [V]; Tower Index #phi; Tower Index #eta", m_nphi, -0.5, m_nphi-0.5, m_neta, -0.5, m_neta-0.5);

    m_hists["hBias"] = new TH1F("hBias","Tower; Bias [V]; Counts", m_bins_bias1, m_bias1_low, m_bias1_high);
    m_hists["hBiasNew1"] = new TH1F("hBiasNew1","Tower; Bias [V]; Counts", m_bins_bias1, m_bias1_low, m_bias1_high);
    m_hists["hBiasNew2"] = new TH1F("hBiasNew2","Tower; Bias [V]; Counts", m_bins_bias2, m_bias2_low, m_bias2_high);
    m_hists["hBiasNew3"] = new TH1F("hBiasNew3","Tower; Bias [V]; Counts", m_bins_bias3, m_bias3_low, m_bias3_high);
    m_hists["h2VbNew1DivCosmicCalib"] = new TH2F("h2VbNew1DivCosmicCalib","EMCal; EMCal Calibration [MeV/ADC]; Vb/Cosmic MPV [V]", m_bins_calib, m_calib_low, m_calib_high, m_bins_vbDivCosmic, m_vbDivCosmic_low, m_vbDivCosmic_high);
    m_hists["h2VbNew2DivCosmicCalib"] = new TH2F("h2VbNew2DivCosmicCalib","EMCal; EMCal Calibration [MeV/ADC]; Vb/Cosmic MPV [V]", m_bins_calib, m_calib_low, m_calib_high, m_bins_vbDivCosmic, m_vbDivCosmic_low, m_vbDivCosmic_high);
    m_hists["h2VbNew3DivCosmicCalib"] = new TH2F("h2VbNew3DivCosmicCalib","EMCal; EMCal Calibration [MeV/ADC]; Vb/Cosmic MPV [V]", m_bins_calib, m_calib_low, m_calib_high, m_bins_vbDivCosmic, m_vbDivCosmic_low, m_vbDivCosmic_high);

    Double_t delta = 0.1;
    Double_t target = 2;

    // compute target mean
    Int_t bin_start = m_hists["h2VbDivCosmicCalib_px"]->FindBin(target-delta);
    Int_t bin_end = m_hists["h2VbDivCosmicCalib_px"]->FindBin(target+delta)-1;

    Double_t targetMean = getMeanY(m_hists["h2VbDivCosmicCalib_px"], target-delta, target+delta);
    Double_t initMean = getMeanY(m_hists["h2VbDivCosmicCalib_px"], 0, target-delta);

    cout << "Initial Mean: " << initMean << endl;
    cout << "Target Mean: " << targetMean << endl;
    cout << "Ratio: " << targetMean / initMean << endl;

    Double_t min_bias1 = 9999;
    Double_t max_bias1 = 0;

    Double_t min_bias2 = 9999;
    Double_t max_bias2 = 0;

    Double_t min_bias3 = 9999;
    Double_t max_bias3 = 0;

    Double_t min_vbDivCosmic1 = 9999;
    Double_t max_vbDivCosmic1 = 0;

    Double_t min_vbDivCosmic2 = 9999;
    Double_t max_vbDivCosmic2 = 0;

    Double_t min_vbDivCosmic3 = 9999;
    Double_t max_vbDivCosmic3 = 0;

    Int_t nprint = 0;

    for(Int_t i = 1; i <= m_nphi; ++i) {
        for(Int_t j = 1; j <= m_neta; ++j) {
            Double_t vb = m_hists["h2Bias"]->GetBinContent(i,j);
            Double_t calib = m_hists["h2Calib"]->GetBinContent(i,j);
            Double_t cosmic = m_hists["h2CosmicMPV"]->GetBinContent(i,j);

            Double_t vbNew1 = vb;
            Double_t vbNew2 = vb;
            Double_t vbNew3 = vb;

            // Double_t vbNew = (calib && calib < target-delta) ? vb*updateBiasFactor : vb;

            if(calib && calib < target-delta) {
                // v1
                Double_t updateBiasFactor = targetMean / initMean;
                vbNew1 *= updateBiasFactor;

                // v2
                Int_t binx = m_hists["h2VbDivCosmicCalib_px"]->FindBin(calib);
                Double_t val = m_hists["h2VbDivCosmicCalib_px"]->GetBinContent(binx);
                updateBiasFactor = (val) ? targetMean / val : 1;
                vbNew2 *= updateBiasFactor;

                // v3
                Double_t max_cosmic = 800;
                if(0 < cosmic && cosmic < max_cosmic) {
                    vbNew3 = targetMean * cosmic;
                    if(m_verbosity && vbNew3 > 100 && nprint++ < 5) {
                        cout << "iphi: " << i-1
                             << ", ieta: " << j-1
                             << ", cosmic MPV: " << cosmic
                             << ", vb: " << vb
                             << " V, vbNew: " << vbNew3
                             << " V, vb/cosmic: " << vb/cosmic
                             << ", vbNew/cosmic: " << vbNew3/cosmic << endl;
                    }
                }
            }

            min_bias1 = min(min_bias1, vbNew1);
            max_bias1 = max(max_bias1, vbNew1);

            min_bias2 = min(min_bias2, vbNew2);
            max_bias2 = max(max_bias2, vbNew2);

            min_bias3 = min(min_bias3, vbNew3);
            max_bias3 = max(max_bias3, vbNew3);

            Double_t vbNew1DivCosmic = (cosmic) ? vbNew1/cosmic : 0;
            Double_t vbNew2DivCosmic = (cosmic) ? vbNew2/cosmic : 0;
            Double_t vbNew3DivCosmic = (cosmic) ? vbNew3/cosmic : 0;

            min_vbDivCosmic1 = min(min_vbDivCosmic1, vbNew1DivCosmic);
            max_vbDivCosmic1 = max(max_vbDivCosmic1, vbNew1DivCosmic);

            min_vbDivCosmic2 = min(min_vbDivCosmic2, vbNew2DivCosmic);
            max_vbDivCosmic2 = max(max_vbDivCosmic2, vbNew2DivCosmic);

            min_vbDivCosmic3 = min(min_vbDivCosmic3, vbNew3DivCosmic);
            max_vbDivCosmic3 = max(max_vbDivCosmic3, vbNew3DivCosmic);

            m_hists["hBias"]->Fill(vb);

            m_hists["h2BiasNew1"]->SetBinContent(i,j,vbNew1);
            m_hists["h2BiasNew2"]->SetBinContent(i,j,vbNew2);
            m_hists["h2BiasNew3"]->SetBinContent(i,j,vbNew3);

            m_hists["hBiasNew1"]->Fill(vbNew1);
            m_hists["hBiasNew2"]->Fill(vbNew2);
            m_hists["hBiasNew3"]->Fill(vbNew3);

            if(cosmic && calib) {
                ((TH2*)m_hists["h2VbNew1DivCosmicCalib"])->Fill(calib,vbNew1DivCosmic);
                ((TH2*)m_hists["h2VbNew2DivCosmicCalib"])->Fill(calib,vbNew2DivCosmic);
                ((TH2*)m_hists["h2VbNew3DivCosmicCalib"])->Fill(calib,vbNew3DivCosmic);
            }
        }
    }
    cout << "################" << endl;
    cout << "Bias 1 Min: " << min_bias1 << ", Max: " << max_bias1 << endl;
    cout << "Bias 2 Min: " << min_bias2 << ", Max: " << max_bias2 << endl;
    cout << "Bias 3 Min: " << min_bias3 << ", Max: " << max_bias3 << endl;

    cout << "VbDivCosmic 1 Min: " << min_vbDivCosmic1 << ", Max: " << max_vbDivCosmic1 << endl;
    cout << "VbDivCosmic 2 Min: " << min_vbDivCosmic2 << ", Max: " << max_vbDivCosmic2 << endl;
    cout << "VbDivCosmic 3 Min: " << min_vbDivCosmic3 << ", Max: " << max_vbDivCosmic3 << endl;

    setEMCalDim(m_hists["h2Bias"]);
    setEMCalDim(m_hists["h2BiasNew1"]);
    setEMCalDim(m_hists["h2BiasNew2"]);
    setEMCalDim(m_hists["h2BiasNew3"]);
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

    m_hists["h2Bias"]->SetMinimum((Int_t)min_bias1);
    m_hists["h2Bias"]->SetMaximum((Int_t)max_bias1+1);

    m_hists["h2Bias"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2Bias"]->GetName()) + "-1.png").c_str());

    m_hists["h2Bias"]->SetMinimum((Int_t)min_bias2);
    m_hists["h2Bias"]->SetMaximum((Int_t)max_bias2+1);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2Bias"]->GetName()) + "-2.png").c_str());

    m_hists["h2Bias"]->SetMinimum((Int_t)min_bias3);
    m_hists["h2Bias"]->SetMaximum((Int_t)max_bias3+1);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2Bias"]->GetName()) + "-3.png").c_str());

    // ---------------------------

    m_hists["h2BiasNew1"]->SetMinimum((Int_t)min_bias1);
    m_hists["h2BiasNew1"]->SetMaximum((Int_t)max_bias1+1);

    m_hists["h2BiasNew1"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2BiasNew1"]->GetName()) + ".png").c_str());

    // ---------------------------

    m_hists["h2BiasNew2"]->SetMinimum((Int_t)min_bias2);
    m_hists["h2BiasNew2"]->SetMaximum((Int_t)max_bias2+1);

    m_hists["h2BiasNew2"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2BiasNew2"]->GetName()) + ".png").c_str());

    // ---------------------------

    m_hists["h2BiasNew3"]->SetMinimum((Int_t)min_bias3);
    m_hists["h2BiasNew3"]->SetMaximum((Int_t)max_bias3+1);

    m_hists["h2BiasNew3"]->Draw("COLZ");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2BiasNew3"]->GetName()) + ".png").c_str());

    // ---------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetLogy();
    gPad->SetGrid(0,0);

    m_hists["hBias"]->Rebin(2);
    m_hists["hBiasNew1"]->Rebin(2);

    m_hists["hBiasNew1"]->SetLineColor(kRed);

    m_hists["hBiasNew1"]->Draw();
    m_hists["hBias"]->Draw("same");

    m_hists["hBiasNew1"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hBiasNew1"]->GetXaxis()->SetTitleOffset(1);

    TLegend* leg = new TLegend(0.7,.75,0.85,.85);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hBias"],"Default","f");
    leg->AddEntry(m_hists["hBiasNew1"],"After","f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hBiasNew1"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["hBiasNew2"]->Rebin(2);
    m_hists["hBiasNew2"]->SetLineColor(kRed);

    m_hists["hBiasNew2"]->Draw();
    m_hists["hBias"]->Draw("same");

    m_hists["hBiasNew2"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hBiasNew2"]->GetXaxis()->SetTitleOffset(1);

    leg = new TLegend(0.7,.75,0.85,.85);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hBias"],"Default","f");
    leg->AddEntry(m_hists["hBiasNew2"],"After","f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hBiasNew2"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["hBiasNew3"]->Rebin(2);
    m_hists["hBiasNew3"]->SetLineColor(kRed);

    m_hists["hBiasNew3"]->Draw();
    m_hists["hBias"]->Draw("same");

    m_hists["hBiasNew3"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hBiasNew3"]->GetXaxis()->SetTitleOffset(1);

    leg = new TLegend(0.7,.75,0.85,.85);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hBias"],"Default","f");
    leg->AddEntry(m_hists["hBiasNew3"],"After","f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["hBiasNew3"]->GetName()) + ".png").c_str());

    // ---------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.16);
    c1->SetRightMargin(.14);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetLogy(0);

    m_hists["h2VbDivCosmicCalib"]->SetMaximum(600);
    m_hists["h2VbDivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2VbDivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    m_hists["h2VbDivCosmicCalib_px"]->SetLineColor(kRed);
    m_hists["h2VbDivCosmicCalib_px"]->SetMarkerColor(kRed);
    m_hists["h2VbDivCosmicCalib_px"]->Draw("same");

    m_hists["h2VbDivCosmicCalib"]->GetYaxis()->SetRangeUser(0,1.2);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2VbDivCosmicCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2VbNew1DivCosmicCalib"]->SetMaximum(600);
    m_hists["h2VbNew1DivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2VbNew1DivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2VbNew1DivCosmicCalib_px = ((TH2*)m_hists["h2VbNew1DivCosmicCalib"])->ProfileX();
    h2VbNew1DivCosmicCalib_px->SetLineColor(kRed);
    h2VbNew1DivCosmicCalib_px->SetMarkerColor(kRed);
    h2VbNew1DivCosmicCalib_px->Draw("same");

    m_hists["h2VbNew1DivCosmicCalib"]->GetYaxis()->SetRangeUser(0,1.2);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2VbNew1DivCosmicCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2VbNew2DivCosmicCalib"]->SetMaximum(600);
    m_hists["h2VbNew2DivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2VbNew2DivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2VbNew2DivCosmicCalib_px = ((TH2*)m_hists["h2VbNew2DivCosmicCalib"])->ProfileX();
    h2VbNew2DivCosmicCalib_px->SetLineColor(kRed);
    h2VbNew2DivCosmicCalib_px->SetMarkerColor(kRed);
    h2VbNew2DivCosmicCalib_px->Draw("same");

    m_hists["h2VbNew2DivCosmicCalib"]->GetYaxis()->SetRangeUser(0,1.2);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2VbNew2DivCosmicCalib"]->GetName()) + ".png").c_str());

    // ---------------------------------

    m_hists["h2VbNew3DivCosmicCalib"]->Draw("COLZ1");
    m_hists["h2VbNew3DivCosmicCalib"]->GetXaxis()->SetTitleOffset(1);

    TH1* h2VbNew3DivCosmicCalib_px = ((TH2*)m_hists["h2VbNew3DivCosmicCalib"])->ProfileX();
    h2VbNew3DivCosmicCalib_px->SetLineColor(kRed);
    h2VbNew3DivCosmicCalib_px->SetMarkerColor(kRed);
    h2VbNew3DivCosmicCalib_px->Draw("same");

    m_hists["h2VbNew3DivCosmicCalib"]->GetYaxis()->SetRangeUser(0,1.2);

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(m_hists["h2VbNew3DivCosmicCalib"]->GetName()) + ".png").c_str());

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

    name << "h2VbDivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2VbDivCosmicCalibA_py = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProjectionY("h2VbDivCosmicCalib_py", bin_start, bin_end);
    TH1* h2VbDivCosmicCalibB_py = ((TH2*)m_hists["h2VbDivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2VbDivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbDivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2VbDivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbDivCosmicCalibB_py->SetLineColor(kRed);

    h2VbDivCosmicCalibB_py->Draw();
    h2VbDivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2VbDivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2VbDivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.7,0.6,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbDivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbDivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2VbDivCosmicCalibA_py->GetName()) + ".png").c_str());

    // ---------------------------------

    name.str("");

    name << "h2VbNew1DivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2VbNew1DivCosmicCalibA_py = ((TH2*)m_hists["h2VbNew1DivCosmicCalib"])->ProjectionY("h2VbNew1DivCosmicCalib_py", bin_start, bin_end);
    TH1* h2VbNew1DivCosmicCalibB_py = ((TH2*)m_hists["h2VbNew1DivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2VbNew1DivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbNew1DivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2VbNew1DivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbNew1DivCosmicCalibB_py->SetLineColor(kRed);

    h2VbNew1DivCosmicCalibB_py->Draw();
    h2VbNew1DivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2VbNew1DivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2VbNew1DivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.7,0.6,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbNew1DivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbNew1DivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2VbNew1DivCosmicCalibA_py->GetName()) + ".png").c_str());

    // ---------------------------------

    name.str("");

    name << "h2VbNew2DivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2VbNew2DivCosmicCalibA_py = ((TH2*)m_hists["h2VbNew2DivCosmicCalib"])->ProjectionY("h2VbNew2DivCosmicCalib_py", bin_start, bin_end);
    TH1* h2VbNew2DivCosmicCalibB_py = ((TH2*)m_hists["h2VbNew2DivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2VbNew2DivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbNew2DivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2VbNew2DivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbNew2DivCosmicCalibB_py->SetLineColor(kRed);

    h2VbNew2DivCosmicCalibB_py->Draw();
    h2VbNew2DivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2VbNew2DivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2VbNew2DivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.7,0.6,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbNew2DivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbNew2DivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2VbNew2DivCosmicCalibA_py->GetName()) + ".png").c_str());

    // ---------------------------------

    name.str("");

    name << "h2VbNew3DivCosmicCalib_Delta_" << delta << "_py";

    TH1* h2VbNew3DivCosmicCalibA_py = ((TH2*)m_hists["h2VbNew3DivCosmicCalib"])->ProjectionY("h2VbNew3DivCosmicCalib_py", bin_start, bin_end);
    TH1* h2VbNew3DivCosmicCalibB_py = ((TH2*)m_hists["h2VbNew3DivCosmicCalib"])->ProjectionY(name.str().c_str(), 1, bin_start-1);

    h2VbNew3DivCosmicCalibB_py->GetYaxis()->SetTitle("Counts");
    h2VbNew3DivCosmicCalibB_py->GetYaxis()->SetRangeUser(5e-1,1e4);
    h2VbNew3DivCosmicCalibB_py->GetXaxis()->SetTitleOffset(1);
    h2VbNew3DivCosmicCalibB_py->SetLineColor(kRed);

    h2VbNew3DivCosmicCalibB_py->Draw();
    h2VbNew3DivCosmicCalibA_py->Draw("same");

    meanA = (Int_t)(h2VbNew3DivCosmicCalibA_py->GetMean()*100)/100.;
    meanB = (Int_t)(h2VbNew3DivCosmicCalibB_py->GetMean()*100)/100.;

    legA.str("");
    legB.str("");

    legB << "0 < Calibration < " << 2-delta << ", #mu = " << meanB << " [V]";
    legA << 2-delta << " #leq Calibration < " << 2+delta << ", #mu = " << meanA << " [V]";

    leg = new TLegend(0.4,.7,0.6,.8);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(h2VbNew3DivCosmicCalibA_py,legA.str().c_str(),"f");
    leg->AddEntry(h2VbNew3DivCosmicCalibB_py,legB.str().c_str(),"f");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    c1->Print((outputDir + "/images/" + string(h2VbNew3DivCosmicCalibA_py->GetName()) + ".png").c_str());

    c1->Print((output + "]").c_str(), "pdf portrait");
    file->Close();
}

void update_Bias(const string &input_hists,
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
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./read-Bias input_hists [output] [outputRoot]" << endl;
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

    update_Bias(argv[1], output, outputRoot);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
