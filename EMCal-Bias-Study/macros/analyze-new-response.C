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
    void readWidth(const string &line);
    void readRunInfo(const string &line);
    void readRun(const string &line);
    Int_t readInt(stringstream &ss, string &cell, char delimiter);
    void initHists();
    void saveHists(const string &outputDir);

    unordered_map<string, Int_t> m_best_pulse; // key: sector,ib ; value: best width
    unordered_map<string,Int_t> m_runInfo; // key: best width,offset ; value: run number
    unordered_map<Int_t,string> m_runInfoRev; // key: run number ; value: best width,offset
    map<string,TH1*> m_hists;

    Double_t kFactor  = 4.42e-4; // Units: [1 / mV]

    UInt_t m_bins_gain = 40;
    Double_t m_gain_low = 0;
    Double_t m_gain_high = 2;

    Bool_t m_saveFig = true;
    Int_t m_verbosity = 2;
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

void myAnalysis::readWidth(const string &line) {
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
      Int_t best_pulse = readInt(ss, cell, ',');

      std::getline(ss, cell, ',');
      string offset = cell;

      key << best_pulse << "," << offset;
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

    m_hists["hResponseRatioDivGain"] = new TH1F("hResponseRatioDivGain","Ratio: Response [New / Default] / Gain; Response [New / Default] / Gain; Counts", m_bins_gain, m_gain_low, m_gain_high);
    m_hists["hResponseRatioVsGain"] = new TH2F("hResponseRatioVsGain","Response [New / Default] Vs Gain; Gain; Response [New / Default]", m_bins_gain, m_gain_low, m_gain_high, m_bins_gain, m_gain_low, m_gain_high);

    // track errors
    m_hists["hResponseRatioDivGain"]->Sumw2();
    // m_hists["hGainCalibInvFactors"]->Sumw2();
    // m_hists["hGainRatio"]->Sumw2();
    // m_hists["hDeltaOffsetCalibInv"]->Sumw2();
    // m_hists["hDeltaOffset"]->Sumw2();
    // m_hists["hDeltaOffsetRatio"]->Sumw2();
}

