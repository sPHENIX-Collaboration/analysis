// -- c++ includes --
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <stdexcept> // For exception handling

// -- root includes --
#include <TFile.h>
#include <TF1.h>
#include <TFitResult.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLine.h>
#include <TProfile.h>
#include <TPaveStats.h>

// -- common utils
#include "myUtils.C"

// -- sPHENIX Style
#include <sPhenixStyle.C>

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
using std::unordered_set;
using std::map;
namespace fs = std::filesystem;

namespace myAnalysis {
    Int_t analyze();
    void make_plots(const string &outputDir);
    Int_t readHists(const string &input);
    void initHists();
    void saveHists(const string &outputDir);

    map<string,TH1*> m_hists;

    Double_t targetSaturation = 45; // GeV
    Double_t kFactor  = 4.42e-4; // Units: [1 / mV]
    Int_t m_saturation_ADC = 0b11111111111111; // 2^14-1

    // Temperature Info
    Double_t m_gain_per_celsius = 0.015; // gain (in percentage) per degree celsius
    Int_t m_temperature_diff = 7; // Between 2024 and 2025, there is a 7C diff
    Double_t m_gain_temp_coef = 1-m_temperature_diff*m_gain_per_celsius;

    // voltage info
    Double_t m_reference_voltage_default = 66.5 * 1e3; // mV
    Double_t m_reference_voltage_new     = 64.5 * 1e3; // mV
    Double_t m_reference_voltage_shift   = m_reference_voltage_default - m_reference_voltage_new; // mV

    // offset info
    Int_t offset_max = 2400; // mV

    Int_t m_bins_gain = 40;
    Double_t m_gain_low = 0;
    Double_t m_gain_high = 2;

    Int_t m_bins_offset = 62;
    Double_t m_offset_low = -3.2e3; // mV
    Double_t m_offset_high = 3e3; // mV

    Int_t m_bins_deltaOffset = 38;
    Double_t m_deltaOffset_low = -2.8e3; // mV
    Double_t m_deltaOffset_high = 1e3; // mV

    Int_t m_bins_saturate = 800;
    Double_t m_saturate_low  = 0;  // GeV
    Double_t m_saturate_high = 80; // GeV

    unordered_set<string> m_knownBadIB = {"4.1","10.3","10.5","25.2","50.1","54.4"}; // sector.IB

    Bool_t m_saveFig = true;
}

