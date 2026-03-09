#include <calobase/TowerInfoDefs.h>

// c++ includes --
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// -- root includes --
#include <TH2F.h>
#include <TFile.h>
#include <TTree.h>
#include <TLine.h>
#include <TCanvas.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::min;
using std::max;
using std::to_string;
using std::ofstream;

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {

    void init(const string& input_CEMC, const string& outputFile = "event-display/test.json", const string& input_HCALIN = "", const string& input_HCALOUT = "");
    void analyze();
    void finalize(const string& run, const string& event);

    ofstream output;

    TFile* finput_CEMC = 0;
    TFile* finput_HCALIN = 0;
    TFile* finput_HCALOUT = 0;

    TTree* tree_CEMC;
    TTree* tree_HCALIN;
    TTree* tree_HCALOUT;

    TH2F* h2ADC_CEMC;
    TH2F* h2ADC_HCALIN;
    TH2F* h2ADC_HCALOUT;

    TH1F* hADC_CEMC;
    TH1F* hADC_HCALIN;
    TH1F* hADC_HCALOUT;

    // CEMC

    UInt_t  eta_bins = 96;
    Float_t eta_min  = -0.5;
    Float_t eta_max  = 95.5;

    UInt_t  phi_bins = 256;
    Float_t phi_min  = -0.5;
    Float_t phi_max  = 255.5;

    UInt_t etabin_max = 95;
    UInt_t phibin_max = 255;

    UInt_t channels_bins = 24576;
    UInt_t channels_min  = 0;
    UInt_t channels_max  = 24576;

    // HCAL

    UInt_t  eta_hcal_bins = 24;
    Float_t eta_hcal_min  = -0.5;
    Float_t eta_hcal_max  = 23.5;

    UInt_t  phi_hcal_bins = 64;
    Float_t phi_hcal_min  = -0.5;
    Float_t phi_hcal_max  = 63.5;

    UInt_t channels_hcal_bins = 1536;
    UInt_t channels_hcal_min  = 0;
    UInt_t channels_hcal_max  = 1536;

    const float eta_map[] = {-1.05417, -0.9625,  -0.870833,-0.779167,-0.6875,  -0.595833,-0.504167,-0.4125,
                             -0.320833,-0.229167,-0.1375,  -0.0458333,0.0458333,0.1375,   0.229167, 0.320833,
                              0.4125,   0.504167, 0.595833, 0.6875,   0.779167, 0.870833, 0.9625,   1.05417};

    const float phi_map[] = {0.0490874, 0.147262, 0.245437, 0.343612, 0.441786, 0.539961, 0.638136, 0.736311,
                             0.834486,  0.93266,  1.03084,  1.12901,  1.22718,  1.32536,  1.42353,  1.52171,
                             1.61988,   1.71806,  1.81623,  1.91441,  2.01258,  2.11076,  2.20893,  2.30711,
                             2.40528,   2.50346,  2.60163,  2.69981,  2.79798,  2.89616,  2.99433,  3.09251,
                             3.19068,   3.28885,  3.38703,  3.4852,   3.58338,  3.68155,  3.77973,  3.8779,
                             3.97608,   4.07425,  4.17243,  4.2706,   4.36878,  4.46695,  4.56513,  4.6633,
                             4.76148,   4.85965,  4.95783,  5.056,    5.15418,  5.25235,  5.35052,  5.4487,
                             5.54687,   5.64505,  5.74322,  5.8414,   5.93957,  6.03775,  6.13592,  6.2341};

    UInt_t  ADC_bins = 65;
    Float_t ADC_min  = 0;
    Float_t ADC_max  = 1.3e3;
}