Int_t myAnalysis::readHists(const string &input) {
    // Read Hist from input
    TFile* tfile = TFile::Open(input.c_str());
    if (!tfile || tfile->IsZombie()) {
        cout << "Error: Could not open ROOT file: " << input << endl;
        return 1;
    }
    m_hists["h2GainFactors"] = static_cast<TH2*>(tfile->Get("h2GainFactors"));
    m_hists["h2GainFactorsV1"] = static_cast<TH2*>(m_hists["h2GainFactors"]->Clone("h2GainFactorsV1"));
    m_hists["h2Offset"] = static_cast<TH2*>(tfile->Get("h2Offset"));
    m_hists["h2Calib"] = static_cast<TH2*>(tfile->Get("h2Calib"));
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

    for(UInt_t i = 0; i < myUtils::m_nphi; ++i) {
        for(UInt_t j = 0; j < myUtils::m_neta; ++j) {

            Double_t calib = static_cast<TH2*>(m_hists["h2Calib"])->GetBinContent(i+1, j+1);

            // skipping previously uncalibrated regions of the EMCal
            if(!calib) continue;

            stringstream key;
            stringstream name;

            pair<Int_t, Int_t> sectorIB = myUtils::getSectorIB(i, j);
            key << sectorIB.first << "," << sectorIB.second;

            // skip known bad IBs
            if(!m_best_pulse.contains(key.str())) {
                if(!skippedIBs.contains(key.str())) {
                    skippedIBs.insert(key.str());
                    cout << "SKIPPING sector,ib = " << key.str() << endl;
                }
                continue;
            }

            // cout << key.str() << endl;
            Int_t best_pulse = m_best_pulse[key.str()];

            key.str("");
            key << best_pulse << ",original";
            // cout << key.str() << endl;
            Int_t runDefault = m_runInfo[key.str()];

            key.str("");
            key << best_pulse << ",new";
            Int_t runNew = m_runInfo[key.str()];

            name << "h2CEMC_" << runDefault;
            // cout << name.str() << endl;
            Double_t responseDefault = static_cast<TH2*>(m_hists[name.str()])->GetBinContent(i+1, j+1);
            static_cast<TH2*>(m_hists["h2ResponseDefault"])->SetBinContent(i+1, j+1, responseDefault);

            name.str("");
            name << "h2CEMC_" << runNew;
            Double_t responseNew = static_cast<TH2*>(m_hists[name.str()])->GetBinContent(i+1, j+1);
            static_cast<TH2*>(m_hists["h2ResponseNew"])->SetBinContent(i+1, j+1, responseNew);

            Double_t gain = static_cast<TH2*>(m_hists["h2GainFactors"])->GetBinContent(i+1, j+1);
            Double_t offset = static_cast<TH2*>(m_hists["h2Offset"])->GetBinContent(i+1, j+1);

            gain = std::min(gain, TMath::Exp(1000*kFactor));             // ensure the delta offset < 1000 mV
            gain = std::max(gain, TMath::Exp(-1*kFactor*(2000+offset))); // ensure the new offset >= -2000 mV

            static_cast<TH2*>(m_hists["h2GainFactorsV1"])->SetBinContent(i+1, j+1, gain);

            Double_t responseRatio = (responseDefault) ? responseNew / responseDefault : 0;
            Double_t responseRatioDivGain = (gain) ? responseRatio / gain : 0;

            if(responseRatioDivGain) {
                min_ResponseRatioDivGain = std::min(responseRatioDivGain, min_ResponseRatioDivGain);
                max_ResponseRatioDivGain = std::max(responseRatioDivGain, max_ResponseRatioDivGain);

                min_ResponseRatio = std::min(responseRatio, min_ResponseRatio);
                max_ResponseRatio = std::max(responseRatio, max_ResponseRatio);

                m_hists["hResponseRatioDivGain"]->Fill(responseRatioDivGain);
                m_hists["hResponseRatioVsGain"]->Fill(gain, responseRatio);
            }
        }
    }

    m_hists["h2ResponseRatio"] = static_cast<TH2*>(m_hists["h2ResponseNew"]->Clone("h2ResponseRatio"));
    m_hists["h2ResponseRatio"]->SetTitle("Ratio: Response [New / Default]");
    m_hists["h2ResponseRatio"]->Divide(m_hists["h2ResponseDefault"]);

    m_hists["h2ResponseRatioDivGain"] = static_cast<TH2*>(m_hists["h2ResponseRatio"]->Clone("h2ResponseRatioDivGain"));
    m_hists["h2ResponseRatioDivGain"]->SetTitle("Ratio: Response [New / Default] / Gain");
    m_hists["h2ResponseRatioDivGain"]->Divide(m_hists["h2GainFactorsV1"]);

    cout << "====================" << endl;
    cout << "Response [New / Default], Min: " << min_ResponseRatio << ", Max: " << max_ResponseRatio << endl;
    cout << "Response [New / Default] / Gain, Min: " << min_ResponseRatioDivGain << ", Max: " << max_ResponseRatioDivGain << endl;
    cout << "Response [New / Default] / Gain within 5% of unity: " << m_hists["hResponseRatioDivGain"]->Integral(m_hists["hResponseRatioDivGain"]->FindBin(0.95), m_hists["hResponseRatioDivGain"]->FindBin(1.05)-1) * 100 / m_hists["hResponseRatioDivGain"]->Integral() << " %" << endl;
    cout << "Response [New / Default] / Gain within 10% of unity: " << m_hists["hResponseRatioDivGain"]->Integral(m_hists["hResponseRatioDivGain"]->FindBin(0.9), m_hists["hResponseRatioDivGain"]->FindBin(1.1)-1) * 100 / m_hists["hResponseRatioDivGain"]->Integral() << " %" << endl;
    // cout << "Delta Offset Ratio, Min: " << min_deltaOffsetRatio << ", Max: " << max_deltaOffsetRatio << endl;
    // cout << "Delta Offset from Cosmic MPV, Integral: " << m_hists["hDeltaOffset"]->Integral() << endl;
    // cout << "Delta Offset from Calib, Integral: " << m_hists["hDeltaOffsetCalibInv"]->Integral() << endl;

    return 0;
}

