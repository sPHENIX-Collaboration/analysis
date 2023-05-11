#include <calobase/TowerInfoDefs.h>

// c++ includes --
#include <string>
#include <vector>
#include <iostream>

// -- root includes --
#include <TH2F.h>
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

    void init(const string& inputFile = "data/LEDTowerBuilder.root");
    void analyze(UInt_t nevents=0);
    void finalize(const string &outputFile="output/test.root");

    TFile* input;
    TTree* led_tree;

    // QA
    TH1F* hTime;
    TH1F* hADC;
    TH1F* hadc;
    TH1F* hPed;
    TH1F* hChannels;

    // 2D correlations
    TH2F* h2TimeVsChannel;
    TH2F* h2ADCVsChannel;
    TH2F* h2PedVsChannel;
    TH2F* h2ADCVsTime;
    TH2F* h2PedVsTime;
    TH2F* h2ADCVsPed;

    TH2F* h2TimeVsChannel_scat;
    TH2F* h2ADCVsChannel_scat;
    TH2F* h2PedVsChannel_scat;
    TH2F* h2ADCVsTime_scat;
    TH2F* h2PedVsTime_scat;
    TH2F* h2ADCVsPed_scat;

    // one graph per channel
    vector<TH2F*> h2adcVsTime;

    UInt_t  time_bins    = 32;
    Float_t time_min     = 0-0.5;
    Float_t time_max     = 32-0.5;

    UInt_t  ADC_bins     = 1800;
    Float_t ADC_min      = 0;
    Float_t ADC_max      = 18000;

    UInt_t  adc_bins     = 1800;
    Float_t adc_min      = 0;
    Float_t adc_max      = 18000;

    UInt_t  ped_bins     = 1650;
    Float_t ped_min      = 0;
    Float_t ped_max      = 16500;

    UInt_t channels_bins = 24576;
    UInt_t channels_min  = 0;
    UInt_t channels_max  = 24576;
}

