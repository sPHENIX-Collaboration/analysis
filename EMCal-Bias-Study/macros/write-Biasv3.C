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
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TProfile.h>

// -- common utils
#include "myUtils.C"

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
    Int_t analyze(const string &input, const string &outputDir);
    void make_plots(const string &outputDir);
    void fillGaps(UInt_t groupSize);

    unordered_map<string,TH1*> m_hists;

    string cosmicHistName = "h2CosmicMPV";
    string offsetHistName = "h2Offset";

    Double_t cosmicMPVMax = 750;
    Double_t targetMPV = 336.7;
    Double_t kFactor  = 4.42e-4; // Units: [1 / mV]

    UInt_t m_bins_gain = 50;
    Double_t m_gain_low = 0;
    Double_t m_gain_high = 25;

    UInt_t m_bins_offset = 210;
    Double_t m_offset_low = -3.5e3;
    Double_t m_offset_high = 7e3;

    UInt_t m_bins_mpv = 220;
    Double_t m_mpv_low = -800;
    Double_t m_mpv_high = 1400;

    unordered_map<Int_t, string> m_fiberTypeMap = {
                                                 {1,"SG47"},
                                                 {2,"K"},
                                                 {3,"SG"},
                                                 {4,"SG-B"},
                                                 {5,"P-SG"},
                                                 {6,"I-K"},
                                                 {7,"PSG+IK+K"}
    };

    unordered_map<string, Int_t> m_myFiberTypeMap = {
                                                 {"SG47",0},
                                                 {"K",6},
                                                 {"SG",3},
                                                 {"SG-B",2},
                                                 {"P-SG",4},
                                                 {"I-K",5},
                                                 {"PSG+IK+K",1}
    };

    Bool_t m_saveFig = true;
}

void myAnalysis::fillGaps(UInt_t groupSize) {
    // compute averge block MPV and fill in the empty spots
    for(UInt_t i = 0; i < myUtils::m_neta / groupSize; ++i) {
        for(UInt_t j = 0; j < myUtils::m_nphi / groupSize; ++j) {
            Double_t avgMPV = 0;
            UInt_t tower_ctr = 0;

            UInt_t lowEta = i*groupSize;
            UInt_t lowPhi = j*groupSize;

            for(UInt_t k = lowEta; k < lowEta+groupSize; ++k) {
                for(UInt_t l = lowPhi; l < lowPhi+groupSize; ++l) {
                    Double_t mpv = static_cast<TH2*>(m_hists[cosmicHistName])->GetBinContent(l+1,k+1);
                    if(mpv > 0 && mpv < cosmicMPVMax) {
                      avgMPV += mpv;
                      ++tower_ctr;
                    }
                }
            }

            if(!tower_ctr) {
                cout << "ERROR: Block contains no cosmic MPV: " << lowPhi << ", " << lowEta << endl;
                continue;
            }

            // compute average MPV
            avgMPV /= tower_ctr;

            for(UInt_t k = lowEta; k < lowEta+groupSize; ++k) {
                for(UInt_t l = lowPhi; l < lowPhi+groupSize; ++l) {
                    Double_t mpv = static_cast<TH2*>(m_hists[cosmicHistName])->GetBinContent(l+1,k+1);
                    if(mpv <= 0 || mpv >= cosmicMPVMax) {
                        static_cast<TH2*>(m_hists[cosmicHistName])->SetBinContent(l+1,k+1, avgMPV);
                    }
                }
            }
        }
    }
}

