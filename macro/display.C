void display(){
   const string inputFile="output/test.root";
   const string outputFile="output/plots.pdf";

   TFile input(inputFile.c_str());

   auto c1 = new TCanvas();

   c1->cd();
   c1->SetTickx();
   c1->SetTicky();

   c1->SetCanvasSize(1500, 1000);

   c1->SetLeftMargin(.1);
   c1->SetRightMargin(.05);
   // gPad->SetLogz();

   // ------------------------------------------------------------

   gPad->SetLogy();
   auto h1 = (TH1D*)(input.Get("hTime"))->Clone();
   // h1->SetStats(0);
   h1->GetYaxis()->SetRangeUser(9e3,1e7);
   h1->Draw();
   gPad->Update();

   auto st = (TPaveStats*)h1->GetListOfFunctions()->FindObject("stats");
   st->SetX1NDC(0.65); //new x start position
   st->SetX2NDC(0.95); //new x end position

   st->SetY1NDC(0.6); //new y start position
   st->SetY2NDC(0.90); //new y end position

   c1->Print((outputFile + "(").c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h1 = (TH1D*)(input.Get("hAdc"))->Clone();
   h1->SetStats(0);
   h1->GetYaxis()->SetRangeUser(5e-1,1e7);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h1 = (TH1D*)(input.Get("hPed"))->Clone();
   h1->SetStats(0);
   h1->GetYaxis()->SetRangeUser(5e-1,1e7);
   h1->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.1);
   c1->SetRightMargin(.1);
   gPad->SetLogy(0);

   auto h2 = (TH2D*)(input.Get("scat/h2TimeVsChannel_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.11);
   // c1->SetRightMargin(.1);
   h2 = (TH2D*)(input.Get("scat/h2AdcVsChannel_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   h2->GetYaxis()->SetRangeUser(0,4e3);
   c1->Print((outputFile).c_str(), "pdf portrait");

   c1->SetLeftMargin(.1);

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("scat/h2PedVsChannel_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.11);
   h2 = (TH2D*)(input.Get("scat/h2AdcVsTime_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");
   c1->SetLeftMargin(.1);

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("scat/h2PedVsTime_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.11);
   h2 = (TH2D*)(input.Get("scat/h2AdcVsPed_scat"))->Clone();
   h2->SetStats(0);
   h2->SetMarkerStyle(8);
   h2->SetMarkerColor(kRed);
   h2->SetFillColor(kRed);
   h2->Draw();
   c1->Print((outputFile + ")").c_str(), "pdf portrait");
   c1->SetLeftMargin(.1);

   // ------------------------------------------------------------

   // Total channels 1536
   UInt_t i_max = 24; // max 24
   UInt_t j_max = 64;

   c1 = new TCanvas();

   c1->SetCanvasSize(1500, 1000);
   c1->DivideSquare(j_max,0,0);

   for(UInt_t i = 0; i < i_max; ++i) {
      for(UInt_t j = 0; j < j_max; ++j) {
         Int_t channel = j_max*i+j;
         c1->cd(j+1);
         gPad->SetTickx();
         gPad->SetTicky();
         h2 = (TH2D*)(input.Get(("AdcVsTime/h2AdcVsTime_"+to_string(channel)).c_str()))->Clone();
         h2->SetTitle("");
         h2->SetStats(0);
         h2->GetYaxis()->SetRangeUser(0,4e3);
         h2->SetMarkerStyle(8);
         h2->SetMarkerColor(kRed);
         h2->Draw();

         auto pt = new TPaveText(.7,.7,.95,.9, "NDC");
         pt->AddText((to_string(channel)).c_str());
         pt->Draw();
      }
      if(i == 0) c1->Print("output/AdcVsTime.pdf(", "pdf portrait");
      else if(i == i_max-1) c1->Print("output/AdcVsTime.pdf)", "pdf portrait");
      else c1->Print("output/AdcVsTime.pdf", "pdf portrait");
   }

   input.Close();
}
