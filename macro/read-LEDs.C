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
    TH1F* hAdc;
    TH1F* hPed;
    TH1F* hChannels;

    // 2D correlations
    TH2F* h2TimeVsChannel;
    TH2F* h2AdcVsChannel;
    TH2F* h2PedVsChannel;
    TH2F* h2AdcVsTime;
    TH2F* h2PedVsTime;
    TH2F* h2AdcVsPed;

    TH2F* h2TimeVsChannel_scat;
    TH2F* h2AdcVsChannel_scat;
    TH2F* h2PedVsChannel_scat;
    TH2F* h2AdcVsTime_scat;
    TH2F* h2PedVsTime_scat;
    TH2F* h2AdcVsPed_scat;

    // one graph per channel
    vector<TH2F*> h2AdcVsTime_ch;

    UInt_t  time_bins    = 60;
    Float_t time_min     = 0;
    Float_t time_max     = 30;

    UInt_t  adc_bins     = 1200;
    Float_t adc_min      = 0;
    Float_t adc_max      = 12000;

    UInt_t  ped_bins     = 60;
    Float_t ped_min      = 1300;
    Float_t ped_max      = 1900;

    UInt_t channels_bins = 1600;
    UInt_t channels_min  = 0;
    UInt_t channels_max  = 1600;
}