Int_t myAnalysis::analyze(const string &input, const string &outputDir) {
    // Read Hist from input
    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists[cosmicHistName] = static_cast<TH2*>(tfile->Get(cosmicHistName.c_str()));

    if (!m_hists[cosmicHistName]) {
      cout << "Error: Histogram '" << cosmicHistName << "' not found in file: " << tfile->GetName() << endl;
      return 1;
    }
    m_hists[offsetHistName] = static_cast<TH2*>(tfile->Get(offsetHistName.c_str()));

    if (!m_hists[offsetHistName]) {
      cout << "Error: Histogram '" << offsetHistName << "' not found in file: " << tfile->GetName() << endl;
      return 1;
    }

    m_hists["h2DummySector"] = static_cast<TH2*>(tfile->Get("h2DummySector"));
    m_hists["h2DummyIB"] = static_cast<TH2*>(tfile->Get("h2DummyIB"));
    m_hists["h2FiberType"] = static_cast<TH2*>(tfile->Get("h2FiberType"));

    tfile->Close();

    string cosmicHistNameV0 = cosmicHistName + "v0";
    m_hists[cosmicHistNameV0] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV0.c_str()));

    // by blocks
    fillGaps(2);

    cout << "Filled in Block Gaps" << endl;

    string cosmicHistNameV1 = cosmicHistName + "v1";
    m_hists[cosmicHistNameV1] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV1.c_str()));

    // by interface boards
    fillGaps(8);

    cout << "Filled in Interface Board Gaps" << endl;

    string cosmicHistNameV2 = cosmicHistName + "v2";
    m_hists[cosmicHistNameV2] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone(cosmicHistNameV2.c_str()));

    m_hists["hCosmicMPVv0"] = new TH1F("hCosmicMPVv0","Cosmic MPV; Cosmic MPV; Counts", m_bins_mpv, m_mpv_low, m_mpv_high);
    m_hists["hCosmicMPVv1"] = new TH1F("hCosmicMPVv1","Cosmic MPV; Cosmic MPV; Counts", m_bins_mpv, m_mpv_low, m_mpv_high);
    m_hists["hCosmicMPVv2"] = new TH1F("hCosmicMPVv2","Cosmic MPV; Cosmic MPV; Counts", m_bins_mpv, m_mpv_low, m_mpv_high);
    m_hists["hGainFactors"] = new TH1F("hGainFactors","Gain Factors; Gain Factor; Counts", m_bins_gain, m_gain_low, m_gain_high);
    m_hists["hOffset"] = new TH1F("hOffset","Bias Offset; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hDeltaOffset"] = new TH1F("hDeltaOffset","#Delta Offset; #Delta Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hDeltaOffsetV2"] = new TH1F("hDeltaOffsetV2","#Delta Offset; #Delta Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hDeltaOffsetV3"] = new TH1F("hDeltaOffsetV3","#Delta Offset; #Delta Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hNewOffset"] = new TH1F("hNewOffset","New Offset; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hNewOffsetV2"] = new TH1F("hNewOffsetV2","New Offset; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hNewOffsetV3"] = new TH1F("hNewOffsetV3","New Offset; Offset [mV]; Counts", m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hCosmicMPVDeltaOffset"] = new TH2F("hCosmicMPVDeltaOffset","Cosmic MPV vs #Delta Offset; #Delta Offset [mV]; Cosmic MPV", m_bins_offset, m_offset_low, m_offset_high, m_bins_mpv, m_mpv_low, m_mpv_high);
    m_hists["hFiberTypeDeltaOffset"] = new TH2F("hFiberTypeDeltaOffset","#Delta Offset vs Fiber Type; Fiber Type; #Delta Offset [mV]", m_fiberTypeMap.size(), 0, m_fiberTypeMap.size(), m_bins_offset, m_offset_low, m_offset_high);
    m_hists["hFiberTypeCosmicMPV"] = new TH2F("hFiberTypeCosmicMPV","Cosmic MPV vs Fiber Type; Fiber Type; Cosmic MPV", m_fiberTypeMap.size(), 0, m_fiberTypeMap.size(), m_bins_mpv, m_mpv_low, m_mpv_high);
    m_hists["hDeltaOffsetGain"] = new TH2F("hDeltaOffsetGain","#Delta Offset vs Gain Factor; Gain Factor; #Delta Offset [mV]", m_bins_gain, m_gain_low, m_gain_high, m_bins_offset, m_offset_low, m_offset_high);

    stringstream title;
    for (const auto &[name, val] : m_myFiberTypeMap) {
        title.str("");
        title << "Cosmic MPV: " << name << "; Cosmic MPV; Counts";
        m_hists[name] = new TH1F(name.c_str(),title.str().c_str(), m_bins_mpv, m_mpv_low, m_mpv_high);

        m_hists["hFiberTypeDeltaOffset"]->GetXaxis()->SetBinLabel(val+1,name.c_str());
        m_hists["hFiberTypeCosmicMPV"]->GetXaxis()->SetBinLabel(val+1,name.c_str());
    }

    m_hists["h2GainFactors"] = static_cast<TH2*>(m_hists[cosmicHistName]->Clone("h2GainFactors"));
    m_hists["h2GainFactors"]->SetTitle("Gain Factors");

    m_hists["h2DeltaOffset"] = static_cast<TH2*>(m_hists[offsetHistName]->Clone("h2DeltaOffset"));
    m_hists["h2DeltaOffset"]->SetTitle("#Delta Offset [mV]");

    // ensuring the max delta offset is 1000 mV
    m_hists["h2DeltaOffsetV2"] = static_cast<TH2*>(m_hists["h2DeltaOffset"]->Clone("h2DeltaOffsetV2"));

    m_hists["h2NewOffset"] = static_cast<TH2*>(m_hists[offsetHistName]->Clone("h2NewOffset"));
    m_hists["h2NewOffset"]->SetTitle("New Offset [mV]");
    m_hists["h2NewOffsetV2"] = static_cast<TH2*>(m_hists["h2NewOffset"]->Clone("h2NewOffsetV2"));

    // ensuring the min offset is -2000 mV
    m_hists["h2DeltaOffsetV3"] = static_cast<TH2*>(m_hists["h2DeltaOffset"]->Clone("h2DeltaOffsetV3"));
    m_hists["h2NewOffsetV3"] = static_cast<TH2*>(m_hists["h2NewOffset"]->Clone("h2NewOffsetV3"));

    for(UInt_t i = 1; i <= myUtils::m_nphi; ++i) {
        for(UInt_t j = 1; j <= myUtils::m_neta; ++j) {
            Double_t mpv_v0 = static_cast<TH2*>(m_hists["h2CosmicMPVv0"])->GetBinContent(i,j);
            Double_t mpv_v1 = static_cast<TH2*>(m_hists["h2CosmicMPVv1"])->GetBinContent(i,j);
            Double_t mpv = static_cast<TH2*>(m_hists[cosmicHistName])->GetBinContent(i,j);
            Double_t offset = static_cast<TH2*>(m_hists[offsetHistName])->GetBinContent(i,j);
            Int_t temp_fiberType = static_cast<TH2*>(m_hists["h2FiberType"])->GetBinContent(i,j);
            string fiberTypeName = m_fiberTypeMap[temp_fiberType];
            Int_t fiberType = m_myFiberTypeMap[fiberTypeName];
            Double_t gain = targetMPV / mpv;

            Double_t deltaOffset = TMath::Log(gain) / kFactor;
            Double_t updateOffset = offset + deltaOffset;

            Double_t deltaOffsetV2 = std::min(deltaOffset, 1000.);
            Double_t updateOffsetV2 = offset + deltaOffsetV2;

            Double_t deltaOffsetV3 = (updateOffsetV2 < -2000) ? -2000 - offset : deltaOffsetV2;
            Double_t updateOffsetV3 = offset + deltaOffsetV3;

            m_hists["hOffset"]->Fill(offset);
            m_hists["hCosmicMPVv0"]->Fill(mpv_v0);
            m_hists["hCosmicMPVv1"]->Fill(mpv_v1);
            m_hists["hCosmicMPVv2"]->Fill(mpv);
            m_hists[fiberTypeName]->Fill(mpv);
            static_cast<TH2*>(m_hists["hCosmicMPVDeltaOffset"])->Fill(deltaOffsetV3, mpv);
            static_cast<TH2*>(m_hists["hFiberTypeDeltaOffset"])->Fill(fiberType, deltaOffsetV2);
            static_cast<TH2*>(m_hists["hFiberTypeCosmicMPV"])->Fill(fiberType, mpv);
            static_cast<TH2*>(m_hists["hDeltaOffsetGain"])->Fill(gain, deltaOffset);

            static_cast<TH2*>(m_hists["h2GainFactors"])->SetBinContent(i, j, gain);
            m_hists["hGainFactors"]->Fill(gain);

            static_cast<TH2*>(m_hists["h2DeltaOffset"])->SetBinContent(i, j, deltaOffset);
            static_cast<TH2*>(m_hists["h2DeltaOffsetV2"])->SetBinContent(i, j, deltaOffsetV2);
            static_cast<TH2*>(m_hists["h2DeltaOffsetV3"])->SetBinContent(i, j, deltaOffsetV3);
            m_hists["hDeltaOffset"]->Fill(deltaOffset);
            m_hists["hDeltaOffsetV2"]->Fill(deltaOffsetV2);
            m_hists["hDeltaOffsetV3"]->Fill(deltaOffsetV3);

            static_cast<TH2*>(m_hists["h2NewOffset"])->SetBinContent(i, j, updateOffset);
            static_cast<TH2*>(m_hists["h2NewOffsetV2"])->SetBinContent(i, j, updateOffsetV2);
            static_cast<TH2*>(m_hists["h2NewOffsetV3"])->SetBinContent(i, j, updateOffsetV3);
            m_hists["hNewOffset"]->Fill(updateOffset);
            m_hists["hNewOffsetV2"]->Fill(updateOffsetV2);
            m_hists["hNewOffsetV3"]->Fill(updateOffsetV3);

            if((i == 153 && j == 12) || (i == 180 && j == 30)){
                cout << "##########################" << endl;
                cout << "EXAMPLE step through tower phi = " << i-1 << " and tower eta = " << j-1 << endl;
                cout << "Cosmic MPV: " << mpv << endl;
                cout << "Gain: " << gain << endl;
                cout << "Default Offset: " << offset << endl;
                cout << "Delta Offset v1: " << deltaOffset  << " mV" << endl;
                cout << "Delta Offset v2: " << deltaOffsetV2 << " mV" << endl;
                cout << "Delta Offset v3: " << deltaOffsetV3 << " mV" << endl;
                cout << "##########################" << endl;
            }
        }
    }

    // save plots to root file
    string output = outputDir + "/updated-Offset-info.root";
    TFile tf(output.c_str(),"recreate");
    tf.cd();

    m_hists[cosmicHistNameV0]->Write();
    m_hists[cosmicHistNameV1]->Write();
    m_hists[cosmicHistNameV2]->Write();
    m_hists["hCosmicMPVv0"]->Write();
    m_hists["hCosmicMPVv1"]->Write();
    m_hists["hCosmicMPVv2"]->Write();
    m_hists["h2GainFactors"]->Write();
    m_hists["hGainFactors"]->Write();
    m_hists[offsetHistName]->Write();
    m_hists["hOffset"]->Write();
    m_hists["h2DeltaOffset"]->Write();
    m_hists["hDeltaOffset"]->Write();
    m_hists["h2NewOffset"]->Write();
    m_hists["hNewOffset"]->Write();
    m_hists["h2DeltaOffsetV2"]->Write();
    m_hists["hDeltaOffsetV2"]->Write();
    m_hists["h2NewOffsetV2"]->Write();
    m_hists["hNewOffsetV2"]->Write();
    m_hists["h2DeltaOffsetV3"]->Write();
    m_hists["hDeltaOffsetV3"]->Write();
    m_hists["h2NewOffsetV3"]->Write();
    m_hists["hNewOffsetV3"]->Write();
    m_hists["hCosmicMPVDeltaOffset"]->Write();
    m_hists["hFiberTypeDeltaOffset"]->Write();
    m_hists["hFiberTypeCosmicMPV"]->Write();
    m_hists["hDeltaOffsetGain"]->Write();

    cout << "####################################" << endl;
    cout << "Average Cosmic MPV by Fiber Type" << endl;
    for (const auto &[val, name] : m_fiberTypeMap) {
        Double_t avg = m_hists[name]->GetMean();
        cout << "Fiber Type: " << name << ", Avg Cosmic MPV: " << avg << endl;
        m_hists[name]->Write();
    }
    cout << "####################################" << endl;

    tf.Close();

    return 0;
}