void myAnalysis::saveHists(const string &outputDir) {
    // save plots to root file
    string output = outputDir + "/updated-offset-comparison.root";
    TFile tf(output.c_str(),"recreate");
    tf.cd();

    m_hists["h2Offset"]->Write();
    m_hists["h2Calib"]->Write();
    m_hists["h2ResponseDefault"]->Write();
    m_hists["h2ResponseNew"]->Write();
    m_hists["h2GainFactors"]->Write();
    m_hists["h2GainFactorsV1"]->Write();
    m_hists["h2ResponseRatio"]->Write();
    m_hists["h2ResponseRatioDivGain"]->Write();
    m_hists["hResponseRatioDivGain"]->Write();
    m_hists["hResponseRatioVsGain"]->Write();

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
    c1->SetLeftMargin(.06);
    c1->SetRightMargin(.1);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    gStyle->SetOptTitle(1);
    gStyle->SetTitleStyle(0);
    gStyle->SetTitleW(1);
    gStyle->SetTitleH(0.08);
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

    m_hists["h2GainFactorsV1"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2GainFactorsV1"]->SetMinimum(0.39);
    m_hists["h2GainFactorsV1"]->SetMaximum(1.83);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2GainFactorsV1.png").c_str());

    // ----------------------------------------

    m_hists["h2ResponseRatio"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2ResponseRatio"]->SetMinimum(0.39);
    m_hists["h2ResponseRatio"]->SetMaximum(1.83);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatio.png").c_str());

    // ----------------------------------------

    m_hists["h2ResponseRatioDivGain"]->Draw("COLZ1");
    m_hists["h2DummySector"]->Draw("TEXT MIN0 same");
    m_hists["h2DummyIB"]->Draw("TEXT MIN0 same");

    m_hists["h2ResponseRatioDivGain"]->SetMinimum(0.64);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/h2ResponseRatioDivGain.png").c_str());

    // ----------------------------------------

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.14);
    c1->SetRightMargin(.05);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);
    gStyle->SetOptStat(1111);

    m_hists["hResponseRatioDivGain"]->Draw("hist e");
    m_hists["hResponseRatioDivGain"]->SetStats();
    m_hists["hResponseRatioDivGain"]->SetLineColor(kRed);
    m_hists["hResponseRatioDivGain"]->SetMarkerColor(kRed);
    m_hists["hResponseRatioDivGain"]->GetXaxis()->SetTitleOffset(0.9);
    m_hists["hResponseRatioDivGain"]->GetXaxis()->SetRangeUser(0.6,1.8);

    gPad->Update();

    TPaveStats* st = static_cast<TPaveStats*>(m_hists["hResponseRatioDivGain"]->FindObject("stats"));
    Double_t xlow = 0.68;
    Double_t ylow = 0.62;
    st->SetX1NDC(xlow);
    st->SetY1NDC(ylow);
    st->SetX2NDC(xlow+0.27);
    st->SetY2NDC(ylow+0.28);

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain.png").c_str());

    gPad->SetLogy();

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioDivGain-logy.png").c_str());

    // ----------------------------------------

    gPad->SetLogy(0);
    gPad->SetLogz();

    c1->SetCanvasSize(1400, 1000);
    c1->SetLeftMargin(.11);
    c1->SetRightMargin(.13);
    c1->SetTopMargin(.1);
    c1->SetBottomMargin(.12);

    m_hists["hResponseRatioVsGain"]->Draw("COLZ1");
    m_hists["hResponseRatioVsGain"]->GetXaxis()->SetRangeUser(0.35,1.85);
    m_hists["hResponseRatioVsGain"]->GetYaxis()->SetRangeUser(0.35,1.85);
    m_hists["hResponseRatioVsGain"]->GetYaxis()->SetTitleOffset(0.85);
    m_hists["hResponseRatioVsGain"]->GetXaxis()->SetTitleOffset(0.9);

    TH1* px = static_cast<TH2*>(m_hists["hResponseRatioVsGain"])->ProfileX();
    px->SetLineColor(kRed);
    px->SetMarkerColor(kRed);
    px->Draw("same");

    TF1* tf = new TF1("tf","x",0,2);
    tf->SetLineStyle(kDashed);
    tf->Draw("same");

    c1->Print(output.c_str(), "pdf portrait");
    if (m_saveFig) c1->Print((outputDir + "/images/hResponseRatioVsGain.png").c_str());


    c1->Print((output + "]").c_str(), "pdf portrait");
}

void analyze_new_response(const string &input_raw,
                  const string &input_raw_info,
                  const string &input_width,
                  const string &input_hists,
                  const string &outputDir=".",
                  const string &plotDir=".") {

    cout << "#############################" << endl;
    cout << "Run Parameters" << endl;
    cout << "input raw: "  << input_raw << endl;
    cout << "input raw info: "  << input_raw_info << endl;
    cout << "input width: "  << input_width << endl;
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

    if(!myUtils::readCSV(input_width, myAnalysis::readWidth)) return;

    if(myAnalysis::m_verbosity > 1) {
        cout << "####################" << endl;
        for (const auto& [key, value] : myAnalysis::m_best_pulse) {
            cout << "Key: " << key << ", Best Pulse: " << value << " ns" << endl;
        }
        cout << "Total: " << myAnalysis::m_best_pulse.size() << endl;
        cout << "####################" << endl;
    }

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

    if(myAnalysis::readHists(input_hists)) return;
    myAnalysis::initHists();
    myAnalysis::analyze();
    myAnalysis::saveHists(outputDir);
    myAnalysis::make_plots(plotDir);
}

# ifndef __CINT__
Int_t main(Int_t argc, const char* const argv[]) {
if(argc < 2 || argc > 7){
        cout << "usage: ./analyze-new-response input_raw input_raw_info input_width input_hists [outputDir] [plotDir]" << endl;
        cout << "input: input root file" << endl;
        cout << "outputDir: output directory" << endl;
        cout << "plotDir: plot directory" << endl;
        return 1;
    }

    string outputDir = ".";
    string plotDir = ".";

    if(argc >= 6) {
        outputDir = argv[5];
    }
    if(argc >= 7) {
        plotDir = argv[6];
    }

    analyze_new_response(argv[1], argv[2], argv[3], argv[4], outputDir, plotDir);

    cout << "======================================" << endl;
    cout << "done" << endl;
    return 0;
}
# endif
