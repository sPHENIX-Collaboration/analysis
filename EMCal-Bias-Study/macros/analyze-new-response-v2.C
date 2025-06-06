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
using std::ofstream;
using std::pair;
using std::make_pair;
using std::unordered_map;
using std::map;
using std::unordered_set;
namespace fs = std::filesystem;

namespace myAnalysis {
    Int_t analyze();
    void make_plots(const string &outputDir);
    Int_t readHists(const string &input);
    void readBestPulseWidth(const string &line);
    void readRunInfo(const string &line);
    void readRun(const string &line);
    Int_t readInt(stringstream &ss, string &cell, char delimiter);
    void initHists();
    void saveHists(const string &outputDir);
    void findBestPulseWidth();

    unordered_map<string, Int_t> m_best_pulse; // key: sector,ib ; value: best width
    unordered_map<string,Int_t> m_runInfo; // key: width,offset ; value: run number
    unordered_map<Int_t,string> m_runInfoRev; // key: run number ; value: width,offset
    map<string,TH1*> m_hists;

    Double_t kFactor  = 4.42e-4; // Units: [1 / mV]

    unordered_set<string> m_knownBadIB = {"4.1","10.3","10.5","25.2","50.1","54.4"}; // sector.IB

    Int_t m_bins_gain = 80;
    Double_t m_gain_low = 0;
    Double_t m_gain_high = 4;

    Int_t m_bins_saturate = 800;
    Double_t m_saturate_low  = 0;  // GeV
    Double_t m_saturate_high = 80; // GeV

    Double_t m_noiseThreshold = 100; // ADC

    Bool_t m_saveFig = true;
    Int_t m_verbosity = 2;
}

void myAnalysis::findBestPulseWidth() {
    stringstream name;
    unordered_map<string, TH1*> m_hists_temp;
    m_hists["h2BestPulse"] = new TH2F("h2BestPulse","Best Pulse Width; IB Index #phi; IB Index #eta", myUtils::m_nphi/8, -0.5, myUtils::m_nphi/8-0.5, myUtils::m_neta/8, -0.5, myUtils::m_neta/8-0.5);
    m_hists["h2ADCBestPulse"] = new TH2F("h2ADCBestPulse","Avg ADC at Best Pulse Width; IB Index #phi; IB Index #eta", myUtils::m_nphi/8, -0.5, myUtils::m_nphi/8-0.5, myUtils::m_neta/8, -0.5, myUtils::m_neta/8-0.5);

    // loop over each board
    for(Int_t i = 0; i < myUtils::m_nphi; i += 8) {
        for(Int_t j = 0; j < myUtils::m_neta; j += 8) {
            pair<Int_t, Int_t> sectorIB_pair = myUtils::getSectorIB(i,j);
            string sectorIB = to_string(sectorIB_pair.first) + "," + to_string(sectorIB_pair.second);
            Int_t ib_phi = i / 8;
            Int_t ib_eta = j / 8;

            Double_t bestAvgADC = 0;
            Double_t bestAvgADCDiff = 1e5;
            Int_t best_pulse = 0;
            // loop over each original bias offset run
            for (const auto& [key, run] : m_runInfo) {

                // ensure original offset is used
                if(key.find("original") == string::npos) continue;

                Int_t pulse = std::stoi(myUtils::split(key,',')[0]);

                name.str("");
                name << "h2CEMC_" << run;

                string newName = name.str() + "_rebin";
                // add rebinned hist to map if not already added
                if(!m_hists_temp.contains(newName)) {
                    m_hists_temp[newName] = static_cast<TH2*>(m_hists[name.str()])->Rebin2D(8, 8, newName.c_str());
                    m_hists_temp[newName]->Scale(1./myUtils::m_nchannel_per_ib);
                }

                Double_t avgADC = static_cast<TH2*>(m_hists_temp[newName])->GetBinContent(ib_phi+1, ib_eta+1);

                if(avgADC > m_noiseThreshold && fabs(3000-avgADC) < bestAvgADCDiff) {
                    bestAvgADCDiff = fabs(3000-avgADC);
                    best_pulse = pulse;
                    bestAvgADC = avgADC;
                }
            }

            static_cast<TH2*>(m_hists["h2BestPulse"])->SetBinContent(ib_phi+1, ib_eta+1, best_pulse);
            static_cast<TH2*>(m_hists["h2ADCBestPulse"])->SetBinContent(ib_phi+1, ib_eta+1, bestAvgADC);

            m_best_pulse[sectorIB] = best_pulse;
        }
    }

    if(myAnalysis::m_verbosity > 1) {
        cout << "####################" << endl;
        for (const auto& [key, value] : myAnalysis::m_best_pulse) {
            cout << "Key: " << key << ", Best Pulse: " << value << " ns" << endl;
        }
        cout << "Total: " << myAnalysis::m_best_pulse.size() << endl;
        cout << "####################" << endl;
    }
}