void myAnalysis::init(const string& inputFile) {
    input = TFile::Open(inputFile.c_str());
    led_tree = (TTree*)input->Get("W");

    // QA

    hTime = new TH1F("hTime", "Peak Location; Time Sample; Counts", time_bins, time_min, time_max);
    hADC = new TH1F("hADC", "ADC; ADC; Counts", ADC_bins, ADC_min, ADC_max);
    hadc = new TH1F("hadc", "adc; adc; Counts", adc_bins, adc_min, adc_max);
    hPed = new TH1F("hPed", "Ped; Ped; Counts", ped_bins, ped_min, ped_max);
    hChannels = new TH1F("hChannels", "Channels; Channel; Counts", channels_bins, channels_min, channels_max);

    // 2D correlations

    h2TimeVsChannel = new TH2F("h2TimeVsChannel", "Peak Location vs Channel; Channel; Time Sample", channels_bins, channels_min, channels_max, time_bins, time_min, time_max);
    h2ADCVsChannel = new TH2F("h2ADCVsChannel", "ADC vs Channel; Channel; ADC", channels_bins, channels_min, channels_max, ADC_bins, ADC_min, ADC_max);
    h2PedVsChannel = new TH2F("h2PedVsChannel", "Ped vs Channel; Channel; Ped", channels_bins, channels_min, channels_max, ped_bins, ped_min, ped_max);

    h2ADCVsTime = new TH2F("h2ADCVsTime", "ADC vs Peak Location; Time Sample; ADC", time_bins, time_min, time_max, ADC_bins, ADC_min, ADC_max);
    h2PedVsTime = new TH2F("h2PedVsTime", "Ped vs Peak Location; Time Sample; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
    h2ADCVsPed = new TH2F("h2ADCVsPed", "ADC vs Ped; Ped; ADC", ped_bins, ped_min, ped_max, ADC_bins, ADC_min, ADC_max);

    // scatter
    h2TimeVsChannel_scat = new TH2F("h2TimeVsChannel_scat", "Peak Location vs Channel; Channel; Time Sample", channels_bins, channels_min, channels_max, time_bins, time_min, time_max);
    h2ADCVsChannel_scat = new TH2F("h2ADCVsChannel_scat", "ADC vs Channel; Channel; ADC", channels_bins, channels_min, channels_max, ADC_bins, ADC_min, ADC_max);
    h2PedVsChannel_scat = new TH2F("h2PedVsChannel_scat", "Ped vs Channel; Channel; Ped", channels_bins, channels_min, channels_max, ped_bins, ped_min, ped_max);

    h2ADCVsTime_scat = new TH2F("h2ADCVsTime_scat", "ADC vs Peak Location; Time Sample; ADC", time_bins, time_min, time_max, ADC_bins, ADC_min, ADC_max);
    h2PedVsTime_scat = new TH2F("h2PedVsTime_scat", "Ped vs Peak Location; Time Sample; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
    h2ADCVsPed_scat = new TH2F("h2ADCVsPed_scat", "ADC vs Ped; Ped; ADC", ped_bins, ped_min, ped_max, ADC_bins, ADC_min, ADC_max);

    for (UInt_t i = 0; i < channels_bins; ++i) {
        h2adcVsTime.push_back(new TH2F(("h2adcVsTime_" + to_string(i)).c_str(),
                                         "adc vs Time Sample; Time Sample; adc",
                                         time_bins, time_min, time_max,
                                         50, adc_min, adc_max));
    }
}

void myAnalysis::analyze(UInt_t nevents) {
    vector<Float_t>* time              = 0;
    vector<Float_t>* ADC               = 0;
    vector<Float_t>* ped               = 0;
    vector<Int_t>* chan                = 0;
    vector<vector<Float_t>>* waveforms = 0; // 2D: channel x time sample

    led_tree->SetBranchAddress("time",&time);
    led_tree->SetBranchAddress("adc",&ADC);
    led_tree->SetBranchAddress("ped",&ped);
    led_tree->SetBranchAddress("chan",&chan);
    led_tree->SetBranchAddress("waveforms",&waveforms);

    // if nevents is 0 then use all events otherwise use the set number of events
    nevents = (nevents) ? nevents : led_tree->GetEntries();

    Int_t counter_event[24576] = {0};
    // maximum waveforms to overlap per channel
    Int_t events_max = 100;
    for(UInt_t i = 0; i < nevents; ++i) {
        if(i%100 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

        led_tree->GetEntry(i);

        UInt_t nchannels = time->size();
        channels_max = max(channels_max,nchannels);

        for(UInt_t j = 0; j < nchannels; ++j) {
            Float_t time_val = time->at(j);
            Float_t ADC_val  = ADC->at(j);
            Float_t ped_val  = ped->at(j);
            Int_t channel    = chan->at(j);
            if(channel >= 24576) {
                cout << "invalid channel number: " << channel << ", event: " << i << endl;
                continue;
            }

            UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            Int_t time_bin    = hTime->FindBin(time_val);
            Int_t ADC_bin     = hADC->FindBin(ADC_val);
            Int_t ped_bin     = hPed->FindBin(ped_val);
            Int_t channel_bin = hChannels->FindBin(channel);

            for (UInt_t sample = 0; sample < time_bins; ++sample) {
                Float_t adc_val = waveforms->at(j).at(sample);
                // Int_t adc_bin   = h2adcVsTime[channel]->GetYaxis()->FindBin(adc_val);

                hadc->Fill(adc_val);
                // overlay at most #n events of waveforms for each channel
                if(counter_event[channel] < events_max) {
                    // h2adcVsTime[channel]->SetBinContent(sample+1, adc_bin, 1);
                    h2adcVsTime[channel]->Fill(sample, adc_val);
                }

                adc_min = min(adc_min, adc_val);
                adc_max = max(adc_max, adc_val);
            }
            ++counter_event[channel];

            hChannels->Fill(channel);

            hTime->Fill(time_val);
            hADC->Fill(ADC_val);
            hPed->Fill(ped_val);

            h2TimeVsChannel->Fill(channel, time_val);
            h2ADCVsChannel->Fill(channel, ADC_val);
            h2PedVsChannel->Fill(channel, ped_val);

            h2TimeVsChannel_scat->SetBinContent(channel_bin, time_bin, 1);
            h2ADCVsChannel_scat->SetBinContent(channel_bin, ADC_bin, 1);
            h2PedVsChannel_scat->SetBinContent(channel_bin, ped_bin, 1);

            h2ADCVsTime->Fill(time_val, ADC_val);
            h2PedVsTime->Fill(time_val, ped_val);
            h2ADCVsPed->Fill(ped_val, ADC_val);

            h2ADCVsTime_scat->SetBinContent(time_bin, ADC_bin, 1);
            h2PedVsTime_scat->SetBinContent(time_bin, ped_bin, 1);
            h2ADCVsPed_scat->SetBinContent(ped_bin, ADC_bin, 1);

            time_min = min(time_min, time_val);
            time_max = max(time_max, time_val);

            ADC_min = min(ADC_min, ADC_val);
            ADC_max = max(ADC_max, ADC_val);

            ped_min = min(ped_min, ped_val);
            ped_max = max(ped_max, ped_val);
        }
    }

    cout << "events processed: " << nevents << endl;
    cout << "max channels per event: " << channels_max << endl;
    cout << "time_min: " << time_min << " time_max: " << time_max << endl;
    cout << "ADC_min: " << ADC_min << " ADC_max: " << ADC_max << endl;
    cout << "adc_min: " << adc_min << " adc_max: " << adc_max << endl;
    cout << "ped_min: " << ped_min << " ped_max: " << ped_max << endl;
}

void myAnalysis::finalize(const string& outputFile) {
    TFile output(outputFile.c_str(),"recreate");
    output.cd();

    hChannels->Write();
    hTime->Write();
    hADC->Write();
    hadc->Write();
    hPed->Write();

    h2TimeVsChannel->Write();
    h2ADCVsChannel->Write();
    h2PedVsChannel->Write();

    h2ADCVsTime->Write();
    h2PedVsTime->Write();
    h2ADCVsPed->Write();

    output.mkdir("scat");
    output.cd("scat");

    h2TimeVsChannel_scat->Write();
    h2ADCVsChannel_scat->Write();
    h2PedVsChannel_scat->Write();

    h2ADCVsTime_scat->Write();
    h2PedVsTime_scat->Write();
    h2ADCVsPed_scat->Write();

    output.cd();

    output.mkdir("adcVsTime");
    output.cd("adcVsTime");
    for(auto h2 : h2adcVsTime) {
        if(h2->GetEntries()) h2->Write();
    }

    // Close root file
    input->Close();
    output.Close();
}

void convert_channel_to_tower_bin() {
    for (UInt_t i = 0; i < 1536; ++i) {
        UInt_t key = TowerInfoDefs::encode_emcal(i);
        UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
        UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);
        // cout << "channel: " << i << ", key: " << key << ", etabin: " << etabin << ", phibin: " << phibin << endl;
        cout << "channel: " << i << ", etabin: " << etabin << ", phibin: " << phibin << endl;
    }
}

void read_LEDs(const string &inputFile="data/LEDTowerBuilder.root",
               const string &outputFile="output/test.root",
               UInt_t nevents=0) {

    myAnalysis::init(inputFile);
    myAnalysis::analyze(nevents);
    myAnalysis::finalize(outputFile);

    // convert_channel_to_tower_bin();
}

# ifndef __CINT__
int main(int argc, char* argv[]) {
    if(argc < 1 || argc > 4){
        cout << "usage: ./bin/read-LEDs inputFile outputFile events" << endl;
        cout << "inputFile: Location of LEDTowerBuilder.root. Default = data/LEDTowerBuilder.root." << endl;
        cout << "outputFile: output root file. Default = output/test.root." << endl;
        cout << "events: Number of events to analyze. Default = 0 (meaning all events)." << endl;
        return 1;
    }

    string input  = "data/LEDTowerBuilder.root";
    string output = "output/test.root";
    UInt_t events = 0;

    if(argc >= 2) {
        input = argv[1];
    }
    if(argc >= 3) {
        output = argv[2];
    }
    if(argc >= 4) {
        events = atoi(argv[3]);
    }

    read_LEDs(input, output, events);

    cout << "done" << endl;
    return 0;
}
# endif