void myAnalysis::init(const string& input_CEMC, const string& outputFile, const string& input_HCALIN, const string& input_HCALOUT) {

    finput_CEMC    = TFile::Open(input_CEMC.c_str());
    if(!input_HCALIN.empty()) finput_HCALIN  = TFile::Open(input_HCALIN.c_str());
    if(!input_HCALOUT.empty()) finput_HCALOUT = TFile::Open(input_HCALOUT.c_str());

    tree_CEMC    = (TTree*)finput_CEMC->Get("W");
    tree_HCALIN  = (TTree*)finput_HCALIN->Get("W");
    tree_HCALOUT = (TTree*)finput_HCALOUT->Get("W");

    output.open(outputFile.c_str());

    hADC_CEMC  = new TH1F("hADC_CEMC", "ADC CEMC; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    h2ADC_CEMC = new TH2F("h2ADC_CEMC", "ADC CEMC; towerid #eta; towerid #phi", eta_bins, eta_min, eta_max,
                                                                                phi_bins, phi_min, phi_max);

    if(finput_HCALIN) {
        hADC_HCALIN  = new TH1F("hADC_HCALIN", "ADC HCALIN; ADC; Counts", ADC_bins, ADC_min, ADC_max);
        h2ADC_HCALIN = new TH2F("h2ADC_HCALIN", "ADC HCALIN; towerid #eta; towerid #phi", eta_hcal_bins, eta_hcal_min, eta_hcal_max,
                                                                                          phi_hcal_bins, phi_hcal_min, phi_hcal_max);
    }

    if(finput_HCALOUT) {
        hADC_HCALOUT  = new TH1F("hADC_HCALOUT", "ADC HCALOUT; ADC; Counts", ADC_bins, ADC_min, ADC_max);
        h2ADC_HCALOUT = new TH2F("h2ADC_HCALOUT", "ADC HCALOUT; towerid #eta; towerid #phi", eta_hcal_bins, eta_hcal_min, eta_hcal_max,
                                                                                             phi_hcal_bins, phi_hcal_min, phi_hcal_max);
    }

output << "{\n \
    \"EVENT\": {\n \
      \"runid\": 1,\n \
      \"evtid\": 1,\n \
      \"time\": 0,\n \
      \"type\": \"Collision\",\n \
      \"s_nn\": 0,\n \
      \"B\": 3.0,\n \
      \"pv\": [0,0,0]\n \
    },\n \
    \"META\": {\n \
      \"HITS\": {\n \
        \"INNERTRACKER\": {\n \
          \"type\": \"3D\",\n \
          \"options\": {\n \
            \"size\": 5,\n \
            \"color\": 16777215\n \
          }\n \
        },\n \
        \"CEMC\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 90,\n \
              \"rmax\": 116.1,\n \
              \"deta\": 0.024,\n \
              \"dphi\": 0.024,\n \
              \"color\": 16766464,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": false\n \
            }\n \
        },\n \
        \"HCALIN\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 117.27,\n \
              \"rmax\": 134.42,\n \
              \"deta\": 0.025,\n \
              \"dphi\": 0.025,\n \
              \"color\": 4290445312,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": false\n \
            }\n \
        },\n \
        \"HCALOUT\": {\n \
          \"type\": \"PROJECTIVE\",\n \
            \"options\": {\n \
              \"rmin\": 183.3,\n \
              \"rmax\": 274.317,\n \
              \"deta\": 0.025,\n \
              \"dphi\": 0.025,\n \
              \"color\": 24773,\n \
              \"transparent\": 0.6,\n \
              \"scaleminmax\": true\n \
            }\n \
        }\n \
      }\n \
    },\n \
\"HITS\": {\n";
}