Int_t myAnalysis::readInt(stringstream &ss, string &cell, char delimiter) {
    Int_t val;
    if (std::getline(ss, cell, delimiter)) {
      try {
        val = std::stoi(cell);
      }
      catch (const std::invalid_argument &e) {
        throw std::runtime_error("Invalid integer in column: " + cell);
      }
      catch (const std::out_of_range &e) {
        throw std::runtime_error("Integer out of range in column: " + cell);
      }
    }
    else {
      throw std::runtime_error("Error parsing column");
    }

    return val;
}

void myAnalysis::readBestPulseWidth(const string &line) {
    stringstream ss(line);

    try {
      string cell;
      stringstream key;

      Int_t sector = readInt(ss, cell, ',');
      Int_t ib = readInt(ss, cell, ',');
      Int_t best_pulse = readInt(ss, cell, ',');

      key << sector << "," << ib;
      m_best_pulse[key.str()] = best_pulse;
    }
    catch (const std::runtime_error &e) {
      cout << "Error processing line: " << line << ".  Error: " << e.what() << endl;
      // You could choose to continue processing or exit here.  For now, continuing.
    }
}

void myAnalysis::readRunInfo(const string &line) {
    stringstream ss(line);

    try {
      string cell;
      stringstream key;

      Int_t run = readInt(ss, cell, ',');
      Int_t pulse = readInt(ss, cell, ',');

      std::getline(ss, cell, ',');
      string offset = cell;

      key << pulse << "," << offset;
      m_runInfo[key.str()] = run;
      m_runInfoRev[run] = key.str();
    }
    catch (const std::runtime_error &e) {
      cout << "Error processing line: " << line << ".  Error: " << e.what() << endl;
      // You could choose to continue processing or exit here.  For now, continuing.
    }
}

void myAnalysis::readRun(const string &line) {
    // Read Hist from input
    TFile* tfile = TFile::Open(line.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << line << endl;
        return;
    }

    string fname = fs::absolute(line).filename().stem().string();
    string run = myUtils::split(myUtils::split(fname,'-')[1],'.')[0];
    string name = "h2CEMC_" + run;

    m_hists[name] = static_cast<TH2*>(tfile->Get("h2CEMC")->Clone(name.c_str()));

    tfile->Close();
}

