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
   gPad->SetLogz();

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

   auto h2 = (TH2D*)(input.Get("h2TimeVsChannel"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   c1->SetLeftMargin(.11);
   c1->SetRightMargin(.1);
   h2 = (TH2D*)(input.Get("h2AdcVsChannel"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("h2PedVsChannel"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("h2AdcVsTime"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("h2PedVsTime"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile).c_str(), "pdf portrait");

   // ------------------------------------------------------------

   h2 = (TH2D*)(input.Get("h2AdcVsPed"))->Clone();
   h2->SetStats(0);
   h2->Draw("COLZ1");
   c1->Print((outputFile + ")").c_str(), "pdf portrait");

   // ------------------------------------------------------------

   input.Close();
}
