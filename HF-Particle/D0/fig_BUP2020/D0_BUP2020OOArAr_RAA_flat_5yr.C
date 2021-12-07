void D0_BUP2020OOArAr_RAA_flat_5yr()
{
//=========Macro generated from canvas: D0_BUP2020OOArAr_RAA_flat_5yr/D0_BUP2020OOArArO_RAA_flat_5yr
//=========  (Wed May 12 23:46:25 2021) by ROOT version 6.22/02
   TCanvas *D0_BUP2020OOArAr_RAA_flat_5yr = new TCanvas("D0_BUP2020OOArAr_RAA_flat_5yr", "D0_BUP2020OOArArO_RAA_flat_5yr",0,0,1100,800);
   gStyle->SetOptFit(1);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetHighLightColor(2);
   D0_BUP2020OOArAr_RAA_flat_5yr->Range(0,0,1,1);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetFillColor(0);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetBorderMode(0);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetBorderSize(2);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetTickx(1);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetTicky(1);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetLeftMargin(0.16);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetRightMargin(0.05);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetTopMargin(0.05);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetBottomMargin(0.16);
   D0_BUP2020OOArAr_RAA_flat_5yr->SetFrameBorderMode(0);
  
// ------------>Primitives in pad: D0_BUP2020OOArAr_RAA_flat_5yr_1
   TPad *D0_BUP2020OOArAr_RAA_flat_5yr_1 = new TPad("D0_BUP2020OOArAr_RAA_flat_5yr_1", "D0_BUP2020OOArAr_RAA_flat_5yr_1",0.01,0.01,0.99,0.99);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->Draw();
   D0_BUP2020OOArAr_RAA_flat_5yr_1->cd();
   D0_BUP2020OOArAr_RAA_flat_5yr_1->Range(-2.227848,-0.243038,11.6962,1.275949);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetFillColor(0);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetBorderMode(0);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetBorderSize(2);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetTickx(1);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetTicky(1);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetLeftMargin(0.16);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetRightMargin(0.05);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetTopMargin(0.05);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetBottomMargin(0.16);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetFrameBorderMode(0);
   D0_BUP2020OOArAr_RAA_flat_5yr_1->SetFrameBorderMode(0);
   
   TH1F *hframe__21 = new TH1F("hframe__21","",1000,0,11);
   hframe__21->SetMinimum(0);
   hframe__21->SetMaximum(1.2);
   hframe__21->SetDirectory(0);
   hframe__21->SetStats(0);
   hframe__21->SetLineWidth(2);
   hframe__21->SetMarkerStyle(20);
   hframe__21->SetMarkerSize(1.2);
   hframe__21->GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
   hframe__21->GetXaxis()->SetLabelFont(42);
   hframe__21->GetXaxis()->SetLabelSize(0.05);
   hframe__21->GetXaxis()->SetTitleSize(0.05);
   hframe__21->GetXaxis()->SetTitleOffset(1.4);
   hframe__21->GetXaxis()->SetTitleFont(42);
   hframe__21->GetYaxis()->SetTitle("#it{R}_{AA}");
   hframe__21->GetYaxis()->SetLabelFont(42);
   hframe__21->GetYaxis()->SetLabelSize(0.05);
   hframe__21->GetYaxis()->SetTitleSize(0.05);
   hframe__21->GetYaxis()->SetTitleOffset(1.4);
   hframe__21->GetYaxis()->SetTitleFont(42);
   hframe__21->GetZaxis()->SetLabelFont(42);
   hframe__21->GetZaxis()->SetLabelSize(0.05);
   hframe__21->GetZaxis()->SetTitleSize(0.05);
   hframe__21->GetZaxis()->SetTitleOffset(1);
   hframe__21->GetZaxis()->SetTitleFont(42);
   hframe__21->Draw(" ");
   
   Double_t Graph0_fx1007[20] = {
   0.45,
   0.95,
   1.45,
   1.95,
   2.45,
   2.95,
   3.45,
   3.95,
   4.45,
   4.95,
   5.45,
   5.95,
   6.45,
   6.95,
   7.45,
   7.95,
   8.45,
   8.95,
   9.45,
   9.95};
   Double_t Graph0_fy1007[20] = {
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7,
   0.7};
   Double_t Graph0_fex1007[20] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
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
   Double_t Graph0_fey1007[20] = {
   0.004701433,
   0.003046274,
   0.002084179,
   0.001932339,
   0.002065075,
   0.002412436,
   0.002660087,
   0.003379354,
   0.004395106,
   0.005836379,
   0.007412599,
   0.009566098,
   0.01235944,
   0.01590416,
   0.01764852,
   0.02276481,
   0.02914109,
   0.03697689,
   0.04646279,
   0.05796681};
   TGraphErrors *gre = new TGraphErrors(20,Graph0_fx1007,Graph0_fy1007,Graph0_fex1007,Graph0_fey1007);
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
   
   TH1F *Graph_Graph01007 = new TH1F("Graph_Graph01007","Graph",100,0,10.9);
   Graph_Graph01007->SetMinimum(0.6304398);
   Graph_Graph01007->SetMaximum(0.7695602);
   Graph_Graph01007->SetDirectory(0);
   Graph_Graph01007->SetStats(0);
   Graph_Graph01007->SetLineWidth(2);
   Graph_Graph01007->SetMarkerStyle(20);
   Graph_Graph01007->SetMarkerSize(1.2);
   Graph_Graph01007->GetXaxis()->SetLabelFont(42);
   Graph_Graph01007->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph01007->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph01007->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph01007->GetXaxis()->SetTitleFont(42);
   Graph_Graph01007->GetYaxis()->SetLabelFont(42);
   Graph_Graph01007->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph01007->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph01007->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph01007->GetYaxis()->SetTitleFont(42);
   Graph_Graph01007->GetZaxis()->SetLabelFont(42);
   Graph_Graph01007->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph01007->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph01007->GetZaxis()->SetTitleOffset(1);
   Graph_Graph01007->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph01007);
   
   gre->Draw("p");
   
   Double_t Graph1_fx1008[8] = {
   2.7,
   3.7,
   4.7,
   5.7,
   6.7,
   7.7,
   8.7,
   9.7};
   Double_t Graph1_fy1008[8] = {
   1,
   1,
   1,
   1,
   1,
   1,
   1,
   1};
   Double_t Graph1_fex1008[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph1_fey1008[8] = {
   0.06027616,
   0.05239853,
   0.05444054,
   0.06810136,
   0.09047271,
   0.1336985,
   0.1752649,
   0.2328725};
   gre = new TGraphErrors(8,Graph1_fx1008,Graph1_fy1008,Graph1_fex1008,Graph1_fey1008);
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
   
   TH1F *Graph_Graph11008 = new TH1F("Graph_Graph11008","Graph",100,2,10.4);
   Graph_Graph11008->SetMinimum(0.7205529);
   Graph_Graph11008->SetMaximum(1.279447);
   Graph_Graph11008->SetDirectory(0);
   Graph_Graph11008->SetStats(0);
   Graph_Graph11008->SetLineWidth(2);
   Graph_Graph11008->SetMarkerStyle(20);
   Graph_Graph11008->SetMarkerSize(1.2);
   Graph_Graph11008->GetXaxis()->SetLabelFont(42);
   Graph_Graph11008->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph11008->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph11008->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph11008->GetXaxis()->SetTitleFont(42);
   Graph_Graph11008->GetYaxis()->SetLabelFont(42);
   Graph_Graph11008->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph11008->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph11008->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph11008->GetYaxis()->SetTitleFont(42);
   Graph_Graph11008->GetZaxis()->SetLabelFont(42);
   Graph_Graph11008->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph11008->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph11008->GetZaxis()->SetTitleOffset(1);
   Graph_Graph11008->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph11008);
   
   gre->Draw("p");
   
   Double_t Graph2_fx1009[20] = {
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
   Double_t Graph2_fy1009[20] = {
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6,
   0.6};
   Double_t Graph2_fex1009[20] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
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
   Double_t Graph2_fey1009[20] = {
   0.002981495,
   0.001931848,
   0.001321718,
   0.001225427,
   0.001309604,
   0.001529888,
   0.001686941,
   0.002143076,
   0.002787233,
   0.003701241,
   0.004700828,
   0.006066508,
   0.007837953,
   0.0100859,
   0.01119212,
   0.0144367,
   0.01848033,
   0.02344954,
   0.02946519,
   0.03676066};
   gre = new TGraphErrors(20,Graph2_fx1009,Graph2_fy1009,Graph2_fex1009,Graph2_fey1009);
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
   
   TH1F *Graph_Graph21009 = new TH1F("Graph_Graph21009","Graph",100,0,10.7);
   Graph_Graph21009->SetMinimum(0.5558872);
   Graph_Graph21009->SetMaximum(0.6441128);
   Graph_Graph21009->SetDirectory(0);
   Graph_Graph21009->SetStats(0);
   Graph_Graph21009->SetLineWidth(2);
   Graph_Graph21009->SetMarkerStyle(20);
   Graph_Graph21009->SetMarkerSize(1.2);
   Graph_Graph21009->GetXaxis()->SetLabelFont(42);
   Graph_Graph21009->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph21009->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph21009->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph21009->GetXaxis()->SetTitleFont(42);
   Graph_Graph21009->GetYaxis()->SetLabelFont(42);
   Graph_Graph21009->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph21009->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph21009->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph21009->GetYaxis()->SetTitleFont(42);
   Graph_Graph21009->GetZaxis()->SetLabelFont(42);
   Graph_Graph21009->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph21009->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph21009->GetZaxis()->SetTitleOffset(1);
   Graph_Graph21009->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph21009);
   
   gre->Draw("p");
   
   Double_t Graph3_fx1010[8] = {
   2.5,
   3.5,
   4.5,
   5.5,
   6.5,
   7.5,
   8.5,
   9.5};
   Double_t Graph3_fy1010[8] = {
   0.9,
   0.9,
   0.9,
   0.9,
   0.9,
   0.9,
   0.9,
   0.9};
   Double_t Graph3_fex1010[8] = {
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0};
   Double_t Graph3_fey1010[8] = {
   0.04013644,
   0.03489091,
   0.03625064,
   0.04534705,
   0.06024359,
   0.0890266,
   0.1167046,
   0.1550642};
   gre = new TGraphErrors(8,Graph3_fx1010,Graph3_fy1010,Graph3_fex1010,Graph3_fey1010);
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
   
   TH1F *Graph_Graph31010 = new TH1F("Graph_Graph31010","Graph",100,1.8,10.2);
   Graph_Graph31010->SetMinimum(0.713923);
   Graph_Graph31010->SetMaximum(1.086077);
   Graph_Graph31010->SetDirectory(0);
   Graph_Graph31010->SetStats(0);
   Graph_Graph31010->SetLineWidth(2);
   Graph_Graph31010->SetMarkerStyle(20);
   Graph_Graph31010->SetMarkerSize(1.2);
   Graph_Graph31010->GetXaxis()->SetLabelFont(42);
   Graph_Graph31010->GetXaxis()->SetLabelSize(0.05);
   Graph_Graph31010->GetXaxis()->SetTitleSize(0.05);
   Graph_Graph31010->GetXaxis()->SetTitleOffset(1.4);
   Graph_Graph31010->GetXaxis()->SetTitleFont(42);
   Graph_Graph31010->GetYaxis()->SetLabelFont(42);
   Graph_Graph31010->GetYaxis()->SetLabelSize(0.05);
   Graph_Graph31010->GetYaxis()->SetTitleSize(0.05);
   Graph_Graph31010->GetYaxis()->SetTitleOffset(1.4);
   Graph_Graph31010->GetYaxis()->SetTitleFont(42);
   Graph_Graph31010->GetZaxis()->SetLabelFont(42);
   Graph_Graph31010->GetZaxis()->SetLabelSize(0.05);
   Graph_Graph31010->GetZaxis()->SetTitleSize(0.05);
   Graph_Graph31010->GetZaxis()->SetTitleOffset(1);
   Graph_Graph31010->GetZaxis()->SetTitleFont(42);
   gre->SetHistogram(Graph_Graph31010);
   
   gre->Draw("p");
   
   TLegend *leg = new TLegend(0.1,0.2,0.55,0.5,NULL,"brNDC");
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
   entry=leg->AddEntry("NULL","37 nb^{-1} str. O+O","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","12 nb^{-1} str. Ar+Ar","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("NULL","86 pb^{-1} str. #it{p}+#it{p}","");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.58,0.2,0.95,0.425,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","O+O","h");
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

   ci = TColor::GetColor("#000099");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(25);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   
   leg = new TLegend(0.68,0.2,0.95,0.425,NULL,"brNDC");
   leg->SetBorderSize(0);
   leg->SetLineColor(1);
   leg->SetLineStyle(1);
   leg->SetLineWidth(1);
   leg->SetFillColor(0);
   leg->SetFillStyle(0);
   entry=leg->AddEntry("NULL","Ar+Ar","h");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);
   entry->SetMarkerColor(1);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(1);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","Prompt #it{D}^{0}","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000000");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(20);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   entry=leg->AddEntry("Graph","#it{B}#rightarrow#it{D}^{0}","p");
   entry->SetLineColor(1);
   entry->SetLineStyle(1);
   entry->SetLineWidth(1);

   ci = TColor::GetColor("#000099");
   entry->SetMarkerColor(ci);
   entry->SetMarkerStyle(21);
   entry->SetMarkerSize(2);
   entry->SetTextFont(42);
   leg->Draw();
   D0_BUP2020OOArAr_RAA_flat_5yr_1->Modified();
   D0_BUP2020OOArAr_RAA_flat_5yr->cd();
   D0_BUP2020OOArAr_RAA_flat_5yr->Modified();
   D0_BUP2020OOArAr_RAA_flat_5yr->cd();
   D0_BUP2020OOArAr_RAA_flat_5yr->SetSelected(D0_BUP2020OOArAr_RAA_flat_5yr);
}