void myAnalysis::make_plots(const string &outputDir) {
    // set sPHENIX plotting style
    SetsPhenixStyle();

    string output = outputDir + "/plots.pdf";

    TCanvas* c1 = new TCanvas();
    c1->SetTickx();
    c1->SetTicky();

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
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

    for (const auto &[name, hist] : m_hists) {
        if(name.starts_with("h2")) myUtils::setEMCalDim(hist);
    }

    c1->Print((output + "[").c_str(), "pdf portrait");

    m_hists["h2CosmicMPVv0"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2CosmicMPVv0.png").c_str());

    m_hists["h2CosmicMPVv0"]->SetMinimum(0);
    m_hists["h2CosmicMPVv0"]->SetMaximum(750);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2CosmicMPVv0-zoom.png").c_str());

    // ----------------------------------------------

    m_hists["h2CosmicMPVv1"]->Draw("COLZ1");

    m_hists["h2CosmicMPVv1"]->SetMinimum(0);
    m_hists["h2CosmicMPVv1"]->SetMaximum(750);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2CosmicMPVv1.png").c_str());

    // ----------------------------------------------

    m_hists["h2CosmicMPVv2"]->Draw("COLZ1");

    m_hists["h2CosmicMPVv2"]->SetMinimum(0);
    m_hists["h2CosmicMPVv2"]->SetMaximum(750);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2CosmicMPVv2.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gPad->SetLogy();

    m_hists["hCosmicMPVv2"]->SetLineColor(kRed);

    m_hists["hCosmicMPVv0"]->Draw();
    m_hists["hCosmicMPVv2"]->Draw("same");

    stringstream legA, legB;

    legA << "Original";
    legB << "Updated";

    Double_t xshift = 0.5;
    Double_t yshift = 0;

    auto leg = new TLegend(0.2+xshift,.75+yshift,0.65+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hCosmicMPVv0"],legA.str().c_str(),"l");
    leg->AddEntry(m_hists["hCosmicMPVv2"],legB.str().c_str(),"l");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hCosmicMPV.png").c_str());

    m_hists["hCosmicMPVv0"]->GetXaxis()->SetRangeUser(0,800);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hCosmicMPV-zoom.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetLogy(0);

    m_hists["h2GainFactors"]->Draw("COLZ1");

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactors.png").c_str());

    m_hists["h2GainFactors"]->SetMinimum(0);
    m_hists["h2GainFactors"]->SetMaximum(2);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactors-zoom-0.png").c_str());

    m_hists["h2GainFactors"]->SetMinimum(1.56);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactors-zoom-1.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gPad->SetLogy();

    m_hists["hGainFactors"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hGainFactors.png").c_str());

    m_hists["hGainFactors"]->GetXaxis()->SetRangeUser(0,6);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hGainFactors-zoom.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gPad->SetLogy(0);

    m_hists["h2Offset"]->Draw("COLZ1");

    m_hists["h2Offset"]->SetMinimum(-2e3);
    m_hists["h2Offset"]->SetMaximum(1.8e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2Offset.png").c_str());

    // ----------------------------------------------

    m_hists["h2DeltaOffset"]->Draw("COLZ1");

    // m_hists["h2Offset"]->SetMinimum(0);
    // m_hists["h2Offset"]->SetMaximum(2);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2DeltaOffset.png").c_str());

    // ----------------------------------------------

    m_hists["h2DeltaOffsetV2"]->Draw("COLZ1");

    // m_hists["h2Offset"]->SetMinimum(0);
    // m_hists["h2Offset"]->SetMaximum(2);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2DeltaOffsetV2.png").c_str());

    // ----------------------------------------------

    m_hists["h2DeltaOffsetV3"]->Draw("COLZ1");

    // m_hists["h2Offset"]->SetMinimum(0);
    // m_hists["h2Offset"]->SetMaximum(2);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2DeltaOffsetV3.png").c_str());

    // ----------------------------------------------

    m_hists["h2NewOffset"]->Draw("COLZ1");

    // m_hists["h2Offset"]->SetMinimum(0);
    // m_hists["h2Offset"]->SetMaximum(2);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2NewOffset.png").c_str());

    // ----------------------------------------------

    m_hists["h2NewOffsetV2"]->Draw("COLZ1");

    // m_hists["h2Offset"]->SetMinimum(0);
    // m_hists["h2Offset"]->SetMaximum(2);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2NewOffsetV2.png").c_str());

    // ----------------------------------------------

    m_hists["h2NewOffsetV3"]->Draw("COLZ1");

    m_hists["h2NewOffsetV3"]->SetMinimum(-2e3);
    m_hists["h2NewOffsetV3"]->SetMaximum(1.8e3);

    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2NewOffsetV3.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    m_hists["hOffset"]->Rebin(2);
    m_hists["hOffset"]->GetXaxis()->SetRangeUser(-2e3,1e3);
    m_hists["hOffset"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hOffset.png").c_str());

    // ----------------------------------------------

    gPad->SetLogy();
    gPad->SetGrid(0,0);

    // gStyle->SetOptStat(1111);

    m_hists["hDeltaOffset"]->Rebin(2);
    // m_hists["hDeltaOffset"]->SetStats();
    m_hists["hDeltaOffset"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hDeltaOffset"]->GetXaxis()->SetRangeUser(-2e3,4e3);
    m_hists["hDeltaOffset"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hDeltaOffset.png").c_str());

    // ----------------------------------------------

    m_hists["hDeltaOffsetV2"]->Rebin(2);
    m_hists["hDeltaOffsetV2"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hDeltaOffsetV2"]->GetXaxis()->SetRangeUser(-2e3,4e3);
    m_hists["hDeltaOffsetV2"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hDeltaOffsetV2.png").c_str());

    // ----------------------------------------------

    m_hists["hDeltaOffsetV3"]->Rebin(2);
    m_hists["hDeltaOffsetV3"]->GetYaxis()->SetRangeUser(5e-1,4e3);
    m_hists["hDeltaOffsetV3"]->GetXaxis()->SetRangeUser(-2e3,4e3);
    m_hists["hDeltaOffsetV3"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hDeltaOffsetV3.png").c_str());

    // ----------------------------------------------

    m_hists["hNewOffset"]->Rebin(2);
    m_hists["hNewOffset"]->GetYaxis()->SetRangeUser(5e-1,3e3);
    m_hists["hNewOffset"]->GetXaxis()->SetRangeUser(-3.5e3,5e3);
    m_hists["hNewOffset"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hNewOffset.png").c_str());

    // ----------------------------------------------

    m_hists["hNewOffsetV2"]->Rebin(2);
    m_hists["hNewOffsetV2"]->GetYaxis()->SetRangeUser(5e-1,3e3);
    m_hists["hNewOffsetV2"]->GetXaxis()->SetRangeUser(-3.5e3,5e3);
    m_hists["hNewOffsetV2"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hNewOffsetV2.png").c_str());

    // ----------------------------------------------

    m_hists["hNewOffsetV3"]->Rebin(2);
    m_hists["hNewOffsetV3"]->GetYaxis()->SetRangeUser(5e-1,3e3);
    m_hists["hNewOffsetV3"]->GetXaxis()->SetRangeUser(-3.5e3,5e3);
    m_hists["hNewOffsetV3"]->Draw();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hNewOffsetV3.png").c_str());

    gPad->SetLogy(0);
    m_hists["hNewOffsetV3"]->SetTitle("Bias Offset");
    m_hists["hNewOffsetV3"]->GetYaxis()->SetRangeUser(0,3.2e3);
    m_hists["hNewOffsetV3"]->GetXaxis()->SetRangeUser(-2e3,2e3);
    m_hists["hOffset"]->SetLineColor(kRed);
    m_hists["hOffset"]->Draw("same");

    leg = new TLegend(0.2+xshift,.75+yshift,0.65+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04);
    leg->AddEntry(m_hists["hOffset"],legA.str().c_str(),"l");
    leg->AddEntry(m_hists["hNewOffsetV3"],legB.str().c_str(),"l");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hNewOffsetV3-overlay.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.1);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gPad->SetGrid();

    m_hists["hCosmicMPVDeltaOffset"]->Draw("COLZ1");

    m_hists["hCosmicMPVDeltaOffset"]->GetXaxis()->SetRangeUser(-2e3,1.1e3);
    m_hists["hCosmicMPVDeltaOffset"]->GetYaxis()->SetRangeUser(0,8e2);

    TH1* px = static_cast<TH2*>(m_hists["hCosmicMPVDeltaOffset"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hCosmicMPVDeltaOffset.png").c_str());

    // ----------------------------------------------

    gPad->SetGrid();
    gPad->SetLogz();

    m_hists["hFiberTypeDeltaOffset"]->Draw("COLZ1");

    m_hists["hFiberTypeDeltaOffset"]->GetYaxis()->SetRangeUser(-2e3,1.1e3);
    m_hists["hFiberTypeDeltaOffset"]->GetYaxis()->SetTitleOffset(1.2);
    m_hists["hFiberTypeDeltaOffset"]->GetXaxis()->SetLabelSize(0.05);

    px = static_cast<TH2*>(m_hists["hFiberTypeDeltaOffset"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetLineWidth(3);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hFiberTypeDeltaOffset.png").c_str());

    // ----------------------------------------------

    m_hists["hFiberTypeCosmicMPV"]->Draw("COLZ1");

    m_hists["hFiberTypeCosmicMPV"]->GetYaxis()->SetRangeUser(0,8e2);
    m_hists["hFiberTypeCosmicMPV"]->GetYaxis()->SetTitleOffset(1.2);
    m_hists["hFiberTypeCosmicMPV"]->GetXaxis()->SetLabelSize(0.05);

    px = static_cast<TH2*>(m_hists["hFiberTypeCosmicMPV"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetLineWidth(3);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hFiberTypeCosmicMPV.png").c_str());

    // ----------------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13);
    c1->SetRightMargin(.12);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    m_hists["hDeltaOffsetGain"]->Draw("COLZ1");

    m_hists["hDeltaOffsetGain"]->GetYaxis()->SetRangeUser(-2e3,4e3);
    m_hists["hDeltaOffsetGain"]->GetXaxis()->SetRangeUser(0,6);
    m_hists["hDeltaOffsetGain"]->GetYaxis()->SetTitleOffset(1.3);
    m_hists["hDeltaOffsetGain"]->GetXaxis()->SetLabelSize(0.05);
    m_hists["hDeltaOffsetGain"]->GetYaxis()->SetLabelSize(0.05);
    m_hists["hDeltaOffsetGain"]->GetXaxis()->SetTitleSize(0.05);
    m_hists["hDeltaOffsetGain"]->GetYaxis()->SetTitleSize(0.05);

    px = static_cast<TH2*>(m_hists["hDeltaOffsetGain"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetLineWidth(3);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hDeltaOffsetGain.png").c_str());

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void write_Biasv3(const string &input,
                  const string &outputDir=".",
                  const string &plotDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input: "  << input << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "plotDir: " << plotDir << endl;
    cout << "#############################" << endl;

    fs::create_directories(outputDir);
    fs::create_directories(plotDir);
    fs::create_directories(plotDir+"/images");

    TH1::AddDirectory(kFALSE);

    myAnalysis::analyze(input, outputDir);
    myAnalysis::make_plots(plotDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, char* argv[]) {
if(argc < 2 || argc > 4){
        cout << "usage: ./write-Biasv3 input [outputDir] [plotDir]" << endl;
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
    if(argc >= 3) {
        plotDir = argv[3];
    }

    write_Biasv3(argv[1], outputDir, plotDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
