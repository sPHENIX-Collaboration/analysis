void D0_BUP2020OOArAr_v2_5yr()
{
//=========Macro generated from canvas: D0_BUP2020OOArAr_v2_5yr/D0_BUP2020OOArArO_v2_5yr
//=========  (Fri Aug 28 00:20:15 2020) by ROOT version 6.16/00
   TCanvas *D0_BUP2020OOArAr_v2_5yr = new TCanvas("D0_BUP2020OOArAr_v2_5yr", "D0_BUP2020OOArArO_v2_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020OOArAr_v2_5yr->SetHighLightColor(2);
   D0_BUP2020OOArAr_v2_5yr->Range(0,0,1,1);
   D0_BUP2020OOArAr_v2_5yr->SetFillColor(0);
   D0_BUP2020OOArAr_v2_5yr->SetBorderMode(0);
   D0_BUP2020OOArAr_v2_5yr->SetBorderSize(2);
   D0_BUP2020OOArAr_v2_5yr->SetTickx(1);
   D0_BUP2020OOArAr_v2_5yr->SetTicky(1);
   D0_BUP2020OOArAr_v2_5yr->SetLeftMargin(0.16);
   D0_BUP2020OOArAr_v2_5yr->SetRightMargin(0.05);
   D0_BUP2020OOArAr_v2_5yr->SetTopMargin(0.05);
   D0_BUP2020OOArAr_v2_5yr->SetBottomMargin(0.16);
   D0_BUP2020OOArAr_v2_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020OOArAr_v2_5yr_1
   TPad *D0_BUP2020OOArAr_v2_5yr_1 = new TPad("D0_BUP2020OOArAr_v2_5yr_1", "D0_BUP2020OOArAr_v2_5yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020OOArAr_v2_5yr_1->Draw();
   D0_BUP2020OOArAr_v2_5yr_1->cd();
   D0_BUP2020OOArAr_v2_5yr_1->Range(-2.227848,-0.3512658,11.6962,0.2816456);
   D0_BUP2020OOArAr_v2_5yr_1->SetFillColor(0);
   D0_BUP2020OOArAr_v2_5yr_1->SetBorderMode(0);
   D0_BUP2020OOArAr_v2_5yr_1->SetBorderSize(2);
   D0_BUP2020OOArAr_v2_5yr_1->SetTickx(1);
   D0_BUP2020OOArAr_v2_5yr_1->SetTicky(1);
   D0_BUP2020OOArAr_v2_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020OOArAr_v2_5yr_1->SetRightMargin(0.05);
   D0_BUP2020OOArAr_v2_5yr_1->SetTopMargin(0.05);
   D0_BUP2020OOArAr_v2_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020OOArAr_v2_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020OOArAr_v2_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__30 = new TH1F("hframe__30","",1000,0,11);
   hframe__30->SetMinimum(-0.25);
   hframe__30->SetMaximum(0.25);
   hframe__30->SetDirectory(0);
   hframe__30->SetStats(0);
   hframe__30->SetLineWidth(2);
   hframe__30->SetMarkerStyle(20);
   hframe__30->SetMarkerSize(1.2);
   hframe__30->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__30->GetXaxis()->SetLabelFont(42);
   hframe__30->GetXaxis()->SetLabelSize(0.05);
   hframe__30->GetXaxis()->SetTitleSize(0.05);
   hframe__30->GetXaxis()->SetTitleOffset(1.4);
   hframe__30->GetXaxis()->SetTitleFont(42);
   hframe__30->GetYaxis()->SetTitle("v_{2}");
   hframe__30->GetYaxis()->SetLabelFont(42);
   hframe__30->GetYaxis()->SetLabelSize(0.05);
   hframe__30->GetYaxis()->SetTitleSize(0.05);
   hframe__30->GetYaxis()->SetTitleOffset(1.4);
   hframe__30->GetYaxis()->SetTitleFont(42);
   hframe__30->GetZaxis()->SetLabelFont(42);
   hframe__30->GetZaxis()->SetLabelSize(0.05);
   hframe__30->GetZaxis()->SetTitleSize(0.05);
   hframe__30->GetZaxis()->SetTitleOffset(1);
   hframe__30->GetZaxis()->SetTitleFont(42);
   hframe__30->Draw(" ");
   
   Double_t Graph0_fx1052[10] = {
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
   Double_t Graph0_fy1052[10] = {
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
   Double_t Graph0_fex1052[10] = {
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
   Double_t Graph0_fey1052[10] = {
   0.00907488,
   0.005029957,
   0.00556875,
   0.007419596,
   0.01246273,
   0.02079894,
   0.03464166,
   0.04951089,
   0.08124443,
   0.1286908};
   TGraphErrors *gre = new TGraphErrors(10,Graph0_fx1052,Graph0_fy1052,Graph0_fex1052,Graph0_fey1052);
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
   gre->SetMarkerStyle(24);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph01052 = new TH1F("Graph_Graph01052","Graph",100,0,10.6);
   Graph_Graph01052->SetMinimum(-0.154429);
   Graph_Graph01052->SetMaximum(0.154429);
   Graph_Graph01052->SetDirectory(0);
   Graph_Graph01052->SetStats(0);
   Graph_Graph01052->SetLineWidth(2);
   Graph_Graph01052->SetMarkerStyle(20);
   Graph_Graph01052->SetMarkerSize(1.2);
   Graph_Graph01052->GetXaxis()->SetLabelFont(42);
   Graph_Graph01052->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01052->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01052->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01052->GetXaxis()->SetTitleFont(42);
   Graph_Graph01052->GetYaxis()->SetLabelFont(42);
   Graph_Graph01052->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01052->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01052->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01052->GetYaxis()->SetTitleFont(42);
   Graph_Graph01052->GetZaxis()->SetLabelFont(42);
   Graph_Graph01052->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01052->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01052->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01052->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01052);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1053[2] = {
   2.3,
   7.3};
   Double_t Graph1_fy1053[2] = {
   0,
   0};
   Double_t Graph1_fex1053[2] = {
   0,
   0};
   Double_t Graph1_fey1053[2] = {
   0.07649507,
   0.1178262};
   gre = new TGraphErrors(2,Graph1_fx1053,Graph1_fy1053,Graph1_fex1053,Graph1_fey1053);
   gre->SetName("Graph1");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000099");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(25);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph11053 = new TH1F("Graph_Graph11053","Graph",100,1.8,7.8);
   Graph_Graph11053->SetMinimum(-0.1413914);
   Graph_Graph11053->SetMaximum(0.1413914);
   Graph_Graph11053->SetDirectory(0);
   Graph_Graph11053->SetStats(0);
   Graph_Graph11053->SetLineWidth(2);
   Graph_Graph11053->SetMarkerStyle(20);
   Graph_Graph11053->SetMarkerSize(1.2);
   Graph_Graph11053->GetXaxis()->SetLabelFont(42);
   Graph_Graph11053->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11053->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11053->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11053->GetXaxis()->SetTitleFont(42);
   Graph_Graph11053->GetYaxis()->SetLabelFont(42);
   Graph_Graph11053->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11053->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11053->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11053->GetYaxis()->SetTitleFont(42);
   Graph_Graph11053->GetZaxis()->SetLabelFont(42);
   Graph_Graph11053->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11053->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11053->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11053->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11053);
   
   gre->Draw("p");
   
   Double_t Graph2_fx1054[10] = {
   0.5,
   1.5,
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph2_fy1054[10] = {
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
   Double_t Graph2_fex1054[10] = {
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
   Double_t Graph2_fey1054[10] = {
   0.004800957,
   0.002661039,
   0.002946081,
   0.003925249,
   0.006593259,
   0.01100343,
   0.01832676,
   0.02619315,
   0.0429814,
   0.06808234};
   gre = new TGraphErrors(10,Graph2_fx1054,Graph2_fy1054,Graph2_fex1054,Graph2_fey1054);
   gre->SetName("Graph2");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000000");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000000");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(20);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph21054 = new TH1F("Graph_Graph21054","Graph",100,0,10.4);
   Graph_Graph21054->SetMinimum(-0.0816988);
   Graph_Graph21054->SetMaximum(0.0816988);
   Graph_Graph21054->SetDirectory(0);
   Graph_Graph21054->SetStats(0);
   Graph_Graph21054->SetLineWidth(2);
   Graph_Graph21054->SetMarkerStyle(20);
   Graph_Graph21054->SetMarkerSize(1.2);
   Graph_Graph21054->GetXaxis()->SetLabelFont(42);
   Graph_Graph21054->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21054->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21054->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21054->GetXaxis()->SetTitleFont(42);
   Graph_Graph21054->GetYaxis()->SetLabelFont(42);
   Graph_Graph21054->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21054->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21054->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21054->GetYaxis()->SetTitleFont(42);
   Graph_Graph21054->GetZaxis()->SetLabelFont(42);
   Graph_Graph21054->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21054->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21054->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21054->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph21054);
   
   gre->Draw("p");
   
   Double_t Graph3_fx1055[2] = {
   2.1,
   7.1};
   Double_t Graph3_fy1055[2] = {
   0,
   0};
   Double_t Graph3_fex1055[2] = {
   0,
   0};
   Double_t Graph3_fey1055[2] = {
   0.04046881,
   0.06233454};
   gre = new TGraphErrors(2,Graph3_fx1055,Graph3_fy1055,Graph3_fex1055,Graph3_fey1055);
   gre->SetName("Graph3");
   gre->SetTitle("Graph");
   gre->SetFillStyle(1000);

   ci = TColor::GetColor("#000099");
   gre->SetLineColor(ci);
   gre->SetLineWidth(4);

   ci = TColor::GetColor("#000099");
   gre->SetMarkerColor(ci);
   gre->SetMarkerStyle(21);
   gre->SetMarkerSize(2);
   
   TH1F *Graph_Graph31055 = new TH1F("Graph_Graph31055","Graph",100,1.6,7.6);
   Graph_Graph31055->SetMinimum(-0.07480145);
   Graph_Graph31055->SetMaximum(0.07480145);
   Graph_Graph31055->SetDirectory(0);
   Graph_Graph31055->SetStats(0);
   Graph_Graph31055->SetLineWidth(2);
   Graph_Graph31055->SetMarkerStyle(20);
   Graph_Graph31055->SetMarkerSize(1.2);
   Graph_Graph31055->GetXaxis()->SetLabelFont(42);
   Graph_Graph31055->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph31055->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph31055->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph31055->GetXaxis()->SetTitleFont(42);
   Graph_Graph31055->GetYaxis()->SetLabelFont(42);
   Graph_Graph31055->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph31055->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph31055->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph31055->GetYaxis()->SetTitleFont(42);
   Graph_Graph31055->GetZaxis()->SetLabelFont(42);
   Graph_Graph31055->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph31055->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph31055->GetZaxis()->SetTitleOffset(1);
   Graph_Graph31055->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph31055);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0.1,0.8,0.55,0.9,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   TLegendEntry *entry=leg->AddEntry("NULL","#it{#bf{sPHENIX}} Projection, Year 1-5","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.2,0.2,1,0.4,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","Prompt #it{D}^{0}","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph"," ","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(24);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph"," ","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.4,0.2,0.65,0.4,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","#it{B}#rightarrow#it{D}^{0}","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","37 nb^{-1} str. O+O, Res(#Psi_{2})=0.3","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000099");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(25);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","12 nb^{-1} str. Ar+Ar, Res(#Psi_{2})=0.4","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000099");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020OOArAr_v2_5yr_1->Modified();
   D0_BUP2020OOArAr_v2_5yr->cd();
   D0_BUP2020OOArAr_v2_5yr->Modified();
   D0_BUP2020OOArAr_v2_5yr->cd();
   D0_BUP2020OOArAr_v2_5yr->SetSelected(D0_BUP2020OOArAr_v2_5yr);
}
