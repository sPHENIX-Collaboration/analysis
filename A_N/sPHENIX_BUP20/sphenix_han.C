void sphenix_han()
{
//=========Macro generated from canvas: sphenix_han/sphenix_han
//=========  (Fri Aug 28 14:59:02 2020) by ROOT version 6.16/00
   TCanvas *sphenix_han = new TCanvas("sphenix_han", "sphenix_han",621,585,1100,800);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   sphenix_han->Range(0,0,1,1);
   sphenix_han->SetFillColor(0);
   sphenix_han->SetBorderMode(0);
   sphenix_han->SetBorderSize(2);
   sphenix_han->SetTickx(1);
   sphenix_han->SetTicky(1);
   sphenix_han->SetLeftMargin(0.16);
   sphenix_han->SetRightMargin(0.05);
   sphenix_han->SetTopMargin(0.05);
   sphenix_han->SetBottomMargin(0.16);
   sphenix_han->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: sphenix_han_1
   TPad *sphenix_han_1 = new TPad("sphenix_han_1", "sphenix_han_1",0.01,0.01,0.99,0.99);
   sphenix_han_1->Draw();
   sphenix_han_1->cd();
   sphenix_han_1->Range(-2.43038,-0.01316456,12.75949,0.06911392);
   sphenix_han_1->SetFillColor(0);
   sphenix_han_1->SetBorderMode(0);
   sphenix_han_1->SetBorderSize(2);
   sphenix_han_1->SetTickx(1);
   sphenix_han_1->SetTicky(1);
   sphenix_han_1->SetLeftMargin(0.16);
   sphenix_han_1->SetRightMargin(0.05);
   sphenix_han_1->SetTopMargin(0.05);
   sphenix_han_1->SetBottomMargin(0.16);
   sphenix_han_1->SetFrameBorderMode(0);
   sphenix_han_1->SetFrameBorderMode(0);
   
   TH1F *hframe__1 = new TH1F("hframe__1","",1000,0,12);
   hframe__1->SetMinimum(-0);
   hframe__1->SetMaximum(0.065);
   hframe__1->SetDirectory(0);
   hframe__1->SetStats(0);
   hframe__1->SetLineWidth(2);
   hframe__1->SetMarkerStyle(20);
   hframe__1->SetMarkerSize(1.2);
   hframe__1->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__1->GetXaxis()->SetLabelFont(42);
   hframe__1->GetXaxis()->SetLabelSize(0.05);
   hframe__1->GetXaxis()->SetTitleSize(0.05);
   hframe__1->GetXaxis()->SetTitleOffset(1.4);
   hframe__1->GetXaxis()->SetTitleFont(42);
   hframe__1->GetYaxis()->SetTitle("A_{N}");
   hframe__1->GetYaxis()->SetLabelFont(42);
   hframe__1->GetYaxis()->SetLabelSize(0.05);
   hframe__1->GetYaxis()->SetTitleSize(0.05);
   hframe__1->GetYaxis()->SetTitleOffset(1.4);
   hframe__1->GetYaxis()->SetTitleFont(42);
   hframe__1->GetZaxis()->SetLabelFont(42);
   hframe__1->GetZaxis()->SetLabelSize(0.05);
   hframe__1->GetZaxis()->SetTitleSize(0.05);
   hframe__1->GetZaxis()->SetTitleOffset(1);
   hframe__1->GetZaxis()->SetTitleFont(42);
   hframe__1->Draw(" ");
   TArrow *arrow = new TArrow(1.8,0.025,7,0.025,0.025,"<|>");
   arrow->SetFillColor(29);
   arrow->SetFillStyle(1001);
   arrow->SetLineColor(29);
   arrow->SetLineWidth(10);
   arrow->Draw();
   arrow = new TArrow(4.4,0.0213,4.4,0.0287,0.025,"<|>");
   arrow->SetFillColor(29);
   arrow->SetFillStyle(1001);
   arrow->SetLineColor(29);
   arrow->SetLineWidth(10);
   arrow->Draw();
   
   Double_t Graph0_fx1001[5] = {
   1,
   3,
   5,
   7,
   9};
   Double_t Graph0_fy1001[5] = {
   0.01,
   0.01,
   0.01,
   0.01,
   0.01};
   Double_t Graph0_fex1001[5] = {
   0,
   0,
   0,
   0,
   0};
   Double_t Graph0_fey1001[5] = {
   0.0001695041,
   0.0001827427,
   0.0008802428,
   0.003831069,
   0.01164666};
   TGraphErrors *gre = new TGraphErrors(5,Graph0_fx1001,Graph0_fy1001,Graph0_fex1001,Graph0_fey1001);
   gre->SetName("Graph0");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#cc0000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc0000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01001 = new TH1F("Graph_Graph01001","Graph",100,0.2,9.8);
   Graph_Graph01001->SetMinimum(-0.003975996);
   Graph_Graph01001->SetMaximum(0.023976);
   Graph_Graph01001->SetDirectory(0);
   Graph_Graph01001->SetStats(0);
   Graph_Graph01001->SetLineWidth(2);
   Graph_Graph01001->SetMarkerStyle(20);
   Graph_Graph01001->SetMarkerSize(1.2);
   Graph_Graph01001->GetXaxis()->SetLabelFont(42);
   Graph_Graph01001->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01001->GetXaxis()->SetTitleFont(42);
   Graph_Graph01001->GetYaxis()->SetLabelFont(42);
   Graph_Graph01001->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01001->GetYaxis()->SetTitleFont(42);
   Graph_Graph01001->GetZaxis()->SetLabelFont(42);
   Graph_Graph01001->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01001->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01001->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01001->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01001);
   
   gre->Draw("p");
   TLatex *   tex = new TLatex(9.5,0.0097,"x_{F}=0.05-0.10");
   tex->SetTextFont(42);
   tex->SetTextSize(0.035);
   tex->SetLineWidth(2);
   tex->Draw();
   
   Double_t Graph1_fx1002[4] = {
   1,
   3,
   5,
   7};
   Double_t Graph1_fy1002[4] = {
   0.02,
   0.02,
   0.02,
   0.02};
   Double_t Graph1_fex1002[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1002[4] = {
   0.006527352,
   0.0008190469,
   0.001524963,
   0.005482558};
   gre = new TGraphErrors(4,Graph1_fx1002,Graph1_fy1002,Graph1_fex1002,Graph1_fey1002);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#cc0000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc0000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11002 = new TH1F("Graph_Graph11002","Graph",100,0.4,7.6);
   Graph_Graph11002->SetMinimum(0.01216718);
   Graph_Graph11002->SetMaximum(0.02783282);
   Graph_Graph11002->SetDirectory(0);
   Graph_Graph11002->SetStats(0);
   Graph_Graph11002->SetLineWidth(2);
   Graph_Graph11002->SetMarkerStyle(20);
   Graph_Graph11002->SetMarkerSize(1.2);
   Graph_Graph11002->GetXaxis()->SetLabelFont(42);
   Graph_Graph11002->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11002->GetXaxis()->SetTitleFont(42);
   Graph_Graph11002->GetYaxis()->SetLabelFont(42);
   Graph_Graph11002->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11002->GetYaxis()->SetTitleFont(42);
   Graph_Graph11002->GetZaxis()->SetLabelFont(42);
   Graph_Graph11002->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11002->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11002->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11002->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11002);
   
   gre->Draw("p");
      tex = new TLatex(9.5,0.0197,"x_{F}=0.10-0.15");
   tex->SetTextFont(42);
   tex->SetTextSize(0.035);
   tex->SetLineWidth(2);
   tex->Draw();
   
   Double_t Graph2_fx1003[3] = {
   3,
   5,
   7};
   Double_t Graph2_fy1003[3] = {
   0.03,
   0.03,
   0.03};
   Double_t Graph2_fex1003[3] = {
   0,
   0,
   0};
   Double_t Graph2_fey1003[3] = {
   0.004575613,
   0.003459797,
   0.008584451};
   gre = new TGraphErrors(3,Graph2_fx1003,Graph2_fy1003,Graph2_fex1003,Graph2_fey1003);
   gre->SetName("Graph2");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#cc0000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc0000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph21003 = new TH1F("Graph_Graph21003","Graph",100,2.6,7.4);
   Graph_Graph21003->SetMinimum(0.01969866);
   Graph_Graph21003->SetMaximum(0.04030134);
   Graph_Graph21003->SetDirectory(0);
   Graph_Graph21003->SetStats(0);
   Graph_Graph21003->SetLineWidth(2);
   Graph_Graph21003->SetMarkerStyle(20);
   Graph_Graph21003->SetMarkerSize(1.2);
   Graph_Graph21003->GetXaxis()->SetLabelFont(42);
   Graph_Graph21003->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21003->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21003->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21003->GetXaxis()->SetTitleFont(42);
   Graph_Graph21003->GetYaxis()->SetLabelFont(42);
   Graph_Graph21003->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21003->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21003->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21003->GetYaxis()->SetTitleFont(42);
   Graph_Graph21003->GetZaxis()->SetLabelFont(42);
   Graph_Graph21003->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21003->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21003->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21003->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph21003);
   
   gre->Draw("p");
      tex = new TLatex(9.5,0.0297,"x_{F}=0.15-0.20");
   tex->SetTextFont(42);
   tex->SetTextSize(0.035);
   tex->SetLineWidth(2);
   tex->Draw();
   
   Double_t Graph3_fx1004[1] = {
   5};
   Double_t Graph3_fy1004[1] = {
   0.04};
   Double_t Graph3_fex1004[1] = {
   0};
   Double_t Graph3_fey1004[1] = {
   0.01008074};
   gre = new TGraphErrors(1,Graph3_fx1004,Graph3_fy1004,Graph3_fex1004,Graph3_fey1004);
   gre->SetName("Graph3");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#cc0000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc0000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph31004 = new TH1F("Graph_Graph31004","Graph",100,4.9,6.1);
   Graph_Graph31004->SetMinimum(0.02790311);
   Graph_Graph31004->SetMaximum(0.05209689);
   Graph_Graph31004->SetDirectory(0);
   Graph_Graph31004->SetStats(0);
   Graph_Graph31004->SetLineWidth(2);
   Graph_Graph31004->SetMarkerStyle(20);
   Graph_Graph31004->SetMarkerSize(1.2);
   Graph_Graph31004->GetXaxis()->SetLabelFont(42);
   Graph_Graph31004->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph31004->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph31004->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph31004->GetXaxis()->SetTitleFont(42);
   Graph_Graph31004->GetYaxis()->SetLabelFont(42);
   Graph_Graph31004->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph31004->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph31004->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph31004->GetYaxis()->SetTitleFont(42);
   Graph_Graph31004->GetZaxis()->SetLabelFont(42);
   Graph_Graph31004->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph31004->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph31004->GetZaxis()->SetTitleOffset(1);
   Graph_Graph31004->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph31004);
   
   gre->Draw("p");
      tex = new TLatex(9.5,0.0397,"x_{F}=0.20-0.25");
   tex->SetTextFont(42);
   tex->SetTextSize(0.035);
   tex->SetLineWidth(2);
   tex->Draw();
      tex = new TLatex(0.4,0.0025,"Shown only points with #delta(A_{N}) <~ 1%");
   tex->SetTextFont(42);
   tex->SetTextSize(0.035);
   tex->SetLineWidth(2);
   tex->Draw();
   
   TLegend *leg = new TLegend(0,0.75,0.83,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("","#it{#bf{sPHENIX}} Projection, Years 1-3","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("","6.2 pb^{-1} str. #it{p}^{#uparrow}+#it{p}#rightarrow h^{+} + X, P=0.57","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   sphenix_han_1->Modified();
   sphenix_han->cd();
   sphenix_han->Modified();
   sphenix_han->cd();
   sphenix_han->SetSelected(sphenix_han);
}
