void D0_BUP2020_pAu_v2_3yr()
{
//=========Macro generated from canvas: D0_BUP2020_pAu_v2_3yr/D0_BUP2020_pAu_v2_3yr
//=========  (Wed May 12 23:46:27 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020_pAu_v2_3yr = new TCanvas("D0_BUP2020_pAu_v2_3yr", "D0_BUP2020_pAu_v2_3yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020_pAu_v2_3yr->SetHighLightColor(2);
   D0_BUP2020_pAu_v2_3yr->Range(0,0,1,1);
   D0_BUP2020_pAu_v2_3yr->SetFillColor(0);
   D0_BUP2020_pAu_v2_3yr->SetBorderMode(0);
   D0_BUP2020_pAu_v2_3yr->SetBorderSize(2);
   D0_BUP2020_pAu_v2_3yr->SetTickx(1);
   D0_BUP2020_pAu_v2_3yr->SetTicky(1);
   D0_BUP2020_pAu_v2_3yr->SetLeftMargin(0.16);
   D0_BUP2020_pAu_v2_3yr->SetRightMargin(0.05);
   D0_BUP2020_pAu_v2_3yr->SetTopMargin(0.05);
   D0_BUP2020_pAu_v2_3yr->SetBottomMargin(0.16);
   D0_BUP2020_pAu_v2_3yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020_pAu_v2_3yr_1
   TPad *D0_BUP2020_pAu_v2_3yr_1 = new TPad("D0_BUP2020_pAu_v2_3yr_1", "D0_BUP2020_pAu_v2_3yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020_pAu_v2_3yr_1->Draw();
   D0_BUP2020_pAu_v2_3yr_1->cd();
   D0_BUP2020_pAu_v2_3yr_1->Range(-1.620253,-0.2512658,8.506329,0.3816456);
   D0_BUP2020_pAu_v2_3yr_1->SetFillColor(0);
   D0_BUP2020_pAu_v2_3yr_1->SetBorderMode(0);
   D0_BUP2020_pAu_v2_3yr_1->SetBorderSize(2);
   D0_BUP2020_pAu_v2_3yr_1->SetTickx(1);
   D0_BUP2020_pAu_v2_3yr_1->SetTicky(1);
   D0_BUP2020_pAu_v2_3yr_1->SetLeftMargin(0.16);
   D0_BUP2020_pAu_v2_3yr_1->SetRightMargin(0.05);
   D0_BUP2020_pAu_v2_3yr_1->SetTopMargin(0.05);
   D0_BUP2020_pAu_v2_3yr_1->SetBottomMargin(0.16);
   D0_BUP2020_pAu_v2_3yr_1->SetFrameBorderMode(0);
   D0_BUP2020_pAu_v2_3yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__26 = new TH1F("hframe__26","",1000,0,8);
   hframe__26->SetMinimum(-0.15);
   hframe__26->SetMaximum(0.35);
   hframe__26->SetDirectory(0);
   hframe__26->SetStats(0);
   hframe__26->SetLineWidth(2);
   hframe__26->SetMarkerStyle(20);
   hframe__26->SetMarkerSize(1.2);
   hframe__26->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__26->GetXaxis()->SetLabelFont(42);
   hframe__26->GetXaxis()->SetLabelSize(0.05);
   hframe__26->GetXaxis()->SetTitleSize(0.05);
   hframe__26->GetXaxis()->SetTitleOffset(1.4);
   hframe__26->GetXaxis()->SetTitleFont(42);
   hframe__26->GetYaxis()->SetTitle("v_{2}");
   hframe__26->GetYaxis()->SetLabelFont(42);
   hframe__26->GetYaxis()->SetLabelSize(0.05);
   hframe__26->GetYaxis()->SetTitleSize(0.05);
   hframe__26->GetYaxis()->SetTitleOffset(1.4);
   hframe__26->GetYaxis()->SetTitleFont(42);
   hframe__26->GetZaxis()->SetLabelFont(42);
   hframe__26->GetZaxis()->SetLabelSize(0.05);
   hframe__26->GetZaxis()->SetTitleSize(0.05);
   hframe__26->GetZaxis()->SetTitleOffset(1);
   hframe__26->GetZaxis()->SetTitleFont(42);
   hframe__26->Draw(" ");
   
   Double_t Graph0_fx1020[10] = {
   0.7,
   1.7,
   2.7,
   3.7,
   4.7,
   5.7,
   6.7,
   7.7,
   8.7,
   9.7};
   Double_t Graph0_fy1020[10] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph0_fex1020[10] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph0_fey1020[10] = {
   0.03555925,
   0.01970952,
   0.02182074,
   0.02907315,
   0.0488343,
   0.08149913,
   0.1357408,
   0.1940048,
   0.3183504,
   0.5042655};
   TGraphErrors *gre = new TGraphErrors(10,Graph0_fx1020,Graph0_fy1020,Graph0_fex1020,Graph0_fey1020);
   gre->SetName("Graph0");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   Int_t ci;      // for color index setting
   TColor *color; // for color definition with alpha
   ci = TColor::GetColor("#000000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01020 = new TH1F("Graph_Graph01020","Graph",100,0,10.6);
   Graph_Graph01020->SetMinimum(-0.6051186);
   Graph_Graph01020->SetMaximum(0.6051186);
   Graph_Graph01020->SetDirectory(0);
   Graph_Graph01020->SetStats(0);
   Graph_Graph01020->SetLineWidth(2);
   Graph_Graph01020->SetMarkerStyle(20);
   Graph_Graph01020->SetMarkerSize(1.2);
   Graph_Graph01020->GetXaxis()->SetLabelFont(42);
   Graph_Graph01020->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01020->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01020->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01020->GetXaxis()->SetTitleFont(42);
   Graph_Graph01020->GetYaxis()->SetLabelFont(42);
   Graph_Graph01020->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01020->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01020->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01020->GetYaxis()->SetTitleFont(42);
   Graph_Graph01020->GetZaxis()->SetLabelFont(42);
   Graph_Graph01020->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01020->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01020->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01020->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01020);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1021[1] = {
   4.8};
   Double_t Graph1_fy1021[1] = {
   0};
   Double_t Graph1_fex1021[1] = {
   0};
   Double_t Graph1_fey1021[1] = {
   0.251405};
   gre = new TGraphErrors(1,Graph1_fx1021,Graph1_fy1021,Graph1_fex1021,Graph1_fey1021);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000099");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11021 = new TH1F("Graph_Graph11021","Graph",100,4.7,5.9);
   Graph_Graph11021->SetMinimum(-0.301686);
   Graph_Graph11021->SetMaximum(0.301686);
   Graph_Graph11021->SetDirectory(0);
   Graph_Graph11021->SetStats(0);
   Graph_Graph11021->SetLineWidth(2);
   Graph_Graph11021->SetMarkerStyle(20);
   Graph_Graph11021->SetMarkerSize(1.2);
   Graph_Graph11021->GetXaxis()->SetLabelFont(42);
   Graph_Graph11021->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11021->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11021->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11021->GetXaxis()->SetTitleFont(42);
   Graph_Graph11021->GetYaxis()->SetLabelFont(42);
   Graph_Graph11021->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11021->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11021->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11021->GetYaxis()->SetTitleFont(42);
   Graph_Graph11021->GetZaxis()->SetLabelFont(42);
   Graph_Graph11021->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11021->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11021->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11021->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11021);
   
   gre->Draw("p");
   
   Double_t v2_D_fx34[20] = {
   0.25,
   0.75,
   1.25,
   1.75,
   2.25,
   2.75,
   3.25,
   3.75,
   4.25,
   4.75,
   5.25,
   5.75,
   6.25,
   6.75,
   7.25,
   7.75,
   8.25,
   8.75,
   9.25,
   9.75};
   Double_t v2_D_fy34[20] = {
   0.003230484,
   0.02045627,
   0.05875515,
   0.1035772,
   0.1351997,
   0.1468273,
   0.1426436,
   0.1296975,
   0.1135213,
   0.09734482,
   0.08267526,
   0.07001466,
   0.05936532,
   0.05052002,
   0.04320955,
   0.03716986,
   0.03216805,
   0.02800891,
   0.02453305,
   0.0216121};
   TGraph *graph = new TGraph(20,v2_D_fx34,v2_D_fy34);
   graph->SetName("v2_D");
   graph->SetTitle("Graph");

   ci = TColor::GetColor("#000000");
   graph->SetLineColor(ci);
   graph->SetLineStyle(2);
   graph->SetLineWidth(2);
   graph->SetMarkerStyle(20);
   graph->SetMarkerSize(1.2);
   
   TH1F *Graph_v2_D34 = new TH1F("Graph_v2_D34","Graph",100,0,10.7);
   Graph_v2_D34->SetMinimum(0);
   Graph_v2_D34->SetMaximum(0.1611869);
   Graph_v2_D34->SetDirectory(0);
   Graph_v2_D34->SetLineWidth(2);
   Graph_v2_D34->SetMarkerStyle(20);
   Graph_v2_D34->SetMarkerSize(1.2);
   Graph_v2_D34->GetXaxis()->SetLabelFont(42);
   Graph_v2_D34->GetXaxis()->SetLabelSize(0.05);
   Graph_v2_D34->GetXaxis()->SetTitleSize(0.05);
   Graph_v2_D34->GetXaxis()->SetTitleOffset(1.4);
   Graph_v2_D34->GetXaxis()->SetTitleFont(42);
   Graph_v2_D34->GetYaxis()->SetLabelFont(42);
   Graph_v2_D34->GetYaxis()->SetLabelSize(0.05);
   Graph_v2_D34->GetYaxis()->SetTitleSize(0.05);
   Graph_v2_D34->GetYaxis()->SetTitleOffset(1.4);
   Graph_v2_D34->GetYaxis()->SetTitleFont(42);
   Graph_v2_D34->GetZaxis()->SetLabelFont(42);
   Graph_v2_D34->GetZaxis()->SetLabelSize(0.05);
   Graph_v2_D34->GetZaxis()->SetTitleSize(0.05);
   Graph_v2_D34->GetZaxis()->SetTitleOffset(1);
   Graph_v2_D34->GetZaxis()->SetTitleFont(42);
   graph->SetHistogram(Graph_v2_D34);
   
   graph->Draw("");
   
   TLegend *leg = new TLegend(0.1,0.75,0.55,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, Years 1-3","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","M.B. p+Au, 10 nb^{-1} str.","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.65,0.65,0.9,0.8,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("Graph","Prompt #it{D}^{0}","lp");

   ci = TColor::GetColor("#000000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","#it{B}#rightarrow#it{D}^{0}","lp");

   ci = TColor::GetColor("#000099");
   entry->SetLineColor(ci);
   entry->SetLineStyle(1);
   entry->SetLineWidth(4);

   ci = TColor::GetColor("#000099");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.2,0.2,0.7,0.3,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("v2_D","#it{D}-meson (Au+Au)","l");

   ci = TColor::GetColor("#000000");
   entry->SetLineColor(ci);
   entry->SetLineStyle(2);
   entry->SetLineWidth(2);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020_pAu_v2_3yr_1->Modified();
   D0_BUP2020_pAu_v2_3yr->cd();
   D0_BUP2020_pAu_v2_3yr->Modified();
   D0_BUP2020_pAu_v2_3yr->cd();
   D0_BUP2020_pAu_v2_3yr->SetSelected(D0_BUP2020_pAu_v2_3yr);
}