void myAnalysis::init(const string& inputFile) {
    input = TFile::Open(inputFile.c_str());
    led_tree = (TTree*)input->Get("W");

    // QA

    hTime = new TH1F("hTime", "Time; Time; Counts", time_bins, time_min, time_max);
    hAdc = new TH1F("hAdc", "Adc; Adc; Counts", adc_bins, adc_min, adc_max);
    hPed = new TH1F("hPed", "Ped; Ped; Counts", ped_bins, ped_min, ped_max);
    hChannels = new TH1F("hChannels", "Channels per Event; Channels; Counts", channels_bins, channels_min, channels_max);

    // 2D correlations

    h2TimeVsChannel = new TH2F("h2TimeVsChannel", "Time vs Channel; Channel; Time", channels_bins, channels_min, channels_max, time_bins, time_min, time_max);
    h2AdcVsChannel = new TH2F("h2AdcVsChannel", "Adc vs Channel; Channel; Adc", channels_bins, channels_min, channels_max, adc_bins, adc_min, adc_max);
    h2PedVsChannel = new TH2F("h2PedVsChannel", "Ped vs Channel; Channel; Ped", channels_bins, channels_min, channels_max, ped_bins, ped_min, ped_max);

    h2AdcVsTime = new TH2F("h2AdcVsTime", "Adc vs Time; Time; Adc", time_bins, time_min, time_max, adc_bins, adc_min, adc_max);
    h2PedVsTime = new TH2F("h2PedVsTime", "Ped vs Time; Time; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
    h2AdcVsPed = new TH2F("h2AdcVsPed", "Adc vs Ped; Ped; Adc", ped_bins, ped_min, ped_max, adc_bins, adc_min, adc_max);

    // scatter
    h2TimeVsChannel_scat = new TH2F("h2TimeVsChannel_scat", "Time vs Channel; Channel; Time", channels_bins, channels_min, channels_max, time_bins, time_min, time_max);
    h2AdcVsChannel_scat = new TH2F("h2AdcVsChannel_scat", "Adc vs Channel; Channel; Adc", channels_bins, channels_min, channels_max, adc_bins, adc_min, adc_max);
    h2PedVsChannel_scat = new TH2F("h2PedVsChannel_scat", "Ped vs Channel; Channel; Ped", channels_bins, channels_min, channels_max, ped_bins, ped_min, ped_max);

    h2AdcVsTime_scat = new TH2F("h2AdcVsTime_scat", "Adc vs Time; Time; Adc", time_bins, time_min, time_max, adc_bins, adc_min, adc_max);
    h2PedVsTime_scat = new TH2F("h2PedVsTime_scat", "Ped vs Time; Time; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
    h2AdcVsPed_scat = new TH2F("h2AdcVsPed_scat", "Adc vs Ped; Ped; Adc", ped_bins, ped_min, ped_max, adc_bins, adc_min, adc_max);
}

void myAnalysis::analyze(UInt_t nevents) {
    std::vector<Float_t>* time = 0;
    std::vector<Float_t>* adc = 0;
    std::vector<Float_t>* ped = 0;

    led_tree->SetBranchAddress("time",&time);
    led_tree->SetBranchAddress("adc",&adc);
    led_tree->SetBranchAddress("ped",&ped);

    // if nevents is 0 then use all events otherwise use the set number of events
    nevents = (nevents) ? nevents : led_tree->GetEntries();

    for(UInt_t i = 0; i < nevents; ++i) {
        if(i%100 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

        led_tree->GetEntry(i);
        UInt_t nchannels = time->size();
        hChannels->Fill(nchannels);
        channels_max = max(channels_max,nchannels);

        for(UInt_t channel = 0; channel < nchannels; ++channel) {
            Float_t time_val = time->at(channel);
            Float_t adc_val  = adc->at(channel);
            Float_t ped_val  = ped->at(channel);

            Int_t time_bin    = hTime->FindBin(time_val);
            Int_t adc_bin     = hAdc->FindBin(adc_val);
            Int_t ped_bin     = hPed->FindBin(ped_val);
            Int_t channel_bin = hChannels->FindBin(channel);

            // UInt_t key    = TowerInfoDefs::encode_emcal(channel);
            // UInt_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
            // UInt_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

            if(h2AdcVsTime_ch.size() < channel+1) {
                h2AdcVsTime_ch.push_back(new TH2F(("h2AdcVsTime_" + to_string(channel)).c_str(),
                                                   "Adc vs Time; Time; Adc",
                                                   time_bins, time_min, time_max,
                                                   adc_bins, adc_min, adc_max));
            }

            // h2AdcVsTime_ch[channel]->Fill(time_val,adc_val);
            h2AdcVsTime_ch[channel]->SetBinContent(time_bin, adc_bin, 1);

            hTime->Fill(time_val);
            hAdc->Fill(adc_val);
            hPed->Fill(ped_val);

            h2TimeVsChannel->Fill(channel, time_val);
            h2AdcVsChannel->Fill(channel, adc_val);
            h2PedVsChannel->Fill(channel, ped_val);

            h2TimeVsChannel_scat->SetBinContent(channel_bin, time_bin, 1);
            h2AdcVsChannel_scat->SetBinContent(channel_bin, adc_bin, 1);
            h2PedVsChannel_scat->SetBinContent(channel_bin, ped_bin, 1);

            h2AdcVsTime->Fill(time_val, adc_val);
            h2PedVsTime->Fill(time_val, ped_val);
            h2AdcVsPed->Fill(ped_val, adc_val);

            h2AdcVsTime_scat->SetBinContent(time_bin, adc_bin, 1);
            h2PedVsTime_scat->SetBinContent(time_bin, ped_bin, 1);
            h2AdcVsPed_scat->SetBinContent(ped_bin, adc_bin, 1);

            time_min = min(time_min, time_val);
            time_max = max(time_max, time_val);

            adc_min = min(adc_min, adc_val);
            adc_max = max(adc_max, adc_val);

            ped_min = min(ped_min, ped_val);
            ped_max = max(ped_max, ped_val);
        }
    }

    cout << "max channels per event: " << channels_max << endl;
    cout << "time_min: " << time_min << " time_max: " << time_max << endl;
    cout << "adc_min: " << adc_min << " adc_max: " << adc_max << endl;
    cout << "ped_min: " << ped_min << " ped_max: " << ped_max << endl;
}

void myAnalysis::finalize(const string& outputFile) {
    TFile output(outputFile.c_str(),"recreate");
    output.cd();

    hChannels->Write();
    hTime->Write();
    hAdc->Write();
    hPed->Write();

    h2TimeVsChannel->Write();
    h2AdcVsChannel->Write();
    h2PedVsChannel->Write();

    h2AdcVsTime->Write();
    h2PedVsTime->Write();
    h2AdcVsPed->Write();

    output.mkdir("scat");
    output.cd("scat");

    h2TimeVsChannel_scat->Write();
    h2AdcVsChannel_scat->Write();
    h2PedVsChannel_scat->Write();

    h2AdcVsTime_scat->Write();
    h2PedVsTime_scat->Write();
    h2AdcVsPed_scat->Write();

    output.cd();

    output.mkdir("AdcVsTime");
    output.cd("AdcVsTime");
    for(auto h2 : h2AdcVsTime_ch) h2->Write();

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