void myAnalysis::analyze() {

    bool first_entry = true;
    vector<Float_t>* ADC = 0;
    vector<Int_t>* chan  = 0;
    // vector<Float_t>* time              = 0;
    // vector<Float_t>* ped               = 0;
    // vector<vector<Float_t>>* waveforms = 0; // 2D: channel x time sample
    Float_t max_ADC = ADC_max;
    if(finput_CEMC && tree_CEMC->GetEntries()) {
        tree_CEMC->SetBranchAddress("adc",&ADC);
        tree_CEMC->SetBranchAddress("chan",&chan);
        tree_CEMC->GetEntry(0);
        UInt_t nchannels = ADC->size();

        for(UInt_t j = 0; j < nchannels; ++j) {
            // convert from high gain to low gain (factor of 16)
            Float_t ADC_val  = ADC->at(j)/16;
            // Float_t e        = 0.002*ADC_val/16;
            Float_t e        = ADC_val;
            // if(ADC_val < 4e3) continue;
            Int_t channel    = chan->at(j);
            if(channel >= channels_max) {
                cout << "CEMC invalid channel number: " << channel << endl;
                continue;
            }

            UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            h2ADC_CEMC->SetBinContent(etabin+1, phibin+1, ADC_val);
            hADC_CEMC->Fill(ADC_val);

            max_ADC = max(max_ADC, ADC_val);

            // mapping: [0,95] -> [-1.1, 1.1]
            Float_t eta = 2.2/etabin_max*etabin-1.1;
            // mapping: [0,127] -> [0, pi] and [128, 255] -> [-pi, 0]
            Float_t phi = (phibin <= phibin_max/2) ? M_PI/(phibin_max/2)*phibin :
                                                    M_PI/(phibin_max/2)*(phibin*1.-phibin_max);

            if(first_entry) {
                output << "\"CEMC\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
                first_entry = false;
            }
            else output << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
            // if(j == 10) break;
        }
        cout << "CEMC Max ADC: " << max_ADC << endl;
        output << "],\n";
    }
    else output << "\"CEMC\": [{}],\n";

    first_entry = true;
    max_ADC = ADC_max;
    if(finput_HCALIN && tree_HCALIN->GetEntries()) {
        ADC  = 0;
        chan = 0;

        tree_HCALIN->SetBranchAddress("adc",&ADC);
        tree_HCALIN->SetBranchAddress("chan",&chan);
        tree_HCALIN->GetEntry(0);

        UInt_t nchannels = ADC->size();

        for(UInt_t j = 0; j < nchannels; ++j) {
            Float_t ADC_val  = ADC->at(j);
            // Float_t e        = 0.002*ADC_val;
            Float_t e        = ADC_val;
            Int_t channel    = chan->at(j);
            if(channel >= channels_max) {
                cout << "HCALIN invalid channel number: " << channel << endl;
                continue;
            }

            UInt_t key    = TowerInfoDefs::encode_hcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            h2ADC_HCALIN->SetBinContent(etabin+1, phibin+1, ADC_val);
            hADC_HCALIN->Fill(ADC_val);

            max_ADC = max(max_ADC, ADC_val);

            Float_t eta = eta_map[etabin];
            Float_t phi = phi_map[phibin];

            if(first_entry){
              output << "\"HCALIN\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
              first_entry = false;
            }
            else output << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
            // if(j == 10) break;
        }
        cout << "HCALIN Max ADC: " << max_ADC << endl;
        output << "],\n";
    }
    else output << "\"HCALIN\": [{}],\n";

    first_entry = true;
    max_ADC = ADC_max;
    if(finput_HCALOUT && tree_HCALOUT->GetEntries()) {
        ADC  = 0;
        chan = 0;

        tree_HCALOUT->SetBranchAddress("adc",&ADC);
        tree_HCALOUT->SetBranchAddress("chan",&chan);
        tree_HCALOUT->GetEntry(0);

        UInt_t nchannels = ADC->size();

        for(UInt_t j = 0; j < nchannels; ++j) {
            Float_t ADC_val  = ADC->at(j);
            // Float_t e        = 0.002*ADC_val;
            Float_t e        = ADC_val;
            // if(ADC_val < 100) continue;
            Int_t channel    = chan->at(j);
            if(channel >= channels_max) {
                cout << "HCALOUT invalid channel number: " << channel << endl;
                continue;
            }

            UInt_t key    = TowerInfoDefs::encode_hcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            h2ADC_HCALOUT->SetBinContent(etabin+1, phibin+1, ADC_val);
            hADC_HCALOUT->Fill(ADC_val);

            max_ADC = max(max_ADC, ADC_val);

            Float_t eta = eta_map[etabin];
            Float_t phi = phi_map[phibin];

            if(first_entry) {
                output << "\"HCALOUT\": [{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
                first_entry = false;
            }
            else output << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << e << "}\n";
            // if(j == 10) break;
            // cout << "etabin: " << etabin << ", phibin: " << phibin << ", ADC: " << ADC_val << endl;
            // cout << ",{ \"eta\": " << eta << ", \"phi\": " << phi << " , \"e\": " << ADC_val << "}" << endl;
        }
        cout << "HCALOUT Max ADC: " << max_ADC << endl;
        output << "]\n";
    }
    else output << "\"HCALOUT\": [{}]\n";
}