void myAnalysis::initHists() {
    m_hists["h2ResponseDefault"] = new TH2F("h2ResponseDefault","Response: Default; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);
    m_hists["h2ResponseNew"] = new TH2F("h2ResponseNew","Response: New; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    m_hists["hResponseRatio"] = new TH1F("hResponseRatio","Ratio: Response [New / Default]; Response [New / Default]; Counts", m_bins_gain, m_gain_low, m_gain_high);

    m_hists["hResponseRatioDivGain"] = new TH1F("hResponseRatioDivGain","Ratio: Response [New / Default] / Gain; Response [New / Default] / Gain; Counts", m_bins_gain, m_gain_low, m_gain_high);
    m_hists["hResponseRatioVsGain"] = new TH2F("hResponseRatioVsGain","Response [New / Default] Vs Gain; Gain; Response [New / Default]", m_bins_gain, m_gain_low, m_gain_high, m_bins_gain, m_gain_low, m_gain_high);

    m_hists["hSaturateV4"] = new TH1F("hSaturateV4","EMCal Saturation; Energy [GeV]; Counts", m_bins_saturate, m_saturate_low, m_saturate_high);
    m_hists["h2SaturateV4"] = new TH2F("h2SaturateV4","EMCal Saturation [GeV]; Tower Index #phi; Tower Index #eta", myUtils::m_nphi, -0.5, myUtils::m_nphi-0.5, myUtils::m_neta, -0.5, myUtils::m_neta-0.5);

    // track errors
    m_hists["hResponseRatio"]->Sumw2();
    m_hists["hResponseRatioDivGain"]->Sumw2();
    m_hists["hSaturateV4"]->Sumw2();
}

Int_t myAnalysis::readHists(const string &input) {
    // Read Hist from input
    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists["h2GainFactor"] = static_cast<TH2*>(tfile->Get("h2GainFactorV2")->Clone("h2GainFactor"));
    m_hists["hGainFactor"] = static_cast<TH2*>(tfile->Get("hGainFactorV2")->Clone("hGainFactor"));
    m_hists["h2Calib"] = static_cast<TH2*>(tfile->Get("h2Calib"));
    m_hists["h2SaturateV2"] = static_cast<TH2*>(tfile->Get("h2SaturateV2"));
    m_hists["h2SaturateV3"] = static_cast<TH2*>(tfile->Get("h2SaturateV3"));
    m_hists["hSaturate"] = static_cast<TH2*>(tfile->Get("hSaturate"));
    m_hists["hSaturateV2"] = static_cast<TH2*>(tfile->Get("hSaturateV2"));
    m_hists["hSaturateV3"] = static_cast<TH2*>(tfile->Get("hSaturateV3"));
    m_hists["h2DummySector"] = static_cast<TH2*>(tfile->Get("h2DummySector"));
    m_hists["h2DummyIB"] = static_cast<TH2*>(tfile->Get("h2DummyIB"));

    tfile->Close();
    return 0;
}

Int_t myAnalysis::analyze() {

    unordered_set<string> skippedIBs;

    Double_t min_ResponseRatioDivGain = 9999;
    Double_t max_ResponseRatioDivGain = 0;

    Double_t min_ResponseRatio = 9999;
    Double_t max_ResponseRatio = 0;

    Int_t uncalibTowers = 0;
    Int_t noBestWidthTowers = 0;
    Int_t failsNoiseThreshTowers = 0;

    for(Int_t i = 0; i < myUtils::m_nphi; ++i) {
        for(Int_t j = 0; j < myUtils::m_neta; ++j) {

            Double_t calib = static_cast<TH2*>(m_hists["h2Calib"])->GetBinContent(i+1, j+1);

            pair<Int_t, Int_t> sectorIB_pair = myUtils::getSectorIB(i,j);
            string sectorIB = to_string(sectorIB_pair.first) + "." + to_string(sectorIB_pair.second);
            Bool_t isBadIB = m_knownBadIB.contains(sectorIB);

            // skipping previously uncalibrated regions of the EMCal
            // skip known bad IBs
            if(!calib || isBadIB) {
                ++uncalibTowers;
                continue;
            }

            stringstream key;
            stringstream name;

            key << sectorIB_pair.first << "," << sectorIB_pair.second;

            // skip additional bad IBs
            if(!m_best_pulse.contains(key.str())) {
                if(!skippedIBs.contains(key.str())) {
                    skippedIBs.insert(key.str());
                    cout << "SKIPPING sector,ib = " << key.str() << endl;
                }
                ++noBestWidthTowers;
                continue;
            }

            Int_t best_pulse = m_best_pulse[key.str()];

            key.str("");
            key << "40,original";
            Int_t runMaxPulse = m_runInfo[key.str()];

            name << "h2CEMC_" << runMaxPulse;
            Double_t responseMaxPulse = static_cast<TH2*>(m_hists[name.str()])->GetBinContent(i+1, j+1);

            // skip channels to fail the noise threshold at max pulse width
            if(responseMaxPulse < m_noiseThreshold || best_pulse == 0) {
                ++failsNoiseThreshTowers;
                continue;
            }

            key.str("");
            key << best_pulse << ",original";
            Int_t runDefault = m_runInfo[key.str()];

            key.str("");
            key << best_pulse << ",new";
            Int_t runNew = m_runInfo[key.str()];

            name.str("");
            name << "h2CEMC_" << runDefault;
            Double_t responseDefault = static_cast<TH2*>(m_hists[name.str()])->GetBinContent(i+1, j+1);
            static_cast<TH2*>(m_hists["h2ResponseDefault"])->SetBinContent(i+1, j+1, responseDefault);

            name.str("");
            name << "h2CEMC_" << runNew;
            Double_t responseNew = static_cast<TH2*>(m_hists[name.str()])->GetBinContent(i+1, j+1);
            static_cast<TH2*>(m_hists["h2ResponseNew"])->SetBinContent(i+1, j+1, responseNew);

            Double_t gain = static_cast<TH2*>(m_hists["h2GainFactor"])->GetBinContent(i+1, j+1);

            Double_t responseRatio = (responseDefault) ? responseNew / responseDefault : 0;
            Double_t responseRatioDivGain = (gain) ? responseRatio / gain : 0;

            Double_t saturateV2 = static_cast<TH2*>(m_hists["h2SaturateV2"])->GetBinContent(i+1, j+1);

            if(responseRatioDivGain) {
                min_ResponseRatioDivGain = std::min(responseRatioDivGain, min_ResponseRatioDivGain);
                max_ResponseRatioDivGain = std::max(responseRatioDivGain, max_ResponseRatioDivGain);

                min_ResponseRatio = std::min(responseRatio, min_ResponseRatio);
                max_ResponseRatio = std::max(responseRatio, max_ResponseRatio);

                m_hists["hResponseRatioDivGain"]->Fill(responseRatioDivGain);
                static_cast<TH2*>(m_hists["hResponseRatioVsGain"])->Fill(gain, responseRatio);
                m_hists["hResponseRatio"]->Fill(responseRatio);

                Double_t saturateV4 = saturateV2 / responseRatio;
                static_cast<TH2*>(m_hists["h2SaturateV4"])->SetBinContent(i+1, j+1, saturateV4);
                m_hists["hSaturateV4"]->Fill(saturateV4);
            }
        }
    }

    m_hists["h2ResponseRatio"] = static_cast<TH2*>(m_hists["h2ResponseNew"]->Clone("h2ResponseRatio"));
    m_hists["h2ResponseRatio"]->SetTitle("Ratio: Response [New / Default]");
    m_hists["h2ResponseRatio"]->Divide(m_hists["h2ResponseDefault"]);

    m_hists["h2ResponseRatioDivGain"] = static_cast<TH2*>(m_hists["h2ResponseRatio"]->Clone("h2ResponseRatioDivGain"));
    m_hists["h2ResponseRatioDivGain"]->SetTitle("Ratio: Response [New / Default] / Gain");
    m_hists["h2ResponseRatioDivGain"]->Divide(m_hists["h2GainFactor"]);

    cout << "====================" << endl;
    cout << "Bad Tower Stats" << endl;
    cout << "Uncalibrated or known bad channels: " << uncalibTowers << endl;
    cout << "No best pulse width channels: " << noBestWidthTowers << endl;
    cout << "Fails noise threshold at max pulse width (40 ns) channels: " << failsNoiseThreshTowers << endl;
    Int_t goodTowers = myUtils::m_nphi*myUtils::m_neta - uncalibTowers - noBestWidthTowers - failsNoiseThreshTowers;
    cout << "Good Towers: " << goodTowers << ", " << goodTowers * 100. / (myUtils::m_nphi*myUtils::m_neta) << " %" << endl;

    cout << "====================" << endl;
    cout << "Response [New / Default], Min: " << min_ResponseRatio << ", Max: " << max_ResponseRatio << endl;
    cout << "Response [New / Default] / Gain, Min: " << min_ResponseRatioDivGain << ", Max: " << max_ResponseRatioDivGain << endl;
    cout << "Response [New / Default] / Gain within 5% of unity: " << m_hists["hResponseRatioDivGain"]->Integral(m_hists["hResponseRatioDivGain"]->FindBin(0.95), m_hists["hResponseRatioDivGain"]->FindBin(1.05)-1) * 100 / m_hists["hResponseRatioDivGain"]->Integral() << " %" << endl;
    cout << "Response [New / Default] / Gain within 10% of unity: " << m_hists["hResponseRatioDivGain"]->Integral(m_hists["hResponseRatioDivGain"]->FindBin(0.9), m_hists["hResponseRatioDivGain"]->FindBin(1.1)-1) * 100 / m_hists["hResponseRatioDivGain"]->Integral() << " %" << endl;

    return 0;
}

void myAnalysis::saveHists(const string &outputDir) {
    // save plots to root file
    string output = outputDir + "/updated-offset-comparison-v2.root";
    TFile tf(output.c_str(),"recreate");
    tf.cd();

    for (const auto &[name, hist] : m_hists) {
        if(!name.starts_with("h2CEMC")) hist->Write();
    }

    tf.mkdir("h2CEMC");
    tf.cd("h2CEMC");

    for (const auto &[name, hist] : m_hists) {
        if(name.starts_with("h2CEMC")) hist->Write();
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

    for (const auto &[name, hist] : m_hists) {
        if(name.starts_with("h2CEMC")) {
            Int_t run = std::stoi(myUtils::split(name,'_')[1]);
            vector<string> width_offset = myUtils::split(m_runInfoRev[run],',');
            stringstream title;
            title << "EMCal [ADC], Run: " << run << ", Pulse Width: " << width_offset[0] << " ns, Offset: " << width_offset[1];

            hist->SetTitle(title.str().c_str());

            hist->Draw("COLZ1");
            m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
            m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

            hist->SetMaximum(1.64e4);

            c1->Print(output.c_str(), "pdf portrait");
            if (m_saveFig) c1->Print((outputDir + "/images/" + name + ".png").c_str());
        }
    }

    // ----------------------------------------

    m_hists["h2ResponseDefault"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2ResponseDefault"]->SetMaximum(1.64e4);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseDefault.png").c_str());

    // ----------------------------------------

    m_hists["h2ResponseNew"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2ResponseNew"]->SetMaximum(1.64e4);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseNew.png").c_str());

    // ----------------------------------------

    m_hists["h2GainFactor"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2GainFactor"]->SetTitle("Gain");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor.png").c_str());

    m_hists["h2GainFactor"]->SetMinimum(0.1);
    m_hists["h2GainFactor"]->SetMaximum(0.9);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactor-zoom.png").c_str());

    // ----------------------------------------

    m_hists["h2ResponseRatio"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatio.png").c_str());

    m_hists["h2ResponseRatio"]->SetMinimum(0.1);
    m_hists["h2ResponseRatio"]->SetMaximum(0.9);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatio-zoom.png").c_str());

    // ----------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.14f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);
    gStyle->SetOptStat(1111);

    m_hists["hResponseRatio"]->Draw("hist e");
    m_hists["hResponseRatio"]->SetStats();
    m_hists["hResponseRatio"]->SetLineColor(kRed);
    m_hists["hResponseRatio"]->SetMarkerColor(kRed);
    m_hists["hResponseRatio"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hResponseRatio"]->GetXaxis()->SetRangeUser(0,1.5);

    gPad->Update();

    TPaveStats* st = static_cast<TPaveStats*>(m_hists["hResponseRatio"]->FindObject("stats"));
    Double_t xlow = 0.68;
    Double_t ylow = 0.62;
    st->SetX1NDC(xlow);
    st->SetY1NDC(ylow);
    st->SetX2NDC(xlow+0.27);
    st->SetY2NDC(ylow+0.28);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatio.png").c_str());

    gPad->SetLogy();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatio-logy.png").c_str());

    gPad->SetLogy(0);

    // ----------------------------------------
    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    m_hists["h2ResponseRatioDivGain"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatioDivGain.png").c_str());

    m_hists["h2ResponseRatioDivGain"]->SetMinimum(0.4);
    m_hists["h2ResponseRatioDivGain"]->SetMaximum(1.1);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatioDivGain-zoom.png").c_str());

    // ----------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.14f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);
    gStyle->SetOptFit(1111); // Shows Probability, Chi2/NDF, Errors, Values

    TFitResultPtr fitResult = myUtils::doGausFit(m_hists["hResponseRatioDivGain"], 0.87, 1.08);

    m_hists["hResponseRatioDivGain"]->Draw("e");
    m_hists["hResponseRatioDivGain"]->SetStats();
    m_hists["hResponseRatioDivGain"]->SetMarkerColor(kBlack);
    m_hists["hResponseRatioDivGain"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hResponseRatioDivGain"]->GetXaxis()->SetRangeUser(0,2);

    gPad->Update();

    st = static_cast<TPaveStats*>(m_hists["hResponseRatioDivGain"]->FindObject("stats"));
    xlow = 0.65;
    ylow = 0.5;
    st->SetX1NDC(xlow);
    st->SetY1NDC(ylow);
    st->SetX2NDC(xlow+0.3);
    st->SetY2NDC(ylow+0.4);
    st->SetTextSize(0.03f);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain.png").c_str());

    gPad->SetLogy();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain-logy.png").c_str());

    // ----------------------------------------

    gPad->SetLogy(0);
    gPad->SetLogz();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.11f);
    c1->SetRightMargin(.13f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);

    m_hists["hResponseRatioVsGain"]->Draw("COLZ1");
    m_hists["hResponseRatioVsGain"]->GetXaxis()->SetRangeUser(0.2,1.2);
    m_hists["hResponseRatioVsGain"]->GetYaxis()->SetRangeUser(0.2,1.2);
    m_hists["hResponseRatioVsGain"]->GetYaxis()->SetTitleOffset(0.85f);
    m_hists["hResponseRatioVsGain"]->GetXaxis()->SetTitleOffset(0.9f);

    TH1* px = static_cast<TH2*>(m_hists["hResponseRatioVsGain"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    TF1* tf = new TF1("tf","x",0,2);
    tf->SetLineStyle(kDashed);
    tf->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioVsGain.png").c_str());

    // ----------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.13f);
    c1->SetRightMargin(.05f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);
    gPad->SetGrid(0,0);

    cout << "Saturate Hists Entries" << endl;
    cout << "hSaturate: " << m_hists["hSaturate"]->Integral(1, m_bins_saturate) << endl;
    cout << "hSaturateV2: " << m_hists["hSaturateV2"]->Integral(1, m_bins_saturate) << endl;
    cout << "hSaturateV3: " << m_hists["hSaturateV3"]->Integral(1, m_bins_saturate) << endl;
    cout << "hSaturateV4: " << m_hists["hSaturateV4"]->Integral(1, m_bins_saturate) << endl;

    Double_t normalize = m_hists["hSaturateV3"]->Integral(1, m_bins_saturate) / m_hists["hSaturateV4"]->Integral(1, m_bins_saturate);

    m_hists["hSaturateV4"]->Scale(normalize);

    cout << "hSaturateV4 (after normalization): " << m_hists["hSaturateV4"]->Integral(1, m_bins_saturate) << endl;

    m_hists["hSaturate"]->Draw("hist e");
    m_hists["hSaturate"]->GetXaxis()->SetTitleOffset(0.9f);
    m_hists["hSaturate"]->GetYaxis()->SetRangeUser(0,4e3);
    m_hists["hSaturate"]->Rebin(10);

    m_hists["hSaturateV2"]->Draw("hist e same");
    m_hists["hSaturateV2"]->Rebin(10);
    m_hists["hSaturateV2"]->SetLineColor(kRed);
    m_hists["hSaturateV2"]->SetMarkerColor(kRed);

    m_hists["hSaturateV3"]->Draw("hist e same");
    m_hists["hSaturateV3"]->Rebin(10);
    m_hists["hSaturateV3"]->SetLineColor(kBlue);
    m_hists["hSaturateV3"]->SetMarkerColor(kBlue);

    m_hists["hSaturateV4"]->Draw("hist e same");
    m_hists["hSaturateV4"]->Rebin(10);
    m_hists["hSaturateV4"]->SetLineColor(kMagenta);
    m_hists["hSaturateV4"]->SetMarkerColor(kMagenta);

    stringstream legA, legB, legC, legD;

    legA << "Before Temperature Correction";
    legB << "After Temperature Correction";
    legC << "After Adjusting Gain (Expected)";
    legD << "After Adjusting Gain (Observed)";

    Double_t xshift = -0.08;
    Double_t yshift = 0.03;

    auto leg = new TLegend(0.2+xshift,.65+yshift,0.54+xshift,.85+yshift);
    leg->SetFillStyle(0);
    leg->SetTextSize(0.04f);
    leg->AddEntry(m_hists["hSaturate"],legA.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hSaturateV2"],legB.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hSaturateV3"],legC.str().c_str(),"lpe");
    leg->AddEntry(m_hists["hSaturateV4"],legD.str().c_str(),"lpe");
    leg->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hSaturate-overlay.png").c_str());

    // ----------------------------------------

    c1->SetCanvasSize(2900, 1000);
    c1->SetLeftMargin(.06f);
    c1->SetRightMargin(.1f);
    c1->SetTopMargin(.1f);
    c1->SetBottomMargin(.12f);
    gPad->SetLogz(0);
    gPad->SetGrid();

    m_hists["h2SaturateV3"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2SaturateV3"]->SetMinimum(10);
    m_hists["h2SaturateV3"]->SetMaximum(60);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV3.png").c_str());

    // ----------------------------------------

    m_hists["h2SaturateV4"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2SaturateV4"]->SetMinimum(10);
    m_hists["h2SaturateV4"]->SetMaximum(60);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2SaturateV4.png").c_str());

    // ----------------------------------------

    c1->Print((output + "]").c_str(), "pdf portrait");
}

void analyze_new_response_v2(const string &input_raw,
                  const string &input_raw_info,
                  const string &input_hists,
                  const string &outputDir=".",
                  const string &plotDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input raw: "  << input_raw << endl;
    cout << "input raw info: "  << input_raw_info << endl;
    cout << "input hists: "  << input_hists << endl;
    cout << "outputDir: " << outputDir << endl;
    cout << "plotDir: " << plotDir << endl;
    cout << "save figs: " << myAnalysis::m_saveFig << endl;
    cout << "verbosity: " << myAnalysis::m_verbosity << endl;
    cout << "#############################" << endl;

    // set sPHENIX plotting style
    SetsPhenixStyle();

    // create output directories
    fs::create_directories(outputDir);
    fs::create_directories(plotDir);
    fs::create_directories(plotDir+"/images");

    TH1::AddDirectory(kFALSE);

    if(!myUtils::readCSV(input_raw_info, myAnalysis::readRunInfo)) return;

    if(myAnalysis::m_verbosity > 1) {
        cout << "####################" << endl;
        for (const auto& [key, value] : myAnalysis::m_runInfo) {
            cout << "Key: " << key << ", Run: " << value << endl;
        }
        cout << "Total: " << myAnalysis::m_runInfo.size() << endl;
        cout << "####################" << endl;
    }

    // do not skip the first line as it's not the usual header
    if(!myUtils::readCSV(input_raw, myAnalysis::readRun, false)) return;

    if(myAnalysis::m_verbosity > 1) {
        cout << "####################" << endl;
        for (const auto& [key, value] : myAnalysis::m_hists) {
            cout << "Key: " << key << ", Run: " << value->GetName() << endl;
        }
        cout << "Total: " << myAnalysis::m_hists.size() << endl;
        cout << "####################" << endl;
    }

    myAnalysis::findBestPulseWidth();

    if(myAnalysis::readHists(input_hists)) return;
    myAnalysis::initHists();
    myAnalysis::analyze();
    myAnalysis::saveHists(outputDir);
    myAnalysis::make_plots(plotDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 6){
        cout << "usage: ./analyze-new-response-v2 input_raw input_raw_info input_hists [outputDir] [plotDir]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        cout << "plotDir: plot directory" << endl;
        return 1;
    }

    string outputDir = ".";
    string plotDir = ".";

    if(argc >= 5) {
        outputDir = argv[4];
    }
    if(argc >= 6) {
        plotDir = argv[5];
    }

    analyze_new_response_v2(argv[1], argv[2], argv[3], outputDir, plotDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
