#include <calobase/TowerInfoDefs.h>

// c++ includes --
#include <string>
#include <vector>
#include <iostream>

// -- root includes --
#include <TH2F.h>
#include <TH3F.h>
#include <TFile.h>
#include <TTree.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::min;
using std::max;
using std::to_string;

R__LOAD_LIBRARY(libcalo_io.so)

namespace myAnalysis {

    void init(const string &inputFile);
    void analyze(UInt_t nevents);
    void finalize(const string &outputFile);

    TFile* input;
    TTree* W;

    // QA
    TH1F* hChannels;
    TH1F* hADC;
    TH1F* hADCNorth;
    TH1F* hADCSouth;
    TH1F* hADCFarNorth; // last IB
    TH1F* hTotalADC;
    TH1F* hADCVsChannels;
    TH1F* hPed;
    TH1F* hTime;

    // 2D correlations
    TH2F* h2Channels;
    TH2F* h2ADC;
    TH2F* h2ADCVsEta;
    TH2F* h2ADCVsTime;
    TH2F* h2PedVsTime;
    TH2F* h2ADCVsPed;

    // 3D correlations
    TH3F* h3ADC;
    TH3F* h3Ped;
    TH3F* h3Time;

    UInt_t  time_bins    = 32;
    Float_t time_min     = -0.5;
    Float_t time_max     = 31.5;

    UInt_t  ADC_bins     = 1500;
    Float_t ADC_min      = 0;
    Float_t ADC_max      = 3e3;

    UInt_t  TotalADC_bins = 2000;
    Float_t TotalADC_min  = 0;
    Float_t TotalADC_max  = 1.5e7;

    UInt_t  ped_bins     = 1250;
    Float_t ped_min      = 0;
    Float_t ped_max      = 5e3;

    UInt_t channels_bins = 24576;
    Float_t channels_min = -0.5;
    Float_t channels_max = 24575.5;

    UInt_t  eta_bins = 96;
    Float_t eta_min  = -0.5;
    Float_t eta_max  = 95.5;

    UInt_t  phi_bins = 256;
    Float_t phi_min  = -0.5;
    Float_t phi_max  = 255.5;
}

