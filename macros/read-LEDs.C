// -- c++ includes --
#include <string>
#include <vector>

// -- root includes --
#include "TTree.h"
#include "TFile.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void read_LEDs(const string &inputFile="data/LEDTowerBuilder.root",
               const string &outputFile="output/test.root",
               Int_t nevents=0) {

   TFile input(inputFile.c_str());
   auto led_tree = (TTree*)input.Get("W");

   std::vector<Float_t>* time = 0;
   std::vector<Float_t>* adc = 0;
   std::vector<Float_t>* ped = 0;

   led_tree->SetBranchAddress("time",&time);
   led_tree->SetBranchAddress("adc",&adc);
   led_tree->SetBranchAddress("ped",&ped);

   // if nevents is 0 then use all events otherwise use the set number of events
   nevents = (nevents) ? nevents : led_tree->GetEntries();

   UInt_t  time_bins    = 60;
   Float_t time_min     = 0;
   Float_t time_max     = 30;

   UInt_t  adc_bins     = 120;
   Float_t adc_min      = 0;
   Float_t adc_max      = 12000;

   UInt_t  ped_bins     = 60;
   Float_t ped_min      = 1300;
   Float_t ped_max      = 1900;

   UInt_t channels_bins = 1600;
   UInt_t channels_min  = 0;
   UInt_t channels_max  = 1.6e6;

   // QA

   auto hTime = new TH1F("hTime", "Time; Time; Counts", time_bins, time_min, time_max);
   auto hAdc = new TH1F("hAdc", "Adc; Adc; Counts", adc_bins, adc_min, adc_max);
   auto hPed = new TH1F("hPed", "Ped; Ped; Counts", ped_bins, ped_min, ped_max);
   auto hChannels = new TH1F("hChannels", "Channels per Event; Channels; Counts", channels_bins, channels_min, channels_max);

   // 2D correlations

   auto h2TimeVsChannel = new TH2F("h2TimeVsChannel", "Time vs Channel; Channel; Time", channels_bins, channels_min, channels_max, time_bins, time_min, time_max);
   auto h2AdcVsChannel = new TH2F("h2AdcVsChannel", "Adc vs Channel; Channel; Adc", channels_bins, channels_min, channels_max, adc_bins, adc_min, adc_max);
   auto h2PedVsChannel = new TH2F("h2PedVsChannel", "Ped vs Channel; Channel; Ped", channels_bins, channels_min, channels_max, ped_bins, ped_min, ped_max);

   auto h2AdcVsTime = new TH2F("h2AdcVsTime", "Adc vs Time; Time; Adc", time_bins, time_min, time_max, adc_bins, adc_min, adc_max);
   auto h2PedVsTime = new TH2F("h2PedVsTime", "Ped vs Time; Time; Ped", time_bins, time_min, time_max, ped_bins, ped_min, ped_max);
   auto h2AdcVsPed = new TH2F("h2AdcVsPed", "Adc vs Ped; Ped; Adc", ped_bins, ped_min, ped_max, adc_bins, adc_min, adc_max);

   for(UInt_t i = 0; i < nevents; ++i) {
      if(i%100 == 0) cout << "Progress: " << i*100./nevents << " %" << endl;

      led_tree->GetEntry(i);
      UInt_t nchannels = time->size();
      hChannels->Fill(nchannels);
      if(channels_max < nchannels) channels_max = nchannels;

      for(UInt_t channel = 0; channel < nchannels; ++channel) {
          Float_t time_val = time->at(channel);
          Float_t adc_val  = adc->at(channel);
          Float_t ped_val  = ped->at(channel);

          hTime->Fill(time_val);
          hAdc->Fill(adc_val);
          hPed->Fill(ped_val);

          h2TimeVsChannel->Fill(channel, time_val);
          h2AdcVsChannel->Fill(channel, adc_val);
          h2PedVsChannel->Fill(channel, ped_val);

          h2AdcVsTime->Fill(time_val, adc_val);
          h2PedVsTime->Fill(time_val, ped_val);
          h2AdcVsPed->Fill(ped_val, adc_val);

          if(time_min > time_val) time_min = time_val;
          if(time_max < time_val) time_max = time_val;

          if(adc_min > adc_val) adc_min = adc_val;
          if(adc_max < adc_val) adc_max = adc_val;

          if(ped_min > ped_val) ped_min = ped_val;
          if(ped_max < ped_val) ped_max = ped_val;
      }
   }

   cout << "max channels per event: " << channels_max << endl;
   cout << "time_min: " << time_min << " time_max: " << time_max << endl;
   cout << "adc_min: " << adc_min << " adc_max: " << adc_max << endl;
   cout << "ped_min: " << ped_min << " ped_max: " << ped_max << endl;

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

   // Close root file
   input.Close();
   output.Close();
}
