#include <calobase/TowerInfoDefs.h>
R__LOAD_LIBRARY(libcalo_io.so)

void display(const string& inputFile="output/test.root", const string& outputDir="output", const string& run = "0", bool do_waveforms = false) {
   TFile input(inputFile.c_str());

   bool style_eta_phi = true;
   auto c1 = new TCanvas();

   c1->cd();
   c1->SetTickx();
   c1->SetTicky();

   c1->SetCanvasSize(1500, 1000);

   c1->SetLeftMargin(.1);
   c1->SetRightMargin(.05);
   // gPad->SetLogz();

   string outputFile = outputDir + "/plots-" + run + ".pdf";
   c1->Print((outputFile + "[").c_str(), "pdf portrait");
   // ------------------------------------------------------------

   gPad->SetLogy();
   auto h1 = (TH1D*)(input.Get("hTime"))->Clone();
   h1->SetTitle(("Run: " + run + ", Peak Location").c_str());
   // h1->SetStats(0);
   // h1->GetYaxis()->SetRangeUser(9e3,1e7);
   h1->Draw();
   gPad->Update();

   auto st = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
   st->SetX1NDC(0.65); //new x start position
   st->SetX2NDC(0.95); //new x end position

   st->SetY1NDC(0.6); //new y start position
   st->SetY2NDC(0.90); //new y end position

   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h1 = (TH1D*)(input.Get("hADC"))->Clone();
   h1->SetTitle(("Run: " + run + ", ADC").c_str());
   h1->SetStats(0);
   // h1->GetYaxis()->SetRangeUser(5e-1,1e7);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h1 = (TH1D*)(input.Get("hadc"))->Clone();
   h1->SetTitle(("Run: " + run + ", adc").c_str());
   h1->SetStats(0);
   // h1->GetYaxis()->SetRangeUser(5e-1,1e7);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h1 = (TH1D*)(input.Get("hPed"))->Clone();
   h1->SetTitle(("Run: " + run + ", Pedestal").c_str());
   h1->SetStats(0);
   // h1->GetYaxis()->SetRangeUser(5e-1,1e7);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   // c1->SetLeftMargin(.1);
   // c1->SetRightMargin(.1);
   gPad->SetLogy(0);

   // auto h2 = (TH2D*)(input.Get("scat/h2TimeVsChannel_scat"))->Clone();
   // h2->SetStats(0);
   // h2->SetMarkerStyle(8);
   // h2->SetMarkerColor(kRed);
   // h2->SetFillColor(kRed);
   // h2->GetXaxis()->SetRangeUser(1400,3200);
   // h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.11);
   c1->SetRightMargin(.03);
   // h2 = (TH2D*)(input.Get("scat/h2ADCVsChannel_scat"))->Clone();
   auto h2 = (TH2D*)(input.Get("h2ADCVsChannel"))->Clone();
   h2->SetStats(0);
   // h2->SetMarkerStyle(8);
   // h2->SetMarkerColor(kRed);
   // h2->SetFillColor(kRed);
   // h2->GetXaxis()->SetRangeUser(1400,3200);
   // h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");

   // h2->GetYaxis()->SetRangeUser(0,4e3);
   // c1->Print((outputFile).c_str(), "pdf portrait");

   h1 = (TH1D*)h2->ProfileX()->Clone();
   h1->SetTitle(("Run: " + run + ", Average ADC vs Channel; Channel; ADC").c_str());
   h1->SetMarkerStyle(6);
   h1->SetMarkerColor(kRed);
   h1->SetStats(0);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // h1 = (TH1D*)((TH2*)input.Get("scat/h2ADCVsChannel_scat"))->ProfileX();
   h1 = (TH1D*)((TH2*)input.Get("h2ADCVsChannel"))->ProfileX();
   TH2D* h2_missing;
   if(style_eta_phi) {
      h2 = new TH2D("h2AvgAdc", "Average ADC; Towerid #eta; Towerid #phi", 96, 0-0.5, 96-0.5, 256, 0-0.5, 256-0.5);
      h2_missing = new TH2D("h2AvgAdcMissing", "Average ADC (Zeros); Towerid #eta; Towerid #phi", 96, 0-0.5, 96-0.5, 256, 0-0.5, 256-0.5);
   }
   else {
      h2 = new TH2D("h2AvgAdc", "Average ADC; Towerid #phi; Towerid #eta", 256, 0-0.5, 256-0.5, 96, 0-0.5, 96-0.5);
      h2_missing = new TH2D("h2AvgAdcMissing", "Average ADC (Zeros); Towerid #phi; Towerid #eta", 256, 0-0.5, 256-0.5, 96, 0-0.5, 96-0.5);
   }

   for (Int_t channel = 0; channel < 24576; ++channel) {
      Int_t key = TowerInfoDefs::encode_emcal(channel);
      Int_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
      Int_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

      Double_t avg_adc = h1->GetBinContent(channel+1);

      //h2->SetBinContent(etabin+1, phibin+1, avg_adc);
      if(style_eta_phi) {
         h2->SetBinContent(etabin+1, phibin+1, avg_adc);
         if(avg_adc == 0) h2_missing->SetBinContent(etabin+1, phibin+1, 1);
      }
      else {
         h2->SetBinContent(phibin+1, etabin+1, avg_adc);
         if(avg_adc == 0) h2_missing->SetBinContent(phibin+1, etabin+1, 1);
      }
      // if(avg_adc != 0) cout << "etabin: " << etabin << ", phibin: " << phibin << ", " << " channel: " << channel << ", avg_adc: " << avg_adc << endl;
   }

   c1->SetRightMargin(.12);
   h2->SetStats(0);
   h2->SetTitle(("Run: " + run + ", Average ADC").c_str());
   // h2->SetMaximum(5e3);
   h2->Draw("COLZ1");

   auto tline = new TLine();

   if(style_eta_phi) {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 95.5, 8*i-0.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 255.5);
      }
   }
   else {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 95.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 255.5, 8*i-0.5);
      }
   }

   c1->Print((outputFile).c_str(), "pdf portrait");

   // h2->GetYaxis()->SetRangeUser(0,50);
   // c1->Print((outputFile).c_str(), "pdf portrait");

   h2_missing->SetStats(0);
   h2_missing->SetTitle(("Run: " + run + ", Average ADC (zeros)").c_str());
   // h2->SetMaximum(4e3);
   h2_missing->Draw("COLZ1");

   tline = new TLine();

   if(style_eta_phi) {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 95.5, 8*i-0.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 255.5);
      }
   }
   else {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 95.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 255.5, 8*i-0.5);
      }
   }

   c1->Print((outputFile).c_str(), "pdf portrait");

   c1->SetRightMargin(.1);
   c1->SetLeftMargin(.1);

   // ------------------------------------------------------------

   c1->SetLeftMargin(.12);
   c1->SetRightMargin(.04);
   h2 = (TH2D*)(input.Get("h2PedVsChannel"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   // h2->GetXaxis()->SetRangeUser(1400,3200);
   h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");

   h1 = (TH1D*)h2->ProfileX()->Clone();
   h1->SetTitle(("Run: " + run + ", Average Pedestal vs Channel; Channel; Pedestal").c_str());
   h1->SetStats(0);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.1);

   h2 = (TH2D*)(input.Get("h2PedVsChannel"))->Clone();
   h1 = (TH1D*)h2->ProfileX()->Clone();

   if(style_eta_phi) {
      h2 = new TH2D("h2AvgPed", "Average Pedestal; Towerid #eta; Towerid #phi", 96, 0-0.5, 96-0.5, 256, 0-0.5, 256-0.5);
   }
   else {
      h2 = new TH2D("h2AvgPed", "Average Pedestal; Towerid #phi; Towerid #eta", 256, 0-0.5, 256-0.5, 96, 0-0.5, 96-0.5);
   }

   for (Int_t channel = 0; channel < 24576; ++channel) {
      Int_t key = TowerInfoDefs::encode_emcal(channel);
      Int_t etabin = TowerInfoDefs::getCaloTowerEtaBin(key);
      Int_t phibin = TowerInfoDefs::getCaloTowerPhiBin(key);

      Double_t avg_ped = h1->GetBinContent(channel+1);

      if(style_eta_phi) h2->SetBinContent(etabin+1, phibin+1, avg_ped);
      else              h2->SetBinContent(phibin+1, etabin+1, avg_ped);
      // if(avg_adc != 0) cout << "etabin: " << etabin << ", phibin: " << phibin << ", " << " channel: " << channel << ", avg_adc: " << avg_adc << endl;
   }

   c1->SetRightMargin(.12);
   h2->SetStats(0);
   h2->SetTitle(("Run: " + run + ", Average Pedestal").c_str());
   h2->SetMaximum(3e3);
   h2->Draw("COLZ1");

   tline = new TLine();

   if(style_eta_phi) {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 95.5, 8*i-0.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 255.5);
      }
   }
   else {
      for (UInt_t i = 0; i < 32; ++i) {
         tline->DrawLine(8*i-0.5, -0.5, 8*i-0.5, 95.5);
      }
      for (UInt_t i = 0; i < 12; ++i) {
         tline->DrawLine(-0.5, 8*i-0.5, 255.5, 8*i-0.5);
      }
   }

   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   // c1->SetLeftMargin(.11);
   // h2 = (TH2D*)(input.Get("scat/h2ADCVsTime_scat"))->Clone();
   // h2->SetStats(0);
   // h2->SetMarkerStyle(8);
   // h2->SetMarkerColor(kRed);
   // h2->SetFillColor(kRed);
   // h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");
   // c1->SetLeftMargin(.1);

   // ------------------------------------------------------------

   // h2 = (TH2D*)(input.Get("scat/h2PedVsTime_scat"))->Clone();
   // h2->SetStats(0);
   // h2->SetMarkerStyle(8);
   // h2->SetMarkerColor(kRed);
   // h2->SetFillColor(kRed);
   // h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   // c1->SetLeftMargin(.13);
   // h2 = (TH2D*)(input.Get("scat/h2ADCVsPed_scat"))->Clone();
   // h2->SetStats(0);
   // h2->SetMarkerStyle(8);
   // h2->SetMarkerColor(kRed);
   // h2->SetFillColor(kRed);
   // h2->Draw();
   // c1->Print((outputFile).c_str(), "pdf portrait");
   // c1->SetLeftMargin(.1);

   c1->Print((outputFile + "]").c_str(), "pdf portrait");
   // ------------------------------------------------------------

   if(do_waveforms) {

      gStyle->SetPalette(kThermometer);

      outputFile = outputDir + "/adcVsTime-" + run + ".pdf";
      // Total channels 24576
      Int_t i_max = 12; // 12
      Int_t j_max = 32; // 32

      // auto h2_pos_global = new TH2F("h2_pos_global", "Towers; Towerid #eta; Towerid #phi", 96, 0-0.5, 96-0.5, 256, 0-0.5, 256-0.5);
      auto h2_pos_global = new TH2F("h2_pos_global", "Towers; Towerid #phi; Towerid #eta", 256, 0-0.5, 256-0.5, 96, 0-0.5, 96-0.5);
      h2_pos_global->SetFillColor(kGreen);
      for(Int_t i = 0; i < i_max; ++i) {
         for (Int_t j = 0; j < j_max; ++j) {
            for (Int_t etabin = i*8+7; etabin >= i*8; --etabin) {
               for (Int_t phibin = j*8; phibin < j*8+8; ++phibin) {
                  UInt_t key = TowerInfoDefs::encode_emcal(etabin, phibin);
                  UInt_t channel = TowerInfoDefs::decode_emcal(key);
                  h2 = (TH2D*)(input.Get(("adcVsTime/h2adcVsTime_"+to_string(channel)).c_str()));
                  if(h2) h2_pos_global->SetBinContent(phibin+1,etabin+1, 1);
                  // if(h2) h2_pos_global->SetBinContent(phibin+1,etabin+1, 1);
               }
            }
         }
      }

      // c1->Print("output/adcVsTime.pdf[", "pdf portrait");
      c1->Print((outputFile + "[").c_str(), "pdf portrait");
      for(Int_t i = 0; i < i_max; ++i) {
         for (Int_t j = 0; j < j_max; ++j) {
            if((i*j_max+j)%3 == 0) cout << "Progress: " << (i*j_max+j)*100./(i_max*j_max) << " %" << endl;

            c1 = new TCanvas();
            c1->SetCanvasSize(1500, 1000);
            auto pad1 = new TPad("pad1", "pad1", 0.01, 0.01, 0.8, 1);
            auto pad2 = new TPad("pad2", "pad2", 0.8, 0.8, 1, 1);
            // auto h2_pos = new TH2F("h2_pos", "Towers; Towerid #eta; Towerid #phi", 96, 0-0.5, 96-0.5, 256, 0-0.5, 256-0.5);
            auto h2_pos = new TH2F("h2_pos", "Towers; Towerid #phi; Towerid #eta", 256, 0-0.5, 256-0.5, 96, 0-0.5, 96-0.5);
            pad1->DivideSquare(64,0,0);
            pad1->Draw();
            pad2->Draw();

            bool contains_channels = false;
            UInt_t pad = 1;
            for (Int_t etabin = i*8+7; etabin >= i*8; --etabin) {
               for (Int_t phibin = j*8; phibin < j*8+8; ++phibin) {
                  UInt_t key = TowerInfoDefs::encode_emcal(etabin, phibin);
                  UInt_t channel = TowerInfoDefs::decode_emcal(key);
                  h2 = (TH2D*)(input.Get(("adcVsTime/h2adcVsTime_"+to_string(channel)).c_str()));
                  if(h2) {
                     h2_pos->SetBinContent(phibin+1,etabin+1, 1);
                     // h2_pos->SetBinContent(phibin+1,etabin+1, 1);
                     pad1->cd(pad);
                     contains_channels = true;

                     h2 = (TH2D*)h2->Clone();
                     // h1 = (TH1D*)h2->ProfileX()->Clone();
                     gPad->SetTickx();
                     gPad->SetTicky();
                     // h1->SetTitle("");
                     // h1->SetStats(0);
                     // h1->GetYaxis()->SetRangeUser(0,17e3);
                     // h1->SetMarkerStyle(8);
                     // h1->SetMarkerSize(0.3);
                     // h1->SetMarkerColor(kRed);
                     h2->SetTitle("");
                     h2->SetStats(0);
                     // h2->GetYaxis()->SetRangeUser(0,17e3);
                     // h2->SetMarkerStyle(8);
                     // h2->SetMarkerSize(0.3);
                     // h2->SetMarkerColor(kRed);
                     h2->Draw("COL");
                     // h2->Draw();

                     auto pt = new TPaveText(.7,.7,.95,.9, "NDC");
                     pt->AddText((to_string(phibin) + ", " + to_string(etabin)).c_str());
                     // pt->AddText((to_string(phibin) + ", " + to_string(etabin)).c_str());
                     pt->Draw();
                  }
                  // cout << "etabin: " << etabin << ", phibin: " << phibin << ", channel: " << channel << ", pad: " << pad << endl;
                  ++pad;
               }
            }
            if(contains_channels) {
               pad2->cd();
               pad2->SetTickx();
               pad2->SetTicky();

               h2_pos_global->SetStats(0);
               h2_pos_global->Draw("BOX");
               h2_pos->Draw("BOX same");
               // c1->Print("output/adcVsTime.pdf", "pdf portrait");
               c1->Print((outputFile).c_str(), "pdf portrait");
               // c1->Print(("output/adcVsTime-"+to_string(i) + "-" + to_string(j) + ".png").c_str(), "png portrait");
               // ++pages;
            }
            delete h2_pos;
            delete pad1;
            delete pad2;
         }
      }
      // c1->Print("output/adcVsTime.pdf]", "pdf portrait");
      c1->Print((outputFile + "]").c_str(), "pdf portrait");
   }

   input.Close();
}