void myAnalysis::finalize(const string& run, const string& event) {

        output << "},\n \
    \"TRACKS\": {\n \
    \"INNERTRACKER\": []\n \
    }}";


    auto c1 = new TCanvas();

    c1->cd();
    // c1->SetTickx();
    // c1->SetTicky();

    c1->SetCanvasSize(1000, 1500);

    c1->SetLeftMargin(.12);
    c1->SetRightMargin(.15);
    // gPad->SetLogz();

    string outputFile = "event-display/plots-" + run + "-" + event + ".pdf";
    c1->Print((outputFile + "[").c_str(), "pdf portrait");

    gPad->SetLogz();
    h2ADC_CEMC->SetStats(0);
    h2ADC_CEMC->SetTitle(("ADC CEMC, Run: " + run + ", event: " + event).c_str());
    // h2ADC_CEMC->SetTickLength(0,"xy");
    // h2ADC_CEMC->SetTitleOffset(0.5, "z");
    // h2ADC_CEMC->SetNdivisions(12,"x");
    // h2ADC_CEMC->SetNdivisions(32,"y");
    h2ADC_CEMC->Draw("COLZ1");

    auto tline = new TLine();

    for (UInt_t i = 0; i < 32; ++i) {
        tline->DrawLine(eta_min, 8*i-0.5, eta_max, 8*i-0.5);
    }
    for (UInt_t i = 0; i < 12; ++i) {
        tline->DrawLine(8*i-0.5, phi_min, 8*i-0.5, phi_max);
    }
    c1->Print((outputFile).c_str(), "pdf portrait");

    // hADC_CEMC->SetStats(0);
    c1->SetCanvasSize(1500, 1000);
    gPad->SetLogy();
    hADC_CEMC->SetTitle(("ADC CEMC, Run: " + run + ", event: " + event).c_str());
    hADC_CEMC->Draw();
    c1->Print((outputFile).c_str(), "pdf portrait");
    gPad->SetLogy(0);

    // gPad->SetLogz(0);
    // h2ADC_CEMC->SetMinimum(4000);
    // c1->Print((outputFile).c_str(), "pdf portrait");

    if(finput_HCALIN) {
        c1->SetCanvasSize(1000, 1500);
        gPad->SetLogz();
        h2ADC_HCALIN->SetStats(0);
        h2ADC_HCALIN->SetTitle(("ADC HCALIN, Run: " + run + ", event: " + event).c_str());
        h2ADC_HCALIN->Draw("COLZ1");
        c1->Print((outputFile).c_str(), "pdf portrait");

        c1->SetCanvasSize(1500, 1000);
        gPad->SetLogy();
        hADC_HCALIN->SetTitle(("ADC HCALIN, Run: " + run + ", event: " + event).c_str());
        hADC_HCALIN->Draw();
        c1->Print((outputFile).c_str(), "pdf portrait");
        gPad->SetLogy(0);

        // gPad->SetLogz(0);
        // h2ADC_HCALIN->SetMinimum(100);
        // c1->Print((outputFile).c_str(), "pdf portrait");
    }


    if(finput_HCALOUT) {
        c1->SetCanvasSize(1000, 1500);
        gPad->SetLogz();
        h2ADC_HCALOUT->SetStats(0);
        h2ADC_HCALOUT->SetTitle(("ADC HCALOUT, Run: " + run + ", event: " + event).c_str());
        h2ADC_HCALOUT->Draw("COLZ1");
        c1->Print((outputFile).c_str(), "pdf portrait");

        c1->SetCanvasSize(1500, 1000);
        gPad->SetLogy();
        hADC_HCALOUT->SetTitle(("ADC HCALOUT, Run: " + run + ", event: " + event).c_str());
        hADC_HCALOUT->Draw();
        c1->Print((outputFile).c_str(), "pdf portrait");
        gPad->SetLogy(0);

        // gPad->SetLogz(0);
        // h2ADC_HCALOUT->SetMinimum(100);
        // c1->Print((outputFile).c_str(), "pdf portrait");
    }

    c1->Print((outputFile + "]").c_str(), "pdf portrait");

    // Close files
    if(finput_CEMC)    finput_CEMC->Close();
    if(finput_HCALIN)  finput_HCALIN->Close();
    if(finput_HCALOUT) finput_HCALOUT->Close();
    output.close();
}

void test_hcal() {
    UInt_t max_eta = 0;
    UInt_t max_phi = 0;
    for (UInt_t channel = 0; channel < myAnalysis::channels_hcal_bins; ++channel) {
        UInt_t key    = TowerInfoDefs::encode_hcal(channel);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

        cout << "channel: " << channel << ", etabin: " << etabin << ", phibin: " << phibin << endl;

        max_eta = max(max_eta, etabin);
        max_phi = max(max_phi, phibin);
    }
    cout << "max etabin: " << max_eta << ", max phibin: " << max_phi << endl;
}

void event_display(const string& run,
                   const string& event,
                   const string& input_CEMC,
                   const string& outputFile="event-display/test.json",
                   const string& input_HCALIN = "",
                   const string& input_HCALOUT = "") {
    myAnalysis::init(input_CEMC, outputFile, input_HCALIN, input_HCALOUT);
    myAnalysis::analyze();
    myAnalysis::finalize(run, event);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 7){
        cout << "usage: ./bin/event-display run event input_CEMC outputFile input_HCALIN input_HCALOUT" << endl;
        cout << "run: run number." << endl;
        cout << "event: event number." << endl;
        cout << "input_CEMC: Location of CEMC root file." << endl;
        cout << "outputFile: output json file. Default = event-display/test.json." << endl;
        cout << "input_HCALIN: Location of HCALIN root file." << endl;
        cout << "input_HCALOUT: Location of HCALOUT root file." << endl;
        return 1;
    }

    string run;
    string event;
    string input_CEMC;
    string output = "event-display/test.json";
    string input_HCALIN = "";
    string input_HCALOUT = "";

    if(argc >= 2) {
        run = argv[1];
    }
    if(argc >= 3) {
        event = argv[2];
    }
    if(argc >= 4) {
        input_CEMC = argv[3];
    }
    if(argc >= 5) {
        output = argv[4];
    }
    if(argc >= 6) {
        input_HCALIN = argv[5];
    }
    if(argc >= 7) {
        input_HCALOUT = argv[6];
    }
    event_display(run, event, input_CEMC, output, input_HCALIN, input_HCALOUT);

    cout << "done" << endl;
    return 0;
}
# endif