void myAnalysis::initHists() {

    m_hists["h2Saturate"] = static_cast<TH2*>(m_hists["h2Calib"]->Clone("h2Saturate"));
    m_hists["h2Saturate"]->Scale(m_saturation_ADC/1e3);
    m_hists["h2Saturate"]->SetTitle("EMCal Saturation [GeV]");

    m_hists["hSaturate"] = new TH1F("hSaturate","EMCal Saturation; Energy [GeV]; Counts", m_bins_saturate, m_saturate_low, m_saturate_high);

    m_hists["h2SaturateV2"] = static_cast<TH2*>(m_hists["h2Saturate"]->Clone("h2SaturateV2"));
    m_hists["h2SaturateV2"]->Scale(m_gain_temp_coef);

    m_hists["hSaturateV2"] = new TH1F("hSaturateV2","EMCal Saturation; Energy [GeV]; Counts", m_bins_saturate, m_saturate_low, m_saturate_high);

    m_hists["h2GainFactor"] = static_cast<TH2*>(m_hists["h2SaturateV2"]->Clone("h2GainFactor"));
    m_hists["h2GainFactor"]->Scale(1/targetSaturation);
    m_hists["h2GainFactor"]->SetTitle("Gain: Target Saturation 45 GeV");

    m_hists["h2GainFactorV2"] = new TH2F("h2GainFactorV2","Gain: Best; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    m_hists["hGainFactor"] = new TH1F("hGainFactor","Gain: Target Saturation = 45 GeV; Gain; Counts", m_bins_gain, m_gain_low, m_gain_high);
    m_hists["hGainFactorV2"] = new TH1F("hGainFactorV2","Gain: Best; Gain; Counts", m_bins_gain, m_gain_low, m_gain_high);

    m_hists["h2SaturateV3"] = new TH2F("h2SaturateV3","EMCal Saturation [GeV]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);
    m_hists["hSaturateV3"] = new TH1F("hSaturateV3","EMCal Saturation; Energy [GeV]; Counts", m_bins_saturate, m_saturate_low, m_saturate_high);

    m_hists["hSaturateV3_vs_V2"] = new TH2F("hSaturateV3_vs_V2","EMCal Saturation; Current [GeV]; New [GeV]", m_bins_saturate, m_saturate_low, m_saturate_high, m_bins_saturate, m_saturate_low, m_saturate_high);

    m_hists["h2OffsetNew"] = new TH2F("h2OffsetNew","New Offset [mV] [Clamp]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);
    m_hists["h2OffsetNewV2"] = new TH2F("h2OffsetNewV2","New Offset [mV] [No Clamp]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    m_hists["h2DeltaOffset"] = new TH2F("h2DeltaOffset","#Delta Offset [mV] [Clamp]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);
    m_hists["h2DeltaOffsetV2"] = new TH2F("h2DeltaOffsetV2","#Delta Offset [mV] [No Clamp]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    m_hists["hOffset"] = new TH1F("hOffset","Bias Offset: Original; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hOffsetNew"] = new TH1F("hOffsetNew","Bias Offset: Clamp; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hOffsetNewV2"] = new TH1F("hOffsetNewV2","Bias Offset: No Clamp; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);

    // track errors
    m_hists["hSaturate"]->Sumw2();
    m_hists["hSaturateV2"]->Sumw2();
    m_hists["hSaturateV3"]->Sumw2();
    m_hists["hGainFactor"]->Sumw2();
    m_hists["hGainFactorV2"]->Sumw2();
    m_hists["hOffset"]->Sumw2();
    m_hists["hOffsetNew"]->Sumw2();
    m_hists["hOffsetNewV2"]->Sumw2();
}

Int_t myAnalysis::readHists(const string &input) {
    // Read Hist from input
    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }

    m_hists["h2Offset"] = static_cast<TH2*>(tfile->Get("h2Offset"));
    m_hists["h2DummySector"] = static_cast<TH2*>(tfile->Get("h2DummySector"));
    m_hists["h2DummyIB"] = static_cast<TH2*>(tfile->Get("h2DummyIB"));
    m_hists["h2Calib"] = static_cast<TH2*>(tfile->Get("h2Calib"));
    m_hists["hCalib"] = static_cast<TH3*>(tfile->Get("h3CalibOffsetCosmicMPV")->Clone("hCalib"))->Project3D("z");

    tfile->Close();
    return 0;
}

Int_t myAnalysis::analyze() {

    for(Int_t i = 1; i <= myUtils::m_nphi; ++i) {
        for(Int_t j = 1; j <= myUtils::m_neta; ++j) {
            Double_t saturate = static_cast<TH2*>(m_hists["h2Saturate"])->GetBinContent(i, j);
            Double_t saturateV2 = static_cast<TH2*>(m_hists["h2SaturateV2"])->GetBinContent(i, j);
            Double_t gain = static_cast<TH2*>(m_hists["h2GainFactor"])->GetBinContent(i, j);
            Double_t offset = static_cast<TH2*>(m_hists["h2Offset"])->GetBinContent(i, j);

            pair<Int_t, Int_t> sectorIB_pair = myUtils::getSectorIB(i-1,j-1);
            string sectorIB = to_string(sectorIB_pair.first) + "." + to_string(sectorIB_pair.second);
            Bool_t isGoodIB = !(m_knownBadIB.contains(sectorIB));

            if(isGoodIB && saturate) {
                m_hists["hOffset"]->Fill(offset);
                m_hists["hSaturate"]->Fill(saturate);
                m_hists["hSaturateV2"]->Fill(saturateV2);

                // with clamp
                Double_t gainV2 = std::max(gain, TMath::Exp(-kFactor*(offset_max+m_reference_voltage_shift+offset))); // ensure the new offset >= -2400 mV
                gainV2 = std::min(gainV2, TMath::Exp(kFactor*(offset_max-m_reference_voltage_shift-offset)));         // ensure the new offset <= -2400 mV

                Double_t deltaOffset = TMath::Log(gainV2) / kFactor;
                Double_t offsetNew = offset + deltaOffset + m_reference_voltage_shift;
                Double_t saturateV3 = saturateV2 / gainV2;

                static_cast<TH2*>(m_hists["h2DeltaOffset"])->SetBinContent(i, j, deltaOffset);
                static_cast<TH2*>(m_hists["h2OffsetNew"])->SetBinContent(i, j, offsetNew);
                m_hists["hOffsetNew"]->Fill(offsetNew);
                m_hists["hGainFactorV2"]->Fill(gainV2);
                m_hists["hSaturateV3"]->Fill(saturateV3);
                static_cast<TH2*>(m_hists["h2GainFactorV2"])->SetBinContent(i, j, gainV2);
                static_cast<TH2*>(m_hists["h2SaturateV3"])->SetBinContent(i, j, saturateV3);
                static_cast<TH2*>(m_hists["hSaturateV3_vs_V2"])->Fill(saturateV2, saturateV3);

                // no clamp
                Double_t deltaOffsetV2 = TMath::Log(gain) / kFactor;
                Double_t offsetNewV2 = offset + deltaOffsetV2 + m_reference_voltage_shift;

                static_cast<TH2*>(m_hists["h2DeltaOffsetV2"])->SetBinContent(i, j, deltaOffsetV2);
                static_cast<TH2*>(m_hists["h2OffsetNewV2"])->SetBinContent(i, j, offsetNewV2);
                m_hists["hOffsetNewV2"]->Fill(offsetNewV2);
                m_hists["hGainFactor"]->Fill(gain);
            }
            else {
                static_cast<TH2*>(m_hists["h2Saturate"])->SetBinContent(i, j, 0);
                static_cast<TH2*>(m_hists["h2SaturateV2"])->SetBinContent(i, j, 0);
                static_cast<TH2*>(m_hists["h2GainFactor"])->SetBinContent(i, j, 0);
            }
        }
    }

    // compute the average bias offset per IB
    // this is for the offsets without any clamping
    m_hists["h2OffsetNewV2_avg"] = static_cast<TH2*>(m_hists["h2OffsetNewV2"]->Clone("h2OffsetNewV2_avg"));
    static_cast<TH2*>(m_hists["h2OffsetNewV2_avg"])->Rebin2D(8,8);
    m_hists["h2OffsetNewV2_avg"]->Scale(1./myUtils::m_nchannel_per_ib);

    cout << "New Saturation [44,46] GeV: " << m_hists["hSaturateV3"]->Integral(m_hists["hSaturateV3"]->FindBin(44), m_hists["hSaturateV3"]->FindBin(46)-1)*100./m_hists["hSaturateV3"]->Integral() << " %" << endl;
    cout << "New Saturation [40,46] GeV: " << m_hists["hSaturateV3"]->Integral(m_hists["hSaturateV3"]->FindBin(40), m_hists["hSaturateV3"]->FindBin(46)-1)*100./m_hists["hSaturateV3"]->Integral() << " %" << endl;

    return 0;
}

void myAnalysis::saveHists(const string &outputDir) {
    // save plots to root file
    string output = outputDir + "/updated-Offset-info-v2.root";
    TFile tf(output.c_str(),"recreate");
    tf.cd();

    for (const auto &[name, hist] : m_hists) {
        hist->Write();
    }

    tf.Close();
}

void myAnalysis::make_plots(const string &outputDir) {
    string output = outputDir + "/plots.pdf";

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08f);
    gStyle->SetTitleFillColor(0);
    gStyle->SetTitleBorderSize(0);

    gPad->SetGrid();

    for (const auto &[name, hist] : m_hists) {
        if(name.starts_with("h2")) myUtils::setEMCalDim(hist);
    }

    c1->Print((output + "[").c_str(), "pdf portrait");

    m_hists["h2Saturate"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2Saturate.png").c_str());

    m_hists["h2Saturate"]->SetMinimum(10);
    m_hists["h2Saturate"]->SetMaximum(30);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2Saturate-zoom.png").c_str());

    // ----------------------------------------------

    m_hists["h2SaturateV2"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV2.png").c_str());

    m_hists["h2SaturateV2"]->SetMinimum(10);
    m_hists["h2SaturateV2"]->SetMaximum(30);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV2-zoom.png").c_str());

    m_hists["h2SaturateV2"]->SetMaximum(50);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV2-zoom-v2.png").c_str());

    // ----------------------------------------------

    m_hists["h2SaturateV3"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV3.png").c_str());

    m_hists["h2SaturateV3"]->SetMinimum(10);
    m_hists["h2SaturateV3"]->SetMaximum(50);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV3-zoom.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    m_hists["hSaturate"]->Draw("hist e");
    m_hists["hSaturate"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hSaturate"]->GetYaxis()->SetRangeUser(0,3e3);
    m_hists["hSaturate"]->Rebin(10);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturate.png").c_str());

    m_hists["hSaturateV2"]->Draw("hist e same");
    m_hists["hSaturateV2"]->Rebin(10);
    m_hists["hSaturateV2"]->SetLineColor(kRed);
    m_hists["hSaturateV2"]->SetMarkerColor(kRed);

    stringstream legA, legB;

    legA << "Before Temperature Correction";
    legB << "After Temperature Correction";

    Double_t xshift = 0.25;
    Double_t yshift = 0.02;

    auto leg = new TLegend(0.2+xshift,.75+yshift,0.65+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04f);
    leg->AddEntry(m_hists["hSaturate"],legA.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hSaturateV2"],legB.str().c_str(),"lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturate-overlay.png").c_str());

    m_hists["hSaturateV3"]->Draw("hist e same");
    m_hists["hSaturate"]->GetYaxis()->SetRangeUser(0,1.3e4);
    m_hists["hSaturateV3"]->Rebin(10);
    m_hists["hSaturateV3"]->SetLineColor(kBlue);
    m_hists["hSaturateV3"]->SetMarkerColor(kBlue);

    stringstream legC;

    legC << "After Adjusting Gain (Expected)";

    xshift = -0.05;

    leg->SetX1NDC(0.2+xshift);
    leg->SetX2NDC(0.4+xshift);
    leg->SetY1NDC(0.65+xshift);
    leg->SetY2NDC(0.85+xshift);

    leg->AddEntry(m_hists["hSaturateV3"],legC.str().c_str(),"lpe");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturate-overlay-v2.png").c_str());

    m_hists["hSaturate"]->GetYaxis()->SetRangeUser(0,3e3);

    leg->SetTextSize(0.03f);
    xshift = 0.42;
    yshift = 0;

    leg->SetX1NDC(0.2+xshift);
    leg->SetX2NDC(0.3+xshift);
    leg->SetY1NDC(0.65+yshift);
    leg->SetY2NDC(0.85+yshift);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturate-overlay-v2-zoom.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.1f);
    c1->SetRightMargin(.13f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gPad->SetLogz();

    m_hists["hSaturateV3_vs_V2"]->Draw("COLZ1");
    m_hists["hSaturateV3_vs_V2"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hSaturateV3_vs_V2"]->GetYaxis()->SetTitleOffset(0.9f);

    static_cast<TH2*>(m_hists["hSaturateV3_vs_V2"])->Rebin2D(10,10);

    m_hists["hSaturateV3_vs_V2"]->GetXaxis()->SetRangeUser(10,50);
    m_hists["hSaturateV3_vs_V2"]->GetYaxis()->SetRangeUser(10,50);

    TF1* tf = new TF1("tf","x",10,50);
    tf->SetLineStyle(kDashed);
    tf->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturateV3_vs_V2.png").c_str());

    gPad->SetLogz(0);

    // ----------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    m_hists["h2GainFactor"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor.png").c_str());

    m_hists["h2GainFactor"]->SetMinimum(0.2);
    m_hists["h2GainFactor"]->SetMaximum(1.2);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor-zoom.png").c_str());

    // ----------------------------------------------

    m_hists["h2GainFactorV2"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactorV2.png").c_str());

    m_hists["h2GainFactorV2"]->SetMinimum(0.2);
    m_hists["h2GainFactorV2"]->SetMaximum(1.2);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactorV2-zoom.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gPad->SetLogy();

    m_hists["hGainFactor"]->Draw("hist e");
    m_hists["hGainFactor"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hGainFactor"]->GetXaxis()->SetRangeUser(0,1.6);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hGainFactor.png").c_str());

    m_hists["hGainFactorV2"]->Draw("hist e same");
    m_hists["hGainFactorV2"]->SetLineColor(kRed);
    m_hists["hGainFactorV2"]->SetMarkerColor(kRed);

    m_hists["hGainFactor"]->SetTitle("Gain");

    legA.str("Target Saturation = 45 GeV");
    legB.str("Best given Voltage Limits");

    xshift = 0.18;
    yshift = -0.02;

    leg = new TLegend(0.25+xshift,.75+yshift,0.65+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05f);
    leg->AddEntry(m_hists["hGainFactor"],legA.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hGainFactorV2"],legB.str().c_str(),"lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hGainFactor-overlay.png").c_str());

    gPad->SetLogy(0);

    // ----------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    m_hists["h2DeltaOffset"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2DeltaOffset.png").c_str());

    // ----------------------------------------------

    m_hists["h2Offset"]->Draw("COLZ1");
    m_hists["h2Offset"]->SetTitle("Offset [mV]: Reference = -66.5 V");

    m_hists["h2Offset"]->SetMinimum(-2.4e3);
    m_hists["h2Offset"]->SetMaximum(2.4e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2Offset.png").c_str());

    // ----------------------------------------------

    m_hists["h2OffsetNew"]->Draw("COLZ1");
    m_hists["h2OffsetNew"]->SetTitle("Offset New [mV]: Reference = -64.5 V");

    m_hists["h2OffsetNew"]->SetMinimum(-2.4e3);
    m_hists["h2OffsetNew"]->SetMaximum(2.4e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2OffsetNew.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gPad->SetGrid(0,0);

    m_hists["hOffset"]->Draw("hist e");
    m_hists["hOffset"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hOffset"]->GetYaxis()->SetRangeUser(0,4e3);

    m_hists["hOffsetNew"]->Draw("hist e same");
    m_hists["hOffsetNew"]->SetLineColor(kRed);
    m_hists["hOffsetNew"]->SetMarkerColor(kRed);

    legA.str("Original: Reference = -66.5 V");
    legB.str("New [with Clamp]: Reference = -64.5 V");
    legC.str("New [without Clamp]: Reference = -64.5 V");

    xshift = -0.05;
    yshift = 0.02;

    leg = new TLegend(0.2+xshift,.7+yshift,0.65+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.05f);
    leg->AddEntry(m_hists["hOffset"],legA.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hOffsetNew"],legB.str().c_str(),"lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hOffset-overlay.png").c_str());

    m_hists["hOffsetNewV2"]->Draw("hist e same");
    m_hists["hOffsetNewV2"]->SetLineColor(kBlue);
    m_hists["hOffsetNewV2"]->SetMarkerColor(kBlue);

    leg->AddEntry(m_hists["hOffsetNewV2"],legC.str().c_str(),"lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hOffset-overlay-v2.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    gPad->SetGrid();

    m_hists["h2DeltaOffsetV2"]->Draw("COLZ1");
    m_hists["h2DeltaOffsetV2"]->SetTitle("#Delta Offset [mV]: Target Saturation 45 GeV");

    m_hists["h2DeltaOffsetV2"]->SetMaximum(1e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2DeltaOffsetV2.png").c_str());

    // ----------------------------------------------

    m_hists["h2OffsetNewV2"]->Draw("COLZ1");
    m_hists["h2OffsetNewV2"]->SetTitle("New Offset [mV]: Target Saturation 45 GeV");

    m_hists["h2OffsetNewV2"]->SetMaximum(1e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2OffsetNewV2.png").c_str());

    // ----------------------------------------------

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void write_Biasv4(const string &input,
                  const string &outputDir=".",
                  const string &plotDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "plotDir: " << plotDir << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // create output directories
    fs::create_directories(outputDir);
    fs::create_directories(plotDir);
    fs::create_directories(plotDir+"/images");

    TH1::AddDirectory(kFALSE);

    if(myAnalysis::readHists(input)) return;
    myAnalysis::initHists();
    myAnalysis::analyze();
    myAnalysis::saveHists(outputDir);
    myAnalysis::make_plots(plotDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./write-Biasv4 input [outputDir] [plotDir]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        cout << "plotDir: plot directory" << endl;
        return 1;
    }

    string outputDir = ".";
    string plotDir = ".";

    if(argc >= 3) {
        outputDir = argv[2];
    }
    if(argc >= 4) {
        plotDir = argv[3];
    }

    write_Biasv4(argv[1], outputDir, plotDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
