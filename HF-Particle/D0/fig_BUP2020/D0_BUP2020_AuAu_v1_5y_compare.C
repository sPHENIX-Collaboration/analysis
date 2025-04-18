void D0_BUP2020_AuAu_v1_5y_compare()
{
//=========Macro generated from canvas: D0_BUP2020_AuAu_v1_5y_compare/D0_BUP2020_AuAu_v1_5y_compare
//=========  (Thu Aug 27 18:01:29 2020) by ROOT version 6.16/00
   TCanvas *D0_BUP2020_AuAu_v1_5y_compare = new TCanvas("D0_BUP2020_AuAu_v1_5y_compare", "D0_BUP2020_AuAu_v1_5y_compare",52,585,1100,800);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_AuAu_v1_5y_compare->Range(0,0,1,1);
   D0_BUP2020_AuAu_v1_5y_compare->SetFillColor(0);
   D0_BUP2020_AuAu_v1_5y_compare->SetBorderMode(0);
   D0_BUP2020_AuAu_v1_5y_compare->SetBorderSize(2);
   D0_BUP2020_AuAu_v1_5y_compare->SetTickx(1);
   D0_BUP2020_AuAu_v1_5y_compare->SetTicky(1);
   D0_BUP2020_AuAu_v1_5y_compare->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v1_5y_compare->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v1_5y_compare->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v1_5y_compare->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v1_5y_compare->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_AuAu_v1_5y_compare_1
   TPad *D0_BUP2020_AuAu_v1_5y_compare_1 = new TPad("D0_BUP2020_AuAu_v1_5y_compare_1", "D0_BUP2020_AuAu_v1_5y_compare_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_AuAu_v1_5y_compare_1->Draw();
   D0_BUP2020_AuAu_v1_5y_compare_1->cd();
   D0_BUP2020_AuAu_v1_5y_compare_1->Range(-1.686076,-0.1405063,1.351899,0.1126582);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetFillColor(0);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetBorderMode(0);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetBorderSize(2);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetTickx(1);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetTicky(1);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetLeftMargin(0.16);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetRightMargin(0.05);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetTopMargin(0.05);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetBottomMargin(0.16);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetFrameBorderMode(0);
   D0_BUP2020_AuAu_v1_5y_compare_1->SetFrameBorderMode(0);
   
   TH1D *d0__1 = new TH1D("d0__1","",1,-1.2,1.2);
   d0__1->SetMinimum(-0.1);
   d0__1->SetMaximum(0.1);
   d0__1->SetStats(0);
   d0__1->SetLineWidth(2);
   d0__1->SetMarkerStyle(20);
   d0__1->SetMarkerSize(1.2);
   d0__1->GetXaxis()->SetTitle("Rapidity");
   d0__1->GetXaxis()->SetLabelFont(42);
   d0__1->GetXaxis()->SetLabelSize(0.05);
   d0__1->GetXaxis()->SetTitleSize(0.05);
   d0__1->GetXaxis()->SetTitleOffset(1.4);
   d0__1->GetXaxis()->SetTitleFont(42);
   d0__1->GetYaxis()->SetTitle("v_{1}");
   d0__1->GetYaxis()->SetLabelFont(42);
   d0__1->GetYaxis()->SetLabelSize(0.05);
   d0__1->GetYaxis()->SetTitleSize(0.05);
   d0__1->GetYaxis()->SetTitleOffset(1.4);
   d0__1->GetYaxis()->SetTitleFont(42);
   d0__1->GetZaxis()->SetLabelFont(42);
   d0__1->GetZaxis()->SetLabelSize(0.05);
   d0__1->GetZaxis()->SetTitleSize(0.05);
   d0__1->GetZaxis()->SetTitleOffset(1);
   d0__1->GetZaxis()->SetTitleFont(42);
   d0__1->Draw("c");
   TLine *line = new TLine(-1.2,0,1.2,0);
   line->Draw();
   
   Double_t Graph0_fx1001[4] = {
   -0.62,
   -0.22,
   0.18,
   0.58};
   Double_t Graph0_fy1001[4] = {
   0.067,
   0.029,
   -0.032,
   -0.029};
   Double_t Graph0_fex1001[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph0_fey1001[4] = {
   0.024,
   0.02,
   0.018,
   0.021};
   TGraphErrors *gre = new TGraphErrors(4,Graph0_fx1001,Graph0_fy1001,Graph0_fex1001,Graph0_fey1001);
   gre->SetName("Graph0");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#cc6666");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(29);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01001 = new TH1F("Graph_Graph01001","Graph",100,-0.74,0.7);
   Graph_Graph01001->SetMinimum(-0.0641);
   Graph_Graph01001->SetMaximum(0.1051);
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
   
   Double_t Graph1_fx1002[4] = {
   -0.58,
   -0.18,
   0.22,
   0.62};
   Double_t Graph1_fy1002[4] = {
   0.07,
   0.008,
   -0.029,
   -0.022};
   Double_t Graph1_fex1002[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1002[4] = {
   0.024,
   0.019,
   0.018,
   0.02};
   gre = new TGraphErrors(4,Graph1_fx1002,Graph1_fy1002,Graph1_fex1002,Graph1_fey1002);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#cc6666");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(30);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11002 = new TH1F("Graph_Graph11002","Graph",100,-0.7,0.74);
   Graph_Graph11002->SetMinimum(-0.0611);
   Graph_Graph11002->SetMaximum(0.1081);
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
   
   Double_t Graph2_fx1[8] = {
   -0.875,
   -0.625,
   -0.375,
   -0.125,
   0.125,
   0.375,
   0.625,
   0.875};
   Double_t Graph2_fy1[8] = {
   0.03942156,
   0.03067408,
   0.01988809,
   0.008240108,
   -0.00452453,
   -0.01728107,
   -0.03025653,
   -0.03917671};
   TGraph *graph = new TGraph(8,Graph2_fx1,Graph2_fy1);
   graph->SetName("Graph2");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(2);
   
   TH1F *Graph_Graph21 = new TH1F("Graph_Graph21","Graph",100,-1.05,1.05);
   Graph_Graph21->SetMinimum(-0.04703654);
   Graph_Graph21->SetMaximum(0.04728138);
   Graph_Graph21->SetDirectory(0);
   Graph_Graph21->SetStats(0);
   Graph_Graph21->SetLineWidth(2);
   Graph_Graph21->SetMarkerStyle(20);
   Graph_Graph21->SetMarkerSize(1.2);
   Graph_Graph21->GetXaxis()->SetLabelFont(42);
   Graph_Graph21->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21->GetXaxis()->SetTitleFont(42);
   Graph_Graph21->GetYaxis()->SetLabelFont(42);
   Graph_Graph21->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21->GetYaxis()->SetTitleFont(42);
   Graph_Graph21->GetZaxis()->SetLabelFont(42);
   Graph_Graph21->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph21);
   
   graph->Draw("c");
   
   Double_t Graph3_fx2[8] = {
   -0.875,
   -0.625,
   -0.375,
   -0.125,
   0.125,
   0.375,
   0.625,
   0.875};
   Double_t Graph3_fy2[8] = {
   0.02645328,
   0.02204966,
   0.01529134,
   0.006949292,
   -0.00231199,
   -0.01082532,
   -0.0186751,
   -0.02261987};
   graph = new TGraph(8,Graph3_fx2,Graph3_fy2);
   graph->SetName("Graph3");
   graph->SetTitle("Graph");
   graph->SetFillStyle(1000);
   graph->SetLineColor(4);
   graph->SetLineWidth(2);
   graph->SetMarkerColor(4);
   graph->SetMarkerStyle(24);
   graph->SetMarkerSize(2);
   
   TH1F *Graph_Graph32 = new TH1F("Graph_Graph32","Graph",100,-1.05,1.05);
   Graph_Graph32->SetMinimum(-0.02752718);
   Graph_Graph32->SetMaximum(0.03136059);
   Graph_Graph32->SetDirectory(0);
   Graph_Graph32->SetStats(0);
   Graph_Graph32->SetLineWidth(2);
   Graph_Graph32->SetMarkerStyle(20);
   Graph_Graph32->SetMarkerSize(1.2);
   Graph_Graph32->GetXaxis()->SetLabelFont(42);
   Graph_Graph32->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph32->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph32->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph32->GetXaxis()->SetTitleFont(42);
   Graph_Graph32->GetYaxis()->SetLabelFont(42);
   Graph_Graph32->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph32->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph32->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph32->GetYaxis()->SetTitleFont(42);
   Graph_Graph32->GetZaxis()->SetLabelFont(42);
   Graph_Graph32->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph32->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph32->GetZaxis()->SetTitleOffset(1);
   Graph_Graph32->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_Graph32);
   
   graph->Draw("c");
   
   Double_t Graph4_fx1003[4] = {
   -0.825,
   -0.575,
   -0.325,
   -0.075};
   Double_t Graph4_fy1003[4] = {
   0.03942156,
   0.03067408,
   0.01988809,
   0.008240108};
   Double_t Graph4_fex1003[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph4_fey1003[4] = {
   0.005125373,
   0.005127926,
   0.005130196,
   0.005131559};
   gre = new TGraphErrors(4,Graph4_fx1003,Graph4_fy1003,Graph4_fex1003,Graph4_fey1003);
   gre->SetName("Graph4");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#666666");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#666666");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph41003 = new TH1F("Graph_Graph41003","Graph",100,-0.9,0);
   Graph_Graph41003->SetMinimum(0.002797694);
   Graph_Graph41003->SetMaximum(0.04869077);
   Graph_Graph41003->SetDirectory(0);
   Graph_Graph41003->SetStats(0);
   Graph_Graph41003->SetLineWidth(2);
   Graph_Graph41003->SetMarkerStyle(20);
   Graph_Graph41003->SetMarkerSize(1.2);
   Graph_Graph41003->GetXaxis()->SetLabelFont(42);
   Graph_Graph41003->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph41003->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph41003->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph41003->GetXaxis()->SetTitleFont(42);
   Graph_Graph41003->GetYaxis()->SetLabelFont(42);
   Graph_Graph41003->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph41003->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph41003->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph41003->GetYaxis()->SetTitleFont(42);
   Graph_Graph41003->GetZaxis()->SetLabelFont(42);
   Graph_Graph41003->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph41003->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph41003->GetZaxis()->SetTitleOffset(1);
   Graph_Graph41003->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph41003);
   
   gre->Draw("p");
   
   Double_t Graph5_fx1004[4] = {
   -0.825,
   -0.575,
   -0.325,
   -0.075};
   Double_t Graph5_fy1004[4] = {
   0.02645328,
   0.02204966,
   0.01529134,
   0.006949292};
   Double_t Graph5_fex1004[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph5_fey1004[4] = {
   0.00512893,
   0.005129819,
   0.005130869,
   0.005131641};
   gre = new TGraphErrors(4,Graph5_fx1004,Graph5_fy1004,Graph5_fex1004,Graph5_fey1004);
   gre->SetName("Graph5");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#6666cc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#6666cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph51004 = new TH1F("Graph_Graph51004","Graph",100,-0.9,0);
   Graph_Graph51004->SetMinimum(0.001635886);
   Graph_Graph51004->SetMaximum(0.03455866);
   Graph_Graph51004->SetDirectory(0);
   Graph_Graph51004->SetStats(0);
   Graph_Graph51004->SetLineWidth(2);
   Graph_Graph51004->SetMarkerStyle(20);
   Graph_Graph51004->SetMarkerSize(1.2);
   Graph_Graph51004->GetXaxis()->SetLabelFont(42);
   Graph_Graph51004->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph51004->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph51004->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph51004->GetXaxis()->SetTitleFont(42);
   Graph_Graph51004->GetYaxis()->SetLabelFont(42);
   Graph_Graph51004->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph51004->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph51004->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph51004->GetYaxis()->SetTitleFont(42);
   Graph_Graph51004->GetZaxis()->SetLabelFont(42);
   Graph_Graph51004->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph51004->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph51004->GetZaxis()->SetTitleOffset(1);
   Graph_Graph51004->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph51004);
   
   gre->Draw("p");
   
   Double_t Graph6_fx1005[4] = {
   0.175,
   0.425,
   0.675,
   0.925};
   Double_t Graph6_fy1005[4] = {
   -0.00452453,
   -0.01728107,
   -0.03025653,
   -0.03917671};
   Double_t Graph6_fex1005[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph6_fey1005[4] = {
   0.005131757,
   0.005130599,
   0.005128032,
   0.005125453};
   gre = new TGraphErrors(4,Graph6_fx1005,Graph6_fy1005,Graph6_fex1005,Graph6_fey1005);
   gre->SetName("Graph6");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#666666");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#666666");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph61005 = new TH1F("Graph_Graph61005","Graph",100,0.1,1);
   Graph_Graph61005->SetMinimum(-0.0487931);
   Graph_Graph61005->SetMaximum(0.005098165);
   Graph_Graph61005->SetDirectory(0);
   Graph_Graph61005->SetStats(0);
   Graph_Graph61005->SetLineWidth(2);
   Graph_Graph61005->SetMarkerStyle(20);
   Graph_Graph61005->SetMarkerSize(1.2);
   Graph_Graph61005->GetXaxis()->SetLabelFont(42);
   Graph_Graph61005->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph61005->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph61005->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph61005->GetXaxis()->SetTitleFont(42);
   Graph_Graph61005->GetYaxis()->SetLabelFont(42);
   Graph_Graph61005->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph61005->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph61005->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph61005->GetYaxis()->SetTitleFont(42);
   Graph_Graph61005->GetZaxis()->SetLabelFont(42);
   Graph_Graph61005->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph61005->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph61005->GetZaxis()->SetTitleOffset(1);
   Graph_Graph61005->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph61005);
   
   gre->Draw("p");
   
   Double_t Graph7_fx1006[4] = {
   0.175,
   0.425,
   0.675,
   0.925};
   Double_t Graph7_fy1006[4] = {
   -0.00231199,
   -0.01082532,
   -0.0186751,
   -0.02261987};
   Double_t Graph7_fex1006[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph7_fey1006[4] = {
   0.00513182,
   0.005131354,
   0.005130391,
   0.005129713};
   gre = new TGraphErrors(4,Graph7_fx1006,Graph7_fy1006,Graph7_fex1006,Graph7_fey1006);
   gre->SetName("Graph7");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#6666cc");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#6666cc");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph71006 = new TH1F("Graph_Graph71006","Graph",100,0.1,1);
   Graph_Graph71006->SetMinimum(-0.03080652);
   Graph_Graph71006->SetMaximum(0.00587677);
   Graph_Graph71006->SetDirectory(0);
   Graph_Graph71006->SetStats(0);
   Graph_Graph71006->SetLineWidth(2);
   Graph_Graph71006->SetMarkerStyle(20);
   Graph_Graph71006->SetMarkerSize(1.2);
   Graph_Graph71006->GetXaxis()->SetLabelFont(42);
   Graph_Graph71006->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph71006->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph71006->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph71006->GetXaxis()->SetTitleFont(42);
   Graph_Graph71006->GetYaxis()->SetLabelFont(42);
   Graph_Graph71006->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph71006->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph71006->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph71006->GetYaxis()->SetTitleFont(42);
   Graph_Graph71006->GetZaxis()->SetLabelFont(42);
   Graph_Graph71006->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph71006->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph71006->GetZaxis()->SetTitleOffset(1);
   Graph_Graph71006->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph71006);
   
   gre->Draw("p");
   
   Double_t Graph8_fx1007[4] = {
   -0.875,
   -0.625,
   -0.375,
   -0.125};
   Double_t Graph8_fy1007[4] = {
   0.03942156,
   0.03067408,
   0.01988809,
   0.008240108};
   Double_t Graph8_fex1007[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph8_fey1007[4] = {
   0.003274965,
   0.003276597,
   0.003278047,
   0.003278918};
   gre = new TGraphErrors(4,Graph8_fx1007,Graph8_fy1007,Graph8_fex1007,Graph8_fey1007);
   gre->SetName("Graph8");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);
   gre->SetLineWidth(4);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph81007 = new TH1F("Graph_Graph81007","Graph",100,-0.95,-0.05);
   Graph_Graph81007->SetMinimum(0.001187657);
   Graph_Graph81007->SetMaximum(0.04647006);
   Graph_Graph81007->SetDirectory(0);
   Graph_Graph81007->SetStats(0);
   Graph_Graph81007->SetLineWidth(2);
   Graph_Graph81007->SetMarkerStyle(20);
   Graph_Graph81007->SetMarkerSize(1.2);
   Graph_Graph81007->GetXaxis()->SetLabelFont(42);
   Graph_Graph81007->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph81007->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph81007->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph81007->GetXaxis()->SetTitleFont(42);
   Graph_Graph81007->GetYaxis()->SetLabelFont(42);
   Graph_Graph81007->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph81007->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph81007->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph81007->GetYaxis()->SetTitleFont(42);
   Graph_Graph81007->GetZaxis()->SetLabelFont(42);
   Graph_Graph81007->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph81007->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph81007->GetZaxis()->SetTitleOffset(1);
   Graph_Graph81007->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph81007);
   
   gre->Draw("p");
   
   Double_t Graph9_fx1008[4] = {
   -0.875,
   -0.625,
   -0.375,
   -0.125};
   Double_t Graph9_fy1008[4] = {
   0.02645328,
   0.02204966,
   0.01529134,
   0.006949292};
   Double_t Graph9_fex1008[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph9_fey1008[4] = {
   0.003277238,
   0.003277806,
   0.003278477,
   0.00327897};
   gre = new TGraphErrors(4,Graph9_fx1008,Graph9_fy1008,Graph9_fex1008,Graph9_fey1008);
   gre->SetName("Graph9");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);
   gre->SetLineColor(4);
   gre->SetLineWidth(4);
   gre->SetMarkerColor(4);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph91008 = new TH1F("Graph_Graph91008","Graph",100,-0.95,-0.05);
   Graph_Graph91008->SetMinimum(0.001064302);
   Graph_Graph91008->SetMaximum(0.03233654);
   Graph_Graph91008->SetDirectory(0);
   Graph_Graph91008->SetStats(0);
   Graph_Graph91008->SetLineWidth(2);
   Graph_Graph91008->SetMarkerStyle(20);
   Graph_Graph91008->SetMarkerSize(1.2);
   Graph_Graph91008->GetXaxis()->SetLabelFont(42);
   Graph_Graph91008->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph91008->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph91008->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph91008->GetXaxis()->SetTitleFont(42);
   Graph_Graph91008->GetYaxis()->SetLabelFont(42);
   Graph_Graph91008->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph91008->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph91008->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph91008->GetYaxis()->SetTitleFont(42);
   Graph_Graph91008->GetZaxis()->SetLabelFont(42);
   Graph_Graph91008->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph91008->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph91008->GetZaxis()->SetTitleOffset(1);
   Graph_Graph91008->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph91008);
   
   gre->Draw("p");
   
   Double_t Graph10_fx1009[4] = {
   0.125,
   0.375,
   0.625,
   0.875};
   Double_t Graph10_fy1009[4] = {
   -0.00452453,
   -0.01728107,
   -0.03025653,
   -0.03917671};
   Double_t Graph10_fex1009[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph10_fey1009[4] = {
   0.003279044,
   0.003278305,
   0.003276664,
   0.003275016};
   gre = new TGraphErrors(4,Graph10_fx1009,Graph10_fy1009,Graph10_fex1009,Graph10_fey1009);
   gre->SetName("Graph10");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);
   gre->SetLineWidth(4);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph101009 = new TH1F("Graph_Graph101009","Graph",100,0.05,0.95);
   Graph_Graph101009->SetMinimum(-0.04657235);
   Graph_Graph101009->SetMaximum(0.002875138);
   Graph_Graph101009->SetDirectory(0);
   Graph_Graph101009->SetStats(0);
   Graph_Graph101009->SetLineWidth(2);
   Graph_Graph101009->SetMarkerStyle(20);
   Graph_Graph101009->SetMarkerSize(1.2);
   Graph_Graph101009->GetXaxis()->SetLabelFont(42);
   Graph_Graph101009->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph101009->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph101009->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph101009->GetXaxis()->SetTitleFont(42);
   Graph_Graph101009->GetYaxis()->SetLabelFont(42);
   Graph_Graph101009->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph101009->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph101009->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph101009->GetYaxis()->SetTitleFont(42);
   Graph_Graph101009->GetZaxis()->SetLabelFont(42);
   Graph_Graph101009->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph101009->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph101009->GetZaxis()->SetTitleOffset(1);
   Graph_Graph101009->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph101009);
   
   gre->Draw("p");
   
   Double_t Graph11_fx1010[4] = {
   0.125,
   0.375,
   0.625,
   0.875};
   Double_t Graph11_fy1010[4] = {
   -0.00231199,
   -0.01082532,
   -0.0186751,
   -0.02261987};
   Double_t Graph11_fex1010[4] = {
   0,
   0,
   0,
   0};
   Double_t Graph11_fey1010[4] = {
   0.003279084,
   0.003278787,
   0.003278171,
   0.003277738};
   gre = new TGraphErrors(4,Graph11_fx1010,Graph11_fy1010,Graph11_fex1010,Graph11_fey1010);
   gre->SetName("Graph11");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);
   gre->SetLineColor(4);
   gre->SetLineWidth(4);
   gre->SetMarkerColor(4);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph111010 = new TH1F("Graph_Graph111010","Graph",100,0.05,0.95);
   Graph_Graph111010->SetMinimum(-0.02858408);
   Graph_Graph111010->SetMaximum(0.003653564);
   Graph_Graph111010->SetDirectory(0);
   Graph_Graph111010->SetStats(0);
   Graph_Graph111010->SetLineWidth(2);
   Graph_Graph111010->SetMarkerStyle(20);
   Graph_Graph111010->SetMarkerSize(1.2);
   Graph_Graph111010->GetXaxis()->SetLabelFont(42);
   Graph_Graph111010->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph111010->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph111010->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph111010->GetXaxis()->SetTitleFont(42);
   Graph_Graph111010->GetYaxis()->SetLabelFont(42);
   Graph_Graph111010->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph111010->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph111010->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph111010->GetYaxis()->SetTitleFont(42);
   Graph_Graph111010->GetZaxis()->SetLabelFont(42);
   Graph_Graph111010->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph111010->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph111010->GetZaxis()->SetTitleOffset(1);
   Graph_Graph111010->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph111010);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0.3,0.77,0.9,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, Years 1-5","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","0-80% Au+Au, Res(#Psi_{1})=0.37","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.2,0.18,0.4,0.4,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","D^{0}","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph6"," ","pe");

   ci = TColor::GetColor("#666666");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#666666");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(24);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph10"," ","pe");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph0"," ","pe");

   ci = TColor::GetColor("#cc6666");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(29);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.25,0.18,0.5,0.4,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL"," #bar{D}^{0}","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph7","sPHENIX, Year 1-3, 21nb^{-1} rec. Au+Au","pe");

   ci = TColor::GetColor("#6666cc");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#6666cc");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(24);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph11","sPHENIX, Year 1-5, 51nb^{-1} rec.+str. Au+Au","pe");
   entry->SetLineColor(4);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);
   entry->SetMarkerColor(4);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph1","STAR, 10-80% Au+Au, PRL#bf{123}, 162301","pe");

   ci = TColor::GetColor("#cc6666");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#cc6666");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(30);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020_AuAu_v1_5y_compare_1->Modified();
   D0_BUP2020_AuAu_v1_5y_compare->cd();
   D0_BUP2020_AuAu_v1_5y_compare->Modified();
   D0_BUP2020_AuAu_v1_5y_compare->cd();
   D0_BUP2020_AuAu_v1_5y_compare->SetSelected(D0_BUP2020_AuAu_v1_5y_compare);
}