void myAnalysis::init(const string& inputFile) {
    input = TFile::Open(inputFile.c_str());
    W = (TTree*)input->Get("W");

    // QA

    hTime          = new TH1F("hTime", "Peak Location; Time Sample; Counts", time_bins, time_min, time_max);
    hADC           = new TH1F("hADC", "ADC; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    hADCNorth      = new TH1F("hADCNorth", "ADC, Towerid #eta #geq 48; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    hADCSouth      = new TH1F("hADCSouth", "ADC, Towerid #eta < 48; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    hADCFarNorth   = new TH1F("hADCFarNorth", "ADC, Towerid #eta #geq 88; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    hTotalADC      = new TH1F("hTotalADC", "ADC; ADC; Counts", TotalADC_bins, TotalADC_min, TotalADC_max);
    hADCVsChannels = new TH1F("hADCVsChannels", "Total ADC; Channel; ADC", channels_bins, channels_min, channels_max);
    hPed           = new TH1F("hPed", "Ped; Ped; Counts", ped_bins, ped_min, ped_max);
    hChannels      = new TH1F("hChannels", "Channels; Channel; Counts", channels_bins, channels_min, channels_max);

    // 2D correlations

    h2Channels = new TH2F("h2Channels", "Channels; Towerid #eta; Towerid #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2ADC      = new TH2F("h2ADC", "ADC; Towerid #eta; Towerid #phi", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max);
    h2ADCVsEta = new TH2F("h2ADCVsEta", "ADC; Towerid #eta; ADC", eta_bins, eta_min, eta_max, ADC_bins, ADC_min, ADC_max);

    h2ADCVsTime = new TH2F("h2ADCVsTime", "ADC vs Peak Location; Time Sample; ADC", time_bins, time_min, time_max, ADC_bins, ADC_min, ADC_max);
    h2PedVsTime = new TH2F("h2PedVsTime", "Ped vs Peak Location; Time Sample; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
    h2ADCVsPed = new TH2F("h2ADCVsPed", "ADC vs Ped; Ped; ADC", ped_bins, ped_min, ped_max, ADC_bins, ADC_min, ADC_max);

    // 3D correlations

    h3ADC  = new TH3F("h3ADC", "ADC; Towerid #eta; Towerid #phi; ADC", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max, ADC_bins, ADC_min, ADC_max);
    h3Ped  = new TH3F("h3Ped", "Ped; Towerid #eta; Towerid #phi; Ped", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max, ped_bins, ped_min, ped_max);
    h3Time = new TH3F("h3Time", "Peak Location; Towerid #eta; Towerid #phi; Time Sample", eta_bins, eta_min, eta_max, phi_bins, phi_min, phi_max, time_bins, time_min, time_max);
}

void myAnalysis::analyze(UInt_t nevents) {
    vector<Float_t>* time = 0;
    vector<Float_t>* ADC  = 0;
    vector<Float_t>* ped  = 0;
    vector<Int_t>*   chan = 0;
    // vector<vector<Float_t>>* waveforms = 0; // 2D: channel x time sample

    W->SetBranchAddress("time",&time);
    W->SetBranchAddress("adc", &ADC);
    W->SetBranchAddress("ped", &ped);
    W->SetBranchAddress("chan",&chan);

    // if nevents is 0 then use all events otherwise use the set number of events
    nevents = (nevents) ? nevents : W->GetEntries();

    Int_t events_max = 100;
    Float_t channels_min = 99999;
    Float_t channels_max = 0;
    for(UInt_t i = 0; i < nevents; ++i) {
        if(i%100 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

        W->GetEntry(i);

        Float_t nchannels = time->size();
        channels_max = max(channels_max,nchannels);
        channels_min = min(channels_min,nchannels);

        for(UInt_t j = 0; j < nchannels; ++j) {
            Float_t time_val = time->at(j);
            Float_t ADC_val  = ADC->at(j);
            Float_t ped_val  = ped->at(j);
            Int_t   channel  = chan->at(j);
            if(channel >= 24576 || channel == 14492 || channel == 24220) {
                continue;
            }

            UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            // 1D
            hChannels->Fill(channel);
            hTime->Fill(time_val);
            hADC->Fill(ADC_val);
            hADCVsChannels->Fill(channel, ADC_val);
            hPed->Fill(ped_val);

            if(etabin >= 48) {
                hADCNorth->Fill(ADC_val);
                // check if tower is in the last IB
                if(etabin >= 88) hADCFarNorth->Fill(ADC_val);
            }
            else hADCSouth->Fill(ADC_val);

            // 2D
            h2Channels->Fill(etabin, phibin);
            h2ADC->Fill(etabin, phibin, ADC_val);
            h2ADCVsEta->Fill(etabin, ADC_val);

            h2ADCVsTime->Fill(time_val, ADC_val);
            h2PedVsTime->Fill(time_val, ped_val);
            h2ADCVsPed->Fill(ped_val, ADC_val);

            // 3D
            h3ADC->Fill(etabin, phibin, ADC_val);
            h3Ped->Fill(etabin, phibin, ped_val);
            h3Time->Fill(etabin, phibin, time_val);

            time_min = min(time_min, time_val);
            time_max = max(time_max, time_val);

            ADC_min = min(ADC_min, ADC_val);
            ADC_max = max(ADC_max, ADC_val);

            ped_min = min(ped_min, ped_val);
            ped_max = max(ped_max, ped_val);
        }
    }

    // for(UInt_t i = 0; i < channels_bins; ++i) {
    //     Float_t val = hADCVsChannels->GetBinContent(i+1);
    //     if(val) hTotalADC->Fill(val);
    // }

    cout << "events processed: " << nevents << endl;
    cout << "channels_min: " << channels_min << ", channels_max: " << channels_max << endl;
    cout << "time_min: " << time_min << ", time_max: " << time_max << endl;
    cout << "ADC_min: " << ADC_min << ", ADC_max: " << ADC_max << endl;
    cout << "ped_min: " << ped_min << ", ped_max: " << ped_max << endl;
    // cout << "Total ADC - 3sd: " << hTotalADC->GetMean()-3*hTotalADC->GetStdDev()
    //      << ", Total ADC + 3sd: " << hTotalADC->GetMean()+3*hTotalADC->GetStdDev() << endl;
}

void myAnalysis::finalize(const string& outputFile) {
    TFile output(outputFile.c_str(),"recreate");
    output.cd();

    hChannels->Write();
    hADC->Write();
    hADCNorth->Write();
    hADCSouth->Write();
    hADCFarNorth->Write();
    hADCVsChannels->Write();
    // hTotalADC->Write();
    hPed->Write();
    hTime->Write();

    h2Channels->Write();
    h2ADC->Write();
    h2ADCVsEta->Write();
    h2ADCVsTime->Write();
    h2PedVsTime->Write();
    h2ADCVsPed->Write();

    // h3ADC->Write();
    // h3Ped->Write();
    // h3Time->Write();

    input->Close();
    output.Close();
}

void read_LEDs(const string &inputFile,
               const string &outputFile="test.root",
                     UInt_t  nevents=0) {

    myAnalysis::init(inputFile);
    myAnalysis::analyze(nevents);
    myAnalysis::finalize(outputFile);
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 2 || argc > 4){
        cout << "usage: ./bin/read-LEDs inputFile outputFile events" << endl;
        cout << "inputFile: Location of input TTree File." << endl;
        cout << "outputFile: output root file. Default = test.root." << endl;
        cout << "events: Number of events to analyze. Default = 0 (meaning all events)." << endl;
        return 1;
    }

    string input ;
    string output = "test.root";
    UInt_t events = 0;

    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        events = atoi(argv[3]);
    }

    read_LEDs(argv[1], output, events);

    cout << "done" << endl;
    return 0;
}
# endif
